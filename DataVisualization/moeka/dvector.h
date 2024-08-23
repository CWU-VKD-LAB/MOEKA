#pragma once
#include <vector>


/// @brief dvector, or datapoint vector, is simply a vector with information about a datapoint
struct dvector
{
	/// @brief the n-D data point to be considered as a vector
	std::vector<int> dataPoint;

	/// @brief the class or value of the dataPoint
	int _class = -1;

	/// @brief the class of the dataPoint, as determined by the Oracle (for use in testing of question reduction methods)
	int oracle = -1;

	/// @brief confirmed answer through either answering the question, Boolean expansion, or double-expansion (terminology for this is tentative)
	bool confirmed = false;

	/// @brief by default the class is great than or equal to. However, if this flag is true, then the class is less than or equal to.
	bool lessThan = false;

	/// @brief by default the class is weak (0 and asked/down-expanded or 1 and asked/up-expanded)
	bool weak = true;

	/// @brief the index of this vector's Hansel Chain and the index of the vector in the Hansel Chain, respectively
	std::pair<int, int> number;

	/// @brief the zero-to-zero expansions that occured
	std::vector<dvector*> down_expansions;

	/// @brief the one-to-one expansions that occured
	std::vector<dvector*> up_expansions;

	/// @brief the zero-to-zero expansions that are possible
	std::vector<dvector*> down_expandable;

	/// @brief the one-to-one expansions that are possible
	std::vector<dvector*> up_expandable;

	/// @brief true if the vector was "fixed" as a result of a violation of monotonicity, or simply an f-change
	bool f_change = false;

	/// @brief a pointer to a dvector if it expanded this dvector
	dvector* expanded_by = NULL;

	/// @brief if the vector is visited
	bool visited = false;

	/// @brief if this vector was asked to the expert/user
	bool asked = false;

	/// @brief signals whether the vector contains a majority of true attributes, meaning either 50% are true or the ceiling of that
	bool majorityFlag = false;

	/// @brief this number represents the order that this question was presented to the expert/user
	int finalQueryOrder = 0;

	/// @brief this number represents the order after being updated due to a chainJump ordering of questions
	int updatedQueryOrder = 0;

	/// @brief this number represents the order of questions that was planned to be presented to the expert/user
	int plannedQueryOrder = 0;
};
