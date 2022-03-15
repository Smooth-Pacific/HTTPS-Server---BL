#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <thread>
#include <atomic>

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp){
   return size * nmemb;
}

struct Curlout {
    int response_code;
    int size;
};

struct Curlout curl_server(){
    CURLcode ret;
    CURL *hnd;

    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://localhost:8080/mime/sample.xml/");
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_USERPWD, "myuser:mypass");
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "text/xml");
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)18);
    curl_easy_setopt(hnd, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.71.1");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_CAINFO, "/usr/local/share/ca-certificates/root.crt");
    curl_easy_setopt(hnd, CURLOPT_SSH_KNOWNHOSTS, "/home/bartell/.ssh/known_hosts");
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);  

    ret = curl_easy_perform(hnd);
    long response_code;
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &response_code);
    curl_off_t size;
    curl_easy_getinfo(hnd, CURLINFO_SIZE_DOWNLOAD_T, &size);
    curl_easy_cleanup(hnd);
    hnd = NULL;
    
    struct Curlout out;
    out.response_code = (int)response_code;
    out.size = (int)size;
    return out;
}


int main(int argc, char *argv[]){
    std::atomic<int> ok = 0;
    std::atomic<long> size = 0;
    int requests = 1000;
    int num_threads = 4;
    int requests_block = requests / num_threads;
    std::vector<std::thread> threads(num_threads);

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads[i] = std::thread([&ok, &requests_block, &size]() {
            int j = 0;
            while (j++ < requests_block) {
                struct Curlout out = curl_server();
                if (out.response_code == 200) { 
                    ok++; 
                    size.fetch_add(out.size);
                }
            }
        });
    }

    for (auto& entry: threads) {
        entry.join();
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    float totaltime = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    double mb = size / 125;
    double mbps = mb / (totaltime / 1000);

    std::cout << "Completed " << requests << " in " << totaltime << "ms" << std::endl;
    std::cout << "Responded with " << mb << "mb" << " of data" << std::endl;
    std::cout << "Average response throughput: " << mbps << "Mbps" << std::endl;
    std::cout << "Processed to error ratio: " << ok << ":" << (requests - ok) << std::endl;
}