#include "oeka.h"


std::vector<std::vector<dvector>> moeka::genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<dvector>>> chain)
{
	std::unordered_map<int, std::vector<std::vector<dvector>>> chains = chain;

	// add key string "0","1",...,num to the beginning of each element in the corresponding key
	for (int j = 0; j < num; j++)
	{
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			for (int l = 0; l < chains.at(j)[k].size(); l++)
			{
				std::vector<int> b(vector_dimension);
				b[vector_dimension - 1] = j;

				for (int i = vector_dimension - 1; i >= 1; i--)
				{
					// FIX
					b[i - 1] = chains.at(j)[k][l].dataPoint[i]; // used to be for std::vector: b[i] = chains.at(j)[k][l][i - 1];
				}

				// doen't work
				//std::reverse(b.begin(), b.end()); // its ugly, but i couldnt figure out how to do this in the immediete above code in the 10 min i tried. not as simple as it looks?

				//b.setLevel();
				dvector c;
				c.dataPoint = b;
				chains.at(j)[k][l] = c;
			}
		}
	}

	// move the largest of each sub std::vector to corresponding sub std::vector in the first key
	// then move the largest of each subsequent sub std::vector to the second key's sub std::vector
	// repeat until chain.at(num-2) is reached since the last key will have nothing moved to it
	// j = current key being added to
	// emptV = number of empty std::vectors
	for (int j = 0; j < chains.size() - 1; j++)
	{
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			// I think I need to make the code below recursive
			int count = j;

			for (count; count < chains.size() - 1; count++)
			{
				if (count + 1 >= chain.size())
				{
					break;
				}

				/// USE FIND AND END because kth std::vector doesn't exist for 3d 3val, k=2
				if (k > chains.at(count + 1).size() - 1)
				{
					break;
				}

				if (chains.at(count + 1).at(k).empty())
				{
					// ### DEAL WITH EMPTY std::vectorS~!###
					// chains.at(to_string(count + 1)).at(k).erase();
					break;
				}

				int end = (int)chains.at(count + 1).at(k).size();

				// add last element from position k of key count+1 to position k of j
				chains.at(j)[k].push_back(chains.at(count + 1)[k].at(end - 1));

				// pop off the moved element
				chains.at(count + 1).at(k).pop_back();
			}
		}
	}

	// we need to prepare for the next iteration of i in the first for loop
	// by adding all chains together, then adding combined chains to each key
	for (int j = 1; j < num; j++)
	{
		for (int k = 0; k < chains.at(j).size(); k++)
		{
			chains.at(0).push_back(chains.at(j)[k]);
		}
	}

	for (int j = 0; j < num; j++) 
	{
		chains.at(j) = chains.at(0);
	}

	std::vector<std::vector<dvector>> chainsTogether;

	return chainsTogether = chains.at(0);
}

 
void moeka::calculateHanselChains(int vector_dimension)
{
	// For n dimensions, iterate through to generate chains and
	for (int dim_num = 0; dim_num < vector_dimension; dim_num++)
	{
		int num = attribute_names[dim_num].kv;

		// Need a base chain to use for genChains
		if (dim_num == 0)
		{
			//generate the base chain
			std::vector<dvector> baseChain;

			for (int k = 0; k < num; k++)
			{
				std::vector<int> b(vector_dimension);
				b[vector_dimension - 1] = k;
				dvector c;
				c.dataPoint = b;
				//b.setLevel();
				baseChain.push_back(c);
			}

			//create a std::vector of std::vectors to hold the baseChain
			hanselChainSet.push_back(baseChain);
		}
		else
		{
			std::unordered_map<int, std::vector<std::vector<dvector>>> chainNum;

			for (int k = 0; k < num; k++)
			{
				chainNum[k] = hanselChainSet;
			}

			hanselChainSet = genChains(num, vector_dimension, chainNum);
		}
	}

	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		if (!hanselChainSet[i].size())
		{
			hanselChainSet.erase(hanselChainSet.begin() + i);
			i--;
		}
	}
}


void moeka::simpleSortHanselChainOrder()
{
	//calculateAllPossibleExpansions();
	graph g(&hanselChainSet);
	auto newHanselChainSet = g.simpleSort();
	hanselChainSet = newHanselChainSet;
}


void moeka::shortestPathHanselChainOrder()
{
	//calculateAllPossibleExpansions();
	graph g(&hanselChainSet);
	hanselChainSet = g.dijkstra(0);
}


