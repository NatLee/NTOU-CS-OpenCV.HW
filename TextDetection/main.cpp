#include "TextDetection.hpp"
#include "TEXTRecognizer.h"

int main()
{
	help();
    //read the stream
	Mat frame;
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		cout << "Cannot open the video file on C++ API" << endl;
		return -1;
	}
	
	while (cap.read(frame) && !frame.empty()) {
		if (char(waitKey(1) == 'c')) {
			resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
			textDetection(frame);
			waitKey(0);
			destroyAllWindows();
		}
		rectangle(frame, Rect(frame.cols / 4, frame.rows / 4, frame.cols / 2, frame.rows / 4), Scalar(0, 0, 255), 2, 8, 0);
		imshow("movie", frame);
		waitKey(10);
	}
    return 0;
}