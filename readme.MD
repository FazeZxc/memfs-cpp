# memFS - A Fast, In-Memory File System

## Overview

**memFS** (pronounced as _memphis_) is an in-memory file system designed for rapid read and write operations, ideal for applications that require high-speed data access. Unlike traditional disk-based file systems, memFS operates entirely in volatile memory (RAM), meaning all data is temporary and will be lost when the application is closed. This makes it perfect for caching, testing environments, and scenarios where data persistence is not critical.

## Features

- **Commands**: Supports common file operations such as `create`, `write`, `read`, `delete`, and `ls`.
- **Multithreading**: Optimized for operations on multiple files by utilizing multithreading for `create`, `write`, and `delete` commands.
- **Benchmarking**: Designed to support benchmarking for various workloads (100, 1000, and 10000 file operations) with different numbers of threads.
- **File Listing**: Displays the size, creation date, and last modified date for each file.

## Installation

### Prerequisites

- **C++11** or higher
- A POSIX-compliant environment (Linux, macOS, or Windows Subsystem for Linux (WSL) recommended)
- `make` for build automation

### Compilation

1. Clone this repository or download the files.
2. Run the following command in the project directory:

   ```bash
   make
   ```

3. This will compile the program and create an executable named `memFS`.

### Usage

To run the program, use the following command:

```bash
./memFS
```

This will open the memFS command line interface, where you can enter the following commands.

### Commands

- Create File(s): Creates one or more files.

  ```bash
  create filename
  create -n number_of_files filename1 filename2 ...
  ```

- Write to File(s): Writes content to a file or multiple files.

  ```bash
  write filename "content"
  write -n number_of_files filename1 "content1" filename2 "content2" ...
  ```

- Delete File(s): Deletes one or more files.

  ```bash
  delete filename
  delete -n number_of_files filename1 filename2 ...
  ```

- Read File(s): Reads the content of a file.
  ```bash
  read filename
  ```
- List Files: Lists all files in memFS. Add the -l flag for detailed info.
  ```bash
  ls
  ls -l
  ```
- Exit: Exits the memFS environment.
  ```bash
  exit
  ```

### Examples

    ```bash
    memFS> create todo.txt
    File created successfully.
    memFS> write todo.txt "Finish the project by 17th November"
    Successfully written to todo.txt.
    memFS> read todo.txt
    Finish the project by 17th November
    memFS> ls -l
    todo.txt 35 bytes Created: 06/11/2024 Last Modified: 06/11/2024
    memFS> delete todo.txt
    File deleted successfully.
    ```

### Benchmarking

    To evaluate the performance of memFS with different workloads, you can create test scripts or functions that call the create, write, read, and delete commands in specified amounts (100, 1000, and 10000 times) with different numbers of threads (1, 2, 4, 8, 16). Record the average latency and CPU/memory usage for each test.

### Files

- memfs.h: Header file for the MemFS class.
- memfs.cpp: Implementation file for the MemFS class.
- main.cpp: Main file containing the command interpreter and program entry point.
- Makefile: Makefile for compiling the project

Note: This project is designed for learning purposes and is not intended for production use, as data is stored only in RAM and will be lost upon closing the program.

```
This `README.md` provides a complete overview of the project, installation instructions, usage examples, and notes on benchmarking, fulfilling all essential documentation needs.
```
