#include "database.hpp"

#include "o2logger/src/o2logger.hpp"
#include <string>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace o2logger;


<<<<<<< HEAD
sql::Driver *MysqlConnection::m_Driver = get_driver_instance();
=======
sql::Driver* MysqlConnection::m_Driver = get_driver_instance();
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
std::mutex MysqlConnection::m_Mutex;

MysqlConnection::MysqlConnection() {
    try {
        m_Connection = std::unique_ptr<sql::Connection>(m_Driver->connect("localhost:3306", "me", "123"));
        m_Connection->setSchema("geochat");
    } catch (sql::SQLException &e) {
        outputError(e);
    }
}

MysqlConnection::~MysqlConnection() {}

<<<<<<< HEAD
void MysqlConnection::updateUserHeartBit(const db::User &user, time_t ts) {
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("updateUserHeartBit");
    try {
        std::cout << "updateUserHeartBit\n";
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        stmt->executeUpdate("UPDATE user SET heartbit=FROM_UNIXTIME(" + std::to_string(ts) + ") WHERE id=" +
                            std::to_string(user.id));
    } catch (sql::SQLException &e) {
=======
void MysqlConnection::updateUserHeartBit(const db::User& user, time_t ts) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("updateUserHeartBit");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "UPDATE user SET heartbit=FROM_UNIXTIME(?) WHERE id=?"));
        pstmt->setUInt(1, ts);
        pstmt->setUInt64(2, user.id);
        pstmt->executeUpdate();
    } catch (sql::SQLException& e) {
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
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
<<<<<<< HEAD
db::User MysqlConnection::createUser(const std::string &name, const std::string &pass, const std::string &stpath,
                                     const std::string &ip) {
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("createUser");
    try {
        std::unique_ptr <sql::PreparedStatement> pstmt(
                m_Connection->prepareStatement("SELECT 1 FROM user WHERE name=?"));
        pstmt->setString(1, name);
        std::unique_ptr <sql::ResultSet> res(pstmt->executeQuery());
=======
db::User MysqlConnection::createUser(const std::string& name, const std::string& pass, const std::string& stpath,
                                     const std::string& ip, const std::string& city) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("createUser");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT 1 FROM user WHERE name=?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        if (res->next())
            return {};

<<<<<<< HEAD
        std::string city = "Moscow";

=======
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        pstmt.reset(m_Connection->prepareStatement("INSERT INTO chat(name) VALUES(?)"));
        pstmt->setString(1, name);
        pstmt->executeUpdate();
        uint64_t chatId = getLastInsertId();

<<<<<<< HEAD
        pstmt.reset(m_Connection->prepareStatement("INSERT INTO user(self_chat_id, name, password, stpath, heartbit,"
                                                   "ip, city) VALUES(?, ?, ?, ?, NOW(), ?, ?)"));
=======
        pstmt.reset(m_Connection->prepareStatement(
                "INSERT INTO user(self_chat_id, name, password, stpath, heartbit, ip, city) "
                "VALUES(?, ?, ?, ?, FROM_UNIXTIME(0), ?, ?)"));
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        pstmt->setUInt64(1, chatId);
        pstmt->setString(2, name);
        pstmt->setString(3, pass);
        pstmt->setString(4, stpath);
        pstmt->setString(5, ip);
        pstmt->setString(6, city);
        pstmt->executeUpdate();
        uint64_t userId = getLastInsertId();

        pstmt.reset(m_Connection->prepareStatement("INSERT INTO chatuser VALUES(?, ?)"));
        pstmt->setUInt64(1, userId);
        pstmt->setUInt64(2, chatId);
        pstmt->executeUpdate();

<<<<<<< HEAD
        return db::User(userId, chatId, name, pass, stpath);
    } catch (sql::SQLException &e) {
=======
        return db::User(userId, chatId, name, pass, stpath, ip, city);
    } catch (sql::SQLException& e) {
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        outputError(e);
    }
    return {};
}

<<<<<<< HEAD
db::Chat MysqlConnection::createChat(const std::string &name, uint64_t uid) {
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("createChat");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT 1 FROM chat WHERE name='" + name + "'"));
=======
db::Chat MysqlConnection::createChat(const std::string& name, uint64_t uid) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("createChat");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT 1 FROM chat WHERE name=?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        if (res->next())
            return {};

        pstmt.reset(m_Connection->prepareStatement("INSERT INTO chat(name) VALUES(?)"));
        pstmt->setString(1, name);
        pstmt->executeUpdate();
        uint64_t chatId = getLastInsertId();

        if (uid > 0) {
            pstmt.reset(m_Connection->prepareStatement("INSERT INTO chatuser VALUES(?, ?)"));
            pstmt->setUInt64(1, uid);
            pstmt->setUInt64(2, chatId);
            pstmt->executeUpdate();
        }

        return db::Chat(chatId, name);
    } catch (sql::SQLException &e) {
        outputError(e);
    }
    return {};
}

