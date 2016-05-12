#include "heartbeats.hpp"

void heartBeat(Mat skin, Mat &frame, vector<Rect> faces) {
	static clock_t time;
	static bool t = true;
	static int Heartbeat = 0;
	static double red_avg = 0, pre_red_avg = 0;

	Scalar font_color = Scalar(0, 0, 255);
	CvPoint point = cvPoint(faces[0].y, faces[0].x);
	double real_heartbeat = 0; //每分鐘心跳
	double red_total = 0.0, passthru = 0.0;
	for (int i = faces[0].y; i < faces[0].y + faces[0].height; i++) {
		for (int j = faces[0].x; j < faces[0].y + faces[0].width; j++) {
			if (skin.ptr<uchar>(i, j)[0]) {//if it's Skin do the process of adding red_scale for averaging
				red_total += (int)frame.ptr<uchar>(i, j)[2];
				passthru++;
			}
		}
	}

	red_avg = red_total / passthru;

	if (pre_red_avg == 0)
		pre_red_avg = red_avg;
	//cout << fixed << setprecision(3);
	if (red_avg - red_thres > pre_red_avg) {
		if (!t) {
			Heartbeat++;
			time = clock();
			cout << Heartbeat / 2 << endl;
			real_heartbeat = Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60;
			//cout << "心跳:" << Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60 << "每分鐘" << endl;
			t = !t;
		}
	}
	else if (red_avg + red_thres < pre_red_avg) {
		if (t) {
			Heartbeat++;
			time = clock();
			cout << Heartbeat / 2 << endl;
			real_heartbeat = Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60;
			//cout << "心跳:" << Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60 << "每分鐘" << endl;
			t = !t;
		}
	}
	pre_red_avg = red_avg;
	char out1[100] = "Rate:", buffer[20] = "";
	sprintf(buffer, "%.2lf ", (Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60));
	strcat(out1, buffer);
	sprintf(buffer, "per minutes");
	strcat(out1, buffer);
	putText(frame, out1, point, FONT_HERSHEY_COMPLEX, 0.7, font_color);
	//imshow("src", frame);
}