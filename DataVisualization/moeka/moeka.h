/*
"Expert" Data Mining with Hansel Chains

Author: Harlow Huber
Supervisor: Dr. Boris Kovalerchuk
*/

#define PY_SSIZE_T_CLEAN
//#include <C:\Users\79har\AppData\Local\Programs\Python\Python310\include\Python.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <mutex>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shellapi.h>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <filesystem>
#include <array>
#include <numeric>
#include "dvector.h"
#include "graph.h"
#pragma comment(lib, "Ws2_32.lib") // causes some linking errors without this...


struct attribute
{
	std::string name = "";
	int kv = 2;
	int trueValue = -1; // "greater than or equal to"
	int trueIndex = -1; // index location for value
};


class moeka
{
public:
	/// flags for synchronization
	///
	/// @brief synchronization for background moeka and UI main thread
	bool* synchronizationFlag = nullptr;

	/// @brief current datapoint to pass to UI
	dvector* currentDatapoint = nullptr;

	/// @brief current class to pass to UI
	int* currentClass = nullptr;
	///
	/// flags for synchronization end

	//variables for when we are running an ML interview in the GUI version
	bool mlInterview = false; // if this is true, we run the interview like normal, but instead of giving the datapoint to the user, it goes to the python. 
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in serverAddr;
	WSADATA wsaData;

	/// @brief only ask from oracle ML without assigning all possibilities
	bool askOracleML = false;

	/// @brief path to ML model which has been BUILT already by running ML on dataset.
	std::string oracleML_path = "moeka\\ML_Oracles\\LinearDiscriminantAnalysis.sav";

	/// @brief path to ML model loading and predicting function
	std::string oracleML_loadingPath = "moeka\\ML_Oracles\\makePredictionsDriver.py";


	// TODO: if doing testing with real file, as take the attributes so that kvalues and such do not have to be entered
	/// flags for testing function on real world dataset which must be ordinal and normalized in some way so that testing can work.
	/// @brief 
	bool useRealOrdinalNormalizedDatasetToTestFunction = false;

	/// @brief path to real ordinal normalized dataset
	std::string realOrdinalNormalizedDataset = "..moeka\\ML_Oracles\\heart_failure_ordinal_normalization.csv";
	///
	/// flags for machine learning end

	/// @brief set of Hansel Chains
	std::vector<std::vector<dvector>> hanselChainSet;

	/// TODO: create functions for console input for Min acceptable datapoint and known low units. 
	/// These orders currently have to be compiled below
	///  
	/// @brief by default this is none but if there is one then initialize here or through startFromUI() function
	std::vector<int> minimumAcceptableDatapoint = {};

	/// @brief low units that the expert knows from observation, intuition, or data
	std::vector<std::vector<int>> knownLowUnits = {};

	/// @brief needs to be changed for whatever oracle is supposed to be used
	std::string generatedOraclePath = "";// "MonotoneDataGenerator/diabetesOracleKV.csv"; //kv3_7D_summation.csv //sum10D8T.csv

	/// @brief hiearchy of functions. 0 means no hiearchy should be used. -1 is uninitialized. 1 is use it...
	int hierarchy = 0;

	/// @brief flag for developer. Ask KV questions, yes or no? don't really need this anymore...
	bool askKV = true;

	/// @brief order of hansel chains
	int hanselChainOrder = -1;

	/// @brief signals whether to use the majority flag or not. True if majority vectors are IN USE. Switches to false when majority vectors have been asked.
	bool useMajorityFlag = false;

	// true if intra-chain binary search is used instead of sequential search at the bottom or top of the chains.
	bool useBinarySearch = false;

	/// @brief signals whether to use a chainJump ordering of questions
	bool chainJump = false;

	/// @brief for chainJump ordering, start at the top of the chain
	bool top = false;




	// constructors

	/// @brief constructor. Used for the top-most MBF/MOR in a hierarchy of functions.
	moeka(char attributeSymbol);


	/// @brief constructor. used for individually testing the various operation reduction methods.
	/// @param attributeSymbol 
	/// @param oraclePath 
	/// @param hanselChainOrder
	/// @param askKV 
	/// @param useMajorityFlag 
	/// @param useBinarySearch 
	/// @param chainJump 
	/// @param top 
	moeka(char attributeSymbol, std::string generatedOraclePath, int hanselChainOrder, bool useMajorityFlag, bool useBinarySearch, bool chainJump, bool top);


	/// @brief constructor. used for individually testing the various operation reduction methods. Uses the oraclePath to select k-values
	/// @param attributeSymbol 
	/// @param oraclePath 
	/// @param askKV 
	/// @param useMajorityFlag 
	/// @param useBinarySearch 
	/// @param chainJump 
	/// @param top 
	//oeka(char attributeSymbol, std::string oraclePath, int useMajorityFlag, bool useBinarySearch, int chainJump, int top);


