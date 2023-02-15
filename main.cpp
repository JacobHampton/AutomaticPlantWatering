#include <iostream>
#include <wiringPi.h>
#include <csignal>
#include <curl/curl.h>
#include <time.h>

bool RUNNING = true;

void my_handler(int s){
	std::cout << "Detected CTRL-C signal number" << s << '\n';
	RUNNING = false; 
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(){
    std::signal(SIGINT, my_handler);
    wiringPiSetupGpio();
    std::string url = "http://et.water.ca.gov/api/data?appKey=d0fe0b6f-f5b0-445d-a750-e0c6ce58123d&targets=169&startDate=2023-02-14&endDate=2023-02-14&dataItems=day-asce-eto,day-eto,day-precip&unitOfMeasure=E";
    //std::string url = "http://google.com";

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    
        /* always cleanup */
        curl_easy_cleanup(curl);
  }
    /*if (curl)
    {
        curl_easy_setopt(curl,CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
 
        //always cleanup 
        std::cout << "Curl: " << curl;
        std::cout << "Result: " << res;
        std::cout << "readBuffer: " << readBuffer;

        curl_easy_cleanup(curl);
    }*/
    curl_global_cleanup();
    return 0;
}