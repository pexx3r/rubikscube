#include "OpenCV/ColorScanner.h"
#include <limits>

using namespace OpenCV;

static cv::Scalar hsvMean(const cv::Mat &roi) {
  cv::Mat hsv; cv::cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
  cv::Scalar mean = cv::mean(hsv);
  return mean; // (H, S, V, )
}

ColorScanner::ColorScanner(int camIndex) {
  cap_.open(camIndex);
  // default centroids (H,S,V) - tuned for typical rubik cube colors
  hsvCentroids_["WHITE"]  = cv::Scalar(0,  5, 230);
  hsvCentroids_["YELLOW"] = cv::Scalar(30,200,200);
  hsvCentroids_["RED"]    = cv::Scalar(0, 200,150);
  hsvCentroids_["ORANGE"] = cv::Scalar(15,230,200);
  hsvCentroids_["BLUE"]   = cv::Scalar(110,200,150);
  hsvCentroids_["GREEN"]  = cv::Scalar(70,200,130);
  // default HSV ranges - wider for better coverage
  hsvRanges_["WHITE"]  = HSVRange{cv::Scalar(0,0,180), cv::Scalar(179,40,255)};
  hsvRanges_["YELLOW"] = HSVRange{cv::Scalar(10,80,80), cv::Scalar(50,255,255)};
  hsvRanges_["RED1"]   = HSVRange{cv::Scalar(0,80,60), cv::Scalar(15,255,255)};
  hsvRanges_["RED2"]   = HSVRange{cv::Scalar(160,80,60), cv::Scalar(179,255,255)};
  hsvRanges_["ORANGE"] = HSVRange{cv::Scalar(5,80,80), cv::Scalar(25,255,255)};
  hsvRanges_["BLUE"]   = HSVRange{cv::Scalar(85,80,60), cv::Scalar(135,255,255)};
  hsvRanges_["GREEN"]  = HSVRange{cv::Scalar(40,60,60), cv::Scalar(100,255,255)};
}

ColorScanner::~ColorScanner(){ cap_.release(); }

void ColorScanner::drawOverlay(cv::Mat &frame) const {
  // Show which face to scan
  const char* faceNames[] = {"UP", "LEFT", "RIGHT", "FRONT", "DOWN", "BACK"};
  std::string instText = "Face: " + std::string(faceNames[currentFaceIndex_]) + "  |  D: debug  M: manual  SPACE: capture  Q: quit";
  cv::putText(frame, instText, cv::Point(10,40), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,255,0), 2);
  cv::putText(frame, "Captured: " + std::to_string(currentFaceIndex_) + "/6", cv::Point(10,80), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(200,200,200), 2);
  
  // Draw layout guide on right side
  int y = 40;
  int lineHeight = 30;
  cv::putText(frame, "Layout:", cv::Point(frame.cols - 200, y), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(200,200,100), 1);
  y += lineHeight;
  cv::putText(frame, "      UP", cv::Point(frame.cols - 200, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,255,255), 1);
  y += lineHeight;
  cv::putText(frame, "L  F  R  B", cv::Point(frame.cols - 200, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,255,255), 1);
  y += lineHeight;
  cv::putText(frame, "     DOWN", cv::Point(frame.cols - 200, y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,255,255), 1);
}

void ColorScanner::printFaceLayout() {
  std::cout << "\n";
  std::cout << "========== Rubik's Cube Face Layout =========\n";
  std::cout << "\n";
  std::cout << "           |UP|\n";
  std::cout << "        |L||F||R||B|\n";
  std::cout << "           |DOWN|\n";
  std::cout << "\n";
  std::cout << "Where:\n";
  std::cout << "  UP    = Top face\n";
  std::cout << "  DOWN  = Bottom face\n";
  std::cout << "  FRONT = Face pointing at camera\n";
  std::cout << "  BACK  = Face pointing away\n";
  std::cout << "  LEFT  = Left side\n";
  std::cout << "  RIGHT = Right side\n";
  std::cout << "\n";
  std::cout << "Scanning order: UP -> LEFT -> RIGHT -> FRONT -> DOWN -> BACK\n";
  std::cout << "===========================================\n\n";
}

