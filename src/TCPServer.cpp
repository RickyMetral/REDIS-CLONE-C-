#include "TCPConnection.hpp"
#include "TCPServer.hpp"


TCPServer::TCPServer(const char* serverPort, int sock_family, bool block) : TCPConnection(sock_family, AI_PASSIVE, block){
    struct sigaction sa;
    if(this->initSocket(NULL, serverPort) <= -1){
        std::cerr << "InitSocket Failed\n";
        exit(1);
    }
    this->queueConns();
    this->reapDeadProcesses(sa);
    printf("server: waiting for connections...\n");
}

TCPServer::~TCPServer(){
    //TODO: Make sure to free whatever is needed in the future
    std::cout << "Nothing to free for now" << std::endl;
}

void TCPServer::reapDeadProcesses(struct sigaction& sa){
    sa.sa_handler = this->sigChildHandler; 
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

}

void TCPServer::sigchld_handler(int s){
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

int32_t TCPServer::establishEndpoint(int socketfd, struct addrinfo *p){
    return bind(socketfd, p->ai_addr, p->ai_addrlen);
}

void TCPServer::queueConns(){
    if (listen(this->sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }
}

int32_t TCPServer::acceptConn(struct sockaddr* clientaddr){
    socklen_t sin_size = sizeof clientaddr;
    return accept(this->sockfd, clientaddr, &sin_size);
}

int32_t TCPServer::handleRequest(int socketfd){
    //4 bytes for the header
    int32_t len = 0;
    char readbuf[4 + MAX_MSGLEN];
    //Read the msg len from header of the client
    int32_t err = this->readAll(socketfd, readbuf, 4);
    if(err <= -1){
        perror("HandlRequest->readall failed");
        exit(1);
    }
    memcpy(&len, readbuf, 4);

    //Read the rest of the message
    err = this->readAll(socketfd, &readbuf[4], len);
    if(err <= -1){
        perror("Handle_Request->Readall failed ");
        return err;
    } 
    if(len > MAX_MSGLEN){
        std::cerr << "Received Message len too long" << std::endl;
        return -1;
    }
    readbuf[len + 4 + 1] = '\0';//Null terminate message received
    std::cout << "Client Says: " << &readbuf[4] << std::endl;

    char writebuf[4 + MAX_MSGLEN];
    const char* reply = "Message Received";
    len = (uint32_t)strlen(reply);
    //Write the len into the header and the msg into the buffer
    memcpy(writebuf, &len, 4);
    memcpy(&writebuf[4], reply, len);

    //Write the buffer to the client
    return this->writeAll(socketfd, writebuf, len + 4);
}


