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
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <dirent.h>
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
    private :
        std::string listDirectory;
    public:
        void getMethod(Data &);
        void sendChunk(int , std::string &,Data& );
        void   openFileAndSendHeader(Data& );
        int checkFileDirPermission(Data &);
        void   openDirFIle(Data & );
        void serveFIle(Data& );
        void sendListDir(Data & );
        int    listingDirectory(Data &);
        std::string   getContentType(Data &);
};

class CGI
{
    private:
        std::string cgiFile;
        ssize_t lenghtFile;
        std::string restRead;
    public:
        void sendBody(Data &);
        void   SendHeader(Data &);
        void makeHeader(Data &,bool );
        void fastCGI(Data &,std::string &);
        void executeScript(Data &,std::string &);
        std::string getType(Data&,std::string &);
        void environmentStore(Data &, std::vector<std::string> &);
        std::string fillMap(std::map<int,std::string> &,std::string ,std::string );
};

class DELETE
{
    private:
        struct stat statInfo;
    public:
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
    CGI OBJCGI;
    bool AlreadyRequestHeader;
    bool isExeceted;
    std::string statusCode;
};


struct Webserv
{
    Data data;
};

double  getCurrentTime(void);
void    sendErrorResponse(Data &);
void    multiplexing(ConfigFile &);
bool    checkPermission(Data &,int );