	/// @brief constructor. Used to create sub-functions for a parent functions in a bottom-up approach
	/// @param parent_attribute is the number of the attribute that the expert data mining object is for
	/// @paraam dimension is the dimension
	moeka(char attributeSymbol, std::string parent_attribute, int dimension);


	/// @brief similar constructor as above, but with a function k-value
	/// @param attributeSymbol 
	/// @param parent_attribute 
	/// @param dimension 
	/// @param function_kv
	moeka(char attributeSymbol, std::string parent_attribute, int dimension, int function_kv);


	/// @brief initialize parent with associated attributes already given
	/// @param attributeSymbol 
	/// @param asociated_attributes 
	moeka(char attributeSymbol, std::vector<std::vector<std::string>> childAttributes);


	/// @brief send and retrieve file for ML
	void askFromOracleMLFile();


	/// @brief get the necessary user inputs to run the program with the hiearchy of functions driver class
	/// @return vector starts with -1 if it is a list of attributes with child functions. Otherwise, first number is group size, next numbers are attributes in that group, and repeat
	std::vector<int> init();


	/// @brief same as above function, except done via the new UI and not from the console
	/// @param attributes 
	/// @param kValues 
	/// @param mof 
	/// @param intraChainOrder 
	/// @param trueAttributes 
	/// @param chainJump 
	/// @param majority 
	/// @param top 
	/// @return 
	std::vector<int> initFromUI(int attributeCount, std::vector<std::string> attributes, std::vector<int> kValues, int functionKV, int staticInterChainOrder,
		std::vector<int> trueAttributes, bool chainJump, bool majority, bool topBottomOrBinarySearch);

	// initialize the Moeka guy for an ML version now. Asks questions in the same way, but doesn't prompt the user, instead just quizzes the computer man.
	std::vector<int> initForMLInterview(std::string mlPath, std::string datasetPath);

	/// @brief start expert data mining sequence
	void start();


	/// @brief same as above but uses flag for synchronization with UI
	/// @param flag 
	void start(std::mutex *, bool*, int *);

	/// @brief print results to file
	/// @param results the results file
	void printToFile(std::fstream& results);

	// moved here so we can set it in other files to initialize stuff. rather than being private. 
	/// @brief by default, this is 2 (a Boolean function), but sometimes it could be a k-value function in the case of nested functions
	int function_kv = -1;

	// also moved so we can set elsewhere. 
	/// @brief determine the the number of attributes (dimension) and what those attributes are
	int dimension = -1;

	/// @brief the name of the attributes of the datapoint
	std::vector<attribute> attribute_names;

private:
	// the number of confirmed values in each chain
	std::vector<int> numConfirmedInChains;


	/// @brief the parent function, or attribute, of the current Boolean function, which represents the attribute that the current sub-function is for. E.g. 1 is attribute x1
	std::string parent_attribute = "";

	/// @brief these are the sub-attributes of the current Boolean function. Index location 0 is for the subattributes of parent attribute x1 (of this Boolean function)
	std::vector<std::vector<std::string>> childAttributes;

	/// @brief the symbols that is used for this Boolean function
	char attributeSymbol = 'a';

	/// @brief name of this Boolean function
	std::string functionName = "";
	
	/// @brief file name of dataset of real data (not expert/ideal data). first line of file are attribute names
	std::string filename = "dataset.csv";

	/// @brief true if there is real data supplied
	bool realData = false;

	/// @brief of the real dataset, if applicable
	std::vector<std::string> real_attributes;

	// refactored flags for synchronizing between front and back end
	std::mutex* turnFlag = nullptr;
	bool *endFlag = nullptr;

	// flag to determine whether we are using ML expert or real expert
	bool *mlFlag = nullptr;

	/// @brief check whether each Chain has been ordered when user is manually ordering them
	std::vector<bool> chainsVisited;

	/// @brief the order of questions that are asked to the expert/user. Even elements are the Chain of the asked question, whereas the odd element is the vector.
	/// used for the summary in the results output.
	std::vector<int> orderOfAskingSummary;

	/// @brief the order of Hansel Chains to be used
	int orderOption = 1;

	/// @brief the total number of questions that were asked
	int questionsAsked = 0;

	/// @brief this number, at the time of visitation, is the number that represents the order in the sequence of questions. Used to assign planned and updated query order
	int questionOrder = 1;

	/// @brief the number of Hansel Chains
	int numChains = -1;

	/// @brief signals whether the majority flag WAS USED or not. Value does not change, unlike useMajorityFlag
	bool usedMajorityFlag = false;