cv::Rect ColorScanner::roiRect(const cv::Mat &frame, int cellRow, int cellCol) const {
  const int w = frame.cols, h = frame.rows;
  const int size = std::min(w,h) * 2 / 3;
  const int cx = w/2, cy = h/2;
  const int left = cx - size/2, top = cy - size/2;
  const int cell = size / 3;
  int pad = std::max(4, cell/8);
  cv::Rect rc(left + cellCol*cell + pad, top + cellRow*cell + pad, cell - 2*pad, cell - 2*pad);
  return rc & cv::Rect(0,0,frame.cols, frame.rows);
}

std::string ColorScanner::classifyNameFromHSV(const cv::Scalar &hsv) const {
  // simple nearest-centroid by Euclidean distance (H,S,V)
  double bestDist = std::numeric_limits<double>::max();
  std::string bestName = "WHITE";
  for(const auto &p : hsvCentroids_) {
    cv::Scalar c = p.second;
    double dh = std::min(std::abs(hsv[0]-c[0]), 180.0 - std::abs(hsv[0]-c[0]));
    double ds = hsv[1]-c[1]; double dv = hsv[2]-c[2];
    double d = dh*dh + ds*ds + dv*dv;
    if(d < bestDist){ bestDist = d; bestName = p.first; }
  }
  return bestName;
}

RubiksCube::COLOR ColorScanner::mapNameToEnum(const std::string &name) const {
  if(name=="WHITE") return RubiksCube::COLOR::WHITE;
  if(name=="YELLOW") return RubiksCube::COLOR::YELLOW;
  if(name=="RED") return RubiksCube::COLOR::RED;
  if(name=="ORANGE") return RubiksCube::COLOR::ORANGE;
  if(name=="BLUE") return RubiksCube::COLOR::BLUE;
  if(name=="GREEN") return RubiksCube::COLOR::GREEN;
  return RubiksCube::COLOR::WHITE;
}

RubiksCube::COLOR ColorScanner::classifyROI(const cv::Mat &roi) const {
  if(roi.empty()) return RubiksCube::COLOR::WHITE;
  
  // Convert to HSV
  cv::Mat hsv; cv::cvtColor(roi, hsv, cv::COLOR_BGR2HSV);
  
  // Sample pixels and vote on color
  std::map<std::string, int> votes;
  int sampleStep = std::max(1, hsv.rows / 5); // Sample grid points
  
  for(int y = 0; y < hsv.rows; y += sampleStep) {
    for(int x = 0; x < hsv.cols; x += sampleStep) {
      cv::Vec3b hsv_val = hsv.at<cv::Vec3b>(y, x);
      double h = hsv_val[0];
      double s = hsv_val[1];
      double v = hsv_val[2];
      
      // Classify this pixel - ranges ordered by priority to avoid overlaps
      std::string color = "WHITE";
      
      // White: low saturation
      if(s < 40) {
        color = "WHITE";
      }
      // Red: H near 0 or 180
      else if((h < 15 || h > 160) && s > 80) {
        color = "RED";
      }
      // Yellow: H around 25-40 (prioritized before Orange)
      else if(h >= 20 && h <= 40 && s > 80) {
        color = "YELLOW";
      }
      // Orange: H around 5-20 (tightened range to avoid Yellow overlap)
      else if(h >= 5 && h <= 19 && s > 80) {
        color = "ORANGE";
      }
      // Green: H around 40-90
      else if(h >= 40 && h <= 90 && s > 60) {
        color = "GREEN";
      }
      // Blue: H around 90-135
      else if(h >= 90 && h <= 135 && s > 60) {
        color = "BLUE";
      }
      else {
        color = "WHITE";
      }
      
      votes[color]++;
    }
  }
  
  // Find the color with most votes
  std::string bestColor = "WHITE";
  int maxVotes = 0;
  for(const auto &p : votes) {
    if(p.second > maxVotes) {
      maxVotes = p.second;
      bestColor = p.first;
    }
  }
  
  return mapNameToEnum(bestColor);
}

