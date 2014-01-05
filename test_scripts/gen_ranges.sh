#!/bin/bash
cd ..
make
cd outputs
rm output.txt output.csv output_radians.csv output_radians.txt
../main -of output.txt
../main -c -of output.csv
../main -r -of output_radians.txt
../main -r -c -of output_radians.csv
