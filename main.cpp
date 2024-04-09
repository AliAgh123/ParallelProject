#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
using namespace std;
using namespace std::chrono;

vector<vector<int>> col;
vector<vector<int>> row;
vector<vector<int>> boxes;
bool wrongValue = false;

bool findUnassignedLocation(vector<vector<int>> &board) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] == 0) return false;
        }
    }
    return true;
}

int boxIdentification(int i, int j, int n) {
    int boxSize = sqrt(n);
    int boxRow = i / boxSize;
    int boxCol = j / boxSize;
    return boxRow * boxSize + boxCol;
}

void fillArrays(vector<vector<int>> &board) {
    int n = board.size();
    col.resize(n, vector<int>(n, 0));
    row.resize(n, vector<int>(n, 0));
    boxes.resize(n, vector<int>(n, 0));

    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] != 0) {
                int num = board[i][j] - 1;
                row[i][num]++;
                col[j][num]++;
                boxes[boxIdentification(i, j, n)][num]++;
            }
        }
    }
}

bool isValid(int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (col[i][j] > 1 || row[i][j] > 1 || boxes[i][j] > 1) return false;
        }
    }
    return true;
}

void solve(vector<vector<int>> &board) {
    if (findUnassignedLocation(board)) {
        return;
    }

    if (!isValid(board.size())) {
        return;
    }

    for (int y = 0; y < board.size(); y++) {
        for (int x = 0; x < board[y].size(); x++) {
            if (board[y][x] != 0) continue;

            for (int i = 0; i < board.size(); i++) { // n choices (n = board.size)
                if (col[x][i] == 0 && row[y][i] == 0 && boxes[boxIdentification(y, x, board.size())][i] == 0) {
                    col[x][i]++;
                    row[y][i]++;
                    boxes[boxIdentification(y, x, board.size())][i]++;
                    board[y][x] = i + 1;
                    solve(board);
                    if (wrongValue) {
                        col[x][i]--;
                        row[y][i]--;
                        boxes[boxIdentification(y, x, board.size())][i]--;
                        board[y][x] = 0;
                        wrongValue = false;
                    }
                }
                if (i == board.size() - 1 && board[y][x] == 0) {
                    wrongValue = true;
                    return;
                }
            }
        }
    }
}

void solveSudoku(vector<vector<int>> &board) {
    fillArrays(board);
    solve(board);
}

void displayBoard(const vector<vector<int>> &board) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            cout << board[i][j] << " ";
            if ((j + 1) % int(sqrt(board.size())) == 0 && j != board.size() - 1) cout << "| ";
        }
        cout << endl;
        if ((i + 1) % int(sqrt(board.size())) == 0 && i != board.size() - 1) {
            for (int k = 0; k < board.size() + int(sqrt(board.size())) - 1; k++) {
                cout << "--";
            }
            cout << endl;
        }
    }
}

int main() {

    // A 10 B 11 C 12 D 13 E 14 F 15 G 16
    vector<vector<int>> board {
            {0, 15, 0, 1, 0, 2, 10, 14, 12, 0, 0, 0, 0, 0, 0, 0},
            {0, 6, 3, 16, 12, 0, 8, 4, 14, 15, 1, 0, 2, 0, 0, 0},
            {14, 0, 9, 7, 11, 3, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {4, 13, 2, 12, 0, 0, 0, 0, 6, 0, 0, 0, 0, 15, 0, 0},
            {0, 0, 0, 0, 14, 1, 0, 0, 3, 5, 10, 0, 0, 8, 0, 12},
            {3, 16, 0, 0, 2, 4, 0, 0, 0, 14, 7, 13, 0, 0, 5, 15},
            {11, 0, 5, 0, 0, 0, 0, 0, 0, 9, 4, 0, 0, 6, 0, 0},
            {0, 0, 0, 0, 13, 0, 16, 5, 15, 0, 0, 12, 0, 0, 0, 0},
            {0, 0, 0, 0, 9, 0, 1, 12, 0, 8, 3, 10, 11, 0, 15, 0},
            {2, 12, 0, 11, 0, 0, 14, 3, 5, 4, 0, 0, 0, 0, 9, 0},
            {6, 3, 0, 4, 0, 0, 13, 0, 0, 11, 9, 1, 0, 12, 16, 2},
            {0, 0, 10, 9, 0, 0, 0, 0, 0, 0, 12, 0, 8, 0, 6, 7},
            {12, 8, 0, 0, 16, 0, 0, 10, 0, 13, 0, 0, 0, 5, 0, 0},
            {5, 0, 0, 0, 3, 0, 4, 6, 0, 1, 15, 0, 0, 0, 0, 0},
            {0, 9, 1, 6, 0, 14, 0, 11, 0, 0, 2, 0, 0, 0, 10, 8},
            {0, 14, 0, 0, 0, 13, 9, 0, 4, 12, 11, 8, 0, 0, 2, 0}
    };



    cout << "Original Sudoku:" << endl;
    displayBoard(board);
    auto start = high_resolution_clock::now();

    solveSudoku(board);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "\nSolved Sudoku:" << endl;
    displayBoard(board);
    cout << "\nTime taken: " << duration.count() << " milliseconds" << endl;


    return 0;
}
