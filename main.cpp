#include <iostream>
#include <dirent.h>

int main() {
    const char* directoryPath = "/nfs/homes/akatfi/Desktop/coding_web/akatfi";  // Replace with your directory path

    DIR* directory = opendir(directoryPath);

    if (directory == nullptr) {
        std::cerr << "Error opening directory." << std::endl;
        return 1;
    }

    dirent* entry;
    while ((entry = readdir(directory)) != nullptr) {
        // Filter out "." and ".." entries
        // if (entry->d_name[0] != '.') {
            std::cout << "File name: " << entry->d_name << std::endl;
        // }
    }

    closedir(directory);

    return 0;
}