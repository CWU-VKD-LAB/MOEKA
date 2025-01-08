#include "moeka.h"


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


void moeka::start()
{
	std::cout << "Thread: hellow from start()" << std::endl;

	if (parent_attribute != "")
	{
		std::cout << "This dataset represents the nested attribute_names for the attribute " << parent_attribute << " of the ems that is one level higher." << std::endl;
	}

	// find real data
	if (parent_attribute != "" && std::filesystem::exists(filename))
	{
		// FIX: dimension is already assigned if file is given
		realData = true;
	}

	if (askOracleML)
	{
		// sort hansel chains and write hansel chains to file (this code should be called in each case of switch)
		// send file to python program for ML classification
		// write to new file here
		// since answers are sorted, can calculate where the correct answer is based on the location fo question in hanselChainSet[i][j]
		// 
		askFromOracleMLFile();
	}

	// order vectors and ask questions
	if (chainJump)
	{
		switch (orderOption)
		{
			// longest chain first order
		case 1:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() > b.size();
				});;
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			chainJumpOrderQuestionsFunc();
			break;

			// manual Hansel Chain order
			// let user order chains however they want
		case 2:
			//manualHanselChainOrder();
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			chainJumpOrderQuestionsFunc();
			break;

			// default order
		case 3:
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			chainJumpOrderQuestionsFunc();
			break;

			// any oldVector order
		/*case 4:
			anyVectorOrder();
			numberAssignment();
			chainJumpOrderQuestionsFunc();
			break;*/

			// shortest chain first order
		default:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() < b.size();
				});
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			chainJumpOrderQuestionsFunc();
			break;
		}
	}
	else
	{
		switch (orderOption)
		{
			// longest chain first order
		case 1:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() > b.size();
				});;
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			staticOrderQuestionsFunc();
			break;

			// manual Hansel Chain order
			// let user order chains however they want
		case 2:
			//manualHanselChainOrder();
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			staticOrderQuestionsFunc();
			break;

			// default order
		case 3:
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			staticOrderQuestionsFunc();
			break;

			// graph order
		case 4:
			mstHanselChainOrder();
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			staticOrderQuestionsFunc();
			break;

			// simple sort
		case 5:
			simpleSortHanselChainOrder();
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			staticOrderQuestionsFunc();
			break;

			// simple sort
		case 6:
			shortestPathHanselChainOrder();
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			staticOrderQuestionsFunc();
			break;

			// shortest chain first order
		default:
			std::sort(hanselChainSet.begin(), hanselChainSet.end(),
				[](const std::vector<dvector>& a, const std::vector<dvector>& b)
				{
					return a.size() < b.size();
				});
			calculateAllPossibleExpansions();
			numberAssignment();
			expandKnownLowUnits();
			staticOrderQuestionsFunc();
			break;
		}
	}

}


