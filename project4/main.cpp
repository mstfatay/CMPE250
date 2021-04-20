#include <vector>
#include <fstream>
#include "flow.h"

using namespace std;


int main(int argv, char** argc) {
	ifstream infile;
	infile.open(argc[1]);
	ofstream outfile;
	outfile.open(argc[2]);

	int T; // number of test cases
	infile >> T;

	for (int k = 0; k < T; k++) {
		int N; // number of vertices
		infile >> N;
		vector<vector<pair<int, int>>> cap(2 * N + 2); // capacities graph as an adjecency list
		vector<vector<pair<int, int>>> cost(2 * N + 2);	// costs graph as an adjecency list
		for (int i = 0; i < N; i++) {
			for (int j = N; j < 2 * N; j++) {
				int c;
				infile >> c;
				cost[i].emplace_back(j, -c); // add negatives of values as cost. Because MinCostMaxFlow minimizes cost. So the voltage will be maximized.
				cap[i].emplace_back(j, 1); // make all capacities 1. Because only every cable can be pluged in one port.
			}

			// add edges begining at source
			cost[2 * N].emplace_back(i, 0);
			cap[2 * N].emplace_back(i, 1);

			// add edges ending at sink
			cost[N + i].emplace_back(2 * N + 1, 0);
			cap[N + i].emplace_back(2 * N + 1, 1);
		}

		// create a MinCostMaxFlow object and calculate flow.
		MinCostMaxFlow mcmf(cap, cost, 2 * N, 2 * N + 1);
		int totalCost = mcmf.calculateFlow();

		outfile << -totalCost << endl; // It is negative because the cost graph consists of negatives of costs.
	}
}
