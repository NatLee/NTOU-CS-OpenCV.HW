#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<cstdio>
#include<vector>


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

Mat1b getDisk(int M)
{
	// M positive and multiple of 3
	CV_Assert((M > 0) && ((M % 3) == 0));

	int k = M / 3;
	int rows = M * 2 + 1;
	int cols = M * 2 + 1;
	Point c(M, M);

	Mat1b strel(rows, cols, uchar(0));


	vector<Point> vertices(8);
	vertices[0].y = c.x - k;
	vertices[0].x = 0;
	vertices[1].y = c.x + k;
	vertices[1].x = 0;

	vertices[2].y = cols - 1;
	vertices[2].x = c.y - k;
	vertices[3].y = cols - 1;
	vertices[3].x = c.y + k;

	vertices[4].y = c.x + k;
	vertices[4].x = rows - 1;
	vertices[5].y = c.x - k;
	vertices[5].x = rows - 1;

	vertices[6].y = 0;
	vertices[6].x = c.y + k;
	vertices[7].y = 0;
	vertices[7].x = c.y - k;

	fillConvexPoly(strel, vertices, Scalar(1));

	return strel;

}



int main()
{

	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;

	for (;;) {
		Mat frame, skin;
		cap >> frame;
		Rect position = Rect(frame.cols / 4, frame.rows / 4, frame.cols / 2, frame.rows / 6), temp;
		Mat roi(frame, position);
		
		Mat roiStrel;
		cvtColor(roi, roiStrel, CV_BGR2GRAY);
		Mat roiGray;
		roiStrel.copyTo(roiGray);
		Mat1b kernel = getDisk(3);
		morphologyEx(roiStrel, roiStrel, MORPH_ERODE, kernel);
		Mat roiSubtract = roiGray - roiStrel;
		double fmax, fmin;
		minMaxLoc(roiSubtract, &fmin, &fmax);

		double level = (fmax - (fmax - fmin) / 3) / 255;
		threshold(roiSubtract, roiSubtract, level, fmax, THRESH_BINARY);
		rectangle(frame, position, Scalar(0, 255, 255), 1, 8, 0);

		imshow("frame", frame);
		//imshow("roiGray", roiGray);
		//imshow("roiStrel", roiStrel);
		imshow("roiSubtract ", roiSubtract);
		//imshow("roi", roi);
		if (waitKey(30) >= 0) break;
	}

	return 0;
}
