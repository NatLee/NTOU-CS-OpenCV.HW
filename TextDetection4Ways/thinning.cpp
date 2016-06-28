#include"thinning.hpp"

void ThinSubiteration1(Mat pSrc, Mat & pDst) {
	int rows = pSrc.rows;
	int cols = pSrc.cols;
	pSrc.copyTo(pDst);
	for (int i = 1; i < rows - 2; i++) {
		for (int j = 1; j < cols - 2; j++) {
			if ((int)pSrc.at<uchar>(i, j) == 255) {
				/// get 8 neighbors
				/// calculate C(p)
				int neighbor0 = (int)pSrc.at<uchar>(i - 1, j - 1) / 255;
				int neighbor1 = (int)pSrc.at<uchar>(i - 1, j) / 255;
				int neighbor2 = (int)pSrc.at<uchar>(i - 1, j + 1) / 255;
				int neighbor3 = (int)pSrc.at<uchar>(i, j + 1) / 255;
				int neighbor4 = (int)pSrc.at<uchar>(i + 1, j + 1) / 255;
				int neighbor5 = (int)pSrc.at<uchar>(i + 1, j) / 255;
				int neighbor6 = (int)pSrc.at<uchar>(i + 1, j - 1) / 255;
				int neighbor7 = (int)pSrc.at<uchar>(i, j - 1) / 255;
				int C = int(~neighbor1 & (neighbor2 | neighbor3)) +
					int(~neighbor3 & (neighbor4 | neighbor5)) +
					int(~neighbor5 & (neighbor6 | neighbor7)) +
					int(~neighbor7 & (neighbor0 | neighbor1));
				if (C == 1) {
					/// calculate N
					int N1 = int(neighbor0 | neighbor1) +
						int(neighbor2 | neighbor3) +
						int(neighbor4 | neighbor5) +
						int(neighbor6 | neighbor7);
					int N2 = int(neighbor1 | neighbor2) +
						int(neighbor3 | neighbor4) +
						int(neighbor5 | neighbor6) +
						int(neighbor7 | neighbor0);
					int N = min(N1, N2);
					if ((N == 1)||(N == 2) || (N == 3)) {
						/// calculate criteria 3
						int c3 = (neighbor1 | neighbor2 | ~neighbor4) & neighbor3;
						if (c3 == 0) {
							pDst.at<uchar>(i, j) = 0;
						}
					}
				}
			}
		}
	}
}

void ThinSubiteration2(Mat pSrc, Mat & pDst) {
	int rows = pSrc.rows;
	int cols = pSrc.cols;
	pSrc.copyTo(pDst);
	for (int i = 1; i < rows - 2; i++) {
		for (int j = 1; j < cols - 2; j++) {
			if ((int)pSrc.at<uchar>(i, j) == 255) {
				/// get 8 neighbors
				/// calculate C(p)
				int neighbor0 = (int)pSrc.at<uchar>(i - 1, j - 1) / 255;
				int neighbor1 = (int)pSrc.at<uchar>(i - 1, j) / 255;
				int neighbor2 = (int)pSrc.at<uchar>(i - 1, j + 1) / 255;
				int neighbor3 = (int)pSrc.at<uchar>(i, j + 1) / 255;
				int neighbor4 = (int)pSrc.at<uchar>(i + 1, j + 1) / 255;
				int neighbor5 = (int)pSrc.at<uchar>(i + 1, j) / 255;
				int neighbor6 = (int)pSrc.at<uchar>(i + 1, j - 1) / 255;
				int neighbor7 = (int)pSrc.at<uchar>(i, j - 1) / 255;
				int C = int(~neighbor1 & (neighbor2 | neighbor3)) +
					int(~neighbor3 & (neighbor4 | neighbor5)) +
					int(~neighbor5 & (neighbor6 | neighbor7)) +
					int(~neighbor7 & (neighbor0 | neighbor1));
				if (C == 1) {
					/// calculate N
					int N1 = int(neighbor0 | neighbor1) +
						int(neighbor2 | neighbor3) +
						int(neighbor4 | neighbor5) +
						int(neighbor6 | neighbor7);
					int N2 = int(neighbor1 | neighbor2) +
						int(neighbor3 | neighbor4) +
						int(neighbor5 | neighbor6) +
						int(neighbor7 | neighbor0);
					int N = min(N1, N2);
					if ((N == 1) || (N == 2) || (N == 3)) {
						int E = (neighbor5 | neighbor6 | ~neighbor0) & neighbor7;
						if (E == 0) {
							pDst.at<uchar>(i, j) = 0;
						}
					}
				}
			}
		}
	}
}