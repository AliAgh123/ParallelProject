#ifndef PARALLELPROJECT_DLX_H
#define PARALLELPROJECT_DLX_H

#include <vector>

using namespace std;

class DLX{
public:
    __global__ static void solve(vector<vector<int>> puzzle);
};
#endif //PARALLELPROJECT_DLX_H