void moeka::mstHanselChainOrder()
{
	// create some sort of graph order
	//calculateAllPossibleExpansions();
	graph g(&hanselChainSet);
	auto result = g.kruskal();
	auto newHanselChainSet = result.first;
	auto minSpanTreeEdgeList = result.second;

	// override old order
	hanselChainSet = newHanselChainSet;

	// print the edgeLists to a file
	// not necessary, just for debugging
	std::fstream fileCG, fileMST;
	fileCG.open("completeGraph.csv", std::ios::out | std::ios::app);
	
	for (const auto& edge : g.completeGraphEdgeList)
	{
		fileCG << std::get<0>(edge) << ","
			<< std::get<1>(edge) << ","
			<< std::get<2>(edge) << std::endl;
	}

	fileCG.close();
	
	fileMST.open("MST.csv", std::ios::out | std::ios::app);

	// create an adjacency matrix to be used in Gephi: graph visualizatiton software
	std::vector<std::vector<double>> adjacencyMatrix(numChains, std::vector<double>(numChains, 0));

	for (const auto& edge : minSpanTreeEdgeList)
	{
		/*fileMST << std::get<0>(edge) << ","
			<< std::get<1>(edge) << ","
			<< std::get<2>(edge) << std::endl;*/
		adjacencyMatrix[std::get<0>(edge)][std::get<1>(edge)] = std::get<2>(edge);
	}

	// print header as the first row in MST file
	fileMST << ",";

	for (int i = 0; i < numChains; i++)
	{
		fileMST << i << ",";
	}

	fileMST << std::endl;

	for (int i = 0; i < numChains; i++)
	{
		fileMST << i << ",";

		for (int j = 0; j < numChains; j++)
		{
			fileMST << adjacencyMatrix[i][j] << ",";
		}

		fileMST << std::endl;
	}

	fileMST.close();
}


void moeka::askMajorityFlag()
{
	// determine whether to do majority vectors first
	std::cout << "\nUse majority flag(1/0)?\n";
	std::cout << "Enter: " << std::flush;
	std::cin >> useMajorityFlag;

	if (useMajorityFlag)
	{
		findMajorityVectors();

		usedMajorityFlag = true;
		int foundTrueMajority = 0;

		std::cout << "Roughly how many majority vectors will result in a positive value? (Enter 0 if that is unknown)." << std::endl;
		std::cin >> trueMajority;

		// assign order to majority vectors
		for (int i = 0; i < numChains; i++)
		{
			for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
			{
				if (hanselChainSet[i][j].majorityFlag)
				{
					hanselChainSet[i][j].plannedQueryOrder = questionOrder;
					questionOrder++;
				}
			}
		}

		// assign the planned order to the rest of the vectors
		for (int i = 0; i < numChains; i++)
		{
			if (top)
			{
				for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
				{
					if (!hanselChainSet[i][j].plannedQueryOrder)
					{
						hanselChainSet[i][j].plannedQueryOrder = questionOrder;
						questionOrder++;
					}
				}
			}
			else
			{
				for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
				{
					if (!hanselChainSet[i][j].plannedQueryOrder)
					{
						hanselChainSet[i][j].plannedQueryOrder = questionOrder;
						questionOrder++;
					}
				}
			}
		}

		// pick a majority oldVector (sequential or random if trueMajority > 0)
		questionOrder = 1;
		srand((unsigned int)time(NULL));

		for (int t = 0; t < majorityVectors.size() - 1; t += 3)
		{
			int vector_class = -1;
			int i = majorityVectors[t];
			int j = majorityVectors[t + 1];

			// if true triples is unknown (0), then do not ask randomly. 
			// otherwise, pick a randoom triple
			if (trueMajority > 0)
			{
				while (true)
				{
					int random = (rand() % ((majorityVectors.size() - 3) / 3 + 1)) * 3;

					// only if visited
					if (!majorityVectors[random + 2])
					{
						i = majorityVectors[random];
						j = majorityVectors[random + 1];
						majorityVectors[random + 2] = 1;
						break;
					}
				}
			}

			// updated order must go before actual question because its tracking the intention of the question, not whether it was asked.
			hanselChainSet[i][j].updatedQueryOrder = questionOrder;
			questionOrder++;

			// if oldVector has not been visited, then ask user class
			// class cannot be retrieve otherwise since majority vectors are asked first (cannot be expanded)
			if (!hanselChainSet[i][j].visited)
			{
				vector_class = askingOfQuestion(i, j);

				if (vector_class)
				{
					foundTrueMajority++;

					// save successful location if true
					if (chainJump && j + 1 < (int)hanselChainSet[i].size())
					{
						trueVectorInd.push_back(i);
						trueVectorInd.push_back(j); // save location of true oldVector
					}
				}
			}

			checkExpansions(vector_class, i, j);

			// determine if the number of triples found is sufficient
			if (trueMajority && (foundTrueMajority >= (trueMajority / 2) + (trueMajority % 2)))
			{
				break;
			}
		}

		// must be set to false (0) to prevent error where in possibleExpansions, a oldVector could be expanded when the current chain is before the given chain
		useMajorityFlag = 0;

		// go to successful chains
		if (chainJump)
		{
			for (int t = 0; t < (int)trueVectorInd.size() - 1; t += 2)
			{
				int vector_class = -1;
				int i = trueVectorInd[t];

				if (top)
				{
					for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
					{
						if (j == trueVectorInd[t + 1] || questionFunc(i, j, vector_class))
						{
							continue;
						}

						checkExpansions(vector_class, i, j);
					}
				}
				else
				{
					for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
					{
						if (j == trueVectorInd[t + 1] || questionFunc(i, j, vector_class))
						{
							continue;
						}

						checkExpansions(vector_class, i, j);
					}
				}
			}
		}

		std::cout << "\nEnd majority flagged questions." << std::endl;
	}
	else
	{
		// assign planned order of questions to vectors
		for (int i = 0; i < numChains; i++)
		{
			if (top)
			{
				for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
				{
					hanselChainSet[i][j].plannedQueryOrder = questionOrder;
					questionOrder++;
				}
			}
			else
			{
				for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
				{
					hanselChainSet[i][j].plannedQueryOrder = questionOrder;
					questionOrder++;
				}
			}
		}

		questionOrder = 1;
	}
}


