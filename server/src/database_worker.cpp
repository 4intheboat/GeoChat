#include <chrono>
#include <atomic>
#include <thread>

#include "apiclient.hpp"
#include "apiclient_utils.hpp"
#include "database_worker.hpp"
#include "../../common/common.hpp"
#include "location.h"
#include <stdio.h>
#include <iostream>


extern std::atomic<bool> g_NeedStop;


DatabaseWorker::DatabaseWorker(db::type_t type,  size_t workers, std::string m_Host, int m_Port):
        m_Workers(workers),
        m_IoThread(std::make_unique<IoThread>("")),
        m_Timeout(m_IoThread->ioService()),
        m_Host(m_Host),
        m_Port(m_Port),
        m_User(DB_USER_NAME),
        m_Password(DB_PASSWORD),
        m_DBname(DB_NAME)
{
    f::loge("databaseworker constructor");
    loge("blablalba");
    m_Ssl = true;
    m_Http = createConnect(m_Ssl);
    m_HttpIdle = createConnect(m_Ssl);
    m_TimedOut = false;
    if (type == db::type_t::MEMORY) {
        m_Db = std::unique_ptr<AbstractDatabase>(new InMemoryDataBase());
    } else {
        m_Db = std::unique_ptr<AbstractDatabase>(new MysqlDataBase());
    }
}

void DatabaseWorker::putTask(db::Task &&task) {
    m_Queue.push(std::move(task));
}

namespace {

    db::User lookup_check_pass(const db::Task &task, AbstractConnection *conn) {
        db::User user = conn->lookupUserById(task.request.uid);
        if (user.id == 0) {
            task.client->sendErrorResponse(409, common::ApiStatusCode::ERR_CONSTRAINT, "user does not exist");
            return {};
        }

        std::string encrypted_pass = std::to_string(utils::crc32(task.request.password));
        if (encrypted_pass != user.password) {
            task.client->sendErrorResponse(403, common::ApiStatusCode::ERR_CONSTRAINT, "wrong password");
            return {};
        }
        return user;
    }

    db::User lookup_check_pass_by_name(const std::string &name, const db::Task &task, AbstractConnection *conn,
                                       bool check_pass) {
        std::vector <db::User> users = conn->lookupUserByName(name);
        if (users.size() != 1) {
            if (users.empty()) {
                // 401 ?
                task.client->sendErrorResponse(404, common::ApiStatusCode::ERR_NOT_FOUND, "user does not exist");
                return {};
            }
            task.client->sendErrorResponse(500, common::ApiStatusCode::ERR_INTERNAL,
                                           "more than one user with that name");
            return {};
        }

        const db::User &user = users[0];

        if (check_pass) {
            std::string encrypted_pass = std::to_string(utils::crc32(task.request.password));
            if (encrypted_pass != user.password) {
                task.client->sendErrorResponse(403, common::ApiStatusCode::ERR_CONSTRAINT, "wrong password");
                return {};
            }
        }

        return user;
    }

    std::vector <apiclient_utils::Message>
    to_api_format(std::vector <std::vector<db::Message>> &&msgs_batch, AbstractConnection *conn) {
        std::vector <apiclient_utils::Message> ret;

        for (const auto &msgs : msgs_batch) {
            for (const auto &msg : msgs) {
                db::Chat chat = conn->lookupChatById(msg.chat_to);
                db::User user = conn->lookupUserById(msg.user_from);
                ret.emplace_back(apiclient_utils::Message(msg.ts, /*from*/user.name, /*to*/chat.name, msg.message));
            }
        }

        return ret;
    }

    std::vector <apiclient_utils::Message> to_api_format(std::vector <db::Message> &&msgs, AbstractConnection *conn) {
        std::vector <apiclient_utils::Message> ret;

        for (const auto &msg : msgs) {
            db::Chat chat = conn->lookupChatById(msg.chat_to);
            db::User user = conn->lookupUserById(msg.user_from);
            ret.emplace_back(apiclient_utils::Message(msg.ts, /*from*/user.name, /*to*/chat.name, msg.message));
        }

        return ret;
    }

