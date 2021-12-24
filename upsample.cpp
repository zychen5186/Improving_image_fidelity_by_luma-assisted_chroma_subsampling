#include "Header.h"

struct block {
	double MSD;
	double weight;
	double distance;// 0.0625, 0.1875, 0.5625
	double finalWeight;
	block() :MSD(0), weight(0), distance(0) {};
	block(double a) :MSD(0), weight(0), distance(a) {};
};

bool cmp2(block *a, block *b)
{
	return a->MSD < b->MSD;
}

Mat Jari_Up(Mat& input_Y, Mat& input_U, Mat& input_V, double t, bool move, string inter) {
	double threshold = t;
	double alpha;
	int height = input_Y.rows;
	int width = input_Y.cols;
	Mat UFinal = Mat::zeros(input_Y.rows, input_Y.cols, CV_64FC1);
	Mat VFinal = Mat::zeros(input_Y.rows, input_Y.cols, CV_64FC1);
	Mat Y = input_Y.clone();
	Y = Y / 255;//luma and chroma values are normalized to interval [0,1]
	Mat map_x(height, width, CV_32FC1);
	Mat map_y(height, width, CV_32FC1);
	/*=====================用bilinear處理邊界，且邊界不做remap====================*/
	resize(input_U, UFinal, Size(width, height), INTER_LINEAR);
	resize(input_V, VFinal, Size(width, height), INTER_LINEAR);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map_x.at<float>(i, j) = j;
			map_y.at<float>(i, j) = i;
		}
	}
	Mat alpha_img = Mat::zeros(input_Y.rows, input_Y.cols, CV_8UC1);
	
	for (int i = 1; i < Y.rows-1; i++) {
		for (int j = 1; j < Y.cols-1; j++) {
			block A(0.5625), B(0.1875), C(0.0625), D(0.1875);
			if (i % 2 == 0 && j % 2 == 0) {//左上
				A.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(i, j)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j + 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), j), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j + 1, Y.cols))), 2)) / 4;

				B.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j - 2, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j - 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j - 2, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j - 1, Y.cols))), 2)) / 4;

				C.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), mir(j - 2, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), mir(j - 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j - 2, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j - 1, Y.cols))), 2)) / 4;

				D.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), j)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), mir(j + 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), j), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j + 1, Y.cols))), 2)) / 4;
			}
			else if (i % 2 == 0 && j % 2 == 1) {//右上
				A.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j - 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j - 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(i, j), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), j)), 2)) / 4;

				B.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), mir(j - 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j - 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), j), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), j)), 2)) / 4;

				C.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), mir(j + 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j + 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i - 2, Y.rows), mir(j + 2, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j + 2, Y.cols))), 2)) / 4;

				D.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j + 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j + 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(i, mir(j + 2, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j + 2, Y.cols))), 2)) / 4;
			}
			else if (i % 2 == 1 && j % 2 == 0) {//左下
				A.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), j)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j + 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(i, j), 2) + pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j + 1, Y.cols))), 2)) / 4;

				B.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), j)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j + 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), j), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), mir(j + 1, Y.cols))), 2)) / 4;

				C.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j - 2, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j - 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), mir(j - 2, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), mir(j - 1, Y.cols))), 2)) / 4;

				D.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j - 2, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j - 1, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(i, mir(j - 2, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j - 1, Y.cols))), 2)) / 4;
			}
			else if (i % 2 == 1 && j % 2 == 1) {//右下
				A.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j - 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), j)), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(i, mir(j - 1, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(i, j)), 2)) / 4;

				B.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j + 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i - 1, Y.rows), mir(j + 2, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(i, mir(j + 1, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(i, mir(j + 2, Y.cols))), 2)) / 4;

				C.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j + 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j + 2, Y.cols))), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), mir(j + 1, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), mir(j + 2, Y.cols))), 2)) / 4;

				D.MSD += (pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), mir(j - 1, Y.cols))), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 1, Y.rows), j)), 2)
					+ pow(Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), mir(j - 1, Y.cols)), 2) + pow((Y.at<double>(i, j) - Y.at<double>(mir(i + 2, Y.rows), j)), 2)) / 4;
			}
			vector<block *>vec = { &A,&B,&C,&D };
			sort(vec.begin(), vec.end(), cmp2);
			for (int i = 0; i < 4; i++)//最大difference值為最小difference block的weight
			{
				(*vec[i]).weight = (*vec[3 - i]).MSD;
			}
			vector<double> v = { A.weight, B.weight, C.weight, D.weight };
			double maxWeight = *max_element(v.begin(), v.end());
			double minWeight = *min_element(v.begin(), v.end());
			if ((maxWeight - minWeight) > threshold) {
				alpha = 1;
			}
			else {
				alpha = (maxWeight - minWeight) / threshold;
			}
			alpha_img.at<uchar>(i, j) = (int)(alpha * 255);
			double denominator = 0;
			for (auto it = vec.begin(); it != vec.end(); it++) {
				denominator += pow((**it).weight, alpha) * pow((**it).distance, (1 - alpha));
			}
			for (auto it = vec.begin(); it != vec.end(); it++) {
				(**it).finalWeight = (pow((**it).weight, alpha) * pow((**it).distance, (1 - alpha))) / denominator;
			}

			if (i % 2 == 0 && j % 2 == 0) {//左上
				UFinal.at<double>(i, j) = input_U.at<double>(i / 2, j / 2) * (A.finalWeight) + input_U.at<double>(i / 2, mir(j / 2 - 1, Y.cols)) * (B.finalWeight) +
					input_U.at<double>(mir(i / 2 - 1, Y.rows), mir(j / 2 - 1, Y.cols)) * (C.finalWeight) + input_U.at<double>(mir(i / 2 - 1, Y.rows), j / 2) * (D.finalWeight);
				VFinal.at<double>(i, j) = input_V.at<double>(i / 2, j / 2) * (A.finalWeight) + input_V.at<double>(i / 2, mir(j / 2 - 1, Y.cols)) * (B.finalWeight) +
					input_V.at<double>(mir(i / 2 - 1, Y.rows), mir(j / 2 - 1, Y.cols)) * (C.finalWeight) + input_V.at<double>(mir(i / 2 - 1, Y.rows), j / 2) * (D.finalWeight);
			}
			else if (i % 2 == 0 && j % 2 == 1) {//右上
				UFinal.at<double>(i, j) = input_U.at<double>(i / 2, j / 2) * (A.finalWeight) + input_U.at<double>(mir(i / 2 - 1, Y.rows), j / 2) * (B.finalWeight) +
					input_U.at<double>(mir(i / 2 - 1, Y.rows), mir(j / 2 + 1, Y.cols)) * (C.finalWeight) + input_U.at<double>(i / 2, mir(j / 2 + 1, Y.cols)) * (D.finalWeight);
				VFinal.at<double>(i, j) = input_V.at<double>(i / 2, j / 2) * (A.finalWeight) + input_V.at<double>(mir(i / 2 - 1, Y.rows), j / 2) * (B.finalWeight) +
					input_V.at<double>(mir(i / 2 - 1, Y.rows), mir(j / 2 + 1, Y.cols)) * (C.finalWeight) + input_V.at<double>(i / 2, mir(j / 2 + 1, Y.cols)) * (D.finalWeight);
			}
			else if (i % 2 == 1 && j % 2 == 0) {//左下
				UFinal.at<double>(i, j) = input_U.at<double>(i / 2, j / 2) * (A.finalWeight) + input_U.at<double>(mir(i / 2 + 1, Y.rows), j / 2) * (B.finalWeight) +
					input_U.at<double>(mir(i / 2 + 1, Y.rows), mir(j / 2 - 1, Y.cols)) * (C.finalWeight) + input_U.at<double>(i / 2, mir(j / 2 - 1, Y.cols)) * (D.finalWeight);
				VFinal.at<double>(i, j) = input_V.at<double>(i / 2, j / 2) * (A.finalWeight) + input_V.at<double>(mir(i / 2 + 1, Y.rows), j / 2) * (B.finalWeight) +
					input_V.at<double>(mir(i / 2 + 1, Y.rows), mir(j / 2 - 1, Y.cols)) * (C.finalWeight) + input_V.at<double>(i / 2, mir(j / 2 - 1, Y.cols)) * (D.finalWeight);
			}
			else if (i % 2 == 1 && j % 2 == 1) {//右下
				UFinal.at<double>(i, j) = input_U.at<double>(i / 2, j / 2) * (A.finalWeight) + input_U.at<double>(i / 2, mir(j / 2 + 1, Y.cols)) * (B.finalWeight) +
					input_U.at<double>(mir(i / 2 + 1, Y.rows), j / 2) * (C.finalWeight) + input_U.at<double>(mir(i / 2 + 1, Y.rows), mir(j / 2 + 1, Y.cols)) * (D.finalWeight);
				VFinal.at<double>(i, j) = input_V.at<double>(i / 2, j / 2) * (A.finalWeight) + input_V.at<double>(i / 2, mir(j / 2 + 1, Y.cols)) * (B.finalWeight) +
					input_V.at<double>(mir(i / 2 + 1, Y.rows), j / 2) * (C.finalWeight) + input_V.at<double>(mir(i / 2 + 1, Y.rows), mir(j / 2 + 1, Y.cols)) * (D.finalWeight);
			}
			

			if (move == true) {
				tuple<float, float> t_xy;
				if (i % 2 == 0 && j % 2 == 0) {//左上
					//cout << "左上" << endl;
					t_xy = move_maping(C.finalWeight, D.finalWeight, B.finalWeight, A.finalWeight);
					map_x.at<float>(i, j) = j - 1.5 + get<0>(t_xy);
					map_y.at<float>(i, j) = i - 1.5 + get<1>(t_xy);
				}
				else if (i % 2 == 0 && j % 2 == 1) {//右上
					//cout << "右上" << endl;
					t_xy = move_maping(B.finalWeight, C.finalWeight, A.finalWeight, D.finalWeight);
					map_x.at<float>(i, j) = j - 0.5 + get<0>(t_xy);
					map_y.at<float>(i, j) = i - 1.5 + get<1>(t_xy);
				}
				else if (i % 2 == 1 && j % 2 == 0) {//左下
					//cout << "左下" << endl;
					t_xy = move_maping(D.finalWeight, A.finalWeight, C.finalWeight, B.finalWeight);
					map_x.at<float>(i, j) = j - 1.5 + get<0>(t_xy);
					map_y.at<float>(i, j) = i - 0.5 + get<1>(t_xy);
				}
				else if (i % 2 == 1 && j % 2 == 1) {//右下
					//cout << "右下" << endl;
					t_xy = move_maping(A.finalWeight, B.finalWeight, D.finalWeight, C.finalWeight);
					map_x.at<float>(i, j) = j - 0.5 + get<0>(t_xy);
					map_y.at<float>(i, j) = i - 0.5 + get<1>(t_xy);
				}
			}	
		}
	}

	if (move == true && inter == "cubic") {
		remap(UFinal, UFinal, map_x, map_y, CV_INTER_CUBIC, BORDER_REFLECT);
		remap(VFinal, VFinal, map_x, map_y, CV_INTER_CUBIC, BORDER_REFLECT);
	}
	if (move == true && inter == "linear") {
		remap(UFinal, UFinal, map_x, map_y, CV_INTER_LINEAR, BORDER_REFLECT);
		remap(VFinal, VFinal, map_x, map_y, CV_INTER_LINEAR, BORDER_REFLECT);
	}
	imwrite("alpha.bmp", alpha_img);
	Mat newYuvImg = Mat::zeros(Y.cols, Y.rows, CV_64FC3);
	vector<Mat> newYuvPlane = { input_Y, UFinal, VFinal };
	merge(newYuvPlane, newYuvImg);
	return newYuvImg;
}


