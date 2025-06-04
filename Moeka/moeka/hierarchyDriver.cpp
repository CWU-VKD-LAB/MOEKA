/*#include "driver.h"


void oekaDriver::createTree(int index)
{
	if (index >= tree.size())
	{
		std::cout << "There are no more pilot questions." << std::endl;
		return;
	}

	std::cout << "\nExpert Data Mining.\n" << std::endl;

	if (tree[index].parent != -1)
	{
		std::cout << "The following questions are for the child function (aka sub-function or nested function) that belongs to the parent function named " 
			<< tree[tree[index].parent].name << std::endl;
		std::cout << "The associated parent attribute is " << tree[index].edm.parent_attribute << std::endl;
	}

	std::cout << "What would you like to name the Boolean function/dataset that the following questions will be about? Please Enter: " << std::flush;
	std::string temp;
	std::cin >> temp;

	tree[index].name = temp;

	std::cout << "Now asking pilot questions:\n\n" << std::endl;
	
	auto genericParentOrChildList = tree[index].edm.init();

	// no parents or children
	if (genericParentOrChildList.empty())
	{
		// do nothing
		// leaving this here for now in case this check turns out to be useful.
	}

	// children
	else if (genericParentOrChildList[0] == -1)
	{
		auto childList = genericParentOrChildList;

		// creat edm objects for each child;
		for (int i = 1; i < childList.size(); i++)
		{
			int k = tree[index].edm.attributes[childList[i] - 1].kv; // this k-value must be the function k-value of the child function. must subtract one for index location...

			std::string parentAttribute = tree[index].edm.attributeSymbol + std::to_string(childList[i]);

			std::cout << "What is the dimension of the child function (aka sub-function that belongs to the parent attribute " 
				<< parentAttribute << "? Please enter: " << std::flush;
			int c;
			std::cin >> c;

			try
			{
				oeka child = NULL;

				if (k > 2)
				{
					child = oeka(attributeSymbols[currentAttrSymbol++], parentAttribute, c, k); // constructor with parent attribute and given function k
				}
				else
				{
					child = oeka(attributeSymbols[currentAttrSymbol++], parentAttribute, c); // constructor with parent attribute
				}

				addChild(child, index);
			}
			catch (std::exception& e)
			{
				std::cerr << "user input fail." << e.what() << std::endl;
			}
		}
	}

	// parent
	else
	{
		// we insert the parent of the old attribute that is popped to the new parent because the old attribute was split into parents and children
		// if there is no parent of the old attribute, then nothing is changed
		Node oldNode = pop(index); 

		auto parentList = genericParentOrChildList;

		std::vector<std::vector<std::string>> childAttributes; // the associated child attributes for each attribute of the parent function

		for (int i = 0; i < (int)parentList.size(); i++)
		{
			// get vector of associated attributes for the group (super attribute/parent attribute)
			std::vector<std::string> group(parentList[i]);

			for (int j = 0; j < parentList[i]; j++)
			{
				group[j] = attributeSymbols[currentAttrSymbol + 1] + std::to_string(parentList[i + j + 1]);
			}

			i += parentList[i];
			childAttributes.push_back(group);
		}

		auto parent = oeka(attributeSymbols[currentAttrSymbol++], childAttributes); // dimension is size of childAttributes
		addParent(parent, oldNode);

		// DO THIS HERE AGAIN SO WE CAN GET K-VALUE AND THEN INITIALIZE CHILDREN WITH K-VALUE!!!!!
		auto genericParentOrChildList = tree[index].edm.init();

		for (int i = 0; i < (int)childAttributes.size(); i++)
		{
			// get k for each child attribute
			// if there is something in the row, then the child function is initialized with function value of k that corresponds to that row...
			int k = tree[index].edm.attributes[i].kv; // attributes at i because dimension of childAttributes and the parent match

			std::string parentAttribute = tree[index].edm.attributeSymbol + std::to_string(i + 1);
			auto child = oeka(attributeSymbols[currentAttrSymbol++], parentAttribute, (int)childAttributes[i].size(), k);
			addChild(child, index);
		}

		//currentAttrSymbol++;
		// commented out because we init parent above //index--; // bc popped 
		std::cout << "The following questions will be about the groups (parent attributes aka super attributes) that were just created. "
			<< "Since we just created groups from those attributes, the previous function will be overwritten by the new parent function and the new child functions." << std::endl;
	}

	createTree(++index);
}


void oekaDriver::executeTree()
{
	std::vector<int> childrenIndices;

	// find the parent or parents
	for (int i = 0; i < tree.size(); i++)
	{
		if (tree[i].parent == -1)
		{
			std::cout << "Now starting to ask questions on the boolean function named " << tree[i].name << std::endl;
			tree[i].edm.start(); // start the parent
			childrenIndices = tree[i].children;

			// start the children
			executeTreeHelper(childrenIndices);

			break; // break because there is only one overall parent
		}
	}
}


void oekaDriver::executeTreeHelper(std::vector<int> childrenIndices)
{
	if (childrenIndices.empty())
	{
		return;
	}

	std::vector<std::vector<int>> childrenList;

	for (int i = 0; i < (int)childrenIndices.size(); i++)
	{
		std::cout << "Now starting to ask questions on the boolean function named " << tree[childrenIndices[i]].name << std::endl;
		tree[childrenIndices[i]].edm.start();
		childrenList.push_back(tree[childrenIndices[i]].children);
	}

	for (auto grandChildrenIndices : childrenList)
	{
		executeTreeHelper(grandChildrenIndices);
	}
}


void oekaDriver::printTree() 
{
	// print to a file a legend for the function hiearchy
	hierarchy.open("function_hierarchy.txt", std::ios::out | std::ios::app);

	hierarchy << "This is file shows a legend for the functions described by the parent, child, and grandchildren files."
		<< "If there is no hierachy, then this file will mostly be blank\n\n";
	
	std::vector<int> childrenIndices;

	// find the parent
	for (int i = 0; i < tree.size(); i++)
	{
		if (tree[i].parent == -1)
		{
			printTreeHierarchy(i);

			std::fstream results;
			results.open("parent.csv", std::ios::out | std::ios::app);
			results << "This file is for the function named " << tree[i].name << "\n";

			std::cout << "\nNow beginning to ask about f-changes for the function named " << tree[i].name << std::endl;
			tree[i].edm.printToFile(results); // start the parent
			childrenIndices = tree[i].children;

			// start the children
			printTreeChildren(childrenIndices);

			break; // break because there is only one overall parent
		}
	}

	hierarchy.close();
}


void oekaDriver::printTreeChildren(std::vector<int> childrenIndices)
{
	if (childrenIndices.empty())
	{
		return;
	}

	std::vector<std::vector<int>> childrenList;

	
	for (int i = 0; i < (int)childrenIndices.size(); i++)
	{
		printTreeHierarchy(childrenIndices[i]);

		// determine name of results file
		std::string resultsFileName = "child_of_" + tree[childrenIndices[i]].edm.parent_attribute + ".csv";
		std::fstream results;
		results.open(resultsFileName, std::ios::out | std::ios::app);
		results << "This file is for the function named " << tree[childrenIndices[i]].name << "\n";

		std::cout << "\nNow beginning to ask about f-changes for the function named " << tree[childrenIndices[i]].name << std::endl;
		tree[childrenIndices[i]].edm.printToFile(results);
		childrenList.push_back(tree[childrenIndices[i]].children);
	}

	for (auto grandChildrenIndices : childrenList)
	{
		printTreeChildren(grandChildrenIndices);
	}
}


void oekaDriver::printTreeHierarchy(int i)
{
	if (tree[i].edm.childAttributes.empty())
	{
		return; // return if there are no associated attributes 
	}

	// print to hierarchy
	// iterate over each attribute
	for (int j = 0; j < tree[i].edm.dimension; j++)
	{
		hierarchy << "f(" << tree[i].edm.attributeSymbol << j + 1 << ") = ";

		// iterate over each associate attribute per the above attribute
		if (tree[i].edm.childAttributes[j].empty())
		{
			hierarchy << tree[i].edm.attributeSymbol << j + 1;
		}
		else
		{
			for (int k = 0; k < tree[i].edm.childAttributes[j].size(); k++)
			{
				hierarchy << tree[i].edm.childAttributes[j][k] << ",";
			}
		}

		hierarchy << "\n";
	}
}


oekaDriver::oekaDriver()
{
	attributeSymbols.reserve(26);

	for (int i = 'a'; i < 'x'; i++)
	{
		attributeSymbols.push_back(i);
	}

	oeka edm(attributeSymbols[currentAttrSymbol++]);

	Node node = { "", edm, -1, {}};
	tree.push_back(node);

	createTree(0);
	executeTree();
	printTree();
}


void oekaDriver::addChild(oeka& child, int parent_index)
{
	// create node and add to tree
	Node node = Node("", child, parent_index, {});
	tree.push_back(node);

	// link child to parent
	tree[parent_index].children.push_back((int)tree.size() - 1);
	int parent_attribute = stoi(tree.back().edm.parent_attribute.substr(1));
}


void oekaDriver::addParent(oeka& parent, Node& oldNode)
{
	parent.parent_attribute = oldNode.edm.parent_attribute; // oldNode parent is same as the new parent's parent
	Node node = Node("", parent, oldNode.parent, {});
	tree.push_back(node);

	// add new child to the parent of the new parent, if that parent of the oldNode existed
	// also need to replace associated attributes of the parent of the new parent, because the oldNode doesn't exist anymore
	if (node.parent != -1)
	{
		tree[tree.back().parent].children.push_back((int)tree.size() - 1);

		std::vector<std::string> group(parent.dimension);

		for (int i = 0; i < parent.dimension; i++)
		{
			group[i] = parent.attributeSymbol + std::to_string(i + 1);
		}

		int s = stoi(parent.parent_attribute.substr(1)) - 1;
		tree[tree.back().parent].edm.childAttributes[s] = group;
	}
}


oekaDriver::Node oekaDriver::pop(int index)
{
	Node node = tree[index];
	tree.erase(tree.begin() + index);

	if (node.parent != -1)
	{
		std::erase(tree[node.parent].children, index);
	}

	if (!node.children.empty())
	{
		for (int child : node.children)
		{
			tree[child].parent = -1;
		}
	}

	currentAttrSymbol--;

	return node;
}


int main()
{
	auto d = oekaDriver();

	return EXIT_SUCCESS;
}*/
