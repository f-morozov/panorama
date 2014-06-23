#ifndef COMPARE_H
#define COMPARE_H

#include <iostream>
#include <opencv2/core.hpp>

#include "akaze/AKAZE.h"
#include "binboost/BoostDesc.h"

using namespace std;
using namespace cv;

const int pano_size = 5;

class Compare
{
public:
	Compare(string detector, string descriptor);
	~Compare();
	Mat compareImages(vector<Mat> images);
	float descTime();
	float matchTime();
	float overallTime();

private:
	float match_time;
	float desc_time;
	int runs;
	int desc_num;

	static const float good_ratio = 0.8f;
	static const float ransac_thresh = 2.5f;
	static const int image_size = 300;

	AKAZE* akazer;
	Ptr<FeatureDetector> det;
	Ptr<DescriptorExtractor> desc;
	Ptr<DescriptorMatcher> matcher;
	bool akaze;
	bool binary;

	int countInliers(vector<KeyPoint> kp1, Mat desc1, vector<KeyPoint> kp2, Mat desc2);
	float toSec(float ms);
	bool isBGM(string descriptor);
	bool isBBoost(string descriptor);
	Ptr<DescriptorExtractor> getBoostExtractor(string descriptorType);
};

#endif
