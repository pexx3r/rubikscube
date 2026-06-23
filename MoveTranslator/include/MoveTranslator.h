#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Model/Representation/RubiksCube.h"

namespace MoveTranslator {

enum class Rotation { CLOCKWISE, COUNTER_CLOCKWISE, DOUBLE };

struct MoveInstruction {
  int step; // 1-based
  RubiksCube::MOVE move; // original move token
  std::string faceName; // e.g. "RIGHT"
  Rotation rotation; // clockwise / counter / double
  std::string text() const; // human readable sentence
};

class Translator {
public:
  // Translate a sequence of RubiksCube::MOVE into human instructions
  static std::vector<MoveInstruction> translate(const std::vector<RubiksCube::MOVE> &moves);

  // Print human instructions to stdout
  static void printInstructions(const std::vector<MoveInstruction> &ins);

  // CLI stepper: shows one step at a time and waits for ENTER
  // Optional renderer callback can be supplied to render instructions in other ways later
  static void stepper(const std::vector<MoveInstruction> &ins,
                      std::function<void(const MoveInstruction&)> renderer = nullptr);
};

} // namespace MoveTranslator
