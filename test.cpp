#include <iostream>
#include <vector>
#include <utility>
#include <string>

struct ServerConfig {
    int listen;
    std::string host;
    std::string domainName;
    // ... other members ...
};

// Function to check if a server with the same domain name exists
bool isDuplicateDomain(const std::vector<ServerConfig>& servers, const std::string& domainName) {
    for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
        if (it->domainName == domainName) {
            return true; // Duplicate domain found
        }
    }
    return false; // No duplicate domain found
}

// Function to check if a server with the same port exists
bool isDuplicatePort(const std::vector<ServerConfig>& servers, int port) {
    for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
        if (it->listen == port) {
            return true; // Duplicate port found
        }
    }
    return false; // No duplicate port found
}

int main() {
    // Vector to store server configurations
    std::vector<ServerConfig> servers;

    // Example servers

    ServerConfig server1 = {8080, "localhost", "intra42.ma"};
    ServerConfig server2= {200, "localhost", "afadlane.1337.com"};
    ServerConfig server3 = {1099, "localhost", "google.com"};
    ServerConfig server4 = {2004, "localhost", "afadlane.1337.com"};

    // Check for duplicate domain name or port for each server
    if (isDuplicateDomain(servers, server1.domainName) || isDuplicatePort(servers, server1.listen)) {
        std::cerr << "Error: Duplicate domain or port found for server 1." << std::endl;
        return 1; // Exit with an error code
    }
    servers.push_back(server1);

    if (isDuplicateDomain(servers, server2.domainName) || isDuplicatePort(servers, server2.listen)) {
        std::cerr << "Error: Duplicate domain or port found for server 2." << std::endl;
        return 1; // Exit with an error code
    }
    servers.push_back(server2);

    if (isDuplicateDomain(servers, server3.domainName) || isDuplicatePort(servers, server3.listen)) {
        std::cerr << "Error: Duplicate domain or port found for server 3." << std::endl;
        return 1; // Exit with an error code
    }
    servers.push_back(server3);

    if (isDuplicateDomain(servers, server4.domainName) || isDuplicatePort(servers, server4.listen)) {
        std::cerr << "Error: Duplicate domain or port found for server 4." << std::endl;
        return 1; // Exit with an error code
    }
    servers.push_back(server4);

    // Continue with other operations...

    return 0;
}
