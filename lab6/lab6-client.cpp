#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <string>

#define BUFFER_SIZE 80
#define IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 8080

#define SHUTDOWN_SERVER "SHUTDOWN_SERVER\0"

void PrintMenu() {
    printf("Menu:\n");
    printf("1. Search prime numbers in given range\n");
    printf("2. Shutdown the server\n");
    printf("3. Shutdown the client\n");
    printf("Enter your choice: ");
}

void ListenToServer(SOCKET clientSocket) {
    int bytesReceived;
    char buffer[BUFFER_SIZE];
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        printf("Received from server: %s", buffer);
    }
}

void InitializeConsole(FILE*& input, FILE*& output, FILE*& error) {
    AllocConsole();
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

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        printf("Error: Socket initialization failed.\n");
        WSACleanup();
    }

    return clientSocket;
}

bool ConnectToServer(SOCKET clientSocket) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(serverAddr.sin_family, IP_ADDRESS, &serverAddr.sin_addr.s_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("Error: Connecting to server failed.\n");
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    return true;
}

void Cleanup(FILE* input, FILE* output, FILE* error, SOCKET clientSocket) {
    closesocket(clientSocket);
    WSACleanup();
    fclose(input);
    fclose(output);
    fclose(error);
    FreeConsole();
}

void SendPrimeNumbersRange(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    int start, end;
    printf("Enter \"x-y\": ");
    scanf_s("%d-%d", &start, &end);
    getchar();
    memset(buffer, NULL, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "%d-%d", start, end);
    send(clientSocket, buffer, (int)(strlen(buffer) + 1), 0);
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenToServer, (LPVOID)clientSocket, 0, NULL);
}

void ShutdownServer(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    strcpy_s(buffer, BUFFER_SIZE, SHUTDOWN_SERVER);
    size_t bufferLength = strlen(buffer);
    buffer[bufferLength] = '\0';
    send(clientSocket, buffer, (int)(bufferLength + 1), 0);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    FILE* input, * output, * error;
    InitializeConsole(input, output, error);

    SOCKET clientSocket = InitializeSocket();
    if (clientSocket == INVALID_SOCKET) return -1;

    //std::wstring wTitle = L"Client: " + std::to_wstring(clientSocket);
   // SetConsoleTitle(wTitle.c_str());

    if (!ConnectToServer(clientSocket)) return -1;

    void (*func_arr[])(SOCKET) = { SendPrimeNumbersRange, ShutdownServer };
    int choice;

    while (true)
    {
        PrintMenu();
        scanf_s("%d", &choice);
        getchar();

        if (choice == 1 || choice == 2)
        {
            func_arr[choice - 1](clientSocket);
        }
        else if (choice == 3) {
            break;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }

    Cleanup(input, output, error, clientSocket);
    return 0;
}
