#ifndef TextDetection_hpp
#define TextDetection_hpp

#include <stdio.h>
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv/cv.h>
#include <iostream>
#include <stdio.h>
#include<vector>

using namespace std;
using namespace cv;

//static Mat on_trackbar(int, void *, Mat &src, Mat &input);
long long int count_thres(Mat in1);
void textDetection(Mat &input);
#endif /* TextDetection_hpp */
