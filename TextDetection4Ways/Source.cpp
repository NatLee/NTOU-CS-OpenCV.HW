#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<stdio.h>
#define INPUT_FILE              "7.jpg"
#define OUTPUT_FOLDER_PATH      string("")

using namespace cv;
using namespace std;

void crossing(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, width*height);
	for (int i = 0; i < height - 1; i++, img += width) {
		for (int j = 0; j < width - 1; j++) {
			if (img[j] != img[j + 1])feature[i]++;
			if (img[j] != img[j + width])feature[j + height]++;
		}
	}
}

void grid(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, (width / 4)*(height / 4));
	for (int i = 0; i < height - 1; i++, img += width) {
		for (int j = 0; j < width - 1; j++) {
			int index = j / 4 + i / 4 * (width / 4);
			if (img[j]) feature[index]++;
		}
	}
}

void projection(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, width + height);
	for (int i = 0; i < height - 1; i++, img += width) {
		for (int j = 0; j < width - 1; j++) {
			if (img[j]) {
				feature[i]++;
				feature[j + height]++;
			}
		}
	}
}

void profile(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, (width * 2) + (height * 2));
	unsigned char* buffer = img;
	int* f;
	for (int i = 0; i < height; i++, buffer += width) {
		for (int j = 0; j < width - 1; j++) {
			if (buffer[j] == 0 && buffer[j + 1] == 0) {
				feature[i]++;
			}
			else
				break;
		}
		f = feature + height;
		for (int j = width - 1; j > 0; j--) {
			if (buffer[j] == 0 && buffer[j - 1] == 0) {
				f[i]++;
			}
			else
				break;
		}
	}
	for (int j = 0; j < width; j++) {
		buffer = img;
		f = feature + 2 * height;
		for (int i = 0; i < height - 1; i++, buffer += width) {
			if (buffer[j] == 0 && buffer[j + width] == 0) {
				f[j]++;
			}
			else
				break;
		}
		buffer = img + (height - 1)*width;
		f = feature + 2 * height + width;
		for (int i = height - 1; i > 0; i--) {
			if (buffer[j] == 0 && buffer[j - width] == 0) {
				f[j]++;
			}
			else
				break;
		}
	}
}

int main()
{
	Mat large = imread(INPUT_FILE);
	Mat rgb;
	// downsample and use it for processing
	pyrDown(large, rgb);
	Mat small;
	cvtColor(rgb, small, CV_BGR2GRAY);
	// morphological gradient
	Mat grad;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
	// binarize
	Mat bw;
	threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	// connect horizontally oriented regions
	Mat connected;
	morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
	morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
	// find contours
	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	// filter contours
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		Rect rect = boundingRect(contours[idx]);
		Mat maskROI(mask, rect);
		maskROI = Scalar(0, 0, 0);
		// fill the contour
		drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		// ratio of non-zero pixels in the filled region
		double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

		if (r > .45 /* assume at least 45% of the area is filled if it contains text */
			&&
			(rect.height > 8 && rect.width > 8) /* constraints on region size */
												/* these two conditions alone are not very robust. better to use something
												like the number of significant peaks in a horizontal projection as a third condition */
			)
		{
			rectangle(rgb, rect, Scalar(0, 255, 0), 2);
		}
	}
	imwrite(OUTPUT_FOLDER_PATH + string("rgb.jpg"), rgb);

	return 0;
}