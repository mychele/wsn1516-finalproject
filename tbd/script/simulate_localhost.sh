#!/bin/bash 

# This script can be used to launch sender and receiver from the same CLI, in localhost, and tests different 
# values for N and K and PER

rm *.txt
rm *.log
echo -e "sent\tinfile\ttime\tgoodput\tthroughput\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logtx".txt
echo -e "dectime\trxtime\ttotrxpck\tdroppck\trxpck\tuselesspck\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE\tfaileddec" >> "logrx".txt

for K_TB_SIZE_VAL in 1000 1500 2500 3500 4500 5500;
do
	for INCREMENT in 1000 1250 1500 1750 2000;
	do
		N_TB_SIZE_VAL=$((K_TB_SIZE_VAL+INCREMENT))
		echo "K_TB_SIZE=$K_TB_SIZE_VAL" >> "report".log
		echo "N_TB_SIZE=$N_TB_SIZE_VAL" >> "report".log
		make clear
		#changes in source files K_TB_SIZE_VALUE and N_TB_SIZE_VALUE with actual values, and recompiles
		sed "s/K_TB_SIZE_VALUE/$K_TB_SIZE_VAL/g" NCpacket.h.template > NCpacket.h.tmp
		sed "s/N_TB_SIZE_VALUE/$N_TB_SIZE_VAL/g" NCpacket.h.tmp > NCpacket.h
		make all
		rm NCpacket.h
		rm NCpacket.h.tmp


		echo $1 >> "report".log

		for PER in 0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8
		do

			echo "PER=$PER" >> "report".log
			i=1
			while [ $i -le 10 ]
			do
				echo $i >> "report".log
				timeout 240 ./receiver audio.wav $PER >> "logrx".txt & # $1 is filename, $2 is PER, in background. 					Timeout of 240 s. Killed if timeout
				sleep 1
				timeout 240 ./sender localhost localhost $1 $PER >> "logtx".txt # $1 is filename
				#simulate $1 $2
				status=$? # $? is exit status of timeout command: 0 if no timeout, 124 otherwise
				echo "exit status of timeout: "$status >> "report".log
				if [ $status -ne 124 ]
					then
						i=$(( i+1 ))
				fi
			done 
		done
	done
done
