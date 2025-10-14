#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char source[256], dest[256];

    printf("Enter source file: ");
    if (scanf("%255s", source) != 1) {
        printf("Error reading source file name\n");
        return 1;
    }

    printf("Enter destination file: ");
    if (scanf("%255s", dest) != 1) {
        printf("Error reading destination file name\n");
        return 1;
    }

    // Open source file for reading
    HANDLE hSource = CreateFile(source, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSource == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Error opening source file: %lu\n", error);
        return 1;
    }

    // Create destination file for writing
    HANDLE hDest = CreateFile(dest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDest == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        printf("Error creating destination file: %lu\n", error);
        CloseHandle(hSource);
        return 1;
    }

    char buffer[1024];
    DWORD bytesRead, bytesWritten;
    BOOL readResult, writeResult;

    // Copy loop
    while ((readResult = ReadFile(hSource, buffer, sizeof(buffer), &bytesRead, NULL)) && bytesRead > 0) {
        writeResult = WriteFile(hDest, buffer, bytesRead, &bytesWritten, NULL);
        if (!writeResult || bytesWritten != bytesRead) {
            DWORD error = GetLastError();
            printf("Error writing to destination file: %lu\n", error);
            CloseHandle(hSource);
            CloseHandle(hDest);
            return 1;
        }
    }

    if (!readResult) {
        DWORD error = GetLastError();
        printf("Error reading from source file: %lu\n", error);
        CloseHandle(hSource);
        CloseHandle(hDest);
        return 1;
    }

    // Close handles
    CloseHandle(hSource);
    CloseHandle(hDest);

    printf("File copied successfully\n");
    return 0;
}
