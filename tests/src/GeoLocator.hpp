#ifndef GeoLocator_hpp
#define GeoLocator_hpp
#include <string>
#include <vector>

class GeoLocator{
public:

    std::string location;
    std::string IP;


    virtual std::vector <std::string> LocationToPoolIP_processing();
    virtual std::string IPToLocation_processing ();
    virtual bool PutLocationOnServer ();
    virtual bool PutPoolOPOnServer(std:: vector <std::string> &pool_IP);

};

#endif