    std::vector <db::Message>
    mix_from_and_to_messages(std::vector <db::Message> &&from, std::vector <db::Message> &&to, uint64_t max) {
        std::vector <db::Message> tmp;
        tmp.reserve(from.size() + to.size());
        tmp.insert(tmp.end(), std::make_move_iterator(from.begin()), std::make_move_iterator(from.end()));
        tmp.insert(tmp.end(), std::make_move_iterator(to.begin()), std::make_move_iterator(to.end()));

        std::sort(tmp.begin(), tmp.end(), [](const db::Message &msg1, const db::Message &msg2) {
            return msg1.ts > msg2.ts;
        });

        if (tmp.size() <= max) {
            return tmp;
        }

        std::vector<db::Message>::const_iterator first = tmp.begin();
        std::vector<db::Message>::const_iterator last = tmp.begin() + max;
        std::vector <db::Message> ret(first, last);

        return ret;
    }

}   // namespace

void DatabaseWorker::processQueue() {
    std::unique_ptr <AbstractConnection> conn = m_Db->getConnection();

    while (!g_NeedStop) {
        db::Task task;
        if (!m_Queue.getTask(task)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        if (task.cmd == common::cmd_t::IDLE) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }

            conn->updateUserHeartBit(user, time(NULL));

            std::vector <std::vector<db::Message>> msgs_batch;

            std::vector <db::Chat> chats = conn->lookupChatsForUserId(user.id);
            for (const auto &chat : chats) {
                // TODO: it does not see messages sored in one second!
                //       need flags: read/unread e.t.c.

                db::get_msg_opt_t opt;
                opt.ts = task.request.ts;
                //opt.only_unread = true;
                std::vector <db::Message> msgs = conn->getMessages(chat.id, opt);
                if (!msgs.empty()) {
                    msgs_batch.emplace_back(msgs);
                }
            }

            if (!msgs_batch.empty()) {
                std::vector <apiclient_utils::Message> api_msgs = to_api_format(std::move(msgs_batch), conn.get());
                task.client->sendMessagesToIdleConn(std::move(api_msgs));
                continue;
            }

            if (task.ping) {
                task.client->sendMessagesToIdleConn({});
            }
        } else if (task.cmd == common::cmd_t::USER_STATUS) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }

            db::User user_to = lookup_check_pass_by_name(task.request.user, task, conn.get(), false);
            if (user_to.id == 0) {
                continue;
            }

            std::string body = apiclient_utils::build_api_ok_response_body(user_to);
            task.client->sendOkResponse(body);
        } else if (task.cmd == common::cmd_t::USER_HISTORY) {
            db::User user_from = lookup_check_pass(task, conn.get());
            if (user_from.id == 0) {
                continue;
            }

            db::User user_to = lookup_check_pass_by_name(task.request.user, task, conn.get(), false);
            if (user_to.id == 0) {
                continue;
            }

            auto f1 = [&user_from, &user_to](const db::Message &msg) -> bool {
                if (msg.user_from == user_from.id && msg.chat_to == user_to.self_chat_id) {
                    return true;
                }
                return false;
            };

            db::get_msg_opt_t opt;
            opt.max_count = task.request.count * 2; // dirty hack :)
            std::vector <db::Message> msgs_to = conn->selectMessages(std::move(f1), opt);

            auto f2 = [&user_from, &user_to](const db::Message &msg) -> bool {
                if (msg.user_from == user_to.id && msg.chat_to == user_from.self_chat_id) {
                    return true;
                }
                return false;
            };
            std::vector <db::Message> msgs_from = conn->selectMessages(std::move(f2), opt);

            std::vector <db::Message> mix = mix_from_and_to_messages(std::move(msgs_from), std::move(msgs_to),
                                                                     task.request.count);
            std::vector <apiclient_utils::Message> api_msgs = to_api_format(std::move(mix), conn.get());
            task.client->sendMessages(std::move(api_msgs));
        } else if (task.cmd == common::cmd_t::MESSAGE_SEND) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }

            std::vector <db::User> users_to = conn->lookupUserByName(task.request.to_user);
            if (users_to.size() != 1) {
                if (users_to.empty()) {
                    task.client->sendErrorResponse(404, common::ApiStatusCode::ERR_NOT_FOUND, "user to does not exist");
                    continue;
                }
                task.client->sendErrorResponse(500, common::ApiStatusCode::ERR_INTERNAL,
                                               "more than one user with that name");
                continue;
            }

            const db::User &user_to = users_to[0];
            db::Message msg(/*from*/user.id, /*to*/user_to.self_chat_id, task.request.message);

            // TODO: need milliseconds!
            msg.ts = time(NULL);
            conn->saveMessage(msg);
            task.client->sendOkResponse("{\"status\": 0}");
        } else if (task.cmd == common::cmd_t::USER_CREATE) {
            std::string encrypted_pass = std::to_string(utils::crc32(task.request.password));
            db::User user = conn->createUser(task.request.user, encrypted_pass, task.storage);
            if (user.id == 0) {
                task.client->sendErrorResponse(409, common::ApiStatusCode::ERR_CONSTRAINT, "user already exists");
                continue;
            }
            std::string body = apiclient_utils::build_api_ok_response_body(user);
            task.client->sendOkResponse(body);
        } else if (task.cmd == common::cmd_t::USER_LOGIN) {
            db::User user = lookup_check_pass_by_name(task.request.user, task, conn.get(), true);
            if (user.id == 0) {
                continue;
            }

            std::string body = apiclient_utils::build_api_ok_response_body(user);
            task.client->sendOkResponse(body);
        } else if (task.cmd == common::cmd_t::CHAT_CREATE) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }

            db::Chat chat = conn->createChat(task.request.chat.name, task.request.uid);
            if (chat.id == 0) {
                task.client->sendErrorResponse(409, common::ApiStatusCode::ERR_CONSTRAINT, "chat already exists");
                continue;
            }

            std::string body = apiclient_utils::build_api_ok_response_body(chat);
            task.client->sendOkResponse(body);
        } else if (task.cmd == common::cmd_t::CHAT_ADDUSER) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }

            db::User to_add = lookup_check_pass_by_name(task.request.chat.adduser, task, conn.get(), false);
            if (to_add.id == 0) {
                continue;
            }

            std::vector <db::Chat> chats = conn->lookupChatByName(task.request.chat.name);
            if (chats.size() != 1) {
                task.client->sendErrorResponse(404, common::ApiStatusCode::ERR_NOT_FOUND, "chat does not exist");
                continue;
            }

            conn->addUserToChat(chats[0], to_add);

            std::string body = apiclient_utils::build_api_ok_response_body(chats[0]);
            task.client->sendOkResponse("{\"status\": 0}");
        } else if (task.cmd == common::cmd_t::MESSAGE_SEND_CHAT) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }

            std::vector <db::Chat> chats = conn->lookupChatByName(task.request.chat.name);
            if (chats.size() != 1) {
                task.client->sendErrorResponse(404, common::ApiStatusCode::ERR_NOT_FOUND, "chat does not exist");
                continue;
            }

            db::Message msg(/*from*/user.id, /*to*/chats[0].id, task.request.message);
            msg.ts = time(NULL);
            conn->saveMessage(msg);

            task.client->sendOkResponse("{\"status\": 0}");
        } else if (task.cmd == common::cmd_t::GET_LOCATIONS) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }
            std::vector <apiclient_utils::Location> *locations;
            locations = location::get_locations();
            std::string body = apiclient_utils::build_api_ok_response_body(locations);
            task.client->sendOkResponse(body);
        } else if (task.cmd == common::cmd_t::CHAT_WITH_LOCATION) {
            db::User user = lookup_check_pass(task, conn.get());
            if (user.id == 0) {
                continue;
            }
            std::vector <apiclient_utils::User> *users;
            std::string country = "Italy";
            users = location::get_users(country);
            std::string body = apiclient_utils::build_api_ok_response_body(users);
            task.client->sendOkResponse(body);
        }
    }
}

