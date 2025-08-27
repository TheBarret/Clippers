#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <filesystem>
#include <curl/curl.h>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath> // Required for std::pow

namespace fs = std::filesystem;

// Configuration structure
struct Config {
    int min_delay_ms = 100;           // Minimum delay between requests
    int max_delay_ms = 2000;          // Maximum delay between requests
    int timeout_seconds = 10;         // Request timeout
    int max_retries = 3;              // Maximum retry attempts
    int max_concurrent = 5;           // Maximum concurrent connections
    int backoff_multiplier = 2;       // Exponential backoff multiplier
    std::string user_agent = "Clippers-Harvest/2.0";
};

// Statistics tracking
struct Stats {
    std::atomic<int> total_requests{0};
    std::atomic<int> successful_requests{0};
    std::atomic<int> failed_requests{0};
    std::atomic<int> retry_attempts{0};
    std::chrono::steady_clock::time_point start_time;
};

// Host rate limiting
class HostRateLimiter {
private:
    std::map<std::string, std::chrono::steady_clock::time_point> last_request;
    std::mutex mtx;
    int min_delay_ms;

public:
    HostRateLimiter(int delay_ms) : min_delay_ms(delay_ms) {}
    
    void wait_if_needed(const std::string& host) {
        std::lock_guard<std::mutex> lock(mtx);
        auto now = std::chrono::steady_clock::now();
        auto it = last_request.find(host);
        
        if (it != last_request.end()) {
            auto time_since_last = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - it->second).count();
            
            if (time_since_last < min_delay_ms) {
                int wait_time = min_delay_ms - time_since_last;
                std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
            }
        }
        
        last_request[host] = std::chrono::steady_clock::now();
    }
};

// CURL connection pool
class CurlPool {
private:
    std::vector<CURL*> handles;
    std::mutex mtx;
    size_t max_size;

public:
    CurlPool(size_t size) : max_size(size) {
        for (size_t i = 0; i < size; ++i) {
            CURL* handle = curl_easy_init();
            if (handle) {
                configure_handle(handle);
                handles.push_back(handle);
            }
        }
    }
    
    ~CurlPool() {
        for (auto handle : handles) {
            curl_easy_cleanup(handle);
        }
    }
    
    CURL* get_handle() {
        std::lock_guard<std::mutex> lock(mtx);
        if (handles.empty()) return nullptr;
        
        CURL* handle = handles.back();
        handles.pop_back();
        return handle;
    }
    
    void return_handle(CURL* handle) {
        if (!handle) return;
        
        curl_easy_reset(handle);
        configure_handle(handle);
        
        std::lock_guard<std::mutex> lock(mtx);
        if (handles.size() < max_size) {
            handles.push_back(handle);
        } else {
            curl_easy_cleanup(handle);
        }
    }

private:
    void configure_handle(CURL* handle) {
        curl_easy_setopt(handle, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle, CURLOPT_USERAGENT, "Clippers-Harvest/2.0");
        curl_easy_setopt(handle, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
    }
};

// Extract host from URL
std::string extract_host(const std::string& url) {
    size_t start = url.find("://");
    if (start == std::string::npos) return "";
    
    start += 3;
    size_t end = url.find('/', start);
    if (end == std::string::npos) end = url.length();
    
    return url.substr(start, end - start);
}

// Check URL with libcurl using connection pool
bool validate_url(const std::string& url, long& response_code, CurlPool& pool, 
                  HostRateLimiter& limiter, const Config& config, Stats& stats) {
    
    // Rate limit by host
    std::string host = extract_host(url);
    if (!host.empty()) {
        limiter.wait_if_needed(host);
    }
    
    CURL* curl = pool.get_handle();
    if (!curl) return false;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, config.timeout_seconds);
    
    int retries = 0;
    bool success = false;
    
    while (retries <= config.max_retries && !success) {
        if (retries > 0) {
            stats.retry_attempts++;
            int backoff_ms = config.min_delay_ms * 
                static_cast<int>(std::pow(config.backoff_multiplier, retries - 1));
            std::this_thread::sleep_for(std::chrono::milliseconds(backoff_ms));
        }
        
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if ((response_code >= 200 && response_code < 300) ||
                (response_code >= 300 && response_code < 400)) {
                success = true;
            }
        } else {
            response_code = -1;
        }
        
        retries++;
    }
    
    pool.return_handle(curl);
    stats.total_requests++;
    
    if (success) {
        stats.successful_requests++;
    } else {
        stats.failed_requests++;
    }
    
    return success;
}

