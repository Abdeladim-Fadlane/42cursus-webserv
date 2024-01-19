#include"webserv.hpp"

// #include"../mkatfi/Server.hpp"
int main()
{
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