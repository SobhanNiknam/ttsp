This repository contains the source code of the novel power budgeting technique, called "*TTSP: Transient-Temperature Based Safe Power Budgeting*". TTSP computes safe power constraint values as a function of the spatial alignment of active cores and their current temperature in a multi-/many-core processor. The use of T-TSP leads to better exploitation of the processor’s thermal heatroom and therefore improved performance in a thermally constrained environment.
-   Creator: Sobhan Niknam
-   License: XXXX

# Publication

Details of TTSP can be found in the following ICCD 2021 paper. Please consider citing this paper in your work if you use this software or a modified version of it.

> Sobhan Niknam, Anuj Pathania, and Andy A. Pimentel. **"T-TSP:Transient-Temperature Based Safe Power Budgeting in Multi-/Many-Core Processors."** *In The 39th IEEE International Conference on Computer Design (ICCD)*, October 24 - 27, 2021.

# Versions
## 1- [HotSniper](https://github.com/anujpathania/HotSniper) Plugin
Follow the steps below for applying the patch file, in TTSP/HotSniper_Plugin, that contains all changes to the original HotSniper:
  
  $ git clone git@github.com:anujpathania/HotSniper.git
  
  $ cd HotSniper
  
  $ git apply plugin.patch
  
  $ <build HotSniper according to default procedure mentioned in git@github.com:anujpathania/HotSniper.git>

## 2- Standalone


# Code Acknowledgements

  HotSniper: https://github.com/anujpathania/HotSniper
  
  MatEx: http://ces.itec.kit.edu/846.php
