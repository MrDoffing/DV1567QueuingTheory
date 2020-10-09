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

struct Node {
	string name = "";
	double demand = 0;
	double Q = 0;
	double Rprime = 0;
	Node(double _demand = 0, string _name = "") {
		demand = _demand;
		name = _name;
	}
};

struct System {
	int thinkTime = 0;
	vector<Node> nodes;
	double X = 0;
	double ResponseTime = 0;
	double concurrentUsers = 0;
	System(int _thinkTime, vector<Node>& _nodes) {
		thinkTime = _thinkTime;
		nodes = _nodes;
	}
	System() {}
};

void calculate(vector<vector<System>>& data) {
	// Calculate
	for (size_t iThink = 0; iThink < data.size(); iThink++)
	{
		vector<System>* iterations = &data[iThink];
		for (size_t iIter = 1; iIter < iterations->size(); iIter++)
		{
			System* system = &iterations->at(iIter);
			// calculate Rprime for all nodes
			for (size_t iNode = 0; iNode < system->nodes.size(); iNode++)
			{
				Node* node = &system->nodes[iNode];
				node->Rprime = node->demand * (1.0 + iterations->at(iIter - 1).nodes[iNode].Q);
				system->ResponseTime += node->Rprime;
			}
			// calculate throughput (Same for all nodes)
			system->X = iIter / (system->thinkTime + system->ResponseTime);
			// calculate concurrent users
			system->concurrentUsers = system->X * system->ResponseTime;
			// calculate queue length
			for (size_t iNode = 0; iNode < system->nodes.size(); iNode++)
			{
				Node* node = &system->nodes[iNode];
				node->Q = system->X * node->Rprime;
			}
		}
	}
}

void toFile(string filename, vector<vector<System>>& data) {
	// write data to file
	fstream file;
	file.open(filename, ios::out);
	if (file.is_open()) {
		string output = "nrOfPeople\t";
		for (size_t iThink = 0; iThink < data.size(); iThink++)
		{
			int thinkTime = data[iThink][0].thinkTime;
			string thStr = " (" + to_string(thinkTime) + "s)";
			output += "Throughput" + thStr + "\t" + "System ResTime" + thStr + "\t" + "Concurrent Users" + thStr + "\t";
			for (size_t iNode = 0; iNode < data[iThink][0].nodes.size(); iNode++)
			{
				string nodeName = data[iThink][0].nodes[iNode].name;
				output += nodeName + " Queue" + thStr + "\t" + nodeName + " ResTime" + thStr + "\t";
			}
		}
		output += "\n";
		for (size_t iIter = 0; iIter < data[0].size(); iIter++)
		{
			//nr of people
			output += to_string(iIter) + "\t";
			for (size_t iThink = 0; iThink < data.size(); iThink++)
			{
				System* system = &data[iThink][iIter];
				// Throughput - Sys resp - conc users
				output += to_string(system->X) + "\t" + to_string(system->ResponseTime) + "\t" + to_string(system->concurrentUsers) + "\t";
				// Queue - ResponseTime
				for (size_t iNode = 0; iNode < system->nodes.size(); iNode++)
					output += to_string(system->nodes[iNode].Q) + "\t" + to_string(system->nodes[iNode].Rprime) + "\t";
			}
			output += "\n";
		}

		output = replace(output, '.', ',');
		file << output;

		file.close();
	}
}

