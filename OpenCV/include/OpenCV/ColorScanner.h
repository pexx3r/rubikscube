#pragma once
#include <opencv2/opencv.hpp>
#include <array>
#include <map>
#include <string>
#include "../../Model/Representation/RubiksCube.h"

namespace OpenCV {

class ColorScanner {
public:
  using FaceArray = std::array<std::array<RubiksCube::COLOR,3>,3>;
  using CubeArray = std::array<FaceArray,6>;

  explicit ColorScanner(int camIndex = 0);
  ~ColorScanner();

  // Blocks and runs the UI capture loop
  void startCapture();
  
  // Print face layout guide
  static void printFaceLayout();

  // Manual input for a face (when auto-detection fails)
  void manualInputFace(int faceIndex);

  // Interactive calibration (opens trackbars)
  void calibrate();

  // After scanning, retrieve faces in scanning order
  CubeArray getScannedFaces() const;

  // Save/load HSV calibration
  void saveCalibration(const std::string &path) const;
  void loadCalibration(const std::string &path);

  // Set/get HSV ranges programmatically
  struct HSVRange { cv::Scalar lower; cv::Scalar upper; };
  void setHSVRange(const std::string &name, const HSVRange &r);
  HSVRange getHSVRange(const std::string &name) const;

private:
  cv::VideoCapture cap_;
  CubeArray faces_{};

  // simple HSV centroids per logical color for distance matching
  std::map<std::string, cv::Scalar> hsvCentroids_;
  std::map<std::string, HSVRange> hsvRanges_;

  // interactive state
  int currentFaceIndex_ = 0;
  bool showMasks_ = false;

  void drawOverlay(cv::Mat &frame) const;
  cv::Rect roiRect(const cv::Mat &frame, int cellRow, int cellCol) const;
  RubiksCube::COLOR classifyROI(const cv::Mat &roi) const;
  std::string classifyNameFromHSV(const cv::Scalar &hsv) const;
  RubiksCube::COLOR mapNameToEnum(const std::string &name) const;
};

} // namespace OpenCV
