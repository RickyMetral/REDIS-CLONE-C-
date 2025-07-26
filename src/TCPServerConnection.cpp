#include "TCPServerConnection.hpp"
#include <stdio.h>

TCPServerConnection::TCPServerConnection(std::string clientAddr, const char* serverPort, const char* clientPort, int sock_family){
    struct sigaction sa;
    int sockfd;
    socklen_t sin_size;

    sockfd = this->createSocket(serverPort);
    this->reapDeadProcesses(sa);
}
TCPServerConnection::~TCPServerConnection(){
    if(this->socketfd >= 0){
        close(this->socketfd);
    }
}

void TCPServerConnection::die(const char *msg){
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

void TCPServerConnection::sigChildHandler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
    
void TCPServerConnection::reapDeadProcesses(struct sigaction& sa){
    sa.sa_handler = sigChildHandler; 
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

}

void TCPServerConnection::bindSocket(){
    if (bind(this->socketfd, (struct sockaddr*)&(this->serveraddr), this->s_socksize) < 0){
        throw std::runtime_error("Bind failed");
    }
}

int TCPServerConnection::createSocket(const char* port){
    addrinfo *serverinfo, *p;
    int sockfd;
    int yes = 1;

    int status = getaddrinfo(NULL, port, &this->hints, &serverinfo);

    if(status != 0){
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    }

    //Find the first available struct and bind to the first to it
    for(p = serverinfo; p != NULL; p->ai_next){
        sockfd = socket(p->ai_family, p->ai_socktype,  p->ai_protocol);
        if(sockfd == -1){
            perror("Server: socket");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            this->die("setsockopt");
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            perror("Bind()");
            continue;
        }
        break;
    }

    if(p == NULL){
        fprintf(stderr, "Server: failed to bind\n");
        exit(-1);
    }

    freeaddrinfo(serverinfo);//Free the linked list, we don't need it anymore
    return sockfd;
}

void TCPServerConnection::queueConnections(const int& sockfd){
    if(listen(sockfd, BACKLOG) == -1){
        this->die("listen()");
    }

    printf("Listening for Connections...");
}

int TCPServerConnection::sendAll(const void* message, int msglen, int flags){
}

int TCPServerConnection::receiveAll(char* buffer, int bufferSize, int flags){
}


