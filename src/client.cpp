#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "4002"
#define MAXDATASIZE 100

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

int main(int argc, char** argv){
    if(argc != 2){
        fprintf(stderr, "Usage server <host_ip/hostname>");
        exit(-1);
    }
    struct addrinfo hints, *serverinfo, *p;
    struct sigaction sa;
    int sockfd, status;
    socklen_t sin_size;
    int yes  = 1;

    //setting padding to 0
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(argv[1], PORT, &hints, &serverinfo);
    if(status != 0){
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    }

    //Find the first available struct and bind to the first to it
    for(p = serverinfo; p != NULL; p->ai_next){
        sockfd = socket(p->ai_family, p->ai_socktype,  p->ai_protocol);
        if(sockfd == -1){
            perror("Client: socket");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            die("setsockopt");
        }
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            perror("Connect()");
            continue;
        }
        break;
    }
    if(p == NULL){
        fprintf(stderr, "Client: failed to connect\n");
        exit(-1);
    }
    freeaddrinfo(serverinfo);//Free the linked list, we don't need it anymore

    printf("Client connected to %s\n", s);
    while(1){

    }
    return 0;

}