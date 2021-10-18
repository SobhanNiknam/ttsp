/**	\file	configurationParameters.cpp
 *	\brief	Source file for general configuration parameters.
 */

#include "configurationParameters.h"




ConfigurationParameters::ConfigurationParameters()
{
	// Set defaults values for all configuration parameters
	numberOfCores = 64;
	Tamb = 45;
	Tdtm = 80;
	Pb_overhead = 0;
	Pb_epoch_length = 100000;
	P_inactive_Core = 0.3;
	outputFileName = "t-tsp";
	CoreMappingFileName = "";
	TransientTemperatureFileName = "";
	ThermalModelMatrixFileName = "";
}

void ConfigurationParameters::parseCommandLine(int argc, char *argv[])
{
	// Iterate through the command line
	int i = 1;
	while(i < argc){
		// Depending on the position of the string, it should be a parameter name or a value. Since we start with i=1, it has to be a name.
		// A parameter name, it should start with '-'
		if(argv[i][0] == '-'){
			Parameter newParameter;
			newParameter.name = &(argv[i][1]);
			i++;

			// Check if it is a valid name
			if(parameterNameValid(newParameter.name)){

				// Check if the new parameter has a value, or if there is no value after the parameter name
				if(i < argc){
					newParameter.value = argv[i];
					i++;

					// If valid, add the configuration parameter to the list
					addNewParameter(newParameter, true);
				}
				else{
					cout << "Error: Invalid command line. Parameter \"" << newParameter.name << "\" has no value. Please check usage." << endl;
					exit(1);
				}
			}
			else{
				cout << "Error: Invalid command line. Parameter name \"" << newParameter.name << "\" is invalid. Please check usage." << endl;
				exit(1);
			}
		}
		else{
			cout << "Error: Invalid command line. Please check usage." << endl;
			exit(1);
		}
	}
}


bool ConfigurationParameters::verify(void)
{
	if(numberOfCores <= 0){
		cout << "Error: The number of cores must be a positive integer value." << endl;
		return false;
	}
	if(Tamb < 0){
		cout << "Error: The ambient temperature cannot be a negative value." << endl;
		return false;
	}
	if(Tdtm < 0){
		cout << "Error: The temperature for triggering DTM cannot be a negative value." << endl;
		return false;
	}
	if(P_inactive_Core < 0){
		cout << "Error: The power of an inactive core cannot be a negative value." << endl;
		return false;
	}
	if(Pb_epoch_length < 0){
		cout << "Error: The length of a power budgeting epoch cannot be a negative value." << endl;
		return false;
	}
	if(Pb_overhead < 0){
		cout << "Error: The run-time overhead of power budgeting cannot be a negative value." << endl;
		return false;
	}
	if(ThermalModelMatrixFileName.size() <= 0){
		cout << "Error: There is no file with the values for the matrices of the thermal model." << endl;
		return false;
	}
	if(TransientTemperatureFileName.size() <= 0){
		cout << "Error: There is no file with the values for the transient temperature of cores." << endl;
		return false;
	}
	if(CoreMappingFileName.size() <= 0){
		cout << "Error: There is no file for the active core mapping on the platform." << endl;
		return false;
	}
	return true;
}


bool ConfigurationParameters::parameterNameValid(const string &newParameterName)
{
	if(	(newParameterName == "c") ||
		(newParameterName == "t") ||
		(newParameterName == "m") ||
		(newParameterName == "t_amb") ||
		(newParameterName == "t_dtm") ||
		(newParameterName == "cores") ||
		(newParameterName == "p_inactive_core") ||
		(newParameterName == "mapping") ||
		(newParameterName == "transient") ||
		(newParameterName == "thermal_model") ||
		(newParameterName == "l_epoch") ||
		(newParameterName == "t_ov") ||
		(newParameterName == "")){

		return true;
	}
	else{
		return false;
	}
}


void ConfigurationParameters::addNewParameter(const Parameter &newParameter, const bool &newHasPriority)
{
	if(newParameter.value.size() > 0){
		// Check if the parameter is repeated.
		int repeatedIndex = -1;
		for(unsigned int existingParameter = 0; existingParameter < parameters.size(); existingParameter++){
			if(newParameter.name == parameters[existingParameter].name)
				repeatedIndex = existingParameter;
		}

		// If it is not repeated or the new one has priority, then we add the value to the configuration if it matches the type.
		if((repeatedIndex < 0) || newHasPriority){

			// Start with the configuration parameters that are names of files
			if(newParameter.name == "o"){
				outputFileName = newParameter.value;
			}
			else if(newParameter.name == "c"){
				CoreMappingFileName = newParameter.value;
			}
			else if(newParameter.name == "t"){
				TransientTemperatureFileName = newParameter.value;
			}
			else if(newParameter.name == "m"){
				ThermalModelMatrixFileName = newParameter.value;
			}
			else{
				try{
					double dValue;
					stringstream streamValue(newParameter.value);
					streamValue.exceptions(stringstream::goodbit);
					streamValue >> dValue;
					if((streamValue.rdstate() == stringstream::goodbit) || (streamValue.rdstate() == stringstream::eofbit)){
						if(newParameter.name == "cores"){
							numberOfCores = dValue;
						}
						else if(newParameter.name == "t_amb"){
							Tamb = dValue;
						}
						else if(newParameter.name == "t_dtm"){
							Tdtm = dValue;
						}
						else if(newParameter.name == "p_inactive_core"){
							P_inactive_Core = dValue;
						}
						else if(newParameter.name == "l_epoch"){
							Pb_epoch_length = dValue;
						}
						else if(newParameter.name == "t_ov"){
							Pb_overhead = dValue;
						}
						else{
							return;
						}
					}
					else
						cout << "Error in configuration file: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
				}
				catch(...){
					cout << "Error in configuration file: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
				}
			}

			// If it is not repeated, then we add it to the list
			if(repeatedIndex < 0){
				parameters.push_back(newParameter);
			}
			// If it is repeated and the first value has priority, then we replace it
			else{
				parameters[repeatedIndex] = newParameter;
			}
		}
	}
}
