#include "find_collar.hpp"

int LY = 0;
int HY = 255;

int LCr = 137;
int HCr = 177;

int LCb = 77;
int HCb = 127;

int now_row = 87;
int breath = 0;
bool change = false;
int row_max = 0;
int row_min = 8787;

int collarPos[2] = { 0,0 };
void setCollarPos(int x, int y) {
	collarPos[0] = x < 0 ? 0 : x;
	collarPos[1] = y < 0 ? 0 : y;
}

int *getCollarPos() {
	return collarPos;
}

Mat skincolor(Mat& src) {
	cvtColor(src, src, cv::COLOR_BGR2YCrCb);
	inRange(src, Scalar(LY, LCr, LCb), Scalar(HY, HCr, HCb), src);
	erode(src, src, Mat(), Point(-1, -1), 3);
	dilate(src, src, Mat(), Point(-1, -1), 3);
	return src;
}

bool skincolor(double b) {
	return b != 0;
}

void CreateTrackbar() {
	namedWindow("Control", CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("LowY", "Control", &LY, 255);
	cvCreateTrackbar("HighY", "Control", &HY, 255);
	cvCreateTrackbar("LowCr", "Control", &LCr, 255);
	cvCreateTrackbar("HighCr", "Control", &HCr, 255);
	cvCreateTrackbar("LowCb", "Control", &LCb, 255);
	cvCreateTrackbar("HighCb", "Control", &LCb, 255);
}

void find_collar(Mat& frame) {
	Mat skin = skincolor(frame);
	for (int row = frame.rows - 2; row >= 0; row--) {//NO FIRST
		for (int col = frame.cols - 2; col >= 0; col--) {
			int count = 0;
			for (int k = col; k > col - long_max && col > long_max && k > 0; k--) {
				if (skincolor(skin.ptr<uchar>(row, k)[0])) {
					if (!skincolor(skin.ptr<uchar>(row + 1, col)[0])) break;
					count++;
				}
				else break;
			}
			if (count == long_max) {
				for (int lineLength = col; lineLength > col - lineLengthMax && col > lineLengthMax; lineLength--) {
					for (int lineWidth = row; lineWidth < row + neck_find && lineWidth < frame.rows - 2; lineWidth++) {
						if (!skincolor(skin.ptr<uchar>(lineWidth + 1, lineLength)[0] && skincolor(skin.ptr<uchar>(lineWidth, lineLength)[0]))) {
							int lineTemp = lineWidthRange;
							while (lineTemp--&&lineWidth + lineTemp < frame.rows - 5) {
								frame.ptr<uchar>(lineWidth + lineTemp, lineLength)[0] = 255;
								frame.ptr<uchar>(lineWidth + lineTemp, lineLength)[1] = 0;
								frame.ptr<uchar>(lineWidth + lineTemp, lineLength)[2] = 0;
							}
							setCollarPos(lineLength- lineLengthMax/2, lineWidth);
							goto out;
						}
					}
				}
			out:
				row_max = max(row_max, row);
				row_min = min(row_min, row);
				if (now_row > row + 7)
					printf("向上移動\n");
				else if (now_row < row - 7)
					printf("向下移動\n");
				now_row = row;
				return;
			}
		}
	}
}
