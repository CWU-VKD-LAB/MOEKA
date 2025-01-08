#include "moeka.h"

// functions for printing, parsing text via console, and overloaded operators are contained here.


template<typename T>
void print(T t, const int& width, const char& separator)
{
	std::cout << std::left << std::setw(width) << std::setfill(separator) << t;
}


std::ostream& operator<<(std::ostream& out, const std::vector<int>& v)
{
	for (int i = 0; i < (int)v.size() - 1; i++)
	{
		out << v[i] << ", ";
	}

	out << v[v.size() - 1];

	return out;
}


std::ostream& operator<<(std::ostream& out, const std::pair<int, int>& p)
{
	out << p.first << '.' << p.second;

	return out;
}


std::vector<int> moeka::parse_input_int(char delimiter, std::string temp)
{
	// parse a string by a delimiter
	std::vector<int> tokens;
	std::string token;
	int pos = 0;

	while ((pos = (int)temp.find(",")) != std::string::npos)
	{
		token = temp.substr(0, pos);
		tokens.push_back(stoi(token));
		temp.erase(0, pos + 1);
	}

	if (temp != "")
	{
		tokens.push_back(stoi(temp));
	}

	return tokens;
}


std::vector<std::string> moeka::parse_input_string(char delimiter, std::string temp)
{
	// parse a string by a delimiter
	std::vector<std::string> tokens;
	std::string token;
	int pos = 0;

	while ((pos = (int)temp.find(",")) != std::string::npos)
	{
		token = temp.substr(0, pos);
		tokens.push_back(token);
		temp.erase(0, pos + 1);
	}

	tokens.push_back(temp);

	return tokens;
}


void moeka::numberAssignment()
{
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			hanselChainSet[i][j].number.first = i + 1;
			hanselChainSet[i][j].number.second = j + 1;
		}
	}
}


