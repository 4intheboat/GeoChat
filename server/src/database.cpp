#include "database.hpp"
#include <iostream>
#include "common/utils.hpp"


std::ostream& operator<<(std::ostream &os, const db::Message &msg)
{
    return os << "user_from: " << msg.user_from << ", chat_to: " << msg.chat_to << ", msg: " << msg.message << "\n";
}


