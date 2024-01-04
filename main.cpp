

#include "./Config/Parsing.hpp"

int main(int ac, char **av)
{
    if (ac != 2)  return (1);
    Conf config_file;
    config_file.setFileName(av[1]);
    config_file.parse();
    // config_file.print_all_data();

}
