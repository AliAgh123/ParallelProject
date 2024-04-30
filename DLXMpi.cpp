#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include "DLX.h"
#include <mpi.h>


using namespace std;

//#define SIZE 25
int SIZE;
//#define SIZE 9
//#define SIZE 4

const int UNASSIGNED = 0;

// Define the type for a single Sudoku puzzle (2D array)
typedef vector<vector<int>> SudokuPuzzle;

// Function to read Sudoku puzzles from file
vector<SudokuPuzzle> readSudokuPuzzlesFromFile(const string& filename) {
    ifstream inFile(filename);
    vector<SudokuPuzzle> puzzles;
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return puzzles;
    }
    string line;
    while (getline(inFile, line)) {
        int i = 0;
        SudokuPuzzle puzzle(SIZE, vector<int>(SIZE, UNASSIGNED));
        while(i < SIZE){
            stringstream ss(line);
            for (int j = 0; j < SIZE; ++j) {
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

int main(){
    int MSG_TAG = 0;
    string filename = "sudoku_puzzles.txt";
    DLX d;
    vector<SudokuPuzzle> puzzles = readSudokuPuzzlesFromFile(filename);
    if (puzzles.empty()) {
        cout << "No Sudoku puzzles found in the file." << endl;
        return 1;
    }
    cout << "Sudoku puzzles read from file: " << puzzles.size() << endl;
    cout<< "Enter the size of the sudoku: \n";
    cin>>SIZE;
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_rank == 0) {

        clock_t totalStart = clock();
        int j;
        for (int i = 0; i < puzzles.size(); i++) {
            MPI_Recv(&j, 1, MPI_INT, MPI_ANY_SOURCE, MSG_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << j << " is solved \n";
        }

        clock_t totalEnd = clock() - totalStart;
        cout << "To solve all puzzles: " << (float) totalEnd / CLOCKS_PER_SEC << " seconds.\n\n";
    }
    else {
        int i = world_rank - 1;
        d.solve(puzzles[i]);
        MPI_Send(&i, 1, MPI_INT, 0, MSG_TAG, MPI_COMM_WORLD);
    }
    MPI_Finalize();

    return 0;
}
