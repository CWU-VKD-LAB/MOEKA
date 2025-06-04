#include "moeka.h"


void moeka::f_change_check()
{
	std::cout << "\n\nChecking for f-changes:\n" << std::endl;

	const char separator = ' ';
	const int width = 15;

	//print("Number", width, separator);
	//print("Datapoint", width, separator);
	//print("Class", width, separator);
	std::cout << "Number\t\tDatapoint\t\tClass" << std::endl;

	for (int i = 0; i < hanselChainSet.size(); i++)
	{
		for (int j = 0; j < hanselChainSet[i].size(); j++)
		{
			//print(hanselChainSet[i][j].number, width, separator);
			//print(hanselChainSet[i][j].dataPoint, width, separator);
			//print(hanselChainSet[i][j]._class, width, separator);
			std::cout << hanselChainSet[i][j].number << "\t\t" << hanselChainSet[i][j].dataPoint
				<< "\t\t" << hanselChainSet[i][j]._class << std::endl;
		}
	}

	std::string temp;
	std::cout << "Please enter the number of any vectors which need any changes in a comma-separated list (e.g. 1.1, 3.2, ..., 7.4): " << std::flush;

	try
	{
		std::getline(std::cin, temp);
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}
	catch (std::exception e)
	{
		std::cerr << "f-change user input fail. " << e.what() << std::flush;

		return;
	}

	// parse user input
	auto tokens = parse_input_string(',', temp);

	// fix any violations of monotonicity
	for (auto token : tokens)
	{
		try
		{
			int pos = (int)token.find(".");
			std::string n = token.substr(0, pos);
			int i = stoi(n) - 1;
			token.erase(0, pos + 1);
			int j = stoi(token) - 1;

			checkViolationOfMonotonicityMethod(i, j);
		}
		catch (std::exception)
		{
			std::cout << "The vector specified does not exist, so it has been skipped." << std::endl;
		}
	}

	// add new attributes for those vectors which have violations 
	for (int i = 0; i < (int)addNewAttributesFor.size() - 2; i += 3)
	{
		// temporary: only latest attribute has the summary available
		orderOfAskingSummary.clear();
		questionsAsked = 0;

		std::cout << "Adding a new attribute." << std::endl;
		fixViolationOfMonotonicityAddAttr();

		std::cout << "Fixing the violation of monotonicity for that new attribute." << std::endl;
		fixViolationOfMonotonicityClass(addNewAttributesFor[i], addNewAttributesFor[i + 1], addNewAttributesFor[i + 2], true);
	}
}


