###################################################
# Makefile for WSN 1516 final project - code only
###################################################

all: sender

sender: sender.cpp
	@clang++ -c -I. NCpacket.cpp
	@clang++ -c -I. utils_wsn.cpp
	@clang++ -c -I. $?
	@clang++ -o sender sender.o NCpacket.o utils_wsn.o
