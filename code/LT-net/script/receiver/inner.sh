#!/bin/bash
arr_K=(1000 2000 3000)
arr_increment=(500 1000 1500 2000)
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
	if [ $increment_index -le 3 ]
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
			K_index=-1
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
timeout 240 ../../receiver ../../audio.wav 0 >> "logrx".txt
echo "exit status of timeout: "$status >> "report".log
if [ $status -ne 124 ]
then
	trial_index=$((trial_index-1))
	echo trial_index=$trial_index > trial_index_file
fi
