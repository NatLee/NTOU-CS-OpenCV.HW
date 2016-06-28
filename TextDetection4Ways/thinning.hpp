#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<cstdio>
#include<vector>

using namespace cv;
using namespace std;

void ThinSubiteration1(Mat pSrc, Mat & pDst);
void ThinSubiteration2(Mat pSrc, Mat & pDst);