#include "database.hpp"

#include "o2logger/src/o2logger.hpp"
#include <string>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

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
    o2logger::logi("updateUserHeartBit");
    try {
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
db::User MysqlConnection::createUser(const std::string& name, const std::string& pass, const std::string& stpath) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("createUser");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement("SELECT 1 FROM user WHERE name=?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
            return {};

        pstmt.reset(m_Connection->prepareStatement("INSERT INTO chat(name) VALUES(?)"));
        pstmt->setString(1, name);
        pstmt->executeUpdate();
        uint64_t chatId = getLastInsertId();

        pstmt.reset(m_Connection->prepareStatement("INSERT INTO user(self_chat_id, name, password, stpath, heartbit) "
                                                   "VALUES(?, ?, ?, ?, NOW())"));
        pstmt->setUInt64(1, chatId);
        pstmt->setString(2, name);
        pstmt->setString(3, pass);
        pstmt->setString(4, stpath);
        pstmt->executeUpdate();
        uint64_t userId = getLastInsertId();

        pstmt.reset(m_Connection->prepareStatement("INSERT INTO chatuser VALUES(?, ?)"));
        pstmt->setUInt64(1, userId);
        pstmt->setUInt64(2, chatId);
        pstmt->executeUpdate();

        return db::User(userId, chatId, name, pass, stpath);
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return {};
}

db::Chat MysqlConnection::createChat(const std::string& name, uint64_t uid) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("createChat");
    try {
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
    o2logger::logi("lookupUserByName");
    try {
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
    o2logger::logi("lookupUserById");
    try {
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
            o2logger::logi("lookupChatsForUserId");
            std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
            std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT chat_id FROM chatuser WHERE user_id=" +
                                                                    std::to_string(uid)));

            while (res->next())
                chats.push_back(res->getUInt64("chat_id"));
        }

        for (uint64_t chatid : chats)
            ret.push_back(lookupChatById(chatid));
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return ret;
}

std::vector<db::Chat> MysqlConnection::lookupChatByName(const std::string& name) const {
    std::vector<db::Chat> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("lookupChatByName");
    try {
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
    o2logger::logi("lookupChatById");
    try {
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM chat WHERE id=" +
                                                               std::to_string(chatid)));

        if (res->next())
            return db::Chat(res->getUInt64("id"), res->getString("name"));
    } catch (sql::SQLException& e) {
        outputError(e);
        return {};
    }
    return {};
}

std::vector<db::User> MysqlConnection::lookupUsersForChatId(uint64_t chatid) const {
    std::vector<db::User> ret;
    try {
        std::vector <uint64_t> uids;
        {
            std::lock_guard <std::mutex> lock(m_Mutex);
            o2logger::logi("lookupUsersForChatId");
            std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
            std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT user_id FROM chatuser WHERE chat_id=" +
                                                                    std::to_string(chatid)));

            while (res->next())
                uids.push_back(res->getUInt64("user_id"));
        }

        for (uint64_t uid : uids)
            ret.push_back(lookupUserById(uid));
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return ret;
}


void MysqlConnection::addUserToChat(const db::Chat& chat, const db::User& user) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("addUserToChat");
    try {
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM chatuser WHERE user_id=" +
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
    o2logger::logi("saveMessage");
    try {
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
    std::vector<db::Message> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("getMessages");
    try {
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        // go from recent messages to oldest
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT id, user_id, chat_id, flags, UNIX_TIMESTAMP(time)"
                                                               " AS unix_time, text FROM message WHERE chat_id=" +
                                                               std::to_string(chatid) + " AND UNIX_TIMESTAMP(time)>" +
                                                               std::to_string(opt.ts) + " ORDER BY id DESC" +
                                                               (opt.max_count ? " LIMIT " + std::to_string(opt.max_count)
                                                               : "")));

        while (res->next()) {
            db::Message msg(res->getUInt64("user_id"), res->getUInt64("chat_id"), res->getString("text"));
            msg.ts = res->getUInt("unix_time");
            msg.flags = static_cast<db::Message::flags_t>(res->getUInt("flags"));
            ret.push_back(msg);
            stmt->executeUpdate("UPDATE message SET flags=" +
                                std::to_string(static_cast<uint8_t>(db::Message::flags_t::READ)) + ") WHERE id=" +
                                std::to_string(res->getUInt64("id")));
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
    o2logger::logi("selectMessages");
    try {
        std::unique_ptr<sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT user_id, chat_id, flags, UNIX_TIMESTAMP(time) AS"
                                                               " unix_time, text FROM message ORDER BY id DESC" +
                                                               (opt.max_count ? " LIMIT " + std::to_string(opt.max_count)
                                                               : "")));

        while (res->next()) {
            db::Message msg(res->getUInt64("user_id"), res->getUInt64("chat_id"), res->getString("text"));
            msg.ts = res->getUInt("unix_time");
            msg.flags = static_cast<db::Message::flags_t>(res->getUInt("flags"));

            if (pred(msg))
                ret.push_back(msg);
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }

    return ret;
}

void MysqlConnection::outputError(sql::SQLException& e) const {
    o2logger::loge("# ERR: SQLException in ", __FILE__, "(", __FUNCTION__, ") on line ", __LINE__);
    o2logger::loge("# ERR: ", e.what(), " (MySQL error code: ", e.getErrorCode(), ", SQLState: ",
                   e.getSQLState(), " )");
}

uint64_t MysqlConnection::getLastInsertId(const std::unique_ptr<sql::Statement>& stmt) const {
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT LAST_INSERT_ID() AS id"));
    res->next();
    return res->getUInt64("id");
}

uint64_t MysqlConnection::getLastInsertId() const {
    std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement("SELECT LAST_INSERT_ID() AS id"));
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
    res->next();
    return res->getUInt64("id");
}
