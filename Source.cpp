#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <cstdio>

#define range 50

using namespace std;
using namespace cv;

bool color(double r , double g , double b){
	if (b > 75 - range && b < 75 + range && g>97 - range && g < 97 + range && r>125 - range && r < 125 + range)
		return true;
	else
		return false;
}


int main()
{
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	namedWindow("Original", WINDOW_AUTOSIZE);
	namedWindow("Shirt", WINDOW_AUTOSIZE);
	for (;;){
		Mat frame;
		cap >> frame;

		flip(frame, frame, 1);//���k½��v��

		//�ŧi�P�j�p����Mat�ΨӸ˦��
		Mat shirt = Mat(Size(frame.cols, frame.rows), CV_8UC1, Scalar::all(0));
		double b, g, r;//3���

		for (int j = frame.rows-1; j >=0; --j)
		{
			for (int i = frame.cols-1; i >=0; --i)
			{
				b = frame.at<Vec3b>(j, i)[0];//B
				g = frame.at<Vec3b>(j, i)[1];//G
				r = frame.at<Vec3b>(j, i)[2];//R
				int count=0;
				for (int k = j; k < j - 100 && j>100; k++){
					if (color(r, g, b)){
						count++; //�p��
						continue;
					}
					else{
						break;
					}
				}
				if (count == 100){
					cout << "���" << endl;
				}
							/*�H�W�O�g�p�G�O����N�b�o���P�_*/
							/*�M��]�ܼƧ�j���n����A�p�G�O �N�O���F*/
							/*�i�O�ڤ��|�A�D�о�*/
			}
		}
		imshow("Original", frame);
		imshow("Shirt", shirt);

		if (waitKey(30) >= 0) break;
	}
	return 0;
}