#include "graph.h"

// add graph instance to oeka class


DSU::DSU(int n)
{
	parent = new int[n];
	rank = new int[n];

	for (int i = 0; i < n; i++) 
	{
		parent[i] = -1;
		rank[i] = 1;
	}
}


// Find function 
int DSU::find(int i)
{
	if (parent[i] == -1)
	{
		return i;
	}

	return parent[i] = find(parent[i]);
}


// Union function 
void DSU::unite(int x, int y)
{
	int s1 = find(x);
	int s2 = find(y);

	if (s1 != s2) 
	{
		if (rank[s1] < rank[s2]) 
		{
			parent[s1] = s2;
		}
		else if (rank[s1] > rank[s2]) 
		{
			parent[s2] = s1;
		}
		else 
		{
			parent[s2] = s1;
			rank[s1]++;
		}
	}
}


graph::graph(std::vector<std::vector<dvector>>* _hanselChainSet)
{
	// initialize member variables
	hanselChainSet = _hanselChainSet;
	numChains = (int)hanselChainSet->size();
	vectorDimension = (int)hanselChainSet->at(0)[0].dataPoint.size();

	for (int i = 0; i < numChains - 1; i++)
	{
		for (int j = i + 1; j < numChains; j++)
		{
			//completeGraphEdgeList.push_back({ i, j, chainHammingDistance(i, j) });
			completeGraphEdgeList.push_back({ i, j, chainHausdorffDistance(i, j) });
			//completeGraphEdgeList.push_back({ i, j, groupAverageHammingDistance(i, j) });
			
			// below doesn't work currently
			//completeGraphEdgeList.push_back({ i, j, expansionDistance(i, j) });
		}
	}
}


std::vector<std::vector<double>> graph::createAdjMatrix()
{
	std::vector<std::vector<double>> adjMatrix(numChains, std::vector<double>(numChains));

	// create adjacency matrix from edge list
	// not the most efficient, but edge list is already created
	for (auto& edge : completeGraphEdgeList)
	{
		adjMatrix[std::get<0>(edge)][std::get<1>(edge)] = std::get<2>(edge);
	}

	return adjMatrix;
}


std::vector<std::vector<dvector>> graph::simpleSort()
{
	std::sort(completeGraphEdgeList.begin(), completeGraphEdgeList.end(),
		[](auto const& l, auto const& r)
		{
			return std::get<2>(l) < std::get<2>(r);
		});

	auto nhcs = createNewHanselChainSet(completeGraphEdgeList);

	return nhcs;
}


std::pair<std::vector<std::vector<dvector>>, std::vector<std::tuple<int, int, double>>> graph::kruskal()
{
	/// @brief resultant edgeList of minimal spanning tree
	std::vector<std::tuple<int, int, double>> minSpanTreeEdgeList;
	minSpanTreeEdgeList.reserve(numChains);

	std::sort(completeGraphEdgeList.begin(), completeGraphEdgeList.end(),
		[](auto const& l, auto const& r) 
		{ 
			return std::get<2>(l) < std::get<2>(r); 
		});

	DSU set(numChains);
	double cost = 0;

	for (auto edge : completeGraphEdgeList) 
	{
		int x = std::get<0>(edge); 
		int y = std::get<1>(edge);
		double w = std::get<2>(edge);

		// Take this edge in MST if it does 
		// not forms a cycle 
		if (set.find(x) != set.find(y)) 
		{
			set.unite(x, y);
			cost += w;

			minSpanTreeEdgeList.push_back(edge);
		}
	}

	std::cout << "Minimum Cost Spanning Tree: " << cost;

	auto nhcs = createNewHanselChainSet(completeGraphEdgeList);

	return std::make_pair<>(nhcs, minSpanTreeEdgeList);
}


// implementation modified from https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
std::vector<std::vector<dvector>> graph::dijkstra(int src)
{
	std::vector<int> shortestPath;
	shortestPath.reserve(numChains);

	auto adjMatrix = createAdjMatrix();

	// The output array.  dist[i] will hold the
	// shortest
	// distance from src to i
	std::vector<double> dist(numChains, DBL_MAX); 

	// sptSet[i] will be true if vertex i is
	// included in shortest
	// path tree or shortest distance from src to i is
	// finalized
	std::vector<bool> sptSet(numChains, false); 
	
	// Distance of source vertex from itself is always 0
	dist[src] = 0;

	// Find shortest path for all vertices
	for (int count = 0; count < numChains; count++) 
	{
		// Pick the minimum distance vertex from the set of
		// vertices not yet processed. u is always equal to
		// src in the first iteration.
		int u = minDistance(dist, sptSet);

		// Mark the picked vertex as processed
		sptSet[u] = true;
		shortestPath.push_back(u);

		// Update dist value of the adjacent vertices of the
		// picked vertex.
		for (int v = 0; v < numChains; v++)
		{
			// Update dist[v] only if is not in sptSet,
			// there is an edge from u to v, and total
			// weight of path from src to  v through u is
			// smaller than current value of dist[v]
			if (!sptSet[v] && adjMatrix[u][v]
				&& dist[u] != INT_MAX
				&& dist[u] + adjMatrix[u][v] < dist[v])
			{
				dist[v] = dist[u] + adjMatrix[u][v];
			}
		}
	}

	std::vector<std::vector<dvector>> newHanselChainSet;
	newHanselChainSet.reserve(numChains);

	for (int& v : shortestPath)
	{
		newHanselChainSet.push_back(hanselChainSet->at(v));
	}

	return newHanselChainSet;
}


