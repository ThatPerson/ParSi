#!/bin/bash
cd ..
make
cd outputs
rm output.txt output.csv output_radians.csv output_radians.txt
../main >> output.txt
../main -c >> output.csv
../main -r >> output_radians.txt
../main -c -r >> output_radians.csv
