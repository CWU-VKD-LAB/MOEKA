#include "moeka.h"


std::vector<int> moeka::init()
{
	std::vector<int> genericParentOrChildList;

	// automatic testing by using a dataset as an oracle
	if (std::filesystem::exists(generatedOraclePath) && generatedOraclePath != "")
	{
		auto oracle = readFile(generatedOraclePath);

		calculateHanselChains(dimension);
		numChains = (int)hanselChainSet.size();
		numConfirmedInChains.resize(numChains);
		chainsVisited.resize(numChains);

		assignOracle(oracle);
	}
	else
	{
		if (dimension <= 0) // only executed when the constructor was not initialized with a child_attributes 
		{
			std::cout << "How many attribute_names are in this dataset (what is the dimension)?";
			std::cout << "\nEnter: " << std::flush;
			std::cin >> dimension;
			std::cin.clear();
			std::cin.ignore(1000, '\n');

			attribute_names.resize(dimension);
			
			// execute if a hiearchy of functions is allowed
			if (hierarchy)
			{
				// start sub-functions
				std::cout << "Are there any nested attribute_names (sub-functions) for this dataset?"
					<< "\nIf not, then enter -1. If yes, please enter a comma-delimited list of which attribute_names have nested attribute_names, where:" << std::endl;

				for (int i = 1; i < dimension + 1; i++)
				{
					std::cout << i << " - x" << i << std::endl;
				}

				std::cout << "Please enter: " << std::flush;
				std::string temp;
				std::getline(std::cin, temp);
				std::cin.clear();
				std::cin.ignore(1000, '\n');

				auto tokens = parse_input_int(',', temp);

				if (!tokens.empty() && tokens[0] != -1)
				{
					genericParentOrChildList.reserve((int)tokens.size() + 1);
					genericParentOrChildList.push_back(-1);

					for (auto token : tokens)
					{
						try
						{
							//int i = stoi(token);
							genericParentOrChildList.push_back(token);
						}
						catch (std::exception& e)
						{
							std::cerr << "User input fail. " << e.what() << std::endl;
						}
					}
				}
				// end sub-functions
			}
		}

		// start parent functions (grouping of attributes)
		if (dimension > 5 && hierarchy)
		{
			std::cout << "Are there any groupings of attribute_names?"
				<< "\nIf not, then enter -1. If yes, please enter how many groups there are (please keep this to 2-5 groups): " << std::endl;
			int groups;
			std::cin >> groups;
			std::cin.clear();
			std::cin.ignore(1000, '\n');

			if (groups > 0)
			{
				std::cout << "Please note that all attribute_names must be in a group since this option was chosen, even if that group is of attribute_names that are unrelated." << std::endl;

				genericParentOrChildList.reserve(groups + dimension);

				for (int i = 0; i < groups; i++)
				{
					std::cout << "Please enter a comma-delimited list of what attribute_names are in group " << i + 1 << ": " << std::flush;

					std::string temp;
					std::getline(std::cin, temp);
					std::cin.clear();
					std::cin.ignore(1000, '\n');

					auto tokens = parse_input_int(',', temp);

					genericParentOrChildList.push_back((int)tokens.size()); // group size

					for (auto token : tokens)
					{
						try
						{
							genericParentOrChildList.push_back(token); // each attribute in this group
						}
						catch (std::exception& e)
						{
							std::cerr << "User input fail. " << e.what() << std::endl;
						}
					}
				}

				std::cout << "Since parent attribute_names have been specified (the groupings), the expert data mining process will start with those attribute_names first." << std::endl;
				return genericParentOrChildList; // return early since this object will be popped out of the tree anyway
			}
		}

		// ask for k-values for each attribute
		for (int i = 0; i < dimension; i++)
		{
			if (askKV)
			{
				std::cout << "\nWhat is the k_value of attribute " + attribute_names[i].name + "?";
				std::cout << "\nEnter: " << std::flush;

				try
				{
					std::cin >> attribute_names[i].kv;

					if (attribute_names[i].kv < 2)
					{
						attribute_names[i].kv = 2;
					}

					std::cin.clear();
					std::cin.ignore(1000, '\n');
				}
				catch (std::exception& e)
				{
					std::cerr << e.what() << std::endl;
					attribute_names[i].kv = 2;
				}
			}
			else
			{
				attribute_names[i].kv = 2;
			}
		}

		// get function_kv if it wasnt already assigned by the constructor
		if (function_kv == -1)
		{
			std::cout << "What is kn+1? It must be 2 or greater. Please enter: " << std::flush;
			int c;
			std::cin >> c;

			if (c < 2)
			{
				std::cout << "By default, kn+1 will be 2." << std::endl;
				function_kv = 2;
			}
			else
			{
				function_kv = c;
			}
		}

		calculateHanselChains(dimension);
		numChains = (int)hanselChainSet.size();
		numConfirmedInChains.resize(numChains);
		chainsVisited.resize(numChains);
	}

	// name every attribute to reduce confusion for user
	for (int i = 0; i < dimension; i++)
	{
		//TODO: change this ???
		/*std::cout << "\nWhat is the name of attribute x" + std::to_string(i + 1) << "?";
		std::cout << "\nEnter: " << std::flush;
		std::cin >> attributes[i];
		std::cin.clear();
		std::cin.ignore(1000, '\n');*/
		attribute_names[i].name = attributeSymbol + std::to_string(i + 1);
	}

	// let the user determine the order of the Hansel Chains
	std::cout << "\nWhat order to use for the Hansel Chains?";
	std::cout << "\n0 - Shortest Hansel Chain First";
	std::cout << "\n1 - Longest Hansel Chain First";
	std::cout << "\n2 - Manual Hansel Chain Order";
	std::cout << "\n3 - Default Hansel Chain Algorithm Order";
	std::cout << "\n4 - MST Hansel Chain Order";
	std::cout << "\n5 - Simple Sort Hansel Chain Order"; // AKA linkage sort order
	std::cout << "\n6 - Shortest Path Hansel Chain Order";
	std::cout << "\nEnter: " << std::flush;

	try
	{
		std::cin >> orderOption;
	}
	catch (std::exception& e)
	{
		std::cerr << "user input fail. " << e.what() << std::endl;
	}

	findTrueAttributes();

	// determine if program should be chainJump or static
	std::cout << "\nDo you want to use chain jumping (1/0)?";
	std::cout << "\nEnter: " << std::flush;
	std::cin >> chainJump;

	askMajorityFlag();

	std::cout << "\nShould the program start at the top of the Hansel Chains? (1/0)";
	std::cout << "\nEnter: " << std::flush;
	std::cin >> top;
	

	return genericParentOrChildList;
}