void moeka::findMajorityVectors()
{
	// first used to determine if middlepoints can be used
	// then, if middlepoints should be used (user selected that option).
	bool middlePoint = false; 

	for (auto attr : attribute_names)
	{
		if (attr.kv > 2)
		{
			middlePoint = true;
			break;
		}
	}

	if (middlePoint)
	{
		std::cout << "Does the user want to use middle points to determine what constitutes a majority datapoint? (1/0) Please Enter: " << std::flush;
		int c;
		std::cin >> c;
		
		if (c)
		{
			middlePoint = true;
		}
		else
		{
			middlePoint = false;
		}
	}

	// calculate max hamming norm
	int max_hamming_norm = 0;

	for (auto attribute : attribute_names)
	{
		max_hamming_norm += attribute.kv - 1;
	}

	// the number of points that need a value greater than 0 for the datapoint to be qualified as a "majority datapoint"
	int majority = (max_hamming_norm / 2) + (max_hamming_norm % 2); 

	// find the "majority vectors"
	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			int hamming_norm = 0;
			int points = 0; // counts how many points in the datapoint have a value greater than 0

			for (int k = 0; k < dimension; k++)
			{
				if (hanselChainSet[i][j].dataPoint[k] > 0)
				{
					hamming_norm += hanselChainSet[i][j].dataPoint[k];
					points++;
				}
			}

			// if middle points are used, then the number of points need to be the same or more as the majority of the datapoint
			if (middlePoint)
			{
				// these two if statements need to be separated 
				if (hamming_norm == majority && points >= ((dimension / 2) + (dimension % 2)))
				{
					majorityVectors.push_back(i);
					majorityVectors.push_back(j);
					majorityVectors.push_back(0);
					hanselChainSet[i][j].majorityFlag = true;
				}
			}
			else if (hamming_norm == majority)
			{
				majorityVectors.push_back(i);
				majorityVectors.push_back(j);
				majorityVectors.push_back(0);
				hanselChainSet[i][j].majorityFlag = true;
			}
		}
	}
}


void moeka::binarySearch(int i, int l, int r)
{
	// change this so that there is a quation for the bounds as well as for the middle (like in regular binary search)
	if (l <= r)
	{
		int j = l + (r - l) / 2; // this is middle
		int vector_class = -1;

		if (!questionFunc(i, j, vector_class))
		{
			// should expand everything
			checkExpansions(vector_class, i, j);

			// dual expansions
			// there needs to be a space between the left and right side of the current vector at location j to be a dual expansion
			if (j - l > 1)
			{
				dualExpansion(i, l, j);
			}

			if (r - j > 1)
			{
				dualExpansion(i, j, r);
			}
		}

		if (vector_class == function_kv - 1)
		{
			// update r with middle, go to left side
			binarySearch(i, l, j - 1);
		}
		else if (vector_class == 0)
		{
			// update left with middle, go to right side
			binarySearch(i, j + 1, r);
		}
		else
		{
			// go to left and right if function_kv > 2
			binarySearch(i, l, j - 1);
			binarySearch(i, j + 1, r);
		}
	}
}


