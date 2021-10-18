/**
 *	Header file for general configuration parameters.
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;


/**
 *	Class that stores the configuration parameters from the command line.
 */
class ConfigurationParameters{
public:

	/* Structre with the definition pair of strings that forms a configuration parameter. */
	struct Parameter {
		string name;		/** Name of the configuration parameter.	*/
		string value;		/** Value of the configuration parameter.	*/
	};

	/**	
	 * Vector containing all the configuration parameters.	
	*/
	vector< Parameter > parameters;

	/**	
	 * 	Method that parses the command line arguments.
	 *	@param[in]	argc	Number of command line arguments.
	 *	@param[in]	argv	Strings with the command line arguments.
	 */
	void parseCommandLine(int argc, char *argv[]);

	/**	
	 * Method that verifies that the configuration parameters are valid.
	 */
	bool verify(void);

	/**	
	 * Method that prints the configuration parameters.
	 */
	void print(void);

	double Tamb;											/*	Ambient temperature [centigrade].											*/
	double Tdtm;											/*	Critical temperature in which DTM is triggered [centigrade].				*/
	double P_inactive_Core;									/*	Minimum power consumption for an inactive core.								*/
	double Pb_epoch_length;									/*	The length of epoch at which the power budgets are computed.				*/
	double Pb_overhead;										/*	The overhead of power budgeting algorithm.									*/
	int numberOfCores;										/*	Number of cores in the floorplan.											*/
	string outputFileName;									/*	Name of the output file in which to store the T-TSP values.					*/
	string CoreMappingFileName;								/*	Name of the input file of a core mapping for which to compute T-TSP.		*/
	string TransientTemperatureFileName;					/*	Name of the input file with the the transient temperature of cores.			*/
	string ThermalModelMatrixFileName;						/*	Name of the input file with the values of the hard-ware dependant matrices.	*/

	/**	Constructor. Initializes all variables with the default values.	*/
	ConfigurationParameters();

private:
	/**	
	 *	Method that checks whether the name of the new parameter is valid.
	 *	@param[in]	newParameterName	Name of the new parameter.
	 *	@return							True if the name is valid, false otherwise.
	 */
	bool parameterNameValid(const string &newParameterName);

	/**	
	 * 	Method that checks whether the value of the new parameter is valid.
	 *	@param[in]	newParameter		Name and value of the new parameter.
	 *	@param[in]	newHasPriority		When the parameter name is repeated and variable is true, the new value replaces the old one. When this variable is false, the old value is preserved.
	 */
	void addNewParameter(const Parameter &newParameter, const bool &newHasPriority);
};
