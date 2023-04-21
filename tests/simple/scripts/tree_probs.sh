#!/bin/bash

# scenario 1: pure birth, no hybridization
echo "scenario 1: pure birth, no hybridization"
./calcDiversiNet -t tests/simple/data/yule_tree.tre -k 128 -l 0.5 -m 0 -e 0 -z 0 -n 0 -s 0 -p 1 -o false

# scenario 2: pure birth, one asymmetrical hybridization event
echo "scenario 2: pure birth, one asymmetrical hybridization event"
./calcDiversiNet -t tests/simple/data/yule_asymmetrical_tree.tre -k 128 -l 0.5 -m 0 -e 0.1 -z 0 -n 0 -s 0 -p 1 -o false

# scenario 3: pure birth, one symmetrical hybridization event
echo "scenario 3: pure birth, one symmetrical hybridization event"
./calcDiversiNet -t tests/simple/data/yule_symmetrical_tree.tre -k 128 -l 0.5 -m 0 -e 0 -z 0.05 -n 0 -s 0 -p 1 -o false

# scenario 4: pure birth, one hybrid speciation event
echo "scenario 4: pure birth, one hybrid speciation event"
./calcDiversiNet -t tests/simple/data/yule_hybrid_speciation_tree.tre -k 128 -l 0.5 -m 0 -e 0 -z 0 -n 0.075 -s 0 -p 1 -o false

# scenario 5: pure birth, one allopolyploid speciation event
echo "scenario 5: pure birth, one allopolyploid speciation event"
./calcDiversiNet -t tests/simple/data/yule_allopolyploid_speciation_tree.tre -k 128 -l 0.5 -m 0 -e 0 -z 0 -n 0 -s 0.015 -p 1 -o false

# scenario 6: pure birth, no hybridization, incomplete sampling
echo "scenario 6: pure birth, no hybridization, incomplete sampling"
./calcDiversiNet -t tests/simple/data/yule_tree.tre -k 128 -l 0.5 -m 0 -e 0 -z 0 -n 0 -s 0 -p 0.5 -o false

# scenario 7: birth-death, no hybridization
echo "scenario 7: birth-death, no hybridization"
./calcDiversiNet -t tests/simple/data/yule_tree.tre -k 128 -l 0.5 -m 0.1 -e 0 -z 0 -n 0 -s 0 -p 0.5 -o false