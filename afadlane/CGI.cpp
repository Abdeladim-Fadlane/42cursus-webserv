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

    environment.push_back("REQUEST_METHOD=" + REQUEST_METHOD);
    environment.push_back("REDIRECT_STATUS=200");
    environment.push_back("CONTENT_TYPE=" + CONTENT_TYPE);
    environment.push_back("CONTENT_LENGTH=" + CONTENT_LENGTH);
    environment.push_back("QUERY_STRING=" + QUERY_STRING);
    environment.push_back("SCRIPT_FILENAME="+ SCRIPT_FILENAM); 
    environment.push_back("SERVER_PROTOCOL=" + SERVER_PROTOCOL);
    environment.push_back("SERVER_ADDR=" + SERVER_ADDR);
    environment.push_back("SERVER_PORT=" + SERVER_PORT); 
}

void   SendHeader(Data &dataClient)
{
    dataClient.fileFd = open("/tmp/tmpFile",O_RDONLY);
    if (dataClient.fileFd == -1)
    {
        close(dataClient.fileFd);
        throw std::runtime_error("internal server error");
    }
    char buffer[BUFFER_SIZE];
    std::string httpResponse;
    ssize_t byteRead = read (dataClient.fileFd,buffer,BUFFER_SIZE);
    std::string line(buffer,byteRead);
    size_t pos = line.find("\r\n\r\n");
    if(pos != std::string::npos)
    {
        httpResponse = line .substr(0,pos);
        dataClient.restRead = line.substr(pos+1);
    }
    httpResponse = dataClient.requeste->http_v + line;
    send(dataClient.fd,httpResponse.c_str(),httpResponse.size(),0);
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
    else
        throw std::runtime_error("Unsupported");


    if(dataClient.requeste->method == "POST")
    {
        /* HUNDL POST*/
        int fd = open("/tmp/tmpFile",O_RDONLY | O_WRONLY | O_CREAT ,777);
        int fdPost = open("afadlane/TOOLS/postFile",O_RDONLY | O_WRONLY | O_CREAT ,777);
        if (fd == -1 || fdPost == -1)
            throw std::runtime_error ("internal server error 0");
        pid_t pid;
        pid = fork();
        if(pid == -1)
            throw std::runtime_error("internal server error");
        if(pid == 0)
        {
            dup2(fd,1);
            dup2(fdPost,0);
            close(fd);
            close(fdPost);
            const char *args[] = {interpreter.c_str(), dataClient.Path.c_str(), NULL};
            execve(interpreter.c_str(), const_cast<char* const*>(args), env);
            throw std::runtime_error ("Cannot exectue script");
        }
        else
            wait(NULL);
        close(fd);
    }
    
    if(dataClient.requeste->method == "GET")
    {

        if(dataClient.isCgi == true)
        {
            char buffer[BUFFER_SIZE];
            std::string httpResponse;
            ssize_t byteRead = read (dataClient.fileFd,buffer,BUFFER_SIZE);
            if(byteRead == 0)
            {
                close(dataClient.fileFd);
                dataClient.readyForClose = true;
                unlink("/tmp/tmpFile");
            }
            else
            {
                std::string line(buffer,byteRead);
                sendChunk(dataClient.fd,line.c_str(),line.size(),dataClient);
            }
        }
        else
        {
            dataClient.isCgi = true;
            pid_t pid;
            int fd = open("/tmp/tmpFile",O_RDONLY | O_WRONLY | O_CREAT ,777);
            if (fd == -1)
                throw std::runtime_error ("internal server error 0");
            pid = fork();
            if(pid == -1)
                throw std::runtime_error("internal server error");
            if (pid == 0)
            {
                dup2(fd, 1);
                const char *args[] = {interpreter.c_str(), dataClient.Path.c_str(), NULL};
                close(fd);
                execve(interpreter.c_str(), const_cast<char* const*>(args), env);
                throw std::runtime_error ("Cannot exectue script");
            } 
            else
                wait(NULL);
            close(fd);
            SendHeader(dataClient);
        }
    }
}