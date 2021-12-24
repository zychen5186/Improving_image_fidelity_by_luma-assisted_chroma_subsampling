#include "Header.h"


int main()
{
	ofstream f;
	f.open("result.csv");
	f << "data, upsample,threshold, move, psnr" << endl;
	
	string path = "classical";
	for (const auto & entry : fs::directory_iterator(path)) {
		string data = entry.path().stem().string();
		cout << data << endl;
		double bilinear_psnr, jari_psnr, jari_linear_psnr, jari_cubic_psnr;
		stringstream imgpath;
		imgpath.str("");
		imgpath.clear();
		imgpath << "classical/" << data << ".bmp";

		Mat imgBgr = imread(imgpath.str());
		int height = imgBgr.rows;
		int width = imgBgr.cols;

		Mat imgYuv;
		Mat Y, U, V;
		Mat Yuv420;

		BGR_to_YUV(height, width, imgYuv, imgBgr);
		convert420_A(imgYuv, Y, U, V, Yuv420);//average U,V pixels in block, 會有位移CDV(-1/2, -1/2)
		//convert420_L(imgYuv, Y, U, V, Yuv420);
		Mat new_U, new_V;
		for (int method = 0; method < 2; method++) {
			/*====================bilinear放大====================*/
			if (method == 0) {
				Mat Yuv444;
				//bilinear(Yuv420, new_U, new_V);
				resize(U, new_U, Size(width, height), INTER_LINEAR);
				resize(V, new_V, Size(width, height), INTER_LINEAR);

				vector<Mat> plane = { Y,new_U,new_V };
				Yuv444 = Mat::zeros(Y.cols, Y.rows, CV_64FC3);
				merge(plane, Yuv444);

				Mat newBgr;
				YUV_to_BGR(height, width, newBgr, Yuv444);
				
				imwrite("result/" + data + "_bilinear.png", newBgr);

				bilinear_psnr = PSNR(imgBgr, newBgr);
				f << data << ",BILINEAR, X, X," << to_string(bilinear_psnr) << endl;

				//bilinear_psnr += getPSNR(imgBgr, newBgr);
				cout << "bilinear psnr = "<< bilinear_psnr << endl;
				/*===========================*/
				//Mat map_x(height, width, CV_32FC1);
				//Mat map_y(height, width, CV_32FC1);
				//for (int i = 0; i < height; i++) {
				//	for (int j = 0; j < width; j++) {
				//		map_x.at<float>(i, j) = j;
				//		map_y.at<float>(i, j) = i-0.5;
				//	}
				//}
				//remap(new_U, new_U, map_x, map_y, CV_INTER_CUBIC, BORDER_REFLECT);
				//remap(new_V, new_V, map_x, map_y, CV_INTER_CUBIC, BORDER_REFLECT);
				//cout << new_U.type() << endl;

				//vector<Mat> move_plane = { Y,new_U,new_V };
				//Yuv444 = Mat::zeros(Y.cols, Y.rows, CV_64FC3);
				//merge(move_plane, Yuv444);

				//Mat move_newBgr;
				//YUV_to_BGR(height, width, move_newBgr, Yuv444);

				//imwrite("result/" + data + "_bilinear.png", move_newBgr);

				//bilinear_psnr = PSNR(imgBgr, move_newBgr);
				//f << data << ",BILINEAR, X, X," << to_string(bilinear_psnr) << endl;

				////bilinear_psnr += getPSNR(imgBgr, newBgr);
				//cout << "bilinear+move psnr = " << bilinear_psnr << endl;

			}
			/*====================Jari放大====================*/
			if (method == 1) {
				double threshold = 0.15;
				vector<bool> move = { false,true };
				vector<string> inter = { "linear","cubic" };
				for (auto it1 = move.begin(); it1 != move.end(); it1++) {
					if (*it1 == true) {
						for (auto it2 = inter.begin(); it2 != inter.end(); it2++) {
							Mat Yuv444;
							Yuv444 = Jari_Up(Y, U, V, threshold, *it1, *it2);//Jari會先把 U,V pixel移到(1/2,1/2)，抵銷掉420A的位移

							Mat newBgr;
							YUV_to_BGR(height, width, newBgr, Yuv444);

							imwrite("result/"+ data + "_Jari_move_" + *it2 + ".png", newBgr);
							
							if (*it2 == "linear") {
								jari_linear_psnr = PSNR(imgBgr, newBgr);
								cout << "move_jari_" + *it2 + " psnr = " << jari_linear_psnr << endl;
								f << data << ",Jari, 0.15," << *it2 << "," << jari_linear_psnr << "," << to_string(jari_linear_psnr - jari_psnr) << endl;
							}
								
							else if (*it2 == "cubic") {
								jari_cubic_psnr = PSNR(imgBgr, newBgr);
								cout << "move_jari_" + *it2 + " psnr = " << jari_cubic_psnr << endl;
								f << data << ",Jari, 0.15," << *it2 << "," << jari_cubic_psnr << "," << to_string(jari_cubic_psnr - jari_psnr) << "," << to_string(jari_psnr - bilinear_psnr) << endl;
								cout << jari_cubic_psnr - jari_psnr << endl;
							}
						}
					}
					if (*it1 == false) {
						Mat Yuv444;
						Yuv444 = Jari_Up(Y, U, V, threshold, *it1, "nothing");//Jari會先把 U,V pixel移到(1/2,1/2)，抵銷掉420A的位移

						Mat newBgr;
						YUV_to_BGR(height, width, newBgr, Yuv444);

						imwrite("result/"+ data + "_Jari_.png", newBgr);

						jari_psnr = PSNR(imgBgr, newBgr);
						cout << "jari psnr = " << jari_psnr << endl;
						f << data<< ",Jari, 0.15, X," << to_string(jari_psnr) << endl;						
					}
				}	
			}
		
			
		}
		cout << endl;
	}
	//cout << "bilinear_psnr = " << bilinear_psnr / (end - 1) << endl;
	//cout << "jari_psnr =" << jari_psnr / (end - 1) << endl;
	//cout << "jari_linear_psnr = "<< jari_linear_psnr/ (end - 1) << endl;
	//cout << "jari_cubic_psnr = " << jari_cubic_psnr / (end - 1) << endl;
}
