# Assignment 2: File Copy Program Using Windows API

## Step-by-Step Explanation of the Assignment

### 1. Understanding the Task
The assignment requires writing a C program called `filecopy.c` that copies files using the Windows API. The program must be compiled with MinGW/gcc, tested for various scenarios, traced for system calls using a Windows debugger, documented, and explained in a PDF.

### 2. Writing the Program
- **Code Structure**: The program uses Windows API functions like `CreateFile`, `ReadFile`, `WriteFile`, and `CloseHandle` to perform file copying.
- **Key Features**:
  - Prompts user for source and destination file paths.
  - Copies data in 1024-byte chunks.
  - Includes error handling for file operations.
- **Headers**: Includes `windows.h` for API, `stdio.h` for I/O, `stdlib.h` for utilities.

### 3. Compilation
- Compiled using `gcc filecopy.c -o filecopy.exe` with MinGW.
- Successful compilation produces `filecopy.exe`.

### 4. Testing Execution
- **Successful Copy**: Copies "test.txt" to "copy4.txt" - outputs "File copied successfully", but content appears garbled due to UTF-16 BOM interpretation in VSCode (original is UTF-8).
- **Empty File**: Copies "empty.txt" to "empty_copy2.txt" - succeeds, but BOM added.
- **Nonexistent Source**: Error code 2 (file not found).
- **Same File**: Error code 32 (sharing violation).
- **Access Denied**: Error code 5 (access denied to system directories).

### 5. Tracing System Calls with Debugger
- Use Windows debugger like WinDbg or Visual Studio Debugger.
- Attach to `filecopy.exe` process.
- Trace calls during execution:
  1. `CreateFile` for source (ntdll.dll!NtCreateFile).
  2. `CreateFile` for destination (ntdll.dll!NtCreateFile).
  3. `ReadFile` loop (ntdll.dll!NtReadFile).
  4. `WriteFile` loop (ntdll.dll!NtWriteFile).
  5. `CloseHandle` for both files (ntdll.dll!NtClose).

### 6. Documentation of System Calls
1. **NtCreateFile** (for source): Opens file for read access.
2. **NtCreateFile** (for destination): Creates/overwrites file for write access.
3. **NtReadFile**: Reads data chunks from source.
4. **NtWriteFile**: Writes data chunks to destination.
5. **NtClose**: Closes file handles.

### 7. Creating the PDF
- Write explanation in Markdown (`code_explanation.md`).
- Convert to PDF manually (e.g., using online tools or print to PDF from browser).

## Program Code Explanation

### Overview
This C program, `filecopy.c`, is designed to copy the contents of one file to another on a Windows system. It uses the Windows API for file operations, making it specific to Windows environments. The program prompts the user for the source and destination file paths, then performs the copy operation in chunks of 1024 bytes.

### Key Components

#### Headers and Includes
- `#include <windows.h>`: Includes the Windows API headers, providing access to functions like `CreateFile`, `ReadFile`, `WriteFile`, and `CloseHandle`.
- `#include <stdio.h>`: Standard I/O library for `printf` and `scanf`.
- `#include <stdlib.h>`: Standard library for general utilities (though not heavily used here).

#### Main Function
The `main` function is the entry point of the program.

##### Variable Declarations
- `char source[256]`: Array to store the source file path (up to 255 characters).
- `char dest[256]`: Array to store the destination file path (up to 255 characters).

##### User Input
- Prompts the user to enter the source file name using `printf` and reads it with `scanf`.
- Similarly prompts for the destination file name.
- Error handling: If `scanf` fails to read input, prints an error message and exits with code 1.

##### File Opening
- **Source File**: Opens the source file for reading using `CreateFile` with `GENERIC_READ` access. If it fails, retrieves the error code with `GetLastError` and prints it, then exits.
- **Destination File**: Creates or opens the destination file for writing using `CreateFile` with `GENERIC_WRITE` access and `CREATE_ALWAYS` disposition (overwrites if exists). Similar error handling.

##### Copy Loop
- Uses a loop to read from the source and write to the destination in 1024-byte chunks.
- `ReadFile`: Reads up to 1024 bytes into the `buffer` array, storing the number of bytes read in `bytesRead`.
- `WriteFile`: Writes the read bytes to the destination, storing bytes written in `bytesWritten`.
- Checks if the write operation succeeded and if all bytes were written; if not, prints an error and exits.
- Continues until `ReadFile` returns false or `bytesRead` is 0.

##### Error Handling in Loop
- After the loop, checks if `ReadFile` failed (not due to end-of-file); if so, prints an error and exits.

##### Cleanup
- Closes both file handles using `CloseHandle`.
- Prints "File copied successfully" and returns 0.

### Error Handling
- Uses `GetLastError` to retrieve Windows error codes for file operations.
- Exits with code 1 on any failure, providing error messages to the user.

### Limitations
- Windows-specific: Relies on Windows API, won't compile/run on other OSes.
- Text-based input: Assumes file paths without spaces or special characters; `scanf` with `%255s` stops at whitespace.
- No progress indication: For large files, no feedback during copy.
- Overwrites destination: Always creates/overwrites the destination file.

### Compilation and Execution
- Compile with a C compiler supporting Windows API, e.g., `gcc filecopy.c -o filecopy.exe`.
- Run the executable; it will prompt for source and destination paths.
- Example: Source: `test.txt`, Destination: `copy.txt`.

### Potential Improvements
- Add command-line arguments for file paths.
- Handle file paths with spaces.
- Add progress bar for large files.
- Cross-platform support using conditional compilation.
- Better input validation and error messages.
