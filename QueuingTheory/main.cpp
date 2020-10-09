#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
//Liu. H, Software performance and scalability
//s.163

string replace(string source, char target, char replacer) {
	for (size_t i = 0; i < source.length(); i++)
		if (source[i] == target)
			source[i] = replacer;
	return source;
}

struct ModelInfo {
	int n;
	double Ui, Rprime, Ri, X, Qi;
	void print() {
		cout << "n = " << n << endl;
		cout << "Ui = " << Ui << "%" << endl;
		cout << "Rprime[n] = " << Rprime << endl;
		cout << "Ri = " << Ri << endl;
		cout << "X[n] = " << X << endl;
		cout << "Qi[n] = " << Qi << endl;
		cout << endl;
	}
	ModelInfo(int _n, double _Ui, double _Rprime, double _Ri, double _X, double _Qi) {
		n = _n;
		Ui = _Ui;
		Rprime = _Rprime;
		Ri = _Ri;
		X = _X;
		Qi = _Qi;
	}
};
class ClosedModel {
public:
	vector<double> Qi; // Queue length at node i
	vector<double> Rprime; // Residence time at queuing node i
	vector<double> X; //System throughput

	int m = 1; // Number of queuing nodes
	int N = 50; // Number of customers

	double Di = 0.0394;//0.25 // Service demand, Rprime[1] = Di -> Residence time at first queuing node
	double Z = 0.0;//2.0 // Think time //Big brain time

public:
	ClosedModel(){}
	ClosedModel(double _Di, double _Z, int _N) {
		Di = _Di;
		Z = _Z;
		N = _N;
		Qi.resize(_N);
		Rprime.resize(_N);
		X.resize(_N);
	}
	~ClosedModel() {}

	vector<ModelInfo> run() {
		Qi[0] = 0.0;

		//Iterate over the number(n) of customers
		vector<ModelInfo> iterations;
		iterations.reserve(N-1);
		for (int n = 1; n < N; n++)
		{
			Rprime[n] = Di * (1.0 + Qi[n - 1]);
			X[n] = n / (Z + Rprime[n]);
			Qi[n] = X[n] * Rprime[n];

			// open model
			double Ui = X[n] * Di * 100;
			double Ri = Di / (1.0 - Ui * 0.01);

			iterations.push_back(ModelInfo(n, Ui, Rprime[n], Ri, X[n], Qi[n]));
		}
		return iterations;
	}
};

int main() {
	// collect data
	int people = 200;
	float thinkTime = 10;
	ClosedModel cpu(0.0394, thinkTime, people);
	ClosedModel disk1(0.0771, thinkTime, people);
	ClosedModel disk2(0.1238, thinkTime, people);
	ClosedModel disk3(0.0804, thinkTime, people);
	ClosedModel disk4(0.235, thinkTime, people);
	ClosedModel diskAverage((0.0771 + 0.1238 + 0.0804 + 0.235) / 4, thinkTime, people);
	cout << 0.0771 + 0.1238 + 0.0804 + 0.235 + 0.0394 << endl;
	cout << 1.f/(0.0771 + 0.1238 + 0.0804 + 0.235+ 0.0394) << endl;
	vector<ModelInfo> iter_cpu = cpu.run();
	vector<ModelInfo> iter_disk1 = disk1.run();
	vector<ModelInfo> iter_disk2 = disk2.run();
	vector<ModelInfo> iter_disk3 = disk3.run();
	vector<ModelInfo> iter_disk4 = disk4.run();
	vector<ModelInfo> iter_diskAverage = diskAverage.run();

	// sort data
	int iterations = iter_cpu.size();
	vector<double> system_responseTime;
	system_responseTime.resize(iterations);
	vector<double> system_responseTime_balancedDiskLoad;
	system_responseTime_balancedDiskLoad.resize(iterations);
	vector<double> line_20Mark;
	line_20Mark.resize(iterations);
	vector<double> line_serviceTime3x;
	line_serviceTime3x.resize(iterations);
	for (size_t i = 0; i < iterations; i++)
	{
		iter_cpu[i].print();

		system_responseTime[i] = iter_cpu[i].Ri + iter_disk1[i].Ri + iter_disk2[i].Ri + iter_disk3[i].Ri + iter_disk4[i].Ri;
		system_responseTime_balancedDiskLoad[i] = iter_cpu[i].Ri + iter_diskAverage[i].Ri * 4;
		line_serviceTime3x[i] = (cpu.Di + disk1.Di + disk2.Di + disk3.Di + disk4.Di)*3;
		line_20Mark[i] = 20;
	}

	fstream file;

	//task 1 graph
	file.open("task1_responseTime_serviceTime3x.txt", ios::out);
	if (file.is_open()) {
		file << "Users\tResponseTime\t3 x ServiceTime" << endl;
		string output = "";
		for (size_t i = 0; i < system_responseTime.size(); i++)
		{
			output += to_string(i+1) + "\t" + to_string(system_responseTime[i]) + "\t" + to_string(line_serviceTime3x[i]) + "\n";
		}
		file << replace(output, '.', ',');
		file.close();
	}

	//task 2 graph
	file.open("task2_responseTime_20sMark.txt", ios::out);
	if (file.is_open()) {
		file << "Users\tResponseTime\t20s Mark" << endl;
		string output = "";
		for (size_t i = 0; i < system_responseTime.size(); i++)
		{
			output += to_string(i + 1) + "\t" + to_string(system_responseTime[i]) + "\t" + to_string(line_20Mark[i]) + "\n";
		}
		file << replace(output, '.', ',');
		file.close();
	}
	//task 2 graph disk bottleneck
	file.open("task2_disksResponseTime.txt", ios::out);
	if (file.is_open()) {
		file << "Users\tCpu\tDisk 1\tDisk 2\tDisk 3\tDisk 4" << endl;
		string output = "";
		for (size_t i = 0; i < system_responseTime.size(); i++)
		{
			output += to_string(i + 1) + "\t" + to_string(iter_cpu[i].Ri) + "\t" + to_string(iter_disk1[i].Ri) + "\t" + to_string(iter_disk2[i].Ri) + "\t" + to_string(iter_disk3[i].Ri) + "\t" + to_string(iter_disk4[i].Ri) +  "\n";
		}
		file << replace(output, '.', ',');
		file.close();
	}

	//task 3 graph
	file.open("task3_responseTime_20sMark_balancedDiskLoad.txt", ios::out);
	if (file.is_open()) {
		file << "Users\tResponseTime\t20s Mark" << endl;
		string output = "";
		for (size_t i = 0; i < system_responseTime.size(); i++)
		{
			output += to_string(i + 1) + "\t" + to_string(system_responseTime_balancedDiskLoad[i]) + "\t" + to_string(line_20Mark[i]) + "\n";
		}
		file << replace(output, '.', ',');
		file.close();
	}

	return 0;
}