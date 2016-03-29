#!/bin/bash 


make clear
make all
rm *.txt

echo -e "sent\t\tinfile\t\ttime\t\tgoodput\t\tthroughput\tPER_estimate" >> "logtx".txt
echo -e "dectime\t\trxtime\t\ttotrxpck\tdroppck\t\trxpck\t\tuselesspck\t\tPER" >> "logrx".txt

echo $1

for PER in 0.6 0.7 0.8
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
