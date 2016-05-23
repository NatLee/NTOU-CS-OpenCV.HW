#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include<vector>

using namespace std;
using namespace cv;

long long int count_thres(Mat);
long long int count_thres(Mat in1)
{
	long long int temp = 0;
	for (int y = 0; y < in1.rows; y++) {
		for (int x = 0; x < in1.cols; x++) {
			temp += in1.ptr<uchar>(y, x)[0];
		}
	}
	return temp / (in1.rows*in1.cols);
}

int main()
{
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	for (;;) {
		Mat input;
		cap >> input;
		resize(input, input, Size(input.cols / 2, input.rows / 2));
		
		cvtColor(input, input, COLOR_BGR2GRAY);

		long long int thres = 0;
		thres = count_thres(input);//do the average of the threshold
		threshold(input, input, thres, 255, THRESH_BINARY_INV);

		for (int y = 0; y < input.rows; y++) {
			for (int x = 0; x < input.cols; x++) {
				if (input.ptr<uchar>(y, x)[0] == 255) {
					int count = 0;
					for (int i = y - 2; i <= y + 2 && i >= 0 && i < input.rows; i++) {
						for (int j = x - 2; j <= x + 2 && j >= 0 && j < input.cols; j++) {
							if (input.ptr<uchar>(i, j)[0] == 255)
								count++;
						}
					}
					if (count <= 4)
						input.ptr<uchar>(y, x)[0] = 0;
				}
			}
		}
		Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
		dilate(input, input, element, Point(-1, -1), 1);

		Rect temp;
		vector<vector<Point>> contours; // Vector for storing contour
		vector<Vec4i> hierarchy;
		findContours(input, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the images

		cout << contours.size() << endl;

		for (int i = 0; i < contours.size(); i++) {
			double area = contourArea(contours[i], false);
			if (area >= 30) {
				temp = boundingRect(contours[i]);
				rectangle(input, temp, Scalar(0, 255, 0), 1, 8, 0);
			}
		}

		imshow("thres", input);

		if (waitKey(30) >= 0) break;
	}
	return 0;
}