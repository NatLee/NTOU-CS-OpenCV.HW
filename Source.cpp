#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <cstdio>

#define range 50
#define blue 203
#define green 224
#define red 252

#define long_max 120
#define lineLengthMax 200

using namespace std;
using namespace cv;


Mat color(Mat& src) {
	inRange(src, Scalar(blue - range, green - range, red - range), Scalar(blue + range, green + range, red + range), src);
	return src;
}



bool color(double b, double g, double r) {
	if ((b > blue - range && b < blue + range) && (g>green - range && g < green + range) && (r>red - range && r < red + range))
		return true;
	else
		return false;
}


int main()
{
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	namedWindow("Original", WINDOW_AUTOSIZE);
	namedWindow("Skin", WINDOW_AUTOSIZE);

	int now_row = 87;
	for (;;) {
		Mat frame;
		cap >> frame;

		flip(frame, frame, 1);

		bool check = false;

		

		for (int row = frame.rows - 2; row >= 0; row--)//NO FIRST
		{
			for (int col = frame.cols - 2; col >= 0; col--)
			{
				int count = 0;
				for (int k = col; k > col - long_max && col>long_max; k--) {
					if (color(frame.at<Vec3b>(row, k)[0], frame.at<Vec3b>(row, k)[1], frame.at<Vec3b>(row,k)[2])){

						if (!color(frame.at<Vec3b>(row + 1, col)[0], frame.at<Vec3b>(row + 1, col)[1], frame.at<Vec3b>(row + 1, col)[2]))
							break;						
						count++;
					}
					else {
						break;
					}
				}
				if (count == long_max) {
					for (int lineLength = col; lineLength > col - lineLengthMax && col > lineLengthMax; lineLength--) {
						for (int lineWidth = row; lineWidth > row - 5 && row > 5; lineWidth--) {
							frame.at<Vec3b>(lineWidth, lineLength)[0] = 0;
							frame.at<Vec3b>(lineWidth, lineLength)[1] = 255;
							frame.at<Vec3b>(lineWidth, lineLength)[2] = 0;
						}
					}
					check = true;
					//cout << now_row << " " << row << endl;
					if (now_row > row)
						cout << "UP" << endl;
					else if (now_row < row)
						cout << "DOWN" << endl;
					else
						cout << "NO MOVE" << endl;
					now_row = row;

					break;
				}

			}
			if (check) {
				break;
			}
		}
		if (check){
			//cout << "Yee" << endl;
		}
		imshow("Original", frame);
		imshow("Skin", color(frame));

		if (waitKey(30) >= 0) break;
	}
	return 0;
}