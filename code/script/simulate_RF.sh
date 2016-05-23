#!/bin/bash 

rm *.txt
rm *.log
make clear
make all

echo $1

echo -e "sent\tinfile\ttime\tgoodput\tthroughput\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logtx".txt
echo -e "dectime\trxtime\ttotrxpck\tdroppck\trxpck\tuselesspck\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logrx".txt


for PER in 0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8
do
	echo "PER=$PER" >> "report".log
	for i in `seq 1 100`;
	do
		echo $i >> "report".log
		./receiver outputfile $PER >> "logrx".txt & # $1 is filename, $2 is PER, in background
		sleep 1
		./sender localhost localhost $1 $PER>> "logtx".txt # $1 is filename
		#simulate $1 $2
	done 
done
