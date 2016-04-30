#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <ctype.h>
#include<algorithm>

#define long_max 120
#define lineLengthMax 130
#define lineWidthRange 5
#define neck_find 50

int LY = 0;
int HY = 255;

int LCr = 137;
int HCr = 177;

int LCb = 77;
int HCb = 127;

using namespace std;
using namespace cv;

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, double, const Scalar& color)
{
	for (int y = 0; y < cflowmap.rows; y += step)
		for (int x = 0; x < cflowmap.cols; x += step)
		{
			const Point2f& fxy = flow.at<Point2f>(y, x);
			line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
				color);
			circle(cflowmap, Point(x, y), 2, color, -1);
		}
}

Mat color(Mat& src) {
	cvtColor(src, src, cv::COLOR_BGR2YCrCb);
	inRange(src, Scalar(LY, LCr, LCb), Scalar(HY, HCr, HCb), src);
	erode(src, src, Mat(), Point(-1, -1), 6);
	dilate(src, src, Mat(), Point(-1, -1), 6);
	return src;
}

bool color(double b) {
	return b != 0;
}

Mat findMaxContours(Mat skin, Mat frame) {

	int largest_area = 0;
	int largest_contour_index = 0;
	Rect bounding_rect;

	threshold(skin, skin, 25, 255, THRESH_BINARY); //Threshold the gray

	vector<vector<Point>> contours; // Vector for storing contour
	vector<Vec4i> hierarchy;

	findContours(skin, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

	for (int i = 0; i < contours.size(); i++) // iterate through each contour. 
	{
		double a = contourArea(contours[i], false);  //  Find the area of contour
		if (a > largest_area) {
			largest_area = a;
			largest_contour_index = i;                //Store the index of largest contour
			bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
		}

	}
	Mat black(skin.rows, skin.cols, CV_8UC1, Scalar::all(0));//bulid black Mat

	Scalar color(255, 255, 255);
	//drawContours(skin, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy); // Draw the largest contour using previously stored index.
	drawContours(black, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy);
	rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
	imshow("src", frame);
	//imshow("skin", skin);
	imshow("largest Contour", black);
	return black;
}


int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;

	namedWindow("Control", CV_WINDOW_AUTOSIZE); 

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

	Mat flow, cflow;
	UMat gray, prevgray, uflow;
	namedWindow("flow", 1);

	for (;;) {
		Mat frame, skin;
		cap >> frame;
		flip(frame, frame, 1);
		frame.copyTo(skin);
		color(skin);

		for (int row = frame.rows - 2; row >= 0; row--) {//NO FIRST
			for (int col = frame.cols - 2; col >= 0; col--) {
				int count = 0;
				for (int k = col; k > col - long_max && col > long_max && k > 0; k--) {
					if (color(skin.ptr<uchar>(row, k)[0])) {
						if (!color(skin.ptr<uchar>(row + 1, col)[0])) break;
						count++;
					}
					else break;
				}
				if (count == long_max) {
					for (int lineLength = col; lineLength > col - lineLengthMax && col > lineLengthMax; lineLength--) {
						for (int lineWidth = row; lineWidth < row + neck_find && lineWidth < frame.rows - 2; lineWidth++) {
							if (!color(skin.ptr<uchar>(lineWidth + 1, lineLength)[0] && color(skin.ptr<uchar>(lineWidth, lineLength)[0]))) {
								int lineTemp = lineWidthRange;
								while (lineTemp--&&lineWidth + lineTemp < frame.rows - 5) {
									frame.ptr<uchar>(lineWidth + lineTemp, lineLength)[0] = 255;
									frame.ptr<uchar>(lineWidth + lineTemp, lineLength)[1] = 0;
									frame.ptr<uchar>(lineWidth + lineTemp, lineLength)[2] = 0;
								}
								break;
							}
						}
					}
					row_max = max(row_max, row);
					row_min = min(row_min, row);
					if (now_row != row) {
						if (now_row > row ? !change : change) {
							if (row_max - row_min > 5) {
								breath++;
								if (breath % 2)cout << "呼吸次數: " << breath / 2 << endl;
							}
							row_max = 0;
							row_min = 8787;
							change = now_row > row ? true : false;
						}
					}
					else;//cout << "NO MOVE" << endl;
					now_row = row;
					goto imgshow;
				}
			}
		}
	imgshow:
		skin=findMaxContours(skin, frame);

		skin.copyTo(gray);

		if (!prevgray.empty())
		{
			cout << 123 << endl;
			calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
			cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
			uflow.copyTo(flow);
			drawOptFlowMap(flow, cflow, 8, 1.5, Scalar(0, 255, 0));
			imshow("flow", cflow);
		}
		swap(prevgray, gray);

		if (waitKey(30) >= 0) break;
	}
	return 0;
}