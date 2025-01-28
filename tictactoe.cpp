#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

void drawBoard(char board[3][3]) {
    cout << "-------------\n";
    for (int i = 0; i < 3; i++) {
        cout << "| ";
        for (int j = 0; j < 3; j++) {
            cout << board[i][j] << " | ";
        }
        cout << "\n-------------\n";
    }
}

bool checkWin(char board[3][3], char player) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
            return true;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player)
            return true;
    }
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player)
        return true;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player)
        return true;
    return false;
}

void serverMode(int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    cout << "Waiting for a connection...\n";
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    cout << "Connection established!\n";

    char board[3][3] = { { ' ', ' ', ' ' }, { ' ', ' ', ' ' }, { ' ', ' ', ' ' } };
    char player = 'X';
    int row, col, turn;

    for (turn = 0; turn < 9; turn++) {
        drawBoard(board);
        if (player == 'X') {
            while (true) {
                cout << "Your turn. Enter row and column: ";
                cin >> row >> col;
                if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') {
                    board[row][col] = player;
                    send(new_socket, &row, sizeof(row), 0);
                    send(new_socket, &col, sizeof(col), 0);
                    break;
                } else {
                    cout << "Invalid move. Try again.\n";
                }
            }
        } else {
            cout << "Waiting for opponent's move...\n";
            recv(new_socket, &row, sizeof(row), 0);
            recv(new_socket, &col, sizeof(col), 0);
            board[row][col] = player;
        }

        if (checkWin(board, player)) {
            drawBoard(board);
            cout << "Player " << player << " wins!\n";
            break;
        }
        player = (player == 'X') ? 'O' : 'X';
    }

    if (turn == 9 && !checkWin(board, 'X') && !checkWin(board, 'O')) {
        drawBoard(board);
        cout << "It's a draw!\n";
    }

    close(new_socket);
    close(server_fd);
}

void clientMode(const string& ip, int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported\n";
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed\n";
        return;
    }

    char board[3][3] = { { ' ', ' ', ' ' }, { ' ', ' ', ' ' }, { ' ', ' ', ' ' } };
    char player = 'O';
    int row, col, turn;

    for (turn = 0; turn < 9; turn++) {
        drawBoard(board);
        if (player == 'O') {
            while (true) {
                cout << "Your turn. Enter row and column: ";
                cin >> row >> col;
                if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') {
                    board[row][col] = player;
                    send(sock, &row, sizeof(row), 0);
                    send(sock, &col, sizeof(col), 0);
                    break;
                } else {
                    cout << "Invalid move. Try again.\n";
                }
            }
        } else {
            cout << "Waiting for opponent's move...\n";
            recv(sock, &row, sizeof(row), 0);
            recv(sock, &col, sizeof(col), 0);
            board[row][col] = player;
        }

        if (checkWin(board, player)) {
            drawBoard(board);
            cout << "Player " << player << " wins!\n";
            break;
        }
        player = (player == 'X') ? 'O' : 'X';
    }

    if (turn == 9 && !checkWin(board, 'X') && !checkWin(board, 'O')) {
        drawBoard(board);
        cout << "It's a draw!\n";
    }

    close(sock);
}

int main() {
    int choice, port;
    string ip;

    cout << "1. Host a game\n2. Join a game\nChoose an option: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Enter port to host the game: ";
        cin >> port;
        serverMode(port);
    } else if (choice == 2) {
        cout << "Enter server IP address: ";
        cin >> ip;
        cout << "Enter server port: ";
        cin >> port;
        clientMode(ip, port);
    } else {
        cout << "Invalid choice. Exiting.\n";
    }

    return 0;
}
