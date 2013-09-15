#!/bin/bash
cd ..
make
cd outputs
rm output.txt output.csv output_radians.csv output_radians.txt
../main -if ../sim/file.sim -of output.txt
../main -c -if ../sim/file.sim -of output.csv
../main -r -if ../sim/file.sim -of output_radians.txt
../main -r -c -if ../sim/file.sim -of output_radians.csv
