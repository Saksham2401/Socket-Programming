#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <boost/asio.hpp>

class Server {
public:
    Server();
    void start();

private:
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::acceptor acceptor;

    bool createUserDirectory(const std::string& username);
    bool registerUser(const std::string& username, const std::string& password);
    bool authenticateUser(const std::string& username, const std::string& password);
};

#endif
