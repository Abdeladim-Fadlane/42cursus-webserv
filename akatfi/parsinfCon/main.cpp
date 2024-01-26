#include "Configfile.hpp"
using namespace std;


int main()
{
    try
    {
        ConfigFile config("file.config");

        config.parceConfig();
        // printserver(config);
        std::cout << "Well Done!" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}