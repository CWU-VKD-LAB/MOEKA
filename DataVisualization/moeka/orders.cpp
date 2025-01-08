#include "moeka.h"

// any order or algorithm to change the order from the default hansel chain order is given here
// except for graph orders

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


void moeka::findTrueAttributes()
{
	// is there any attribute which must be true for answer to be true?
	std::cout << "\nIs there any attribute which must be true for the datapoint to be true?"
		<< "\nEnter the number assigned to each attribute or -1 if there is no such attribute."
		<< "\nIf there are multiple attribute_names, separate them with a comma.\n";

	for (int i = 0; i < dimension; i++)
	{
		std::cout << attribute_names[i].name + " - " + std::to_string(i) + "\n";
	}

	std::cout << "Enter: " << std::flush;
	std::string temp = "";

	try
	{
		std::cin >> temp;
	}
	catch (std::exception& e)
	{
		std::cerr << "user input fail. " << e.what() << std::endl;
	}

	size_t pos = 0;
	std::string token;

	auto f = [&temp, &pos, this](int i)
	{
		if (-1 < i && i < dimension)
		{
			attribute_names[i].trueIndex = i;

			if (attribute_names[i].kv > 2)
			{
				std::cout << "The k-value of this attribute is: " << attribute_names[i].kv << std::endl;
				std::cout << "For what value of k is this attribute " << attribute_names[i].name << "true ? Please enter : " << std::flush;
				int c;
				std::cin >> c;

				if (-1 < i && i < dimension)
				{
					attribute_names[i].trueValue = c;
				}
				else
				{
					attribute_names[i].trueValue = 1;
				}
			}
		}

		temp.erase(0, pos + 1);
	};

	while ((pos = temp.find(",")) != std::string::npos)
	{
		token = temp.substr(0, pos);
		f(std::stoi(token));
	}

	f(std::stoi(temp));
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


// experimental/unfinished, could be useful for experiments, but unlikely i think
// idea was to manually order chains with a high amount of insight from developer or SME 
// how this insight can be obtained -- no idea
/*void oeka::manualHanselChainOrder()
{
	std::cout << "\nThere are " << numChains << " Hansel Chains, labelled 1 through " << numChains << "." << std::endl;

	for (int i = 0; i < numChains; i++)
	{
		std::string suffix;

		if (i == 0) suffix = "st";
		else if (i == 1) suffix = "nd";
		else if (i == 2) suffix = "rd";
		else suffix = "th";

		std::cout << "\nWhat is the " << i + 1 << suffix << " Hansel Chain?";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> hanselChainOrder[i];
		hanselChainOrder[i]--;

		if (hanselChainOrder[i] == -1)
		{
			std::cout << "There is no chain zero. Select a different number chain." << std::endl;
			i--;
		}
		else if (!chainsVisited[hanselChainOrder[i]])
		{
			chainsVisited[hanselChainOrder[i]] = true;
		}
		else
		{
			std::cout << "You already selected this chain. Pick a different chain to go next." << std::endl;
			i--;
		}

		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}

	auto tempSet = hanselChainSet;

	for (int i = 0; i < numChains; i++)
	{
		hanselChainSet[i] = tempSet[hanselChainOrder[i]];
	}
}*/