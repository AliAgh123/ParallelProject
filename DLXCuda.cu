#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>


using namespace std;

__device__ int MAX_K = 1000;

//#define SIZE 25
__device__ int SIZE = 16;
//#define SIZE 9
//#define SIZE 4

__device__ const int UNASSIGNED = 0;

// Define the type for a single Sudoku puzzle (2D array)
typedef vector<vector<int>> SudokuPuzzle;


struct Node {

    Node *left;
    Node *right;
    Node *up;
    Node *down;
    Node *head;

    int size;		//used for Column header
    int rowID[3];	//used to identify row in order to map solutions to a sudoku grid
    //ID Format: (Candidate, Row, Column)
};

__device__ const int SIZE_SQUARED = SIZE*SIZE;
__device__ const int SIZE_SQRT = sqrt((double)SIZE);
__device__ const int ROW_NB = SIZE*SIZE*SIZE;
__device__ const int COL_NB = 4 * SIZE*SIZE;

__device__ struct Node Head;
__device__ struct Node* HeadNode = &Head;
__device__ struct Node* solution[MAX_K];
__device__ struct Node* orig_values[MAX_K];
__device__ bool matrix[ROW_NB][COL_NB] = { { 0 } };
__device__ bool isSolved = false;
__device__ void MapSolutionToGrid(int Sudoku[][SIZE]);
__device__ void PrintGrid(int Sudoku[][SIZE]);

__device__ clock_t timer, timer2;

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


//===============================================================================================================//
//---------------------------------------------DLX Functions-----------------------------------------------------//
//===============================================================================================================//

__device__ void coverColumn(Node* col) {
    col->left->right = col->right;
    col->right->left = col->left;
    for (Node* node = col->down; node != col; node = node->down) {
        for (Node* temp = node->right; temp != node; temp = temp->right) {
            temp->down->up = temp->up;
            temp->up->down = temp->down;
            temp->head->size--;
        }
    }
}

__device__ void uncoverColumn(Node* col) {
    for (Node* node = col->up; node != col; node = node->up) {
        for (Node* temp = node->left; temp != node; temp = temp->left) {
            temp->head->size++;
            temp->down->up = temp;
            temp->up->down = temp;
        }
    }
    col->left->right = col;
    col->right->left = col;
}

__device__ void search(int k) {

    if (HeadNode->right == HeadNode) {
        timer2 = clock() - timer;
        int Grid[SIZE][SIZE] = { {0} };
        MapSolutionToGrid(Grid);
        PrintGrid(Grid);
        cout << " --> Time Elapsed: " << (float)timer2 * 1000.0 / CLOCKS_PER_SEC << " ms.\n\n";
        timer = clock();
        isSolved = true;
        return;
    }

    //Choose Column Object Deterministically: Choose the column with the smallest Size
    Node* Col = HeadNode->right;
    for (Node* temp = Col->right; temp != HeadNode; temp = temp->right)
        if (temp->size < Col->size)
            Col = temp;

    coverColumn(Col);

    for (Node* temp = Col->down; temp != Col; temp = temp->down) {
        solution[k] = temp;
        for (Node* node = temp->right; node != temp; node = node->right) {
            coverColumn(node->head);
        }

        search(k + 1);

        temp = solution[k];
        solution[k] = NULL;
        Col = temp->head;
        for (Node* node = temp->left; node != temp; node = node->left) {
            uncoverColumn(node->head);
        }
    }

    uncoverColumn(Col);
}

//===============================================================================================================//
//----------------------Functions to turn a Sudoku grid into an Exact Cover problem -----------------------------//
//===============================================================================================================//

//--------------------------BUILD THE INITIAL MATRIX CONTAINING ALL POSSIBILITIES--------------------------------//

