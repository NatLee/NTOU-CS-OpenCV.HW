#include"skin_color.hpp"
#include"heartbeats.hpp"

static String face_cascade_name = "haarcascade_frontalface_alt.xml";
static String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
static CascadeClassifier face_cascade;
static CascadeClassifier eyes_cascade;
static string window_name = "Face detection";

void detectAndDisplay(Mat, vector<Rect> &);

int main(int argc, const char** argv)
{
	VideoCapture  capture(0);
	Mat frame, black;
	//Load the cascades
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading\n"); return -1; };

	while (true) {
		vector<Rect> faces;
		capture >> frame;
		flip(frame, frame, 1);
		frame.copyTo(black);
		black = skincolor(black);//change the image to black or white
		detectAndDisplay(frame, faces);//detection face
		if (faces.size() == 1)
			heartBeat(black, frame, faces);
		imshow("black", black);
		imshow(window_name, frame);

		if ((char)waitKey(1) == 27)//Press Esc stop the program
			break;
	}
	return 0;
}

void detectAndDisplay(Mat frame, vector<Rect> &faces)
{
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	//Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(101, 101));

	for (int i = 0; i < faces.size(); i++) {
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(0, 255, 0), 2, 8, 0);
		//cout << faces[i] << endl;
		Mat faceROI = frame_gray(faces[i]);

	}
	//imshow(window_name, frame);
}