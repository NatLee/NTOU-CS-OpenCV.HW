#ifndef TextDetection_hpp
#define TextDetection_hpp

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv/cv.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;
using namespace cv;

//static Mat on_trackbar(int, void *, Mat &src, Mat &input);
void textDetection(Mat &input);
Mat clearDots(Mat);
void help();
#endif /* TextDetection_hpp */
