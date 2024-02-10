#include"afadlane/webserv.hpp"


int main()
{
    int fd = open("/nfs/homes/afadlane/tmp/file0",O_RDONLY);
    std::cout<<"fd= "<<fd<<"\n";
}