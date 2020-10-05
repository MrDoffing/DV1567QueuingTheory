#include <iostream>
#include <string>
using namespace std;
//Liu. H, Software performance and scalability
//s.163
class ClosedModel {
private:
	double Qi[100] = { 0 }; // Queue length at node i
	double Rprime[100] = { 0 }; // Residence time at queuing node i
	double X[100] = { 0 }; //System throughput

	int m = 1; // Number of queuing nodes
	int N = 50; // Number of customers

	double Di = 0.25; // Rprime[1] = Di -> Residence time at first queuing node
	double Z = 2.0; // Think time //Big brain time

public:
	ClosedModel(){}
	ClosedModel(double _Di, double _Z) {
		double Di = _Di; //Service demand
		double Z = _Z; // Think time
	}
	~ClosedModel() {}

	void run() {
		Qi[0] = 0.0;

		//Iterate over the number(n) of customers
		for (int n = 1; n < N; n++)
		{
			Rprime[n] = Di * (1.0 + Qi[n - 1]);
			X[n] = n / (Z + Rprime[n]);
			Qi[n] = X[n] * Rprime[n];

			// open model
			double Ui = X[n] * Di * 100;
			double Ri = Di / (1.0 - Ui * 0.01);

			cout << "n = " << n << endl;
			cout << "Ui = " << Ui << "%" << endl;
			cout << "Rprime[n] = " << Rprime[n] << endl;
			cout << "Ri = " << Ri << endl;
			cout << "X[n] = " << X[n] << endl;
			cout << "Qi[n] = " << Qi[n] << endl;
			cout << endl;
		}
	}
};

int main() {
	ClosedModel model;
	model.run();

	return 0;
}