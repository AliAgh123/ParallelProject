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

bool findUnassignedLocation(vector<vector<char>> &board) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] == '.') return false;
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

void fillArrays(vector<vector<char>> &board) {
    int n = board.size();
    col.resize(n, vector<int>(n, 0));
    row.resize(n, vector<int>(n, 0));
    boxes.resize(n, vector<int>(n, 0));

    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] != '.') {
                int num = board[i][j] - '1';
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

void solve(vector<vector<char>> &board) {
    if (findUnassignedLocation(board)) {
        return;
    }

    if (!isValid(board.size())) {
        return;
    }

    for (int y = 0; y < board.size(); y++) {
        for (int x = 0; x < board[y].size(); x++) {
            if (board[y][x] != '.') continue;

            for (int i = 0; i < board.size(); i++) {
                if (col[x][i] == 0 && row[y][i] == 0 && boxes[boxIdentification(y, x, board.size())][i] == 0) {
                    col[x][i]++;
                    row[y][i]++;
                    boxes[boxIdentification(y, x, board.size())][i]++;
                    board[y][x] = i + '1';
                    solve(board);
                    if (wrongValue) {
                        col[x][i]--;
                        row[y][i]--;
                        boxes[boxIdentification(y, x, board.size())][i]--;
                        board[y][x] = '.';
                        wrongValue = false;
                    }
                }
                if (i == board.size() - 1 && board[y][x] == '.') {
                    wrongValue = true;
                    return;
                }
            }
        }
    }
}

void solveSudoku(vector<vector<char>> &board) {
    fillArrays(board);
    solve(board);
}

void displayBoard(const vector<vector<char>> &board) {
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
    vector<vector<char>> board {
            { '3', '.', '6', '5', '.', '8', '4', '.', '.' },
            { '5', '2', '.', '.', '.', '.', '.', '.', '.' },
            { '.', '8', '7', '.', '.', '.', '.', '3', '1' },
            { '.', '.', '3', '.', '1', '.', '.', '8', '.' },
            { '9', '.', '.', '8', '6', '3', '.', '.', '5' },
            { '.', '5', '.', '.', '9', '.', '6', '.', '.' },
            { '1', '3', '.', '.', '.', '.', '2', '5', '.' },
            { '.', '.', '.', '.', '.', '.', '.', '7', '4' },
            { '.', '.', '5', '2', '.', '6', '3', '.', '.' }
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
