#include"webserv.hpp"

CGI::CGI()
{
    cgiFile = "";
    pid = -1;
    isFork = false;
    sendHeader = false;
    isReadingCgi = false;
    fdFile         =   new std::ifstream();
}

CGI::~CGI()
{
    if(fdFile->is_open())
        fdFile->close();
    if(!cgiFile.empty())
        std::remove(cgiFile.c_str());
    delete fdFile;
}

void CGI::environmentStore(Data &dataClient, std::vector<std::string> &environment)
{
    std::stringstream wiss;
    wiss << dataClient.requeste->port;
    std::string SERVER_PORT = wiss.str();
    std::string REQUEST_METHOD = dataClient.requeste->method;
    std::string CONTENT_TYPE = dataClient.requeste->content_type;
    std::string CONTENT_LENGTH = dataClient.requeste->content_length;
    std::string QUERY_STRING = dataClient.requeste->query_str ; 
    std::string SCRIPT_FILENAM = dataClient.Path;
    std::string SERVER_PROTOCOL = dataClient.requeste->http_v;
    std::string SERVER_ADDR = dataClient.requeste->host;
    environment.push_back("REQUEST_METHOD=" + REQUEST_METHOD);
    environment.push_back("REDIRECT_STATUS=200");
    environment.push_back("CONTENT_TYPE=" + CONTENT_TYPE);
    environment.push_back("CONTENT_LENGTH=" + CONTENT_LENGTH);
    environment.push_back("QUERY_STRING=" + QUERY_STRING);
    environment.push_back("SCRIPT_FILENAME="+ SCRIPT_FILENAM); 
    environment.push_back("SERVER_PROTOCOL=" + SERVER_PROTOCOL);
    environment.push_back("SERVER_ADDR=" + SERVER_ADDR);
    environment.push_back("SERVER_PORT=" + SERVER_PORT);
    if (dataClient.requeste->requeste_map.find("Cookie") != dataClient.requeste->requeste_map.end())
        environment.push_back("HTTP_COOKIE=" + dataClient.requeste->requeste_map.find("Cookie")->second);
}

std::string CGI::fillMap( std::map<int,std::string> &headerMap,std::string lenght,std::string line)
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
            headerMap[2] = token.append("\n");
        if(token.find("Location: ") != std::string::npos)
            headerMap[3] = token;
        if(token.find("Set-Cookie: ") != std::string::npos)
            headerMap[4] = token;
    }
    std::string header;
    for (size_t i = 0; i < headerMap.size(); i++)
    {
        header.append(headerMap[i]);
    }
    return header;
}

void CGI::makeHeader(Data &dataClient,bool eof)
{
    std::map<int,std::string> headerMap;
    std::string body;
    std::string tmp;
    size_t pos;
    pos = restRead.find("\r\n\r\n");
    if(pos != std::string::npos)
    {
        tmp = restRead.substr(0,pos);
        restRead = restRead.substr(pos+1);
        size_t bodyLenght = lenghtFile  - tmp.size();
        std::stringstream ss;
        ss << bodyLenght;
        std::string header = dataClient.requeste->http_v;
        header.append(fillMap(headerMap,ss.str(),tmp).append("\r\n\r\n"));
        sendResponce(dataClient,header);
        sendHeader = true;
    }
    if(eof == true)
    {
        std::stringstream ss;
        ss <<  lenghtFile ;
        std::string header = dataClient.requeste->http_v;
        header.append(fillMap(headerMap,ss.str(),tmp).append("\r\n\r\n"));
        header.append(restRead);
        sendResponce(dataClient,header);
        dataClient.readyForClose = true;
    }
}

void   CGI::SendHeader(Data &dataClient)
{
    if(isReadingCgi == false)
    {
        fdFile->open(cgiFile.c_str());
        if(!fdFile->is_open())
            throw std::runtime_error("error");
        isReadingCgi = true;
        struct stat fileInfo;
        if(stat(cgiFile.c_str(),&fileInfo))
            throw std::runtime_error("error");
        lenghtFile = fileInfo.st_size;
    }
    char buffer[BUFFER_SIZE];
    fdFile->read(buffer,BUFFER_SIZE);
    if(fdFile->bad())
        throw std::runtime_error("error");
    std::streamsize bytesRead = fdFile->gcount();
    if(bytesRead == 0)
    {
        makeHeader(dataClient,true);
        return ;
    }
    std::string httpresponse(buffer,bytesRead);
    restRead.append(httpresponse);
    makeHeader(dataClient,false);
    
}

