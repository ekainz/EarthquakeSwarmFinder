#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <cmath>



double haversineDistance(double lat1, double lon1, double lat2, double lon2);




const double searchDistance = 0.5;


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
       std::string url = "https://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/all_day.geojson"; // Replace with your JSON URL


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
               json parsed = json::parse(response);
               json earthquakes[0] = parsed["features"];

               n = 0;
               while true {
                   // if earthquakes[n] is empty, exit loop and print statistics
                   if (earthquakes[n].empty()) {
                       break;
                }

                // filter out earthquakes not within searchDistance of another earthquake
                // optimisation would be to just run this once at start, then only check if the first earthquake on later interations
                json earthquakesFiltered;
                for (int i = 0; i < (earthquakes[n].size() - 1); i++) {
                    for (int j = 0 + 1; j < earthquakes[n].size(); j++) {
                        if (j == i) {
                            j++;
                        }

                        double a = earthquakes[n][i]["geometry"]["coordinates"][0];
                        double b = earthquakes[n][i]["geometry"]["coordinates"][1];
                        double x = earthquakes[n][j]["geometry"]["coordinates"][0];
                        double y = earthquakes[n][j]["geometry"]["coordinates"][1];

                        // check distance
                        if  (haversineDistance(a, b, x, y) < searchDistance) {
                            earthquakesFiltered.push_back(earthquakes[n][i]);
                            break;
                       }
                    }
                }


                double avgLocation[2];
                avgLocation[0] = earthquakesFiltered[1]["geometry"]["coordinates"][0];
                avgLocation[1] = earthquakesFiltered[1]["geometry"]["coordinates"][1];




                while true {
                    // check distance to each earthquake in earthquakesFiltered
                    for (int i = 0; i < earthquakesFiltered.size(); i++) {
                        double x = earthquakesFiltered[i]["geometry"]["coordinates"][0];
                        double y = earthquakesFiltered[i]["geometry"]["coordinates"][1];

                        if (haversineDistance(avgLocation[0], avgLocation[1], x, y) > searchDistance){
                            swarm[n].push_back(earthquakesFiltered[i])

                        } else {
                            earthquakes[n+1].push_back(earthquakesFiltered[i]);
                        }
                    }

                    // Calculate average location of swarm [n]
                    double newAvgLocation[2] = 0;
                    for (int i = 0; i < swarm[n].size(); i++) {
                        newAvgLocation[0] = newAvgLocation[0] + swarm[i]["geometry"]["coordinates"][0];
                        newAvgLocation[1] = newAvgLocation[1] + swarm[i]["geometry"]["coordinates"][1];
                    }

                    newAvgLocation[0] = newAvgLocation[0] / swarm[n].size();
                    newAvgLocation[1] = newAvgLocation[1] / swarm[n].size();

                    if (newAvgLocation[0] == avgLocation[0] && newAvgLocation[1] == avgLocation[1]){
                        break;
                    }
                    avgLocation = newAvgLocation;
                    quakes[n].clear();
                    earthquakes[n+1].clear();
                }
                n++;
            }


            // print statistics here

           } catch (const std::exception& e) {
               std::cerr << "JSON parsing error: " << e.what() << std::endl;
           }
       } else {
           std::cerr << "Failed to fetch data from the URL" << std::endl;
       }
   }


   return 0;
}

double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1)*(M_PI/180);
    double dLon = (lon2 - lon1)*(M_PI/180);

    lat1 = lat1*(M_PI/180);
    lat2 = lat2*(M_PI/180);

    // Apply formula
    double a = pow(sin(dLat / 2), 2) +
               pow(sin(dLon / 2), 2) *
               cos(lat1) * cos(lat2);
    double c = 2 * asin(sqrt(a));
    double R = 6371; // Radius of Earth in Kilometers
    return R * c;
}
