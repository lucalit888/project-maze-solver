#!/bin/bash

# testing.sh
# 
# Connor, Mack, Luca, Sean


# -------INVALID ARGUMENTS--------
echo -e "TESTING ARGUMENT VALIDATION"

# Test AMStartup w/ 0 arguments 
echo "-> Testing w/ 0 arguments"
./AMStartup
echo -e "\n"

# Test AMStartup w/ hostname only
echo "-> Testing w/ hostname only"
./AMStartup -h flume.cs.dartmouth.edu
echo -e "\n"

# Test AMStartup w/ difficulty only
echo "-> Testing w/ difficulty only"
./AMStartup -d 3
echo -e "\n"

# Test AMStartup w/ numAvatars only
echo "-> Testing w/ numAvatars only"
./AMStartup -n 3
echo -e "\n"

# Test AMStartup w/ 2 arguments
echo "-> Testing w/ 2 arguments"
./AMStartup -h flume.cs.dartmouth.edu -d 3
echo -e "\n"

# Test AMStartup with invalid difficulty
echo "-> Testing w/ invalid difficulty"
./AMStartup -h flume.cs.dartmouth.edu -d 10 -n 3
echo -e "\n"

# Test AMStartup with invalid numAvatars
echo "-> Testing w/ invalid numAvatars"
./AMStartup -h flume.cs.dartmouth.edu -d 5 -n 11
echo -e "\n"


# -------NETWORK TESTING--------
echo -e "TESTING NETWORK"
# Test AMStartup with invalid hostname
echo "-> Testing w/ invalid hostname"
./AMStartup -h fake.cs.dartmouth.edu -d 5 -n 3
echo -e "\n"


# -------UNIT TESTING----------
echo -e "UNIT TESTING"
# See designTest.c and graphicstest.c for each test in those files
echo "-> Unit testing avatar.c and mazeSolver.c modules" >> testing.out
./designTest >> testing.out
echo -e "\n" >> testing.out

echo "-> Unit testing graphics.c module"
./graphicstest
