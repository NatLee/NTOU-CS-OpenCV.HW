#ifndef heartbeats_hpp
#define heartbeats_hpp

#include "skin_color.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#define red_thres 0.0
void heartBeat(Mat&,Mat&,Rect&);
Rect findMaxContours(Mat skin, Mat frame);
#endif