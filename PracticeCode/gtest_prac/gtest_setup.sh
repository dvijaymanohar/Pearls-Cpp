#!/bin/bash 

cd ~/
git clone https://github.com/google/googletest.git
cd googletest
mkdir install
cd install
cmake ../  #creates a make file
make #compiles Google Test
sudo make install #installs Google Test
echo "export CPLUS_INCLUDE_PATH=/usr/local/include" >> ~/.bash_profile
echo "export LIBRARY_PATH=/usr/local/lib" >> ~/.bash_profile

source ~/.bash_profile