void moeka::checkViolationOfMonotonicityMethod(int i, int j)
{
	int vector_class = hanselChainSet[i][j]._class;
	hanselChainSet[i][j].expanded_by = nullptr;

	if (vector_class)
	{
		vector_class = 0;
	}
	else
	{
		vector_class = 1;
	}

	hanselChainSet[i][j]._class = vector_class; // re-assign here. used to be in the lambda func

	// check if the change needs to be fixed in the first place
	// if the new vector class is supposed to be 1 and the vector above it is also one, then there is no violation
	// however, we still need to check those other vectors that are not monotonically related to the changed vector
	if (vector_class)
	{
		auto f1 = [&vector_class, &i, &j, this]()
		{
			// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
			// in this these, since the class went from 0 to 1, we re-ask those vectors which were expanded to a class of 0
			for (auto vector : hanselChainSet[i][j].down_expansions)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				// if the class changed from 0 to 1 (or higher...) for those vectors which were re-asked due to being expanded by the source of the f-change,
				// then, we need to fix the expansions for those vectors.
				if (vector_class != 0)
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}

			hanselChainSet[i][j].down_expansions.clear();
		};

		try
		{
			// if there is a vector above and the classes are equal, or if the current vector is the top
			if (hanselChainSet[i].size() - 1 > j && vector_class == hanselChainSet[i][j + 1]._class || hanselChainSet[i].size() - 1 == j)
			{
				f1();

				return;
			}
		}
		catch (std::exception&) // shouldn't be necessary
		{
			f1();

			return;
		};
	}

	// check if the change needs to be fixed in the first place
	// if the new vector class is supposed to be 0 and the vector below it is also 0, then there is no violation
	// however, we still need to check those other vectors that are not monotonically related to the changed vector
	else
	{
		auto f2 = [&vector_class, &i, &j, this]()
		{
			// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
			// in this case, since the class went from 1 to 0, were re-ask those vectors which were expanded to a class of 1
			for (auto vector : hanselChainSet[i][j].up_expansions)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				// if the class changed from 1 to 0 for those vectors which were re-asked due to being expanded by the source of the f-change,
				// then, we need to fix the expansions for those vectors.
				if (vector_class != 1)
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}

			hanselChainSet[i][j].up_expansions.clear();
		};

		try
		{
			// if there is a vector below and the classes are equal, or if the current vector is the lowest in the chain
			if (j > 0 && vector_class == hanselChainSet[i][j - 1]._class || j == 0)
			{
				f2();

				return;
			}
		}
		catch (std::exception&) // this shouldn't be necessary
		{
			f2();

			return;
		};
	}

	int c;
	std::cout << "Do you want to fix the violation by adding a new attribute to the expert dataset or by changing the class? Enter (1/0): " << std::flush;
	std::cin >> c;
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	// if the violation of monotoncity was an vector that was asked, directly rectify it 
	if (vector_class != hanselChainSet[i][j]._class)
	{
		if (c)
		{
			std::cout << "When adding a new attribute to fix the monotonicity, we assume that the current vector's class "
				<< "will the opposite of what it was originally given." << std::endl;
			addNewAttributesFor.push_back(i);
			addNewAttributesFor.push_back(j);
			addNewAttributesFor.push_back(vector_class);
		}
		else // find correct answer
		{
			std::cout << "Does the user want to preserve monotonicity? Enter (1/0): " << std::flush;
			std::cin >> c;
			std::cin.clear();
			std::cin.ignore(1000, '\n');

			if (c)
			{
				fixViolationOfMonotonicityClass(i, j, vector_class, true);
			}
			else
			{
				fixViolationOfMonotonicityClass(i, j, vector_class, false);
			}
		}
	}


	// NOTE: below code is commented out because it is no longer needed to find the parent before fixing a violation,
	// but it is possible that we may want to do something with that idea in the future.

	// if the violation of monotoncity was a oldVector that was expanded, find the parent and rectify it.
	// then, rectify the children
	/*else if (vector_class != hanselChainSet[i][j]._class && hanselChainSet[i][j].expanded_by)
	{
		if (c)
		{
			std::cout << "When adding a new attribute to fix the monotonicity, we assume that the current vector's class "
				<< "will the opposite of what it was originally given." << std::endl;

			auto parent = hanselChainSet[i][j].expanded_by;

			while (true)
			{
				if (parent->expanded_by)
				{
					parent = parent->expanded_by;
				}
				else
				{
					break;
				}
			}

			addNewAttributesFor.push_back(parent->number.first - 1);
			addNewAttributesFor.push_back(parent->number.second - 1);
			addNewAttributesFor.push_back(vector_class);
		}
		else // find correct answer
		{
			auto parent = hanselChainSet[i][j].expanded_by;

			while (true)
			{
				if (parent->expanded_by)
				{
					parent = parent->expanded_by;
				}
				else
				{
					break;
				}
			}

			std::cout << "Does the user want to preserve monotonicity? Enter (1/0): " << std::flush;
			std::cin >> c;

			if (c)
			{
				fixViolationOfMonotonicityClass(parent->number.first - 1, parent->number.second - 1, vector_class, true);
			}
			else
			{
				fixViolationOfMonotonicityClass(parent->number.first - 1, parent->number.second - 1, vector_class, false);
			}
		}
	}*/
}


void moeka::monotonicityReaffirmation(int i, int a, int j, int vector_class)
{
	std::cout << "Monotonicity reaffirmation process:\n"
		<< "What is the class of the vector : (" << hanselChainSet[i][a].dataPoint << ") ? Enter (1/0) : " << std::flush;
	int c;
	std::cin >> c;
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	if (c != vector_class)
	{
		std::cout << "The vector (" << hanselChainSet[i][a].dataPoint << ") is adjacent to the vector which was specified to have an f-change,\nbut the class"
			<< " is different, causing a violation. Since monotonicity was not preserved,\nwould the user like to add a new attribute to fix the monotonicity (1),"
			<< " or would the user prefer to use a non-monotonic ems (0)? Enter (1/0): " << std::flush;
		std::cin >> c;
		std::cin.clear();
		std::cin.ignore(1000, '\n');

		if (c)
		{
			std::cout << "\nWhen adding a new attribute to fix the monotonicity, we assume that the current vector's class "
				<< "will the opposite of what it was originally given so that it matches with its parent vector." << std::endl;
			addNewAttributesFor.push_back(i);
			addNewAttributesFor.push_back(a);
			addNewAttributesFor.push_back(vector_class);
		}
		// non-monotonic function
		else
		{
			nonMonotonicVectors.push_back(&hanselChainSet[i][j]);
			non_monotone = true;
		}
	}
}