void moeka::staticOrderQuestionsFunc()
{
	std::cout << "Thread: hello from static order questions func. Number of chains: " << numChains << std::endl;

	for (int i = 0; i < numChains; i++)
	{
		std::cout << "Thread: starting questions for hansel chain " << i << std::endl;

		// start binary search
		int chainSize = (int)hanselChainSet[i].size();

		if (useBinarySearch)
		{
			int l = 0;
			int r = chainSize - 1;

			// increment l if its classified and confirmed (only need to check if confirmed since confirmed == classified but not vice versa)
			while (l < r - 2)
			{
				if (hanselChainSet[i][l].confirmed)
				{
					l++;
				}
				else
				{
					break;
				}
			}

			// increment r if its classified
			while (r > l + 2) // L + 2
			{
				if (hanselChainSet[i][r].confirmed)
				{
					r--;
				}
				else
				{
					break;
				}
			}

			std::cout << "Thread: starting binary search." << std::endl;

			binarySearch(i, l, r);
		}
		else
		{
			// non-binary method
			std::cout << "Thread: starting sequential search." << std::endl;

			if (top)
			{
				for (int j = chainSize - 1; j >= 0; j--)
				{
					int vector_class = -1;

					if (questionFunc(i, j, vector_class))
					{
						continue;
					}

					checkExpansions(vector_class, i, j);
				}
			}
			else
			{
				for (int j = 0; j < chainSize; j++)
				{
					int vector_class = -1;

					if (questionFunc(i, j, vector_class))
					{
						continue;
					}

					checkExpansions(vector_class, i, j);
				}
			}
		}
	}
}


bool moeka::chainJumpMajorityFlagHelper(int i)
{
	for (int t = 0; t < (int)trueVectorInd.size() - 1; t += 2)
	{
		if (i == trueVectorInd[t])
		{
			return true;
		}
	}

	return false;
}


void moeka::chainJumpOrderQuestionsFunc()
{
	std::vector<int> skippedVectors;

	if (top)
	{
		for (int i = 0; i < numChains; i++)
		{
			// skip if majority flag was used and if i is one of the Hansel Chains that was the location of a "true oldVector" 
			if (usedMajorityFlag && chainJumpMajorityFlagHelper(i))
			{
				continue;
			}

			for (int j = (int)hanselChainSet[i].size() - 1; j >= 0; j--)
			{
				int vector_class = -1;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);

				// if vector_class = 1 and the oldVector is the first in the chain, 
				// then go to the next chain in hopes that it is 1 and can expand the previous chain
				if (vector_class && j == (int)hanselChainSet[i].size() - 1)
				{
					skippedVectors.push_back(i);
					break;
				}
			}
		}

		for (int i : skippedVectors)
		{
			for (int j = (int)hanselChainSet[i].size() - 2; j >= 0; j--)
			{
				int vector_class = -1;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);
			}
		}
	}
	else
	{
		for (int i = 0; i < numChains; i++)
		{
			// skip if majority flag was used and if i is one of the Hansel Chains that was the location of a "true oldVector" 
			if (usedMajorityFlag && chainJumpMajorityFlagHelper(i))
			{
				continue;
			}

			for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
			{
				int vector_class = -1;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);

				// if vector_class = 0 and the oldVector is the first in the chain, 
				// then go to the next chain in hopes that it is 1 and can expand the previous chain
				if (!vector_class && !j)
				{
					skippedVectors.push_back(i);
					break;
				}
			}
		}

		for (int i : skippedVectors)
		{
			for (int j = 1; j < (int)hanselChainSet[i].size(); j++)
			{
				int vector_class = -1;

				if (questionFunc(i, j, vector_class))
				{
					continue;
				}

				checkExpansions(vector_class, i, j);
			}
		}
	}
}


