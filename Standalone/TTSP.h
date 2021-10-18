#ifndef __TTSP_H
#define __TTSP_H


#include <fstream>
#include <iostream>
#include <vector>

class TTSP {
public:
    TTSP(unsigned int numberOfCores, const std::string thermalModelFilename, double ambientTemperature, double maxTemperature, double inactivePower, long pb_epoch_length, long pb_time_overhead);
    std::vector<double> powerBudgetTTSP(std::string temperature_file, std::string mapping_file) const;
    std::vector<float> getSteadyState(const std::vector<double> &powers) const;

private:

    double ambientTemperature;
    double maxTemperature;
    double inactivePower;
    template<typename T> T readValue(std::ifstream &file) const;
    std::string readLine(std::ifstream &file) const;
    void readDoubleMatrix(std::ifstream &file, double ***matrix, unsigned int rows, unsigned int columns) const;

    unsigned int numberCores;

    long pb_epoch_length;
    long pb_time_overhead;
    double *Gamb;
    double *eigenValues;
    double *Tsteady;
    double *Tinit;
    double **BInv;

    double **eigenVectors;
    double **eigenVectorsInv;
    double *exponentials;
    double *exponentials_overhead;
    double **matrix_exponentials;
    double **Inv_matrix_exponentials;
    double **matrix_exponentials_overhead;
    double **Inv_matrix_exponentials_overhead;

    std::vector<double> budgets;
};

#endif