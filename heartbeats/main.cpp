#include "skin_color.hpp"
#include "heartbeats.hpp"
int main() {
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	
	for (;;) {
        Mat frame,skin;
        cap>>frame;
        flip(frame, frame, 1);
        skin = skincolor(frame);
        imshow("skin", skin);
        Rect temp;
        temp = findMaxContours(skin, frame);
        heartBeat(skin,frame,temp);
        imshow("dst", frame);

		if (waitKey(30) >= 0) break;
	}
	return 0;
}
