#ifndef HEADER_H
#define HEADER_H


#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <filesystem>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

void BGR_to_YUV(int height, int width, Mat& YUV, const Mat& BGR);
void YUV_to_BGR(int height, int width, Mat& BGR, const Mat& YUV);
double getPSNR(const Mat& I1, const Mat& I2);
tuple<float, float> move_maping(double UL_wieght, double UR_weight, double LL_weight, double LR_weight);

int mir(int x, int edge);
void convert420_L(Mat&YUV, Mat&Y, Mat&U, Mat&V, Mat&YUV420);
void convert420_A(Mat&YUV, Mat&Y, Mat&U, Mat&V, Mat&YUV420);

Mat Jari_Up(Mat& input_Y, Mat& input_U, Mat& VFinal, double t, bool move, string inter);
void bilinear(Mat& expand444, Mat& Ufinal, Mat& Vfinal);
#endif
#pragma once