void CGI::sendBody(Data &dataClient)
{
    char buffer[BUFFER_SIZE];
    fdFile->read(buffer,BUFFER_SIZE);
    if(fdFile->bad())
        throw std::runtime_error("error");
    std::streamsize bytesRead  = fdFile->gcount();
    if(bytesRead == 0)
    {
        if(!restRead.empty())
            sendResponce(dataClient,restRead);
        dataClient.readyForClose = true;     
    }
    else
    {
        std::string httpresponse(buffer,bytesRead);
        restRead.append(httpresponse);
        sendResponce(dataClient,restRead);
        restRead.clear();
    }
}

std::string CGI::getType(Data&dataClient,std::string &type)
{
    isFork = true;
    struct timeval currentTime;
    gettimeofday(&currentTime,NULL);
    std::ostringstream oss;
    oss << currentTime.tv_usec;
    cgiFile.append("TOOLS/UTILS/file").append(oss.str());
    if(type == ".py" )
        return dataClient.requeste->Location_Server.cgi[".py"];
    else if(type == ".php")
        return dataClient.requeste->Location_Server.cgi[".php"];
    else if(type == ".pl")
        return dataClient.requeste->Location_Server.cgi[".pl"];
    return "";
}

void CGI::executeScript(Data &dataClient,std::string &type)
{
    std::vector<std::string> environment;
    environmentStore(dataClient,environment);
    char* env[environment.size() + 1]; 
    for(size_t i = 0; i < environment.size();i++)
    {
        env[i] = const_cast<char*>(environment[i].c_str());
    }
    env[environment.size()] = NULL;
    std::string interpreter = getType(dataClient,type);
    startTime = getCurrentTime();
    pid = fork();
    if(pid == -1)
        throw std::runtime_error("error");
    if (pid == 0)
    {
        std::freopen(cgiFile.c_str() , "w", stdout);
        if(dataClient.requeste->method == "POST")
            std::freopen(dataClient.requeste->post->cgi_path.c_str(), "r", stdin);
        const char *args[] = {interpreter.c_str(), dataClient.Path.c_str(), NULL};
        size_t pos = dataClient.Path.find_last_of("/");
        std::string strpwd;
        if(pos != std::string::npos)
            strpwd = dataClient.Path.substr(0,pos);
        if(chdir(strpwd.c_str()) == -1 || execve(interpreter.c_str(), const_cast<char* const*>(args), env) == -1)
            exit(1);
    }
}

void CGI::fastCGI(Data &dataClient,std::string &type)
{
    try
    {
        if(sendHeader == false)
        {
            int status;
            if(isFork == false)
                executeScript(dataClient,type);
            if(waitpid(pid,&status,WNOHANG) == 0)
            {
                size_t n = dataClient.requeste->Server_Requeste.cgi_timeout;
                if(getCurrentTime() - startTime >= n)
                {
                    kill(pid,SIGKILL);
                    waitpid(pid,&status,0);
                    dataClient.statusCode =" 504 Gateway Timeout"; 
                    dataClient.code = 504;
                }
            }
            else
            {
                if (WEXITSTATUS(status) == 1)
                {
                    if(dataClient.requeste->method == "POST")
                        std::remove(dataClient.requeste->post->cgi_path.c_str());
                    throw std::runtime_error("error");
                }
                if(dataClient.requeste->method == "POST")
                {
                    if(std::remove(dataClient.requeste->post->cgi_path.c_str()) == -1)
                        throw std::runtime_error("error");
                }
                SendHeader(dataClient);
            }
        }
        else if(sendHeader == true)
            sendBody(dataClient);
    }
    catch(const std::exception& e)
    {
        if(strcmp(e.what(),"error send") == 0)
            dataClient.readyForClose = true;
        else
        {
            dataClient.statusCode = " 500 Internal Server Error";
            dataClient.code = 500;
        }     
    }
    
}