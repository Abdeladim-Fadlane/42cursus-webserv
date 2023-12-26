#include"webserv.hpp"
int paceUrl(std::string line,Get *object)
{
    std::istringstream iss(line);
    if (!(iss >> object->method >> object->path >> object->version)) {
        // An error occurred during parsing
        std::cerr << "Error parsing the request." << std::endl;
        return 0;
    }

    return(1);
}
int getMethod(int fd)
{
     Get object;
    std::string line;
    ssize_t bytesRead;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // Read until a complete HTTP request header is received
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        line += buffer;
        if (line.find("\r\n\r\n") != std::string::npos) {
            break; // Found the end of the header
        }
    }

    std::cout << "Received Header: " << line << std::endl;

    std::string firstLine;
    std::istringstream stream(line);

    // Extract the first line from the header
    std::getline(stream, firstLine);
    paceUrl(firstLine, &object);

    std::cout << "Method: " << object.method << std::endl;
    std::cout << "Path: " << object.path << std::endl;
    std::cout << "Version: " << object.version << std::endl;

    return 1;
}


// int main()