void moeka::fixViolationOfMonotonicityClass(int i, int j, int vector_class, bool preserve)
{
	hanselChainSet[i][j]._class = vector_class; // reassign class here 
	hanselChainSet[i][j].f_change = true;

	// if vector class is true
	if (vector_class)
	{
		// need to delete any "expanded ones" of other vectors that possibly expanded this one because the class has been changed.
		// needs to be done before the actual process of fixing because these expanded_zeroes 
		// 
		// needs to check the expandable ones to check for those vectors which are greater than the source of the f-change (changed to one, so check vectors above which expanded to below)
		for (auto& vector : hanselChainSet[i][j].up_expandable)
		{
			bool flag = false;

			for (int k = 0; k < (int)vector->down_expansions.size(); k++)
			{
				if (vector->down_expansions[k] == &hanselChainSet[i][j])
				{
					vector->down_expansions.erase(vector->down_expansions.begin() + k);
					flag = true;
					break;
				}
			}

			// break again because there can only be one parent that expanded a particular vector
			if (flag)
			{
				break;
			}
		}

		hanselChainSet[i][j].up_expansions.clear();

		// monotonicity preservation
		if (preserve)
		{
			for (auto vector : hanselChainSet[i][j].up_expandable)
			{
				if (!vector->f_change && vector->_class != hanselChainSet[i][j]._class)
				{
					vector->expanded_by = &hanselChainSet[i][j];
					hanselChainSet[i][j].up_expansions.push_back(vector);
					fixViolationOfMonotonicityClass(vector->number.first - 1, vector->number.second - 1, vector_class, true);
				}
			}
		}
		// monotonicity reaffirmation
		else
		{
			if (hanselChainSet[i].size() > j + 1)
			{
				monotonicityReaffirmation(i, j + 1, j, vector_class);
			}
		}

		// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
		// in this case, since the class went from 0 to 1, were re-ask those vectors which were expanded to a class of 0
		for (auto vector : hanselChainSet[i][j].down_expansions)
		{
			if (!vector->f_change)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				if (vector_class != vector->_class)
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}
		}

		hanselChainSet[i][j].down_expansions.clear();
	}
	// vector class is false
	else
	{
		// need to delete any "expanded ones" of other vectors that possibly expanded this one because the class has been changed.
		// needs to be done before the actual process of fixing because these expanded_zeroes 
		// 
		// needs to check the expandable zeroes to check for those vectors which are smaller than the source of the f-change (changed to zero, so check below that expanded above)
		for (auto& vector : hanselChainSet[i][j].down_expandable)
		{
			bool flag = false;

			for (int k = 0; k < (int)vector->up_expansions.size(); k++)
			{
				if (vector->up_expansions[k] == &hanselChainSet[i][j])
				{
					vector->up_expansions.erase(vector->up_expansions.begin() + k);
					flag = true;
					break;
				}
			}

			// break again because there can only be one parent that expanded a particular vector
			if (flag)
			{
				break;
			}
		}

		hanselChainSet[i][j].down_expansions.clear();

		// monotonicity preservation
		if (preserve)
		{
			for (auto vector : hanselChainSet[i][j].down_expandable)
			{
				if (!vector->f_change && vector->_class != hanselChainSet[i][j]._class)
				{
					vector->expanded_by = &hanselChainSet[i][j];
					hanselChainSet[i][j].down_expansions.push_back(vector);
					fixViolationOfMonotonicityClass(vector->number.first - 1, vector->number.second - 1, vector_class, true);
				}
			}
		}
		// monotonicitiy reaffirmation
		else
		{
			if (j > 0)
			{
				monotonicityReaffirmation(i, j - 1, j, vector_class);
			}
		}

		// need to re-ask questions about any vectors which were expanded previously (not monotonically related anymore)
		// in this these, since the class went from 1 to 0, were re-ask those vectors which were expanded to a class of 1
		for (auto vector : hanselChainSet[i][j].up_expansions)
		{
			if (!vector->f_change)
			{
				vector_class = askingOfQuestion(vector->number.first - 1, vector->number.second - 1);

				if (vector_class != vector->_class)
				{
					fixExpansions(vector_class, vector->number.first - 1, vector->number.second - 1);
				}
			}
		}

		hanselChainSet[i][j].up_expansions.clear();
	}
}


