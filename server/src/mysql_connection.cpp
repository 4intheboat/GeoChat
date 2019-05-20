#include "database.hpp"

#include "o2logger/src/o2logger.hpp"
#include <iostream>
#include <string>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <boost/asio.hpp>
#include "location_client.hpp"

using namespace o2logger;




sql::Driver* MysqlConnection::m_Driver = get_driver_instance();
std::mutex MysqlConnection::m_Mutex;

MysqlConnection::MysqlConnection() {
    try {
        m_Connection = std::unique_ptr<sql::Connection>(m_Driver->connect("localhost:3306", "me", "123"));
        m_Connection->setSchema("geochat");
    } catch (sql::SQLException& e) {
        outputError(e);
    }
}

MysqlConnection::~MysqlConnection() {}

void MysqlConnection::updateUserHeartBit(const db::User& user, time_t ts) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "updateUserHeartBit\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        stmt->executeUpdate("UPDATE user SET heartbit=FROM_UNIXTIME(" + std::to_string(ts) + ") WHERE id=" +
                            std::to_string(user.id));
    } catch (sql::SQLException& e) {
        outputError(e);
    }
}

/*
 * transaction start
 * insert into chat
 * into user ^^ right chat_id
 * into chatuser ^^ user_id + chat_id !!!
 * commit
*/
// TODO sql injections!!!
db::User MysqlConnection::createUser(const std::string& name, const std::string& pass, const std::string& stpath, const std::string &ip) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "createUser\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1 FROM user WHERE name='" + name + "'"));

        if (res->next())
            return {};

        std::cout << ip << std::endl;


        boost::asio::io_service io;
        LocationClient location_client(io);
        location_client.connect_to_api();
//        std::string ip = "91.192.20.94";  // Uncomment for check (Истра)

        std::string address = location_client.get_city_by_ip(ip);
        std::cout << "Этот IP из города " << address << std::endl;


        stmt->executeUpdate("INSERT INTO chat(name) VALUES('" + name + "')");
        uint64_t chatId = getLastInsertId(stmt);
        stmt->executeUpdate("INSERT INTO user(self_chat_id, name, password, stpath, ip, city, heartbit) VALUES(" +
                            std::to_string(chatId) + ", '" + name + "', '" + pass + "', '" + stpath + "', '" + ip + "', '" + address + "', NOW())");
        uint64_t userId = getLastInsertId(stmt);
        stmt->executeUpdate("INSERT INTO chatuser VALUES(" + std::to_string(userId) + ", " +
                            std::to_string(chatId) + ")");
        return db::User(userId, chatId, name, pass, stpath);
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return {};
}

db::Chat MysqlConnection::createChat(const std::string& name, uint64_t uid) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "createChat\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1 FROM chat WHERE name='" + name + "'"));

        if (res->next())
            return {};

        stmt->executeUpdate("INSERT INTO chat(name) VALUES('" + name + "')");
        uint64_t chatId = getLastInsertId(stmt);

        if (uid > 0) {
            stmt->executeUpdate("INSERT INTO chatuser VALUES(" + std::to_string(uid) + ", " +
                                std::to_string(chatId) + ")");
        }

        return db::Chat(chatId, name);
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return {};
}

std::vector<db::User> MysqlConnection::lookupUserByName(const std::string& name) const {
    std::vector<db::User> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "lookupUserByName\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM user WHERE name='" + name + "'"));

        while (res->next()) {
            ret.push_back(db::User(res->getUInt64("id"), res->getUInt64("self_chat_id"), res->getString("name"),
                                   res->getString("password"), res->getString("stpath")));
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return ret;
}

db::User MysqlConnection::lookupUserById(uint64_t id) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "lookupUserById\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM user WHERE id=" + std::to_string(id)));

        if (res->next()) {
            return db::User(res->getUInt64("id"), res->getUInt64("self_chat_id"), res->getString("name"),
                            res->getString("password"), res->getString("stpath"));
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return {};
}

std::vector<db::Chat> MysqlConnection::lookupChatsForUserId(uint64_t uid) const {
    std::vector<db::Chat> ret;
    std::vector <uint64_t> chats;
    try {
        {
            std::lock_guard <std::mutex> lock(m_Mutex);
            std::cout << "lookupChatsForUserId\n";
            std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
            std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT chat_id FROM chatuser WHERE user_id=" +
                                                                    std::to_string(uid)));

            while (res->next())
                chats.push_back(res->getUInt64("chat_id"));
        }

        for (uint64_t chatid : chats) {
            db::Chat chat = lookupChatById(chatid);
            ret.push_back(chat);
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return ret;
}

