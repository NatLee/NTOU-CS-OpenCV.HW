#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include<features2d.hpp>
#include <iostream>
#include "iomanip"
#include <time.h>

using namespace cv;
using namespace std;

void CreatSkinColorTrackbar();
Mat skincolor(const Mat&);