__device__ void BuildSparseMatrix(bool matrix[ROW_NB][COL_NB]) {

    //Constraint 1: There can only be one value in any given cell --> row-column  relation (intersection between the rows and columns
    int j = 0, counter = 0;
    for (int i = 0; i < ROW_NB; i++) { //iterate over all rows
        matrix[i][j] = 1;
        counter++;
        if (counter >= SIZE) {
            j++;
            counter = 0;
        }
    }

    //Constraint 2: There can only be one instance of a number in any given row --> row-number relation (R1#1 R1#2 .... R4#4 ..)
    int x = 0;
    counter = 1;
    for (j = SIZE_SQUARED; j < 2 * SIZE_SQUARED; j++) {
        for (int i = x; i < counter*SIZE_SQUARED; i += SIZE)
            matrix[i][j] = 1;

        if ((j + 1) % SIZE == 0) {
            x = counter*SIZE_SQUARED;
            counter++;
        }
        else
            x++;
    }

    //Constraint 3: There can only be one instance of a number in any given column --> column-number relation (C1#1 C1#2 .... C4#4)
    j = 2 * SIZE_SQUARED;
    for (int i = 0; i < ROW_NB; i++)
    {
        matrix[i][j] = 1;
        j++;
        if (j >= 3 * SIZE_SQUARED)
            j = 2 * SIZE_SQUARED;
    }

    //Constraint 4: There can only be one instance of a number in any given region --> box-number relation (B1#1 B1#2 ... B4#4)
    x = 0;
    for (j = 3 * SIZE_SQUARED; j < COL_NB; j++) {

        for (int l = 0; l < SIZE_SQRT; l++) {
            for (int k = 0; k<SIZE_SQRT; k++)
                matrix[x + l*SIZE + k*SIZE_SQUARED][j] = 1;
        }

        int temp = j + 1 - 3 * SIZE_SQUARED;

        if (temp % (int)(SIZE_SQRT * SIZE) == 0)
            x += (SIZE_SQRT - 1)*SIZE_SQUARED + (SIZE_SQRT - 1)*SIZE + 1;
        else if (temp % SIZE == 0)
            x += SIZE*(SIZE_SQRT - 1) + 1;
        else
            x++;
    }
}

//-------------------BUILD A TOROIDAL DOUBLY LINKED LIST OUT OF THE SPARSE MATRIX-------------------------//

__device__ void BuildLinkedList(bool matrix[ROW_NB][COL_NB]) {

    Node* header = new Node;
    header->left = header;
    header->right = header;
    header->down = header;
    header->up = header;
    header->size = -1;
    header->head = header;
    Node* temp = header;

    //Create all Column Nodes
    for (int i = 0; i < COL_NB; i++) {
        Node* newNode = new Node;
        newNode->size = 0;
        newNode->up = newNode;
        newNode->down = newNode;
        newNode->head = newNode;
        newNode->right = header;
        newNode->left = temp;
        temp->right = newNode;
        temp = newNode;
    }

    int ID[3] = { 0,1,1 }; // (candidate, row, column)
    //Add a Node for each 1 present in the sparse matrix and update Column Nodes accordingly
    for (int i = 0; i < ROW_NB; i++) {
        Node* top = header->right;
        Node* prev = NULL;

        if (i != 0 && i%SIZE_SQUARED == 0) {
            ID[0] -= SIZE - 1;
            ID[1]++;
            ID[2] -= SIZE - 1;
        }
        else if (i!= 0 && i%SIZE == 0) {
            ID[0] -= SIZE - 1;
            ID[2]++;
        }
        else {
            ID[0]++;
        }

        for (int j = 0; j < COL_NB; j++, top = top->right) {
            if (matrix[i][j]) {
                Node* newNode = new Node;
                newNode->rowID[0] = ID[0];
                newNode->rowID[1] = ID[1];
                newNode->rowID[2] = ID[2];
                if (prev == NULL) {
                    prev = newNode;
                    prev->right = newNode;
                }
                newNode->left = prev;
                newNode->right = prev->right;
                newNode->right->left = newNode;
                prev->right = newNode;
                newNode->head = top;
                newNode->down = top;
                newNode->up = top->up;
                top->up->down = newNode;
                top->size++;
                top->up = newNode;
                if (top->down == top)
                    top->down = newNode;
                prev = newNode;
            }
        }
    }

    HeadNode = header;
}

//-------------------COVERS VALUES THAT ARE ALREADY PRESENT IN THE GRID-------------------------//

