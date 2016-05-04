#ifndef skin_color_hpp
#define skin_color_hpp
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <time.h>

#define red_thres 2.4
#define find_place 50
using namespace cv;
using namespace std;

Mat skincolor(const Mat&);
#endif
