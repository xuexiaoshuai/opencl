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


void getVector(string fileName, vector<Testcase>& test_cases){
	ifstream ifs(fileName.c_str());
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
}


int main(int argc, char** argv)
{
	if (argc < 3){
		cout << "error: command line parameter is not enough" << endl;
	}
	string file1 = argv[1];
	string file2 = argv[2];
	vector<Testcase> tune;
	getVector(file1.c_str(), tune);

	vector<Testcase> tuned;
	getVector(file2.c_str(), tuned);

	int count1 = 0, count2 = 0;


	for (unsigned i = 0; i < tune.size(); i++){
		Testcase t1 = tune[i];
		Testcase t2 = tuned[i];
		cout << t1->name << endl;
		for (unsigned j = 0; j < t1->nodes.size(); j++){
			Node n1 = t1->nodes[j];
			Node n2 = t2->nodes[j];

			double a = n1->gpu_pure_time;
			double b = n2->gpu_pure_time;
			double c;
			if (a > 0.0000000001){
				c = (a - b) / a * 100;
				if (c > 20.0){
					count1++;
				}
				if (c < -20.0){
					count2++;
				}
			}
			else{
				c = 0.0;
			}
			cout << "Non tuned kernel time " << a << ", tuned kernel time " << b;
			cout << ", tuned runtime is " << c << "\% faster" << endl;
		}
	}
	cout << "faster more than 20%: " << count1 << endl;
	cout << "slower more than 20%: " << count2 << endl;
        //cout<<tune.size()<<endl;
       // cout<<tuned.size()<<endl;
	cout << endl << "------end------" << endl;
	return 0;
}
