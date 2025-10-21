#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char used[7] = {0}; // 0 CreateFile, 1 GetLastError, 2 ReadFile, 3 WriteFile, 4 CloseHandle, 5 FormatMessage, 6 LocalFree
void PrintError(DWORD error) {
    used[5] = 1;
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("Error: %s\n", (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
    used[6] = 1;
}
int main() {
    char source[256], dest[256];
    char logBuffer[2048] = "";  // Buffer to accumulate log content
    DWORD lastError = 0;  // To track errors
    printf("Enter source file: ");
    if (!fgets(source, sizeof(source), stdin)) {
        printf("Error reading source file name\n");
        strcat(logBuffer, "Error: Failed to read source file name from input.\n");
        lastError = 1;
    } else {
        source[strcspn(source, "\n")] = 0;  // Remove newline
        if (strlen(source) == 0) {
            printf("Source file name cannot be empty\n");
            strcat(logBuffer, "Error: Source file name is empty.\n");
            lastError = 1;
        }
    }
    printf("Enter destination file: ");
    if (!fgets(dest, sizeof(dest), stdin)) {
        printf("Error reading destination file name\n");
        strcat(logBuffer, "Error: Failed to read destination file name from input.\n");
        lastError = 1;
    } else {
        dest[strcspn(dest, "\n")] = 0;  // Remove newline
        if (strlen(dest) == 0) {
            printf("Destination file name cannot be empty\n");
            strcat(logBuffer, "Error: Destination file name is empty.\n");
            lastError = 1;
        }
    }
    // Open source file for reading
    HANDLE hSource = INVALID_HANDLE_VALUE;
    if (lastError == 0) {
        hSource = CreateFile(source, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        used[0] = 1;
        if (hSource == INVALID_HANDLE_VALUE) {
            lastError = GetLastError();
            used[1] = 1;
            printf("Error opening source file: ");
            PrintError(lastError);
            char errorMsg[256];
            sprintf(errorMsg, "Error opening source file (%s): %lu\n", source, lastError);
            strcat(logBuffer, errorMsg);
        }
    }
    // Open destination file for writing
    HANDLE hDest = INVALID_HANDLE_VALUE;
    if (lastError == 0) {
        hDest = CreateFile(dest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        used[0] = 1;
        if (hDest == INVALID_HANDLE_VALUE) {
            lastError = GetLastError();
            used[1] = 1;
            printf("Error opening destination file: ");
            PrintError(lastError);
            char errorMsg[256];
            sprintf(errorMsg, "Error opening destination file (%s): %lu\n", dest, lastError);
            strcat(logBuffer, errorMsg);
        }
    }
    // Copy file contents
    if (lastError == 0) {
        char buffer[1024];
        DWORD bytesRead, bytesWritten;
        while (ReadFile(hSource, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
            used[2] = 1;
            if (!WriteFile(hDest, buffer, bytesRead, &bytesWritten, NULL) || bytesWritten != bytesRead) {
                lastError = GetLastError();
                used[1] = 1;
                printf("Error writing to destination file\n");
                char errorMsg[256];
                sprintf(errorMsg, "Error writing to destination file: %lu\n", lastError);
                strcat(logBuffer, errorMsg);
                break;
            }
            used[3] = 1;
        }
        if (lastError == 0) {
            used[2] = 1;
            used[3] = 1;
        }
    }
    // Close handles
    if (hSource != INVALID_HANDLE_VALUE) {
        CloseHandle(hSource);
        used[4] = 1;
    }
    if (hDest != INVALID_HANDLE_VALUE) {
        CloseHandle(hDest);
        used[4] = 1;
    }
    // Write log to file
    if (lastError == 0) {
        printf("File copied successfully.\n");
    } else {
        printf("File copy failed.\n");
    }
    // Write the numbered list of system calls to system_calls_log.txt
    HANDLE hLog = CreateFile("system_calls_log.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    used[0] = 1;
    if (hLog != INVALID_HANDLE_VALUE) {
        char logContent[4096] = "";
        char *calls[7] = {"CreateFile", "GetLastError", "ReadFile", "WriteFile", "CloseHandle", "FormatMessage", "LocalFree"};
        int callCount = 0;
        for (int i = 0; i < 7; i++) {
            if (used[i]) {
                callCount++;
                char line[512];
                sprintf(line, "%d. %s: ", callCount, calls[i]);
                if (strcmp(calls[i], "CreateFile") == 0) {
                    strcat(line, "Creates or opens a file or I/O device, returning a handle for subsequent operations like reading or writing.\n");
                } else if (strcmp(calls[i], "GetLastError") == 0) {
                    strcat(line, "Retrieves the last error code set by the operating system for the current thread, useful for diagnosing failures in API calls.\n");
                } else if (strcmp(calls[i], "ReadFile") == 0) {
                    strcat(line, "Reads data from a file or input device into a buffer, specifying the number of bytes to read and returning the actual bytes read.\n");
                } else if (strcmp(calls[i], "WriteFile") == 0) {
                    strcat(line, "Writes data from a buffer to a file or output device, specifying the number of bytes to write and returning the actual bytes written.\n");
                } else if (strcmp(calls[i], "CloseHandle") == 0) {
                    strcat(line, "Closes an open object handle, such as a file handle, to free system resources and ensure data is flushed.\n");
                } else if (strcmp(calls[i], "FormatMessage") == 0) {
                    strcat(line, "Formats a message string from a message definition, often used to convert error codes into human-readable strings.\n");
                } else if (strcmp(calls[i], "LocalFree") == 0) {
                    strcat(line, "Frees a local memory object allocated by LocalAlloc, releasing the memory back to the system.\n");
                }
                strcat(logContent, line);
            }
        }
        DWORD bytesWritten;
        WriteFile(hLog, logContent, strlen(logContent), &bytesWritten, NULL);
        used[3] = 1;
        CloseHandle(hLog);
        used[4] = 1;
    }
    return lastError == 0 ? 0 : 1;
}

