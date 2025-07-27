#include "Epoll.hpp"


Epoll::Epoll(vector<struct epoll_event> event_arr){
    std::cout << "Initializing Epoll fd" << std::endl;

    this->epoll_fd = epoll_create(255);
    if(this->epoll_fd < 0){
        perror("epoll_create failed: ");
        exit(1);
    }

    for(int i = 0; i < event_arr.size(); i++){
        if(event_arr[i].data.fd < 0){
            std::cout << "Could not add file descriptor: " << event_arr[i].data.fd << "Not a valid fd." << std::endl;
            continue;
        }
        if(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, event_arr[i].data.fd, &event_arr[i]) == -1){
            perror("epoll_ctl failed");
            exit(1);
        }
    }
}

Epoll::Epoll(struct epoll_event event_arr[], int32_t& num_events){
    std::cout << "Initializing Epoll fd" << std::endl;

    this->epoll_fd = epoll_create(255);
    if(this->epoll_fd < 0){
        perror("epoll_create failed: ");
        exit(1);
    }

    for(int i = 0; i < num_events; i++){
        if(event_arr[i].data.fd < 0){
            std::cout << "Could not add file descriptor: " << event_arr[i].data.fd << "Not a valid fd." << std::endl;
            continue;
        }
        if(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, event_arr[i].data.fd, &event_arr[i]) == -1){
            perror("epoll_ctl failed");
            exit(1);
        }
    }
}

Epoll::~Epoll(){
    if(close(this->epoll_fd)){
        fprintf(stderr, "Failed to close epoll file descriptor\n");
        exit(1);
    }
}
