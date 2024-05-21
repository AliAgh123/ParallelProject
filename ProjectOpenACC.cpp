#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include "DLX.h"

using namespace std;

// #define SIZE 25
 #define SIZE 16
//int SIZE;
// #define SIZE 4

const int UNASSIGNED = 0;

// Define the type for a single Sudoku puzzle (2D array)
typedef vector<vector<int>> SudokuPuzzle;

// Function to read Sudoku puzzles from file
vector<SudokuPuzzle> readSudokuPuzzlesFromFile(const string &filename)
{
    ifstream inFile(filename);
    vector<SudokuPuzzle> puzzles;
    if (!inFile.is_open())
    {
        cerr << "Error: Unable to open file." << endl;
        return puzzles;
    }
    string line;
    while (getline(inFile, line))
    {
        int i = 0;
        SudokuPuzzle puzzle(SIZE, vector<int>(SIZE, UNASSIGNED));
        while (i < SIZE)
        {
            stringstream ss(line);
            for (int j = 0; j < SIZE; ++j)
            {
                ss >> puzzle[i][j];
            }
            getline(inFile, line);
            i++;
        }
        puzzles.push_back(puzzle);
    }
    inFile.close();
    return puzzles;
}

int main()
{

    string filename = "sudoku_puzzles.txt";
    vector<SudokuPuzzle> puzzles = readSudokuPuzzlesFromFile(filename);
    if (puzzles.empty())
    {
        cout << "No Sudoku puzzles found in the file." << endl;
        return 1;
    }
    cout << "Sudoku puzzles read from file: " << puzzles.size() << endl;
    DLX d;
    int i;
    clock_t totalStart = clock();
#pragma acc parallel loop gang num_gangs(puzzles.size()) vector_length(threadsPerBlock)
    for (int i = 0; i < puzzles.size(); i++)
        d.solve(puzzles[i]);
    clock_t totalEnd = clock() - totalStart;
    cout<<"-------------------------------------------------\n";
    cout << "To solve all puzzles: " << (float)totalEnd * 1000.0/ CLOCKS_PER_SEC << " ms.\n\n";
    //    cin.get();
    return 0;
}
