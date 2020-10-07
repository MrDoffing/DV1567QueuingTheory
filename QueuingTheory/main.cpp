#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
//Liu. H, Software performance and scalability
//s.163

string printWithComma(double val) {
	//return to_string(val);
	if (isinf(val))
		return "inf";
	long round = val;
	double rest = val - round;
	int decimals = 3;
	return to_string(round)+","+to_string(long(rest*pow(10,decimals)));
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
	int people = 200;
	float thinkTime = 30;
	ClosedModel cpu(0.0394, thinkTime, people);
	ClosedModel disk1(0.0771, thinkTime, people);
	ClosedModel disk2(0.1238, thinkTime, people);
	ClosedModel disk3(0.0804, thinkTime, people);
	ClosedModel disk4(0.235, thinkTime, people);

	vector<ModelInfo> iter_cpu = cpu.run();
	vector<ModelInfo> iter_disk1 = disk1.run();
	vector<ModelInfo> iter_disk2 = disk2.run();
	vector<ModelInfo> iter_disk3 = disk3.run();
	vector<ModelInfo> iter_disk4 = disk4.run();

	int iterations = iter_cpu.size();
	vector<double> system_responseTime;
	system_responseTime.resize(iter_cpu.size());
	for (size_t i = 0; i < iterations; i++)
	{
		iter_cpu[i].print();
		system_responseTime[i] = iter_cpu[i].Ri + iter_disk1[i].Ri + iter_disk2[i].Ri + iter_disk3[i].Ri + iter_disk4[i].Ri;
		//system_responseTime[i] = iter_cpu[i].Rprime + iter_disk1[i].Rprime + iter_disk2[i].Rprime + iter_disk3[i].Rprime + iter_disk4[i].Rprime;
	}

	fstream file;
	file.open("system_responseTime.txt", ios::out, ios::trunc);
	if (file.is_open()) {
		file << "Users\tResponseTime" << endl;
		string output = "";
		for (size_t i = 0; i < system_responseTime.size(); i++)
		{
			output += to_string(i+1) + "\t" + to_string(system_responseTime[i]) + "\n";
		}
		for (size_t i = 0; i < output.length(); i++)
		{
			if (output[i] == '.')
				output[i] = ',';
		}
		file << output;
		file.close();
	}

	return 0;
}