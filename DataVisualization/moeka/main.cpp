#include "moeka.h"

// testing of an individual MBF/MOF
void testIndividual()
{
	// oraclePaths
	//"MonotoneDataGenerator/kv2_5D_x1x3x5_v_x2x3x5_v_x4x5.csv"; //kv2_5D_x1x3x5_v_x2x3x5_v_x4x5.csv //kv3_7D_summation.csv
	moeka o('x');
	o.init();
	o.start();

	std::fstream results;
	results.open("housingResults.csv", std::ios::out | std::ios::app);
	o.printToFile(results); // start the parent
}

// test many different MBF and MOFs from a folder of oracle files.
void testBulk()
{

}


/*int main()
{
	testIndividual();
}*/