std::vector<int> moeka::initFromUI(int attributeCount, std::vector<std::string> attributes, std::vector<int> kValues, int functionKV, int staticInterChainOrder,
	std::vector<int> trueAttributes, bool chainJump, bool majority, bool topBottomOrBinarySearch)
{
	std::vector<int> genericParentOrChildList;

	// TODO: hierachical modeling 

	function_kv = functionKV;

	// init attributes and k-values
	attribute_names.resize(attributeCount);
	dimension = attributeCount;
	
	for (size_t i = 0; i < dimension; i++)
	{
		attribute_names[i].kv = kValues[i];
		attribute_names[i].name = attributeSymbol + std::to_string(i + 1);
	}

	// init hansel chains
	calculateHanselChains(dimension);
	numChains = (int)hanselChainSet.size();
	numConfirmedInChains.resize(numChains);
	chainsVisited.resize(numChains);

	int orderOption = 0; 

	if (staticInterChainOrder == 0)
	{
		orderOption = 3; // default order
	}
	else if (staticInterChainOrder == 1)
	{
		orderOption = 0; // SHCF
	}
	else if (staticInterChainOrder == 2)
	{
		orderOption = 1; // LHCF
	}
	else if (staticInterChainOrder == 3)
	{
		orderOption = 5; // linkage sort order aka simple sort hc order (should be refactored)
	}

	// TODO: implement new trueAttributes function
	for (int i = 0; i < dimension; i++)
	{
		if (trueAttributes[i] != -1)
		{
			attributes[i] = trueAttributes[i];
		}
	}

	this->chainJump = chainJump;

	return genericParentOrChildList;	
}

#pragma comment(lib, "Ws2_32.lib")

