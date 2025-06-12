#include "moeka.h"


std::vector<std::vector<dvector>> moeka::genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<dvector>>> chains) {

	/*  A) shift datapoints & set new coordinate  */
	for (int j = 0; j < num; ++j) {
		for (auto& innerVec : chains[j]) {
			for (auto& dv : innerVec) {
				auto& p = dv.dataPoint;
				for (int i = 0; i + 1 < vector_dimension; ++i)
					p[i] = p[i + 1];
				p[vector_dimension - 1] = j;
			}
		}
	}

	/*  B) move “largest” elements down the chain hierarchy  */
	for (int j = 0; j + 1 < num; ++j) {
		auto& here = chains[j];
		for (size_t k = 0; k < here.size(); ++k) {
			for (int nxt = j; nxt + 1 < num; ++nxt) {
				auto& src = chains[nxt + 1];
				if (k >= src.size() || src[k].empty()) break;
				here[k].push_back(std::move(src[k].back()));
				src[k].pop_back();
			}
		}
	}

	/*  C) concat every sub-vector into chains[0] once  */
	auto& master = chains[0];
	size_t total = master.size();
	for (int j = 1; j < num; ++j) total += chains[j].size();
	master.reserve(total);

	for (int j = 1; j < num; ++j)
		for (auto& vec : chains[j])
			master.push_back(std::move(vec));

	/*  D) replicate if the caller really wants N independent copies  */
	for (int j = 1; j < num; ++j)
		chains[j] = master;  // unavoidable copy per key

	return master; // return with move instead of copy.
}

void moeka::calculateHanselChains(int vector_dimension) {
	
	hanselChainSet.clear();

	/*  build 1-D base chain – reserve once  */
	int num0 = attribute_names[0].kv;
	std::vector<dvector> base;
	base.reserve(num0);
	
	for (int k = 0; k < num0; ++k) {
		dvector dv;
		dv.dataPoint.assign(vector_dimension, 0);
		dv.dataPoint.back() = k;
		base.emplace_back(std::move(dv));
	}
	
	hanselChainSet.emplace_back(std::move(base));

	/*  extend one dimension at a time  */
	for (int dim = 1; dim < vector_dimension; ++dim) {
		
		int num = attribute_names[dim].kv;

		/*  build map<k, copy of hanselChainSet> in one pass  */
		std::unordered_map<int, std::vector<std::vector<dvector>>> chainMap;
		chainMap.reserve(num);
		
		for (int k = 0; k < num; ++k)
			chainMap.emplace(k, hanselChainSet);   // copy once per k (required)

		hanselChainSet = genChains(num, vector_dimension, std::move(chainMap));
	}

	/*  strip empties  */
	hanselChainSet.erase(std::remove_if(hanselChainSet.begin(), hanselChainSet.end(), [](const auto& v) { return v.empty(); }), hanselChainSet.end());
}
// CONTAINS ALL THE QUESTION ASKING FUNCTIONS AND THE CHAIN EXPANSION
void moeka::start()
{

	printf("STARTING INTERVIEW!!!\n");

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
				});
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

	// set the end flag so we can be done. Form.cpp is listening to this one here. 
	if(this->endFlag)
		*endFlag = true;

	// shut down the python server. close our socket.
	if (this->mlInterview)
	{
		// Now send the exit flag to shut down the server communication
		std::string exitMsg = "exit";
		send(ConnectSocket, exitMsg.c_str(), (int)exitMsg.size(), 0);
		std::cout << "Sent shutdown flag: " << exitMsg << "\n";

		// Cleanup
		closesocket(ConnectSocket);
		WSACleanup();
	}

}

// start used for when we are using UI interv
void moeka::start(std::mutex *t, bool *endFlag, int *c)
{

	// set our turn flag mutex now that we are ready
	this->turnFlag = t;
	this->endFlag = endFlag;
	this->currentClass = c;

	start();
	return;
}

void moeka::staticOrderQuestionsFunc()
{

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

			binarySearch(i, l, r);
		}
		else
		{
			// non-binary method

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

// function which asks a question if we determine that we need to ask a question for this point
bool moeka::questionFunc(int i, int j, int& vector_class)
{

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
		vector_class = hanselChainSet[i][j]._class;
	}

	return false;
}

int moeka::askingOfQuestion(int i, int j)
{

	// if we're using UI version, so we have a turn flag, we are going to wait to lock that mutex
	if (turnFlag)
		turnFlag->lock();

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

			// if the flag isn't null, meaning we're using UI version
			if (turnFlag)
			{
				// where we update the new datapoint.
				currentDatapoint = &hanselChainSet[i][j];

				// now we unlock, so that the supervisor has time to lock, and then lock again to set the vector class when that's done
				turnFlag->unlock();
				
				// in between these locks, the lock in form.cpp needs to hit. 
				Sleep(250);

				// use this so that we are blocking to wait until we get a result in currentClass
				turnFlag->lock();
				vector_class = *currentClass;
			}
			else if (mlInterview) {

				currentDatapoint = &hanselChainSet[i][j];
				std::string numbers = "";
				for (int att = 0; att < dimension; att++) {
					numbers.append(std::to_string(hanselChainSet[i][j].dataPoint[att]) + " ");
				}

				int iResult = send(ConnectSocket, numbers.c_str(), (int)numbers.size(), 0);
				if (iResult == SOCKET_ERROR) {
					std::cerr << "send failed: " << WSAGetLastError() << "\n";
					closesocket(ConnectSocket);
					WSACleanup();
					throw std::runtime_error("send failed");
				}


				// Receive the server's reply
				char recvbuf[512];
				iResult = recv(ConnectSocket, recvbuf, 511, 0);
				if (iResult > 0) {
					recvbuf[iResult] = '\0'; // null-terminate the string
				}
				else {
					throw new std::runtime_error("COMMUNICATION ERROR");
				}

				// parse the reply for our number and put it where it goes
				std::string receivedStr(recvbuf);
				int result = std::stoi(receivedStr);
				vector_class = result;
			}
			//console version
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

	if (turnFlag)
		turnFlag->unlock();

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
