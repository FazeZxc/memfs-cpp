CXX = g++
CXXFLAGS = -std=c++17 -pthread
MEMFS_TARGET = memfs.exe
BENCHMARK_TARGET = benchmark.exe

# Source files for each target
MEMFS_SOURCES = memfs.cpp main.cpp
BENCHMARK_SOURCES = benchmark.cpp

# Object files for each target
MEMFS_OBJECTS = $(MEMFS_SOURCES:.cpp=.o)
BENCHMARK_OBJECTS = $(BENCHMARK_SOURCES:.cpp=.o)

all: $(MEMFS_TARGET) $(BENCHMARK_TARGET)

$(MEMFS_TARGET): $(MEMFS_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BENCHMARK_TARGET): $(BENCHMARK_OBJECTS) memfs.o  # Link memfs.o for benchmark
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MEMFS_TARGET) $(BENCHMARK_TARGET) $(MEMFS_OBJECTS) $(BENCHMARK_OBJECTS)