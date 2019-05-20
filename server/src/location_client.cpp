//
// Created by artbakulev on 18.05.19.
//

#include "location_client.hpp"
#include <boost/array.hpp>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

LocationClient::LocationClient(boost::asio::io_service &io_service) : io_service_(io_service)  {
    socket_ = new tcp::socket(io_service_);
    resolver_ = new tcp::resolver(io_service_);
}

void LocationClient::connect(std::string host, std::string port) {
    boost::system::error_code ec;
    tcp::resolver::query query(host, port);
    auto endpoints = resolver_->resolve(query, ec);
    boost::asio::connect((*socket_), endpoints);
}

std::string LocationClient::read() {
    std::string result;
    for (;;) {
        boost::array<char, 128> buf{};
        boost::system::error_code ec;
        size_t len = socket_->read_some(boost::asio::buffer(buf), ec);
        if (ec == boost::asio::error::eof) {
            break;
        } else if (ec) {
            std::cerr << ec.value() << std::endl;
        }
        result.append(buf.data(), len);
    }
    return result;
}

void LocationClient::send(std::string message) {
    boost::system::error_code ignored_error;
    boost::asio::write(*socket_, boost::asio::buffer(message), ignored_error);
}

void LocationClient::send(boost::asio::streambuf &message) {
    boost::system::error_code ignored_error;
    boost::asio::write(*socket_, message, ignored_error);
}


std::string LocationClient::get_city_by_ip(const std::string &ip) {

    std::string api_key;
    LocationClient::get_api_key(api_key);

    boost::asio::streambuf request;
    LocationClient::make_request_for_get_city_by_ip(request, ip, api_key);

    send(request);

    std::string response = read();

    rapidjson::Document json_document;
    LocationClient::parse_response(json_document, response);
    LocationClient::get_document_value(json_document, json_document, "location");
    LocationClient::get_document_value(json_document, json_document, "data");

    std::string result;

    result = LocationClient::get_string_value(json_document, "city");

    return result;
}

void LocationClient::make_request_for_get_city_by_ip(boost::asio::streambuf &request, const std::string &ip,
                                                     std::string &api_key) {
    std::ostream request_stream(&request);
    request_stream << "GET " << "/suggestions/api/4_1/rs/iplocate/address?ip=" << ip << " HTTP/1.0\r\n";
    request_stream << "Host: " << "suggestions.dadata.ru" << "\r\n";
    request_stream << "Authorization: Token " << api_key << "\r\n";
    request_stream << "Accept: application/json" << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
}


void LocationClient::get_api_key(std::string &api_key) {
    std::ifstream api_key_file("api_key.txt");
    if (!api_key_file) {
        std::cerr << "File does not exist." << std::endl;
        return;
    }
    api_key_file >> api_key;
    api_key_file.close();
}


void LocationClient::parse_response(rapidjson::Document &json_document, std::string &response) {
    size_t start = response.find("{\"", 0);
    std::string raw_json = response.substr(start, response.length() - start);
    json_document.Parse(raw_json.c_str());
}

void LocationClient::connect_to_api() {
    connect("dadata.ru", "80");
}

std::string LocationClient::get_string_value(rapidjson::Document &json_document, std::string key) {
    if (!json_document.HasMember(key.c_str())) {
        std::cerr << "Json document do not have member " << key << std::endl;
        return std::string();
    }

    if (!json_document[key.c_str()].IsString()) {
        std::cerr << "Json value is not a string" << std::endl;
        return std::string();
    }


    return json_document[key.c_str()].GetString();

}

void
LocationClient::get_document_value(rapidjson::Document &json_document, rapidjson::Document &new_json_document,
                                   std::string key) {
    using namespace rapidjson;

    if (!json_document.HasMember(key.c_str())) {
        std::cerr << "Json document do not have member " << key << std::endl;
        return;
    }

    if (!json_document[key.c_str()].IsObject()) {
        std::cerr << "Json value can not be transform to subquery" << std::endl;
        return;
    }
    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    json_document[key.c_str()].Accept(writer);


    new_json_document.Parse(buffer.GetString());

}

void LocationClient::output_json(rapidjson::Document &json_document) {
    using namespace rapidjson;
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    json_document.Accept(writer);
    std::cout << buffer.GetString() << std::endl;
}






