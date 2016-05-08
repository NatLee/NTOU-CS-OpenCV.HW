#include "skin_color.hpp"

Mat skincolor(const Mat& src) {
	static int LY = 0;
	static int HY = 255;

	static int LCr = 137;
	static int HCr = 177;

	static int LCb = 77;
	static int HCb = 127;

	namedWindow("Control", 1);
	cvCreateTrackbar("LowY", "Control", &LY, 255);
	cvCreateTrackbar("HighY", "Control", &HY, 255);
	cvCreateTrackbar("LowCr", "Control", &LCr, 255);
	cvCreateTrackbar("HighCr", "Control", &HCr, 255);
	cvCreateTrackbar("LowCb", "Control", &LCb, 255);
	cvCreateTrackbar("HighCb", "Control", &LCb, 255);

	Mat dst;
	cvtColor(src, dst, COLOR_BGR2YCrCb);
	inRange(dst, Scalar(LY, LCr, LCb), Scalar(HY, HCr, HCb), dst);
	erode(dst, dst, Mat(), Point(-1, -1), 2);
	dilate(dst, dst, Mat(), Point(-1, -1), 2);
	return dst;
}
