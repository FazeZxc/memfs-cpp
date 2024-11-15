#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>

struct FileInfo {
    std::string content;
    int size;                       // File size in bytes
    std::string creationDate;       // Creation date
    std::string lastModifiedDate;   // Last modification date
};

std::unordered_map<std::string, FileInfo> memFS; // In-memory file system with metadata
std::mutex fs_mutex; // Mutex for thread-safe access

// Helper function to get current date in "dd/mm/yyyy" format
std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    std::ostringstream oss;
    oss << std::put_time(now_tm, "%d/%m/%Y");
    return oss.str();
}

// Function to create a single file in memory
void createFile(const std::string& filename) {
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
void createMultipleFiles(int num_files, const std::vector<std::string>& filenames) {
    if (num_files != filenames.size()) {
        std::cerr << "Error: The number of file names provided does not match the specified count (-n " 
                  << num_files << ").\n";
        return;
    }
    
    std::vector<std::thread> threads;
    for (const auto& filename : filenames) {
        threads.emplace_back(createFile, filename);
    }
    for (auto& t : threads) t.join();
}

// Function to write data to files in memory
void writeFile(const std::string& filename, const std::string& content) {
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
void deleteFiles(int num_files, const std::vector<std::string>& filenames) {
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
void readFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(fs_mutex);
    auto it = memFS.find(filename);
    if (it == memFS.end()) {
        std::cerr << "Error: File " << filename << " does not exist.\n";
    } else {
        std::cout << "Content of " << filename << ": " << it->second.content << "\n";
    }
}

// Function to list all files in memory with optional detailed format
void listFiles(bool longFormat = false) {
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

// Benchmark function to test performance
void benchmark() {
    int workloads[] = {100, 1000, 10000};
    int thread_counts[] = {1, 2, 4, 8, 16};
    
    // Map to store time taken for each workload and thread count
    std::map<int, std::map<int, long long>> times;

    for (int workload : workloads) {
        for (int thread_count : thread_counts) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            // Create files in parallel
            std::vector<std::thread> threads;
            for (int i = 0; i < workload; ++i) {
                std::string filename = "file" + std::to_string(i);
                threads.emplace_back(createFile, filename);
            }
            for (auto& t : threads) t.join();
            threads.clear();

            // Write to files in parallel
            for (int i = 0; i < workload; ++i) {
                std::string filename = "file" + std::to_string(i);
                std::string content = "This is content for " + filename;
                threads.emplace_back(writeFile, filename, content);
            }
            for (auto& t : threads) t.join();
            threads.clear();

            // Delete files in parallel
            for (int i = 0; i < workload; ++i) {
                std::string filename = "file" + std::to_string(i);
                threads.emplace_back(deleteFiles,1, std::vector<std::string>{filename});
            }
            for (auto& t : threads) t.join();

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            // Store the duration in the map
            times[workload][thread_count] = duration;

            // Output the benchmarking results
            std::cout << "Benchmark results for workload " << workload << " with " << thread_count << " threads:\n";
            std::cout << "Average Latency: " << duration / (3 * workload) << " ms\n";
        }
    }

    // Print the cumulative times for each workload and thread count
    std::cout << "\nSummary of Times for Each Workload and Thread Count:\n";
    for (const auto& workload_entry : times) {
        int workload = workload_entry.first;
        for (const auto& thread_entry : workload_entry.second) {
            int thread_count = thread_entry.first;
            long long duration = thread_entry.second;
            std::cout << "Workload: " << workload << ", Threads: " << thread_count 
                      << ", Time Taken: " << duration << " ms\n";
        }
    }
}

// Function to parse commands from user input
void parseCommand(const std::string& command) {
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
                createMultipleFiles(num_files, filenames);
            }
        } else {
            createFile(option);
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
                writeFile(filenames[i], contents[i]);
            }

            std::cout << "Successfully written to the given files.\n";
        } else {
            // Writing to a single file
            std::string filename = option;
            std::string content;
            std::getline(iss, content, '"');
            std::getline(iss, content, '"'); // Read text enclosed in quotes

            writeFile(filename, content);
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

        deleteFiles(num_files, filenames);
    } else {
        // Deleting a single file
        std::string filename = option;
        deleteFiles(1, {filename});
    }
}
    else if (cmd == "read") {
        std::string filename;
        iss >> filename;
        readFile(filename);
    } else if (cmd == "ls") {
        std::string option;
        iss >> option;
        bool longFormat = (option == "-l");
        listFiles(longFormat);
    } else if (cmd == "benchmark") {
        benchmark();
    } else if (cmd == "exit") {
        std::cout << "Exiting memFS.\n";
        exit(0);
    } else {
        std::cerr << "Unknown command: " << cmd << "\n";
    }
}

int main() {
    // Interactive mode
    std::string command;
    while (true) {
        std::cout << "memfs> ";
        std::getline(std::cin, command);
        parseCommand(command);
    }
    return 0;
}