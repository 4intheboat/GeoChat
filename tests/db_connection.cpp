#include "catch.hpp"
#include "../server/src/database.hpp"

bool usersEqual(const db::User& firstUser, const db::User& secondUser) {
    return firstUser.id == secondUser.id &&
           firstUser.self_chat_id == secondUser.self_chat_id &&
           firstUser.name == secondUser.name &&
           firstUser.password == secondUser.password &&
           firstUser.stpath == secondUser.stpath;
}

bool chatsEqual(const db::Chat& firstChat, const db::Chat& secondChat) {
    return firstChat.id == secondChat.id &&
           firstChat.name == secondChat.name;
}

bool messagesEqual(const db::Message& firstMessage, const db::Message& secondMessage) {
    return firstMessage.user_from == secondMessage.user_from &&
           firstMessage.chat_to == secondMessage.chat_to &&
           firstMessage.message == secondMessage.message &&
           firstMessage.ts == secondMessage.ts &&
           firstMessage.flags == secondMessage.flags;
}

TEST_CASE("Users are created", "[createUser]") {
    AbstractConnection* conn = new InMemoryConnection();

    SECTION("Creating two users") {
        db::User createdUser1 = conn->createUser("John", "q1w2e3W!098", "");
        db::User expectedUser1(1, 1, "John", "q1w2e3W!098", "");
        db::User createdUser2 = conn->createUser("Kitty", "asqwerU!098", "");
        db::User expectedUser2(2, 2, "Kitty", "asqwerU!098", "");

        REQUIRE(usersEqual(createdUser1, expectedUser1));
        REQUIRE(usersEqual(createdUser2, expectedUser2));
    }

    SECTION("Creating two users with the same names") {
        db::User createdUser1 = conn->createUser("John", "q1w2e3W!098", "");
        db::User createdUser2 = conn->createUser("John", "asqwerU!098", "");

        REQUIRE(usersEqual(createdUser2, db::User()));
    }
}

TEST_CASE("User heartbit is updated", "[updateUserHeartBit]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user = conn->createUser("John", "q1w2e3W!098", "");
    conn->updateUserHeartBit(user, 2);

    REQUIRE(user.heartbit == 2);
}

TEST_CASE("Chats are created", "[createChat]") {
    AbstractConnection* conn = new InMemoryConnection();

    SECTION("Creating two chats") {
        db::Chat createdChat1 = conn->createChat("Chat1", 0);
        db::Chat expectedChat1(1, "Chat1");
        db::Chat createdChat2 = conn->createChat("Chat2", 0);
        db::Chat expectedChat2(2, "Chat2");

        REQUIRE(chatsEqual(createdChat1, expectedChat1));
        REQUIRE(chatsEqual(createdChat2, expectedChat2));
    }

    SECTION("Creating two chats with the same names") {
        db::Chat createdChat1 = conn->createChat("Chat1", 0);
        db::Chat createdChat2 = conn->createChat("Chat1", 0);

        REQUIRE(chatsEqual(createdChat2, db::Chat()));
    }
}

TEST_CASE("User is looked up by name", "[lookupUserByName]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user = conn->createUser("John", "q1w2e3W!098", "");
    std::vector<db::User> users = conn->lookupUserByName("John");

    REQUIRE(usersEqual(user, users[0]));
}

TEST_CASE("User is looked up by id", "[lookupUserById]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user = conn->createUser("John", "q1w2e3W!098", "");
    db::User foundUser = conn->lookupUserById(1);

    REQUIRE(usersEqual(user, foundUser));
}

TEST_CASE("Chats are looked up for user id", "[lookupChatsForUserId]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user = conn->createUser("John", "q1w2e3W!098", "");
    db::Chat chat = conn->createChat("Chat1", 1);
    std::vector<db::Chat> chats = conn->lookupChatsForUserId(1);

    REQUIRE(chatsEqual(chat, chats[0]));
}

TEST_CASE("Chat is looked up by name", "[lookupChatByName]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::Chat chat = conn->createChat("Chat1", 0);
    std::vector<db::Chat> chats = conn->lookupChatByName("Chat1");

    REQUIRE(chatsEqual(chat, chats[0]));
}

TEST_CASE("Chat is looked up by id", "[lookupChatById]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::Chat chat = conn->createChat("Chat1", 0);
    db::Chat foundChat = conn->lookupChatById(1);

    REQUIRE(chatsEqual(chat, foundChat));
}

TEST_CASE("Users are looked up for chat id", "[lookupUsersForChatId]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user = conn->createUser("John", "q1w2e3W!098", "");
    db::Chat chat = conn->createChat("Chat1", 1);
    std::vector<db::User> users = conn->lookupUsersForChatId(1);

    REQUIRE(usersEqual(user, users[0]));
}

TEST_CASE("User is added to chat", "[addUserToChat]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user = conn->createUser("John", "q1w2e3W!098", "");
    db::Chat chat = conn->createChat("Chat1", 0);
    conn->addUserToChat(chat, user);
    std::vector<db::User> users = conn->lookupUsersForChatId(1);

    REQUIRE((users.size() == 1 && usersEqual(user, users[0])) == true);
}

TEST_CASE("Messages are added and got", "[getMessages]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user = conn->createUser("John", "q1w2e3W!098", "");
    db::Chat chat = conn->createChat("Chat1", 0);
    conn->addUserToChat(chat, user);
    db::Message message(1, 1, "Hello!");
    conn->saveMessage(message);
    db::get_msg_opt_t opts;
    std::vector<db::Message> messages = conn->getMessages(1, opts);

    REQUIRE((messages.size() == 1 && messagesEqual(message, messages[0])) == true);
}

TEST_CASE("Messages are selected correctly", "[selectMessages]") {
    AbstractConnection* conn = new InMemoryConnection();
    db::User user1 = conn->createUser("User1", "q1w2e3W!098", "");
    db::Chat chat1 = conn->createChat("Chat1", 0);
    conn->addUserToChat(chat1, user1);
    db::Message message1(1, 1, "Hello!");
    conn->saveMessage(message1);

    db::User user2 = conn->createUser("User2", "q1w2e3W!097", "");
    db::Chat chat2 = conn->createChat("Chat2", 0);
    conn->addUserToChat(chat2, user2);
    db::Message message2(2, 2, "Hi!");
    conn->saveMessage(message2);

    db::Message message3(1, 1, "World!");
    conn->saveMessage(message3);

    db::get_msg_opt_t opts;
    std::vector<db::Message> messages = conn->selectMessages([](const db::Message& msg) {
        return !msg.message.empty() && msg.message[0] == 'H'; }, opts);

    REQUIRE((messages.size() == 2 && messagesEqual(message1, messages[0]) && messagesEqual(message2, messages[1])) == true);
}