void moeka::expandKnownLowUnits()
{
	// if low units are known, then expand here
	// boolean only implementation
	// TODO: user can enter known low units through console or through UI
	// TODO: k-value implementation
	for (auto& lowUnit : knownLowUnits)
	{
		for (int i = 0; i < numChains; i++)
		{
			for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
			{
				if (lowUnit == hanselChainSet[i][j].dataPoint)
				{
					int vector_class = 1;

					hanselChainSet[i][j]._class = vector_class;

					hanselChainSet[i][j].asked = true;
					hanselChainSet[i][j].confirmed = true;
					numConfirmedInChains[i]++;
					hanselChainSet[i][j].visited = true;

					if (hanselChainSet[i][j]._class == 0 || hanselChainSet[i][j]._class == function_kv - 1)
					{
						hanselChainSet[i][j].weak = false;
					}

					checkExpansions(vector_class, i, j);

					if (j > 0)
					{
						int k = j - 1;
						vector_class = 0;

						hanselChainSet[i][k]._class = vector_class;
						hanselChainSet[i][k].asked = true;
						hanselChainSet[i][k].confirmed = true;
						numConfirmedInChains[i]++;
						hanselChainSet[i][k].visited = true;

						if (hanselChainSet[i][k]._class == 0 || hanselChainSet[i][k]._class == function_kv - 1)
						{
							hanselChainSet[i][k].weak = false;
						}

						checkExpansions(vector_class, i, j - 1);
					}
				}
			}
		}
	}
}


void moeka::calculateAllPossibleExpansions()
{
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			for (int p = 0; p < dimension; p++)
			{
				// add k-value here
				for (int d = 0; d < attribute_names[p].kv; d++)
				{
					possibleExpansions(d, i, j, p, 0);
				}

				// used to be just this for boolean
				//possibleExpansions(1, i, j, p, 0);
				//possibleExpansions(0, i, j, p, 0);
			}
		}
	}
}


void moeka::possibleExpansions(int newValue, int i, int j, int p, int startChain)
{
	int oldValue = hanselChainSet[i][j].dataPoint[p];

	// possible expansions from successive chains for a given class
	if (newValue != oldValue)
	{
		dvector expanded;
		expanded.dataPoint = hanselChainSet[i][j].dataPoint;
		expanded.dataPoint[p] = newValue;

		// starting in the current chain, search for expanded oldVector
		for (int hc = startChain; hc < numChains; hc++)
		{
			for (int v = (int)hanselChainSet[hc].size() - 1; v >= 0; v--)
			{
				// expand the oldVector and mark it as visited
				// these are "used" expansions
				if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && !hanselChainSet[hc][v].visited && &hanselChainSet[hc][v] != &hanselChainSet[i][j]) // comparing memory locations in last clause
				{
					// if newValue is greater than the oldValue, it means that it is an expansion in the positive direction, so the class should be 1.
					if (newValue == oldValue + 1) // used to be just ">"
					{
						hanselChainSet[i][j].up_expandable.push_back(&hanselChainSet[hc][v]);
					}

					// if newValue is less than the oldValue, it measn that the expansion is in the negative direction, so the class should be 0.
					else if (newValue == oldValue - 1) // used to be just "<", or nothing actually (simply an else statement)
					{
						hanselChainSet[i][j].down_expandable.push_back(&hanselChainSet[hc][v]);
					}

					return;
				}

				// this is obsolete now
				/*else if (expanded.dataPoint == hanselChainSet[hc][v].dataPoint && &hanselChainSet[hc][v] != &hanselChainSet[i][j]) // if oldVector is visited, then add to "unused" expansions
				{
					// if newValue is greater than the oldValue, it means that it is an expansion in the positive direction, so the class should be 1.
					if (newValue > oldValue)
					{
						hanselChainSet[i][j].unexpandable_one.push_back(&hanselChainSet[hc][v]);
					}
					// if newVlua is less than the oldValue, it measn that the expansion is in the negative direction, so the class should be 0.
					else
					{
						hanselChainSet[i][j].unexpandable_zero.push_back(&hanselChainSet[hc][v]);
					}

					return;
				}*/
			}
		}
	}
}


void moeka::checkExpansions(int vector_class, int i, int j)
{
	// use dynamic programming solution to not keep checking stuff
	std::map<int, std::vector<int>>* visited_map = new std::map<int, std::vector<int>>;

	if (function_kv == 2)
	{
		if (vector_class)
		{
			checkUp(i, j, vector_class, visited_map);
		}
		else
		{
			checkDown(i, j, vector_class, visited_map);
		}
	}
	else if (function_kv > 2)
	{
		if (vector_class == function_kv - 1)
		{
			checkUp(i, j, vector_class, visited_map);
		}
		else if (vector_class == 0)
		{
			checkDown(i, j, vector_class, visited_map);
		}
		else
		{
			checkUp(i, j, vector_class, visited_map);
			checkDown(i, j, vector_class, visited_map);
		}
	}

	// now go through and find any dual expansions
	for (int k = 0; k < numChains; k++)
	{
		int s = (int)hanselChainSet[k].size();

		if (numConfirmedInChains[k] <= s - 2 && s > 2)
		{
			findDualExpansion(k);
		}
	}

	delete visited_map;
}


