#include "OpenCVRenderer.h"
#include <chrono>
#include <sstream>

using namespace MoveTranslator;

OpenCVRenderer::OpenCVRenderer(int width, int height) : width_(width), height_(height) {}
OpenCVRenderer::~OpenCVRenderer(){ cv::destroyAllWindows(); }

static void putMultilineText(cv::Mat &img, const std::string &text, int x, int y, double scale=1.0){
  std::istringstream iss(text);
  std::string line; int dy = 30;
  int i = 0;
  while(std::getline(iss, line)){
    cv::putText(img, line, cv::Point(x, y + i*dy), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(255,255,255), 2);
    i++;
  }
}

void OpenCVRenderer::drawArrow(cv::Mat &img, const std::string &face, Rotation rot) const {
  cv::Point center(width_/2, height_/2 + 80);
  int len = std::min(width_, height_) / 6;
  cv::Scalar arrowColor(0,200,255);
  if(face == "RIGHT"){
    cv::arrowedLine(img, center - cv::Point(len,0), center + cv::Point(len,0), arrowColor, 6);
  } else if(face == "LEFT"){
    cv::arrowedLine(img, center + cv::Point(len,0), center - cv::Point(len,0), arrowColor, 6);
  } else if(face == "TOP"){
    cv::arrowedLine(img, center + cv::Point(0,len), center - cv::Point(0,len), arrowColor, 6);
  } else if(face == "BOTTOM" || face == "DOWN"){
    cv::arrowedLine(img, center - cv::Point(0,len), center + cv::Point(0,len), arrowColor, 6);
  } else if(face == "FRONT"){
    cv::arrowedLine(img, center - cv::Point(len,0), center + cv::Point(len,0), arrowColor, 6);
  } else if(face == "BACK"){
    cv::arrowedLine(img, center + cv::Point(len,0), center - cv::Point(len,0), arrowColor, 6);
  }
  if(rot == Rotation::DOUBLE){
    // draw double indicator: two concentric arrows (simplified)
    cv::circle(img, center, len/2, cv::Scalar(180,180,180), 2);
  }
}

