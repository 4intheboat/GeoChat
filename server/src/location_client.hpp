#include <boost/asio.hpp>
#include "rapidjson/document.h"


using boost::asio::ip::tcp;

#ifndef GEOCHAT_LOCATION_IO_SERVICE_LOCATIONCLIENT_HPP
#define GEOCHAT_LOCATION_IO_SERVICE_LOCATIONCLIENT_HPP


#define api_key_ "2787668b805fa7cb7e6109a1b21299da3fded7e7"
#define api_host_ "dadata.ru"
#define api_port_ "80"

class LocationClient {
public:
    explicit LocationClient();

    const std::string get_city_by_ip(const std::string &ip);

private:
    std::unique_ptr<boost::asio::io_service> _io_service;
    std::unique_ptr<tcp::socket> _socket;
    std::unique_ptr<tcp::resolver> _resolver;

    const std::string _api_key = api_key_;
    const std::string _host = api_host_;
    const std::string _port = api_port_;

    static void parse_response(rapidjson::Document &json_document, const std::string &response);

    static std::string get_string_value(rapidjson::Document &json_document, const std::string &key);

    static void
    get_document_value(rapidjson::Document &json_document, rapidjson::Document &new_json_document, std::string key);

    void connect(const std::string &host, const std::string &port);

    static void make_request(boost::asio::streambuf &request, const std::string &ip, const std::string &api_key);

    void connect_to_api();

    std::string read();

    void send(boost::asio::streambuf &message);

};


#endif //GEOCHAT_LOCATION_IO_SERVICE_LOCATIONCLIENT_HPP