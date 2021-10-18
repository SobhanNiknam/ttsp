#include "TTSP.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <math.h>

using namespace std;

#ifndef M_E
#define M_E (2.7182818284590452354)
#endif

TTSP::TTSP(unsigned int numberCores, const std::string thermalModelFilename, double ambientTemperature, double maxTemperature, double inactivePower, long pb_epoch_length, long pb_time_overhead)
    : numberCores(numberCores), ambientTemperature(ambientTemperature), maxTemperature(maxTemperature), inactivePower(inactivePower), pb_epoch_length(pb_epoch_length), pb_time_overhead(pb_time_overhead) {

    std::ifstream f;
    f.open(thermalModelFilename.c_str());

    unsigned int numberUnits = readValue<unsigned int>(f);
    unsigned int numberNodesAmbient = readValue<unsigned int>(f);
    unsigned int numberThermalNodes = readValue<unsigned int>(f);

    if (numberUnits != numberCores) {
        std::cout << "Assertion error in thermal model file: numberUnits != coreRows * coreColumns" << std::endl;
		exit (1);
    }
    if (numberThermalNodes != 4 * numberUnits + 12) {
        std::cout << "Assertion error in thermal model file: numberThermalNodes != 4 * numberUnits + 12" << std::endl;
		exit (1);
    }
    if (numberNodesAmbient != numberThermalNodes - 3 * numberUnits) {
        std::cout << "Assertion error in thermal model file: numberNodesAmbient != numberThermalNodes - 3 * numberUnits" << std::endl;
		exit (1);
    }

    for (unsigned int u = 0; u < numberUnits; u++) {
        std::string unitName = readLine(f);
    }

    readDoubleMatrix(f, &BInv, numberThermalNodes, numberThermalNodes);

    Gamb = new double[numberNodesAmbient];
    for (int c = 0; c < numberNodesAmbient; c++) {
            Gamb[c] = readValue<double>(f);
    }

    eigenValues = new double[numberThermalNodes];
    for (int c = 0; c < numberThermalNodes; c++) {
        eigenValues[c] = readValue<double>(f);
    }
    
    readDoubleMatrix(f, &eigenVectors, numberThermalNodes, numberThermalNodes);
    readDoubleMatrix(f, &eigenVectorsInv, numberThermalNodes, numberThermalNodes);

    matrix_exponentials = new double*[numberThermalNodes];
    Inv_matrix_exponentials = new double*[numberThermalNodes];
    matrix_exponentials_overhead = new double*[numberThermalNodes];
    Inv_matrix_exponentials_overhead = new double*[numberThermalNodes];

    for(int i = 0; i < numberThermalNodes; i++){
        matrix_exponentials[i] = new double[numberThermalNodes];
        Inv_matrix_exponentials[i] = new double[numberThermalNodes];
        matrix_exponentials_overhead[i] = new double[numberThermalNodes];
        Inv_matrix_exponentials_overhead[i] = new double[numberThermalNodes];
    }   
    Tsteady = new double[numberThermalNodes];
    Tinit = new double[numberThermalNodes]; 
    exponentials = new double[numberThermalNodes];
    exponentials_overhead = new double[numberThermalNodes];

    for(int i = 0; i < numberThermalNodes; i++){
        exponentials[i] = pow((double)M_E, eigenValues[i] * pb_epoch_length * pow(10,-9));
        exponentials_overhead[i] = pow((double)M_E, eigenValues[i] * pb_time_overhead * 1e-9);
    }
    for(int k = 0; k < numberThermalNodes; k++){
        for(int j = 0; j < numberThermalNodes; j++){
            matrix_exponentials[k][j] = 0;
            Inv_matrix_exponentials[k][j] = 0;
            for(int i = 0; i < numberThermalNodes; i++){
                matrix_exponentials[k][j] += eigenVectors[k][i]*eigenVectorsInv[i][j]*exponentials[i];
                matrix_exponentials_overhead[k][j] += eigenVectors[k][i]*eigenVectorsInv[i][j]*exponentials_overhead[i];
            }
            Inv_matrix_exponentials[k][j] = (k==j) ? matrix_exponentials[k][j]-1.0: matrix_exponentials[k][j];
            Inv_matrix_exponentials_overhead[k][j] = (k==j) ? matrix_exponentials_overhead[k][j]-1.0: matrix_exponentials_overhead[k][j];
        }
    }

    f.close();
}

template<typename T>
T TTSP::readValue(std::ifstream &file) const {
    T value;
    file.read((char*)&value, sizeof(T));
    if(file.rdstate() != std::stringstream::goodbit){
        std::cout << "Assertion error in thermal model file: file ended too early." << std::endl;
        file.close();
        exit(1);
    }
    return value;
}

std::string TTSP::readLine(std::ifstream &file) const {
    std::string value;
    std::getline(file, value);
    if(file.rdstate() != std::stringstream::goodbit){
        std::cout << "Assertion error in thermal model file: file ended too early." << std::endl;
        file.close();
        exit(1);
    }
    return value;
}

void TTSP::readDoubleMatrix(std::ifstream &file, double ***matrix, unsigned int rows, unsigned int columns) const {
    (*matrix) = new double*[rows];
    for (unsigned int r = 0; r < rows; r++) {
        (*matrix)[r] = new double[columns];
        for (unsigned int c = 0; c < columns; c++) {
            (*matrix)[r][c] = readValue<double>(file);
        }
    }
}