std::vector<int> moeka::initForMLInterview(std::string classifierName, std::string datasetPath) {
	this->mlInterview = true;
	namespace fs = std::filesystem;

	// ────────────────────────────────────────────────────────────────────────────
	//  Define ML_Oracles directory relative to current working directory
	// ────────────────────────────────────────────────────────────────────────────
	// Derive the source directory from the location of this source file
	fs::path sourceDir = fs::path(__FILE__).parent_path();
	// ML_Oracles sits alongside this .cpp in the same directory
	fs::path oracleDir = sourceDir / "ML_Oracles";
	if (!fs::is_directory(oracleDir))
		throw std::runtime_error("Cannot find ML_Oracles in current directory: " + oracleDir.string());

	// ────────────────────────────────────────────────────────────────
//  Compute dataset CSV path by stripping any leading directories from the input
// ────────────────────────────────────────────────────────────────
	fs::path dsInput(datasetPath);
	// Extract just the filename (e.g. "fisher_iris.csv")
	std::string baseName = dsInput.filename().string();
	// If there's no extension, append .csv
	if (!dsInput.has_extension()) {
		baseName += ".csv";
	}
	fs::path dataFile = fs::current_path() / "datasets" / baseName;
	if (!fs::exists(dataFile))
		throw std::runtime_error("Dataset file not found: " + dataFile.string());
	// ────────────────────────────────────────────────────────────────
	//  Build primary paths
	// ────────────────────────────────────────────────────────────────
	fs::path makeModelsPy = oracleDir / "makeMLModels.py";
	fs::path serverPy = oracleDir / "MLInterviewServer.py";
	fs::path attrFile = oracleDir / "functionAttributes.txt";
	fs::path modelFile = oracleDir / (datasetPath + "_" + classifierName + ".sav");

	this->oracleML_path = modelFile.string();
	this->realOrdinalNormalizedDataset = datasetPath + "_ordinal_normalization.csv";

	// ────────────────────────────────────────────────────────────────
	//  Run makeMLModels.py within ML_Oracles
	// ────────────────────────────────────────────────────────────────
	std::string cmdMake = "cmd /C \"cd /d " + oracleDir.string() +
		" && python \"" + makeModelsPy.filename().string() +
		"\" \"" + dataFile.string() +
		"\" \"" + classifierName + "\"\"";
	std::cout << "[DEBUG] " << cmdMake << std::endl;
	std::system(cmdMake.c_str());

	// ────────────────────────────────────────────────────────────────
	//  Locate the .sav model
	// ────────────────────────────────────────────────────────────────
	fs::path savFile;
	for (auto& e : fs::directory_iterator(oracleDir)) {
		if (e.is_regular_file() && e.path().extension() == ".sav") {
			savFile = e.path();
			break;
		}
	}
	if (savFile.empty())
		throw std::runtime_error("No .sav model in " + oracleDir.string());
	std::cout << "Found model: " << savFile << "\n";

	// ────────────────────────────────────────────────────────────────
	//  Launch the Python interview server
	// ────────────────────────────────────────────────────────────────
	std::string cmdServer = "cmd /C \"cd /d " + oracleDir.string() +
		" && python \"" + serverPy.filename().string() +
		"\" 6969 \"" + savFile.string() + "\"\"";
	STARTUPINFOA        si{ sizeof(si) };
	PROCESS_INFORMATION pi{};
	if (CreateProcessA(nullptr, _strdup(cmdServer.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		std::cout << "Server launched." << std::endl;
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	// ────────────────────────────────────────────────────────────────
	//  Connect over Winsock
	// ────────────────────────────────────────────────────────────────
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		throw std::runtime_error("WSAStartup failed");
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET)
		throw std::runtime_error("socket() failed");
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(6969);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	while (connect(ConnectSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Waiting for server..." << std::endl;
		Sleep(1000);
	}
	std::cout << "Connected to server." << std::endl;

	// ────────────────────────────────────────────────────────────────
	//  Read back the attributes file
	// ────────────────────────────────────────────────────────────────
	std::ifstream inFile(attrFile);
	if (!inFile)
		throw std::runtime_error("Missing functionAttributes.txt");
	inFile >> this->function_kv;
	std::string name; int kVal;
	while (inFile >> name >> kVal)
		attribute_names.push_back({ name, kVal, -1, -1 });
	inFile.close();
	fs::remove(attrFile);

	// ────────────────────────────────────────────────────────────────
	//  Compute Hansel chains
	// ────────────────────────────────────────────────────────────────
	this->dimension = static_cast<int>(attribute_names.size());
	calculateHanselChains(dimension);
	numChains = static_cast<int>(hanselChainSet.size());
	numConfirmedInChains.resize(numChains);
	chainsVisited.resize(numChains);

	// remove our saved ML model
	if (!savFile.empty() && fs::exists(savFile)) {
		fs::remove(savFile);
	}

	return {};
}



moeka::moeka(char attributeSymbol)
{
	this->attributeSymbol = attributeSymbol;
}


moeka::moeka(char attributeSymbol, std::string generatedOraclePath, int hanselChainOrder, bool useMajorityFlag, bool useBinarySearch, bool chainJump, bool top)
{
	this->attributeSymbol = attributeSymbol;
	this->generatedOraclePath = generatedOraclePath;
	this->hanselChainOrder = hanselChainOrder;
	this->useMajorityFlag = useMajorityFlag;
	this->useBinarySearch = useBinarySearch;
	this->chainJump = chainJump;
	this->top = top;
}


moeka::moeka(char attributeSymbol, std::string parent_attribute, int dimension) // used in top down approach after parent is constructed, create the children for that parent
{
	this->attributeSymbol = attributeSymbol;
	this->parent_attribute = parent_attribute;
	this->dimension = dimension;
	attribute_names.resize(dimension);
	childAttributes.resize(dimension);
}


moeka::moeka(char attributeSymbol, std::string parent_attribute, int dimension, int function_kv) // used in top down approach after parent is constructed, create the children for that parent
{
	this->attributeSymbol = attributeSymbol;
	this->parent_attribute = parent_attribute;
	this->dimension = dimension;
	this->function_kv = function_kv;
	attribute_names.resize(dimension);
	childAttributes.resize(dimension);
}


moeka::moeka(char attributeSymbol, std::vector<std::vector<std::string>> childAttributes) // used in top down approach, creating parent
{
	this->attributeSymbol = attributeSymbol;
	this->childAttributes = childAttributes;
	dimension = (int)childAttributes.size();
	attribute_names.resize(dimension);
}



