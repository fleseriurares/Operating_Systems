# Operating Systems Assignments🖥️

This repository contains three assignments for the Operating Systems course. Each assignment covers different aspects of operating systems and low-level programming.

---

### Assignment 1: File System Module📄

#### Objective

The objective of this assignment is to implement a file system module that can read and manipulate a specific binary file format called SF (Section File) format.

#### Assignment Description

We are given an SF file format that consists of a header and a body. The header contains information about the file, while the body contains sections with text data. The goal is to implement a C program that performs various operations on SF files.

#### Requirements

1. **Compile and Run**: The C program must compile without errors and provide the minimum required functionality.
2. **Display Variant**: Output the identifier of the assignment variant.
3. **List Directory Contents**: Display names of elements in a directory based on filtering criteria.
4. **Parse Section Files**: Check if a file complies with the SF format and display relevant information.
5. **Extract Section Line**: Find and display a specific line from a section of an SF file.
6. **Find Certain SF Files**: Search for SF files with specific section criteria.

---

### Assignment 2: Processes and Threads🔄🚦

#### Objective

The objective of this assignment is to create Linux processes and threads and synchronize their executions. The synchronization will affect the order in which messages are displayed.

#### Assignment Description

We need to create Linux processes and threads and synchronize their executions using provided helper functions. The main process and all created processes and threads must call the `info()` function at their start and before their end with specific arguments.

#### Requirements

1. **Compile and Run**: The C program must compile without errors and provide the minimum required functionality.
2. **Process and Thread Naming Convention**: Use unique identifiers for processes and threads as specified.
3. **Process Hierarchy**: Generate a specific process hierarchy.
4. **Synchronizing Threads**: Implement synchronization mechanisms to control the execution order of threads.
5. **Threads Barrier**: Ensure a maximum number of threads are running simultaneously.
6. **Synchronizing Threads from Different Processes**: Implement synchronization between threads of different processes.

---

### Assignment 3: Inter-Process Communication Mechanisms🔗

#### Objective

The objective of this assignment is to implement inter-process communication mechanisms using named pipes and shared memory.

#### Assignment Description

We need to implement a C program that communicates with a testing program using named pipes and handles various requests related to shared memory and file mapping.

#### Requirements

1. **Compile and Run**: The C program must compile without errors and provide the minimum required functionality.
2. **Pipe-Based Connection**: Establish a communication with the tester using named pipes.
3. **Variant Request**: Handle a request to display the assignment variant.
4. **Create Shared Memory**: Handle a request to create a shared memory region.
5. **Write to Shared Memory**: Handle a request to write data to the shared memory region.
6. **Memory-Map File**: Handle a request to map a file into memory.
7. **Read from File Offset**: Handle a request to read data from a specific offset in the memory-mapped file.
8. **Read from File Section**: Handle a request to read data from a specific section of the memory-mapped file.
9. **Read from Logical Memory Space Offset**: Handle a request to read data from a logical memory space offset in the memory-mapped file.
10. **Exit Request**: Handle a request to terminate the program and close the pipes.