void moeka::expandUp(int i, int j, dvector* vector, int vector_class)
{
	if (!vector->visited)
	{
		hanselChainSet[i][j].up_expansions.push_back(vector);
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
		vector->visited = true;
	}

	// if vector class is greater than the current class even though the previous was already visited
	// the vector class must be "greater than or equal" to in this case
	else if (vector_class > vector->_class && !vector->lessThan && !vector->asked)
	{
		// delete the pointer to this vector in the vector that expanded this one
		std::erase(vector->expanded_by->up_expansions, vector);

		// reassign expanded by
		// don't need to mark as visited
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
	}

	// the vector is a strong value if it is maximum function kv and was up expanded
	if (vector->_class == function_kv - 1)
	{
		vector->weak = false;
		vector->confirmed = true; // always confirmed when Boolean
		numConfirmedInChains[vector->number.first - 1]++;
	}
}


void moeka::checkUp(int i, int j, int vector_class, std::map<int, std::vector<int>>* visited_map)
{
	for (auto vector : hanselChainSet[i][j].up_expandable)
	{
		int e_i = vector->number.first - 1;
		int e_j = vector->number.second - 1;
		// base case
		// if vector is not in visited map, then recurse
		if (visited_map->find(e_i) == visited_map->end() ||
			!std::binary_search(visited_map->at(e_i).begin(), visited_map->at(e_i).end(), e_j))
		{
			expandUp(i, j, vector, vector_class);

			// insert expanded vector into map
			if (visited_map->find(e_i) == visited_map->end())
			{
				visited_map->insert(std::pair<int, std::vector<int>>(e_i, std::vector<int>{e_j}));
			}
			else
			{
				visited_map->at(e_i).push_back(e_j);
			}

			// recurse on expanded vector
			checkUp(e_i, e_j, vector->_class, visited_map);
		}
	}
}


void moeka::expandDown(int i, int j, dvector* vector, int vector_class)
{
	if (!vector->visited)
	{
		hanselChainSet[i][j].down_expansions.push_back(vector);
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
		vector->visited = true;
		vector->lessThan = true; // mark as "less than or equal to" for the vector class
	}

	// if vector class is less than the current class even though the previous was already visited
	// the vector class must be "less than or equal to", in this case.
	else if (vector_class < vector->_class && vector->lessThan && !vector->asked)
	{
		// delete the pointer to this vector in the vector that expanded this one
		std::erase(vector->expanded_by->down_expansions, vector);

		// reassign expanded by
		// don't need to mark as visited
		vector->expanded_by = &hanselChainSet[i][j];
		vector->_class = vector_class;
	}

	// the vector is a strong value if is 0 and was down expanded
	if (vector->_class == 0)
	{
		vector->weak = false;
		vector->confirmed = true; // always confirmed when Boolean
		numConfirmedInChains[vector->number.first - 1]++;
	}
}


void moeka::checkDown(int i, int j, int vector_class, std::map<int, std::vector<int>>* visited_map)
{
	for (auto vector : hanselChainSet[i][j].down_expandable)
	{
		// update i and j with location for expanded vector
		int e_i = vector->number.first - 1;
		int e_j = vector->number.second - 1;

		// base case
		// if vector is not in visited map, then recurse
		if (visited_map->find(e_i) == visited_map->end() ||
			!std::binary_search(visited_map->at(e_i).begin(), visited_map->at(e_i).end(), e_j))
		{
			expandDown(i, j, vector, vector_class);

			// insert expanded vector into map
			if (visited_map->find(e_i) == visited_map->end())
			{
				// make new vector in the map for hansel chain at e_i 
				visited_map->insert(std::pair<int, std::vector<int>>(e_i, std::vector<int>{e_j}));
			}
			else
			{
				// insert into existing hansel chain
				visited_map->at(e_i).push_back(e_j);
			}

			checkDown(e_i, e_j, vector->_class, visited_map);
		}
	}
}


