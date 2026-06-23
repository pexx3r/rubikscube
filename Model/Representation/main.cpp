#include "RubiksCube.h"
#include "RubiksCube3dArray.cpp"
#include "../../Solver/dfs_solver.h"
#include <unordered_map>
using namespace std;

int main(){
    RubiksCube3dArray cube1;
    vector<RubiksCube::MOVE>moves = cube1.randomShuffleCube(3);
    
    for(auto x : moves) cout<<cube1.getMove(x)<<" ";
    
    cube1.print();

    DFS_Solver<RubiksCube3dArray,Hash3d> DFS_Solver(cube1,5);
    vector<RubiksCube::MOVE> moves_r = DFS_Solver.solve();
    DFS_Solver.rubikscube.print();
    for(auto x : moves_r) cout<<cube1.getMove(x)<<" ";



}