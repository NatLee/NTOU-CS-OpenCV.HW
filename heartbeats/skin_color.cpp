#include "skin_color.hpp"

Mat skincolor(const Mat& src) {
    int LY = 0;
    int HY = 255;
    
    int LCr = 137;
    int HCr = 177;
    
    int LCb = 77;
    int HCb = 127;
    
    namedWindow("Control", 1);
    cvCreateTrackbar("LowY", "Control", &LY, 255);
    cvCreateTrackbar("HighY", "Control", &HY, 255);
    cvCreateTrackbar("LowCr", "Control", &LCr, 255);
    cvCreateTrackbar("HighCr", "Control", &HCr, 255);
    cvCreateTrackbar("LowCb", "Control", &LCb, 255);
    cvCreateTrackbar("HighCb", "Control", &LCb, 255);
    
	Mat dst;
	cvtColor(src, dst, cv::COLOR_BGR2YCrCb);
	inRange(dst, Scalar(LY, LCr, LCb), Scalar(HY, HCr, HCb), dst);
	//erode(dst, dst, Mat(), Point(-1, -1), 2);
	//dilate(dst, dst, Mat(), Point(-1, -1), 2);
	return dst;
}