<<<<<<< HEAD
std::vector <db::User> MysqlConnection::lookupUserByName(const std::string &name) const {
    std::vector <db::User> ret;
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("lookupUserByName");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT * FROM user WHERE name='" + name + "'"));
=======
std::vector<db::User> MysqlConnection::lookupUserByName(const std::string& name) const {
    std::vector<db::User> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("lookupUserByName");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT id, self_chat_id, name, password, stpath, ip, city, UNIX_TIMESTAMP(heartbit) AS unix_heartbit "
                "FROM user WHERE name=?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        while (res->next()) {
            ret.push_back(db::User(res->getUInt64("id"), res->getUInt64("self_chat_id"), res->getString("name"),
                                   res->getString("password"), res->getString("stpath"), res->getString("ip"),
                                   res->getString("city"), res->getUInt("unix_heartbit")));
        }
    } catch (sql::SQLException &e) {
        outputError(e);
    }
    return ret;
}

db::User MysqlConnection::lookupUserById(uint64_t id) const {
<<<<<<< HEAD
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("lookupUserById");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT * FROM user WHERE id=" + std::to_string(id)));
=======
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("lookupUserById");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT id, self_chat_id, name, password, stpath, ip, city, UNIX_TIMESTAMP(heartbit) AS unix_heartbit "
                "FROM user WHERE id=?"));
        pstmt->setUInt64(1, id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        if (res->next()) {
            return db::User(res->getUInt64("id"), res->getUInt64("self_chat_id"), res->getString("name"),
                            res->getString("password"), res->getString("stpath"), res->getString("ip"),
                            res->getString("city"), res->getUInt("unix_heartbit"));
        }
    } catch (sql::SQLException &e) {
        outputError(e);
    }
    return {};
}

<<<<<<< HEAD
std::vector <db::Chat> MysqlConnection::lookupChatsForUserId(uint64_t uid) const {
    std::vector <db::Chat> ret;
    std::vector <uint64_t> chats;
=======
std::vector<db::User> MysqlConnection::lookupUserByCity(const std::string& city) const {
    std::vector<db::User> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("lookupUserByCity");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT id, self_chat_id, name, password, stpath, ip, city, UNIX_TIMESTAMP(heartbit) AS unix_heartbit "
                "FROM user WHERE city=?"));
        pstmt->setString(1, city);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next()) {
            ret.push_back(db::User(res->getUInt64("id"), res->getUInt64("self_chat_id"), res->getString("name"),
                                   res->getString("password"), res->getString("stpath"), res->getString("ip"),
                                   res->getString("city"), res->getUInt("unix_heartbit")));
        }
    } catch (sql::SQLException& e) {
        outputError(e);
    }
    return ret;
}