__device__ void TransformListToCurrentGrid(vector<vector<int>> Puzzle) {
    int index = 0;
    for(int i = 0 ; i<SIZE; i++ )
        for(int j = 0 ; j<SIZE; j++)
            if (Puzzle[i][j] > 0) {
                Node* Col = NULL;
                Node* temp = NULL;
                for (Col = HeadNode->right; Col != HeadNode; Col = Col->right) {
                    for (temp = Col->down; temp != Col; temp = temp->down)
                        if (temp->rowID[0] == Puzzle[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j)
                            goto ExitLoops;
                }
                ExitLoops:		coverColumn(Col);
                orig_values[index] = temp;
                index++;
                for (Node* node = temp->right; node != temp; node = node->right) {
                    coverColumn(node->head);
                }

            }

}

//===============================================================================================================//
//----------------------------------------------- Print Functions -----------------------------------------------//
//===============================================================================================================//

__device__ void MapSolutionToGrid(int Sudoku[][SIZE]) {

    for (int i = 0; solution[i] != NULL; i++) {
        Sudoku[solution[i]->rowID[1]-1][solution[i]->rowID[2]-1] = solution[i]->rowID[0];
    }
    for (int i = 0; orig_values[i] != NULL; i++) {
        Sudoku[orig_values[i]->rowID[1] - 1][orig_values[i]->rowID[2] - 1] = orig_values[i]->rowID[0];
    }
}

//---------------------------------PRINTS A SUDOKU GRID OF ANY SIZE---------------------------------------------//

__device__ void PrintGrid(int Sudoku[][SIZE]){
    string ext_border = "+", int_border = "|";
    int counter = 1;
    int additional = 0;
    if (SIZE > 9)
        additional = SIZE;
    for (int i = 0; i < ((SIZE +SIZE_SQRT - 1) * 2 +additional+ 1); i++) {
        ext_border += '-';

        if (i > 0 && i % ((SIZE_SQRT*2+SIZE_SQRT*(SIZE>9)+1)*counter + counter-1) == 0) {
            int_border += '+';
            counter++;
        }
        else
            int_border += '-';
    }
    ext_border += '+';
    int_border += "|";

    cout << ext_border << endl;
    for (int i = 0; i<SIZE; i++){
        cout << "| ";
        for (int j = 0; j<SIZE; j++){
            if (Sudoku[i][j] == 0)
                cout << ". ";
            else
                cout << Sudoku[i][j] << " ";
            if (additional > 0 && Sudoku[i][j]<10)
                cout << " ";
            if ((j+1)%SIZE_SQRT == 0)
                cout << "| ";
        }
        cout << endl;
        if ((i + 1) % SIZE_SQRT == 0 && (i+1)<SIZE)
            cout << int_border << endl;
    }
    cout << ext_border << endl << endl;
}

//--------------------------------------------------------------------------------//

__device__ void SolveSudoku(vector<vector<int>> Sudoku) {
    timer = clock();
    BuildSparseMatrix(matrix);
    BuildLinkedList(matrix);
    TransformListToCurrentGrid(Sudoku);
    search(0);
    if (!isSolved)
        cout << "No Solution!" << endl;
    isSolved = false;
    cout << "solved\n";
}

__device__ void solve(vector<vector<int>> puzzle){

    clock_t totalStart = clock();
    SolveSudoku(puzzle);
    clock_t totalEnd = clock() - totalStart;
    cout << "Total Time Elapsed: " << (float)totalEnd * 1000.0 / CLOCKS_PER_SEC << " ms.\n\n";

    return;
}


__global__ void solveInParallel(SudokuPuzzle* puzzles, int n)
{
    int index = blockIdx.y * blockDim.y + threadIdx.y;
    if(index < n){
        solve(puzzles[index]);
    }
}

int main(){

    int n = 2048;

    string filename = "sudoku_puzzles.txt";
    vector<SudokuPuzzle> puzzles = readSudokuPuzzlesFromFile(filename);
    if (puzzles.empty()) {
        cout << "No Sudoku puzzles found in the file." << endl;
        return 1;
    }
    cout << "Sudoku puzzles read from file: " << puzzles.size() << endl;

    SudokuPuzzle* h_puzzles = new SudokuPuzzle[puzzles.size()];

    // Allocate memory on the device
    SudokuPuzzle* d_puzzles;
    cudaMalloc((void**)&d_puzzles, puzzles.size() * sizeof(SudokuPuzzle));

    // Copy puzzles from host to device
    cudaMemcpy(d_puzzles, h_puzzles, puzzles.size() * sizeof(SudokuPuzzle), cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocksPerGrid = (puzzles.size() + threadsPerBlock - 1) / threadsPerBlock;

    solveInParallel<<<blocksPerGrid, threadsPerBlock>>>(d_puzzles, puzzles.size());

    cudaDeviceSynchronize();

    cudaMemcpy(h_puzzles, d_puzzles, SIZE * SIZE * puzzles.size() * sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(d_puzzles);
    cout<<"done solving";
    return 0;
}