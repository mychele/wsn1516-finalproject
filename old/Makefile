###################################################
# Makefile for WSN 1516 final project - code only
###################################################

all: sender receiver

sender: sender.cpp
	@g++ -std=c++11 -c -I. NCpacket.cpp
	@g++ -std=c++11 -c -I. utils_wsn.cpp
	@g++ -std=c++11 -c -I. $?
	@g++ -std=c++11 -o sender sender.o NCpacket.o utils_wsn.o

receiver: receiver.cpp
	@g++ -std=c++11 -c -I. NCpacket.cpp
	@g++ -std=c++11 -c -I. utils_wsn.cpp
	@g++ -std=c++11 -c -I. $?
	@g++ -std=c++11 -o receiver receiver.o NCpacket.o utils_wsn.o

utils: utils_wsn.cpp
	@g++ -std=c++11 -c -I. $?

ncpacket: NCpacket.cpp
	@g++ -std=c++11 -c -I. $?

clear:
	rm receiver sender *.o
