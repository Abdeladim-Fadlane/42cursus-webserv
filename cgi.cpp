#include"webserv.hpp"

int fastCGI()
{
    pid_t cgi_pid;
    int fd = open("/tmp/tmpFile", O_WRONLY | O_CREAT ,777);
    // std::cout<<"fd == "<<fd<<"\n";
    if (fd == -1)
    {
        std::cerr << "Error opening file /tmp/tmpFile" << std::endl;
        exit(EXIT_FAILURE);
    }
    cgi_pid = fork();
    if (cgi_pid == 0)
    {
        dup2(fd, 1);
        const char *args[] = {"/usr/bin/php-cgi8.2", "/home/afadlane/webserv/tools/utils/phpinfo.php", NULL};
        char *env[] = {NULL};
        close(fd);
        execve("/usr/bin/php-cgi8.2", const_cast<char* const*>(args), env);
        perror("Error executing CGI");
        exit(EXIT_FAILURE);
    } 
    else
    {
        // int status;
        // waitpid(cgi_pid, &status, 0);

        // if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // } else{
        //     std::cerr << "Error executing CGI process" << std::endl;
        // }
        wait(NULL);
    }
    // close(f)
    return fd;
}
