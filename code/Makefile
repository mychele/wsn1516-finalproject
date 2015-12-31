###################################################
# Makefile for WSN 1516 final project - code only
###################################################

all: sender receiver

sender: sender.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include NCpacket.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include utils_wsn.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include $?
	@g++ -std=c++11 -o sender sender.o NCpacket.o utils_wsn.o -L/Users/Michele/sw/lib -lntl -lm

receiver: receiver.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include NCpacket.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include utils_wsn.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include decoding_function.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include $?
	@g++ -std=c++11 -o receiver receiver.o NCpacket.o utils_wsn.o decoding_function.o -L/Users/Michele/sw/lib -lntl -lm

utils: utils_wsn.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include $? 

ncpacket: NCpacket.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include $?

decoding_function: decoding_function.cpp
	@g++ -std=c++11 -c -I/Users/Michele/sw/include $?

clear:
	rm receiver sender *.o