	/// @brief if majority flag is used, then this states how many true majority vectors there are (user specified). A value of 0 means the number is unknown.
	int trueMajority = 0;

	/// @brief indices of successful vectors in order of Hansel Chain and vector, alternating even and odd indices. Used for chainJump majority vectors.
	std::vector<int> trueVectorInd;


	/// @brief first element is chain, next element is vector, next element is if that vector is visited
	std::vector<int> majorityVectors;

	/// @brief represents what vectors to rectify when adding new attributes
	std::vector<int> addNewAttributesFor;

	/// @brief a list of vectors where each vector cannot be represented by the monotone Boolean Function, so if there is a lower one in the chain that it belongs to, 
	/// they will be represented by a "NOT" clause
	std::vector<dvector*> nonMonotonicVectors;

	/// @brief truye if nonMonotonicVectors is used
	bool non_monotone = false;


	/// @brief generates a Hansel Chain from a given dimension and number
	/// @param num 
	/// @param vector_dimension 
	/// @param chain a Hansel Chain
	/// @return a Hansel Chain
	std::vector<std::vector<dvector>> genChains(int num, int vector_dimension, std::unordered_map<int, std::vector<std::vector<dvector>>> chain);


	/// @brief calculates a set of Hansel Chains from a given dimension and k-valued attributes
	/// @param vector_dimension 
	void calculateHanselChains(int vector_dimension);


	/// @brief ask if majority flag should be used, and if yes, then asks questions based on a random sequence of "majority vectors."
	/// These vectors do not have "prior" expansions because they occur before any other expansion that they produce.
	/// Hence, they are not prior to any expansion except another majority vector, which is not possible because they 
	/// would have the same Hamming Norm. Also assigns planned and updated order of questions
	void askMajorityFlag();


	/// @brief Find the majority vectors in the set of Hansel Chains
	void findMajorityVectors();


	/// @brief take known low units and expand them before the interview and after order has been established through the pilot questions
	void expandKnownLowUnits();


	/// @brief Updates the query order of the vector, then calls askingOfQuestion(), and then determines if it should be expanded or was already expanded.
	/// @param i the Hansel Chain
	/// @param j the vector in the Hansel Chain
	/// @param vector_class vector class variable to be assigned to
	/// @return true if the vector was already expanded (skip this vector). False otherwise.
	bool questionFunc(int i, int j, int& vector_class);


	/// @brief the order of questions in a static pattern (the assigned order
	void staticOrderQuestionsFunc();


	/// @brief Check if the index i corresponds to the Hansel Chain index of a true majority vector
	/// @param i index of a Hansel Chain
	/// @return True if the index i corresponds to the Hansel Chain index of a true majority vector
	bool chainJumpMajorityFlagHelper(int i);


	/// @brief the order of questions in a chainJump pattern (the assigned order as default, but with chainJump capabilities)
	void chainJumpOrderQuestionsFunc();


	/// @brief binary search methodology for questions. binary search is intra-chain
	void binarySearch(int i, int l, int r);


	/// @brief order the Hansel Chains by MST (kruskal's algorithm)
	void mstHanselChainOrder();


	/// @brief dijkstra's algorithm
	void shortestPathHanselChainOrder();

	/// @brief simply sort by shortest distance between pairs of Hansel Chains
	void simpleSortHanselChainOrder();



	/// @brief expandable expansions and unexpandable expansions
	/// Expandable expansions are expansions which are used. Unexpandable expansions are expansions which are unused 
	/// because either the vector that would have been expanded was already expanded or if the vector of origin's 
	/// class is different than that of the expansion (e.g. a vector of class 1 with an expansion of class 0).
	/// @param newValue the new value at the point k. Determined by k-value for that attribute, so newValue is between 0 and k
	/// @param i the Hansel Chain
	/// @param j a vector in the Hansel Chain
	/// @param p an element in the vector that was determined to be changed
	/// @param startChain equal to either i for a standard ordering of questions, or 0 for majority vector questions.
	void possibleExpansions(int newValue, int i, int j, int p, int startChain);


	/// @brief aux function used to expand a vector upwards
	/// @param vector 
	/// @param vector_class 
	void expandUp(int i, int j, dvector* vector, int vector_class);


	/// @brief aux function used to expand a vector downwards
	/// @param vector 
	/// @param vector_class 
	void expandDown(int i, int j, dvector* vector, int vector_class);


	/// @brief check up Expansions
	/// @param i 
	/// @param j 
	/// @param vector_class 
	/// @param visited_map since multiple sources 
	/// @return true when recursion should be used. false otherwise
	void checkUp(int i, int j, int vector_class, std::unordered_map<int, std::unordered_set<int>>* visited_map);