std::vector<db::Chat> MysqlConnection::lookupChatByName(const std::string& name) const {
    std::vector<db::Chat> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "lookupChatByName\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM chat WHERE name='" + name + "'"));

        while (res->next())
            ret.push_back(db::Chat(res->getUInt64("id"), res->getString("name")));
    } catch (sql::SQLException& e) {
        outputError(e);
        return ret;
    }
    return ret;
}

db::Chat MysqlConnection::lookupChatById(uint64_t chatid) const {
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "lookupChatById\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM chat WHERE id=" +
                                                               std::to_string(chatid)));

        if (res->next()) {
            std::cout << chatid << " has next\n";
            return db::Chat(res->getUInt64("id"), res->getString("name"));
        }
    } catch (sql::SQLException& e) {
        outputError(e);
        return {};
    }
    return {};
}

std::vector<db::User> MysqlConnection::lookupUsersForChatId(uint64_t chatid) const {
    std::vector<db::User> ret;
    std::vector <uint64_t> uids;
    try {
        {
            std::lock_guard <std::mutex> lock(m_Mutex);
            std::cout << "lookupUsersForChatId\n";
            std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
            std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT user_id FROM chatuser WHERE chat_id=" +
                                                                    std::to_string(chatid)));

            while (res->next())
                uids.push_back(res->getUInt64("user_id"));
        }

        for (uint64_t uid : uids) {
            db::User user = lookupUserById(uid);
            ret.push_back(user);
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return ret;
}


void MysqlConnection::addUserToChat(const db::Chat& chat, const db::User& user) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "addUserToChat\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1 FROM chatuser WHERE user_id=" +
                                            std::to_string(user.id) + " and chat_id=" + std::to_string(chat.id)));

        if (res->next())
            return;

        stmt->executeUpdate("INSERT INTO chatuser VALUES(" + std::to_string(user.id) + ", " +
                            std::to_string(chat.id) + ")");
    } catch (sql::SQLException& e) {
        outputError(e);
    }
}

void MysqlConnection::saveMessage(const db::Message& msg) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "saveMessage\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        stmt->executeUpdate("INSERT INTO message(user_id, chat_id, flags, time, text) VALUES(" +
                            std::to_string(msg.user_from) + ", " + std::to_string(msg.chat_to) + ", " +
                            std::to_string(static_cast<uint8_t>(msg.flags)) + ", FROM_UNIXTIME(" +
                            std::to_string(msg.ts) + "), '" + msg.message + "')");
    } catch (sql::SQLException& e) {
        outputError(e);
    }
}

std::vector<db::Message> MysqlConnection::getMessages(uint64_t chatid, const db::get_msg_opt_t& opt) const {
    // go from recent messages to oldest
    std::vector<db::Message> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "getMessages\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT id, user_id, chat_id, flags, UNIX_TIMESTAMP(time)"
                                                               " AS unix_time, text FROM message WHERE chat_id=" +
                                                               std::to_string(chatid) + " AND UNIX_TIMESTAMP(time)>" +
                                                               std::to_string(opt.ts) + " ORDER BY id DESC"));

        while (res->next()) {
            db::Message msg(res->getUInt64("user_id"), res->getUInt64("chat_id"), res->getString("text"));
            msg.ts = res->getUInt("unix_time");
            msg.flags = static_cast<db::Message::flags_t>(res->getUInt("flags"));
            ret.push_back(msg);
            stmt->executeUpdate("UPDATE message SET flags=" +
                                std::to_string(static_cast<uint8_t>(db::Message::flags_t::READ)) + ") WHERE id=" +
                                std::to_string(res->getUInt64("id")));

            if (opt.max_count && ret.size() >= opt.max_count)
                break;
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return ret;
}

std::vector<db::Message> MysqlConnection::selectMessages(std::function<bool(const db::Message&)>&& pred,
                                                         const db::get_msg_opt_t& opt) const {
    std::vector<db::Message> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    try {
        std::cout << "selectMessages\n";
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT user_id, chat_id, flags, UNIX_TIMESTAMP(time) AS"
                                                               " unix_time, text FROM message ORDER BY id DESC"));

        while (res->next()) {
            db::Message msg(res->getUInt64("user_id"), res->getUInt64("chat_id"), res->getString("text"));
            msg.ts = res->getUInt("unix_time");
            msg.flags = static_cast<db::Message::flags_t>(res->getUInt("flags"));

            if (pred(msg))
                ret.push_back(msg);

            if (opt.max_count && ret.size() >= opt.max_count)
                break;
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }

    return ret;
}

void MysqlConnection::outputError(sql::SQLException& e) const {
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
}

uint64_t MysqlConnection::getLastInsertId(const std::unique_ptr<sql::Statement>& stmt) const {
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT LAST_INSERT_ID() AS id"));
    res->next();
    return res->getUInt64("id");
}
