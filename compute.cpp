#include "compute.h"
#include "utils.h"
#include "compare.h"

#include <iostream>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

const int min_pano = 3;
const int similar_thresh = 20;

vector<Mat> getImages(int index, string path);
Mat compareImages(vector<Mat> images);
Mat selectPanoram(Mat sim);
void dfs(int start, Mat sim, Mat& res);

int compute(string detector, string descriptor, string input, string output, int start, int finish)
{	
	Mat good(Mat::ones(finish - start, pano_size, CV_32S));
	int num = finish - start;

	Compare compare(detector, descriptor);

	cout << endl << "Starting panoram solving: " << detector << " " << descriptor << endl;
	cout << "0% complete";
    
	for(int i = start; i < finish; i++) {
		vector<Mat> images = getImages(i, input);
		Mat similarity = compare.compareImages(images);
		Mat res = selectPanoram(similarity);
		for(int j = 0; j < pano_size; j++) {
			good.at<int>(i - start, j) = res.at<int>(j);
		}
		
		double status = (i - start + 1) * 100.0 / num;
		cout << detector << " " << descriptor << ": " << status << "% complete" << endl;
		//cout.flush();
	}	
	cout << "\rPanoram solving finished" << endl;

	stringstream ss;
	ss << output << "/" << "res/" << detector << "_" << descriptor << ".txt";
	writeResults(good, ss.str());
	cout << "Results written to " << ss.str() << endl;

	float score = check("../data.txt", ss.str(), start, finish);

	ofstream stat((output + string("/stats.txt")).c_str(), ofstream::app);
	stat << detector << " " << descriptor << " " 
		 << start << " " << finish << " "
		 << score << " " 
		 << compare.descTime() / num<< " "  
		 << compare.overallTime() / num << endl;
	stat.close();

	cout << endl;
	cout << "Score: " << score << endl;
	return 0;
}

vector<Mat> getImages(int pano_index, string path)
{
	vector<Mat> images(pano_size, Mat());
	string ext(".jpg");

	for(int i = 0; i < pano_size; i++) {
		stringstream ss;
		ss << path << "/" << pano_index + 1 << "_" << i + 1 << ext;
		images[i] = imread(ss.str(), IMREAD_GRAYSCALE);		
	}
	return images;
}

Mat selectPanoram(Mat sim)
{
	Mat res(Mat::zeros(1, pano_size, CV_32S));

	int max_sum = 0;
	int max_i = 0;
	for(int i = 0; i < pano_size; i++) {	
		int s = sum(sim.row(i))[0];
		if(s > max_sum) {
			max_sum = s;
			max_i = i;
		}
	}

	dfs(max_i, sim, res);
	if(sum(res)[0] < min_pano) {
		res = 1 - res;
	}
	return res;
}

void dfs(int start, Mat sim, Mat& res)
{
	if(!res.at<int>(start)) {
		res.at<int>(start) = 1;
		for(int i = 0; i < pano_size; i++) {
			if(sim.at<int>(start, i) > similar_thresh) {
				dfs(i, sim, res);
			}
		}
	}
}

