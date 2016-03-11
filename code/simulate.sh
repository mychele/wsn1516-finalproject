#!/bin/bash 


make clear
make all

echo $1
echo $2
echo -e "sent\t\tinfile\t\ttime\t\tgoodput\t\tthroughput\tPER_estimate" >> "logtx$2".txt
echo -e "dectime\t\trxtime\t\ttotrxpck\tdroppck\t\trxpck\t\tuselesspck\t\tPER" >> "logrx$2".txt

for i in `seq 1 2`;
do
    echo $i
    ./receiver audio.wav $2 >> "logrx$2".txt & # $1 is filename, $2 is PER, in background
   	sleep 1
   	./sender localhost localhost $1 >> "logtx$2".txt # $1 is filename
    #simulate $1 $2
done 
