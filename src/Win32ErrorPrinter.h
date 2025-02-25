#pragma once

#include <string>
#include <windows.h>
#include <cstdio>

inline void PrintWin32Error() {

        DWORD   dwErr = GetLastError();
        CHAR   wszMsgBuff[512];  // Buffer for text.
        DWORD   dwChars;  // Number of chars returned.

        // Try to get the message from the system errors.
        dwChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dwErr,
            0,
            wszMsgBuff,
            512,
            NULL);


        if ( dwChars ) {
            std::string message = std::to_string(dwErr) + ": " + std::string{wszMsgBuff};
            MessageBox(NULL, message.c_str(), TEXT("ERROR"), MB_OK | MB_ICONERROR);
        };
        // printf("Error value %d message: %s\n",
        //     dwErr, dwChars ? wszMsgBuff : "Error message not found\n");

}
