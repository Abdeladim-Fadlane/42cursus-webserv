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
struct Data;
class GETMETHOD
{
    public:
        void getMethod(Data &dataClient);
        void sendChunk(int clientSocket, std::string &data,Data& dataClient);
        void   openFileAndSendHeader(Data& dataClient);
        int checkFileDirPermission(Data &dataClient);
        void   openDirFIle(Data & dataClient);
        void serveFIle(Data& dataClient);
        void sendListDir(Data & dataClient);
        int    listingDirectory(Data &dataClient);
        std::string   getContentType(Data &method);
};
class CGI
{
    public:

};
class DELETE
{
    private:
        struct stat statInfo;
    public:
        // void  dataDel(Data &);
        void  IsDir(Data &);
        void  deleteMethod(Data &);
        void  IsFIle(Data &);
};

struct Data
{  
    int fd ;
    bool isDelete ;
    int fileFd;
    int fileFdCgi;
    int errorFd;
    bool  isDone;
    bool  isFork;
    pid_t pid;
    long code;
    bool sendHeader;
    double startTime;
    bool autoIndex;
    bool isReading;
    bool isReadingCgi;
    bool Alreadyopen;
    bool Alreadparce;
    std::string Path;
    bool modeAutoIndex;
    bool readyForClose;
    Requeste *requeste ;
    DELETE OBJDEL;
    GETMETHOD OBJGET;
    std::string listDirectory;
    std::string restRead;
    bool AlreadyRequestHeader;
    std::vector<std::string> autoFile;
    bool isExeceted;
    std::string cgiFile;
    std::string statusCode;
    ssize_t lenghtFile;
};


struct Webserv
{
    Data data;
};

double  getCurrentTime(void);
void    fastCGI(Data &,std::string &);
void    multiplexing(ConfigFile &config);
bool    checkPermission(Data &,int );
void    sendErrorResponse(Data &);