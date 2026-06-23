#include "MoveTranslator.h"
#include <iostream>
#include <unordered_map>
#include "OpenCVRenderer.h"

using namespace MoveTranslator;

std::string MoveInstruction::text() const {
  std::string rot;
  switch(rotation){
    case Rotation::CLOCKWISE: rot = "clockwise."; break;
    case Rotation::COUNTER_CLOCKWISE: rot = "counter-clockwise."; break;
    case Rotation::DOUBLE: rot = "180 degrees."; break;
  }
  return "Step " + std::to_string(step) + ":\nRotate the " + faceName + " face " + rot;
}

static void moveToFaceAndRotation(RubiksCube::MOVE m, std::string &faceOut, Rotation &rotOut){
  // Map every MOVE enum to a face name and rotation type.
  // This table assumes your RubiksCube::MOVE enum contains these symbols.
  switch(m){
    case RubiksCube::MOVE::R: faceOut = "RIGHT"; rotOut = Rotation::CLOCKWISE; break;
    case RubiksCube::MOVE::RPrime: faceOut = "RIGHT"; rotOut = Rotation::COUNTER_CLOCKWISE; break;
    case RubiksCube::MOVE::R2: faceOut = "RIGHT"; rotOut = Rotation::DOUBLE; break;

    case RubiksCube::MOVE::L: faceOut = "LEFT"; rotOut = Rotation::CLOCKWISE; break;
    case RubiksCube::MOVE::LPrime: faceOut = "LEFT"; rotOut = Rotation::COUNTER_CLOCKWISE; break;
    case RubiksCube::MOVE::L2: faceOut = "LEFT"; rotOut = Rotation::DOUBLE; break;

    case RubiksCube::MOVE::U: faceOut = "TOP"; rotOut = Rotation::CLOCKWISE; break;
    case RubiksCube::MOVE::UPrime: faceOut = "TOP"; rotOut = Rotation::COUNTER_CLOCKWISE; break;
    case RubiksCube::MOVE::U2: faceOut = "TOP"; rotOut = Rotation::DOUBLE; break;

    case RubiksCube::MOVE::D: faceOut = "BOTTOM"; rotOut = Rotation::CLOCKWISE; break;
    case RubiksCube::MOVE::DPrime: faceOut = "BOTTOM"; rotOut = Rotation::COUNTER_CLOCKWISE; break;
    case RubiksCube::MOVE::D2: faceOut = "BOTTOM"; rotOut = Rotation::DOUBLE; break;

    case RubiksCube::MOVE::F: faceOut = "FRONT"; rotOut = Rotation::CLOCKWISE; break;
    case RubiksCube::MOVE::FPrime: faceOut = "FRONT"; rotOut = Rotation::COUNTER_CLOCKWISE; break;
    case RubiksCube::MOVE::F2: faceOut = "FRONT"; rotOut = Rotation::DOUBLE; break;

    case RubiksCube::MOVE::B: faceOut = "BACK"; rotOut = Rotation::CLOCKWISE; break;
    case RubiksCube::MOVE::BPrime: faceOut = "BACK"; rotOut = Rotation::COUNTER_CLOCKWISE; break;
    case RubiksCube::MOVE::B2: faceOut = "BACK"; rotOut = Rotation::DOUBLE; break;

    default:
      faceOut = "UNKNOWN"; rotOut = Rotation::CLOCKWISE; break;
  }
}

std::vector<MoveInstruction> Translator::translate(const std::vector<RubiksCube::MOVE> &moves){
  std::vector<MoveInstruction> out; out.reserve(moves.size());
  int step = 1;
  for(auto m : moves){
    MoveInstruction mi; mi.step = step++; mi.move = m;
    moveToFaceAndRotation(m, mi.faceName, mi.rotation);
    out.push_back(mi);
  }
  return out;
}

void Translator::printInstructions(const std::vector<MoveInstruction> &ins){
  for(const auto &mi : ins){
    std::cout << mi.text() << std::endl << std::endl;
  }
}

void Translator::stepper(const std::vector<MoveInstruction> &ins, std::function<void(const MoveInstruction&)> renderer){
  if(ins.empty()) return;
  OpenCVRenderer defaultRenderer;
  for(const auto &mi : ins){
    if(renderer) {
      renderer(mi);
      std::cout << "Press ENTER for next step..." << std::flush;
      std::string dummy; std::getline(std::cin, dummy);
    } else {
      // Use OpenCV overlay renderer by default
      defaultRenderer.renderAndWait(mi);
    }
  }
}