void moeka::start(bool* flag)
{
	std::cout << "Thread: hello from start." << std::endl;
	std::cout << "Thread: Synchronization flag is " << *flag << std::endl;

	synchronizationFlag = flag;

	// TODO: possible bug fixes with UI if real data is used and such
	start();

	std::cout << "Thread: moeka thread is ending." << std::endl;

	*synchronizationFlag = false;

	return;
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

		if (hanselChainSet[i][j].oracle == -1)
		{
			std::cout << "\nEnter the class for this data point:\n";

			for (int k = 0; k < dimension; k++)
			{
				std::cout << attribute_names[k].name + "\t\t\t= " << hanselChainSet[i][j].dataPoint[k] << std::endl;
			}

			if (synchronizationFlag)
			{
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


// assume a "<=" relationship when using k-value. for example k = 3. attribute is 1, but user said that the datapoint was true, so then if attrbute is 2, it is also true
std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> moeka::restoreFunction(int target)
{
	if (target >= function_kv)
	{
		std::cout << "impossible to restore ems." << std::endl;
	}

	// restore monotone Boolean function
	// iterate over every hansel chain, and check each chain for its "lower one" vector, if it has one
	// first large loop gets complete non simplified bool func
	// last large loop simnplifies this bool func
	// non simplified is stored in new variable
	std::vector<std::vector<int>> boolFunc;

	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		bool all_zero = true; // to make sure that a datapoint with the correect target class is not all zeroes
		int j;

		for (j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			// first vector of class 1 is "lower one" vector
			if (hanselChainSet[i][j]._class == target) // must be equal to highest function_kv to be "low unit"
			{
				bool first = false;

				// change for k-value

				// for every element 1 in the "lower" one vector, 
				// that element is in a clause in the monotone boolean funciton
				for (int k = 0; k < dimension; k++)
				{
					// if target is 0, then treat datapoints with 0 as a value as a low unit
					if (hanselChainSet[i][j].dataPoint[k] > 0 || !target) // used to be " == 1"
					{
						all_zero = false;

						if (!first)
						{
							std::vector<int> temp(dimension);
							temp[k] = hanselChainSet[i][j].dataPoint[k]; // used to be " = 1"
							boolFunc.push_back(temp);
							first = true;
						}
						else
						{
							boolFunc[boolFunc.size() - 1][k] = hanselChainSet[i][j].dataPoint[k];
						}
					}
				}

				// need to check if the latest clause has a non-monotonic vector
				// if yes, then add that vector to the end of the clause
				for (int k = 0; k < (int)nonMonotonicVectors.size(); k++)
				{
					auto vector = nonMonotonicVectors[k];

					if (i == vector->number.first - 1) // i is the index location of current hansel chain...
					{
						boolFunc[boolFunc.size() - 1].insert(boolFunc[boolFunc.size() - 1].end(), vector->dataPoint.begin(), vector->dataPoint.end());
						nonMonotonicVectors.erase(nonMonotonicVectors.begin() + k);
						k--;
					}
				}

				break; // break if we found low unit
			}
		}

		if (all_zero && hanselChainSet[i][--j]._class == target) // break if datapoint is all zeroes
		{
			break;
		}
	}

	const auto nonSimplifiedBoolFunc = boolFunc;

	// simplify monotone Boolean function
	// check if the absolute value of two difference clauses Hamming norms are equal to the difference of the vectors
	// difference, as in, how many attributes differ from each other
	// then, determine which statement is minimal (less attributes)
	for (int i = 0; i < (int)boolFunc.size() - 1; i++)
	{
		bool decrement_i = false;

		for (int j = i + 1; j < (int)boolFunc.size(); j++)
		{
			int difference = 0;
			int left_hamming_norm = 0;
			int right_hamming_norm = 0;

			for (int k = 0; k < dimension; k++)
			{
				if (boolFunc[i][k] != boolFunc[j][k])
				{
					difference += abs(boolFunc[i][k] - boolFunc[j][k]);
					// used to be for simply Boolean: difference++
				}

				left_hamming_norm += boolFunc[i][k];
				right_hamming_norm += boolFunc[j][k];
			}

			if (abs(left_hamming_norm - right_hamming_norm) == difference)
			{
				if (left_hamming_norm < right_hamming_norm)
				{
					boolFunc.erase(boolFunc.begin() + j);
					j--;
				}
				else if (left_hamming_norm > right_hamming_norm)
				{
					boolFunc.erase(boolFunc.begin() + i);
					decrement_i = true;
				}
			}
		}

		if (decrement_i)
		{
			i--;
		}
	}

	return std::make_pair(boolFunc, nonSimplifiedBoolFunc);
}


std::pair<std::string, std::string> moeka::functionToString(std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> boolFuncPair, std::string sign)
{
	// non-simplified
	auto boolFunc = boolFuncPair.second;

	// convert Boolean function to string
	std::string boolFuncStrNonSimplified = "";

	// convert not clause to string
	auto checkNotClause = [&boolFunc, &sign, this](std::string& temp, int& i)
	{
		if (boolFunc[i].size() > dimension)
		{
			temp += " & !";

			for (int j = dimension; j < boolFunc[i].size(); j++)
			{
				if (boolFunc[i][j])
				{
					if (attribute_names[j].kv > 2)
					{
						temp += "(";
						temp += attributeSymbol;
						temp += std::to_string(j + 1 - dimension); // just change to boolFunc[i][j] for k-value?
						temp += sign;
						temp += std::to_string(boolFunc[i][j]);
						temp += ")";
					}
					else
					{
						temp += attributeSymbol;
						temp += std::to_string(j + 1);
					}
				}
			}
		}
	};

	for (int i = 0; i < (int)boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j])
			{
				if (attribute_names[j].kv > 2)
				{
					temp += "(";
					temp += attributeSymbol;
					temp += std::to_string(j + 1);
					temp += sign;
					temp += std::to_string(boolFunc[i][j]);
					temp += ")";
				}
				else
				{
					temp += attributeSymbol;
					temp += std::to_string(j + 1);
				}
			}
		}

		checkNotClause(temp, i);

		if (!temp.empty() && i > 0)
		{
			boolFuncStrNonSimplified += " v " + temp;
		}
		else if (!temp.empty())
		{
			boolFuncStrNonSimplified += temp;
		}
	}

	// simplified
	boolFunc = boolFuncPair.first;

	// convert Boolean function to string
	std::string boolFuncStrSimplified = "";

	for (int i = 0; i < (int)boolFunc.size(); i++)
	{
		std::string temp = "";

		for (int j = 0; j < dimension; j++)
		{
			if (boolFunc[i][j])
			{
				if (attribute_names[j].kv > 2)
				{
					temp += "(";
					temp += attributeSymbol;
					temp += std::to_string(j + 1);
					temp += sign;
					temp += std::to_string(boolFunc[i][j]);
					temp += ")";
				}
				else
				{
					// if sign is "less than" and attribute is boolean, then use a "NOT" symbol in the function
					if (sign == "<")
					{
						temp += "!";
					}

					temp += attributeSymbol;
					temp += std::to_string(j + 1);
				}
			}
		}

		checkNotClause(temp, i);

		if (!temp.empty() && i > 0)
		{
			boolFuncStrSimplified += " v " + temp;
		}
		else if (!temp.empty())
		{
			boolFuncStrSimplified += temp;
		}
	}

	return std::make_pair(boolFuncStrSimplified, boolFuncStrNonSimplified);
}
