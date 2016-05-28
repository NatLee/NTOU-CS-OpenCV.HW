#include "TextDetection.hpp"
#include "TEXTRecognizer.h"

bool cmp(int a, int b)
{
	return a > b;
}

Mat clearDots(Mat &src) {
	for (int r = 0; r < src.rows; ++r) {
		for (int c = 0; c < src.cols; ++c) {
			int label = src.ptr<uchar>(r, c)[0];
			if (label == 255) {
				int count = 0;
				for (int i = r - 2; i < r + 2 && i >= 0 && i < src.rows; i++) {
					for (int j = c - 2; j < c + 2 && j >= 0 && j < src.cols; j++) {
						if (src.ptr<uchar>(i, j)[0] == 255) count++;
					}
				}
				if (count < 5)
					src.ptr<uchar>(r, c)[0] = 0;
			}
		}
	}
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(1, 1));
	erode(src, src, element, cv::Point(-1, -1), 2);
	element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(src, src, element, cv::Point(-1, -1), 1);
	return src;
}


static Mat on_trackbar(int, void *, Mat &src, Mat &input)
{
	TEXTRecognizer textHandler;
	textHandler.initialize();
	double confidence = 0.0;
	string str;
	//imshow("binary", src);

	Mat labelImage(src.size(), CV_32SC1);

	src = clearDots(src);

	imshow("clear", src);





	int nLabels = connectedComponents(src, labelImage, 8);
	cout << nLabels << endl;

	map<int, Rect> contour;
	vector<int>area_count;//counting the all contour average area, delete the biggest contour

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
			//cout << setfill(' ') << setw(3) << label;
		}
		//cout << endl;
	}

	string tstring = "roi";

	int area_ave = 0;//do average area
	for (int i = 1; i < nLabels; i++) {
		int area = (contour[i].height - contour[i].y)*(contour[i].width - contour[i].x);
		area_count.push_back(area);
	}
	sort(area_count.begin(), area_count.end(), cmp);
	for (int i = 0; i < nLabels / 2; i++)
		area_ave += area_count[i];
	area_ave /= (nLabels / 2);//end

	int count = 1;
	for (int i = 1; i < nLabels; i++) {//find text
		int area = (contour[i].height - contour[i].y)*(contour[i].width - contour[i].x);
		contour[i].width -= (contour[i].x );
		contour[i].height -= (contour[i].y );
		if (area >= area_ave - d&&area <= area_ave + d&&area>0) {//Set the area ,don't take if exceed the region
			//cout << contour[i].x << " " << contour[i].y << " " << contour[i].width << " " << contour[i].height << " " << area << endl;
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
	//erode(src, src, element, cv::Point(-1, -1), 1);
	//dilate(src, src, element, cv::Point(-1, -1), 1);
	input = on_trackbar(100, 0, src, input);
}