// Print progress bar
void print_progress(int current, int total, const std::string& current_url) {
    const int bar_width = 50;
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(bar_width * progress);
    
    std::cout << "\r[";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) 
              << (progress * 100.0) << "% (" << current << "/" << total << ")";
    
    if (!current_url.empty()) {
        std::cout << " - " << current_url;
    }
    
    std::cout << std::flush;
}

// Print statistics
void print_stats(const Stats& stats) {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        end_time - stats.start_time).count();
    
    std::cout << "\n\n=== HARVEST STATISTICS ===\n";
    std::cout << "Total requests: " << stats.total_requests.load() << "\n";
    std::cout << "Successful: " << stats.successful_requests.load() << "\n";
    std::cout << "Failed: " << stats.failed_requests.load() << "\n";
    std::cout << "Retry attempts: " << stats.retry_attempts.load() << "\n";
    std::cout << "Duration: " << duration << " seconds\n";
    
    if (duration > 0) {
        double req_per_sec = static_cast<double>(stats.total_requests.load()) / duration;
        std::cout << "Rate: " << std::fixed << std::setprecision(2) 
                  << req_per_sec << " requests/second\n";
    }
}

int main() {
    Config config;
    Stats stats;
    stats.start_time = std::chrono::steady_clock::now();
    
    // Initialize CURL
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "Failed to initialize CURL\n";
        return 1;
    }
    
    // Create connection pool and rate limiter
    CurlPool pool(config.max_concurrent);
    HostRateLimiter limiter(config.min_delay_ms);
    
    std::cout << "[*] Harvest v2.0 starting...\n";
    std::cout << "[*] Configuration:\n";
    std::cout << "    - Min delay: " << config.min_delay_ms << "ms\n";
    std::cout << "    - Max delay: " << config.max_delay_ms << "ms\n";
    std::cout << "    - Timeout: " << config.timeout_seconds << "s\n";
    std::cout << "    - Max retries: " << config.max_retries << "\n";
    std::cout << "    - Max concurrent: " << config.max_concurrent << "\n\n";
    
    // Check if clean directory exists
    if (!fs::exists("clean") || !fs::is_directory("clean")) {
        std::cerr << "Error: 'clean' directory not found\n";
        curl_global_cleanup();
        return 1;
    }
    
    int total_files = 0;
    int total_urls = 0;
    
    // First pass: count total URLs
    for (auto& entry : fs::directory_iterator("clean")) {
        if (!entry.is_regular_file()) continue;
        
        std::ifstream infile(entry.path());
        if (!infile.is_open()) continue;
        
        std::string url;
        while (std::getline(infile, url)) {
            if (!url.empty()) total_urls++;
        }
        infile.close();
        total_files++;
    }
    
    std::cout << "[*] Found " << total_files << " files with " << total_urls << " URLs\n\n";
    
    int processed_urls = 0;
    
    // Process each file
    for (auto& entry : fs::directory_iterator("clean")) {
        if (!entry.is_regular_file()) continue;
        
        std::string path = entry.path().string();
        std::ifstream infile(path);
        if (!infile.is_open()) continue;

        std::cout << "\n[*] Processing file: " << fs::path(path).filename().string() << "\n";

        std::set<std::string> validated;
        std::vector<std::string> urls;
        std::string url;
        
        // Read all URLs first
        while (std::getline(infile, url)) {
            if (!url.empty()) urls.push_back(url);
        }
        infile.close();
        
        // Process URLs with progress bar
        for (size_t i = 0; i < urls.size(); ++i) {
            const std::string& current_url = urls[i];
            
            long code = 0;
            bool ok = validate_url(current_url, code, pool, limiter, config, stats);
            
            if (ok) {
                validated.insert(current_url);
            }
            
            processed_urls++;
            print_progress(processed_urls, total_urls, current_url);
            
            // Adaptive delay based on response
            int delay = ok ? config.min_delay_ms : config.min_delay_ms * 2;
            delay = std::min(delay, config.max_delay_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        
        std::cout << "\n[*] File complete: " << validated.size() << "/" << urls.size() 
                  << " URLs valid\n";

        // Write validated URLs back
        std::string tmpfile = path + ".tmp";
        std::ofstream outfile(tmpfile);
        for (auto& u : validated) {
            outfile << u << "\n";
        }
        outfile.close();
        fs::rename(tmpfile, path);
    }
    
    std::cout << "\n";
    print_stats(stats);
    
    curl_global_cleanup();
    std::cout << "\n[*] Harvest complete.\n";
    return 0;
}
