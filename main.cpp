#include "memfs.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <ctime>
#include <vector>

void executeCommand(MemFS &fs, const std::string &commandLine) {
    std::istringstream ss(commandLine);
    std::string command;
    ss >> command;

    if (command == "create") {
        std::string filename;
        std::vector<std::thread> threads;
        while (ss >> filename) {
            threads.emplace_back([&fs, filename] { fs.createFile(filename); });
        }
        for (auto &t : threads) {
            if (t.joinable()) t.join();
        }
    } else if (command == "write") {
        std::string filename, content;
        std::vector<std::thread> threads;
        while (ss >> filename) {
            std::getline(ss >> std::ws, content, '\"');
            threads.emplace_back([&fs, filename, content] { fs.writeFile(filename, content); });
        }
        for (auto &t : threads) {
            if (t.joinable()) t.join();
        }
    } else if (command == "delete") {
        std::string filename;
        std::vector<std::thread> threads;
        while (ss >> filename) {
            threads.emplace_back([&fs, filename] { fs.deleteFile(filename); });
        }
        for (auto &t : threads) {
            if (t.joinable()) t.join();
        }
    } else if (command == "read") {
        std::string filename;
        ss >> filename;
        std::cout << fs.readFile(filename) << "\n";
    } else if (command == "ls") {
        std::string option;
        bool detailed = false;
        if (ss >> option && option == "-l") {
            detailed = true;
        }
        fs.listFiles(detailed);
    } else if (command == "exit") {
        std::cout << "Exiting memFS.\n";
        exit(0);
    } else {
        std::cerr << "Invalid command!\n";
    }
}

int main() {
    MemFS fs;
    std::string commandLine;

    std::cout << "memFS> ";
    while (std::getline(std::cin, commandLine)) {
        executeCommand(fs, commandLine);
        std::cout << "memFS> ";
    }
    return 0;
}
