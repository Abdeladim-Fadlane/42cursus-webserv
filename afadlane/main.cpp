#include"webserv.hpp"


int main()
{
    try
    {
        multiplexing();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    } 
}