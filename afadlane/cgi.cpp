#include"webserv.hpp"
void environmentStore(std::vector<std::string>& environment,std::string path)
{
    std::string REQUEST_METHOD = "GET";
    std::string CONTENT_TYPE = "text/html";
    std::string CONTENT_LENGTH = "";
    std::string QUERY_STRING = "";
    std::string SCRIPT_FILENAM = path;
    std::string SERVER_PROTOCOL = "HTTP/1.1";
    std::string SERVER_ADDR = "127.0.0.1";

    std::vector<std::string> environment;
    environment.push_back("REQUEST_METHOD=" + REQUEST_METHOD);
    environment.push_back("CONTENT_TYPE=" + CONTENT_TYPE);
    environment.push_back("CONTENT_LENGTH=" + CONTENT_LENGTH);
    environment.push_back("QUERY_STRING=" + QUERY_STRING);
    environment.push_back("SCRIPT_FILENAME="+ SCRIPT_FILENAM); 
    environment.push_back("SERVER_PROTOCOL=" + SERVER_PROTOCOL);
    environment.push_back("SERVER_ADDR=" + SERVER_ADDR);  // Update this IP address
    // environment.push_back("SERVER_PORT=80");  // Update this port number
}

void fastCGI(std::string &path)
{
    std::vector<std::string> environment;
    char* env[environment.size() + 1]; 
    for(size_t i = 0; i< environment.size();i++)
    {
        env[i] = const_cast<char*>(environment[i].c_str());
    }

    env[environment.size()] = NULL;

    pid_t cgi_pid;
    int fd = open("/tmp/tmpFile", O_WRONLY | O_CREAT ,777);
    if (fd == -1)
    {
        std::cerr << "Error opening file /tmp/tmpFile" << std::endl;
        exit(EXIT_FAILURE);
    }
    cgi_pid = fork();
    if (cgi_pid == 0)
    {
        dup2(fd, 1);
        const char *args[] = {"/usr/bin/php-cgi8.2", path.c_str(), NULL};
        close(fd);
        execve("/usr/bin/php-cgi8.2", const_cast<char* const*>(args), env);
        // perror("Error executing CGI");
        // exit(EXIT_FAILURE);
    } 
    else
        wait(NULL);
    close(fd);
}