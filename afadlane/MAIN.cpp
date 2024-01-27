#include"webserv.hpp"
#include "../akatfi/parsinfCon/Configfile.hpp"

int main(int ac, char **av)
{
    try
    {
        ConfigFile config(av[1]);
        config.parceConfig();
        multiplexing();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    } 
}