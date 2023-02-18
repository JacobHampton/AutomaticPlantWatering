#include <iostream>
#include <string>
#include <wiringPi.h>
#include <csignal>
#include <curl/curl.h>
#include <time.h>
#include <regex>

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

std::string getWeatherData(){
    std::string url = "http://et.water.ca.gov/api/data?appKey=d0fe0b6f-f5b0-445d-a750-e0c6ce58123d&targets=169&startDate=2023-01-15&endDate=2023-01-15&dataItems=day-asce-eto,day-eto,day-precip&unitOfMeasure=E";
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
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
    curl_global_cleanup();

    return readBuffer;
}

std::string parseWeatherData (std::string dataType, std::string weatherData)
{
    //std::cout << weatherData << "\n";

    int startIndex = weatherData.find(dataType);
    //std::cout << "\n" << startIndex;

    std::string startSubstr = weatherData.substr(startIndex, 35);
    //std::cout << startSubstr << "\n";
    int commaIndex = startSubstr.find_first_of(",");
    int bracketIndex = startSubstr.find("{");
    int stringValueStartIndex = bracketIndex + 10;
    int stringValueEndIndex = commaIndex - 1;

    std::string stringValue = startSubstr.substr(stringValueStartIndex, stringValueEndIndex - stringValueStartIndex);
    //std::cout << stringValue << "\n";

    return stringValue;
}

int main(){
    std::signal(SIGINT, my_handler);
    wiringPiSetupGpio();

    //std::string weatherData = getWeatherData();
    //std::cout << weatherData << "\n";
    /*
    int startEtoIndex = weatherData.find("DayEto");
    //std::cout << "\n" << startEto;

    std::string etoSubstr = weatherData.substr(startEtoIndex, 23);
    //std::cout << str2 << "\n";

    int etoValueIndex = etoSubstr.find("0.");
    std::string etoValue = etoSubstr.substr(etoValueIndex, 4);
    std::cout << etoValue;
    */
    std::string weatherData = getWeatherData();

    std::string etoValue = parseWeatherData("DayEto", weatherData);
    std::string precipValue = parseWeatherData("DayPrecip", weatherData);

    std::cout << "Eto: " << etoValue << "\n";
    std::cout << "Precip: " << precipValue << "\n";
    return 0;
}