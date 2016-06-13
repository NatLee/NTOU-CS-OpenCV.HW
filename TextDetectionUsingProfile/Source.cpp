#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <cstdio>
#include <iostream>
#include <list>

#define H 667
#define W 500
using namespace cv;
using namespace std;


list<CvRect> GetRowRects(IplImage* src) {
	list<CvRect> rows;
	int projection[H] = { 0 };
	for (int y = 0; y < src->height; y++) {
		for (int x = 0; x < src->width; x++) {
			if (cvGet2D(src, y, x).val[0] == 255)
				projection[y]++;
		}
	}
	bool inLine = false;
	int start = 0;
	for (int i = 0; i < H; i++) {
		if (!inLine && projection[i] > 10) {
			inLine = true;
			start = i;
		}
		else if ((i - start > 5) && projection[i] < 10 && inLine) {
			inLine = false;
			if (i - start > 10) {
				auto rect = cvRect(0, start - 1, W, i - start + 2);
				rows.push_back(rect);//rows.Add(rect);
			}
		}
	}
	return rows;
}
list<CvRect> GetBlockRects(IplImage* src, int rowY) {
	list<CvRect> blocks;
	int projection[W] = { 0 };
	auto kernel=cvCreateStructuringElementEx(3,1,1,0,CV_SHAPE_RECT);
	cvDilate(src, src, kernel,4);
	for (int x = 0; x < src->width; x++) {
		for (int y = 0; y < src->height; y++) {
			if (cvGet2D(src, y, x).val[0] == 255)
				projection[x]++;
		}
	}
	bool inBlock = false;
	int start = 0;
	for (int i = 0; i < W; i++) {
		if (!inBlock&&projection[i] >= 2) {
			inBlock = true;
			start = i;
		}else if((i - start > 10) && projection[i] < 2 && inBlock){
			inBlock = false;
			if (blocks.size() > 0) {// blocks.Count>0
				// var last = blocks[blocks.Count - 1];
				auto front = blocks.begin();
				advance(front, blocks.size() - 1);
				auto last = *front;
				if (start - last.x - last.width <= 5) {
					blocks.pop_back();//blocks.RemoveAt(blocks.Count - 1)
					auto rect = CvRect(last.x, rowY, i - last.x, H);
					blocks.push_back(rect);//blocks.Add(rect);
				}else {
					auto rect = CvRect(start, rowY, i - start, H);
					blocks.push_back(rect);
				}
			}else {
				CvRect rect = CvRect(start, rowY, i - start, H);
				blocks.push_back(rect);
			}
		}
	}

	return blocks;

}
int main()
{
	char fileName[]= "7.jpg";
	IplImage* src = cvLoadImage(fileName, CV_LOAD_IMAGE_COLOR);//load src
	IplImage* gray = cvLoadImage(fileName, CV_LOAD_IMAGE_GRAYSCALE);//load & convert to gray
	cvThreshold(gray, gray, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);//use threshold
	int h[H] = { 0 }; //c++裡必須是常數運算式...無法動態配置 哭
	
	list<CvRect> rows = GetRowRects(gray);
	list<CvRect> items ;
	for each(auto row in rows) {
		auto cols = GetBlockRects(gray, row.y);
		items.insert(items.end(),cols.begin(),cols.end());//items.AddRange(cols);
	}

	for each(auto rect in items) {
		cvDrawRect(src, rect.x,rect.y, cvScalar(255), 1);
	}
	cvShowImage("src", src);
	cvShowImage("gray", gray);

	cvWaitKey(0);
    return 0;
}