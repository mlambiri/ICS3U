#!/bin/bash
cat testconfig1.txt > testresults.txt; ./pong -v -d -l 1  -c testconfig1.txt >> testresults.txt
cat testconfig1.txt >> testresults.txt; ./pong -v -d -l 1 -c testconfig2.txt >> testresults.txt
cat testconfig1.txt >> testresults.txt; ./pong -v -d -l 1 -c testconfig3.txt >> testresults.txt
grep Info testresults.txt
grep Warn testresults.txt
grep Error testresults.txt
