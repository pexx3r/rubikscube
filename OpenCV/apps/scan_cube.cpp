#include "OpenCV/ColorScanner.h"
#include "../../Model/Representation/RubiksCube.h"
#include "../../Model/Representation/RubiksCube3dArray.cpp"
#include "../../Solver/dfs_solver.h"
#include <iostream>

int main(int argc, char** argv){
  int camIndex = 0;
  if(argc > 1) camIndex = std::stoi(argv[1]);
  
  OpenCV::ColorScanner::printFaceLayout();
  
  OpenCV::ColorScanner scanner(camIndex);
  scanner.startCapture();
  auto faces = scanner.getScannedFaces();

  RubiksCube3dArray cube;
  for(int f=0; f<6; ++f){
    for(int r=0; r<3; ++r){
      for(int c=0; c<3; ++c){
        cube.setColor(static_cast<RubiksCube::FACE>(f), r, c, faces[f][r][c]);
      }
    }
  }
  
  std::cout << "\n=== Scanned Cube ===" << std::endl;
  cube.print();

  std::cout << "\n=== Solving ===" << std::endl;
  DFS_Solver<RubiksCube3dArray, Hash3d> solver(cube, 7);
  auto moves = solver.solve();

  std::cout << "\n=== Solution Moves (" << moves.size() << " steps) ===" << std::endl;
  for(size_t i = 0; i < moves.size(); ++i){
    std::cout << (i+1) << ". " << RubiksCube::getMove(moves[i]) << std::endl;
  }

  return 0;
}
