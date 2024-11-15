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
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Struct to store benchmark results for each configuration
struct Result {
    int workload;
    int thread_count;
    double avg_latency;
    double cpu_usage;
    size_t memory_usage;
};

// Placeholder for measuring CPU and memory utilization
void measureUtilization(double &cpuUsage, size_t &memoryUsage) {
    // Platform-specific logic for measuring CPU and memory usage
    cpuUsage = 0.0;  // Replace with actual CPU usage logic
    memoryUsage = 0; // Replace with actual memory usage logic
}

void performWorkload(MemFS &fs, int workload, int thread_count, double &avgLatency, double &cpuUsage, size_t &memoryUsage) {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    std::mutex mtx;
    int filesPerThread = workload / thread_count;

    // Create files
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&fs, &mtx, t, filesPerThread]() {
            for (int i = t * filesPerThread; i < (t + 1) * filesPerThread; ++i) {
                std::string filename = "file" + std::to_string(i);
                std::lock_guard<std::mutex> lock(mtx);
                fs.createFile(filename);
            }
        });
    }
    for (auto &t : threads) t.join();
    threads.clear();

    // Write to files
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&fs, &mtx, t, filesPerThread]() {
            for (int i = t * filesPerThread; i < (t + 1) * filesPerThread; ++i) {
                std::string filename = "file" + std::to_string(i);
                std::string content = "This is content for " + filename;
                std::lock_guard<std::mutex> lock(mtx);
                fs.writeFile(filename, content);
            }
        });
    }
    for (auto &t : threads) t.join();
    threads.clear();

    // Delete files
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&fs, &mtx, t, filesPerThread]() {
            for (int i = t * filesPerThread; i < (t + 1) * filesPerThread; ++i) {
                std::string filename = "file" + std::to_string(i);
                std::lock_guard<std::mutex> lock(mtx);
                fs.deleteFiles(1, std::vector<std::string>{filename});
            }
        });
    }
    for (auto &t : threads) t.join();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // Calculate average latency
    avgLatency = static_cast<double>(duration) / (3 * workload);

    // Measure CPU and memory utilization
    measureUtilization(cpuUsage, memoryUsage);
}

void benchmark(MemFS &fs) {
    int workloads[] = {100,1000 ,10000};
    int thread_counts[] = {1, 2, 4, 8, 16};
    
    std::vector<Result> results; // Vector to store all results

    for (int workload : workloads) {
        for (int thread_count : thread_counts) {
            double avgLatency = 0.0;
            double cpuUsage = 0.0;
            size_t memoryUsage = 0;

            performWorkload(fs, workload, thread_count, avgLatency, cpuUsage, memoryUsage);

            // Store the results in the vector
            results.push_back({workload, thread_count, avgLatency, cpuUsage, memoryUsage});
        }
    }

    // Print summary of all results
    std::cout << "\nSummary of Benchmark Results:\n";
    std::cout << std::setw(10) << "Workload" 
              << std::setw(15) << "Threads" 
              << std::setw(20) << "Avg Latency (ms)" 
              << std::setw(15) << "CPU Usage (%)" 
              << std::setw(20) << "Memory Usage (KB)\n";
    std::cout << std::string(80, '-') << "\n";
    
    for (const auto& result : results) {
        std::cout << std::setw(10) << result.workload
                  << std::setw(15) << result.thread_count
                  << std::setw(20) << std::fixed << std::setprecision(2) << result.avg_latency
                  << std::setw(15) << result.cpu_usage
                  << std::setw(20) << result.memory_usage / 1024 << "\n"; // Convert to KB
    }
}

int main() {
    MemFS fs;
    benchmark(fs);
    return 0;
}
