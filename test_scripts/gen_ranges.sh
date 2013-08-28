#!/bin/bash
make
cd outputs
rm output.txt output.csv output_radians.csv output_radians.txt
../main 0 0 >> output.txt
../main 1 0 >> output.csv
../main 0 1 >> output_radians.txt
../main 1 1 >> output_radians.csv
