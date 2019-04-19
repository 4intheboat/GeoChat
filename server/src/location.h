#pragma once

#include <vector>

#include "common/common.hpp"
#include "apiclient_utils.hpp"


namespace location {
    std::vector <apiclient_utils::Location> *get_locations();

    std::vector <apiclient_utils::User> *get_users(std::string country);
}   // namespace location
