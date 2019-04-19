#define CATCH_CONFIG_MAIN

#include <iostream>
//#include "../../common/http.hpp"
//#include "../../common/common.hpp"
#include "../../server/src/location.h"
#include "../../server/src/database_worker.hpp"
#include "../../server/src/database_worker.cpp"
#include "../../server/src/apiclient_utils.cpp"
#include "../../server/src/apiclient.hpp"
#include "../../server/src/apiclient.cpp"
#include "../../server/src/apiclient_utils.hpp"
#include "../../server/src/inmemory_dbconn.cpp"
#include "../../server/src/location.cpp"
#include "../../server/src/location.h"


#include "catch.hpp"

#include "o2logger/src/o2logger.hpp"

SCENARIO("server tests", "[server]") {
    RequestDetails details = RequestDetails();
    GIVEN("HttpReply") {

        WHEN("REGISTER artyom Artefakt1998") {
            std::string raw = "{\"user\":\"artyom\", \"password\":\"Artefakt1998\"}";
            parse_meta(details, raw, common::cmd_t::USER_CREATE);\
            THEN("password check") {
                REQUIRE(apiclient_utils::password_check(details.params.password, 8));
            }
        }
        WHEN("GET_LOCATIONS") {
            auto locations = new std::vector<apiclient_utils::Location>;
            locations = location::get_locations();
            REQUIRE(locations->back().location == "Italy");
        }

        WHEN("CHAT_WITH_LOCATION Italy") {
            auto users = new std::vector<apiclient_utils::User>;
            users = location::get_users("Italy");
            REQUIRE(users->back().username == "Asya");
            REQUIRE(users->back().location == "Italy");
        }


    }
}