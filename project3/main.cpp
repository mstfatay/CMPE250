#include <iostream>
#include <vector>
#include<list>
#include <fstream>
#include <deque>

using namespace std;



/*
returns true if the given graph is an eulerian graph.

If indegree and outdegree of all vertices of a graph are equal then that graph is an eulerian graph.
This function uses this information.
*/
bool isEulerian(vector<deque<int>>& vertices, int vertexNum) {
	vector<int> indegrees(vertexNum, 0); // indegrees of vertices.

	// iterate trough all edges
	for (int i = 0; i < vertexNum; i++) {
		for (int item : vertices[i]) {
			indegrees[item]++;  // We found an inconnection so increase that vertex's indegree by one
		}
	}
	for (int i = 0; i < vertexNum; i++) {
		if (vertices[i].size() != indegrees[i]) {
			// if any vertex's indegree not equal to its outdegree then return false
			return false;
		}
	}
	// all verteices' indegree are equal to theri outdegree so return true
	return true;
}


// this is a helper printing function to print iterables to an ostream. Default is cout.
template <class con>
void printToFile(con& c, ostream& out = cout) {
	for (auto a : c) {
		out << a << " ";
	}
}


int main(int argn, char* argv[])
{
	string inputFile = argv[1];
	string outputFile = argv[2];

	ifstream infile;  // input file stream
	ofstream outfile;  // output file stream

	infile.open(inputFile);
	outfile.open(outputFile);


	int vertexNum;  // number of vertices
	infile >> vertexNum;
	
	vector<deque<int>> vertices(vertexNum);  // the graph's adjacency list

	int edgeNum = 0; // the number of edges

	// getting input and creating our graph
	for (int i = 0; i < vertexNum; i++) {
		int vertexId, outdegree;
		infile >> vertexId >> outdegree;

		for (int j = 0; j < outdegree; j++) {
			int vertex;
			infile >> vertex;
			vertices[i].push_back(vertex);
			edgeNum++;
		}
	}

	int startVertex;
	infile >> startVertex;


	// check whether there is a possible eulerian path
	if (!isEulerian(vertices, vertexNum)) {
		outfile << "no path";
		return 0;
	}

	list<int> eulerianCircuit; // the eulerian circuit as a list
	eulerianCircuit.push_back(startVertex); // add starting vertex
	int currVertex = startVertex; 
	list<int>::iterator currIt = eulerianCircuit.end(); // this iterator is used to merge eulerian circuit with a new tour
	//list<int>::iterator beforeIt = eulerianCircuit.begin();


	/*
	Till our eulerian circuit is completed this loop will continue. To understand whether our eulerian 
	circuit is completed we check its size
	*/
	while (int(eulerianCircuit.size()) <= edgeNum) {
		list<int> tour; //A closed path we will find and then merge with the eulerian circuit.

		// loop till there is no new vertex we can travel. The vertex we stop should be the vertex we started the loop.
		while (vertices[currVertex].size() != 0) {
			// vetices[any_vertex] is sorted. So front is the minimum element.
			int newVertex = vertices[currVertex].front();
			vertices[currVertex].pop_front(); 
			currVertex = newVertex; // To continue traveling we update currVertex.
			tour.push_back(newVertex);
		}

		/*
		merge tour with eulerian circuit.
		CurrIt keeps the vertex which is successor of the vertex where tour starts. So currIt is the merge point.
		Because eulerian circuit and tour are list this operations runs in O(1) time complexity.
		*/
		eulerianCircuit.insert(currIt, tour.begin(), tour.end());
		
		//
		for (auto it = eulerianCircuit.begin(); it != eulerianCircuit.end(); it++) {
			if (vertices[*it].size() != 0) {
				currVertex = *it; // currVertex is the first vertex which has a path which hasn't traveled yet.
				currIt = ++it; // curIt keeps the place where mergeing happens.
				break;
			}
		}
	}
	printToFile(eulerianCircuit, outfile); // write eulerian cicuit to the output file.

	return 0;
}

