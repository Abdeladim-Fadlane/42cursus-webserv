#include"webserv.hpp"
void environmentStore(Data &dataClient, std::vector<std::string> &environment)
{
    std::stringstream wiss;
    wiss << dataClient.requeste->port;
    std::string REQUEST_METHOD = dataClient.requeste->method;
    std::string CONTENT_TYPE = dataClient.requeste->content_type;
    std::string CONTENT_LENGTH = dataClient.requeste->content_length;
    std::string QUERY_STRING = dataClient.requeste->query_str ; 
    std::string SCRIPT_FILENAM = dataClient.Path;
    std::string SERVER_PROTOCOL = dataClient.requeste->http_v;
    std::string SERVER_ADDR = dataClient.requeste->host;
    std::string SERVER_PORT = wiss.str();

    environment.push_back("REQUEST_METHOD=GET");
    environment.push_back("REDIRECT_STATUS=200");
    environment.push_back("CONTENT_TYPE=" + CONTENT_TYPE);
    environment.push_back("CONTENT_LENGTH=" + CONTENT_LENGTH);
    environment.push_back("QUERY_STRING=" + QUERY_STRING);
    environment.push_back("SCRIPT_FILENAME="+ SCRIPT_FILENAM); 
    environment.push_back("SERVER_PROTOCOL=" + SERVER_PROTOCOL);
    environment.push_back("SERVER_ADDR=" + SERVER_ADDR);
    environment.push_back("SERVER_PORT=" + SERVER_PORT); 
}
std::string checkElemnetExit(std::vector<string> & header,const char *str)
{
    for(size_t i  = 0; i < header.size(); i++)
    {
        if(header[i].find(str) != std::string::npos)
            return header[i];
    }
    return "";
}

std::string splitHeader(std::string &line,std::string &lenght)
{
    std::istringstream wiss(line);
    std::string token;
    
    std::string returnHerader;
    std::vector<std::string>Header;
    while(std::getline(wiss,token,'\n'))
    {
        Header.push_back(token);
    }
    if(checkElemnetExit(Header,"Status").empty())
        returnHerader = std::string(" Status: 200 OK\r\n");
    else
        returnHerader = checkElemnetExit(Header,"Status");
    if(!checkElemnetExit(Header,"Location:").empty())
        returnHerader += checkElemnetExit(Header,"Location:");
    if(checkElemnetExit(Header,"Content-Type:").empty())
        returnHerader += "Content-Type: text/html\r\n";
    else
        returnHerader += checkElemnetExit(Header,"Content-Type:");
    if(checkElemnetExit(Header,"Content-Length:").empty())
        returnHerader += "Content-Length: " + lenght + "\r\n\r\n";
    else
        returnHerader += checkElemnetExit(Header,"Content-Length:") + "\r\n\r\n";
    return returnHerader;
}

std::string makeHeader(std::string &line,std::string &lenght)
{
    std::string header;
    std::string body;
    size_t pos;
    pos = line.find("\r\n\r\n");
    if(pos != std::string::npos)
    {
        header = line.substr(0,pos);
        header = splitHeader(header,lenght);
        body   = line.substr(pos+1);
    }
    return header + body;
}
void   SendHeader(Data &dataClient)
{
    dataClient.fileFd = open(dataClient.cgiFile.c_str(),O_RDONLY);
    if (dataClient.fileFd == -1)
        throw std::runtime_error("internal server error");
    // if(dataClient.fileFd == 0)
    // {
    //     dataClient.readyForClose = true;
    //     return;
    // }
    std::cout<<"fd 2   = "<<dataClient.fileFd<<"\n";

    struct stat fileInfo;
    stat(dataClient.cgiFile.c_str(),&fileInfo);
    std::ostringstream oss;
    oss << fileInfo.st_size;
    std::string lengh = oss.str();
    char buffer[BUFFER_SIZE];
    memset(buffer,0,BUFFER_SIZE);
    std::string httpResponse;
    ssize_t byteRead = read (dataClient.fileFd,buffer,BUFFER_SIZE -1);
    if(byteRead <= 0)
        throw std::runtime_error("faild read");
    std::string line(buffer);
    httpResponse = dataClient.requeste->http_v + makeHeader(line,lengh);
    if(send(dataClient.fd,httpResponse.c_str(),httpResponse.size(),0) == -1)
        throw std::runtime_error("error send ");
}

void fastCGI(Data &dataClient,std::string &type)
{
    std::vector<std::string> environment;
    environmentStore(dataClient,environment);
    char* env[environment.size() + 1]; 
    for(size_t i = 0; i< environment.size();i++)
    {
        env[i] = const_cast<char*>(environment[i].c_str());
    }
    env[environment.size()] = NULL;
    std::string interpreter ;
    if(type == ".py")
        interpreter = dataClient.requeste->Location_Server.cgi[".py"];
    else if(type == ".php")
        interpreter = dataClient.requeste->Location_Server.cgi[".php"];
    else if(type == ".sh")
        interpreter = dataClient.requeste->Location_Server.cgi[".sh"];
    if(dataClient.isCgi == true)
    {
        char buffer[BUFFER_SIZE];
        std::string httpResponse;
        ssize_t byteRead = read (dataClient.fileFd,buffer,BUFFER_SIZE);
        if(byteRead <= 0)
        {
            close(dataClient.fileFd);
            dataClient.readyForClose = true;
            unlink(dataClient.cgiFile.c_str());
        }
        else
        {
            std::string line(buffer,byteRead);
            if(send(dataClient.fd,line.c_str(),line.size(),0) == -1)
                throw std::runtime_error("error send");
        }
    }
    else
    {
        int fd;
        if(dataClient.isFork == false)
        {
            dataClient.startTime = getCurrentTime();
            dataClient.isFork = true;
            std::ostringstream oss;
            oss <<  dataClient.fd;
            dataClient.cgiFile = "/nfs/homes/afadlane/tmp/file" + oss.str();
            fd = open(dataClient.cgiFile.c_str() ,O_WRONLY  | O_CREAT | O_TRUNC,0644);
            if (fd == -1)
                throw std::runtime_error ("internal server error");
            dataClient.pid = fork();
            if(dataClient.pid == -1)
            {
                close(fd);
                throw std::runtime_error("internal server error");
            }
            if (dataClient.pid == 0)
            {
                dup2(fd, 1);
                close(fd);
                const char *args[] = {interpreter.c_str(), dataClient.Path.c_str(), NULL};
                if(execve(interpreter.c_str(), const_cast<char* const*>(args), env) == -1)
                    throw std::runtime_error ("Cannot exectue script");
            }
        }
        int status;
        if(waitpid(dataClient.pid,&status,WNOHANG) == 0)
        {
            /* child proccess still runing */
            if(getCurrentTime() - dataClient.startTime >=  3)
            {
                close(fd);
                kill(dataClient.pid,SIGTERM);
                std::string status =" 504 Gateway Timeout"; 
                sendResponse(dataClient,status);
                dataClient.code = 504;
                unlink(dataClient.cgiFile.c_str());
            }
        }
        else
        {
            close(fd);
            SendHeader(dataClient);
            dataClient.isCgi = true;
        }
    }
}