#!/bin/bash 

rm *.txt
rm *.log
sshpass -p remotepassword ssh michele@192.168.2.107 'echo -e "sent\tinfile\ttime\tgoodput\tthroughput\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logtx".txt'
sshpass -p remotepassword ssh michele@192.168.2.105 'echo -e "dectime\trxtime\ttotrxpck\tdroppck\trxpck\tuselesspck\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE\tfaileddec" >> "logrx".txt'

for K_TB_SIZE_VAL in 2500 3500 4500 5500;
do
	for INCREMENT in 1000 1500 2000;
	do
		echo $K_TB_SIZE_VAL >> report.log
		echo $INCREMENT	 >> report.log
		sshpass -p remotepassword ssh michele@192.168.2.105 "./repos/wsn1516-finalproject/code/LT/compile.sh $K_TB_SIZE_VAL $INCREMENT"
		sshpass -p remotepassword ssh michele@192.168.2.107 "./wsn1516-finalproject/code/LT/compile.sh $K_TB_SIZE_VAL $INCREMENT"
			
		PER=0

		i=1
		while [ $i -le 90 ]
		do
			echo $i >> report.log
			sshpass -p remotepassword ssh michele@192.168.2.105 "echo $i >> ./repos/wsn1516-finalproject/code/LT/report.log"
			sshpass -p remotepassword ssh michele@192.168.2.107 "echo $i >> ./wsn1516-finalproject/code/LT/report.log"
			sshpass -p remotepassword ssh michele@192.168.2.105 "timeout 200 ./repos/wsn1516-finalproject/code/LT/receiver audio.wav $PER >> logrx.txt" & # $1 is filename, $2 is PER, in background. 					Timeout of 240 s. Killed if timeout
			sleep 3
			sshpass -p remotepassword ssh michele@192.168.2.107 "timeout 200 ./wsn1516-finalproject/code/LT/sender 192.168.2.107 192.168.2.105 ./wsn1516-finalproject/code/LT/audio.wav $PER >> logtx.txt" 	# $1 is filename

			status=$? # $? is exit status of timeout command: 0 if no timeout, 124 otherwise
			echo "exit status of timeout: "$status >> report.log
			if [ $status -ne 124 ]
				then
					i=$(( i+1 ))
			fi
		done 
		
	done
done

sshpass -p remotepassword scp michele@192.168.2.105:logrx.txt logrx.txt
sshpass -p remotepassword scp michele@192.168.2.107:logtx.txt logtx.txt