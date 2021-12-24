#include "Header.h"
int mir(int x, int edge) {			// 101
	if (x < 0)
		x = -x;
	if (x >= edge)
		x = edge - (x - edge + 2);
	return x;
}

void BGR_to_YUV(int height, int width, Mat& YUV, const Mat& BGR)
{
	YUV = Mat::zeros(height, width, CV_64FC3);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			YUV.at<Vec3d>(y, x)[0] = 0.257 * BGR.at<Vec3b>(y, x)[2] + 0.504 * BGR.at<Vec3b>(y, x)[1] + 0.098 * BGR.at<Vec3b>(y, x)[0] + 16;
			YUV.at<Vec3d>(y, x)[1] = -0.148 * BGR.at<Vec3b>(y, x)[2] - 0.291* BGR.at<Vec3b>(y, x)[1] + 0.439 * BGR.at<Vec3b>(y, x)[0] + 128;
			YUV.at<Vec3d>(y, x)[2] = 0.439 * BGR.at<Vec3b>(y, x)[2] - 0.368 * BGR.at<Vec3b>(y, x)[1] - 0.071 * BGR.at<Vec3b>(y, x)[0] + 128;
		}
	}
}

void YUV_to_BGR(int height, int width, Mat& BGR, const Mat& YUV)
{
	/*BGR = Mat::zeros(height, width, CV_8UC3);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			double B = 1.164 * (YUV.at<Vec3d>(y, x)[0] - 16) + 2.018 * (YUV.at<Vec3d>(y, x)[1] - 128);
			double G = 1.164 * (YUV.at<Vec3d>(y, x)[0] - 16) - 0.391 *(YUV.at<Vec3d>(y, x)[1] - 128) - 0.813 * (YUV.at<Vec3d>(y, x)[2] - 128);
			double R = 1.164 * (YUV.at<Vec3d>(y, x)[0] - 16) + 1.596 * (YUV.at<Vec3d>(y, x)[2] - 128);

			if (B > 255)
				BGR.at<Vec3b>(y, x)[0] = 255;
			else if (B < 0)
				BGR.at<Vec3b>(y, x)[0] = 0;
			else
				BGR.at<Vec3b>(y, x)[0] = (uchar)round(B);

			if (G > 255)
				BGR.at<Vec3b>(y, x)[1] = 255;
			else if (G < 0)
				BGR.at<Vec3b>(y, x)[1] = 0;
			else
				BGR.at<Vec3b>(y, x)[1] = (uchar)round(G);

			if (R > 255)
				BGR.at<Vec3b>(y, x)[2] = 255;
			else if (R < 0)
				BGR.at<Vec3b>(y, x)[2] = 0;
			else
				BGR.at<Vec3b>(y, x)[2] = (uchar)round(R);
		}
	}*/

	BGR = Mat::zeros(height, width, CV_64FC3);
	for (int i = 0; i < YUV.rows; i++)
		for (int j = 0; j < YUV.cols; j++) {
			BGR.at<Vec3d>(i, j)[0] = 1.164*(YUV.at<Vec3d>(i, j)[0] - 16) + 2.018*(YUV.at<Vec3d>(i, j)[1] - 128) + 0 * (YUV.at<Vec3d>(i, j)[2] - 128);
			BGR.at<Vec3d>(i, j)[1] = 1.164*(YUV.at<Vec3d>(i, j)[0] - 16) - 0.391*(YUV.at<Vec3d>(i, j)[1] - 128) - 0.813 * (YUV.at<Vec3d>(i, j)[2] - 128);
			BGR.at<Vec3d>(i, j)[2] = 1.164*(YUV.at<Vec3d>(i, j)[0] - 16) + 0 * (YUV.at<Vec3d>(i, j)[1] - 128) + 1.596 * (YUV.at<Vec3d>(i, j)[2] - 128);
		}

	BGR.convertTo(BGR, CV_8UC3);

}



double getPSNR(const Mat& I1, const Mat& I2)
{
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // 不能在8位矩陣上做平方運算
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);         // 疊加每個通道的元素

	double sse = s.val[0] + s.val[1] + s.val[2]; // 疊加所有通道

	if (sse <= 1e-10) // 如果值太小就直接等於0
		return 0;
	else
	{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0*log10((255 * 255) / mse);
		return psnr;
	}
}

tuple<float,float> move_maping(double UL_weight, double UR_weight, double LL_weight, double LR_weight) {
	//因為此二維權重filter的邊長是2個pixel所以才*2
	double x = 2 * (UR_weight + LR_weight);
	double y = 2 * (LL_weight + LR_weight);
	//cout << x << " " << y <<endl;
	return { x, y };
}