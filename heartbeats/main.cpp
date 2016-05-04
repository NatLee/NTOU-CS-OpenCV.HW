#include "skin_color.hpp"
Mat findMaxContours(Mat skin, Mat frame, Rect &temp) {

	int largest_area = 0;
	int largest_contour_index = 0;

	vector<vector<Point>> contours; // Vector for storing contour
	vector<Vec4i> hierarchy;
	findContours(skin, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

	for (int i = 0; i < contours.size(); i++) // iterate through each contour.
	{
		double a = contourArea(contours[i], false);  //  Find the area of contour
		if (a > largest_area) {
			largest_area = a;
			largest_contour_index = i;                //Store the index of largest contour
			temp = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
		}
	}
	Mat black(skin.rows, skin.cols, CV_8UC1, Scalar::all(0));//bulid black Mat
	drawContours(black, contours, largest_contour_index, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	temp.x = temp.x + (temp.width / 4);
	temp.y = temp.y + (temp.height / 4);
	temp.width = temp.width/2;
	temp.height = temp.height/2;
	rectangle(frame, temp, Scalar(0, 255, 0), 1, 8, 0);
	imshow("src", frame);//source image
	imshow("largest Contour", black);// Largest Contour
	skin = black;
	return black;
}

int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	CreateTrackbar();
	bool t = true;
	int Heartbeat = 0;
	double red_avg = 0, pre_red_avg = 0;
	DWORD t1, t2;
	t1 = GetTickCount();
	for (;;) {
		Mat frame, skin;
		cap >> frame;
		flip(frame, frame, 1);
		skin = skincolor(frame);

		Rect temp;
		skin = findMaxContours(skin, frame, temp);
		imshow("skin", skin);


		double red_total = 0.0, passthru = 0.0;
		for (int i = temp.y; i < temp.y + temp.height; i++) {
			for (int j = temp.x; j < temp.x + temp.width; j++) {
				if (skin.ptr<uchar>(i, j)[0] == 255) {//if it's Skin do the process of adding red_scale for averaging
					red_total += (int)frame.ptr<uchar>(i, j)[2];
					passthru++;
				}
			}
		}
		red_avg = red_total / passthru;
		if (pre_red_avg == 0)
			pre_red_avg = red_avg;
		//printf("%f  %f\n", red_avg1, red_avg);
		if (red_avg - red_thres > pre_red_avg) {
			if (!t) {
				Heartbeat++;
				t = !t;
			}
		}
		else if (red_avg + red_thres < pre_red_avg) {
			if (t) {
				Heartbeat++;
				t = !t;
			}
		}
		if (Heartbeat) {
			t2 = GetTickCount();
			printf("可能的心跳頻率:%.0lf\n", double(double(Heartbeat) / (0.001*(t2 - t1))) * 60);
			t1 = t2;
			Heartbeat = 0;
		}
		pre_red_avg = red_avg;
		if (waitKey(30) >= 0) break;
	}
	return 0;
}