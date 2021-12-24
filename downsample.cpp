#include "Header.h"
void convert420_L(Mat&YUV, Mat&Y, Mat&U, Mat&V, Mat&YUV420) {
	YUV420 = Mat::zeros(YUV.rows, YUV.cols, CV_64FC3);
	Y = Mat::zeros(YUV.rows, YUV.cols, CV_64FC1);
	U = Mat::zeros(YUV.rows / 2, YUV.cols / 2, CV_64FC1);
	V = Mat::zeros(YUV.rows / 2, YUV.cols / 2, CV_64FC1);

	int a = 0, b = 0;
	for (int i = 0; i < YUV.rows; i++)
		for (int j = 0; j < YUV.cols; j++) {
			YUV420.at<Vec3d>(i, j)[0] = YUV.at<Vec3d>(i, j)[0];
			Y.at<double>(i, j) = YUV.at<Vec3d>(i, j)[0];
			if (i % 2 == 0 && j % 2 == 0) {

				YUV420.at<Vec3d>(i, j)[1] = YUV420.at<Vec3d>(mir(i + 1, YUV.rows), j)[1] = (YUV.at<Vec3d>(i, j)[1] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[1]) / 2;
				YUV420.at<Vec3d>(i, j)[2] = YUV420.at<Vec3d>(mir(i + 1, YUV.rows), j)[2] = (YUV.at<Vec3d>(i, j)[2] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[2]) / 2;
				U.at<double>(a, b) = (YUV.at<Vec3d>(i, j)[1] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[1]) / 2;
				V.at<double>(a, b) = (YUV.at<Vec3d>(i, j)[2] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[2]) / 2;
				if (b == (YUV.cols / 2) - 1) {
					a++;
					b = 0;
				}
				else
					b++;
			}
		}
}


void convert420_A(Mat&YUV, Mat&Y, Mat&U, Mat&V, Mat&YUV420) {
	YUV420 = Mat::zeros(YUV.rows, YUV.cols, CV_64FC3);
	Y = Mat::zeros(YUV.rows, YUV.cols, CV_64FC1);
	U = Mat::zeros(YUV.rows / 2, YUV.cols / 2, CV_64FC1);
	V = Mat::zeros(YUV.rows / 2, YUV.cols / 2, CV_64FC1);

	int a = 0, b = 0;
	for (int i = 0; i < YUV.rows; i++)
		for (int j = 0; j < YUV.cols; j++) {
			//cout << i << " " << j << endl;
			YUV420.at<Vec3d>(i, j)[0] = YUV.at<Vec3d>(i, j)[0];
			Y.at<double>(i, j) = YUV.at<Vec3d>(i, j)[0];
			if (i % 2 == 0 && j % 2 == 0) {
				YUV420.at<Vec3d>(i, j)[1] = (YUV.at<Vec3d>(i, j)[1] + YUV.at<Vec3d>(i, mir(j + 1, YUV.cols))[1] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[1] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), mir(j + 1, YUV.cols))[1]) / 4;
				YUV420.at<Vec3d>(i, j)[2] = (YUV.at<Vec3d>(i, j)[2] + YUV.at<Vec3d>(i, mir(j + 1, YUV.cols))[2] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[2] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), mir(j + 1, YUV.cols))[2]) / 4;
				U.at<double>(a, b) = (YUV.at<Vec3d>(i, j)[1] + YUV.at<Vec3d>(i, mir(j + 1, YUV.cols))[1] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[1] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), mir(j + 1, YUV.cols))[1]) / 4;
				V.at<double>(a, b) = (YUV.at<Vec3d>(i, j)[2] + YUV.at<Vec3d>(i, mir(j + 1, YUV.cols))[2] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), j)[2] + YUV.at<Vec3d>(mir(i + 1, YUV.rows), mir(j + 1, YUV.cols))[2]) / 4;
				if (j == (YUV.cols - 1)) {
					a++;
					b = 0;
				}
				else
					b++;
			}
		}

}