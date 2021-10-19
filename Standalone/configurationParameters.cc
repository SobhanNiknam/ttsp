#include "configurationParameters.h"

ConfigurationParameters::ConfigurationParameters()
{
	// Set defaults values for all configuration parameters
	numberOfCores = 64; 				// Number of cores in the processor
	Tamb = 45; 							// Ambient temperature (in Celsius)
	Tdtm = 80;							// Thermal threshold (in Celsius)
	Pb_overhead = 0;					// Timing overhead of the power budgeting algorithm
	Pb_epoch_length = 100000; 			// Epoch length of power budgeting
	P_inactive_Core = 0.3;				// Power consumption of inactive cores
	CoreMappingFileName = ""; 		   	// File containing the list of active cores
	TransientTemperatureFileName = ""; 	// File containing the transient temperature of thermal nodes 
	ThermalModelMatrixFileName = "";   	// File containing matrices of the thermal model
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

			if(newParameter.name == "help"){
				cout << std::endl << "USAGE: " << argv[0]
                << " -c Core_Mapping_FileName" << endl
				<<   "              -t Transient_Temperature_FileName" << endl
				<<   "              -m Thermal_Model_FileName [-n Number_of_Cores]" << endl
				<<   "              [-a Ambient_Temperature_(in Celsius)]" << endl
				<<   "              [-d Threshold_Temperature_(in Celsius)]" << endl
				<<   "              [-e Epoch_Length]" << endl
				<<   "              [-i Power_of_inactive_core]" << endl
				<<   "              [-v timing_overhead_(in microseconds)]" << endl
                << std::endl;
				exit(1);	
			}

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
					cout << std::endl << "Error: Invalid command line. Parameter \"" << newParameter.name << "\" has no value. Please check usage using -help." << endl;
					exit(1);
				}
			}
			else{
				cout << std::endl << "Error: Invalid command line. Parameter name \"" << newParameter.name << "\" is invalid. Please check usage using -help." << endl;
				exit(1);
			}
		}
		else{
			cout << std::endl << "Error: Invalid command line. Please check usage using -help" << endl;
			exit(1);
		}
	}
}


bool ConfigurationParameters::verify(void)
{
	if(numberOfCores <= 0){
		cout << std::endl << "Error: The number of cores must be a positive integer value." << endl;
		return false;
	}
	if(Tamb < 0){
		cout << std::endl << "Error: The ambient temperature cannot be a negative value." << endl;
		return false;
	}
	if(Tdtm < 0){
		cout << std::endl << "Error: The temperature for triggering DTM cannot be a negative value." << endl;
		return false;
	}
	if(P_inactive_Core < 0){
		cout << std::endl << "Error: The power of an inactive core cannot be a negative value." << endl;
		return false;
	}
	if(Pb_epoch_length < 0){
		cout << std::endl << "Error: The length of a power budgeting epoch cannot be a negative value." << endl;
		return false;
	}
	if(Pb_overhead < 0){
		cout << std::endl << "Error: The run-time overhead of the power budgeting algorithm cannot be a negative value." << endl;
		return false;
	}
	if(ThermalModelMatrixFileName.size() <= 0){
		cout << std::endl << "Error: There is no file with the values for the matrices of the thermal model." << endl;
		return false;
	}
	if(TransientTemperatureFileName.size() <= 0){
		cout << std::endl << "Error: There is no file with the values for the transient temperature of cores." << endl;
		return false;
	}
	if(CoreMappingFileName.size() <= 0){
		cout << std::endl << "Error: There is no file for the active core mapping on the platform." << endl;
		return false;
	}
	return true;
}


bool ConfigurationParameters::parameterNameValid(const string &newParameterName)
{
	if( (newParameterName == "c") ||
		(newParameterName == "t") ||
		(newParameterName == "m") ||
		(newParameterName == "n") ||
		(newParameterName == "a") ||
		(newParameterName == "d") ||
		(newParameterName == "i") ||
		(newParameterName == "e") ||
		(newParameterName == "v") ||
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
			if(newParameter.name == "c"){
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
						if(newParameter.name == "n"){
							numberOfCores = dValue;
						}
						else if(newParameter.name == "a"){
							Tamb = dValue;
						}
						else if(newParameter.name == "d"){
							Tdtm = dValue;
						}
						else if(newParameter.name == "i"){
							P_inactive_Core = dValue;
						}
						else if(newParameter.name == "e"){
							Pb_epoch_length = dValue;
						}
						else if(newParameter.name == "v"){
							Pb_overhead = dValue;
						}
						else{
							return;
						}
					}
					else
						cout << std::endl << "Error in configuration file: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
				}
				catch(...){
					cout << std::endl << "Error in configuration file: Value of parameter \"" << newParameter.name << "\" is invalid." << endl;
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