void bilinear(Mat& expand444, Mat& Ufinal, Mat& Vfinal) {
	Ufinal = Mat::zeros(expand444.rows, expand444.cols, CV_64FC1);
	Vfinal = Mat::zeros(expand444.rows, expand444.cols, CV_64FC1);

	for (int i = 0; i < expand444.rows; i++)
		for (int j = 0; j < expand444.cols; j++) {
			if (i % 2 == 0 && j % 2 == 0) {
				Ufinal.at<double>(i, j) = expand444.at<Vec3d>(i, j)[1];
				Vfinal.at<double>(i, j) = expand444.at<Vec3d>(i, j)[2];
			}
			else if (i % 2 == 0 && j % 2 == 1) {
				Ufinal.at<double>(i, j) = (expand444.at<Vec3d>(i, mir(j - 1, expand444.cols))[1] + expand444.at<Vec3d>(i, mir(j + 1, expand444.cols))[1]) / 2;
				Vfinal.at<double>(i, j) = (expand444.at<Vec3d>(i, mir(j - 1, expand444.cols))[2] + expand444.at<Vec3d>(i, mir(j + 1, expand444.cols))[2]) / 2;
			}
			else if (i % 2 == 1 && j % 2 == 0) {
				Ufinal.at<double>(i, j) = (expand444.at<Vec3d>(mir(i - 1, expand444.rows), j)[1] + expand444.at<Vec3d>(mir(i + 1, expand444.rows), j)[1]) / 2;
				Vfinal.at<double>(i, j) = (expand444.at<Vec3d>(mir(i - 1, expand444.rows), j)[2] + expand444.at<Vec3d>(mir(i + 1, expand444.rows), j)[2]) / 2;
			}
			else if (i % 2 == 1 && j % 2 == 1) {
				Ufinal.at<double>(i, j) = (expand444.at<Vec3d>(mir(i - 1, expand444.rows), mir(j - 1, expand444.cols))[1] + expand444.at<Vec3d>(mir(i - 1, expand444.rows), mir(j + 1, expand444.cols))[1]
					+ expand444.at<Vec3d>(mir(i + 1, expand444.rows), mir(j - 1, expand444.cols))[1] + expand444.at<Vec3d>(mir(i + 1, expand444.rows), mir(j + 1, expand444.cols))[1]) / 4;
				Vfinal.at<double>(i, j) = (expand444.at<Vec3d>(mir(i - 1, expand444.rows), mir(j - 1, expand444.cols))[2] + expand444.at<Vec3d>(mir(i - 1, expand444.rows), mir(j + 1, expand444.cols))[2]
					+ expand444.at<Vec3d>(mir(i + 1, expand444.rows), mir(j - 1, expand444.cols))[2] + expand444.at<Vec3d>(mir(i + 1, expand444.rows), mir(j + 1, expand444.cols))[2]) / 4;
			}
		}
}