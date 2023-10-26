#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <asio.hpp>

#include "server.h"

Server::Server(const int port_num) : acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_num)) {
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
            // Read the client request
            asio::streambuf requestBuffer;
            asio::read_until(socket, requestBuffer, '\0');  // Change delimiter to '\0'

            std::string request = asio::buffer_cast<const char*>(requestBuffer.data());

            requestBuffer.consume(requestBuffer.size());
            std::cout << request << '\n';
            socket.send(asio::buffer("fuck"));

            // Split the request by ':' to separate the action, username, and password.
            size_t firstColonPos = request.find(':');
            if (firstColonPos == std::string::npos) {
                std::cerr << "Invalid request format." << std::endl;
                continue;
            }

            std::string action = request.substr(0, firstColonPos);
            std::string remainingData = request.substr(firstColonPos + 1);

            size_t secondColonPos = remainingData.find(':');
            if (secondColonPos == std::string::npos) {
                std::cerr << "Invalid request format." << std::endl;
                continue;
            }

            std::string username = remainingData.substr(0, secondColonPos);
            std::string password = remainingData.substr(secondColonPos + 1);

            // If "REGISTER" or "LOGIN" keyword is included in the request, remove it from the username.
            if (username.find("REGISTER") == 0) {
                username = username.substr(8);
            } else if (username.find("LOGIN") == 0) {
                username = username.substr(5);
            }

            std::string response;

            if (action == "REGISTER") {
                // Registration
                if (registerUser(username, password) && createUserDirectory(username)) {
                    response = "Registration successful for " + username;
                    std::cout << "Registration successful for " << username << std::endl;
                } else {
                    response = "Registration failed because the user already exists";
                    std::cerr << "Registration failed for " << username << std::endl;
                }
            } else if (action == "LOGIN") {
                // Login
                if (authenticateUser(username, password)) {
                    response = "Authentication successful for " + username;
                    std::cout << "Authentication successful for " << username << std::endl;
                } else {
                    response = "Authentication failed for " + username;
                    std::cerr << "Authentication failed for " << username << std::endl;
                }
            } else {
                response = "Invalid request type.";
                std::cerr << "Invalid request type." << std::endl;
            }

            // Send the response back to the client.
            response += '\0';  
            asio::write(socket, asio::buffer(response));
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
    std::ifstream userFile("users.txt");
    std::string line;

    while (std::getline(userFile, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string storedUsername = line.substr(0, pos);
            if (storedUsername == username) {
                return false; // User already exists
            }
        }
    }

    std::ofstream userFileOut("users.txt", std::ios_base::app);
    if (!userFileOut.is_open()) {
        std::cerr << "Error opening user file for writing." << std::endl;
        return false;
    }
    userFileOut << username << ':' << password << std::endl;
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
    Server server(8080);
    server.start();
}
