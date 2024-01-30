#include"webserv.hpp"
#include "../akatfi/parsinfCon/Configfile.hpp"

int main()
{
    try
    {
        // if(argc != 2)
        //     std::runtime_error("Error argments");
        // ConfigFile config(argv[1]);
        // config.parceConfig();
        multiplexing();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    } 
}