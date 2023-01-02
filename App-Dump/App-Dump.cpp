#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
//One Cpp to control them all
int main()
{
    // Get the path of the EXE from the user
    std::cout << "Enter the path of the EXE: ";
    std::wstring exePath;
    std::getline(std::wcin, exePath);

    std::wcout << L"Opening EXE file: " << exePath << std::endl;
    // Open the EXE file
    HANDLE hFile = CreateFile(exePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to open the EXE file. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Getting size of EXE file" << std::endl;
    // Get the size of the EXE file
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        std::cout << "Failed to get the size of the EXE file. Error code: " << GetLastError() << std::endl;
        CloseHandle(hFile);
        return 1;
    }

    std::cout << "Allocating memory for EXE file" << std::endl;
    // Read the EXE file into memory
    LPVOID lpFileData = VirtualAlloc(NULL, dwFileSize, MEM_COMMIT, PAGE_READWRITE);
    if (lpFileData == NULL)
    {
        std::cout << "Failed to allocate memory for the EXE file. Error code: " << GetLastError() << std::endl;
        CloseHandle(hFile);
        return 1;
    }

    std::cout << "Reading EXE file into memory" << std::endl;
    DWORD dwBytesRead;
    if (!ReadFile(hFile, lpFileData, dwFileSize, &dwBytesRead, NULL))
    {
        std::cout << "Failed to read the EXE file. Error code: " << GetLastError() << std::endl;
        VirtualFree(lpFileData, 0, MEM_RELEASE);
        CloseHandle(hFile);
        return 1;
    }

    // Close the EXE file
    std::cout << "Closing EXE file" << std::endl;
    CloseHandle(hFile);

    // Dump the EXE file data to a text file in hexadecimal format
    std::cout << "Dumping EXE file data to dump.txt" << std::endl;
    std::ofstream out("dump.txt");
    for (size_t i = 0; i < dwFileSize; i += 16)
    {
        // Print the memory address in hexadecimal format
        out << std::setfill('0') << std::setw(8) << std::hex << i << ": ";

        // Print the data in hexadecimal format
        for (size_t j = i; j < i + 16; ++j)
        {
            if (j < dwFileSize)
                out << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(reinterpret_cast<unsigned char*>(lpFileData)[j]) << " ";
            else
                out << "   ";
        }

        out << "  ";

        // Print the data as characters
        for (size_t j = i; j < i + 16; ++j)
        {
            if (j < dwFileSize)
            {
                unsigned char ch = reinterpret_cast<unsigned char*>(lpFileData)[j];
                if (std::isprint(ch))
                    out << ch;
                else
                    out << '.';
            }
        }

        out << std::endl;
    }
    out.close();

    // Free the allocated memory
    std::cout << "Freeing allocated memory" << std::endl;
    VirtualFree(lpFileData, 0, MEM_RELEASE);

    return 0;
}
