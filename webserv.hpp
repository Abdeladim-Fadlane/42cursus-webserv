#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <stdio.h>
#include <cerrno>
#include <stdlib.h>
#include <sys/epoll.h>
#include<vector>
#include <unistd.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <algorithm>
#define TRUE 1
#define FALSE 0

#define  MAX_EVENTS 10
#define  PORT  8080
#define  SERVER_IP "0.0.0.0"
#define  BUFFER_SIZE 1024
#define  SERVERS 3

class Webserv
{
    private:
        std::vector<int> servers;
    public:
        Webserv(/* args */){};
        ~Webserv(){};
};
void multiplexing();
int fastCGI();