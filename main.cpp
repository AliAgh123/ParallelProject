#include "bits/stdc++.h"

using namespace std;

int col[9][9];
int row[9][9];
int boxes[9][9];
bool wrongValue = false;

bool findUnassignedLocation(vector<vector<char>> &board)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (board[i][j] == '.')
                return false;
        }
    }
    return true;
}

int boxIdentification(int i, int j)
{
    if (i <= 2 && j <= 2)
    {
        return 0;
    }
    else if (i <= 2 && 2 < j && j <= 5)
    {
        return 1;
    }
    else if (i <= 2 && 5 < j && j <= 8)
    {
        return 2;
    }
    else if (2 < i && i <= 5 && j <= 2)
    {
        return 3;
    }
    else if (2 < i && i <= 5 && 2 < j && j <= 5)
    {
        return 4;
    }
    else if (2 < i && i <= 5 && 5 < j && j <= 8)
    {
        return 5;
    }
    else if (5 < i && i <= 8 && j <= 2)
    {
        return 6;
    }
    else if (5 < i && i <= 8 && 2 < j && j <= 5)
    {
        return 7;
    }
    else if (5 < i && i <= 8 && 5 < j && j <= 8)
    {
        return 8;
    }
    return -1;
}

void fillArrays(vector<vector<char>> &board)
{
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            if (board[i][j] != '.')
            {
                row[i][board[i][j] - '1']++;
                col[j][board[i][j] - '1']++;
                boxes[boxIdentification(i, j)][board[i][j] - '1']++;
            }
        }
    }
}

bool isSafe()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (col[i][j] > 1 || row[i][j] > 1 || boxes[i][j] > 1)
                return false;
        }
    }
    return true;
}
void solve(vector<vector<char>> &board)
{
    if (findUnassignedLocation(board)){
        return;
    }

    if(!isSafe()){
        return;
    }

    for (int y = 0; y < 9; y++)
    {
        for (int x = 0; x < 9; x++)
        {
            if (board[y][x] != '.')
                continue;
            for (int i = 0; i < 9; i++)
            {
                if (col[x][i] == 0 && row[y][i] == 0 && boxes[boxIdentification(y, x)][i] == 0)
                {
                    col[x][i]++;
                    row[y][i]++;
                    boxes[boxIdentification(y, x)][i]++;
                    board[y][x] = i + '1';
                    solve(board);
                    if(wrongValue){
                        col[x][i]--;
                        row[y][i]--;
                        boxes[boxIdentification(y, x)][i]--;
                        board[y][x] = '.';
                        wrongValue = false;
                    }


                }
                if(i==8 && board[y][x]=='.'){
                    wrongValue = true;
                    return;
                }
            }
        }
    }
}


void solveSudoku(vector<vector<char>>& board) {
    fillArrays(board);
    solve(board);
}

int main() {

    vector<vector<char>> board {
            {'5','3','.','.','7','.','.','.','.'},
            {'6','.','.','1','9','5','.','.','.'},
            {'.','9','8','.','.','.','.','6','.'},
            {'8','.','.','.','6','.','.','.','3'},
            {'4','.','.','8','.','3','.','.','1'},
            {'7','.','.','.','2','.','.','.','6'},
            {'.','6','.','.','.','.','2','8','.'},
            {'.','.','.','4','1','9','.','.','5'},
            {'.','.','.','.','8','.','.','7','9'}};


    solveSudoku(board);
    for(auto row : board){
        for(auto c : row){
            cout<<c<<" ";
        }
        cout<<endl;
    }
    return 0;
}
