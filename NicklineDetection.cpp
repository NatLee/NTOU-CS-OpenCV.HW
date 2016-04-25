#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <cstdio>

#define range 30
#define blue 75
#define green 97
#define red 125

#define long_max 120
#define lineLengthMax 200

using namespace std;
using namespace cv;


Mat color(Mat& src) {
    inRange(src, Scalar(blue - range, green - range, red - range), Scalar(blue + range, green + range, red + range), src);
    return src;
}
bool color(Mat& frame,int row,int k){
    int b=frame.at<Vec3b>(row, k)[0];
    int g=frame.at<Vec3b>(row, k)[1];
    int r=frame.at<Vec3b>(row, k)[2];
    //cout<<(int)b<<" ";
    if(b==0&&g==0&&r==0)
        return false;
    else return true;
    /*
     if ((b > blue - range && b < blue + range) && (g>green - range && g < green + range) && (r>red - range && r < red + range))
     return true;
     else
     return false;
     */
}

int main(){
    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;
    namedWindow("Original", WINDOW_AUTOSIZE);
    namedWindow("Skin", WINDOW_AUTOSIZE);
    
    int now_row = 0;
    int breath=0;
    bool change = false;
    int row_max = 0;
    int row_min=8787;
    
    for (;;) {
        Mat frame,skin;
        cap >> frame;
        skin=frame;
        color(skin);
        //flip(frame, frame, 1);
        bool check = false;
        for (int row = frame.rows - 2; row >= 0; row--){//NO FIRST
            for (int col = frame.cols - 2; col >= 0; col--){
                int count = 0;
                for (int k = col; k > col - long_max && col>long_max; k--) {
                    if (color(skin,row,k)){
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
                    row_max=max(row_max,row);
                    row_min=min(row_min,row);
                    if (now_row > row){
                        if (!change){
                            if (row_max - row_min > 5){
                                breath++;
                                cout << "呼吸次數: " << breath / 2 << endl;
                            }
                            row_max = 0;
                            row_min = 8787;
                            change = true;
                        }
                        //cout << "UP" << endl;
                    }
                    else if (now_row < row){
                        if (change){
                            if (row_max - row_min > 5){
                                breath++;
                                cout << "呼吸次數: " << breath / 2 << endl;
                            }
                            row_max = 0;
                            row_min = 8787;
                            change = false;
                        }
                        //cout << "DOWN" << endl;
                    }
                    else;
                    //cout << "NO MOVE" << endl;
                    now_row = row;
                    break;
                }
            }
            if (check) break;
        }
        imshow("Original", frame);
        imshow("Skin", color(frame));
        if (waitKey(30) >= 0) break;
    }
    return 0;
}