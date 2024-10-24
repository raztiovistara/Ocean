#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include "file_modified_time.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

// std::tm parse_time(const std::string& time_str) {
//     std::tm tm = {};
//     std::istringstream ss(time_str);
//     ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S"); 
//     return tm;
// }
void checktime(char* my_time,const std::string& body){
  
}
int main() {
    char* time= getFileCreationTime("G://ToDo.txt");
    try {
        net::io_context ioc;
        tcp::acceptor acceptor{ ioc, {tcp::v4(), 3000} };

        for (;;) {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            std::string response_body = "OK";

            if (req.target() == "/status") {//strcmp("/Opera", target) == 0
                response_body = "Server is running";
            }
            if(req.target() == "/checktime"){
                checktime(time, req.body());
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