std::vector<db::Chat> MysqlConnection::lookupChatsForUserId(uint64_t uid) const {
    std::vector<db::Chat> ret;
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
    try {
        std::vector <uint64_t> chats;
        {
            std::lock_guard <std::mutex> lock(m_Mutex);
            o2logger::logi("lookupChatsForUserId");
<<<<<<< HEAD
            std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
            std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT chat_id FROM chatuser WHERE user_id=" +
                                                                    std::to_string(uid)));
=======
            std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                    "SELECT chat_id FROM chatuser WHERE user_id=?"));
            pstmt->setUInt64(1, uid);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

            while (res->next())
                chats.push_back(res->getUInt64("chat_id"));
        }

        for (uint64_t chatid : chats)
            ret.push_back(lookupChatById(chatid));
<<<<<<< HEAD
    } catch (sql::SQLException &e) {
=======
    } catch (sql::SQLException& e) {
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        outputError(e);
    }
    return ret;
}

<<<<<<< HEAD
std::vector <db::Chat> MysqlConnection::lookupChatByName(const std::string &name) const {
    std::vector <db::Chat> ret;
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("lookupChatByName");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT * FROM chat WHERE name='" + name + "'"));
=======
std::vector<db::Chat> MysqlConnection::lookupChatByName(const std::string& name) const {
    std::vector<db::Chat> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("lookupChatByName");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT * FROM chat WHERE name=?"));
        pstmt->setString(1, name);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        while (res->next())
            ret.push_back(db::Chat(res->getUInt64("id"), res->getString("name")));
    } catch (sql::SQLException &e) {
        outputError(e);
    }
    return ret;
}

db::Chat MysqlConnection::lookupChatById(uint64_t chatid) const {
<<<<<<< HEAD
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("lookupChatById");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT * FROM chat WHERE id=" +
                                                                std::to_string(chatid)));

        if (res->next())
            return db::Chat(res->getUInt64("id"), res->getString("name"));
    } catch (sql::SQLException &e) {
=======
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("lookupChatById");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT * FROM chat WHERE id=?"));
        pstmt->setUInt64(1, chatid);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
            return db::Chat(res->getUInt64("id"), res->getString("name"));
    } catch (sql::SQLException& e) {
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        outputError(e);
    }
    return {};
}

<<<<<<< HEAD
std::vector <db::User> MysqlConnection::lookupUsersForChatId(uint64_t chatid) const {
    std::vector <db::User> ret;
=======
std::vector<db::User> MysqlConnection::lookupUsersForChatId(uint64_t chatid) const {
    std::vector<db::User> ret;
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
    try {
        std::vector <uint64_t> uids;
        {
            std::lock_guard <std::mutex> lock(m_Mutex);
            o2logger::logi("lookupUsersForChatId");
<<<<<<< HEAD
            std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
            std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT user_id FROM chatuser WHERE chat_id=" +
                                                                    std::to_string(chatid)));
=======
            std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                    "SELECT user_id FROM chatuser WHERE chat_id=?"));
            pstmt->setUInt64(1, chatid);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

            while (res->next())
                uids.push_back(res->getUInt64("user_id"));
        }

        for (uint64_t uid : uids)
            ret.push_back(lookupUserById(uid));
<<<<<<< HEAD
    } catch (sql::SQLException &e) {
=======
    } catch (sql::SQLException& e) {
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        outputError(e);
    }
    return ret;
}


<<<<<<< HEAD
void MysqlConnection::addUserToChat(const db::Chat &chat, const db::User &user) {
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("addUserToChat");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT * FROM chatuser WHERE user_id=" +
                                                                std::to_string(user.id) + " and chat_id=" +
                                                                std::to_string(chat.id)));
=======
void MysqlConnection::addUserToChat(const db::Chat& chat, const db::User& user) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("addUserToChat");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT * FROM chatuser WHERE user_id=? AND chat_id=?"));
        pstmt->setUInt64(1, user.id);
        pstmt->setUInt64(2, chat.id);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        if (res->next())
            return;

