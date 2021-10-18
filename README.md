This repository contains the source code of the novel power budgeting technique, called "*TTSP: Transient-Temperature Based Safe Power Budgeting", for multi-/many-core processors.
-   Creator: Sobhan Niknam
-   License: XXXX

# Publication

Details of TTSP can be found in our ICCD 2021 paper, and please consider citing this paper in your work if you find this tool useful in your research.

> Sobhan Niknam, Anuj Pathania, and Andy A. Pimentel. **"T-TSP:Transient-Temperature Based Safe Power Budgeting in Multi-/Many-Core Processors."** *In The 39th IEEE International Conference on Computer Design (ICCD), October 24 - 27, 2021.

# Versions
## 1- HotSniper Plugin
Follow the steps below for applying the patch file, in TTSP/HotSniper_Plugin, that contains all changes to the original HotSniper:
  
  $ git clone git@github.com:anujpathania/HotSniper.git
  $ cd HotSniper
  $ git apply plugin.patch
  $ <build HotSniper according to default procedure mentioned in git@github.com:anujpathania/HotSniper.git>

## 2- Standalone


# Code Acknowledgements

  HotSniper: https://github.com/anujpathania/HotSniper
  
  MatEx: http://ces.itec.kit.edu/846.php
