###################################################
# Makefile for WSN 1516 final project - code only
###################################################

all: sender receiver

sender: sender.cpp
	@clang++ -c -I. NCpacket.cpp
	@clang++ -c -I. utils_wsn.cpp
	@clang++ -c -I. $?
	@clang++ -o sender sender.o NCpacket.o utils_wsn.o

receiver: receiver.cpp
	@clang++ -c -I. NCpacket.cpp
	@clang++ -c -I. utils_wsn.cpp
	@clang++ -c -I. $?
	@clang++ -o receiver receiver.o NCpacket.o utils_wsn.o

utils: utils_wsn.cpp
	@clang++ -c -I. $?

ncpacket: NCpacket.cpp
	@clang++ -c -I. $?

clear:
	rm receiver sender *.o
