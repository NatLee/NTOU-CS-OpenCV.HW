#include "skin_color.hpp"
#include "heartbears.hpp"
int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	
	for (;;) {
        Mat frame;
        cap>>frame;
        heartBeat(frame);
        imshow("src", frame);
		//printf("HeartBeat: %.2lf time per minutes\n", heartBeat(cap));
		if (waitKey(30) >= 0) break;
	}
	return 0;
}