boost::shared_ptr <AsyncHttpClient> DatabaseWorker::createConnect(bool use_ssl) {
    boost::shared_ptr <TcpClient> socket;
    if (use_ssl) {
        socket = boost::make_shared<TcpClient>(m_IoThread->ioService(), true, /*timeout*/ 10);
    } else {
        socket = boost::make_shared<TcpClient>(m_IoThread->ioService(), false, /*timeout*/ 10);
    }
    return boost::make_shared<AsyncHttpClient>(socket);
}

void DatabaseWorker::run() {

    std::cout << "we are in run\n";
    m_Http->asyncConnect(m_Host, m_Port,
                         boost::bind(&DatabaseWorker::onHttpConnect, this, boost::asio::placeholders::error));

    // TODO: timeouts :(
    std::cout << "we are after connect\n";
    return;
    m_Timeout.expires_from_now(std::chrono::milliseconds(5000));
    m_Timeout.async_wait([this](auto ec) {
        if (!ec) {
            m_TimedOut = true;
            this->m_Http->cancel();
        }
    });

    for (size_t i = 0; i < m_Workers; ++i) {
        m_Threads.push_back(std::thread(std::bind(&DatabaseWorker::processQueue, this)));
    }
}

void DatabaseWorker::onHttpConnect(const ConnectionError &e) {

    printf("we are in on http connect\n");
    if (e.code) {
        f::loge("http connect error [host: {0}:{1}, e: {2}] ", m_Host, m_Port, e.code.message());

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        m_Http->asyncConnect(m_Host, m_Port,
                             boost::bind(&DatabaseWorker::onHttpConnect, this, boost::asio::placeholders::error));
        return;
    }

    //m_Ui->showMsg("connected to server: " + m_Host + ":" + std::to_string(m_Port));

    if (!m_User.empty() && !m_Password.empty()) {
        std::string body = cli_utils::build_user_pass_body(m_User, m_Password);
        std::string req = cli_utils::build_request(common::cmd2string(common::cmd_t::USER_LOGIN),
                                                   "application/json", body);
        sendRequest(input::cmd_t::LOGIN, req);
    }
}

