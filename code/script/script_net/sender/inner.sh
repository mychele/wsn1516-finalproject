#!/bin/bash

cd /home/michele/repos/wsn1516-finalproject/code/LT-net/script/sender

arr_K=(2500 3500 4500)
arr_increment=(1000 1500)
source K_index_file
source increment_index_file
source trial_index_file
if [ $trial_index -lt 10 ]
	then
	trial_index=$((trial_index+1))
	echo trial_index=$trial_index > trial_index_file
else
	trial_index=0
	echo trial_index=$trial_index > trial_index_file
	increment_index=$((increment_index+1))
	if [ $increment_index -le 1 ]
		then
		echo increment_index=$increment_index > increment_index_file
	else
		increment_index=0
		echo increment_index=$increment_index > increment_index_file
		K_index=$((K_index+1))
		if [ $K_index -le 2 ]
			then
			echo K_index=$K_index > K_index_file
			else
			K_index=0
			echo "end of operations" >> "report".log
			echo "end of operations" >>  "logtx".txt
		fi
	fi
fi
K=${arr_K[$K_index]}
increment=${arr_increment[$increment_index]}
N=$((K+increment))
echo $trial_index >> "report".log
../../compile.sh $K $increment
sleep 30
timeout 240 ../../sender 192.168.1.100 128.178.70.97 ../../$1 0 >> "logtx".txt
echo "exit status of timeout: "$? >> "report".log
if [ $? -eq 124 ]
then
	trial_index=$((trial_index-1))
	echo trial_index=$trial_index > trial_index_file
fi
