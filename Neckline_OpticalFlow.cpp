#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp" 
#include <iostream>
#include <ctype.h>


int LY = 0;
int HY = 255;

int LCr = 137;
int HCr = 177;

int LCb = 77;
int HCb = 127;

using namespace cv;
using namespace std;

#define UNKNOWN_FLOW_THRESH 1e9

void makecolorwheel(vector<Scalar> &colorwheel)
{
	int RY = 15;
	int YG = 6;
	int GC = 4;
	int CB = 11;
	int BM = 13;
	int MR = 6;

	int i;

	for (i = 0; i < RY; i++) colorwheel.push_back(Scalar(255, 255 * i / RY, 0));
	for (i = 0; i < YG; i++) colorwheel.push_back(Scalar(255 - 255 * i / YG, 255, 0));
	for (i = 0; i < GC; i++) colorwheel.push_back(Scalar(0, 255, 255 * i / GC));
	for (i = 0; i < CB; i++) colorwheel.push_back(Scalar(0, 255 - 255 * i / CB, 255));
	for (i = 0; i < BM; i++) colorwheel.push_back(Scalar(255 * i / BM, 0, 255));
	for (i = 0; i < MR; i++) colorwheel.push_back(Scalar(255, 0, 255 - 255 * i / MR));
}

void motionToColor(Mat flow, Mat &color)//optical flow
{
	if (color.empty())
		color.create(flow.rows, flow.cols, CV_8UC3);

	static vector<Scalar> colorwheel; //Scalar r,g,b  
	if (colorwheel.empty())
		makecolorwheel(colorwheel);

	// determine motion range:  
	float maxrad = -1;

	// Find max flow to normalize fx and fy  
	for (int i = 0; i < flow.rows; ++i)
	{
		for (int j = 0; j < flow.cols; ++j)
		{
			Vec2f flow_at_point = flow.at<Vec2f>(i, j);
			float fx = flow_at_point[0];
			float fy = flow_at_point[1];
			if ((fabs(fx) >  UNKNOWN_FLOW_THRESH) || (fabs(fy) >  UNKNOWN_FLOW_THRESH))
				continue;
			float rad = sqrt(fx * fx + fy * fy);
			maxrad = maxrad > rad ? maxrad : rad;
		}
	}

	for (int i = 0; i < flow.rows; ++i)
	{
		for (int j = 0; j < flow.cols; ++j)
		{
			uchar *data = color.data + color.step[0] * i + color.step[1] * j;
			Vec2f flow_at_point = flow.at<Vec2f>(i, j);

			float fx = flow_at_point[0] / maxrad;
			float fy = flow_at_point[1] / maxrad;
			if ((fabs(fx) >  UNKNOWN_FLOW_THRESH) || (fabs(fy) >  UNKNOWN_FLOW_THRESH))
			{
				data[0] = data[1] = data[2] = 0;
				continue;
			}
			float rad = sqrt(fx * fx + fy * fy);

			float angle = atan2(-fy, -fx) / CV_PI;
			float fk = (angle + 1.0) / 2.0 * (colorwheel.size() - 1);
			int k0 = (int)fk;
			int k1 = (k0 + 1) % colorwheel.size();
			float f = fk - k0;
			//f = 0; // uncomment to see original color wheel  

			for (int b = 0; b < 3; b++)
			{
				float col0 = colorwheel[k0][b] / 255.0;
				float col1 = colorwheel[k1][b] / 255.0;
				float col = (1 - f) * col0 + f * col1;
				if (rad <= 1)
					col = 1 - rad * (1 - col); // increase saturation with radius  
				else
					col *= .75; // out of range  
				data[2 - b] = (int)(255.0 * col);
			}
		}
	}
}

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

void contourFilter(Mat image)
{
	Mat temp;
	Mat prevgray, gray, flow;
	Mat motion2color;

	RNG rng(12345);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy,
		CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	//在threshold_img影像中尋找所有的輪廓

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		//計算可以包含輪廓的最小長方形區域

		boundRect[i] = boundingRect(Mat(contours_poly[i]));

		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//隨機給一個顏色

		if (boundRect[i].area() > 50000)
		{//長方形區域面積超過50000，則畫在影像上
			drawContours(image, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			boundRect[i].y += 50;
			rectangle(image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);

			int x1= boundRect[i].tl().x, y1= boundRect[i].tl().y, x2= boundRect[i].br().x, y2= boundRect[i].br().y;
			if (boundRect[i].tl().x >= image.cols)x1 = image.cols - 2;
			if (boundRect[i].tl().y >= image.rows)y1 = image.rows - 2;
			if (boundRect[i].br().x >= image.cols)x2 = image.cols - 2;
			if (boundRect[i].br().y >= image.rows)y2 = image.rows - 2;

			Rect r(x1, y1, x2-x1, y2-y1);
			image(r).copyTo(temp);

			namedWindow("flow", 1);//測試切出來的光流

			if (prevgray.data) {
				calcOpticalFlowFarneback(prevgray, temp, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
				motionToColor(flow, motion2color);

				imshow("flow", motion2color);
			}
			swap(prevgray, temp);
		}
	}
	
	namedWindow("tmp", CV_WINDOW_AUTOSIZE);
	imshow("tmp", temp);

	namedWindow("coutour", CV_WINDOW_AUTOSIZE);
	imshow("coutour", image);
}
int main()
{
	VideoCapture cap;
	cap.open(0);

	Mat image;
	/*Mat prevgray, gray, flow;
	Mat motion2color;*/

	for (;;)
	{
		Mat frame;
		cap >> frame;
		if (frame.empty())
			break;

		frame.copyTo(image);
		color(image);
		contourFilter(image);

		/*namedWindow("flow", 1);//main裡面的光流

		if (prevgray.data) {
			calcOpticalFlowFarneback(prevgray, image, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
			motionToColor(flow, motion2color);

			imshow("flow", motion2color);
		}
		swap(prevgray, image);*/

		if (waitKey(30) >= 0) break;
	}
	return 0;
}