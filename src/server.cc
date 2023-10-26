#include <iostream>
#include <string>
#include <fstream>
#include <filesystem.hpp>
#include <asio.hpp>

#include "server.h"

Server::Server(const int port_num) : acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_num)) {
    std::filesystem::create_directory("user_directories");
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

            std::istringstream iss(request);

            std::string action;
            iss >> action;

            iss.ignore(); // ignore space

            std::string username;
            std::getline(iss, username, ':');

            iss.ignore(); // ignore colion

            std::string password;
            iss >> password;

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
    asio::error_code ec;
    std::filesystem::path::userDirectory = "user_directories/" + username;

    // Check if the parent directory exists, and create it if not.
    std::filesystem::path::parentDirectory = userDirectory.parent_path();
    if (!std::filesystem::exists(parentDirectory)) {
        std::filesystem::create_directories(parentDirectory, ec);
        if (ec) {
            std::cerr << "Error creating parent directory: " << ec.message() << std::endl;
            return false;
        }
    }

    // Create the user's directory.
    std::filesystem::create_directory(userDirectory, ec);
    if (ec) {
        std::cerr << "Error creating directory for user " << username << ": " << ec.message() << std::endl;
        return false;
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