void moeka::findDualExpansion(int i)
{
	int chainSize = (int)hanselChainSet[i].size();

	// start on left and right side. left increments and right decrements
	// if two vectors on each side have equivalent classes, then expand everything in the middle
	for (int l = 0, r = chainSize - 1; l < chainSize / 2 && r > chainSize / 2; l ++, r--)
	{
		// dual expansion if classes are not unassigned, are equal, but not the same vector
		if (hanselChainSet[i][l].confirmed && hanselChainSet[i][r].confirmed &&
			hanselChainSet[i][l]._class == hanselChainSet[i][r]._class)
		{
			dualExpansion(i, l, r);

			return;
		}
		else if (hanselChainSet[i][l + 1].confirmed && hanselChainSet[i][r].confirmed &&
			hanselChainSet[i][l + 1]._class == hanselChainSet[i][r]._class)
		{
			dualExpansion(i, l + 1, r);

			return;
		}
		else if (hanselChainSet[i][l].confirmed && hanselChainSet[i][r - 1].confirmed &&
			hanselChainSet[i][l]._class == hanselChainSet[i][r - 1]._class)
		{
			dualExpansion(i, l, r - 1);

			return;
		}
	}
}


void moeka::dualExpansion(int i, int l, int r)
{
	// j is for current to-be-expanded vector
	for (int j = l + 1; j < r; j++)
	{
		// if not confirmed, then 
		if (!hanselChainSet[i][j].confirmed)
		{
			hanselChainSet[i][j]._class = hanselChainSet[i][l]._class;
			hanselChainSet[i][j - 1].up_expansions.push_back(&hanselChainSet[i][j]);
			hanselChainSet[i][j].expanded_by = &hanselChainSet[i][j - 1];
			hanselChainSet[i][j].visited = true;
			hanselChainSet[i][j].confirmed = true; // always confirmed when Boolean
			numConfirmedInChains[i]++;

			// the vector is a strong value if it is maximum function kv and was up expanded
			if (hanselChainSet[i][j]._class == function_kv - 1)
			{
				hanselChainSet[i][j].weak = false;
			}
			// the vector is a strong value if is 0 and was down expanded
			else if (hanselChainSet[i][j]._class == 0)
			{
				hanselChainSet[i][j].weak = false;
			}

			std::cout << "dual expansion!" << std::endl;
		}
	}
}


void moeka::fixExpansions(int vector_class, int i, int j)
{
	if (vector_class)
	{
		for (auto vector : hanselChainSet[i][j].up_expandable)
		{
			if (vector->expanded_by && !vector->f_change && vector->_class != vector_class) // if it was expanded and has not been fixed
			{
				hanselChainSet[i][j].up_expansions.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->f_change = true;
				vector->down_expansions.clear(); // clear expanded zero because we are now going to expand vectors with a class of 1
				fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
			}
		}
	}
	else
	{
		for (auto vector : hanselChainSet[i][j].down_expandable)
		{
			if (vector->expanded_by && !vector->f_change && vector->_class != vector_class) // if it was expanded and has not been fixed
			{
				hanselChainSet[i][j].down_expansions.push_back(vector);
				vector->expanded_by = &hanselChainSet[i][j];
				vector->_class = vector_class;
				vector->f_change = true;
				vector->up_expansions.clear(); // clear expanded one because we are now going to expand vectors with a class of 0
				fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
			}
		}
	}
}


bool moeka::questionFunc(int i, int j, int& vector_class)
{
	std::cout << "Thread: within question func." << std::endl;

	// updated order must go before actual question because its tracking the intention of the question, not whether it was asked.
	if (!hanselChainSet[i][j].majorityFlag || !usedMajorityFlag || !hanselChainSet[i][j].visited || !hanselChainSet[i][j].updatedQueryOrder) // used, not useMajority flag because otherwise may expand twice.
	{
		hanselChainSet[i][j].updatedQueryOrder = questionOrder;
		questionOrder++;
	}

	// skip if vector is majority flag and usedMajorityFlag, was visited, and updatedQuery Order > 0 
	// (if updated query order is 0, then visited (expanded), but not asked, so retrieve class and do immediete expansions)
	// Otherwise, question may be asked.
	else
	{
		return true;
	}

	// if vector has not been visited, then ask user class
	// else, retrieve class
	//if (!hanselChainSet[i][j].visited || hanselChainSet[i][j].weak) // or class is a weak value
	if (!hanselChainSet[i][j].confirmed)
	{
		// used to be in the if statement above
		// || (hanselChainSet[i][j]._class > -1 && hanselChainSet[i][j]._class < function_kv - 1)
		vector_class = askingOfQuestion(i, j);
	}
	else
	{
		std::cout << "Thread: question was expanded. " << std::endl;
		vector_class = hanselChainSet[i][j]._class;
	}

	return false;
}


