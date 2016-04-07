#!/bin/bash 



rm *.txt
echo -e "sent\tinfile\ttime\tgoodput\tthroughput\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logtx".txt
echo -e "dectime\trxtime\ttotrxpck\tdroppck\trxpck\tuselesspck\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logrx".txt

for K_TB_SIZE_VAL in 1000 1500 2500 3500 4500 5500;
do
	for INCREMENT in 1000 1250 1500 1750 2000;
	do
		N_TB_SIZE_VAL=$((K_TB_SIZE_VAL+INCREMENT))
		echo "K_TB_SIZE=$K_TB_SIZE_VAL"
		echo "N_TB_SIZE=$N_TB_SIZE_VAL"
		make clear
		sed "s/K_TB_SIZE_VALUE/$K_TB_SIZE_VAL/g" NCpacket.h.template > NCpacket.h.tmp
		sed "s/N_TB_SIZE_VALUE/$N_TB_SIZE_VAL/g" NCpacket.h.tmp > NCpacket.h
		make all
		rm NCpacket.h
		rm NCpacket.h.tmp


		echo $1

		for PER in 0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8
		do

			echo "PER=$PER"
			i=1
			while [ $i -le 10 ]
			do
				echo $i
				timeout 120 ./receiver audio.wav $PER >> "logrx".txt & # $1 is filename, $2 is PER, in background
				sleep 1
				timeout 120 ./sender localhost localhost $1 $PER >> "logtx".txt # $1 is filename
				#simulate $1 $2
				status=$?
				echo $status
				if [ $status -ne 124 ]
					then
						i=$(( i+1 ))
				fi
			done 
		done
	done
done
