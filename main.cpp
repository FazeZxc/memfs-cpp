#include "memfs.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <ctime>

void parseCommand(MemFS &fs,const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "create") {
        std::string option;
        iss >> option;

        if (option == "-n") {
            int num_files;
            iss >> num_files;

            if (num_files <= 0) {
                std::cerr << "Error: The number of files (-n) must be a positive integer.\n";
                return;
            }

            std::vector<std::string> filenames;
            std::string filename;
            while (iss >> filename) {
                filenames.push_back(filename);
            }

            if (filenames.size() != num_files) {
                std::cerr << "Error: The number of file names provided does not match the specified count (-n " 
                          << num_files << ").\n";
            } else {
                fs.createMultipleFiles(num_files, filenames);
            }
        } else {
            fs.createFile(option);
        }
    } 
    else if (cmd == "write") {
        std::string option;
        iss >> option;

        if (option == "-n") {
            // Writing to multiple files
            int num_files;
            iss >> num_files;
            if (num_files <= 0) {
                std::cerr << "Error: The number of files (-n) must be a positive integer.\n";
                return;
            }

            std::vector<std::string> filenames;
            std::vector<std::string> contents;
            std::string filename, content;

            for (int i = 0; i < num_files; ++i) {
                iss >> filename;
                std::getline(iss, content, '"');
                std::getline(iss, content, '"'); // Read text enclosed in quotes
                filenames.push_back(filename);
                contents.push_back(content);
            }

            // Write to each file in the list
            for (int i = 0; i < num_files; ++i) {
                fs.writeFile(filenames[i], contents[i]);
            }

            std::cout << "Successfully written to the given files.\n";
        } else {
            // Writing to a single file
            std::string filename = option;
            std::string content;
            std::getline(iss, content, '"');
            std::getline(iss, content, '"'); // Read text enclosed in quotes

            fs.writeFile(filename, content);
        }
    } 
    else if (cmd == "delete") {
    std::string option;
    iss >> option;

    if (option == "-n") {
        // Deleting multiple files
        int num_files;
        iss >> num_files;

        if (num_files <= 0) {
            std::cerr << "Error: The number of files (-n) must be a positive integer.\n";
            return;
        }

        std::vector<std::string> filenames;
        std::string filename;
        while (iss >> filename) {
            filenames.push_back(filename);
        }

        fs.deleteFiles(num_files, filenames);
    } else {
        // Deleting a single file
        std::string filename = option;
        fs.deleteFiles(1, {filename});
    }
}
    else if (cmd == "read") {
        std::string filename;
        iss >> filename;
        fs.readFile(filename);
    } else if (cmd == "ls") {
        std::string option;
        iss >> option;
        bool longFormat = (option == "-l");
        fs.listFiles(longFormat);
    } else if (cmd == "exit") {
        std::cout << "Exiting memFS.\n";
        exit(0);
    } else {
        std::cerr << "Unknown command: " << cmd << "\n";
    }
}

int main() {
    MemFS fs;
    std::string command;
    while (true) {
        std::cout << "memfs> ";
        std::getline(std::cin, command);
        parseCommand(fs, command);
    }
    return 0;
}