void ColorScanner::manualInputFace(int faceIndex) {
  std::cout << "\n========== Manual Input for Face ==========\n";
  const char* faceNames[] = {"UP", "LEFT", "RIGHT", "FRONT", "DOWN", "BACK"};
  std::cout << "Entering Manual Input for: " << faceNames[faceIndex] << "\n\n";
  std::cout << "Color codes: W(white), G(green), R(red), Y(yellow), O(orange), B(blue)\n\n";
  std::cout << "Enter 9 colors in this pattern (row by row, left to right):\n";
  std::cout << "  [0][1][2]\n";
  std::cout << "  [3][4][5]\n";
  std::cout << "  [6][7][8]\n\n";
  
  for(int r = 0; r < 3; ++r) {
    for(int c = 0; c < 3; ++c) {
      int idx = r * 3 + c;
      while(true) {
        std::cout << "Cell [" << idx << "] (row " << r << ", col " << c << "): ";
        std::string input;
        std::getline(std::cin, input);
        
        if(input.length() == 1) {
          char ch = std::toupper(input[0]);
          RubiksCube::COLOR col = RubiksCube::COLOR::WHITE;
          if(ch == 'W') col = RubiksCube::COLOR::WHITE;
          else if(ch == 'G') col = RubiksCube::COLOR::GREEN;
          else if(ch == 'R') col = RubiksCube::COLOR::RED;
          else if(ch == 'Y') col = RubiksCube::COLOR::YELLOW;
          else if(ch == 'O') col = RubiksCube::COLOR::ORANGE;
          else if(ch == 'B') col = RubiksCube::COLOR::BLUE;
          else {
            std::cout << "  Invalid! Use W/G/R/Y/O/B\n";
            continue;
          }
          faces_[faceIndex][r][c] = col;
          break;
        } else {
          std::cout << "  Please enter a single character (W/G/R/Y/O/B)\n";
        }
      }
    }
  }
  std::cout << "\nFace " << faceNames[faceIndex] << " entered manually.\n\n";
}

