#define CATCH_CONFIG_MAIN


#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "libproperty/src/libproperty.hpp"

#include <boost/make_shared.hpp>

#include <chrono>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <string>

#include <boost/optional.hpp>


#include "common/utils.hpp"
#include "common/sysutils.hpp"
#include "common/lock_queue.hpp"
//#include "../../client/src/client.hpp"
#include "../../client/src/event_worker.hpp"
#include "../../client/src/ui.hpp"
#include "../../client/src/apiclient_utils.hpp"

#include "catch.hpp"

#include "o2logger/src/o2logger.hpp"

#define OK 0

/*TEST_CASE("run client", "[Client]")
{
    //Client::Client(const std::string& host, int port, const std::string &user, const std::string &password)
    std::string a("host");
    //Client client(a, 8001,a,a);

    try
    {
       //Client client(a, 8001,a,a);
       //client.run(); 
    }
    catch(int i)
    {
        
    }
}
   //EventsWorker(size_t workers);
    //void putEvent(Event &&event);
    //void run();
    //void join();
    //void registerOnInputCallback(std::function<void(Event &&event)> &&callback);
   
TEST_CASE("run event worker", "[EventsWorker]")
{
    try
    {
        size_t num = 5;
        EventsWorker ew(num);
        ew.run();
    }
    catch(int i){}
}
TEST_CASE("join event worker", "[EventsWorker]")
{
    try
    {
        size_t num = 5;
        EventsWorker ew(num);
        ew.join();
    }
    catch(int i){}
}
TEST_CASE("putevent event worker", "[EventsWorker]")
{
    try
    {
        size_t num = 5;
        Event event;
        EventsWorker ew(num);
        ew.putEvent(enent);
    }
    catch(int i){}
}
TEST_CASE("registeroninput callback worker", "[EventsWorker]")
{
    try
    {
        size_t num = 5;
        std::function<void(Event &&event)> callback;
        EventsWorker ew(num);
        ew.registerOnInputCallback(callback);
    }
    catch(int i){}
}
 //virtual void showMsg(const ui::Message &msg) = 0;
   // virtual void showMsg(const std::string &msg) = 0;
    //virtual void run() = 0;
    //virtual void join() = 0;

TEST_CASE("consolerun","[ConsoleUi]")
{
    try
    {
        size_t num = 5;
        EventsWorker ew(num);
        ConsoleUi ui(ew);
        ui.run();
    }
    catch(int i){}
}

TEST_CASE("consolejoin","[ConsoleUi]")
{
    try
    {
        size_t num = 5;
        EventsWorker ew(num);
        ConsoleUi ui(ew);
        ui.join();
    }
    catch(int i){}
}
*/

TEST_CASE("test eventsworker", "[EventsWorker]")
{
    size_t num = 5;
    EventsWorker ew(num);

    SECTION ("TEST eventsworker -> run", "[EventsWorker]"){
        REQUIRE(ew.run() == OK);  
    }
    SECTION ("TEST eventsworker -> join", "[EventsWorker]"){
        REQUIRE(ew.join() == OK);  
    }

    SECTION ("TEST eventsworker -> putEvent", "[EventsWorker]"){
        Event event;
        REQUIRE(ew.putEvent(event) == OK);  
    }
    SECTION ("TEST eventsworker -> register", "[EventsWorker]"){
        Event event;
        REQUIRE(ew.registerOnInputCallback(callback) == OK);  
    }
}
TEST_CASE("test client", "[Client]")
{
    std::string a("host");
    Client client(a, 8001,a,a);

    SECTION ("TEST Client -> run", "[Client]"){
        REQUIRE(client.run() == OK);  
    }
}
TEST_CASE("test consoleUi", "[ConsoleUi]")
{
    size_t num = 5;
    EventsWorker ew(num);
    ConsoleUi ui(ew);

    SECTION ("TEST ConcoleUi -> run", "[ConsoleUi]"){
        REQUIRE(ui.run() == OK);  
    }
    SECTION ("TEST ConcoleUi -> join", "[ConsoleUi]"){
        REQUIRE(ui.join() == OK);  
    }
}