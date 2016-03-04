###################################################
# Makefile for WSN 1516 final project - code only
###################################################

all: sender receiver

sender: sender.cpp
	@g++ -std=c++11 -c -pthread NCpacket.cpp
	@g++ -std=c++11 -c -pthread utils_wsn.cpp
	@g++ -std=c++11 -c -pthread $?
	@g++ -std=c++11 -pthread -o sender sender.o NCpacket.o utils_wsn.o

receiver: receiver.cpp
	@g++ -std=c++11 -c -pthread NCpacket.cpp
	@g++ -std=c++11 -c -pthread utils_wsn.cpp
	@g++ -std=c++11 -c -pthread decoding_function.cpp
	@g++ -std=c++11 -c -pthread timecounter.cpp
	@g++ -std=c++11 -c -pthread $?
	@g++ -std=c++11 -pthread -o receiver timecounter.o receiver.o NCpacket.o utils_wsn.o decoding_function.o

clear:
	rm receiver sender *.o
