The LT and RF folders contain respectively the code to run LT and RF network coding systems. Notice that, according to how the /etc/hosts file on the hosting machine is configured, when switching from localhost to a real network the receiver must be adapted (see ALTERNATIVE at line 140 in receiver.cpp file). RF code needs also an installation of NTL (http://www.shoup.net/ntl/) with correct paths in the Makefile.

The script folder contains 3 different scripts. 
- simulate_localhost.sh must be placed in LT folder, and launches a simulation campaign for different N, K and PERs. 
- simulate_RF.sh be placed in RF folder, and launches a simulation campaign for different PERs. 
- simulate_wifi.sh can be used by a host to launch sender and receiver on two remote machines. Paths to the folder with the code must be adapted to the sender/receiver host. It can be used whenever ssh access to remote hosts is available. The script compile.sh is needed in the folder with the code.
- the folder script_net contains two subfolders with different scripts for sender and receiver, for launching simulations on remote hosts where ssh access is not available (i.e. remote access is not available, but local access is). In both sender and receiver hosts a cron task is installed and runs every 5 minutes, launching sender and receiver respectively. Auxiliary text files are used to read and store the progress of the experiment. The script compile.sh is needed in the folder with the code. Paths must be adapted.

In order to launch manually an experiment in localhost, open two terminals and cd into the code folder. Then launch the receiver
./receiver filename PER
where PER is a value of PER that you want to simulate, and the sender on the other terminal
./sender localhost localhost filename PER
where PER is used just to logging purposes. 
Update the verb variable in sender and receiver to change the verbosity of the output.
In order to lauch a script simply place it in the correct directory and call ./scriptname filename. It produces as output a file named fileoutput.