	/// @brief check down expansions. 
	/// @param i 
	/// @param j 
	/// @param vector_class 
	/// @return true if recursion should be used. false otherwise
	void checkDown(int i, int j, int vector_class, std::unordered_map<int, std::unordered_set<int>>* visited_map);

	
	/// @brief expand an entire chain
	/// @param i 
	/// @param j 
	/// @param vector_class 
	//void chainExpansions(int i, int j, int vector_class);


	/// @brief find a dual expansion in a chain
	/// @param i 
	void findDualExpansion(int i);


	/// @brief expand the space between two datapoints
	/// @param i the hansel chain
	/// @param l left 
	/// @param r right
	void dualExpansion(int i, int l, int r);


	/// @brief checks the possible expansions for a given vector (i, j), and expands those vectors if possible. recurses breadthwise to get all possible expansions
	/// @param vector_class 
	/// @param i the Hansel Chain
	/// @param j a vector in the Hansel Chain
	void checkExpansions(int vector_class, int i, int j);


	/// @brief similar to checkExpansions, except it checks if a vector was expanded and hasn't been monotonically fixed. Used for violation of monotonicity rectification.
	/// @param vector_class 
	/// @param i the Hansel Chain
	/// @param j a vector in the Hansel Chain
	void fixExpansions(int vector_class, int i, int j);


	/// @brief helper function that asks the user a question. Assigns the final query order
	/// @return the class of the vector;
	int askingOfQuestion(int i, int j);


	/// @brief assign numbers to vectors
	void numberAssignment();


	/// @brief call checkViolationOfMonotonicity ofor vectors whichc have violations of monotonicity
	void f_change_check();


	/// @brief monotonicity reaffirmation: described in detail in README
	/// @param i the hansel chain
	/// @param a the adjacent vector
	/// @param j the source of the f-change or potential violation
	void monotonicityReaffirmation(int i, int a, int j, int vector_class);


	/// @brief determine what method to use to fix violations of monotonicity
	void checkViolationOfMonotonicityMethod(int i, int j);


	/// @brief fix violation of monotonicity by changing the class
	/// @param root is true if the vector in question was asked. False if it was expanded.
	void fixViolationOfMonotonicityClass(int i, int j, int vector_class, bool preserve);


	/// @brief add a new attribute to the dataset to fix violation of monotonicity
	void fixViolationOfMonotonicityAddAttr();


	/// @brief add new attributes to real data
	/// @return the dataset, minus the classes
	std::vector<std::vector<int>> addNewAttributesToRealData();


	void changeAttributesOfRealData(std::vector<std::vector<int>> dataset);


	void applyBoolFuncToRealData(std::vector<std::vector<int>> boolFunc, std::vector<std::vector<int>> dataset);


	void findTrueAttributes();


	/// @brief call possibleExpansions on every single vector
	void calculateAllPossibleExpansions();


	/// @brief calculate the function based on the user's answers
	/// @param target restore with respect to the target value
	/// @return a pair of the function: a non-simplified function, and a simplified function
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> restoreFunction(int target);


	/// @brief convert the function to a pair of strings
	/// @return a pair of the function: a simplified function and a non-simplied version of the function
	std::pair<std::string, std::string> functionToString(std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> boolFuncPair, std::string sign);


	/// @brief write a table to the results file that contains the user's answers
	/// @param results output file
	/// @param boolFuncStrSimplified 
	/// @param boolFuncStrNonSimplified 
	/// @param include_violation variable which speciied whether or not to include the "fixed" field of the dvector struct
	void printTable(std::fstream& results, std::vector<int> targets, std::vector<std::string> boolFuncStrSimplified, std::vector<std::string> boolFuncStrNonSimplified, bool include_f_change);

	/// @brief parse a string by a given delimiter into a vector of ints
	std::vector<int> parse_input_int(char delimiter, std::string temp);


	/// @brief parse a string by a given delimeter into a vector of strings
	std::vector<std::string> parse_input_string(char delimiter, std::string temp);


	std::map<int, std::vector<std::vector<int>>> readFile(std::string fileName);


	void assignOracle(std::map<int, std::vector<std::vector<int>>> oracle);
};


/// @brief generic printing function
/// @tparam T 
/// @param t 
/// @param width the width of the output in terms of character spaces
/// @param separator the characters to use as fillers in the output if needed to reach the specified width
template<typename T>
void print(T t, const int& width, const char& separator);


/// @brief << overload for printing std::vector<int>
std::ostream& operator<<(std::ostream& out, const std::vector<int>& v);


/// @brief << overload for printing std::pair<int, int>
std::ostream& operator<<(std::ostream& out, const std::pair<int, int>& p);
