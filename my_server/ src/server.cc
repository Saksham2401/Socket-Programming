#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../include/src/server.h" // Adjust the path as needed

void startServer(int port) {
    int serverSocket;
    struct sockaddr_in serverAddr;
    
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error in socket");
        return;
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error in bind");
        close(serverSocket);
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Error in listen");
        close(serverSocket);
        return;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    while (true) {
        int clientSocket;
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Accept a connection from a client
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Error in accept");
            continue;
        }

        std::cout << "Client connected from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // Handle the client connection (You can add your server logic here)

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket (won't be reached in this simple example)
    close(serverSocket);
}

int main() {
    int port = 8080; // Change this to the desired port
    startServer(port);
    return 0;
}
