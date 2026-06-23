#include "MoveTranslator.h"
#include <iostream>
#include <sstream>
#include <fstream>

// Helper: parse textual move tokens into RubiksCube::MOVE
static bool parseMoveToken(const std::string &tok, RubiksCube::MOVE &out){
  // Accept tokens like R, R', R2, U, U', etc.
  if(tok == "R") { out = RubiksCube::MOVE::R; return true; }
  if(tok == "R'") { out = RubiksCube::MOVE::RPrime; return true; }
  if(tok == "R2") { out = RubiksCube::MOVE::R2; return true; }
  if(tok == "L") { out = RubiksCube::MOVE::L; return true; }
  if(tok == "L'") { out = RubiksCube::MOVE::LPrime; return true; }
  if(tok == "L2") { out = RubiksCube::MOVE::L2; return true; }
  if(tok == "U") { out = RubiksCube::MOVE::U; return true; }
  if(tok == "U'") { out = RubiksCube::MOVE::UPrime; return true; }
  if(tok == "U2") { out = RubiksCube::MOVE::U2; return true; }
  if(tok == "D") { out = RubiksCube::MOVE::D; return true; }
  if(tok == "D'") { out = RubiksCube::MOVE::DPrime; return true; }
  if(tok == "D2") { out = RubiksCube::MOVE::D2; return true; }
  if(tok == "F") { out = RubiksCube::MOVE::F; return true; }
  if(tok == "F'") { out = RubiksCube::MOVE::FPrime; return true; }
  if(tok == "F2") { out = RubiksCube::MOVE::F2; return true; }
  if(tok == "B") { out = RubiksCube::MOVE::B; return true; }
  if(tok == "B'") { out = RubiksCube::MOVE::BPrime; return true; }
  if(tok == "B2") { out = RubiksCube::MOVE::B2; return true; }
  return false;
}

int main(int argc, char** argv){
  std::vector<RubiksCube::MOVE> moves;
  if(argc >= 2){
    // read moves from file or inline
    std::ifstream in(argv[1]);
    if(in){
      std::string tok;
      while(in >> tok){
        RubiksCube::MOVE m;
        if(parseMoveToken(tok, m)) moves.push_back(m);
      }
    } else {
      std::cerr << "Failed to open file: " << argv[1] << std::endl; return 2;
    }
  } else {
    // example move sequence (R U R' U' F2)
    moves = {RubiksCube::MOVE::R, RubiksCube::MOVE::U, RubiksCube::MOVE::RPrime, RubiksCube::MOVE::UPrime, RubiksCube::MOVE::F2};
  }

  auto ins = MoveTranslator::Translator::translate(moves);
  std::cout << "Steps: " << ins.size() << std::endl << std::endl;
  MoveTranslator::Translator::stepper(ins);
  return 0;
}
