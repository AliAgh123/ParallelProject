#ifndef PARALLELPROJECT_DLX_H
#define PARALLELPROJECT_DLX_H

#include <vector>

using namespace std;

class DLX{
public:
    void solve(vector<vector<int>> puzzle);
    int SIZE;
    DLX(int SIZE);
};
#endif //PARALLELPROJECT_DLX_H
