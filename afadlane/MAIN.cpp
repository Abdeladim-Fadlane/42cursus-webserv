#include"webserv.hpp"

void LaunchingWebserver()
{
    const char* movements[] = {"... ", " .. ", ". ..", ".. ."};
    int i = 0;
    while (true)
    {
        std::cout << "\r";
        std::cout << "\033[1m\033[32mLaunching Webserver\033[0m";
        std::cout << movements[i % 4] << std::flush;
        usleep(200000);
        if( i == 20)
            break;
        i++;
    }
    std::cout << "\r";
}

int main(int argc,char *argv[])
{
    try
    {
        if(argc > 2)
            throw std::runtime_error("Error argments");
        if(argc == 1)
            argv[1] =const_cast<char*> ("file.conf");
        ConfigFile config(argv[1]);
        // LaunchingWebserver();
        config.parceConfig();
        multiplexing(config);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    } 
}