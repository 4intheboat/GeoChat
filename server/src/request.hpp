#pragma once

#include <string>
#include "common/common.hpp"

/*
 * Common info about request
 */
struct RequestDetails
{
    RequestDetails() {}

    struct Params
    {
        uint64_t uid = 0;
        time_t ts = 0;
        uint64_t count = 0;         // of messages

        std::string message;        // send
        std::string to_user;

        std::string user;           // lookup user by name
        std::string password;

        std::string country; //location

        std::string ip_adress;

        struct chat
        {
            std::string name;
            std::string adduser;
        } chat;
    } params;
    
    common::cmd_t command;
    std::string remote_address;
    std::string resource;
    std::string method;
    std::string sessid;
};

