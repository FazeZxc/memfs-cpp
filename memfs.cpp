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

std::string MemFS::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    std::ostringstream oss;
    oss << std::put_time(now_tm, "%d/%m/%Y");
    return oss.str();
}

// Function to create a single file in memory
void MemFS::createFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(fs_mutex);
    if (memFS.find(filename) != memFS.end()) {
        std::cerr << "Error: File " << filename << " already exists.\n";
        return;
    }
    FileInfo newFile = {"", 0, getCurrentDate(), getCurrentDate()};
    memFS[filename] = newFile;
    std::cout << "File " << filename << " created successfully.\n";
}

// Function to create multiple files in memory
void MemFS::createMultipleFiles(int num_files, const std::vector<std::string>& filenames) {
    if (num_files != filenames.size()) {
        std::cerr << "Error: The number of file names provided does not match the specified count (-n " 
                  << num_files << ").\n";
        return;
    }
    
    std::vector<std::thread> threads;
    for (const auto& filename : filenames) {
        threads.emplace_back([this, filename]() {
                createFile(filename);
            });
    }
    for (auto& t : threads) t.join();
}

// Function to write data to files in memory
void MemFS::writeFile(const std::string& filename, const std::string& content) {
    std::lock_guard<std::mutex> lock(fs_mutex);
    auto it = memFS.find(filename);
    if (it == memFS.end()) {
        std::cerr << "Error: File " << filename << " does not exist.\n";
        return;
    }
    it->second.content = content;
    it->second.size = content.size();
    it->second.lastModifiedDate = getCurrentDate();
    std::cout << "Content written to " << filename << ".\n";
}

// Function to delete one or more files from memory
void MemFS::deleteFiles(int num_files, const std::vector<std::string>& filenames) {
    std::lock_guard<std::mutex> lock(fs_mutex);

    // Check if the number of files matches the specified count
    if (num_files != filenames.size()) {
        std::cerr << "Error: The number of filenames provided does not match the specified count (-n "
                  << num_files << ").\n";
        return;
    }

    bool all_files_deleted = true;
    bool any_errors = false;

    // Attempt to delete each file in the provided list
    for (int i = 0; i < num_files; ++i) {
        const std::string& filename = filenames[i];
        auto it = memFS.find(filename);

        if (it == memFS.end()) {
            // File does not exist
            std::cerr << "Error: File " << filename << " does not exist.\n";
            all_files_deleted = false;
            any_errors = true;
        } else {
            // File exists, delete it
            memFS.erase(it);
        }
    }

    if (any_errors) {
        std::cout << "Remaining files deleted successfully.\n";
    } else {
        std::cout << "Files deleted successfully.\n";
    }
}


// Function to read data from a file in memory
void MemFS::readFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(fs_mutex);
    auto it = memFS.find(filename);
    if (it == memFS.end()) {
        std::cerr << "Error: File " << filename << " does not exist.\n";
    } else {
        std::cout << "Content of " << filename << ": " << it->second.content << "\n";
    }
}

// Function to list all files in memory with optional detailed format
void MemFS::listFiles(bool longFormat) {
    std::lock_guard<std::mutex> lock(fs_mutex);
    if (!longFormat) {
        std::cout << "Listing files in memFS:\n";
        for (const auto& file : memFS) {
            std::cout << file.first << "\n";
        }
    } else {
        std::cout << "size\tcreated\t\tlast modified\tfilename\n";
        for (const auto& file : memFS) {
            const FileInfo& info = file.second;
            std::cout << info.size << "\t" 
                      << info.creationDate << "\t" 
                      << info.lastModifiedDate << "\t" 
                      << file.first << "\n";
        }
    }
}
