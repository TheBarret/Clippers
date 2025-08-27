#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <set>
#include <gumbo.h>

// Recursive DOM walker
void search_for_links(GumboNode* node, std::set<std::string>& urls) {
    if (node->type != GUMBO_NODE_ELEMENT) return;

    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        std::string url = href->value;

        // Basic URL validation (http/https only, no javascript:)
        static const std::regex url_regex(R"(^https?://[^\s]+$)",
                                          std::regex::icase);
        if (std::regex_match(url, url_regex)) {
            urls.insert(url);
        }
    }

    // Recurse children
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links(static_cast<GumboNode*>(children->data[i]), urls);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file.html\n";
        return 1;
    }

    // Read HTML file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open " << argv[1] << "\n";
        return 1;
    }
    std::string html((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    // Parse HTML with Gumbo
    GumboOutput* output = gumbo_parse(html.c_str());
    std::set<std::string> urls;
    search_for_links(output->root, urls);
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // Print collected URLs
    for (const auto& url : urls) {
        std::cout << url << "\n";
    }

    return 0;
}
