###################################################
# Makefile for WSN 1516 final project - code only
###################################################

all: sender receiver

sender: sender.cpp
	@g++ -std=c++11 -c -pthread -O3 NCpacket.cpp
	@g++ -std=c++11 -c -pthread -O3 utils_wsn.cpp
	@g++ -std=c++11 -c -pthread -O3 NCpacketHelper.cpp
	@g++ -std=c++11 -c -pthread -O3 $?
	@g++ -std=c++11 -pthread -O3 -o sender sender.o NCpacket.o utils_wsn.o NCpacketHelper.o

receiver: receiver.cpp
	@g++ -std=c++11 -c -pthread -O3 NCpacket.cpp
	@g++ -std=c++11 -c -pthread -O3 utils_wsn.cpp
	@g++ -std=c++11 -c -pthread -O3 NCpacketHelper.cpp
	@g++ -std=c++11 -c -pthread -O3 decoding_function.cpp
	@g++ -std=c++11 -c -pthread -O3 timecounter.cpp
	@g++ -std=c++11 -c -pthread -O3 $?
	@g++ -std=c++11 -pthread -O3 -o receiver timecounter.o receiver.o NCpacket.o utils_wsn.o decoding_function.o NCpacketHelper.o

clear:
	rm receiver sender *.o
