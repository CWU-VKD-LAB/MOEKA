#include "oeka.h"


// these functions are called in the initialization if the test dataset file is present as well as useOracle variable in oeka.h

std::map<int, std::vector<std::vector<int>>> moeka::readFile(std::string fileName)
{
	std::string line;
	std::map<int, std::vector<std::vector<int>>> oracle; // last element in each vector is the class

	// file format 
	// header -> k-values for each attribute, function kv
	// rows -> datapoint, class, hamming norm (so it doesn't need to be calculated)
	std::ifstream file;
	file.open(fileName);

	if (file.is_open())
	{
		// get dimension
		// get k-values
		// get function kv
		std::getline(file, line);
		auto header = parse_input_int(',', line);
		dimension = (int)header.size() - 1; // subtract 1 to account for the class
		attribute_names.resize(dimension);

		for (int i = 0; i < dimension; i++)
		{
			attribute_names[i].kv = header[i];
		}
		
		function_kv = header[dimension];
		
		while (file.good())
		{
			std::getline(file, line);
			auto in = parse_input_int(',', line);

			// checking if the format is correct
			if (in.size() > 2)
			{
				// if hamming norm, use retrieve last value of array
				int h = std::accumulate(in.begin(), in.end() - 1, 0); // in[in.size() - 1]; // get hamming norm
				std::vector<int> v;
				v.insert(v.begin(), in.begin(), in.end()); // copy vector, except for hamming norm // -0 when hamming norm not avalailable
				oracle[h].push_back(v);
			}
		}
	}

	file.close();

	return oracle;
}


void moeka::assignOracle(std::map<int, std::vector<std::vector<int>>> oracle)
{
	// index location of class in the oracle, which is simply the last element
	int c = (int)oracle[0][0].size() - 1;

	// use vector or counter or somethingg to keep track of questions which have already been answered?

	// iterate over hansel chains
	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			// get hamming norm of current data point to quickly find the 
			int h = 0;

			for (auto e : hanselChainSet[i][j].dataPoint)
			{
				h += e;
			}

			// compare current vector against the oracle (ordered by hamming norm)
			for (int k = 0; k < (int)oracle[h].size(); k++)
			{
				std::vector<int> v;
				v.insert(v.end(), oracle[h][k].begin(), oracle[h][k].end() - 1); // last element is class, so remove it for comparison

				if (hanselChainSet[i][j].dataPoint == v)
				{
					hanselChainSet[i][j].oracle = oracle[h][k][c];
				}
			}
		}
	}
}

// add some sort of functionality to store results and run all experiments automatically?