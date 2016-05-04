#!/bin/bash
echo increment_index=0 > increment_index_file
echo K_index=0 > K_index_file
echo trial_index=1 > trial_index_file
(crontab -l 2>/dev/null; echo "*/1 * * * * /home/stefano/inner.sh") | crontab -
