#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <asio.hpp>

#include "server.h"

Server::Server() : acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080)) {
    // Constructor code for initialization
}

void Server::start() {
    while (true) {
        asio::error_code ec;
        asio::ip::tcp::socket socket(ioContext);
        
        acceptor.accept(socket, ec);
        if (ec) {
            std::cerr << "Error accepting client: " << ec.message() << std::endl;
            continue;
        }

        try {
            // Read the client request (register or login)
            asio::streambuf requestBuffer;
            asio::read_until(socket, requestBuffer, "\n");

            std::string request = asio::buffer_cast<const char*>(requestBuffer.data());

            requestBuffer.consume(requestBuffer.size());

            size_t colonPos = request.find(':');
            if (colonPos == std::string::npos) {
                std::cerr << "Invalid request format." << std::endl;
                continue;
            }

            std::string username = request.substr(0, colonPos);
            std::string password = request.substr(colonPos + 1);
            
            if (request.find("REGISTER") == 0) {
                // Registration
                if (registerUser(username, password) && createUserDirectory(username)) {
                    // Registration successful
                    std::cout << "Registration successful for " << username << std::endl;
                } else {
                    // Registration failed
                    std::cerr << "Registration failed for " << username << std::endl;
                }
            } else if (request.find("LOGIN") == 0) {
                // Login
                if (authenticateUser(username, password)) {
                    // Authentication successful
                    std::cout << "Authentication successful for " << username << std::endl;
                    // Grant access or perform other actions here
                } else {
                    // Authentication failed
                    std::cerr << "Authentication failed for " << username << std::endl;
                }
            } else {
                std::cerr << "Invalid request type." << std::endl;
            }
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        socket.close();
    }
}

bool Server::createUserDirectory(const std::string& username) {
    boost::system::error_code ec;
    boost::filesystem::path userDirectory = "user_directories/" + username;

    if (!boost::filesystem::exists(userDirectory)) {
        if (!boost::filesystem::create_directory(userDirectory, ec)) {
            std::cerr << "Error creating directory for user " << username << ": " << ec.message() << std::endl;
            return false;
        }
    }
    return true;
}

bool Server::registerUser(const std::string& username, const std::string& password) {
    std::ofstream userFile("users.txt", std::ios_base::app);
    if (!userFile.is_open()) {
        std::cerr << "Error opening user file for writing." << std::endl;
        return false;
    }
    userFile << username << ':' << password << std::endl;
    return true;
}

bool Server::authenticateUser(const std::string& username, const std::string& password) {
    std::ifstream userFile("users.txt");
    if (!userFile.is_open()) {
        std::cerr << "Error opening user file for reading." << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(userFile, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string storedUsername = line.substr(0, pos);
            std::string storedPassword = line.substr(pos + 1);
            if (storedUsername == username && storedPassword == password) {
                return true;
            }
        }
    }
    return false;
}

int main() {
}
