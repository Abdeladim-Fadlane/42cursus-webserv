#include"webserv.hpp"
void environmentStore(std::vector<std::string>& environment,std::string path)
{
    std::string REQUEST_METHOD = "GET";
    std::string CONTENT_TYPE = "text/html";
    std::string CONTENT_LENGTH = "";
    std::string QUERY_STRING = "?first_name=Abdeladim&last_name=Fadlane";
    std::string SCRIPT_FILENAM = path;
    std::string SERVER_PROTOCOL = "HTTP/1.1";
    std::string SERVER_ADDR = "127.0.0.1";
    std::string SERVER_PORT = "8080";

    environment.push_back("REQUEST_METHOD=" + REQUEST_METHOD);
    environment.push_back("CONTENT_TYPE=" + CONTENT_TYPE);
    environment.push_back("CONTENT_LENGTH=" + CONTENT_LENGTH);
    environment.push_back("QUERY_STRING=" + QUERY_STRING);
    environment.push_back("SCRIPT_FILENAME="+ SCRIPT_FILENAM); 
    environment.push_back("SERVER_PROTOCOL=" + SERVER_PROTOCOL);
    environment.push_back("SERVER_ADDR=" + SERVER_ADDR);
    environment.push_back("SERVER_PORT=" + SERVER_PORT); 
}

void fastCGI(std::string &path,std::string &type)
{
    std::vector<std::string> environment;
    char* env[environment.size() + 1]; 
    for(size_t i = 0; i< environment.size();i++)
    {
        env[i] = const_cast<char*>(environment[i].c_str());
    }
    env[environment.size()] = NULL;
    pid_t pid;
    int fd = open("/tmp/tmpFile",O_RDONLY | O_WRONLY | O_CREAT ,777);
    if (fd == -1)
        throw std::runtime_error ("internal server error");
    pid = fork();
    if(pid == -1)
        throw std::runtime_error("internal server error");
    std::string interpreter ;
    if(type == "py")
        interpreter = "/usr/bin/python3";
    else if(type == "php")
        interpreter = "/usr/bin/php-cgi8.2";
    else
        throw std::runtime_error("Unsupported");
    if (pid == 0)
    {
        dup2(fd, 1);
        const char *args[] = {interpreter.c_str(), path.c_str(), NULL};
        close(fd);
        execve(interpreter.c_str(), const_cast<char* const*>(args), env);
        throw std::runtime_error ("Cannot exectue script");
    } 
    else
        wait(NULL);
    close(fd);
}