void ColorScanner::startCapture(){
  if(!cap_.isOpened()) return;
  int currentFace = currentFaceIndex_;
  cv::Mat frame;
  int detectedFrameCounter = 0;
  const int DETECTED_DISPLAY_FRAMES = 30;
  bool debugMode = false;
  
  while(true){
    if(!cap_.read(frame)) break;
    
    drawOverlay(frame);
    
    // Debug mode: show detected colors for 3x3 grid in real-time
    if(debugMode) {
      int centerX = frame.cols / 2;
      int centerY = frame.rows / 2;
      int cellSize = std::min(frame.cols, frame.rows) / 8;
      
      for(int r=0; r<3; ++r) {
        for(int c=0; c<3; ++c) {
          int x = centerX + (c - 1) * cellSize;
          int y = centerY + (r - 1) * cellSize;
          int sampleSize = cellSize * 3 / 4;
          cv::Rect rc(x - sampleSize/2, y - sampleSize/2, sampleSize, sampleSize);
          rc = rc & cv::Rect(0, 0, frame.cols, frame.rows);
          
          if(rc.area() > 0) {
            cv::Mat roi = frame(rc).clone();
            RubiksCube::COLOR col = classifyROI(roi);
            
            // Draw rectangle around cell
            cv::rectangle(frame, rc, cv::Scalar(255, 255, 255), 2);
            
            // Show color name using consistent switch-based mapping (same as getColorLetter)
            std::string colorStr;
            switch(col) {
              case RubiksCube::COLOR::WHITE: colorStr = "W"; break;
              case RubiksCube::COLOR::GREEN: colorStr = "G"; break;
              case RubiksCube::COLOR::RED: colorStr = "R"; break;
              case RubiksCube::COLOR::YELLOW: colorStr = "Y"; break;
              case RubiksCube::COLOR::ORANGE: colorStr = "O"; break;
              case RubiksCube::COLOR::BLUE: colorStr = "B"; break;
            }
            cv::putText(frame, colorStr, cv::Point(rc.x + 5, rc.y + 25),
                       cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
          }
        }
      }
      
      cv::putText(frame, "DEBUG MODE - D: close  M: manual input  SPACE: capture  Q: quit", 
                  cv::Point(10, frame.rows - 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, 
                  cv::Scalar(0, 255, 255), 2);
    }
    
    if(detectedFrameCounter > 0){
      cv::putText(frame, "DETECTED!", cv::Point(frame.cols/2 - 100, frame.rows/2 - 50), 
                  cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 255, 0), 3);
      detectedFrameCounter--;
    }
    
    cv::imshow("Cube Scanner", frame);
    int key = cv::waitKey(10);
    
    if(key == 27 || key == 'q' || key == 'Q') break;
    if(key == 'd' || key == 'D') debugMode = !debugMode;
    
    if(key == ' ') {
      // Capture current face
      int centerX = frame.cols / 2;
      int centerY = frame.rows / 2;
      int cellSize = std::min(frame.cols, frame.rows) / 8;
      
      for(int r=0; r<3; ++r) {
        for(int c=0; c<3; ++c) {
          int x = centerX + (c - 1) * cellSize;
          int y = centerY + (r - 1) * cellSize;
          int sampleSize = cellSize * 3 / 4;
          cv::Rect rc(x - sampleSize/2, y - sampleSize/2, sampleSize, sampleSize);
          rc = rc & cv::Rect(0, 0, frame.cols, frame.rows);
          
          if(rc.area() > 0) {
            cv::Mat roi = frame(rc).clone();
            faces_[currentFace][r][c] = classifyROI(roi);
          }
        }
      }
      
      detectedFrameCounter = DETECTED_DISPLAY_FRAMES;
      currentFace++;
      currentFaceIndex_ = currentFace;
      if(currentFace >= 6) { 
        cv::destroyAllWindows(); 
        break; 
      }
    }
    
    if(key == 'm' || key == 'M') {
      // Manual input for current face
      cv::destroyAllWindows();
      manualInputFace(currentFace);
      currentFace++;
      currentFaceIndex_ = currentFace;
      if(currentFace >= 6) break;
      // Restart capture window
      debugMode = false;
    }
  }
}

void ColorScanner::calibrate(){
  // Interactive calibration with trackbars for each named color.
  const std::vector<std::string> order = {"WHITE","YELLOW","RED1","RED2","ORANGE","BLUE","GREEN"};
  int idx = 0;
  while(true){
    cv::Mat frame; if(!cap_.read(frame)) break;
    cv::Mat hsv; cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    const std::string &name = order[idx];
    // show mask
    cv::Mat mask; cv::inRange(hsv, hsvRanges_[name].lower, hsvRanges_[name].upper, mask);
    cv::imshow("Calibration - mask", mask);
    cv::imshow("Calibration - frame", frame);
    int k = cv::waitKey(30);
    if(k == 27 || k == 'q' || k == 'Q') break;
    if(k == 'n' || k == 'N') { idx = (idx+1) % order.size(); }
    if(k == 's' || k == 'S') { // save and exit
      saveCalibration("OpenCV/calibration.yml"); break; }
  }
  cv::destroyWindow("Calibration - mask");
  cv::destroyWindow("Calibration - frame");
}

ColorScanner::CubeArray ColorScanner::getScannedFaces() const { return faces_; }

void ColorScanner::saveCalibration(const std::string &path) const {
  cv::FileStorage fs(path, cv::FileStorage::WRITE);
  fs << "ranges" << "{";
  for(const auto &p: hsvRanges_){
    fs << p.first << "[" << (int)p.second.lower[0] << (int)p.second.lower[1] << (int)p.second.lower[2]
       << (int)p.second.upper[0] << (int)p.second.upper[1] << (int)p.second.upper[2] << "]";
  }
  fs << "}";
  fs.release();
}

void ColorScanner::loadCalibration(const std::string &path){
  cv::FileStorage fs(path, cv::FileStorage::READ);
  if(!fs.isOpened()) return;
  cv::FileNode n = fs["ranges"];
  for(auto it = n.begin(); it != n.end(); ++it){
    std::string name = (*it).name();
    std::vector<int> vals; (*it) >> vals;
    if(vals.size()>=6) hsvRanges_[name] = HSVRange{cv::Scalar(vals[0], vals[1], vals[2]), cv::Scalar(vals[3], vals[4], vals[5])};
  }
  fs.release();
}

void ColorScanner::setHSVRange(const std::string &name, const HSVRange &r){ hsvRanges_[name] = r; }
ColorScanner::HSVRange ColorScanner::getHSVRange(const std::string &name) const { return hsvRanges_.at(name); }
