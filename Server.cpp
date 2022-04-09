/*
 * Server
 * Author: Avital Aviv 316125855 and Noa Benita 209281344
 */
#include "Server.h"
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>

Server::Server(int port)throw (const char*) {
    // first open socket
    int protocol = 0;
    fileDescriptor = socket(AF_INET, SOCK_STREAM, protocol);
    // check if the connection succeed
    if (fileDescriptor < 0){
        throw "socket failed";
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    // define port
    server.sin_port = htons(port);
    int len = sizeof server;
    //binding the port to the socket
    int checkBind = bind(fileDescriptor, (struct sockaddr *) &server, len);
    if (checkBind < 0){
        throw "bind failure";
    }
    int checkListening = listen(fileDescriptor, 3);
    if (checkListening < 0){
        throw "listen failure";
    }
}

/////how to open new function to create new thread
void Server::start(ClientHandler& ch)throw(const char*){
    this->makeConnection = true;
    t = new thread([&ch, this](){
        while (makeConnection){
            socklen_t sizeOfClient = sizeof(client);
            int aClient = accept(fileDescriptor,(struct sockaddr *) &client, &sizeOfClient);
            if (aClient < 0){
                throw "connection error";
            }
            ch.handle(aClient);
            close(aClient);

        }

    });
}

void Server::stop(){
    this->makeConnection = false;
    t->join(); // do not delete this!

    close(fileDescriptor);

}

Server::~Server() {
}

