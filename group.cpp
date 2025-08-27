#include <iostream>
#include <string>
#include <map>
#include <set>
#include <regex>
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

// Extract domain from URL (http/https only)
std::string get_domain(const std::string& url) {
    static const std::regex re(R"(^(?:https?://)([^/]+))", std::regex::icase);
    std::smatch match;
    if (std::regex_search(url, match, re)) {
        return match[1];
    }
    return {};
}

int main() {
    std::map<std::string, std::set<std::string>> domain_map;
    std::string line;

    // Read URLs from stdin
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::string domain = get_domain(line);
        if (!domain.empty()) {
            domain_map[domain].insert(line);
        }
    }

    // Ensure "clean" directory exists
    if (!fs::exists("clean")) {
        fs::create_directory("clean");
    }

    // Write files for domains with more than 1 URL
    for (const auto& [domain, urls] : domain_map) {
        if (urls.size() > 1) {
            std::string filename = "clean/" + domain + ".txt";

            // sanitize filename (replace illegal chars with '_')
            std::replace_if(filename.begin(), filename.end(),
                            [](char c) { return !(std::isalnum(c) || c == '.' || c == '-' || c == '/' ); },
                            '_');

            // Append instead of overwrite
            std::ofstream outfile(filename, std::ios::app);
            if (!outfile.is_open()) {
                std::cerr << "Error: Cannot write file " << filename << "\n";
                continue;
            }

            for (const auto& url : urls) {
                outfile << url << "\n";
            }
        }
    }

    return 0;
}
