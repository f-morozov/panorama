#include "compare.h"
#include "akaze/AKAZE.h"
#include "binboost/BoostDesc.h"

#include <iostream>
#include <vector>
#include <ctime>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/nonfree.hpp>

using namespace std;
using namespace cv;
using namespace boostDesc;

Compare::Compare(string detector, string descriptor) : match_time(0), desc_time(0), akazer(NULL), binary(true), 
												       akaze(false), runs(0), desc_num(0)
{
	if(descriptor == "AKAZE" || detector == "AKAZE") {
		akaze = true;
		AKAZEOptions options;
		options.img_width = image_size;
		options.img_height = image_size;
		akazer = new AKAZE(options);
	}

	if(descriptor == "SIFT" || descriptor == "SURF" || isBGM(descriptor)) {
		binary = false;
	}
	if(binary) {
		matcher = DescriptorMatcher::create("BruteForce-Hamming");
	} else {
		matcher = DescriptorMatcher::create("BruteForce-L1");
	}

	if(isBGM(descriptor) || isBBoost(descriptor)) {
		desc = getBoostExtractor(descriptor);
	} else if(!akaze){
		desc = DescriptorExtractor::create(descriptor.c_str());
	}

	if(!akaze) {
		det = FeatureDetector::create(detector.c_str());
	}
}

Compare::~Compare()
{
	if(akazer) {
		delete akazer;
		akazer = NULL;
	}
}

Mat Compare::compareImages(vector<Mat> images)
{
	runs++;

	Mat sim(Mat::zeros(pano_size, pano_size, CV_32S));
	vector< vector<KeyPoint> > keypoints(pano_size, vector<KeyPoint>());
	vector<Mat> descriptors(pano_size, Mat());
	
	for(int i = 0; i < pano_size; i++) {
		if(akaze) {
			Mat img32;
			images[i].convertTo(img32, CV_32F, 1.0/255.0, 0);
			
			akazer->Create_Nonlinear_Scale_Space(img32);
			akazer->Feature_Detection(keypoints[i]);
			int start = clock();
			akazer->Compute_Descriptors(keypoints[i],descriptors[i]);
			desc_num += descriptors[i].rows;
			desc_time += clock() - start;
		} else {
			det->detect(images[i], keypoints[i]);
			int start = clock();
			desc->compute(images[i], keypoints[i], descriptors[i]);
			desc_num += descriptors[i].rows;
			desc_time += clock() - start;
		}
	}

	for(int i = 0; i < pano_size; i++) {
		for(int j = 0; j < pano_size; j++) {
			if(j == i) {
				sim.at<int>(i, j) = 0;
			} else {
				float single_match;
				sim.at<int>(i, j) = countInliers(keypoints[i], descriptors[i], 
				 							 keypoints[j], descriptors[j]);
				sim.at<int>(j, i) = sim.at<int>(i, j);
			}
		}
	}
	return sim;
}

int Compare::countInliers(vector<KeyPoint> kp1, Mat desc1, vector<KeyPoint> kp2, Mat desc2)
{
	if(desc1.rows == 0 || desc2.rows == 0)
		return 0;
	vector< vector<DMatch> > matches;
	int start = clock();
	matcher->knnMatch(desc1, desc2, matches, 2);
	match_time += clock() - start;

	vector<Point2f> matched1;
	vector<Point2f> matched2;
	for(unsigned i = 0; i < matches.size(); i++) {
		DMatch first = matches[i][0];

		float dist1 = matches[i][0].distance;
    	float dist2 = matches[i][1].distance;

    	if(dist1 < good_ratio * dist2) {
    		matched1.push_back(kp1[first.queryIdx].pt);
			matched2.push_back(kp2[first.trainIdx].pt);
    	}
	}
	if(matched1.size() < 4) {
		return 0;
	}

	Mat mask;
	Mat homography = findHomography(matched1, matched2, RANSAC, ransac_thresh, mask);
	return countNonZero(mask);
}

float Compare::descTime()
{
	return toSec(desc_time / desc_num) / runs;
}

float Compare::matchTime()
{
	return toSec(match_time / (pano_size * pano_size)) / runs;
}

float Compare::overallTime()
{
	return toSec(desc_time + match_time) / runs;
}

float Compare::toSec(float ms)
{
	return 1000.0 * ms / CLOCKS_PER_SEC;
}

bool Compare::isBGM(string descriptor) 
{
	const string bgm("BGM");
	const string lbgm("LBGM");
	return !descriptor.compare(0, bgm.size(), bgm) || !descriptor.compare(0, lbgm.size(), lbgm); 
}

bool Compare::isBBoost(string descriptor) 
{
	const string bboost("BINBOOST");
	return !descriptor.compare(0, bboost.size(), bboost);
}

Ptr<DescriptorExtractor> Compare::getBoostExtractor(string descriptorType)
{
 	Ptr<DescriptorExtractor> extractor;
	if (descriptorType == "BGM" || descriptorType == "BINBOOST_1" || descriptorType == "BINBOOST_1-256")
		extractor = Ptr<DescriptorExtractor>(new BGM());
	else if (descriptorType == "BGM-HARD" || descriptorType == "BINBOOST_1-HARD")
	  	extractor = Ptr<DescriptorExtractor>(new BGM("../binboost/matrices/bgm_hard.bin"));
	else if (descriptorType == "BGM-BILINEAR" || descriptorType == "BINBOOST_1-BILINEAR")
		extractor = Ptr<DescriptorExtractor>(new BGM("../binboost/matrices/bgm_bilinear.bin"));
	else if (descriptorType == "LBGM" || descriptorType == "FPBOOST" || descriptorType == "FPBOOST_512" || descriptorType == "FPBOOST_512-64")
		extractor = Ptr<DescriptorExtractor>(new LBGM());
	else if (descriptorType == "BINBOOST" || descriptorType == "BINBOOST_128" || descriptorType == "BINBOOST_128-64")
		extractor = Ptr<DescriptorExtractor>(new BinBoost());
	else if (descriptorType == "BINBOOST-128" || descriptorType == "BINBOOST_128-128")
	    extractor = Ptr<DescriptorExtractor>(new BinBoost("../binboost/matrices/binboost_128.bin"));
	else if (descriptorType == "BINBOOST-256" || descriptorType == "BINBOOST_128-256")
	    extractor = Ptr<DescriptorExtractor>(new BinBoost("../binboost/matrices/binboost_256.bin"));
	return extractor;
}