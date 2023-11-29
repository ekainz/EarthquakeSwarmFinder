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
               json earthquakes = json::parse(response);
               earthquakes = earthquakes["features"];


              // std::cout << earthquakes.dump(4) << std::endl;


               // Filter out Points not within searchDistance of another point
                   // Select First point

                   // check distance with Pythagorean Therom to points with larger index
                   // if distance < searchDistance copy both points to new json
                   // select next point

               auto size = earthquakes.size();
              
               json filteredEarthquakes[size/2];

               for(int i=0; i<size-1; i++){ 
                   for(int j=i+1; j<size; j++){


                       double a = earthquakes[i]["geometry"]["coordinates"][0];
                       double b = earthquakes[i]["geometry"]["coordinates"][1];
                       double x = earthquakes[j]["geometry"]["coordinates"][0];
                       double y = earthquakes[j]["geometry"]["coordinates"][1];


                        
                      
                       if(haversineDistance(a, b, x, y) < searchDistance){
                           filteredEarthquakes[0].push_back(earthquakes[i]);            
                           break;
                       }
                 
                   }
                  
               }
                   std::cout << filteredEarthquakes[0].dump(4) << std::endl;



               // Find center of group of quakes
                   // Select First point
                   // check distance with pithagoream theorum to points with larger index
                   // if distance < searchDistance copy points to new swarm[n]

                   // find average location of swarm
                   // clear swarm[n]
                   // check distance to all earthquakes, copy earthquakes closer than search distance to swarm[n]
                   
                   // repeat above 3 steps until average location stops changing
                   // check distance to all earthquakes, copy earthquakes not within distance to new 


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