void DatabaseWorker::sendRequest(input::cmd_t cmd, const std::string &req) {
    printf("we are in send request\n");
    m_LastCmd = cmd;
    m_Http->asyncRequest(req, [this](const ConnectionError &error) {
        onHttpWrite(error);
    });
}

void DatabaseWorker::onHttpWrite(const ConnectionError &error) {
    printf("we are in on http write\n");
    if (error.code) {
        m_Timeout.cancel();
        loge("http request error: ", error.asString());
        return;
    }

    m_Http->asyncResponse(
            [this](const ConnectionError &error, const HttpReply &reply) {
                onHttpRead(error, reply);
            });
}

void DatabaseWorker::onHttpRead(const ConnectionError &error, const HttpReply &reply) {
    printf("we are in on http read\n");
    m_Timeout.cancel();

    if (error.code) {
        f::loge("http read [e: {0}]", error.asString());
        m_Http = createConnect(m_Ssl);
        m_Http->asyncConnect(m_Host, m_Port,
                             boost::bind(&DatabaseWorker::onHttpConnect, this, boost::asio::placeholders::error));
        return;
    }

    if (reply._status != 200) {
        //->showMsg(std::to_string(reply._status));
        //m_Ui->showMsg(reply._body);
        return;
    }

    logd2("reply: ", reply._body);
    //m_Ui->showMsg("200 OK");

    if (m_LastCmd == input::cmd_t::LOGIN) {

    } else if (m_LastCmd == input::cmd_t::DIRECT_MSG_USER) {
    } else if (m_LastCmd == input::cmd_t::HISTORY_USER) {

    } else if (m_LastCmd == input::cmd_t::STATUS_USER) {
    }
}

void DatabaseWorker::join() {
    for (auto &thread : m_Threads) {
        thread.join();
    }
}