void moeka::printTable(std::fstream& results, std::vector<int> targets, std::vector<std::string> boolFuncsSimplified, std::vector<std::string> boolFuncs, bool include_f_change)
{
	std::string askStr = "";
	std::string answerStr = "";

	for (int i = 0; i < (int)orderOfAskingSummary.size() - 1; i += 2)
	{
		answerStr += std::to_string(hanselChainSet[orderOfAskingSummary[i]][orderOfAskingSummary[i + 1]]._class) + ",";
		askStr += std::to_string(orderOfAskingSummary[i] + 1) + "." + std::to_string(orderOfAskingSummary[i + 1] + 1) + ",";
	}

	if (non_monotone)
	{
		for (int i = 0; i < (int)boolFuncsSimplified.size(); i++)
		{
			results << "Non-monotone Boolean Function Simplified: k = " << targets[i] << " = " << boolFuncsSimplified[i] + "\n";
			results << "Non-monotone Boolean Function Non-simplified: k = " << targets[i] << " = " << boolFuncs[i] + "\n";
		}
	}
	else
	{
		for (int i = 0; i < (int)boolFuncsSimplified.size(); i++)
		{
			results << "Monotone Boolean Function Simplified: k = " << targets[i] << " = " << boolFuncsSimplified[i] + "\n";
			results << "Monotone Boolean Function Non-simplified: k = " << targets[i] << " = " << boolFuncs[i] + "\n";
		}
	}

	results << "Order of Questions:," + askStr + "\n";
	results << "Answers:," + answerStr + "\n";
	results << "Total Questions: " + std::to_string(questionsAsked) + "\n\n";
	results << "Reference Number,Vector,";

	// these orders are irrelevent for violation of monotonicity
	if (!include_f_change)
	{
		results << "Planned Query Order,"
			<< "Updated Query Order,"
			<< "Final Query Order,"
			<< "Majority Flag,";
	}

	results << "Class,"
		<< "Oracle,"
		<< "Confirmed,";

	if (include_f_change)
	{
		results << "f-Change,";
	}

	results << "Expanded By,"
		<< "Up Expansions,"
		<< "Down Expansions,"
		<< "Up Expandable,"
		<< "Down Expandable\n";

	for (int i = 0; i < numChains; i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			std::string vecStr = "";
			std::string expandedOneStr = "";
			std::string expandedZeroStr = "";
			std::string expandableOneStr = "";
			std::string expandableZeroStr = "";

			// data point as string
			for (int k = 0; k < dimension - 1; k++)
			{
				vecStr += std::to_string(hanselChainSet[i][j].dataPoint[k]) + ";";
			}

			vecStr += std::to_string(hanselChainSet[i][j].dataPoint[dimension - 1]);

			// planned order of questions
			std::string plannedQueryOrder = std::to_string(hanselChainSet[i][j].plannedQueryOrder);

			// updated order of questions
			std::string updatedQueryOrder = std::to_string(hanselChainSet[i][j].updatedQueryOrder);

			// final query order
			std::string finalQueryOrder = "";

			if (hanselChainSet[i][j].finalQueryOrder)
			{
				finalQueryOrder += std::to_string(hanselChainSet[i][j].finalQueryOrder);
			}

			// expanded up expansions
			for (auto element : hanselChainSet[i][j].up_expansions)
			{
				expandedOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expanded down expansions
			for (auto element : hanselChainSet[i][j].down_expansions)
			{
				expandedZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expandable up expansions
			for (auto element : hanselChainSet[i][j].up_expandable)
			{
				expandableOneStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			// expandable down expansions
			for (auto element : hanselChainSet[i][j].down_expandable)
			{
				expandableZeroStr += std::to_string(element->number.first) + "." + std::to_string(element->number.second) + ";";
			}

			results << std::to_string(i + 1) + "." + std::to_string(j + 1) + ",";
			results << std::setfill('0') << std::setw(dimension) << vecStr << ",";

			if (!include_f_change)
			{
				results << plannedQueryOrder << ",";
				results << updatedQueryOrder << ",";
				results << finalQueryOrder << ",";
				results << hanselChainSet[i][j].majorityFlag << ",";
			}

			results << hanselChainSet[i][j]._class << ",";
			results << hanselChainSet[i][j].oracle << ",";
			results << hanselChainSet[i][j].confirmed << ",";

			if (include_f_change)
			{
				if (hanselChainSet[i][j].f_change)
				{
					results << "1,";
				}
				else
				{
					results << ",";
				}
			}

			if (hanselChainSet[i][j].expanded_by)
			{
				results << hanselChainSet[i][j].expanded_by->number.first << "." << hanselChainSet[i][j].expanded_by->number.second << ",";
			}
			else
			{
				results << ",";
			}

			results << expandedOneStr << ",";
			results << expandedZeroStr << ",";
			results << expandableOneStr << ",";
			results << expandableZeroStr << "\n";
		}
	}

	results << "\n";
}


void moeka::printToFile(std::fstream& results)
{
	// print vectors and monotone Boolean Function to a file

	if (parent_attribute != "")
	{
		results << "This results file represents the nested ems that is associated with attribute " << parent_attribute
			<< " of the ems that is one level higher.\n\n";
	}

	results << "Attributes, k-value, sub-attribute_names\n";

	for (int i = 0; i < dimension; i++)
	{
		results << attributeSymbol << i + 1 << "," << attribute_names[i].kv << ",";

		if (!childAttributes.empty())
		{
			if (!childAttributes[i].empty())
			{
				for (auto associateAttr : childAttributes[i])
				{
					results << associateAttr << ",";
				}
			}
			else
			{
				results << "None";
			}
		}

		results << "\n";
	}

	// Pilot Questions:
	results << "\nPilot Questions:\n";

	// order
	switch (orderOption)
	{
	case 1:
		results << "Longest Hansel Chain First\n";
		break;

	case 2:
		results << "Manual Hansel Chain Order\n";
		break;

	case 3:
		results << "Default Order\n";
		break;

	case 4:
		results << "Any Vector Order\n";
		break;

	default:
		results << "Shortest Hansel Chain First\n";
		break;
	}

	// true attributes
	results << "True Attributes:,";

	for (auto attribute : attribute_names)
	{
		if (attribute.trueIndex != -1)
		{
			results << attributeSymbol << attribute.trueIndex + 1 << "=" << attribute.trueValue << ", ";
		}
	}

	results << "\n";

	// majority flag
	if (usedMajorityFlag)
	{
		results << "Majority Flag used," << trueMajority << "\n";
	}
	else
	{
		results << "Majority Flag not used\n";
	}

	// chainJump
	if (chainJump)
	{
		results << "Chain jump ";

		if (top)
		{
			results << "top: start at the top and skip that chain if the oldVector class is 1 (revisit later)\n\n";
		}
		else
		{
			results << "bottom: start at the bottom and skip that chain if the oldVector class is 0 (revisit later)\n\n";
		}
	}
	else
	{
		results << "Static\n\n";
	}

	// print original results
	results << "Original Results\n";
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> boolFunc;
	std::vector<std::string> boolFuncs;
	std::vector<std::string> boolFuncsSimplified;
	std::vector<int> restoreValues;
	std::string tmp;
	std::vector<int> tokens;
	std::vector<int> targets;

	//std::cout << "Restore function for all values of kn+1? Please Enter (1/0): " << std::flush;
	int r = 1;
	//std::cin >> r;

	if (r)
	{
		for (int i = 1; i < function_kv; i++)
		{
			targets.push_back(i);
			boolFunc = restoreFunction(i);
			auto str = functionToString(boolFunc, ">=");
			boolFuncs.push_back(str.second);
			boolFuncsSimplified.push_back(str.first);

			// push back again, for target = 0, which is just target = 1 but with "<" signs instead of ">="
			if (i == 1)
			{
				targets.insert(targets.begin(), i - 1);
				auto str = functionToString(boolFunc, "<");
				boolFuncs.insert(boolFuncs.begin(), str.second);
				boolFuncsSimplified.insert(boolFuncsSimplified.begin(), str.first);
			}
		}

		// compare against real dataset and print out function accuracy results
		if (useRealOrdinalNormalizedDatasetToTestFunction && std::filesystem::exists(realOrdinalNormalizedDataset))
		{
			auto& testFunction = boolFunc.first;

			std::vector<std::vector<int>> dataset;
			std::vector<int> labels;

			std::ifstream file;

			file.open(realOrdinalNormalizedDataset);
			//file.open("diabetes_prediction_dataset.csv");

			if (file.is_open())
			{
				std::string line;

				std::getline(file, line);
				//attribute_names = parse_input_string(',', line);
				//dimension = (int)attribute_names.size() - 1;

				while (file.good())
				{
					try {
						std::getline(file, line);
						auto in = parse_input_int(',', line);

						if (in.size() < dimension + 1)
						{
							break;
						}

						std::vector<int> d(in.begin(), in.end() - 1);
						int l = in[int(in.size()) - 1];

						dataset.push_back(d);
						labels.push_back(l);
					}
					catch (std::exception e)
					{
						std::cout << e.what();
					}
				}
			}

			file.close();

			double truePositives = 0;
			double falsePositives = 0;
			double trueNegatives = 0;
			double falseNegatives = 0;
			double counter = 0;
			std::vector<std::vector<int>> trimmedFunc;

			for (int i = 0; i < (int)dataset.size(); i++)
			{
				for (auto& clause : testFunction)
				{
					// true positive
					if (clause <= dataset[i] && labels[i])
					{
						counter++;
						truePositives++;
						break;
					}
					// true negative
					else if (clause > dataset[i] && !labels[i])
					{
						counter++;
						trueNegatives++;
						break;
					}
					// false positive
					else if (clause <= dataset[i] && !labels[i])
					{
						falsePositives++;
						break;
					}
					// false negative
					else if (clause > dataset[i] && labels[i])
					{
						falseNegatives++;
						break;
					}
				}
			}

			std::cout << "Accuracy: " << (counter / (double)dataset.size()) * 100 << std::endl;
			std::cout << "Precision: " << (truePositives / (truePositives + falsePositives)) * 100 << std::endl;
			std::cout << "Recall: " << (truePositives / (truePositives + falseNegatives)) * 100 << std::endl;
			std::cout << "f1-score: " << (truePositives / (truePositives + 0.5 * (falsePositives + falseNegatives))) * 100 << std::endl;

			results << "Accuracy on real ordinal normalized dataset: " << (counter / (double)dataset.size()) * 100 << std::endl;
			results << "True Positives" << truePositives << std::endl;
			results << "True Negatives" << trueNegatives << std::endl;
			results << "False Positives" << falsePositives << std::endl;
			results << "False Negatives" << falseNegatives << std::endl;
			results << "Precision on real ordinal normalized dataset: " << (truePositives / (truePositives + falsePositives)) << std::endl;
			results << "Recall on real ordinal normalized dataset: " << (truePositives / (truePositives + falseNegatives)) << std::endl;
			results << "f1-score on real ordinal normalized dataset: " << (truePositives / (truePositives + 0.5 * (falsePositives + falseNegatives))) << std::endl;


			// this doesnt work because a clause can be true or false depending on the datapoint
			/*
			// trim function and print results here
			truePositives = 0;
			falsePositives = 0;
			trueNegatives = 0;
			falseNegatives = 0;
			counter = 0;

			for (int i = 0; i < (int)dataset.size(); i++)
			{

				for (auto& clause : trimmedFunc)
				{
					// true positive
					if (clause <= dataset[i] && labels[i])
					{
						counter++;
						truePositives++;
						break;
					}
					// true negative
					else if (clause > dataset[i] && !labels[i])
					{
						counter++;
						trueNegatives++;
						break;
					}
					// false positive
					if (clause <= dataset[i] && !labels[i])
					{
						falsePositives++;
						break;
					}
					// false negative
					else if (clause > dataset[i] && labels[i])
					{
						falseNegatives++;
						break;
					}
				}
			}

			std::cout << "Accuracy: " << (counter / (double)dataset.size()) * 100 << std::endl;
			std::cout << "Precision: " << (truePositives / (truePositives + falsePositives)) * 100 << std::endl;
			std::cout << "Recall: " << (truePositives / (truePositives + falseNegatives)) * 100 << std::endl;
			std::cout << "f1-score: " << (truePositives / (truePositives + 0.5 * (falsePositives + falseNegatives))) * 100 << std::endl;

			results << "TRIMMED FUNC accuracy on real ordinal normalized dataset: " << (counter / (double)dataset.size()) * 100 << std::endl;
			results << "TRIMMED FUNC True Positives" << truePositives << std::endl;
			results << "TRIMMED FUNC True Negatives" << trueNegatives << std::endl;
			results << "TRIMMED FUNC False Positives" << falsePositives << std::endl;
			results << "TRIMMED FUNC False Negatives" << falseNegatives << std::endl;
			results << "TRIMMED FUNC Precision on real ordinal normalized dataset: " << (truePositives / (truePositives + falsePositives)) << std::endl;
			results << "TRIMMED FUNC Recall on real ordinal normalized dataset: " << (truePositives / (truePositives + falseNegatives)) << std::endl;
			results << "TRIMMED FUNC f1-score on real ordinal normalized dataset: " << (truePositives / (truePositives + 0.5 * (falsePositives + falseNegatives))) << std::endl;
			*/
		}
	}
	else
	{
		std::cout << "For what values of k " << 0 << function_kv - 1 << " does the user want to restore? Please enter a comma-delimited list: " << std::flush;
		std::getline(std::cin, tmp);
		tokens = parse_input_int(',', tmp);

		for (auto token : tokens)
		{
			int target = token;
			targets.push_back(target);
			boolFunc = restoreFunction(target);
			auto str = functionToString(boolFunc, ">=");
			boolFuncs.push_back(str.second);
			boolFuncsSimplified.push_back(str.first);

			// push back again, for target = 0, which is just target = 1 but with "<" signs instead of ">="
			if (target == 1)
			{
				targets.insert(targets.begin(), target - 1);
				auto str = functionToString(boolFunc, "<");
				boolFuncs.insert(boolFuncs.begin(), str.second);
				boolFuncsSimplified.insert(boolFuncsSimplified.begin(), str.first);
			}
		}
	}

	printTable(results, targets, boolFuncsSimplified, boolFuncs, false);

	// reset variables
	questionsAsked = 0;
	orderOfAskingSummary.clear();
	boolFunc.first.clear();
	boolFunc.second.clear();
	boolFuncs.clear();
	boolFuncsSimplified.clear();

	// commented out because its not being used right now
	/*
	// print rectified results
	results << "\nResults After Applying Any f-Changes\n";
	f_change_check();

	for (auto target : targets)
	{
		std::pair<std::string, std::string> str;

		if (target == 0)
		{
			boolFunc = restoreFunction(target + 1);
			str = functionToString(boolFunc, "<");
		}
		else
		{
			boolFunc = restoreFunction(target);
			str = functionToString(boolFunc, ">=");
		}

		boolFuncs.push_back(str.second);
		boolFuncsSimplified.push_back(str.first);
	}


	if (realData)
	{
		std::cout << "Does the user want to change the attributes of the real data or to apply the Boolean function to the real data? (1/0): " << std::flush;
		int c;
		std::cin >> c;
		std::cin.clear();
		std::cin.ignore(1000, '\n');

		// if new attributes have been added, real data must be expanded
		auto dataset = addNewAttributesToRealData();

		if (c)
		{
			changeAttributesOfRealData(dataset);
		}
		// apply monotoone Boolean Function on real data
		// use boolFunc.first for simplified function
		else
		{
			applyBoolFuncToRealData(boolFunc.first, dataset);
		}
	}

	printTable(results, targets, boolFuncsSimplified, boolFuncs, true);

	// close file
	results << "\n\n";
	results.close();
	*/
}
