#!/bin/bash 



rm *.txt
echo -e "sent\tinfile\ttime\tgoodput\tthroughput\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logtx".txt
echo -e "dectime\trxtime\ttotrxpck\tdroppck\trxpck\tuselesspck\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logrx".txt
for K_TB_SIZE_VAL in 3500 4500 5500;
do
	N_TB_SIZE_VAL=$((K_TB_SIZE_VAL+1000))
	echo $K_TB_SIZE_VAL
	make clear
	sed "s/K_TB_SIZE_VALUE/$K_TB_SIZE_VAL/g" NCpacket.h.template > NCpacket.h.tmp
	sed "s/N_TB_SIZE_VALUE/$N_TB_SIZE_VAL/g" NCpacket.h.tmp > NCpacket.h
	make all
	rm NCpacket.h
	rm NCpacket.h.tmp


	echo $1

	for PER in 0.6 0.7
	do

		for i in `seq 1 10`;
		do
			echo $i
			./receiver audio.wav $PER >> "logrx".txt & # $1 is filename, $2 is PER, in background
			sleep 1
			./sender localhost localhost $1 $PER >> "logtx".txt # $1 is filename
			#simulate $1 $2
		done 
	done
done
