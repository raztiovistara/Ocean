#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include "file_modified_time.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

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
    // Send remaining bytes
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

void checktime(const std::string& my_time, const std::string& body, tcp::socket& socket) {
    std::string file_to_transfer = "G://path_to_file//file_to_transfer.txt"; // Change to your file path

    if (my_time < body) {
        // Transfer file from this side
        std::cout << "Transferring file from this side.\n";
        send_file(socket, file_to_transfer);
    } else {
        // Receive file from that side
        std::cout << "Receiving file from that side.\n";
        receive_file(socket, "received_file.txt"); // Change to your desired received file path
    }
}

int main() {
    std::string time = getFileCreationTime("G://watcher//watched_folder//hidden_folder//The_Hidden_Bedrock_of_Ramen.wld");
    try {
        net::io_context ioc;
        tcp::acceptor acceptor{ioc, {tcp::v4(), 3000}};

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            std::string response_body = "";

            if (req.target() == "/status") {
                response_body = "Server is running";
            } else if (req.target() == "/checktime") {
                checktime(time, req.body(), socket);
                response_body = "Time checked and action taken.";
            } else {
                response_body = "Invalid request.";
            }

            http::response<http::string_body> res{http::status::ok, req.version()};
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
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}
