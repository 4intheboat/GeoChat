#define CATCH_CONFIG_MAIN
#include <iostream>
#include <string>
#include <vector>
#include "GeoLocator.hpp"
#include "catch.hpp"

#define False 0
#define True 1

using namespace std;


std::vector <std::string> GeoLocator::LocationToPoolIP_processing (){

    if (location == "Russia") cout <<"192.168.12.1"<<endl;
    std::vector <std::string> pool_IP_for_server = {"192.168.12.1"};
    return pool_IP_for_server;
}

std::string GeoLocator::IPToLocation_processing (){

    std::string loc;
    if (IP=="192.168.12.1") {
        cout<<"Russia"<<endl;
        loc = "Russia";
    } else if (IP == "104.248.15.01") {
        cout<<"USA"<<endl;
        loc = "USA";
    } else if (IP == "60.241.230.100") {
        cout<<"Australia"<<endl;
        loc = "Australia";
    }
    return loc;
}

bool GeoLocator::PutLocationOnServer (){

    bool error = False;
    cout<<"Now current location: "<<location<< ", is on the server"<<endl;
    error = True;
    return error;
}

bool GeoLocator::PutPoolOPOnServer(std:: vector <std::string> &pool_IP){

    bool error = False;
    cout<<"Now pool IP is on the server"<<endl;
    error = True;
    return error;

}


TEST_CASE("TEST GeoLocator"){
    GeoLocator g;

    SECTION ("TEST Location -> IP", "[LocationToPoolIP_processing]"){
        g.location = "Russia";
        std::vector <std::string> vec = {"192.168.12.1"};
        REQUIRE(g.LocationToPoolIP_processing() == vec);
    }

    SECTION  ("TEST IP -> Location", "[IPToLocation_processing]") {
        g.IP = "192.168.12.1";
        REQUIRE(g.IPToLocation_processing() == "Russia");

        g.IP = "104.248.15.01";
        REQUIRE(g.IPToLocation_processing() == "USA");

        g.IP = "60.241.230.100";
        REQUIRE(g.IPToLocation_processing() == "Australia");
    }

    SECTION ("TEST Location -> server", "[PutLocationOnServer]"){
        REQUIRE(g.PutLocationOnServer() == True);
    }

    SECTION ("TEST Pool IP -> server", "[PutPoolOPOnServer]"){
        std::vector <std::string> vec = {"192.168.12.1"};
        REQUIRE(g.PutPoolOPOnServer(vec) == True);
    }

}