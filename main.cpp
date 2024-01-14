#include"webserv.hpp"

int fastCGI() {
    pid_t cgi_pid;
    int fd = open("tools/index.html", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        std::cerr << "Error opening file 'index.html'" << std::endl;
        exit(EXIT_FAILURE);
    }

    cgi_pid = fork();

    if (cgi_pid == 0) {
        dup2(fd, 1);
        const char *args[] = {"/usr/bin/php-cgi8.2", "/home/afadlane/webserv/tools/phpinfo.php", NULL};
        char *env[] = {NULL};
        close(fd);
        execve("/usr/bin/php-cgi8.2", const_cast<char* const*>(args), env);
        perror("Error executing CGI");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(cgi_pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        } else {
            std::cerr << "Error executing CGI process" << std::endl;
        }
    }

    close(fd);
    return fd;
}

int main()
{
    ///here we go ///
    Method method;
    try
    {
        multiplexing(method);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}