int main() {
	vector<Node> systemTest1_nodes = { 
		Node(0.0394,"Cpu"),
		Node(0.0771,"Disk1"),
		Node(0.1238,"Disk2"),
		Node(0.0804,"Disk3"),
		Node(0.235,"Disk4") 
	};
	double balancedDisksDemand = (0.0771 + 0.1238 + 0.0804 + 0.235) / 4;
	vector<Node> systemTest2_nodes_balancedDisks = { 
		Node(0.0394,"Cpu"),
		Node(balancedDisksDemand,"Disk1"),
		Node(balancedDisksDemand,"Disk2"),
		Node(balancedDisksDemand,"Disk3"),
		Node(balancedDisksDemand,"Disk4") 
	};
	vector<Node> systemTest3_nodes_TLS = { 
		Node(0.137,"Cpu"),
		Node(balancedDisksDemand,"Disk1"),
		Node(balancedDisksDemand,"Disk2"),
		Node(balancedDisksDemand,"Disk3"),
		Node(balancedDisksDemand,"Disk4") 
	};
	double coveringIndexIncrease = 1.0 / 1.55;
	vector<Node> systemTest4_nodes_CoveringIndex = { 
		Node(0.0394 * coveringIndexIncrease,"Cpu"),
		Node(balancedDisksDemand * coveringIndexIncrease,"Disk1"),
		Node(balancedDisksDemand * coveringIndexIncrease,"Disk2"),
		Node(balancedDisksDemand * coveringIndexIncrease,"Disk3"),
		Node(balancedDisksDemand * coveringIndexIncrease,"Disk4") 
	};
	double cursorSharingIncrease = 1.0 / 2.0;
	vector<Node> systemTest5_nodes_CursorSharing = { 
		Node(0.0394 * cursorSharingIncrease,"Cpu"),
		Node(balancedDisksDemand * cursorSharingIncrease,"Disk1"),
		Node(balancedDisksDemand * cursorSharingIncrease,"Disk2"),
		Node(balancedDisksDemand * cursorSharingIncrease,"Disk3"),
		Node(balancedDisksDemand * cursorSharingIncrease,"Disk4") 
	};

	vector<int> thinkTimes = { 2, 5, 10 };//in seconds

	// Allocate
	vector<vector<System>> systemTest1;
	systemTest1.resize(thinkTimes.size());
	vector<vector<System>> systemTest2_balancedDisks;
	systemTest2_balancedDisks.resize(thinkTimes.size());
	vector<vector<System>> systemTest3_TLS;
	systemTest3_TLS.resize(thinkTimes.size());
	vector<vector<System>> systemTest4_CoveringIndex;
	systemTest4_CoveringIndex.resize(thinkTimes.size());
	vector<vector<System>> systemTest5_CursorSharing;
	systemTest5_CursorSharing.resize(thinkTimes.size());
	for (size_t i = 0; i < thinkTimes.size(); i++)
	{
		systemTest1[i].resize(200); // nr of users
		for (size_t iIter = 0; iIter < systemTest1[i].size(); iIter++)
		{
			systemTest1[i][iIter] = System(thinkTimes[i], systemTest1_nodes);
		}
		systemTest2_balancedDisks[i].resize(300); // nr of users
		for (size_t iIter = 0; iIter < systemTest2_balancedDisks[i].size(); iIter++)
		{
			systemTest2_balancedDisks[i][iIter] = System(thinkTimes[i], systemTest2_nodes_balancedDisks);
		}
		systemTest3_TLS[i].resize(300); // nr of users
		for (size_t iIter = 0; iIter < systemTest3_TLS[i].size(); iIter++)
		{
			systemTest3_TLS[i][iIter] = System(thinkTimes[i], systemTest3_nodes_TLS);
		}
		systemTest4_CoveringIndex[i].resize(400); // nr of users
		for (size_t iIter = 0; iIter < systemTest4_CoveringIndex[i].size(); iIter++)
		{
			systemTest4_CoveringIndex[i][iIter] = System(thinkTimes[i], systemTest4_nodes_CoveringIndex);
		}
		systemTest5_CursorSharing[i].resize(500); // nr of users
		for (size_t iIter = 0; iIter < systemTest5_CursorSharing[i].size(); iIter++)
		{
			systemTest5_CursorSharing[i][iIter] = System(thinkTimes[i], systemTest5_nodes_CursorSharing);
		}
	}

	// Calculate
	calculate(systemTest1);
	calculate(systemTest2_balancedDisks);
	calculate(systemTest3_TLS);
	calculate(systemTest4_CoveringIndex);
	calculate(systemTest5_CursorSharing);

	toFile("data_test1.txt", systemTest1);
	toFile("data_test2_balancedDisks.txt", systemTest2_balancedDisks);
	toFile("data_test3_TLS.txt", systemTest3_TLS);
	toFile("data_test4_CoveringIndex.txt", systemTest4_CoveringIndex);
	toFile("data_test5_CursorSharing.txt", systemTest5_CursorSharing);


	return 0;
}