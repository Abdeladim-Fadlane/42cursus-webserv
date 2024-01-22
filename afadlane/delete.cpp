
#include"webserv.hpp"
using namespace std;

void   deleteMethod(int fd ,const char *path)
{
    /* success 204 No Content*/
    std::string html = "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n" ;
    std::string htttpresponce = "<center><h1>403 Forbidden</h1></center>"; 
    if(access(path,W_OK) != 0)
    {
        write(fd,htttpresponce.c_str(),htttpresponce.size());
        return;
    }
    /* <center><h1>409 Conflict</h1></center> */
    /* if path di=ont end with / */
    DIR *dir = opendir(path);
    if(!dir)
    {
        htttpresponce.clear();
        htttpresponce = "<p>Error opning dir</p>";
        write(fd,htttpresponce.c_str(),htttpresponce.size());
        return ;
    }
    struct dirent * it;
    struct stat statInfo;

    while((it = readdir(dir)) != NULL)
    {
        std::string itPath  = std::string(path) + "/" + it->d_name;
        if(strcmp(it->d_name,".") != 0 && strcmp(it->d_name,"..") != 0)
        {
            if(stat(itPath.c_str(),&statInfo) == -1)
            {
                htttpresponce.clear();
                htttpresponce = "<p>Error statting  dir</p>";
                write(fd,htttpresponce.c_str(),htttpresponce.size());
                closedir(dir);
                return ;
            }
            if(S_ISDIR(statInfo.st_mode))
            {
                deleteMethod(fd,itPath.c_str());
            }
            else
            {
                if(unlink(itPath.c_str()) == -1)
                {
                    htttpresponce.clear();
                    htttpresponce = "<p>Error removing file </p>";
                    write(fd,htttpresponce.c_str(),htttpresponce.size());
                }
            }
        }
    }
    closedir(dir);
    if (rmdir(path) == -1)
    {
        htttpresponce.clear();
        htttpresponce = "<p>Error removing directory </p>";
        write(fd,htttpresponce.c_str(),htttpresponce.size());
    }
    write(fd,htttpresponce.c_str(),htttpresponce.size());
    return ;
}

int main()
{
    const char* path = "test";
    deleteMethod(1,path);
}