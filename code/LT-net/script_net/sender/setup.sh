#!/bin/bash
rm *.txt
rm *.log
echo increment_index=0 > increment_index_file
echo K_index=0 > K_index_file
echo trial_index=0 > trial_index_file
echo -e "sent\tinfile\ttime\tgoodput\tthroughput\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logtx".txt

#(crontab -l 2>&1/home/michele/crontaberror.txt; echo "*/1 * * * * /home/michele/repos/wsn1516-finalproject/code/LT-net/script/sender/inner.sh") | crontab -
