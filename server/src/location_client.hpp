//
// Created by artbakulev on 18.05.19.
//
#include <boost/asio.hpp>
#include "rapidjson/document.h"
#include <iostream>
#include <fstream>

using boost::asio::ip::tcp;

#ifndef GEOCHAT_LOCATION_CLIENT_HPP
#define GEOCHAT_LOCATION_CLIENT_HPP


class LocationClient {
public:
    explicit LocationClient(boost::asio::io_service &io_service_);

    void connect_to_api();

    void connect(std::string host, std::string port);

    std::string read();

    void send(std::string message);

    void send(boost::asio::streambuf &message);

    std::string get_city_by_ip(const std::string &ip);

    static void make_request_for_get_city_by_ip(boost::asio::streambuf &request, const std::string &ip,
                                                std::string &api_key); // TODO: Сделай это уже реализованными средствами

    static void get_api_key(std::string &api_key);

    static void output_json(rapidjson::Document &json_document);

    static void parse_response(rapidjson::Document &json_document, std::string &response);

    static std::string get_string_value(rapidjson::Document &json_document, std::string key);

    static void
    get_document_value(rapidjson::Document &json_document, rapidjson::Document &new_json_document, std::string key);

private:
    boost::asio::io_service &io_service_;
    tcp::socket *socket_;
    tcp::resolver *resolver_;

};




#endif //GEOCHAT_LOCATIONCLIENT_HPP
