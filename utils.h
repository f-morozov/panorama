#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>

#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

int parseData(string fin_name, string fout_name);
void writeResults(Mat res, string fout_name);
Mat readResults(string fin_name);
float check(string good_path, string res_path, int start, int finish);

#endif
