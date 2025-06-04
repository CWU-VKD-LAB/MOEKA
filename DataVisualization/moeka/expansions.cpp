/**
 * @file expansions.cpp
 * @brief Implementation of expansion-related algorithms for MOEKA
 * 
 * This file contains algorithms for handling expansions in the MOEKA system.
 * It includes functionality for:
 * - Calculating all possible expansions
 * - Handling upward and downward expansions
 * - Managing dual expansions between datapoints
 * - Checking and validating expansions
 * 
 * Note: This file specifically handles expansion algorithms that are not
 * related to function changes (f-changes) or order-based modifications.
 */

#include "moeka.h"

/**
 * @brief Calculates all possible expansions for each vector in the Hansel chain set
 * 
 * Iterates through all vectors in the Hansel chain set and calculates possible
 * expansions for each attribute value. This is the main entry point for expansion
 * calculations.
 */
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
			}
		}
	}
}

/**
 * @brief Calculates possible expansions for a specific vector and attribute
 * 
 * @param newValue The new value to check for expansion
 * @param i Index of the Hansel chain
 * @param j Index of the vector within the chain
 * @param p Index of the attribute being modified
 * @param startChain Starting chain index for searching expansions
 * 
 * Determines if a vector can be expanded by changing an attribute value,
 * and if so, adds it to either up_expandable or down_expandable lists.
 */
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
			}
		}
	}
}

/**
 * @brief Checks and processes expansions for a given vector
 * 
 * @param vector_class The class value of the vector
 * @param i Index of the Hansel chain
 * @param j Index of the vector within the chain
 * 
 * Uses dynamic programming to check expansions in both upward and downward
 * directions based on the vector's class value. Also handles dual expansions
 * for confirmed vectors.
 */
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

/**
 * @brief Expands a vector upward in the class hierarchy
 * 
 * @param i Index of the Hansel chain
 * @param j Index of the vector within the chain
 * @param vector Pointer to the vector being expanded
 * @param vector_class The class value to assign to the expanded vector
 * 
 * Handles upward expansion of vectors, marking them as visited and updating
 * their class values. Also handles cases where vectors need to be re-expanded
 * with a higher class value.
 */
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

/**
 * @brief Recursively checks upward expansions from a given vector
 * 
 * @param i Index of the Hansel chain
 * @param j Index of the vector within the chain
 * @param vector_class The class value to propagate
 * @param visited_map Map tracking visited vectors to prevent cycles
 * 
 * Recursively processes upward expansions while maintaining a visited map
 * to prevent infinite recursion and cycles.
 */
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

/**
 * @brief Expands a vector downward in the class hierarchy
 * 
 * @param i Index of the Hansel chain
 * @param j Index of the vector within the chain
 * @param vector Pointer to the vector being expanded
 * @param vector_class The class value to assign to the expanded vector
 * 
 * Handles downward expansion of vectors, marking them as visited and updating
 * their class values. Also handles cases where vectors need to be re-expanded
 * with a lower class value.
 */
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

/**
 * @brief Recursively checks downward expansions from a given vector
 * 
 * @param i Index of the Hansel chain
 * @param j Index of the vector within the chain
 * @param vector_class The class value to propagate
 * @param visited_map Map tracking visited vectors to prevent cycles
 * 
 * Recursively processes downward expansions while maintaining a visited map
 * to prevent infinite recursion and cycles.
 */
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

/**
 * @brief Finds potential dual expansions in a Hansel chain
 * 
 * @param i Index of the Hansel chain to check
 * 
 * Searches for pairs of confirmed vectors with the same class value that
 * can be used to expand vectors between them. This is used to fill in
 * unconfirmed vectors between two confirmed vectors of the same class.
 */
void moeka::findDualExpansion(int i)
{
	int chainSize = (int)hanselChainSet[i].size();

	// start on left and right side. left increments and right decrements
	// if two vectors on each side have equivalent classes, then expand everything in the middle
	for (int l = 0, r = chainSize - 1; l < chainSize / 2 && r > chainSize / 2; l++, r--)
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

/**
 * @brief Performs dual expansion between two vectors
 * 
 * @param i Index of the Hansel chain
 * @param l Left boundary vector index
 * @param r Right boundary vector index
 * 
 * Expands all unconfirmed vectors between two confirmed vectors of the same
 * class, assigning them the same class value and marking them as confirmed.
 */
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
