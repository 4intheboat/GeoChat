//
// Created by artbakulev on 18.04.19.
//
#include "common/utils.hpp"
#include "apiclient_utils.hpp"

namespace location {
    std::vector <apiclient_utils::Location> *get_locations() {
        auto *locations = new std::vector<apiclient_utils::Location>;
        locations->push_back(apiclient_utils::Location("Spain"));
        locations->push_back(apiclient_utils::Location("Russia"));
        locations->push_back(apiclient_utils::Location("Portugal"));
        locations->push_back(apiclient_utils::Location("Germany"));
        locations->push_back(apiclient_utils::Location("Czech Republic"));
        locations->push_back(apiclient_utils::Location("Italy"));
        return locations;
    }

    std::vector <apiclient_utils::User> *get_users(std::string country) {
        auto *users = new std::vector<apiclient_utils::User>;
        if (country == "Italy") {
            users->push_back(apiclient_utils::User("Artyom", "Italy"));
            users->push_back(apiclient_utils::User("Misha", "Italy"));
            users->push_back(apiclient_utils::User("Nastya", "Italy"));
            users->push_back(apiclient_utils::User("Liza", "Italy"));
            users->push_back(apiclient_utils::User("Masha", "Italy"));
            users->push_back(apiclient_utils::User("Asya", "Italy"));
        }
        return users;
    }
}