void moeka::fixViolationOfMonotonicityAddAttr()
{
	dimension++;
	attribute_names.push_back(attribute{});
	attribute_names[attribute_names.size() - 1].name = attributeSymbol + std::to_string(dimension - 1); // will this work?

	// ask the k-value of the new attribute here
	if (askKV)
	{
		std::cout << "What is the k-value of the new attribute? Please enter: " << std::flush;

		try
		{
			std::cin >> attribute_names[attribute_names.size() - 1].kv;
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	else
	{
		attribute_names[attribute_names.size() - 1].kv = 2;
	}
	auto oldChains = hanselChainSet;
	hanselChainSet.clear();
	//std::cout << "What is the name of the new attribute?: " << std::flush;
	calculateHanselChains(dimension);
	numChains = (int)hanselChainSet.size();
	calculateAllPossibleExpansions();
	numberAssignment();

	// for each old oldVector in the old chains
	for (int i = 0; i < (int)oldChains.size(); i++)
	{
		for (int j = 0; j < (int)oldChains[i].size(); j++)
		{
			auto oldVector = oldChains[i][j];

			// for each new oldVector in the new chains
			for (int k = 0; k < (int)hanselChainSet.size(); k++)
			{
				bool flag = false;

				for (int l = 0; l < (int)hanselChainSet[k].size(); l++)
				{
					bool difference = false;

					// for each value of a oldVector in the new hansel chains
					for (int o = 0; o < dimension - 1; o++)
					{
						if (hanselChainSet[k][l].dataPoint[o] != oldVector.dataPoint[o])
						{
							difference = true;
						}
					}


					// does function k-value change anything below? anything else
					// 
					// 
					// if no difference (besides the new attribute)
					// it is assumed that the new attribute with a value equal to or greater than 1, in addition to an already true data point, will be true.
					// else, it can be assumed that a new attribute with a value of 0, in addition to an already false data point will be true
					if (!difference &&
						(hanselChainSet[k][l].dataPoint[dimension - 1] >= 1 && oldVector._class >= 1 ||
							hanselChainSet[k][l].dataPoint[dimension - 1] == 0 && oldVector._class == 0))
					{
						hanselChainSet[k][l].visited = true;
						hanselChainSet[k][l]._class = oldVector._class;

						// update index locations of add new attributes.
						// this works because if the old vectors meet the constraints of above, then we can update with the new location of the new vector
						for (int a = 0; a < (int)addNewAttributesFor.size() - 2; a += 3)
						{
							if (oldVector.number.first - 1 == addNewAttributesFor[a] && oldVector.number.second - 1 == addNewAttributesFor[a + 1])
							{
								addNewAttributesFor[a] = k;
								addNewAttributesFor[a + 1] = l;
							}
						}

						flag = true;
					}
				}

				if (flag)
				{
					break;
				}
			}
		}
	}

	// ask new questions on the new vectors
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < (int)hanselChainSet[i].size(); j++)
		{
			if (!hanselChainSet[i][j].visited)
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

	std::cout << "Done adding new attribute_names and asking questions about those attribute_names." << std::endl;
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


std::vector<std::vector<int>> moeka::addNewAttributesToRealData()
{
	std::vector<std::vector<int>> dataset;
	std::fstream file;
	file.open(filename);

	if (file)
	{
		std::string str;
		std::getline(file, str);
		auto tokens = parse_input_string(',', str);
		real_attributes = tokens;
		int attributesToAdd = dimension - ((int)tokens.size() - 1); // subtract 1 because last element is class

		// put dataset into data structure
		while (std::getline(file, str))
		{
			// parse input
			auto temp = parse_input_int(',', str);
			std::vector<int> tokens;
			tokens.reserve(temp.size());

			for (auto token : temp)
			{
				tokens.push_back(token);
			}

			dataset.push_back(tokens);
		}

		// add attributes to dataset
		for (int i = 0; i < attributesToAdd; i++)
		{
			const int size = (int)dataset.size();

			for (int j = 0; j < size; j++)
			{
				// copy datapoint
				std::vector<int> new_datapoint(dataset[j].size());
				std::copy(dataset[j].begin(), dataset[j].end(), new_datapoint.begin());

				// append 1 to old datapoint
				dataset[j].insert(dataset[j].end() - 1, 1);

				// append 0 to new datapoint and add to dataset
				new_datapoint.insert(dataset[j].end() - 1, 0);
				dataset.push_back(new_datapoint);
			}
		}

		file.close();
	}

	return dataset;
}


void moeka::applyBoolFuncToRealData(std::vector<std::vector<int>> boolFunc, std::vector<std::vector<int>> dataset)
{
	for (auto& datapoint : dataset)
	{
		// check if datapoint matches a clause in the Boolean function
		// clauses represent if a clause in the Boolean function is true or false for a given datapoint
		std::vector<bool> clauses(boolFunc.size(), true); // if clause i is true, then clauses[i] = true

		for (int i = 0; i < boolFunc.size(); i++)
		{
			for (int j = 0; j < dimension; j++)
			{
				if (boolFunc[i][j] && !datapoint[j])
				{
					clauses[i] = false;
					break;
				}
			}

			// even if datapoint succeeded in being true, we need to check if it is a nonmonotonic exception 
			if (clauses[i] && (int)boolFunc[i].size() > dimension)
			{
				bool isException = true; // this variable needs to be true if there is a nonmonotonic exception

				for (int j = dimension; j < (int)boolFunc[i].size(); j++)
				{
					if (boolFunc[i][j] != datapoint[j])
					{
						isException = false; // flipped to false if at any point the exception does not match the current datapoint, meaning there is no exception
						break;
					}
				}

				// if the datapoint is a nonmonotonic exception, then the datapoint is false whereas it would have been true otherwise
				if (isException)
				{
					clauses[i] = false;
				}
			}
		}

		// determine class
		bool wrote_1 = false;

		for (bool clause : clauses)
		{
			if (clause) // as long as one clause is true, then class is 1
			{
				datapoint[datapoint.size() - 1] = 1;
				wrote_1 = true;
				break;
			}
		}

		if (!wrote_1) // if no clause is true, then class is 0
		{
			datapoint[datapoint.size() - 1] = 0;
		}
	}

	// write new dataset to file
	std::ofstream file("new_" + filename);

	if (file)
	{
		for (auto at : real_attributes)
		{
			file << at << ",";
		}

		file << "\n";

		for (auto datapoint : dataset)
		{
			file << datapoint << "\n";
		}

		file.close();
	}
}


void moeka::changeAttributesOfRealData(std::vector<std::vector<int>> dataset)
{
	// ask user which attribute to change, or if just the class can be changed
	auto askAttributeToChange = [](std::vector<int>& datapoint)
	{
		int c;
		std::cout << "The real datapoint (" << datapoint << ") has a value which does not match the value of the expert datapoint.\n"
			<< "Do we want to change the real datapoint? Enter (1/0): " << std::flush;
		std::cin >> c;

		if (c)
		{
			std::cout << "\nDo we want to change the class of the real datapoint to match the expert datapoint (1),"
				<< " or do we want to change an attribute of the real datapoint (0)? Enter (1/0): " << std::flush;
			std::cin >> c;

			if (c)
			{
				return;
			}
			else
			{
				std::cout << "\nWhich attribute needs to be changed in this datapoint to match the expert data: ("
					<< datapoint << ")? (Enter a valid index location, or anything else to stop): " << std::flush;
				std::cin >> c;

				try
				{
					if (datapoint[c])
					{
						datapoint[c] = 0;
					}
					else
					{
						datapoint[c] = 1;
					}
				}
				catch (std::exception e)
				{
					std::cerr << "Improper index location: " << e.what() << std::flush;
				}
			}
		}
	};

	// change real data attributes or class
	for (int i = 0; i < (int)hanselChainSet.size(); i++)
	{
		for (int j = 0; j < hanselChainSet[i].size(); j++)
		{
			for (auto& datapoint : dataset)
			{
				auto temp = datapoint;
				int _class = datapoint.back();
				temp.pop_back(); // pop off class for comparison

				if (hanselChainSet[i][j].dataPoint == temp && hanselChainSet[i][j]._class != _class)
				{
					// ask user which attribute to swap
					datapoint.pop_back();
					askAttributeToChange(datapoint);
					datapoint.push_back(hanselChainSet[i][j]._class); // should be equal to dimension, but just in case
				}
			}
		}
	}

	std::ofstream file("new_" + filename);

	if (file)
	{
		for (auto at : real_attributes)
		{
			file << at << ",";
		}

		file << "\n";

		for (auto datapoint : dataset)
		{
			file << datapoint << "\n";
		}

		file.close();
	}
}

