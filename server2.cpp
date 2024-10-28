#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include "file_modified_time.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void send_request(std::string t){
    net::io_context ioc;
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve("192.168.1.9", "3000"); 
    net::connect(stream.socket(), results.begin(), results.end());

    http::request<http::string_body> req{http::verb::get, "/checktime", 11};
    req.body() = t;
    req.prepare_payload();
    req.set(http::field::host, "192.168.1.9"); 
    req.set(http::field::user_agent, "Recall");

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    std::cout << res << std::endl;
}
void start_server(){
    try {
        net::io_context ioc;
        tcp::acceptor acceptor{ ioc, {tcp::v4(), 3001} };

        for (;;) {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            std::string response_body = "OK";

            if (req.target() == "/status") {
                response_body = "Server is running";
            }

            http::response<http::string_body> res{ http::status::ok, req.version() };
            res.set(http::field::server, "Boost.Beast");
            res.set(http::field::content_type, "text/plain");
            res.keep_alive(req.keep_alive());
            res.body() = response_body;
            res.prepare_payload();

            http::write(socket, res);

            std::cout << "Request target: " << req.target() << std::endl;
            beast::error_code ec;
            socket.shutdown(tcp::socket::shutdown_send, ec);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
int main() {
    std::string t=getFileCreationTime("G://watcher//watched_folder//The_Hidden_Bedrock_of_Ramen.wld");
    send_request(t);
    //start_server();
    return 0;
}
