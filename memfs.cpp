#include "memfs.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <ctime>

bool MemFS::createFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock(mtx);
    if (files.find(filename) != files.end()) {
        std::cerr << "Error: " << filename << " already exists.\n";
        return false;
    }
    if (filename.empty()) {
        std::cerr << "Error: Invalid filename.\n";
        return false;
    }
    files[filename] = File{"", 0, std::time(nullptr), std::time(nullptr)};
    std::cout << "File " << filename << " created successfully.\n";
    return true;
}

bool MemFS::writeFile(const std::string &filename, const std::string &content) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = files.find(filename);
    if (it == files.end()) {
        std::cerr << "Error: " << filename << " does not exist.\n";
        return false;
    }
    it->second.content = content;
    it->second.size = content.size();
    it->second.modifiedTime = std::time(nullptr);
    std::cout << "Successfully written to " << filename << ".\n";
    return true;
}

bool MemFS::deleteFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock(mtx);
    if (files.erase(filename) == 0) {
        std::cerr << "Error: " << filename << " does not exist.\n";
        return false;
    }
    std::cout << "File " << filename << " deleted successfully.\n";
    return true;
}

std::string MemFS::readFile(const std::string &filename) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = files.find(filename);
    if (it == files.end()) {
        std::cerr << "Error: " << filename << " does not exist.\n";
        return "";
    }
    return it->second.content;
}

void MemFS::listFiles(bool detailed) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = files.begin(); it != files.end(); ++it) {
        const std::string &filename = it->first;
        const File &file = it->second;
        std::cout << filename;
        if (detailed) {
            std::cout << "\tCreated: " << std::put_time(std::localtime(&file.creationTime), "%Y-%m-%d %H:%M:%S")
              << "\tLast Modified: " << std::put_time(std::localtime(&file.modifiedTime), "%Y-%m-%d %H:%M:%S");
        }
        std::cout << "\n";
    }
}
