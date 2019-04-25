#pragma once

#include <thread>
#include <boost/noncopyable.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/make_shared.hpp>

#include "database.hpp"
#include "../../common/lock_queue.hpp"

#include "../../net/client.hpp"
#include "../../common/io_thread.hpp"

#include "../../common/http.hpp"
#include "../../client/src/apiclient_utils.hpp"

#define DB_USER_NAME "root"
#define DB_PASSWORD "password123"
#define DB_NAME "chat_db"

class DatabaseWorker
{
public:
    DatabaseWorker(db::type_t type,  size_t workers, std::string m_Host, int m_Port);
    void putTask(db::Task &&task);
    void run();
    void join();

private:
    void processQueue();
private:
    void sendRequest(input::cmd_t cmd, const std::string &req);

private:
    void onHttpConnect(const ConnectionError &e);
    void onHttpWrite(const ConnectionError &error);
    void onHttpRead(const ConnectionError &error, const HttpReply &reply);

    void onHttpIdleConnect(const ConnectionError &e);
    void onHttpIdleWrite(const ConnectionError &error);
    void onHttpIdleRead(const ConnectionError &error, const HttpReply &reply);
private:
    boost::shared_ptr<AsyncHttpClient> createConnect(bool use_ssl);

private:
    size_t m_Workers;
    Queue<db::Task> m_Queue;
    std::unique_ptr<AbstractDatabase> m_Db;
    std::vector<std::thread> m_Threads;
    boost::shared_ptr<AsyncHttpClient> m_Http;
    boost::shared_ptr<AsyncHttpClient> m_HttpIdle;

    std::unique_ptr<IoThread> m_IoThread;
    boost::asio::steady_timer m_Timeout;

    std::string m_Host;
    int m_Port;
    bool m_TimedOut;
    bool m_Ssl       = true;
    bool m_IdleState = false;

    input::cmd_t m_LastCmd = input::cmd_t::NONE;
    std::string m_User;
    std::string m_Password;
    std::string m_DBname;
    uint64_t m_SelfId     = 0;
    uint64_t m_HeartBit   = 0;
};
