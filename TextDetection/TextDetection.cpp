#include "TextDetection.hpp"
#include "TEXTRecognizer.h"
#include <iomanip>
#include <map>
static Mat on_trackbar(int, void *, Mat &src, Mat &input)
{
	TEXTRecognizer textHandler;
	textHandler.initialize();
	double confidence = 0.0;
	string str;
	imshow("binary", src);

	Mat labelImage(src.size(), CV_32SC1);
	int nLabels = connectedComponents(src, labelImage, 8);
	cout << nLabels << endl;

	map<int, Rect> contour;
	
	for (int r = 0; r < labelImage.rows; ++r) {//store top left point and bottom right point
		for (int c = 0; c < labelImage.cols; ++c) {
			int label = labelImage.at<int>(r, c);
			if (contour[label].x == 0 && contour[label].y == 0) {
				contour[label].x = 99999;
				contour[label].y = 99999;
			}
			contour[label].x = c < contour[label].x ? c : contour[label].x;
			contour[label].y = r < contour[label].y ? r : contour[label].y;
			contour[label].width = c > contour[label].width ? c : contour[label].width;
			contour[label].height = r > contour[label].height ? r : contour[label].height;
			cout << setfill(' ') << setw(3) << label;
		}
		cout << endl;
	}
	string tstring = "roi";
	int count = 1;
	for (int i = 1; i < nLabels; i++) {
		int area = (contour[i].height - contour[i].y)*(contour[i].width - contour[i].x);//contour's area
		contour[i].width -= (contour[i].x-2);
		contour[i].height -= (contour[i].y-2);
		//cout << contour[i].tl_x << " " << contour[i].tl_y << " " << contour[i].br_x << " " << contour[i].br_y << " " << area << endl;
		if (area >= 50 & area <= 1000) {//Set the area ,don't take if exceed the region
			stringstream ss;
			ss << count++;
			cout << tstring + ss.str();
			Mat roi(input, contour[i]);
			namedWindow(tstring + ss.str(), WINDOW_AUTOSIZE);
			imshow(tstring + ss.str(), roi);
			textHandler.charDecode(roi, str, confidence);
			cout << " Character = " << str << ", Confidence = " << confidence << std::endl;
			rectangle(input, contour[i], Scalar(255, 0, 0), 1, 8, 0);
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

	Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	//erode(src, src, element, cv::Point(-1, -1), 2);
	//dilate(src, src, element, cv::Point(-1, -1), 1);
	input = on_trackbar(100, 0, src, input);
}
