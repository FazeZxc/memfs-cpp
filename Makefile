# Makefile
CXX = g++
CXXFLAGS = -std=c++17 -pthread
TARGET = memFS

all: $(TARGET)

$(TARGET): main.cpp memfs.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp memfs.cpp

clean:
	rm -f $(TARGET)
