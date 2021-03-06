#!/bin/bash
cd /home/olivotto/RemoteDirLTH/WSN/LT-net/script/receiver
arr_K=(2500 3500)
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
		if [ $K_index -le 1 ]
			then
			echo K_index=$K_index > K_index_file
			else
			K_index=0
			echo K_index=$K_index > K_index_file
			echo "end of operations" >> "report".log
			echo "end of operations" >>  "logrx".txt
		fi
	fi
fi
K=${arr_K[$K_index]}
increment=${arr_increment[$increment_index]}
N=$((K+increment))
echo $trial_index >> "report".log
../../compile.sh $K $increment
timeout 240 ../../receiver ../../outputfile 0 >> "logrx".txt
status=$?
echo "exit status of timeout: "$status >> "report".log
if [ $status -eq 124 ]
	then
		trial_index=$((trial_index-1))
		echo trial_index=$trial_index > trial_index_file
fi
