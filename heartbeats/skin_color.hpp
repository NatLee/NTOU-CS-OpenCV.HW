#ifndef skin_color_hpp
#define skin_color_hpp
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "iomanip"
#include <time.h>


using namespace cv;
using namespace std;

void CreatSkinColorTrackbar();

Mat skincolor(const Mat&);


//double heartBeat(VideoCapture , clock_t &, bool &, int &, double &, double &)
#endif
