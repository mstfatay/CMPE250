#include "flow.h"



// -------------- PUBLIC METHODS ---------------

/**
	* the only constructor of this class.
	* @var cap capacitites graph of flow. A directed weighted adjacency list graph.
	* @var cost costs graph of flow. A directed weighted adjacency list graph.
	* @source index of source vertex of this flow.
	* @sink index of sink vertex of this flow.
*/
MinCostMaxFlow::MinCostMaxFlow(vector<vector<pair<int, int>>>& cap, vector<vector<pair<int, int>>>& cost, int source, int sink) {
	this->cap = cap;
	this->cost = cost;
	this->source = source;
	this->sink = sink;
	N = cap.size();

	this->flow = vector<vector<pair<int, int>>>(N);
}


/**
	* Solves the min-cost-max-flow problem and returns min-cost value.
	* @return min-cost value as int.
*/
int MinCostMaxFlow::calculateFlow() {
	// Find all agumenting paths and update cap, cost and flow graphs according to that path.
	while (findAgumentingPath(source, sink)) {
		int localBootleNeck = getBottleNeck();
		addFlow();
	}

	// Find all negative cycles and update cap, cost and flow graphs according to that path.
	while (findNegativeCycle()) {
		addFlow();
	}

	return totalCost;
}




// -------------- PRIVATE METHODS ---------------

/*
	given graph is a directed weighted adjacency list graph. This method adds value to the weight of u-v edge.
	If that edge doesn't exists, then creats an u-v edge with weight = value.
*/
void MinCostMaxFlow::addToGraph(vector<vector<pair<int, int>>>& graph, int u, int v, int value) {
	int pos;
	bool found = false;

	// detect whether u-v edge already exists.
	for (int i = 0; i < graph[u].size(); i++) {
		if (graph[u][i].first == v) {
			pos = i;
			found = true;
			break;
		}
	}

	if (found) {
		graph[u][pos].second += value;
	}
	else {
		graph[u].push_back({ v, value });
	}

}


/*
	Trys to find a new agumenting path using a bfs algortihm. If there is no agumenting path in cap graph, returns false.
	Otherwise updates private member: deque<int> path and returns true.
*/
bool MinCostMaxFlow::findAgumentingPath(int source, int sink) {
	path.clear(); // path will be updated.
	vector<bool> locked(N, false); // whether vertices are traversed.
	vector<int> predecessor(N, -2);
	deque<int> q; // queue

	q.push_back(source);
	locked[source] = true;
	predecessor[source] = -1;

	bool found = false;

	// bfs
	while (!q.empty() && !found) {
		int currv = q.back();
		q.pop_back();

		for (auto a : cost[currv]) {
			int tov = a.first;
			if (!locked[tov]) {
				q.push_back(tov);
				locked[tov] = true;
				predecessor[tov] = currv;

				if (tov == sink) {
					found = true;
					break;
				}
			}
		}
	}

	if (predecessor[sink] == -2) {
		return false;
	}

	// updates path (private member) to make it the path we just found.
	int ind = sink;
	path.push_front(ind);
	while (ind != source) {
		ind = predecessor[ind];
		path.push_front(ind);
	}

	return true;
}

/*
	Uses a bfs like algorithm to find a cycle in predecessor graph. IF there is any cycle in graph updates private member: path and
	returns true. Otherwise, returns false.
*/
bool MinCostMaxFlow::detectCycle(vector<int>& predecessor) {
	vector<bool> locked(N, false); // whether this vertex is traversed.
	path.clear();
	int ind = 0;
	path.push_front(ind);

	bool found = false;
	int i = 0;
	while (true) {
		if (ind == -2) {
			// That means the last path we found isn't a part of a cyle. So we make ind a non-traversed 
			// vertex to continue cyle searching.

			found = false;

			path.clear(); // Last path we found isn't a cycle so clear it to try to examine a new one.
			for (i; i < N; i++) {
				if (!locked[i]) {
					ind = i;
					found = true;
					path.push_front(ind);
					break;
				}
			}
			if (!found) {
				return false;
			}
		}

		// if the current vertex (ind) is path already then we find a cycle. 
		for (auto it = path.begin() + 1; it != path.end(); it++) {
			// first element of path is last added ind. So we started from second element in iteration.
			if (*it == ind) {
				// we found a cycle. Erase non-cycle part. Return true.
				path.erase(it + 1, path.end());
				return true;
			}
		}
		locked[ind] = true;
		ind = predecessor[ind];
		path.push_front(ind);
	}
}