// modified from https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
int graph::minDistance(std::vector<double>& dist, std::vector<bool>& sptSet)
{
	// Initialize min value
	double min = DBL_MAX;
	int min_index = 0;

	for (int v = 0; v < numChains; v++)
	{
		if (sptSet[v] == false && dist[v] <= min)
		{
			min = dist[v];
			min_index = v;
		}
	}

	return min_index;
}


std::vector<std::vector<dvector>> graph::createNewHanselChainSet(std::vector<std::tuple<int, int, double>> edgeList)
{
	std::vector<bool> visited(numChains, false);
	std::vector<std::vector<dvector>> newHanselChainSet;
	newHanselChainSet.reserve(numChains);

	for (auto& edge : edgeList)
	{
		int x = std::get<0>(edge);
		int y = std::get<1>(edge);

		if (!visited[x])
		{
			newHanselChainSet.push_back(hanselChainSet->at(x));
			visited[x] = true;
		}

		if (!visited[y])
		{
			newHanselChainSet.push_back(hanselChainSet->at(y));
			visited[y] = true;
		}
	}

	return newHanselChainSet;
}


double graph::hammingDistance(int l, int r, int li, int rj)
{
	double hammingDistance = 0;

	for (int k = 0; k < vectorDimension; k++)
	{
		hammingDistance += abs(hanselChainSet->at(l)[li].dataPoint[k] - hanselChainSet->at(r)[rj].dataPoint[k]);
	}

	return hammingDistance;
}


double graph::chainHammingDistance(int l, int r)
{
	std::vector<dvector>* smallChain;
	std::vector<dvector>* largeChain;
	int largeChainSize;

	// find minimum size between the two hansel chains
	if (hanselChainSet->at(l).size() < hanselChainSet->at(r).size())
	{
		smallChain = &hanselChainSet->at(l);
		largeChain = &hanselChainSet->at(r);
	}
	else
	{
		smallChain = &hanselChainSet->at(r);
		largeChain = &hanselChainSet->at(l);
	}

	largeChainSize = (int)largeChain->size();

	// if you take the size of the larger hanselChain and subtract the size of the smaller hansel chain and divide the result by 2:
	// the result is the index location of the vector with the same Hamming Norm as the first vector of the smaller chain
	// due to this property, we can simply do this to find where to compare the two different hansel chains
	int start = (largeChainSize - (int)smallChain->size()) / 2;
	int end = largeChainSize - start;
	double hammingDistance = 0;

	for (int i = 0; i < largeChainSize; i++)
	{
		// directly compare i-th vector in large chain to [i-th - start] vector in small chain
		if (start <= i && i < end)
		{
			for (int j = 0; j < vectorDimension; j++)
			{
				if (largeChain->at(i).dataPoint[j] != smallChain->at(i - start).dataPoint[j])
				{
					// this is to accomodate for k-value, but maybe leave it so that we only add 1 to hammingDistance if not equal
					hammingDistance += abs(largeChain->at(i).dataPoint[j] - smallChain->at(i - start).dataPoint[j]);
				}
			}
		}
		// else, the hamming distance is the size of the vector
		// maybe change this to also include k-value?
		/*else
		{
			hammingDistance += vectorDimension;
		}*/
	}

	return hammingDistance / (int)smallChain->size();
	//return hammingDistance / (largeChainSize + (int)smallChain->size());
}


double graph::chainHausdorffDistance(int l, int r)
{
	std::vector<double> distances;

	for (int i = 0; i < (int)hanselChainSet->at(l).size(); i++ )
	{
		double minDistance = DBL_MAX;

		// find minimum distance of element in one chain to an element in another chain
		for (int j = 0; j < (int)hanselChainSet->at(r).size(); j++)
		{
			double d = hammingDistance(l, r, i, j);

			if (d < minDistance)
			{
				minDistance = d;
			}
		}

		// don't add it when the inner for loop isnt executed...
		if (minDistance != DBL_MAX)
		{
			distances.push_back(minDistance);
		}
	}

	// find maximum distance of min distances
	double hausDistance = *std::max_element(distances.begin(), distances.end());

	return hausDistance;
}


double graph::groupAverageHammingDistance(int l, int r)
{
	double distance = 0;

	for (int i = 0; i < (int)hanselChainSet->at(l).size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet->at(r).size(); j++)
		{
			distance += hammingDistance(l, r, i, j);
		}
	}

	distance /= (int)(hanselChainSet->at(l).size() * hanselChainSet->at(r).size());

	return distance;
}


double graph::expansionDistance(int l, int r)
{
	auto adjMatrix = createAdjMatrix();

	double distance1 = 0;
	double distance2 = 0;

	for (auto& dl : hanselChainSet->at(l))
	{
		for (auto expansion : dl.down_expandable)
		{
			for (auto dr : hanselChainSet->at(r))
			{
				if (expansion == &dr)
				{
					distance1 += adjMatrix[dl.number.first - 1][dr.number.first - 1];
				}
			}
		}
	}

	double distance = std::max(distance1, distance2);

	// the higher the value, the lower the distance, so invert the numbers
	return 1 / distance;
}
