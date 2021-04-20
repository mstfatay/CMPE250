#include <vector>
#include <deque>
#include <limits>

using namespace std;


#ifndef FLOW
#define FLOW



/**
	* This class solves min-cost-max-flow problem using a fast negative cycle detection
	* algorithm. 
	* First initialize using the constructor and then call calculateFlow function to 
	* solve the min-cost-max-flow problem. That function returns min-cost value.
*/
class MinCostMaxFlow {
	vector<vector<pair<int, int>>> cap; // residuential graph   pair<int,int> represents an edge => <to_vertex, capacity>
	vector<vector<pair<int, int>>> flow; // flow graph			pair<int,int> represents an edge => <to_vertex, flow>
	vector<vector<pair<int, int>>> cost; // costs graph			pair<int,int> represents an edge => <to_vertex, cost>

	int N; // number of vertices
	int source; // index of source vertex
	int sink; // index of sink vertex

	const int INF = numeric_limits<int>::max(); 


	/*
		last found agumenting path or negative cycle path.
		if it represents  an agumenting path -> first element is source vertex and last element is sink vertex.
		If it represents a negative cyle -> first and last elements are the same vertices.
	*/
	deque<int> path; 


	int bottleNeck; // deque<int> path 's bottle neck value (min capacity of edges of path)
	int totalCost = 0; // costs are added to this variable. At the end this value becomes the min-cost.

	void addToGraph(vector<vector<pair<int, int>>>& graph, int u, int v, int value);
	bool findAgumentingPath(int source, int sink);
	bool detectCycle(vector<int>& predecessor);
	bool findNegativeCycle();
	int getBottleNeck();
	void addFlow();


public:
	MinCostMaxFlow(vector<vector<pair<int, int>>>& cap, vector<vector<pair<int, int>>>& cost, int source, int sink);
	int calculateFlow();

};


#endif