#!/bin/bash
rm *.txt
rm *.log
echo increment_index=0 > increment_index_file
echo K_index=0 > K_index_file
echo trial_index=0 > trial_index_file
echo -e "sent\tinfile\ttime\tgoodput\tthroughput\tPER_estimate\tPER\tK_TB_SIZE\tN_TB_SIZE" >> "logtx".txt

(crontab -l 2>/dev/null; echo "*/5 * * * * /home/stefano/inner.sh $1") | crontab -
