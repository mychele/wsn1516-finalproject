###################################################
# Makefile for WSN 1516 final project - code only
###################################################

all: sender receiver

sender: sender.cpp
	@g++ -std=c++11 -c -I../../lib/ntl/include -pthread NCpacket.cpp
	@g++ -std=c++11 -c -I../../lib/ntl/include -pthread utils_wsn.cpp
	@g++ -std=c++11 -c -I../../lib/ntl/include -pthread $?
	@g++ -std=c++11 -pthread -o sender sender.o NCpacket.o utils_wsn.o -L../../lib/ntl/lib -lntl -lm

receiver: receiver.cpp
	@g++ -std=c++11 -c -I../../lib/ntl/include -pthread NCpacket.cpp
	@g++ -std=c++11 -c -I../../lib/ntl/include -pthread utils_wsn.cpp
	@g++ -std=c++11 -c -I../../lib/ntl/include -pthread decoding_function.cpp
	@g++ -std=c++11 -c -pthread timecounter.cpp
	@g++ -std=c++11 -c -I../../lib/ntl/include -pthread $?
	@g++ -std=c++11 -pthread -o receiver timecounter.o receiver.o NCpacket.o utils_wsn.o decoding_function.o -L../../lib/ntl/lib -lntl -lm

clear:
	rm receiver sender *.o
