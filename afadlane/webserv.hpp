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

class DELETE
{
    private:
        struct stat statInfo;
    public:
        void  IsDir(Data &);
        void  IsFIle(Data &);
        void  deleteMethod(Data &);
        void  checkErrnoStat(Data &);
};

class GETMETHOD
{
    private :
        int             fileFd;
        bool            isReading;
        bool            Alreadyopen;
        bool            modeAutoIndex;
        std::string     listDirectory;
    public:
        GETMETHOD();
        std::string     getContentType(Data &);
        void            getMethod(Data &);
        void            sendChunk(int , std::string &);
        void            openFileAndSendHeader(Data& );
        int             checkFileDirPermission(Data &);
        void            openDirFIle(Data & );
        void            serveFIle(Data& );
        void            sendListDir(Data & );
        int             listingDirectory(Data &);
        bool            getAutoFile(Data & ,char * );
};

class CGI
{
    private:
        int             fileFdCgi;
        bool            isFork;
        ssize_t         lenghtFile;
        std::string     restRead;
        double          startTime;
        bool            isReadingCgi;
        bool            sendHeader;
    public:
        std::string     cgiFile;
        pid_t           pid;
        CGI();
        void            sendBody(Data &);
        void            SendHeader(Data &);
        void            makeHeader(Data &,bool );
        std::string     getType(Data&,std::string &);
        void            fastCGI(Data &,std::string &);
        void            executeScript(Data &,std::string &);
        void            environmentStore(Data &, std::vector<std::string> &);
        std::string     fillMap(std::map<int,std::string>&,std::string,std::string);
        
};

struct Data
{  
    int fd ;
    long code;
    CGI OBJCGI;
    int errorFd;
    bool  isDone;
    DELETE OBJDEL;
    bool isDelete ;
    bool autoIndex;
    std::string Path;
    bool Alreadparce;
    GETMETHOD OBJGET;
    Requeste *requeste ;
    bool readyForClose;
    std::string statusCode;
    bool AlreadyRequestHeader;
};

struct Webserv
{
    Data data;
};

double  getCurrentTime(void);
void    insialStruct(Data &  );
void    sendErrorResponse(Data &);
void    multiplexing(ConfigFile &);
bool    checkPermission(Data &,int );
void    closeServers(std::vector<int> &);
bool    isServer(std::vector<int> & ,int );
void    EpollCtrDEL(int ,int ,std::map<int,struct Webserv>&);
void    inisialData(std::map<int,struct Webserv> & ,ConfigFile &,int &);