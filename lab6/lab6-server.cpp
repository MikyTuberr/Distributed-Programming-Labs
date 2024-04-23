#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <memory>

#define BUFFER_SIZE 512
#define MAX_CLIENTS 10
#define SERVER_PORT 8080

#define SHUTDOWN_SERVER "SHUTDOWN_SERVER\0"

struct ThreadArgs {
    int a, b;
    char clientId[10];
    SOCKET cSocket;
};

CRITICAL_SECTION cs;
CONDITION_VARIABLE cv;
SOCKET serverSocket;
bool isShuttingDown = false;
bool isFileOpen = false;

void SendToClient(SOCKET clientSocket, const char* message) {
    int bytesSent = send(clientSocket, message, (int)strlen(message) + 1, 0);
    if (bytesSent == SOCKET_ERROR) {
        printf("Error: Sending data to client failed.\n");
    }
}

bool IsPrime(int number) {
    if (number <= 1) return false;
    if (number == 2) return true;
    if (number % 2 == 0) return false;
    for (int i = 3; i <= sqrt(number); i += 2) {
        if (number % i == 0) return false;
    }
    return true;
}

void OpenFileAndWritePrime(FILE*& fileHandle, const char* clientId, int prime) {
    fopen_s(&fileHandle, "shared_file.txt", "a");
    if (fileHandle == NULL)
    {
        printf("Error: Opening file failed.\n");
        return;
    }
    isFileOpen = true;
    fprintf(fileHandle, "ClientId %s found prime number: %d\n", clientId, prime);
    fclose(fileHandle);
    isFileOpen = false;
}

void SendMessageAndPrint(const char* clientId, int prime, SOCKET cSocket) {
    char message[BUFFER_SIZE];
    sprintf_s(message, BUFFER_SIZE, "ClientId %s found prime number: %d\n", clientId, prime);
    SendToClient(cSocket, message);
    printf("ClientId %s found prime number: %d\n", clientId, prime);
}

void CalculatePrimes(ThreadArgs* args) {
    FILE* fileHandle;
    for (int i = args->a; i <= args->b; i++) {
        if (IsPrime(i) && !isShuttingDown) {
            EnterCriticalSection(&cs);
            OpenFileAndWritePrime(fileHandle, args->clientId, i);
            LeaveCriticalSection(&cs);
            WakeConditionVariable(&cv);
            SendMessageAndPrint(args->clientId, i, args->cSocket);
        }
    }
    delete args;
    args = nullptr;
}

ThreadArgs* ProcessBuffer(SOCKET clientSocket, char* buffer, const char* clientId) {
    ThreadArgs* args = nullptr;
    int start, end;
    if (sscanf_s(buffer, "%d-%d", &start, &end) == 2) {
        args = new ThreadArgs;
        args->a = start;
        args->b = end;
        strcpy_s(args->clientId, clientId);
        args->cSocket = clientSocket;
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CalculatePrimes, (LPVOID)args, 0, NULL);
        if (hThread == NULL) {
            delete args;
            args = nullptr;
        }
        else {
            CloseHandle(hThread);
        }
    }
    return args;
}

void ShutdownServerAndCleanup(SOCKET clientSocket, SOCKET serverSocket, ThreadArgs* args) {
    isShuttingDown = true;
    if (args != nullptr) {
        delete args;
        args = nullptr;
    }
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

void ProcessClient(SOCKET clientSocket)
{
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    char clientId[10];
    sprintf_s(clientId, "%d", (int)clientSocket);

    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        printf("Received from client %s: %s\n", clientId, buffer);
        ThreadArgs* args = ProcessBuffer(clientSocket, buffer, clientId);

        if (strcmp(buffer, SHUTDOWN_SERVER) == 0) {
            ShutdownServerAndCleanup(clientSocket, serverSocket, args);
            return;
        }
        memset(buffer, NULL, BUFFER_SIZE);
    }

    closesocket(clientSocket);
}

void InitializeConsole(FILE*& input, FILE*& output, FILE*& error) {
    AllocConsole();
    SetConsoleTitle(L"Server");
    freopen_s(&output, "CONOUT$", "w", stdout);
    freopen_s(&input, "CONIN$", "r", stdin);
    freopen_s(&error, "CONOUT$", "w", stderr);
}

SOCKET InitializeSocket() {
    WSADATA wsData;
    WORD version = MAKEWORD(2, 2);
    if (WSAStartup(version, &wsData) != 0)
    {
        printf("Error: WSA initialization failed.\n");
        return INVALID_SOCKET;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        printf("Error: Socket initialization failed.\n");
        WSACleanup();
    }

    return serverSocket;
}

bool BindAndListen(SOCKET serverSocket) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("Error: Binding socket failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR)
    {
        printf("Error: Listening on socket failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    return true;
}

void Cleanup(FILE* input, FILE* output, FILE* error, SOCKET serverSocket) {
    DeleteCriticalSection(&cs);
    fclose(input);
    fclose(output);
    fclose(error);
    FreeConsole();
    closesocket(serverSocket);
    WSACleanup();
}

int WINAPI WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    FILE* input, * output, * error;
    InitializeConsole(input, output, error);

    SOCKET serverSocket = InitializeSocket();
    if (serverSocket == INVALID_SOCKET) return -1;

    if (!BindAndListen(serverSocket)) return -1;

    InitializeCriticalSection(&cs);
    InitializeConditionVariable(&cv);

    SOCKET clientSocket;
    while (true)
    {
        clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            if (isShuttingDown) {
                printf("Server is shutting down.\n");
            }
            else printf("Error: Accepting client failed.\n");
            break;
        }
        printf("Client %lld accepted.\n", clientSocket);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessClient, (LPVOID)clientSocket, 0, NULL);
    }

    while (!isShuttingDown && isFileOpen) {
        SleepConditionVariableCS(&cv, &cs, INFINITE);
    }

    Cleanup(input, output, error, serverSocket);
    return 0;
}