void OpenCVRenderer::renderAndWait(const MoveInstruction &mi, int camIndex){
  if(camIndex >= 0){
    cv::VideoCapture cap(camIndex);
    if(!cap.isOpened()){
      // fallback to static rendering
      cv::Mat img(height_, width_, CV_8UC3, cv::Scalar(bgColor_));
      putMultilineText(img, mi.text(), 50, 80, 0.9);
      drawArrow(img, mi.faceName, mi.rotation);
      cv::imshow("Instructions", img);
      cv::waitKey(0);
      return;
    }
    cv::Mat frame;
    while(true){
      if(!cap.read(frame)) break;
      // draw centered 3x3 overlay similar to scanner
      const int w = frame.cols, h = frame.rows;
      const int size = std::min(w,h) * 2 / 3; // overlay square size
      const int cx = w/2, cy = h/2;
      const int left = cx - size/2, top = cy - size/2;
      const int cell = size / 3;
      cv::Scalar gridColor(0,255,0);
      for(int r=0;r<3;++r){
        for(int c=0;c<3;++c){
          cv::Rect rc(left + c*cell, top + r*cell, cell, cell);
          cv::rectangle(frame, rc, gridColor, 2);
        }
      }
      // map face name to cell coordinate
      std::pair<int,int> coord{1,1};
      if(mi.faceName == "RIGHT") coord = {1,2};
      else if(mi.faceName == "LEFT") coord = {1,0};
      else if(mi.faceName == "TOP") coord = {0,1};
      else if(mi.faceName == "BOTTOM" || mi.faceName == "DOWN") coord = {2,1};
      else if(mi.faceName == "FRONT") coord = {1,1};
      else if(mi.faceName == "BACK") coord = {1,1};

      // compute arrow start/end based on coord and rotation
      cv::Point center(left + coord.second*cell + cell/2, top + coord.first*cell + cell/2);
      int len = cell/3;
      cv::Scalar arrowColor(0,200,255);
      if(mi.faceName == "RIGHT"){
        if(mi.rotation == Rotation::CLOCKWISE) cv::arrowedLine(frame, center - cv::Point(len,0), center + cv::Point(len,0), arrowColor, 4);
        else if(mi.rotation == Rotation::COUNTER_CLOCKWISE) cv::arrowedLine(frame, center + cv::Point(len,0), center - cv::Point(len,0), arrowColor, 4);
        else cv::arrowedLine(frame, center - cv::Point(len,0), center + cv::Point(len,0), arrowColor, 4), cv::circle(frame, center, len/2, cv::Scalar(180,180,180),2);
      } else if(mi.faceName == "LEFT"){
        if(mi.rotation == Rotation::CLOCKWISE) cv::arrowedLine(frame, center + cv::Point(len,0), center - cv::Point(len,0), arrowColor, 4);
        else if(mi.rotation == Rotation::COUNTER_CLOCKWISE) cv::arrowedLine(frame, center - cv::Point(len,0), center + cv::Point(len,0), arrowColor, 4);
        else cv::arrowedLine(frame, center + cv::Point(len,0), center - cv::Point(len,0), arrowColor, 4), cv::circle(frame, center, len/2, cv::Scalar(180,180,180),2);
      } else if(mi.faceName == "TOP"){
        if(mi.rotation == Rotation::CLOCKWISE) cv::arrowedLine(frame, center + cv::Point(0,len), center - cv::Point(0,len), arrowColor, 4);
        else if(mi.rotation == Rotation::COUNTER_CLOCKWISE) cv::arrowedLine(frame, center - cv::Point(0,len), center + cv::Point(0,len), arrowColor, 4);
        else cv::arrowedLine(frame, center + cv::Point(0,len), center - cv::Point(0,len), arrowColor, 4), cv::circle(frame, center, len/2, cv::Scalar(180,180,180),2);
      } else if(mi.faceName == "BOTTOM" || mi.faceName == "DOWN"){
        if(mi.rotation == Rotation::CLOCKWISE) cv::arrowedLine(frame, center - cv::Point(0,len), center + cv::Point(0,len), arrowColor, 4);
        else if(mi.rotation == Rotation::COUNTER_CLOCKWISE) cv::arrowedLine(frame, center + cv::Point(0,len), center - cv::Point(0,len), arrowColor, 4);
        else cv::arrowedLine(frame, center - cv::Point(0,len), center + cv::Point(0,len), arrowColor, 4), cv::circle(frame, center, len/2, cv::Scalar(180,180,180),2);
      } else { // FRONT/BACK default
        if(mi.rotation == Rotation::CLOCKWISE) cv::arrowedLine(frame, center - cv::Point(len,0), center + cv::Point(len,0), arrowColor, 4);
        else if(mi.rotation == Rotation::COUNTER_CLOCKWISE) cv::arrowedLine(frame, center + cv::Point(len,0), center - cv::Point(len,0), arrowColor, 4);
        else cv::arrowedLine(frame, center - cv::Point(len,0), center + cv::Point(len,0), arrowColor, 4), cv::circle(frame, center, len/2, cv::Scalar(180,180,180),2);
      }

      // draw text
      putMultilineText(frame, mi.text(), 20, 40, 0.8);
      cv::imshow("Instructions Overlay", frame);
      int k = cv::waitKey(30);
      // proceed when ENTER (13), SPACE(32), or newline(10)
      if(k == 13 || k == 32 || k == 10) break;
      if(k == 27) { cap.release(); cv::destroyWindow("Instructions Overlay"); exit(0); }
    }
    cap.release();
    cv::destroyWindow("Instructions Overlay");
    return;
  }
  // fallback: static image
  cv::Mat img(height_, width_, CV_8UC3, cv::Scalar(bgColor_));
  putMultilineText(img, mi.text(), 50, 80, 0.9);
  drawArrow(img, mi.faceName, mi.rotation);
  cv::imshow("Instructions", img);
  cv::waitKey(0);
}
