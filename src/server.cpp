#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include "TCPServerConnection.hpp"

#define PORT "4001"

#define BACKLOG 10
void find_first_socket(struct addrinfo& p, struct addrinfo& serverinfo, int& sockfd){

}
void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

static void die(const char *msg){
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main(){
    struct addrinfo hints, *serverinfo, *p;
    struct sockaddr_storage clientaddr;
    struct sigaction sa;
    int sockfd, status;
    socklen_t sin_size;
    int yes  = 1;

    //setting padding to 0
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, PORT, &hints, &serverinfo);
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
            die("setsockopt");
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

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    if(listen(sockfd, BACKLOG) == -1){
        die("listen()");
    }

    printf("Listening for Connections...");

    while(1){
        sin_size = sizeof(clientaddr);
        int newfd = accept(sockfd, (struct sockaddr*)&clientaddr, &sin_size);

        if(newfd == -1){
            perror("accept");
            continue;
        }

        if(!fork()){//Make sure we are in child process
            if(send(newfd, "Hello_world", 13, 0) == -1){
                perror("send");
            }
            close(sockfd);//closing file descriptors for child
            close(newfd);
            exit(0);
        }
        close(newfd);//Parent doesnt need it
    }
    return 0;

}