void inplaceGauss(std::vector<std::vector<float> > &A, std::vector<float> &b) {
    unsigned int n = b.size();
    for (unsigned int row = 0; row < n; row++) {
        // divide
        float pivot = A.at(row).at(row);
        b.at(row) /= pivot;
        for (unsigned int col = 0; col < n; col++) {
            A.at(row).at(col) /= pivot;
        }

        // add
        for (unsigned int row2 = 0; row2 < n; row2++) {
            if (row != row2) {
                float factor = A.at(row2).at(row);
                b.at(row2) -= factor * b.at(row);
                for (unsigned int col = 0; col < n; col++) {
                    A.at(row2).at(col) -= factor * A.at(row).at(col);
                }
            }
        }
    }
}

/** powerBudgetTTSP
 * Compute per-core power budgets considering the location and transient temperature of the cores that (if matched by the power consumption) heats every core exactly to the critical temperature.
 */
std::vector<double> TTSP::powerBudgetTTSP(std::string temperature_file, std::string mapping_file) const {
    
    std::vector<int> activeIndices;
    std::vector<double> inactivePowers(numberCores, 0);
    std::vector<double> activeCores(numberCores, 0);
    std::vector<double> powers(numberCores, inactivePower);

	ifstream coreLogFile(mapping_file.c_str());

    for (int i = 0; i < numberCores; i++) {
        coreLogFile >> activeCores.at(i);
        if (activeCores.at(i)) {
            activeIndices.push_back(i);
        } else {
            inactivePowers.at(i) = inactivePower;
        }
    } 

    //1.Reading the transient temperatures 
	ifstream temperatureLogFile(temperature_file.c_str());
	std::string header;
    double temp;
 
    int component = 0;
    while(temperatureLogFile >> header >> temp)
    {
        Tinit[component] = temp - 273.15;
        component++;
    }
    temperatureLogFile.close();
    
    //2. Predicting the temperatures development during power budget computing (considering time overhead of the algorithm, t_ov = pb_time_overhead := 0 by default)
    std::vector<float> T_steady = getSteadyState(budgets);
    std::vector<float> pred_Tinit(activeIndices.size());
    for(unsigned int i = 0; i < activeIndices.size(); i++){
        pred_Tinit.at(i) = T_steady.at(activeIndices.at(i));
        for (unsigned int j = 0; j < activeIndices.size(); j++) {
            pred_Tinit.at(i) += matrix_exponentials_overhead[activeIndices.at(i)][activeIndices.at(j)]*(Tinit[activeIndices.at(j)]-T_steady.at(activeIndices.at(j)));
        }
    }

    //3. Computing the targeted steady-state termperatures
    std::vector<float> A(activeIndices.size());
    for (unsigned int i = 0; i < activeIndices.size(); i++) {
        A.at(i) = 0;
        for (unsigned int j = 0; j < activeIndices.size(); j++) {
            A.at(i) += matrix_exponentials[activeIndices.at(i)][activeIndices.at(j)]*pred_Tinit[activeIndices.at(j)];
        }
        A.at(i) -= maxTemperature;
    }
    T_steady = A;
        
    std::vector<std::vector<float> > B;
    for (unsigned int i = 0; i < activeIndices.size(); i++) {
        std::vector<float> row;
        for (unsigned int j = 0; j < activeIndices.size(); j++) {
            row.push_back(Inv_matrix_exponentials[activeIndices.at(i)][activeIndices.at(j)]);
        }
        B.push_back(row);
    }

    // Now solve B * Tsteady = A
    inplaceGauss(B, T_steady);
    for(int k = 0; k < activeIndices.size(); k++)
    {
        cout<<"[TTSP]: The initial, predicted (after time overhead), and targeted steady-state termperatures of core "<< activeIndices.at(k) <<" for the upcoming epoch: "<< Tinit[activeIndices.at(k)] <<", "<<pred_Tinit.at(k)<<","<<T_steady.at(k) <<endl; 
    }

    //4. Computing the corresponding power budget to the computed steady-state termperatures
    std::vector<float> tInactive = getSteadyState(inactivePowers);
    std::vector<float> ambient(activeIndices.size());
    std::vector<float> headroomTrunc(activeIndices.size());

    for (unsigned int i = 0; i < activeIndices.size(); i++) {
        int index = activeIndices.at(i);
        headroomTrunc.at(i) = T_steady.at(i) - tInactive.at(index);
    }
    std::vector<float> powersTrunc = headroomTrunc;
    std::vector<std::vector<float> > BInvTrunc;
    for (unsigned int i = 0; i < activeIndices.size(); i++) {
        std::vector<float> row;
        for (unsigned int j = 0; j < activeIndices.size(); j++) {
            row.push_back(BInv[activeIndices.at(i)][activeIndices.at(j)]);
        }
        BInvTrunc.push_back(row);
    }
    // Now solve BInvTrunc * powersTrunc = headroomTrunc
    inplaceGauss(BInvTrunc, powersTrunc);

    for (unsigned int i = 0; i < activeIndices.size(); i++) {
         powers.at(activeIndices.at(i)) = powersTrunc.at(i);
    }

    return powers;
}


std::vector<float> TTSP::getSteadyState(const std::vector<double> &powers) const {
    std::vector<float> temperatures(numberCores);
    for (unsigned int core = 0; core < (unsigned int)(numberCores); core++) {
        temperatures.at(core) = ambientTemperature;
        for (unsigned int i = 0; i < (unsigned int)(numberCores); i++) {
            temperatures.at(core) += powers.at(i) * BInv[core][i];
        }
    }
    return temperatures;
}
