#include "utils.h"
#include "compute.h"

#include <string>
#include <vector>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/nonfree.hpp>

using namespace std;
using namespace cv;

void printHelp()
{
	ifstream man_file("man.txt");
	cout << string((istreambuf_iterator<char>(man_file)), istreambuf_iterator<char>());
}

int main(int argc, char **argv)
{
	initModule_nonfree();

	if(argc < 2) {
		printHelp();
		return 0;
	}

	string mode(argv[1]);
	if(mode == "parse") {
		if(argc < 4) {
			cerr << "Parameters missing." << endl;
			return 1;
		}
		parseData(argv[2], argv[3]);
	} else if(mode == "compute") {
		if (argc < 8) {
			cerr << "Parameters missing." << endl;
			return 1;
		}
		stringstream ss(argv[6]);
		int start = 0;
		ss >> start;
		stringstream ss2(argv[7]);
		int finish = 0;
		ss2 >> finish;
		compute(argv[2], argv[3], argv[4], argv[5], start, finish);
	} else if(mode == "check") {
		if(argc < 6) {
			cerr << "Parameters missing." << endl;
			return 1;
		}
		stringstream ss(argv[4]);
		int start = 0;
		ss >> start;
		stringstream ss2(argv[5]);
		int finish = 0;
		ss2 >> finish;
		cout << check(argv[2], argv[3], start, finish) << endl;
	} else if(mode == "help") {
		printHelp();		
	} else {
		cerr << "Wrong program mode!" << endl;
		printHelp();
	}
	return 0;
}