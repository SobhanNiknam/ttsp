# TTSP
This repository contains the source code of the novel power budgeting technique, called "*TTSP: Transient-Temperature Based Safe Power Budgeting*". TTSP computes safe power constraint values as a function of the spatial alignment of active cores and their transient temperature in a multi-/many-core processor. The use of T-TSP leads to better exploitation of the processor’s thermal headroom and therefore improved performance in a thermally constrained environment.

# Publication

Details of TTSP can be found in the following ICCD 2021 paper. Please consider citing this paper in your work if you use this software or a modified version of it.

> Sobhan Niknam, Anuj Pathania, and Andy A. Pimentel. **"T-TSP:Transient-Temperature Based Safe Power Budgeting in Multi-/Many-Core Processors."** *In The 39th IEEE International Conference on Computer Design (ICCD)*, October 24 - 27, 2021.

# Versions
## 1- [HotSniper](https://github.com/anujpathania/HotSniper) Plugin
Follow the steps below for applying the patch file, in TTSP/HotSniper_Plugin, that contains all changes to the original HotSniper:
  
    git clone git@github.com:anujpathania/HotSniper.git
    cd HotSniper
    git apply plugin.patch
    <build HotSniper according to default procedure mentioned in git@github.com:anujpathania/HotSniper.git>

## 2- Standalone
To use standalone implementation of TTSP, several input files, i.e., a temperature trace file (containing transient temperature of thermal nodes), an eigeninput file (containing the eigenvectors, eigenvalues, and all required matrices corresponding to the RC thermal model), and an input file indicating the active cores, are required. An example for each of these input files is available in TTSP/standalone. Use the follwoing command to build and run the software using the given examplery input files:
  
  cd TTSP/standalone
  make
  ./TTSP -c givenMapping.map -t Temperature.init -m eigendata.bin
  
In addition, the tool has additional variables that can be seen using the following command:

  ./TTSP -help

# Code Acknowledgements

  HotSniper: https://github.com/anujpathania/HotSniper
  
  MatEx: http://ces.itec.kit.edu/846.php
