#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <cstdio>

#define long_max 120
#define lineLengthMax 130

int LY = 0;
int HY = 255;

int LCr = 137;
int HCr = 177;

int LCb = 77;
int HCb = 127;

using namespace std;
using namespace cv;

Mat color(Mat& src) {
	//cvtColor(src, src, CV_BGR2HSV);
	//inRange(src, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), src);
	cvtColor(src, src, cv::COLOR_BGR2YCrCb);
	inRange(src, Scalar(LY, LCr, LCb), Scalar(HY, HCr, HCb), src);
	//inRange( src, Scalar(80, 135, 85), Scalar(255, 180, 135), src );

	erode(src, src, Mat(), Point(-1, -1), 6);
	dilate(src, src, Mat(), Point(-1, -1), 6);
	return src;
}

bool color(double b) {
	return b != 0;
}

int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	namedWindow("Original", WINDOW_AUTOSIZE);
	namedWindow("Skin", WINDOW_AUTOSIZE);
	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
												//Create trackbars in "Control" window
	cvCreateTrackbar("LowY", "Control", &LY, 255);
	cvCreateTrackbar("HighY", "Control", &HY, 255);
	cvCreateTrackbar("LowCr", "Control", &LCr, 255);
	cvCreateTrackbar("HighCr", "Control", &HCr, 255);
	cvCreateTrackbar("LowCb", "Control", &LCb, 255);
	cvCreateTrackbar("HighCb", "Control", &LCb, 255);

	int now_row = 87;
	int breath = 0;
	bool change = false;
	int row_max = 0;
	int row_min = 8787;

	for (;;) {
		Mat frame, skin;
		cap >> frame;
		flip(frame, frame, 1);
		frame.copyTo(skin);
		color(skin);

		for (int row = frame.rows - 2; row >= 0; row--) {//NO FIRST
			for (int col = frame.cols - 2; col >= 0; col--) {
				int count = 0;
				for (int k = col; k > col - long_max && col>long_max && k>0; k--) {
					if (color(skin.ptr<uchar>(row, k)[0])) {
						if (!color(skin.ptr<uchar>(row + 1, col)[0])) break;
						count++;
					}
					else break;
				}
				if (count == long_max) {
					for (int lineLength = col; lineLength > col - lineLengthMax && col > lineLengthMax; lineLength--) {
						for (int lineWidth = row; lineWidth < row + 50 && lineWidth < frame.rows-2; lineWidth++) {
							if (!color(skin.ptr<uchar>(lineWidth+1, lineLength)[0]&& color(skin.ptr<uchar>(lineWidth, lineLength)[0]))) {
								frame.ptr<uchar>(lineWidth, lineLength)[0] = 0;
								frame.ptr<uchar>(lineWidth, lineLength)[1] = 255;
								frame.ptr<uchar>(lineWidth, lineLength)[2] = 0;
								break;
							}
						}
					}
					row_max = max(row_max, row);
					row_min = min(row_min, row);
					if (now_row != row) {
						if (now_row>row ? !change : change) {
							if (row_max - row_min > 5) {
								breath++;
								cout << "呼吸次數: " << breath / 2 << endl;
							}
							row_max = 0;
							row_min = 8787;
							change = now_row>row ? true : false;
						}
					}
					else;//cout << "NO MOVE" << endl;
					now_row = row;
					goto imgshow;
				}
			}
		}
	imgshow:
		imshow("Original", frame);
		imshow("Skin", skin);
		if (waitKey(30) >= 0) break;
	}
	return 0;
}
