CPPTYPE = -std=c++17 

all: 
	@if [ ! -d ./build ]; then mkdir -p ./build; fi; 
	make cmsketch_test

cmsketch_test: ./cmsketch_test.cpp
	g++ -O2 -o ./build/cmsketch_test.out ./cmsketch_test.cpp