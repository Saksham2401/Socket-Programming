#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <asio.hpp>

class Server {
public:
    Server(const int port_num);
    void start();

private:
    asio::io_context ioContext;
    asio::ip::tcp::acceptor acceptor;

    bool createUserDirectory(const std::string& username);
    bool registerUser(const std::string& username, const std::string& password);
    bool authenticateUser(const std::string& username, const std::string& password);
};

#endif
