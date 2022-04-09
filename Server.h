/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_
#include <thread>
#include "commands.h"
#include "CLI.h"
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
public:
    virtual void handle(int clientID)=0;
};

// you can add helper classes here and implement on the cpp file

class dioForSocket : public DefaultIO {
    int socketFileDescriptor;
public:
   //constructor
   dioForSocket(int fileD) {
       this->socketFileDescriptor = fileD;
   }

   void write (float f) override {
       ostringstream ss;
       ss <<f;
       string s(ss.str());
       write(s);
//       int flag = 0;
//       // make space for len + 1
//       char textChar[sizeof(f) + 1];
//       size_t sizeOfText = sizeof(textChar);
//       sprintf(textChar, "%f", f);
//       send(this->socketFileDescriptor, textChar, sizeOfText, flag);
   }

   void write (string text) override {
       int flag = 0;
       const char* textChar = text.c_str();
       send(this->socketFileDescriptor, textChar, text.size(), flag);
   }

   string read() override{
       int flag = 0;
       string inputToServer = "";
       char end = '0';
       recv(this->socketFileDescriptor, &end, sizeof(char), flag);
       while (end != '\n') {
           inputToServer = inputToServer + end;
           recv(this->socketFileDescriptor, &end, sizeof(char), flag);

       }
       return inputToServer;
   }

   void read (float *f) override {
       int flag = 0;
       recv(this->socketFileDescriptor, f, sizeof(*f), flag);
   }
};


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler{
public:
    virtual void handle(int clientID){
        dioForSocket dio(clientID);
        CLI c (&dio);;
        c.start();
    }
};


// implement on Server.cpp
class Server {
    thread* t; // the thread to run the start() method in
    // you may add data members

public:
    bool makeConnection;
    int fileDescriptor;
    struct sockaddr_in server;
    struct sockaddr_in client;
    Server(int port) throw (const char*);
    virtual ~Server();
    void start(ClientHandler& ch) throw(const char*);
    void stop();
};

#endif /* SERVER_H_ */
