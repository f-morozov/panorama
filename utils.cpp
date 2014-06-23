#include "utils.h"

#include <opencv2/highgui.hpp>
#include <fstream>

using namespace cv;
using namespace std;

void writeResults(Mat res, string fout_name)
{
	ofstream fout(fout_name.c_str());
	fout << res.rows << ' ' << res.cols << endl << endl;
	for(unsigned i = 0; i < res.rows; i++) {
		for(unsigned j = 0; j < res.cols; j++) {
		    fout << res.at<int>(i, j) << " ";
		}
		fout << endl;
	}
}

Mat readResults(string fin_name)
{
	ifstream fin(fin_name.c_str());
	int rows, cols;
	fin >> rows >> cols;
	Mat res(rows, cols, CV_32S);
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			int val;
			fin >> val;
			res.at<int>(i, j) = val;
		}
	}
	return res;
}

int parseData(string fin_name, string fout_name)
{
	ifstream fin(fin_name.c_str());
	int img_num = 5;

	int pan_num;
	fin >> pan_num;

	Mat good(Mat::ones(pan_num, img_num, CV_32S));

	while(!fin.eof())
	{
		string str;
		fin >> str;
		int pano, img;
		stringstream ss(str);
		(ss >> pano).ignore(1) >> img;

		if(pano > pan_num) {
			break;
		} else {
			good.at<int>(pano - 1, img - 1) = 0;	
		}
	}

	writeResults(good, fout_name);
	Mat res = readResults(fout_name);
	writeResults(res, fout_name);
	return 0;
}

float check(string good_path, string res_path, int start, int finish)
{
	Mat good = readResults(good_path);
	Mat res = readResults(res_path);
	good = good.rowRange(start, finish);

	if(good.size() != res.size()){
		return 0;
	} else {
		return countNonZero(good == res) * 1.0 / good.total();
	}
}
