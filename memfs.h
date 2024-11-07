#ifndef MEMFS_H
#define MEMFS_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <ctime>

struct File {
    std::string content;
    size_t size;
    std::time_t creationTime;
    std::time_t modifiedTime;
};

class MemFS {
public:
    bool createFile(const std::string &filename);
    bool writeFile(const std::string &filename, const std::string &content);
    bool deleteFile(const std::string &filename);
    std::string readFile(const std::string &filename);
    void listFiles(bool detailed = false);
    
private:
    std::unordered_map<std::string, File> files;
    std::mutex mtx; 
};

#endif
