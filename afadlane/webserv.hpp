#pragma once
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <cstring>
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
#define  BUFFER_SIZE 1024
using namespace  std;

struct Data
{  
    int fd ;
    int fileFd;
    int errorFd;
    bool  isDone;
    bool  isFork;
    pid_t pid;
    long code;
    bool isCgi;
    double startTime;
    bool autoIndex;
    bool isReading;
    bool Alreadyopen;
    bool Alreadparce;
    std::string Path;
    bool modeAutoIndex;
    bool readyForClose;
    Requeste *requeste ;
    std::string listDirectory;
    std::string restRead;
    bool AlreadyRequestHeader;
    std::vector<std::string> autoFile;
    bool isExeceted;
    std::string cgiFile;
    std::string statusCode;
};

struct Webserv
{
    Data data;
};


void    getMethod(Data &);
void    deleteMethod(Data &);
double  getCurrentTime(void);
void    fastCGI(Data &,std::string &);
void    multiplexing(ConfigFile &config);
bool    checkPermission(Data &, const char *,int );
void    sendErrorResponse(Data &dataClient);
void    sendChunk(int clientSocket, const char* data, ssize_t length,Data& dataClient);
std::string makeHeader(std::string &line,std::string &lenght);