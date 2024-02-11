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

std::string fillMap( std::map<int,std::string> &headerMap,std::string lenght,std::string line)
{
    headerMap[0] = " 200 OK\r\n";
    headerMap[1] = "Content-type: text/html\r\n";
    headerMap[2] = std::string("Content-Lenght: ").append(lenght).append("\r\n");
    std::istringstream wiss(line);
    std::string token;
    while(std::getline(wiss,token,'\n'))
    {
        if(token.find("Status:") != std::string::npos)
            headerMap[0] = token.append("\n");
        if(token.find("Content-type:") != std::string::npos)
            headerMap[1] = token.append("\n");
        if(token.find("Content-Lenght: ") != std::string::npos)
            headerMap[2] = token;
        if(token.find("Location: ") != std::string::npos)
            headerMap[3] = token;
    }
    std::string header;
    for (size_t i = 0; i < headerMap.size(); i++)
    {
        header.append(headerMap[i]);
    }
    return header;
}

void makeHeader(Data &dataClient,ssize_t &lenght)
{
    std::map<int,std::string> headerMap;
    std::string body;
    std::string tmp;
    size_t pos;
    pos = dataClient.restRead.find("\r\n\r\n");
    if(pos != std::string::npos)
    {
        tmp = dataClient.restRead.substr(0,pos);
        dataClient.restRead = dataClient.restRead.substr(pos+1);
        size_t bodyLenght = lenght  - tmp.size();
        std::stringstream ss;
        ss << bodyLenght;
        std::string header = dataClient.requeste->http_v.append(fillMap(headerMap,ss.str(),tmp).append("\r\n\r\n"));
        send(dataClient.fd,header.c_str(),header.size(),0);
        dataClient.isCgi = true;
    }
}

void   SendHeader(Data &dataClient)
{
    dataClient.fileFd = open(dataClient.cgiFile.c_str(),O_RDONLY);
    if (dataClient.fileFd == -1)
        throw std::runtime_error("internal server error");
    struct stat fileInfo;
    stat(dataClient.cgiFile.c_str(),&fileInfo);
    ssize_t lenght = fileInfo.st_size;
    char buffer[BUFFER_SIZE];
    ssize_t byteRead = read (dataClient.fileFd,buffer,BUFFER_SIZE -1);
    if(byteRead <= 0)
        throw std::runtime_error("faild read");
    dataClient.restRead.append(std::string(buffer,byteRead));
    makeHeader(dataClient,lenght);
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
            dataClient.restRead.append(buffer,byteRead);
            send(dataClient.fd, dataClient.restRead.c_str(), dataClient.restRead.size(),0);
            dataClient.restRead.clear();
        }
    }
    else
    {
        if(dataClient.isFork == false)
        {
            dataClient.startTime = getCurrentTime();
            dataClient.isFork = true;
            std::ostringstream oss;
            oss <<  dataClient.fd;
            dataClient.cgiFile = "/nfs/homes/afadlane/tmp/file" + oss.str();
            dataClient.fileFd = open(dataClient.cgiFile.c_str() ,O_WRONLY  | O_CREAT | O_TRUNC,0644);
            if (dataClient.fileFd == -1)
                throw std::runtime_error ("internal server error");
            dataClient.pid = fork();
            if(dataClient.pid == -1)
            {
                close(dataClient.fileFd);
                throw std::runtime_error("internal server error");
            }
            if (dataClient.pid == 0)
            {
                dup2(dataClient.fileFd, 1);
                close(dataClient.fileFd);
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
                close(dataClient.fileFd);
                kill(dataClient.pid,SIGTERM);
                std::string status =" 504 Gateway Timeout"; 
                sendResponse(dataClient,status);
                dataClient.code = 504;
                unlink(dataClient.cgiFile.c_str());
            }
        }
        else
        {
            close(dataClient.fileFd);
            SendHeader(dataClient);
        }
    }
}