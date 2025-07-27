#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <vector>

#ifndef EPOLL_HEADER_H
#define EPOLL_HEADER_H

#define MAX_EVENTS 1024

//TODO Add a way to add more fds, and check if epoll is alr instantiated
using std::vector;

class Epoll{
private:
    int32_t epoll_fd;

public:
    Epoll(vector<struct epoll_event> event_arr);//Adds all events and fds to the epoll instance
    Epoll(struct epoll_event event_arr[], int32_t& num_events);//Adds all events and fds to the epoll instance
    ~Epoll();//Closes epoll_fd. Does NOT close fds under epoll watch

    //calls epoll_wait once, then calls passed callback function once for every ready connection. Up to the user to repeatedly call this function 
    //Automatically checks for any EPOLLHUP events and closes that fd
    template <typename Userdata>
    void mainloop(void(*callback)(const struct epoll_event&, Userdata*), Userdata* userdata = nullptr, int32_t timeout = -1){
        struct epoll_event ready_events[MAX_EVENTS];
        int nfds = epoll_wait(epoll_fd, ready_events, MAX_EVENTS, timeout);
        if(nfds > 0){
            //Userdata is any data that the user wants to pass into their callback function
            for(int i = 0; i < nfds; i++){
                if(ready_events[i].events & EPOLLHUP){
                    close(ready_events[i].data.fd);
                    continue;
                }
                callback(ready_events[i], userdata); 
            }
        }
        if(nfds < 0){
            perror("epoll_wait faled");
        }
    }
    
    //calls epoll_wait once, then calls passed callback function once. Up to the user to repeatedly call this function 
    //Up to the user to check for EPOLLHUP to close socket
    template <typename Userdata>
    void mainloop(void(*callback)(const struct epoll_event*, const int32_t&, Userdata*), Userdata* userdata = nullptr, int32_t timeout = -1){
        struct epoll_event ready_events[MAX_EVENTS];
        int nfds = epoll_wait(epoll_fd, ready_events, MAX_EVENTS, timeout);
        if(nfds > 0){
            //Userdata is any data that the user wants to pass into their callback function
            callback(ready_events, nfds, userdata); 
        }
        if(nfds < 0){
            perror("epoll_wait faled");
        }
    }
};

#endif