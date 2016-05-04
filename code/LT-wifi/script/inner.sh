#!/bin/bash
arr_K=(1000 2000 3000)
arr_increment=(500 1000 1500 2000)
source K_index_file
source increment_index_file
source trial_index_file
if [ $trial_index -le 10 ]
	then
	trial_index=$((trial_index+1))
	echo trial_index=$trial_index > trial_index_file
else
	trial_index=1
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
			K_index=0
			echo "end of operations"
		fi
	fi
fi
K=${arr_K[$K_index]}
increment=${arr_increment[$increment_index]}
echo -----indices >> output_ex.txt
echo $K_index >> output_ex.txt
echo $increment_index >> output_ex.txt
echo -----values >> output_ex.txt
echo $K >> output_ex.txt
echo $increment >> output_ex.txt
echo $trial_index >> output_ex.txt
