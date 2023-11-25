#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <cmath>





using json = nlohmann::json;
using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

int main() {
    // Initialize libcurl
    CURL* curl = curl_easy_init();

    if (curl) {
        std::string url = "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_month.geojson"; // Replace with your JSON URL

        // Perform the HTTP request and store the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            try {
                // Parse the JSON data using nlohmann/json
                json earthquakes = json::parse(response);


                std::cout << earthquakes.dump(4) << std::endl;

                // Filter out Points not within searchDistance of another point
                    // Select First point
                    // check distance with Pythagorean Therom to points with larger index
                    // store shortest distance to shortestDistance
                    // if shortestDistance > searchDistance run erase on point (using json library "erase")
                    // select next point (may have same index if we erased a point)

                // Find all points within searchDistance of a point
                    // Select First point
                    // check distance with pithagoream theorum to points with larger index
                    // if distance < searchDistance copy point to new struct, run erase on point (using json library "erase")
                    // select next point (may have same index if we erased a point)

            } catch (const std::exception& e) {
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "Failed to fetch data from the URL" << std::endl;
        }
    }

    return 0;
}


