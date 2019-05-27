#include <iostream>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "boost/array.hpp"
#include "location_client.hpp"
#include "o2logger/src/o2logger.hpp"


LocationClient::LocationClient() {
    _io_service = std::make_unique<boost::asio::io_service>();
    _socket = std::make_unique<tcp::socket>(*_io_service);
    _resolver = std::make_unique<tcp::resolver>(*_io_service);
}

void LocationClient::parse_response(rapidjson::Document &json_document, const std::string &response) {
    size_t start = response.find("{\"", 0);
    std::string raw_json = response.substr(start, response.length() - start);
    json_document.Parse(raw_json.c_str());
}

std::string LocationClient::get_string_value(rapidjson::Document &json_document, const std::string &key) {
    if (!json_document.HasMember(key.c_str())) {
        o2logger::loge("Json document do not have member ");
        return std::string();
    }

    if (!json_document[key.c_str()].IsString()) {
        o2logger::loge("Json value is not a string");
        return std::string();
    }


    return json_document[key.c_str()].GetString();
}

void LocationClient::make_request(boost::asio::streambuf &request, const std::string &ip,
                                  const std::string &api_key) {
    std::ostream request_stream(&request);
    request_stream << "GET " << "/suggestions/api/4_1/rs/iplocate/address?ip=" << ip << " HTTP/1.0\r\n";
    request_stream << "Host: " << "suggestions.dadata.ru" << "\r\n";
    request_stream << "Authorization: Token " << api_key << "\r\n";
    request_stream << "Accept: application/json" << "\r\n";
    request_stream << "Connection: keep-alive\r\n\r\n";

}

void LocationClient::connect(const std::string &host, const std::string &port) {
    boost::system::error_code ec;
    tcp::resolver::query query(host, port);
    auto endpoints = _resolver->resolve(query, ec);
    if (ec) {
	std::cout << ec.message() << std::endl;
        o2logger::loge(ec.message());
    } else {
        boost::asio::connect((*_socket), endpoints);
    }
}

void LocationClient::connect_to_api() {
    connect(_host, _port);

}

void LocationClient::get_document_value(rapidjson::Document &json_document, rapidjson::Document &new_json_document,
                                        std::string key) {
    using namespace rapidjson;

    if (!json_document.HasMember(key.c_str())) {
        o2logger::loge("Json document do not have member." + key);
        return;
    }

    if (!json_document[key.c_str()].IsObject()) {
        o2logger::loge("Json value can not be transform to subquery.");
        return;
    }
    StringBuffer buffer;
    Writer <rapidjson::StringBuffer> writer(buffer);
    json_document[key.c_str()].Accept(writer);


    new_json_document.Parse(buffer.GetString());


}

const std::string LocationClient::get_city_by_ip(const std::string &ip) {
    this->connect_to_api();
    boost::asio::streambuf request;
    LocationClient::make_request(request, ip, _api_key);
    send(request);
    std::string response = read();
    std::string result = "";

    if (response.size() > 0) {
        rapidjson::Document json_document;
        LocationClient::parse_response(json_document, response);
        LocationClient::get_document_value(json_document, json_document, "location");
        LocationClient::get_document_value(json_document, json_document, "data");
        result = LocationClient::get_string_value(json_document, "city");
    }
    return result;
}

std::string LocationClient::read() {
    std::string result;
    for (;;) {
        boost::array<char, 128> buf{};
        boost::system::error_code ec;
        size_t len = _socket->read_some(boost::asio::buffer(buf), ec);
        if (ec == boost::asio::error::eof) {
            break;
        } else if (ec) {
            o2logger::loge(ec.value());
        }
        result.append(buf.data(), len);
    }
    return result;
}

void LocationClient::send(boost::asio::streambuf &message) {
    boost::system::error_code ignored_error;
    boost::asio::write(*_socket, message, ignored_error);

}
