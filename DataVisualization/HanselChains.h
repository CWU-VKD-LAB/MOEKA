#pragma once
#include <vector>
#include <unordered_map>

class HanselChains
{
public:
	bool classesAssigned = false;

	std::vector<std::vector<std::vector<int>>> hanselChainSet;

	std::vector<bool> hanselChainContainsLowUnit;

	//note: comments below are messy but useful to testing historically

	// random function
	//std::vector<int> attributes = { 5, 5, 2, 2 };

	// housing attributes
	//std::vector<int> attributes = { 6, 4, 2, 2, 2, 3 };// total MOF {3, 3, 4};// housing complete{ 6, 4, 2, 2, 2, 3, 4, 2, 4, 2, 3, 2, 4 };
	//std::vector<int> attributes = { 4, 2, 4, 2, 3, 2};
	//std::vector<int> attributes = { 3, 3, 4 };

	// diabetes
	//std::vector<int> attributes = { 4, 2, 2, 3, 4, 4, 4 };
	//std::vector<int> attributes = { 5, 5, 2, 2 };
	//std::vector<int> attributes = { 2, 2, 2 };

	// gout
	//std::vector<int> attributes = { 5, 2, 5};
	// std::vector<int> attributes = { 5, 5, 2, 2 };

	// amount of attributes
	int dimension = 0;

	// the list of k values for each attribute
	std::vector<int> attributes;

	void calculateHanselChains(int vector_dimension);

private:
	std::vector<std::vector<std::vector<int>>> genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<std::vector<int>>>> chain);
};

