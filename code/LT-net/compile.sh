cd ../..
K_TB_SIZE_VAL=$1
INCREMENT=$2
N_TB_SIZE_VAL=$(($1+$2))
echo "K_TB_SIZE=$1" >> "report".log
echo "N_TB_SIZE=$N_TB_SIZE_VAL" >> "report".log
make clear;
sed "s/K_TB_SIZE_VALUE/$1/g" NCpacket.h.template > NCpacket.h.tmp
sed "s/N_TB_SIZE_VALUE/$N_TB_SIZE_VAL/g" NCpacket.h.tmp > NCpacket.h
make all
rm NCpacket.h
rm NCpacket.h.tmp
