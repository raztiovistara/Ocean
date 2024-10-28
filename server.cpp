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

void run_server(std::string time)
{
    try { 
        net::io_context ioc;
        tcp::acceptor acceptor{ ioc, {tcp::v4(), 3000} };

        for (;;) {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            std::string response_body = "";

            if (req.target() == "/status") {
                response_body = "Server is running";
            }
            if(req.target() == "/checktime"){
                if(my_time < req.body())
                {
                    //transfer file from this side
                }
                else
                {
                    //recive file from that side
                }
            }
            response_body = "OK";
            http::response<http::string_body> res{ http::status::ok, req.version() };
            res.set(http::field::server, "MIni Server");
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
    std::string time= getFileCreationTime("G://watcher//watched_folder//hidden_folder//The_Hidden_Bedrock_of_Ramen.wld");
    run_server(time);  
}
