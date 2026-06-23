#pragma once
#include "MoveTranslator.h"
#include <opencv2/opencv.hpp>
#include <string>

namespace MoveTranslator {

class OpenCVRenderer {
public:
  OpenCVRenderer(int width = 800, int height = 600);
  ~OpenCVRenderer();
  // Render the instruction and block until user presses ENTER or SPACE.
  // If camIndex >= 0 the renderer will open the webcam and overlay onto live frames.
  void renderAndWait(const MoveInstruction &mi, int camIndex = 0);

private:
  int width_, height_;
  cv::Scalar bgColor_{30,30,30};
  void drawArrow(cv::Mat &img, const std::string &face, Rotation rot) const;
};

} // namespace MoveTranslator
