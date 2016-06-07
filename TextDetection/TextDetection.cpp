#include "TextDetection.hpp"
#include "TEXTRecognizer.h"

void help()
{
	printf("Press key C to stop the frame and do text recognization\n");
	printf("If you want to continue and press another key\n");
}

bool cmp(int a, int b)
{
	return a > b;
}

Mat clearDots(Mat src) {
	for (int r = 0; r < src.rows; ++r) {
		for (int c = 0; c < src.cols; ++c) {
			int label = src.ptr<uchar>(r, c)[0];
			if (label == 255) {
				int count = 0;
				for (int i = r - 3; i < r + 3 && i >= 0 && i < src.rows; i++) {
					for (int j = c - 3; j < c + 3 && j >= 0 && j < src.cols; j++) {
						if (src.ptr<uchar>(i, j)[0] == 255) count++;
					}
				}
				if (count < 4)
					src.ptr<uchar>(r, c)[0] = 0;
			}
		}
	}
	/*Mat element = getStructuringElement(MORPH_ELLIPSE, Size(1, 1));
	erode(src, src, element, cv::Point(-1, -1), 2);
	element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(src, src, element, cv::Point(-1, -1), 1);*/
	return src;
}


static Mat on_trackbar(int, void *, Mat &src, Mat &input)
{
	rectangle(input, Rect(input.cols / 4, input.rows / 4, input.cols / 2, input.rows / 4), Scalar(0, 0, 255), 2, 8, 0);
	//only do text recognization in this region

	TEXTRecognizer textHandler;
	textHandler.initialize();
	double confidence = 0.0;
	string text;
	//imshow("binary", src);

	Mat labelImage(src.size(), CV_32SC1);

	src = clearDots(src);

	imshow("clear", src);

	int nLabels = connectedComponents(src, labelImage, 8);
	cout << nLabels << endl;

	map<int, Rect> contour;
	vector<int>area_count;//counting the all contour average area, delete the biggest contour

	for (int r = labelImage.rows / 4; r < labelImage.rows / 2; ++r) {//store top left point and bottom right point
		for (int c = labelImage.cols / 4; c < labelImage.cols * 3 / 4; ++c) {
			int label = labelImage.at<int>(r, c);
			if (contour[label].x == 0 && contour[label].y == 0) {
				contour[label].x = 99999;
				contour[label].y = 99999;
			}
			contour[label].x = c < contour[label].x ? c : contour[label].x;
			contour[label].y = r < contour[label].y ? r : contour[label].y;
			contour[label].width = (c + 2 > contour[label].width&&c + 2 < labelImage.cols) ? c + 2 : contour[label].width;
			contour[label].height = (r + 2 > contour[label].height&&r + 2 < labelImage.rows) ? r + 2 : contour[label].height;
			//cout << setfill(' ') << setw(3) << label;
		}
		//cout << endl;
	}

	int area_ave = 0;//do average area
	for (int i = 1; i < nLabels; i++) {
		int area = (contour[i].height - contour[i].y)*(contour[i].width - contour[i].x);
		area_count.push_back(area);
	}
	sort(area_count.begin(), area_count.end(), cmp);
	for (int i = 0; i < 5 && i < nLabels; i++)
		area_ave += area_count[i];
	area_ave /= 5;//end

	int d = area_ave * 2 / 3;

	for (int i = 1; i < nLabels; i++) {//find text
		int area = (contour[i].height - 2 - contour[i].y)*(contour[i].width - 2 - contour[i].x);
		contour[i].width -= (contour[i].x);
		contour[i].height -= (contour[i].y);
		if (area >= area_ave - d&&area <= area_ave + d) {//Set the area ,don't take if exceed the region
			//cout << contour[i].x << " " << contour[i].y << " " << contour[i].width << " " << contour[i].height << " " << area << endl;
			Mat roi(src, contour[i]);
			textHandler.charDecode(roi, text, confidence);
			//cout << " Character = " << text << ", Confidence = " << confidence << std::endl;
			rectangle(input, contour[i], Scalar(255, 0, 0), 1, 8, 0);
			putText(input, text, Point(contour[i].x, contour[i].y + contour[i].height), CV_FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255, 255, 0));
			//imshow(text, roi);
		}
	}
	imshow("rect", input);
	return src;
}

void textDetection(Mat &input) {
	Mat src;
	input.copyTo(src);
	cvtColor(src, src, COLOR_BGR2GRAY);
	double maxVal = 255;
	adaptiveThreshold(src, src, maxVal, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 11, 2);
	input = on_trackbar(100, 0, src, input);
}
