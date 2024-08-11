#include "Function.h"

Function::Function (char* name) {
	functionName = name;
	hanselChains = nullptr;
}


void Function::initializeHanselChains(){
	hanselChains = new HanselChains();
	hanselChains->attributes = this->kValues;
	hanselChains->dimension = this->attributeCount;
	hanselChains->calculateHanselChains(hanselChains->dimension);
	hanselChains->hanselChainContainsLowUnit.reserve(hanselChains->hanselChainSet.size());
}


void Function::setUpHanselChains()
{
	// sorting like this might be obsolete depending on how Model works but its fine to leave for now
	std::sort(hanselChains->hanselChainSet.begin(), hanselChains->hanselChainSet.end(),
		[](const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b)
		{
			return a.size() > b.size();
		});

	if (dnfOnlyVisualization)
	{
		// TODO - will be a bit different than other program
		// apply function strictly w/o expansion of monotonicity
	}
	else
	{
		// apply function w/ expansion of monotonicity
		applyFunctiontoChains();
	}
}


void Function::applyFunctiontoChains()
{
	// for each chain
	for (auto& chain : hanselChains->hanselChainSet)
	{
		// for each datapoint of the chain
		for (auto& d : chain)
		{
			// get each MOF starting at kn+1 = 1 up to maximum kn+1
			for (int i = (int)subfunctionList.size() - 1; i >= 0; --i)
			{
				auto& mof = subfunctionList[i];

				// get each clause
				for (auto& clause : mof)
				{
					bool flag = true;

					// get each element of each clause
					for (int j = 0; j < hanselChains->dimension; j++)
					{
						if (d[j] < (*clause)[j])
						{
							flag = false;
							break;
						}
					}

					if (flag)
					{
						int _class = i + 1; // clauseList is assumed to be in order of smallest value of k to largest
						d.push_back(_class);
					}
					else if (i == 0)
					{
						// only assign class of 0 if it is the MOF for the smallest value of function_k(kn+1), which would be the absence of k = 1
						int _class = 0;
						d.push_back(_class);
					}
				}
			}
		}
	}

	hanselChains->classesAssigned = true;
}


Function::~Function () {
	delete(clause);
	for (auto a : subfunctionList) {
		//delete(a);
	}
	for (auto a : attributeNames) {
		delete(a);
	}
	delete(functionName);

	delete hanselChains;
}
