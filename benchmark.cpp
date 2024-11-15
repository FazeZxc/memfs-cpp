#include "memfs.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <map>
#include <chrono>
#include <iomanip>
#include <ctime>

void benchmark(MemFS &fs) {
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
                threads.emplace_back([&fs, filename]() { // Capture fs by reference
                    fs.createFile(filename);
                });
            }
            for (auto& t : threads) t.join();
            threads.clear();

            // Write to files in parallel
            for (int i = 0; i < workload; ++i) {
                std::string filename = "file" + std::to_string(i);
                std::string content = "This is content for " + filename;
                threads.emplace_back([&fs, filename, content]() { // Capture fs by reference
                    fs.writeFile(filename, content);
                });
            }
            for (auto& t : threads) t.join();
            threads.clear();

            // Delete files in parallel
            for (int i = 0; i < workload; ++i) {
                std::string filename = "file" + std::to_string(i);
                threads.emplace_back([&fs, filename]() { // Capture fs by reference
                    fs.deleteFiles(1, std::vector<std::string>{filename});
                });
            }
            for (auto& t : threads) t.join();

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
            
            // Store the duration in the map
            times[workload][thread_count] = duration;

            // Output the benchmarking results
            std::cout << "Benchmark results for workload " << workload << " with " << thread_count << " threads:\n";
            std::cout << "Total Time: " << duration << " ms\n";
            std::cout << "Average Latency: " << static_cast<double>(duration) / (3 * workload) << " ms\n"; // Average over 3 operations
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

int main() {
    MemFS fs;
    benchmark(fs);
    return 0;
}