int moeka::askingOfQuestion(int i, int j)
{
	// syncrhonization here
	std::cout << "Thread: beginning asking of question..." << std::endl;

	if (synchronizationFlag)
	{
		std::cout << "Thread: checking synchronization flag..." << std::endl;

		while (true)
		{
			// if true, then continue
			if (*synchronizationFlag)
			{
				std::cout << "Thread: synchroniziation flag is true." << std::endl;
				break;
			}
			// else wait until flag is turned true
			else
			{
				std::cout << "Thread: checking synchronization flag... currently" << std::endl;
				Sleep(100);
			}
		}
	}

	int vector_class = -1;
	bool ask = true;

	// check if the vector contains any attribute that must be true for the vector to be true
	// if not, then that oldVector must have a class of 0
	for (auto attribute : attribute_names)
	{
		int t = attribute.trueIndex;

		if (-1 < t && t < dimension && hanselChainSet[i][j].dataPoint[t] < attribute.trueValue)
		{
			ask = false;
			vector_class = 0;
			break;
		}
	}

	// vector must be greater than the minimum acceptable datapoint in order to be considered.
	if (minimumAcceptableDatapoint.size() == dimension)
	{
		for (int k = 0; k < dimension; k++)
		{
			if (minimumAcceptableDatapoint[k] > hanselChainSet[i][j].dataPoint[k])
			{
				ask = false;
				vector_class = 0;
				break;
			}
		}
	}

	if (ask)
	{
		orderOfAskingSummary.push_back(i);
		orderOfAskingSummary.push_back(j);

		if (webOracle != "")
		{

		}


		// if there is no oracle, then ask expert
		if (hanselChainSet[i][j].oracle == -1)
		{
			std::cout << "\nEnter the class for this data point:\n";

			for (int k = 0; k < dimension; k++)
			{
				std::cout << attribute_names[k].name + "\t\t\t= " << hanselChainSet[i][j].dataPoint[k] << std::endl;
			}

			if (synchronizationFlag)
			{
				// TODO: send data to UI and edit below code to work with this in mind
				std::cout << "Thread: sending data to supervisor..." << std::endl;
				currentDatapoint = &hanselChainSet[i][j];

				*synchronizationFlag = false;

				while (true)
				{
					if (!*synchronizationFlag)
					{
						std::cout << "Thread: waiting for user to assign class..." << std::endl;
						Sleep(100);
					}
					else
					{
						std::cout << "Thread: user assigned class of " << *currentClass << std::endl;
						break;
					}
				}

				vector_class = *currentClass;
			}
			else
			{

				// if function kv is not binary, the current vector's class is not -1 (unassigned), and there is a possibility that the class could be greater (class is less than highest value)
				if (function_kv > 2) // this used to be here, but its also in questionFunc(): && hanselChainSet[i][j]._class > -1 && hanselChainSet[i][j]._class < function_kv - 1
				{
					// do something
					if (!hanselChainSet[i][j].lessThan)
					{
						std::cout << "\nThe current class of this datapoint can be between " << hanselChainSet[i][j]._class << " and " << function_kv - 1
							<< ".\nIt was already expanded, but the class can be a higher value according to the given ems k-value." << std::flush;
						std::cout << "Enter Class (" << hanselChainSet[i][j]._class << " - " << function_kv - 1 << "): " << std::flush;
					}
					else
					{
						std::cout << "\nThe current class of this datapoint can be between 0 and " << hanselChainSet[i][j]._class
							<< ".\nIt was already expanded, but the class can be a lower value according to the given ems k-value." << std::flush;
						std::cout << "Enter Class (" << 0 << " - " << hanselChainSet[i][j]._class << "): " << std::flush;
					}
				}
				else
				{
					std::cout << "Enter Class (0 - " << function_kv - 1 << "): " << std::flush;
				}

				std::cin >> vector_class;
				std::cin.clear();
				std::cin.ignore(1000, '\n');
			}
		}
		// query oracle
		else
		{
			vector_class = hanselChainSet[i][j].oracle;
		}

		questionsAsked++;
		hanselChainSet[i][j].asked = true;
		hanselChainSet[i][j].confirmed = true;
		numConfirmedInChains[i]++;
		hanselChainSet[i][j].finalQueryOrder = questionsAsked;
	}

	hanselChainSet[i][j]._class = vector_class;
	hanselChainSet[i][j].visited = true;

	if (hanselChainSet[i][j]._class == 0 || hanselChainSet[i][j]._class == function_kv - 1)
	{
		hanselChainSet[i][j].weak = false;
	}

	/*if (synchronizationFlag)
	{
		// set synchronization flag to false so that when it gets to the next question, it will not continue yet
		//*synchronizationFlag = false;

		return currentDatapoint->_class;
	}*/

	return vector_class;
}
