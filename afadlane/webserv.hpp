#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <stdio.h>
#include <cerrno>
#include <stdlib.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <signal.h>
#include <algorithm>
#include <dirent.h>
#include <iomanip> 
#include <map>
#include <stdexcept>
#include <sys/stat.h>
#include<cstdio>
#include<cstdlib>
#include "../akatfi/PostMethod.hpp"
#include"../akatfi/Requeste.hpp"
#define  MAX_EVENTS 1024
#define  PORT  8080
#define  BUFFER_SIZE 5120
#define  SERVERS 3
struct Data
{  
    int fd ;
    int isReading;
    int Alreadyopen;
    int Alreadparce;
    int modeAutoIndex;
    int readyForClose;
    int AlreadyRequestHeader;
    Requeste *requeste = NULL;
};


struct dataClient
{
    Data data; 
};

struct ServerConfig
{
    int port ;
    std::string listen;
    std::string host;
    std::string domainName;
    std::vector<std::string> errorPage;
    std::string clientMaxBodySize;
    int size ;
    std::string root ;
    std::string autoFile ;
    // std::vector<LocationConfig> locations;
};

struct Method
{
    std::string method;
    std::string path;
    std::string version;
    std::string buff;
    std::string rootLocation;
    std::string host;
    std::string port;
    std::string addressIp;
    std::string autoFile;
};

class Webserv
{
    private:
        std::vector<int> servers;
    public:
        Webserv(/* args */){};
        ~Webserv(){};
};

void multiplexing(Method&);
void fastCGI(std::string &);
void    parceRequest(Data & Data,Method &method,int fd);
void    deleteMethod(const std::string &,const std::string &);
void    getMethod(Data & datacleint,Method &,std::vector<std::pair<std::string,ServerConfig> > &,int );