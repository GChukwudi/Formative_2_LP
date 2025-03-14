# System Calls and Multithreading Performance Analysis

This repository contains solutions for an assessment on ELF binary debugging, system call tracing, and performance analysis using multithreading and synchronization.

## Table of Contents
- [Requirements](#requirements)
- [Compilation](#compilation)
- [Question 1: File and Process Operations](#question-1-file-and-process-operations)
- [Question 2: Buffered vs Unbuffered File Copy](#question-2-buffered-vs-unbuffered-file-copy)
- [Question 3: Multithreaded Prime Number Counter](#question-3-multithreaded-prime-number-counter)
- [Question 4: Multithreaded File Copy](#question-4-multithreaded-file-copy)
- [Performance Analysis](#performance-analysis)

## Requirements

- Linux operating system
- GCC compiler
- POSIX threads library
- strace utility for system call tracing

## Compilation

## Question 1: File and Process Operations

### Program Description
The program `file_process.c` demonstrates basic file operations (open, read, write, close) and process operations (fork, exec, wait).

### Compilation
```
gcc -o file_process file_process.c 
```

### Execution
```
./file_process
```

### Analyzing System Calls with strace
To analyze system calls:
```
strace -e trace=open,read,write,close,fork,execve,wait4 ./file_process
```

### Key System Calls

#### File Operations:
- `open`: Opens a file and returns a file descriptor
- `read`: Reads from a file descriptor into a buffer
- `write`: Writes from a buffer to a file descriptor
- `close`: Closes a file descriptor

#### Process Operations:
- `fork`: Creates a new process by duplicating the calling process
- `execve`: Executes a program
- `wait4`: Waits for a child process to terminate

## Question 2: Buffered vs Unbuffered File Copy

### Program Description
Two programs demonstrate file copying:
1. `unbuffered_copy.c`: Uses low-level I/O functions (open, read, write, close)
2. `buffered_copy.c`: Uses standard library I/O functions (fopen, fread, fwrite, fclose)

### Compilation
```
gcc -o unbuffered_copy unbuffered_copy.c
gcc -o buffered_copy buffered_copy.c
```

### Execution
```
./unbuffered_copy source_file destination_file
./buffered_copy source_file destination_file
```

### Analyzing System Calls
```
strace -c ./unbuffered_copy source_file destination_file
strace -c ./buffered_copy source_file destination_file
```

### Performance Comparison
To compare performance:

```
time ./unbuffered_copy large_file.txt copy1.txt
time ./buffered_copy large_file.txt copy2.txt
```

### Key Differences
- Unbuffered I/O makes direct system calls for each read/write operation
- Buffered I/O uses internal buffers to reduce the number of system calls
- Buffered I/O typically shows fewer system calls but has increased memory usage
- Performance varies based on file size and buffer configuration

## Question 3: Multithreaded Prime Number Counter

### Program Description
The program `prime_threads.c` uses 20 threads to count prime numbers between 0 and 10,000.

### Compilation
```
gcc -o prime_threads prime_threads.c -pthread -lm
```

### Execution
```
./prime_threads
```

### Implementation Details
- Each thread is assigned an equal-sized segment of the range
- A mutex is used to safely update the global prime count
- The program combines results from all threads for the final count

## Question 4: Multithreaded File Copy

### Program Description
The program `mulitthread_copier.c` uses multiple threads to copy a large file by dividing it into chunks.

### Execution
```
./mulitthread_copier source_file destination_file num_threads
```

### Performance Analysis
To analyze performance with different thread counts:

```
time ./mulitthread_copier big.txt copy_2.txt 2
time ./mulitthread_copier big.txt copy_4.txt 4
time ./mulitthread_copier big.txt copy_8.txt 8
time ./mulitthread_copier big.txt copy_16.txt 16
```

### Implementation Details
- File size is determined and divided into equal-sized chunks
- Each thread processes a separate chunk
- The program positions file pointers using lseek() for each thread
- Performance varies based on the number of threads, file size, and system resources

## Performance Analysis

### System Call Analysis
- Use `strace -c` to count system calls:
  ```
  strace -c ./program
  ```

### Thread Performance
- Analyze thread performance using different thread counts
- The optimal number of threads depends on:
  - CPU cores available
  - I/O bottlenecks
  - Overhead of thread creation and synchronization

### Memory Usage Analysis
- Monitor memory usage:
  ```
  /usr/bin/time -v ./program
  ```

### Additional Notes
- System call overhead is significant for small, frequent operations
- Thread synchronization overhead increases with more threads
- I/O bound operations benefit less from multiple threads than CPU bound operations