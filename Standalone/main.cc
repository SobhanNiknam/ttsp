/**
 * Main source file of the application.
 */

#include "TTSP.h"
#include "configurationParameters.h"



/**	Main function of the application.
 *
 */
int main(int argc, char *argv[])
{
	ConfigurationParameters configurationParameters;

	if(argc > 1)
	{	
		// Read the configuration from the command line
		configurationParameters.parseCommandLine(argc, argv);

		// Verify the configuration
		if(configurationParameters.verify() == false)
			exit(-1);

    	TTSP ttsp(configurationParameters.numberOfCores, configurationParameters.ThermalModelMatrixFileName, \
			  configurationParameters.Tamb, configurationParameters.Tdtm, configurationParameters.P_inactive_Core, \
			  configurationParameters.Pb_epoch_length, configurationParameters.Pb_overhead);

    	std::vector<double> budgets = ttsp.powerBudgetTTSP(configurationParameters.CoreMappingFileName, configurationParameters.TransientTemperatureFileName);
	}
	else
		cout << "Error: Invalid command line. Please check usage using -help." << endl;	
}