/*
	Trys to find a negative cycle in costs graph. If there is such a cycle finds it, updates private member: deque<int> path 
	and returns true. Otherwise returns false. This method works like bellman-ford algorithm
*/
bool MinCostMaxFlow::findNegativeCycle() {
	vector<int> predecessor(N, -2);
	vector<int> costs(N, 0);
	vector<int> inside(N, false); // whether that vertex is inside the queue (q).
	path.clear();

	deque<int> q; // a queue
	for (int i = 0; i < N; i++) {
		q.push_back(i); // add every vertex to it initially.
	}

	bool found = false; // It will be true when we find a negative cycle.

	int counter = 0;

	
	while (!q.empty() && !found) {
		int currv = q.front();
		q.pop_front();
		inside[currv] = false;

		for (int k = 0; k < cost[currv].size(); k++) {
			int tov = cost[currv][k].first;
			int edgecost = cost[currv][k].second;
			int currcost = edgecost + costs[currv];
			if (costs[tov] > currcost) {
				// A less costed path to vertex:tov is found
				costs[tov] = currcost;
				predecessor[tov] = currv;

				counter += 1;

				// in order not to increase time complexity this part should be called once in an order of vertex number.
				// By doing some benchmark tests, I figure out that if that order is 2, this method works pretty good.
				if (counter == 2 * N) {
					counter = 0;
					//predecessor graph is a spanning tree if there is no cycle in it. If there is a cyle, that cyle must be
					// a negative cycle because costs of all of its vertices are non-positive.
					if (detectCycle(predecessor)) {
						found = true; // there is a cyle in predecessor graph so we found a negative cycle.
						break;
					}
				}

				if (!inside[tov]) {
					q.push_back(tov);
					inside[tov] = true;
				}
			}
		}
	}

	// Because after last time this part called above, there might be a cycle formed. This is the last check.
	if (!found && detectCycle(predecessor)) {
		found = true;
	}

	return found;
}


/*
	Finds bottle neck of last found path (private member deque<int> path).
*/
int MinCostMaxFlow::getBottleNeck() {
	int bootleNeck = INF;
	for (int i = 0; i < path.size() - 1; i++) {
		//iterate through path
		int val = INF;
		int ind = path[i];
		for (auto a : cap[ind]) {
			if (path[i + 1] == a.first) {
				val = a.second;
				break;
			}
		}
		if (val < bootleNeck) {
			bootleNeck = val;
		}
	}
	this->bottleNeck = bootleNeck;
	return bootleNeck;
}


/*
	In short, this method updats graphs according to last found agumenting path. It is a typical part
	of max flow algorithms.

	Adds bottle neck flow to last path found. Alse, updates cap graph by adding reverse flow to it and
	removing its edges with bottle neck capacities on the path. Also, updates cost graph by adding 
	negative of the reverse flow to it and removing its edges with bottle neck capacities on the path.
	That path might be an agumenting path or a negative circle, nothing changes because the operations
	are totally the same.
*/
void MinCostMaxFlow::addFlow() {
	for (int i = 0; i < path.size() - 1; i++) {
		// iterate trough path.

		int find = path[i]; // first ind
		int sind = path[i + 1]; // second ind

		int pos; // pos of where we find sind in cap graph
		for (int j = 0; j < cap[find].size(); j++) {
			if (cap[find][j].first == sind) {
				pos = j;
				break;
			}
		}
		totalCost += cost[find][pos].second * bottleNeck; // add cost of edge

		// add new edges to graphs
		addToGraph(cap, sind, find, bottleNeck);
		addToGraph(cost, sind, find, -cost[find][pos].second);
		addToGraph(flow, find, sind, bottleNeck);

		if (cap[find][pos].second == bottleNeck) {
			// remove edges with bottle neck capacities.
			cap[find].erase(cap[find].begin() + pos);
			cost[find].erase(cost[find].begin() + pos);
		}
		else {
			cap[find][pos].second -= bottleNeck;
		}
	}
}


