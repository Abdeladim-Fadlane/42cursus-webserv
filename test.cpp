#include <iostream>
#include <cstdio>
#include <cstdlib>

int main() {
    // Replace "example.txt" with your file path
    const char* filePath = "webserv.hpp";

    // Open the file and get a file descriptor
    FILE* fileStream = fopen(filePath, "r");
    if (fileStream == NULL) {
        perror("Error opening the file");
        return 1;
    }

    // Read and print each line from the file
    char line[256];  // Adjust the buffer size as needed
    while (fgets(line, sizeof(line), fileStream) != NULL) {
        std::cout << "Line: " << line;
    }

    // Clean up
    fclose(fileStream);

    return 0;
}
