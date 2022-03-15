#include <curl/curl.h>

#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp){
   return size * nmemb;
}

int curl_server(){
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
    
    curl_easy_cleanup(hnd);
    hnd = NULL;
 
    return (int)response_code;
}


int main(int argc, char *argv[]){
    std::vector<int> latency;
    int requests = 1000;
    int ok = 0;

    int i = 0;
    while(i++ < requests){
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        int response = curl_server();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        if (response == 200) { ok++; }
        latency.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count());

    }
    float sum = std::accumulate(latency.begin(), latency.end(), 0);

    std::cout << "Completed " << requests << " in " << sum << "ms" << std::endl;
    std::cout << "Average latency per request: " << (sum / requests) << "ms" << std::endl;
    std::cout << "Requests per second: " << (requests / (sum / 1000)) << std::endl;
    std::cout << "Processed to error ratio: " << ok << ":" << (requests - ok) << std::endl;

}