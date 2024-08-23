/*#include "oeka.h"


class oekaDriver
{
public:

	/// @brief stores an expert data mining object and the location of its parent or children
	struct Node
	{
		std::string name;			// name of the function
		oeka edm;
		int parent;					// index location in tree. should be given -1 if there is no parent
		std::vector<int> children;	// index locations in tree. empty if no children
	};

	/// @brief a data structure which represents a hierarchy of expert data mining objects (Boolean functions)
	std::vector<Node> tree;

	/// @brief possible names for attributes. Passed to EDM constructors to name the attributes generically
	std::vector<char> attributeSymbols{'x', 'y', 'z'};

	/// @brief index location of the current symbol for an attribute (what hasnt been used)
	int currentAttrSymbol = 0;

	/// @brief file for the hierachy of functions
	std::fstream hierarchy;


	// for the following several methods, it probably would've been better to use a standard BFS traversal with a queue instead of the iterative + recursive approach


	/// @brief creates a tree-like structure of Nodes, using the tree field (a vector). Also asks pilot questions in order to make the tree-like hiearchy
	/// @param index the index at which to start asking pilot questions (doesn't have to be the latest or last node if multiple are added at the same time i.e. children)
	void createTree(int index);


	/// @brief runs the Expert data mining process on all the nodes in the tree, starting with the root (super parent), and executing breadthwise for its children
	void executeTree();


	/// @brief helper for executeTree()
	void executeTreeHelper(std::vector<int> childrenIndices);


	/// @brief prints the results of the Expert data mining process of all the nodes in the tree, starting with the root (super parent), and executing breadthwise for its children
	void printTree();


	/// @brief helper for printTree() for printing children and grandchildren
	void printTreeChildren(std::vector<int> childrenIndices);


	/// @brief printTree() helper for printing the function hierarchy to the hierarchy file (see fstream field)
	void printTreeHierarchy(int index);


	/// @brief constructor
	oekaDriver();


	/// @brief add a child node to tree
	void addChild(oeka& child, int parent_index);


	/// @brief add a parent node to tree
	void addParent(oeka& parent, Node& oldNode);


	/// @brief pop the node at the corresponding index off the tree.
	/// @param index 
	Node pop(int index);


	//bool checkName();
};*/
