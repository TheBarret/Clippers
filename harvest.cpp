#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <filesystem>
#include <curl/curl.h>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

// Check URL with libcurl
bool validate_url(const std::string& url, long& response_code) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);          // HEAD request
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // follow redirects
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Clippers-Harvest/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);        // 10s timeout

    CURLcode res = curl_easy_perform(curl);
    bool ok = false;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if ((response_code >= 200 && response_code < 300) ||
            (response_code >= 300 && response_code < 400)) {
            ok = true;
        }
    } else {
        response_code = -1; // signal error
    }

    curl_easy_cleanup(curl);
    return ok;
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    for (auto& entry : fs::directory_iterator("clean")) {
        if (!entry.is_regular_file()) continue;
        std::string path = entry.path().string();
        std::ifstream infile(path);
        if (!infile.is_open()) continue;

        std::cout << "[*] Processing file: " << path << "\n";

        std::set<std::string> validated;
        std::string url;
        while (std::getline(infile, url)) {
            if (url.empty()) continue;

            long code = 0;
            std::cout << "  -> Checking: " << url << " ... ";
            bool ok = validate_url(url, code);

            if (ok) {
                validated.insert(url);
                std::cout << "OK (" << code << ")\n";
            } else {
                std::cout << "FAIL (" << code << ")\n";
            }

            // polite delay
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        infile.close();

        std::cout << "[*] Updating file: " << path 
                  << " (" << validated.size() << " valid URLs)\n";

        // Write only validated URLs back (atomic replace)
        std::string tmpfile = path + ".tmp";
        std::ofstream outfile(tmpfile);
        for (auto& u : validated) {
            outfile << u << "\n";
        }
        outfile.close();
        fs::rename(tmpfile, path);
    }

    curl_global_cleanup();
    std::cout << "[*] Harvest complete.\n";
    return 0;
}