<<<<<<< HEAD
        stmt->executeUpdate("INSERT INTO chatuser VALUES(" + std::to_string(user.id) + ", " +
                            std::to_string(chat.id) + ")");
    } catch (sql::SQLException &e) {
=======
        pstmt.reset(m_Connection->prepareStatement("INSERT INTO chatuser VALUES(?, ?)"));
        pstmt->setUInt64(1, user.id);
        pstmt->setUInt64(2, chat.id);
        pstmt->executeUpdate();
    } catch (sql::SQLException& e) {
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        outputError(e);
    }
}

<<<<<<< HEAD
void MysqlConnection::saveMessage(const db::Message &msg) {
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("saveMessage");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        stmt->executeUpdate("INSERT INTO message(user_id, chat_id, flags, time, text) VALUES(" +
                            std::to_string(msg.user_from) + ", " + std::to_string(msg.chat_to) + ", " +
                            std::to_string(static_cast<uint8_t>(msg.flags)) + ", FROM_UNIXTIME(" +
                            std::to_string(msg.ts) + "), '" + msg.message + "')");
    } catch (sql::SQLException &e) {
=======
void MysqlConnection::saveMessage(const db::Message& msg) {
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("saveMessage");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "INSERT INTO message(user_id, chat_id, flags, time, text) VALUES(?, ?, ?, FROM_UNIXTIME(?), ?)"));
        pstmt->setUInt64(1, msg.user_from);
        pstmt->setUInt64(2, msg.chat_to);
        pstmt->setUInt(3, static_cast<uint8_t>(msg.flags));
        pstmt->setUInt(4, msg.ts);
        pstmt->setString(5, msg.message);
        pstmt->executeUpdate();
    } catch (sql::SQLException& e) {
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        outputError(e);
    }
}

<<<<<<< HEAD
std::vector <db::Message> MysqlConnection::getMessages(uint64_t chatid, const db::get_msg_opt_t &opt) const {
    std::vector <db::Message> ret;
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("getMessages");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        // go from recent messages to oldest
        std::unique_ptr <sql::ResultSet> res(
                stmt->executeQuery("SELECT id, user_id, chat_id, flags, UNIX_TIMESTAMP(time)"
                                   " AS unix_time, text FROM message WHERE chat_id=" +
                                   std::to_string(chatid) + " AND UNIX_TIMESTAMP(time)>" +
                                   std::to_string(opt.ts) + " ORDER BY id DESC" +
                                   (opt.max_count ? " LIMIT " + std::to_string(opt.max_count)
                                                  : "")));
=======
std::vector<db::Message> MysqlConnection::getMessages(uint64_t chatid, const db::get_msg_opt_t& opt) const {
    std::vector<db::Message> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("getMessages");
    try {
        // go from recent messages to oldest
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT id, user_id, chat_id, flags, UNIX_TIMESTAMP(time) AS unix_time, text FROM message "
                "WHERE chat_id=? AND UNIX_TIMESTAMP(time)>? ORDER BY id DESC" +
                (opt.max_count ? " LIMIT " + std::to_string(opt.max_count) : "")));
        pstmt->setUInt64(1, chatid);
        pstmt->setUInt(2, opt.ts);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        while (res->next()) {
            db::Message msg(res->getUInt64("user_id"), res->getUInt64("chat_id"), res->getString("text"));
            msg.ts = res->getUInt("unix_time");
            msg.flags = static_cast<db::Message::flags_t>(res->getUInt("flags"));
            ret.push_back(msg);
<<<<<<< HEAD
            stmt->executeUpdate("UPDATE message SET flags=" +
                                std::to_string(static_cast<uint8_t>(db::Message::flags_t::READ)) + ") WHERE id=" +
                                std::to_string(res->getUInt64("id")));
=======

            pstmt.reset(m_Connection->prepareStatement("UPDATE message SET flags=? WHERE id=?"));
            pstmt->setUInt(1, static_cast<uint8_t>(db::Message::flags_t::READ));
            pstmt->setUInt64(2, res->getUInt64("id"));
            pstmt->executeUpdate();
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
        }
    } catch (sql::SQLException &e) {
        outputError(e);
    }
    return ret;
}

