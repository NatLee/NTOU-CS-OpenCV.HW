#include"thinning.hpp"
#include "thinning.cpp"

using namespace cv;
using namespace std;

void grid(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, (width / 4)*(height / 4));
	for (int i = 0; i < height - 1; i++, img += width) {
		for (int j = 0; j < width - 1; j++) {
			int index = j / 4 + i / 4 * (width / 4);
			if (img[j]) feature[index]++;
		}
	}
}
//ROW
void rowProjection(Mat img, int *feature, int &average) {
	for (int i = 0; i < img.rows - 1; i++) {
		for (int j = 0; j < img.cols - 1; j++) {
			if ((int)img.at<uchar>(i, j)) {
				feature[i]++;
			}
		}
	}
	for (int i = 0; i < img.rows; i++) {
		average += feature[i];
	}
	average /= img.rows;


}
//COL
void colProjection(Mat img, int *feature, int &average) {
	for (int i = 0; i < img.rows - 1; i++) {
		for (int j = 0; j < img.cols - 1; j++) {
			if ((int)img.at<uchar>(i, j)) {
				feature[j]++;
			}
		}
	}
	for (int i = 0; i < img.cols; i++) {
		average += feature[i];
	}
	average /= img.cols;
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
	/*VideoCapture cap(0);
	if (!cap.isOpened()) return -1;*/

	for (;;) {
		Mat frame, skin;
		//cap >> frame;
		frame = imread("TEST.jpg", 1);
		/*Rect position = Rect(frame.cols / 4, frame.rows / 4, frame.cols / 2, frame.rows / 6), temp;*/
		Rect position = Rect(0, 0, frame.cols - 1, frame.rows - 1), temp;
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

		double level = (fmax - (fmax - fmin) / 3);

		threshold(roiSubtract, roiSubtract, level, 255, THRESH_BINARY);
		rectangle(frame, position, Scalar(0, 255, 255), 1, 8, 0);

		//fill 
		Mat im_floodfill = roiSubtract.clone();
		floodFill(im_floodfill, Point(0, 0), Scalar(255));
		Mat im_floodfill_inv;
		bitwise_not(im_floodfill, im_floodfill_inv);
		Mat im_out = (roiSubtract | im_floodfill_inv);


		int *featureMaxRowRoi = new  int[roiSubtract.rows]();
		int avgRow = 0;
		rowProjection(im_floodfill_inv, featureMaxRowRoi, avgRow);

		int startP = 0, endP = 0, max = 0;


		IplImage *image = cvCreateImage(cvSize(500, 500), 8, 3);
		for (int i = 0; i < im_floodfill_inv.rows - 1; i++) {
			int t_startP, t_max = 0;
			if (featureMaxRowRoi[i] >= avgRow / 2) {
				t_startP = i;
				while (i < im_floodfill_inv.rows - 1 && featureMaxRowRoi[i++] >= avgRow / 2) {
					t_max = i - t_startP;
					if (t_max > max) {
						startP = t_startP;
						endP = i;
					}
				}
			}
		}

		Mat maxColRoi(im_floodfill_inv, Rect(0, startP, im_floodfill_inv.cols, endP - startP));

		int *featureMaxColRoi = new  int[maxColRoi.cols]();
		int avgCol = 0;
		colProjection(maxColRoi, featureMaxColRoi, avgCol);

		vector<Rect>rect_temp;

		for (int i = 0; i < maxColRoi.cols - 1; i++) {
			int t_startP, t_endP = 0;
			if (featureMaxColRoi[i] >= 3) {
				t_startP = i;
				while (featureMaxColRoi[i++] >= 3) {}
				t_endP = i;
				if (t_endP - t_startP > 5) {
					rectangle(roi, Rect(t_startP, startP, t_endP - t_startP, endP - startP), Scalar(0, 0, 255), 1, 8, 0);
					//cout << Rect(t_startP, startP, t_endP - t_startP, endP - startP) << endl;
					rect_temp.push_back(Rect(t_startP, startP, t_endP - t_startP, endP - startP));
				}
			}
			cvLine(image, cvPoint(i, 0), cvPoint(i, featureMaxColRoi[i]), cvScalar(255, 255, 255));
		}
		vector<Rect>::iterator i;
		std::cout << "/////////////\n";
		for (i = rect_temp.begin(); i != rect_temp.end(); i++) {
			double blackdot[4] = {};
			for (int row = i->tl().y; row < i->tl().y + i->height / 2; row++) {//left top
				for (int col = i->tl().x; col < i->tl().x + i->width / 2; col++) {
					if ((int)im_floodfill_inv.at<uchar>(row, col)) {
						blackdot[0]++;
					}
				}
			}
			for (int row = i->tl().y; row < i->tl().y + i->height / 2; row++) {//right top
				for (int col = i->tl().x + i->width / 2; col < i->tl().x + i->width; col++) {
					if ((int)im_floodfill_inv.at<uchar>(row, col)) {
						blackdot[1]++;
					}
				}
			}
			for (int row = i->tl().y + i->height / 2; row < i->tl().y + i->height; row++) {//left bottom
				for (int col = i->tl().x; col < i->tl().x + i->width / 2; col++) {
					if ((int)im_floodfill_inv.at<uchar>(row, col)) {
						blackdot[2]++;
					}
				}
			}
			for (int row = i->tl().y + i->height / 2; row < i->tl().y + i->height; row++) {//right bottom
				for (int col = i->tl().x + i->width / 2; col < i->tl().x + i->width; col++) {
					if ((int)im_floodfill_inv.at<uchar>(row, col)) {
						blackdot[3]++;
					}
				}
			}

			int maxCorrectNumber = 0, lastCorrectNumber = 0;
			for (int num = 0; num < 10; num++) {
				double sum = blackdot[0] + blackdot[1] + blackdot[2] + blackdot[3];

				char fileName[6] = "0.txt";
				fileName[0] = num + '0';

				freopen(fileName, "r", stdin);
				double a, b, c, d;
				int times=0;
				while (scanf("%lf%lf%lf%lf", &a, &b, &c, &d) != EOF) {
					int correct = 0;
					if (blackdot[0] / sum >= a - thres_num&&blackdot[0] / sum <= a + thres_num) {
						++correct;
					}
					if (blackdot[1] / sum >= b - thres_num&&blackdot[1] / sum <= b + thres_num) {
						++correct;
					}
					if (blackdot[2] / sum >= c - thres_num&&blackdot[2] / sum <= c + thres_num) {
						++correct;
					}
					if (blackdot[3] / sum >= d - thres_num&&blackdot[3] / sum <= d + thres_num) {
						++correct;
					}
					if (correct > 2) {
						times++;
					}
				}
				if (times > maxCorrectNumber) {
					maxCorrectNumber =times;
					lastCorrectNumber = num;
				}
				fclose(stdin);

			}

			std::cout << lastCorrectNumber << endl;
		}

		thinSubiteration1(maxColRoi, maxColRoi);
		thinSubiteration2(maxColRoi, maxColRoi);


		if (maxColRoi.cols != 0 && maxColRoi.rows != 0)//avoid crashing
			cv::imshow("maxColRoi", maxColRoi);
		else
			continue;

		cvShowImage("image", image);
		cvReleaseImage(&image);

		cv::imshow("flood_inv", im_floodfill_inv);
		cv::imshow("frame", frame);
		cv::imshow("roi", roi);
		//imshow("roiSubtract ", roiSubtract);
		cv::waitKey(0);
		if (cv::waitKey(30) >= 0) break;
	}

	return 0;
}
