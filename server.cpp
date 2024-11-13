#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include "file_modified_time.h"

#define IP "192.168.1.4"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void send_file(tcp::socket& socket, const std::string& filepath);
void receive_file(tcp::socket& socket, const std::string& filepath);

void run_server(std::string time,std::string filepath)
{
    try { 
        net::io_context ioc;
        tcp::acceptor acceptor{ ioc, {tcp::v4(), 3000} };

        for (;;) {
            int c = 0;
            tcp::socket socket{ ioc };
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            std::string response_body = "";

            if (req.target() == "/status") {
                response_body = "Server is running";
            }
            else if(req.target() == "/checktime"){
                if(time < req.body())
                {
                    response_body = "Reciveing";
                    c = 1;
                }
                else if(time > req.body())
                {
                    response_body = "Sending";
                    c = 2;
                }
            }
            else
            {
                response_body = "Invalid";
            }
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

void send_file(tcp::socket& socket, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open the file: " << filepath << std::endl;
        return;
    }

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        net::write(socket, net::buffer(buffer, file.gcount()));
    }
    net::write(socket, net::buffer(buffer, file.gcount()));
    std::cout << "File sent: " << filepath << std::endl;
}

void receive_file(tcp::socket& socket, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    char buffer[1024];
    size_t bytes_received;

    while ((bytes_received = socket.read_some(net::buffer(buffer))) > 0) {
        file.write(buffer, bytes_received);
    }
    std::cout << "File received: " << filepath << std::endl;
}

int main() {
    std::string time= getFileCreationTime("G://watcher//watched_folder//hidden_folder//The_Hidden_Bedrock_of_Ramen.wld");
    run_server(time, "G://watcher//watched_folder//hidden_folder//The_Hidden_Bedrock_of_Ramen.wld");  
}