<<<<<<< HEAD
std::vector <db::Message> MysqlConnection::selectMessages(std::function<bool(const db::Message &)> &&pred,
                                                          const db::get_msg_opt_t &opt) const {
    std::vector <db::Message> ret;
    std::lock_guard <std::mutex> lock(m_Mutex);
    o2logger::logi("selectMessages");
    try {
        std::unique_ptr <sql::Statement> stmt(m_Connection->createStatement());
        std::unique_ptr <sql::ResultSet> res(
                stmt->executeQuery("SELECT user_id, chat_id, flags, UNIX_TIMESTAMP(time) AS"
                                   " unix_time, text FROM message ORDER BY id DESC" +
                                   (opt.max_count ? " LIMIT " + std::to_string(opt.max_count)
                                                  : "")));
=======
std::vector<db::Message> MysqlConnection::selectMessages(std::function<bool(const db::Message&)>&& pred,
                                                         const db::get_msg_opt_t& opt) const {
    std::vector<db::Message> ret;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("selectMessages");
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT user_id, chat_id, flags, UNIX_TIMESTAMP(time) AS unix_time, text FROM message "
                "ORDER BY id DESC" + (opt.max_count ? " LIMIT " + std::to_string(opt.max_count) : "")));
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf

        while (res->next()) {
            db::Message msg(res->getUInt64("user_id"), res->getUInt64("chat_id"), res->getString("text"));
            msg.ts = res->getUInt("unix_time");
            msg.flags = static_cast<db::Message::flags_t>(res->getUInt("flags"));

            if (pred(msg))
                ret.push_back(msg);
        }
    } catch (sql::SQLException &e) {
        outputError(e);
    }

    return ret;
}

<<<<<<< HEAD
void MysqlConnection::outputError(sql::SQLException &e) const {
    o2logger::loge("# ERR: SQLException in ", __FILE__, "(", __FUNCTION__, ") on line ", __LINE__);
    o2logger::loge("# ERR: ", e.what(), " (MySQL error code: ", e.getErrorCode(), ", SQLState: ",
                   e.getSQLState(), " )");
}

uint64_t MysqlConnection::getLastInsertId(const std::unique_ptr <sql::Statement> &stmt) const {
    std::unique_ptr <sql::ResultSet> res(stmt->executeQuery("SELECT LAST_INSERT_ID() AS id"));
    res->next();
    return res->getUInt64("id");
=======
std::set<std::string> MysqlConnection::getAllDistinctLocations() const {
    std::set<std::string> result;
    std::lock_guard<std::mutex> lock(m_Mutex);
    o2logger::logi("getAllDistinctLocations");

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement(
                "SELECT DISTINCT city FROM user"));
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        while (res->next())
            result.insert(res->getString("city"));
    } catch (sql::SQLException& e) {
        outputError(e);
    }

    return result;
}

void MysqlConnection::outputError(sql::SQLException& e) const {
    o2logger::loge("# ERR: SQLException in ", __FILE__, "(", __FUNCTION__, ") on line ", __LINE__);
    o2logger::loge("# ERR: ", e.what(), " (MySQL error code: ", e.getErrorCode(), ", SQLState: ",
                   e.getSQLState(), " )");
>>>>>>> 624259933e99440f41a5dbfaad6efc99b6cae1cf
}

uint64_t MysqlConnection::getLastInsertId() const {
    std::unique_ptr <sql::PreparedStatement> pstmt(m_Connection->prepareStatement("SELECT LAST_INSERT_ID() AS id"));
    std::unique_ptr <sql::ResultSet> res(pstmt->executeQuery());
    res->next();
    return res->getUInt64("id");
}

uint64_t MysqlConnection::getLastInsertId() const {
    std::unique_ptr<sql::PreparedStatement> pstmt(m_Connection->prepareStatement("SELECT LAST_INSERT_ID() AS id"));
    std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
    res->next();
    return res->getUInt64("id");
}
