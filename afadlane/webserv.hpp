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
#define  BUFFER_SIZE 1024
#define  SERVERS 3
struct Method
{
    std::string path;
    std::string version;
    std::string buff;
    std::string rootLocation;
    std::string host;
    std::string autoFile;
    std::string fullPath;
};
struct Data
{  
    int fd ;
    bool isReading;
    bool Alreadyopen;
    bool Alreadparce;
    bool modeAutoIndex;
    bool readyForClose;
    bool AlreadyRequestHeader;
    bool isCgi;
    Requeste *requeste ;
    Method   method;
};


struct Webserv
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

void multiplexing();
void fastCGI(std::string &,std::string &);
void    parceRequest(Data & ,Method &,int );
bool   deleteMethod(int ,std::string &,bool &);
bool checkPermission(const char *path,int fd,std::string &version,int type,bool &);
void    getMethod(Data & ,Method &,std::vector<std::pair<std::string,ServerConfig> > &,int );
void    sendResponse(int fd,std::string &version,std::string &status,bool &isReadyForClose);