#define CATCH_CONFIG_MAIN

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

#include "../../client/src/apiclient_utils.hpp"

using namespace input;


TEST_CASE("test parse", "[init]")
{
    args_t answer;
    std::string string;
    //args_t parse(const std::string &string);
    
    {
        answer.cmd = cmd_t::HELP;
        string = "help";
        args_t real_answ = parse(string);
        REQUIRE(real_answ.cmd == answer.cmd);
    }
    //help
    
    //unchat 
    {
        answer.cmd = cmd_t::UNCHAT;
        string = "unchat";
        args_t real_answ = parse(string);
        REQUIRE(real_answ.cmd == answer.cmd);
    }
    //login
    {
        answer.cmd = cmd_t::LOGIN;
        answer.login.name = "name";
        answer.login.pass = "password";
        string = "login name password";
        args_t real_answ = parse(string);
        REQUIRE((real_answ.cmd == answer.cmd
               && real_answ.login.name == answer.login.name
               && real_answ.login.pass == answer.login.pass));
    }
    
    //register
    {
        answer.cmd = cmd_t::REGISTER_USER;
        answer.reg.name = "name";
        answer.reg.pass = "password";
        string = "register name password";
        args_t real_answ = parse(string);
        REQUIRE((real_answ.cmd == answer.cmd
               && real_answ.reg.name == answer.reg.name
               && real_answ.reg.pass == answer.reg.pass));
    }
    //history
    {
        answer.cmd = cmd_t::HISTORY_USER;
        answer.history.name = "name";
        string = "history name ";
        args_t real_answ = parse(string);
        REQUIRE((real_answ.cmd == answer.cmd
               && real_answ.history.name == answer.history.name
               ));
    }
    //direct
     {
        answer.cmd = cmd_t::DIRECT_MSG_USER;
        answer.direct.name = "name";
        answer.direct.message = "blablabla";
        string = "direct_msg name blablabla";
        args_t real_answ = parse(string);
        REQUIRE((real_answ.cmd == answer.cmd
               && real_answ.direct.name == answer.direct.name
               && real_answ.direct.message == answer.direct.message));
    }
    //status
    {
        answer.cmd = cmd_t::STATUS_USER;
        answer.status.name = "name";
        string = "status name ";
        args_t real_answ = parse(string);
        REQUIRE((real_answ.cmd == answer.cmd
               && real_answ.status.name == answer.status.name
               ));
    }
    //quit
    {
        answer.cmd = cmd_t::QUIT;
        string = "quit";
        args_t real_answ = parse(string);
        REQUIRE(real_answ.cmd == answer.cmd);
    }
    //chat_with
    {
        answer.cmd = cmd_t::CHAT_WITH_USER;
        answer.chat.name = "name";
        string = "chat_with name";
        args_t real_answ = parse(string);
        REQUIRE((real_answ.cmd == answer.cmd
               && real_answ.chat.name == answer.chat.name
               ));
    }
    //chat_with_location
    {
        answer.cmd = cmd_t::CHAT_WITH_LOCATION;
        answer.location.location = "name";
        string = "chat_with_location name";
        args_t real_answ = parse(string);
        REQUIRE((real_answ.cmd == answer.cmd
               && real_answ.location.location == answer.location.location
               ));
    }
    // get_locations
    {
        answer.cmd = cmd_t::GET_LOCATIONS;
        string = "get_locations";
        args_t real_answ = parse(string);
        REQUIRE(real_answ.cmd == answer.cmd);
    }
    
    //None
    {
        answer.cmd = cmd_t::NONE;
        string = "qqwqww qewqew qweneqw";
        args_t real_answ = parse(string);
        REQUIRE(real_answ.cmd == answer.cmd);
    }
}

using namespace cli_utils;

TEST_CASE("test pasre_msg_response","[cli_utils]")
{
    input::cmd_t cmd;
    std::string json; 
    std::vector<cli_utils::msg_response_t> response;
    std::string answer;
    {
        cmd = input::cmd_t::NONE;
        json = "json";
        answer = "bad request, invalid json";
        std::string real_answ = parse_msg_response(cmd,json,response);
        REQUIRE(real_answ == answer);
    }
    
}
TEST_CASE("test pasre_response_answer","[cli_utils]")
{
    input::cmd_t cmd;
    std::string json; 
    cli_utils::response_t resp;
    std::string answer;
    {
        cmd = input::cmd_t::NONE;
        json = "json";
        answer = "bad request, invalid json";
        std::string real_answ = parse_response_aswer(cmd,json,resp);
        REQUIRE(real_answ == answer);
    }
    
}
TEST_CASE("test build request","[cli_utils]")
{
    std::string resource;
    std::string content_type;
    std::string body;
    std::string answer;
    {
        resource = "resourse";
        content_type = "content_type";
        body = "body";
        answer = "POST resourse HTTP/1.1\r\nContent-Length: 4\r\nContent-Type: content_type\r\n\r\nbody";
        std::string real_answ = build_request(resource, content_type, body);
        REQUIRE(real_answ == answer);
    }
    
}
TEST_CASE("test build user pass body","[cli_utils]")
{
    std::string user;                    
    std::string pas;
    std::string answer;
    {
        user = "user";
        pas = "pas";
        answer ={ '{','"','u','s','e','r','"',':','"','u','s','e','r','"',',','"','p','a','s','s','w','o','r','d','"',':','"','p','a','s','"','}'};
        std::string real_answ = build_user_pass_body(user,pas);
        REQUIRE(real_answ == answer);
    } 
}
TEST_CASE("test build get locations body","[cli_utils]")
{
    int64_t from;                    
    std::string pas;
    std::string client_ip;
    std::string answer;
    {
        from = 0;
        pas = "pas";
        client_ip = "188.10.10.10";
        answer ="{\"uid\":0,\"client_ip\":\"188.10.10.10\",\"password\":\"pas\"}";
        std::string real_answ = build_get_locations_body(from,client_ip,pas);
        REQUIRE(real_answ == answer);
    } 
}
TEST_CASE("test build location body","[cli_utils]")
{
    int64_t from;                    
    std::string pas;
    std::string country;
    std::string answer;
    {
        from = 0;
        pas = "pas";
        country = "country";
        answer ="{\"uid\":0,\"country\":\"country\",\"password\":\"pas\"}";
        std::string real_answ = build_location_body(from,country,pas);
        REQUIRE(real_answ == answer);
    } 
}

TEST_CASE("test build_user_send_msg_body","[cli_utils]")
{
    uint64_t from;                           
    uint64_t ts;
    std::string to;
    std::string msg;
    std::string pass;
    std::string answer;
    {
        from = 0;
        ts = 0;
        to = "to";
        msg = "msg";
        pass = "pass";
        answer = "{\"uid\":0,\"to\":\"to\",\"message\":\"msg\",\"password\":\"pass\"}";
        std::string real_answ = build_user_send_msg_body(from,ts,to,msg,pass);
        REQUIRE(real_answ == answer);
    }
    
    
}
TEST_CASE("test build_user_history_msg_body","[cli_utils]")
{
    uint64_t from;
    std::string name;
    uint64_t count;
    std::string pass;
    std::string answer;
    {
        from = 0;
        name = "name";
        count = 0;
        pass = "pass";
        answer = "{\"uid\":0,\"user\":\"name\",\"count\":0,\"password\":\"pass\"}";
        std::string real_answ =  build_user_history_msg_body(from,name,count,pass);
        REQUIRE(real_answ == answer);
    }
    
}
