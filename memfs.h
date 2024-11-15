#ifndef MEMFS_H
#define MEMFS_H

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <mutex>
#include <unordered_map>

class MemFS {
public:
    // Structure to represent file metadata
    struct FileInfo {
        std::string content;
        int size;                       // File size in bytes
        std::string creationDate;       // Creation date
        std::string lastModifiedDate;   // Last modification date

    };

    
    // Helper function to get current date in "dd/mm/yyyy" format
    std::string getCurrentDate(); 

    // File operations for multi-threading support
    void createFile(const std::string& filename);
    void createMultipleFiles(int num_files, const std::vector<std::string>& filenames);

    void writeFile(const std::string& filename, const std::string& content);
    void deleteFiles(int num_files, const std::vector<std::string>& filenames);
    
    // Single-threaded file operations

    void readFile(const std::string& filename);
    void listFiles(bool longFormat = false);

    private:
    std::unordered_map<std::string, FileInfo> memFS; // In-memory file system with metadata
    std::mutex fs_mutex; // Mutex for thread-safe access

};

#endif // MEMFS_H