#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
using namespace std;

typedef struct node{
	double cpu_time;
	double gpu_time;
	double gpu_pure_time;
} *Node;


typedef struct testcase{
	string name;
	vector<Node> nodes;
} *Testcase;



int main(int argc, char** argv)
{
	if (argc < 2){
		cout << "error: command line parameter is not enough" << endl;
	}
	
	string fileName = argv[1];
	ifstream ifs(fileName.c_str());
	vector<Testcase> test_cases;

	if (ifs.fail()){
		cout << "read file error" << endl;
	}
	string buf;
	bool endflag = false;
	while (getline(ifs, buf)){
		while (buf.find("RUN") == buf.npos){
			getline(ifs, buf);
			if (buf.find("PASSED") != buf.npos){
				endflag = true;
				break;
			}
		}
		if (endflag){
			break;
		}

	
		Testcase t = new testcase;
		buf.erase(0, 13);
		t->name = buf;
		//		cout << buf << endl;

		getline(ifs, buf);
		double cpu_time = 0.0;
		double gpu_time = 0.0;
		double gpu_pure_time = 0.0;
		while (buf.find("OK") == buf.npos){
			//getline(ifs, buf);
			//cout << buf << endl;

			if (buf.find("average cpu") != buf.npos){
				double tmp;
				int index1 = buf.find("is");
				int index2 = buf.find("ms", index1);
				index1 += 4;
				tmp = atof(buf.substr(index1, index2 - index1).c_str());
				cpu_time = tmp;
				
			}
			else if (buf.find("average ocl runtime is") != buf.npos){

				double tmp;
				int index1 = buf.find("is");
				int index2 = buf.find("ms", index1);
				index1 += 4;
				tmp = atof(buf.substr(index1, index2 - index1).c_str());
				gpu_time = tmp;
				Node n = new node;
				n->cpu_time = cpu_time;
				n->gpu_time = gpu_time;
				n->gpu_pure_time = gpu_pure_time;
				t->nodes.push_back(n);
				cpu_time = 0.0;
				gpu_time = 0.0;
			}
			else if (buf.find("without") != buf.npos){
				double tmp;
				int index1 = buf.find("is");
				int index2 = buf.find("ms", index1);
				index1 += 4;
				tmp = atof(buf.substr(index1, index2 - index1).c_str());
				gpu_pure_time = tmp;
				t->nodes[t->nodes.size() - 1]->gpu_pure_time = gpu_pure_time;
				gpu_pure_time = 0.0;

			}
			getline(ifs, buf);
		}
		test_cases.push_back(t);

	}
	ifs.close();

	//	cout << test_cases.size() << endl;
	for (unsigned i = 0; i < test_cases.size(); i++){
		Testcase t = test_cases[i];
		cout << t->name << endl;
		for (int j = 0; j < t->nodes.size(); j++){
			Node n = t->nodes[j];
			//cout << n->cpu_time << endl;
			if (n->cpu_time < 0.0000001){
				cout << "no cpu time" << endl;
				continue;
			}
			cout << "speed up: " << (n->cpu_time / n->gpu_time) << endl;
			if (n->gpu_pure_time < 0.0000001){
				cout << "kernel speed up: no kernel time" << endl;
			}
			else{
				cout << "kernel speed up: " << (n->cpu_time / n->gpu_pure_time) << endl;
			}
			
		}
	}

//	cout << test_cases.size() << endl;
	cout << endl << "------end------" << endl;
	return 0;
}
