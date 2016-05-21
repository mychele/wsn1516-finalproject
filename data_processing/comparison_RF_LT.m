clear all;
close all hidden;
%% Random fountain RX
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate PER
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate	PER	K_TB_SIZE	N_TB_SIZE
data_RF_rx=importdata('./data/data_RF_loc_rx.txt',' ');
Nmc=100; %number of Montecarlo trials for each tuple of PER-K-N
PERs=unique(data_RF_rx(:,8));
Ks_RF=unique(data_RF_rx(:,9));
increments=unique(data_RF_rx(find(data_RF_rx(:,9)==Ks_RF(1)),10))-Ks_RF(1);

for i=1:length(PERs)
    for j=1:length(Ks_RF)
        for k=1:length(increments)
    data_PER_K_N_rx=[];
    data_PER_K_N_rx=data_RF_rx(find(data_RF_rx(:,8)==PERs(i) & data_RF_rx(:,9)==Ks_RF(j) & data_RF_rx(:,10)==(Ks_RF(j)+increments(k))),:);
    dectime_RF(i,j,k)=mean(data_PER_K_N_rx(:,1));
    rxtime_RF(i,j,k)=mean(data_PER_K_N_rx(:,2));
    totrxpck_RF(i,j,k)=mean(data_PER_K_N_rx(:,3));
    droppck_RF(i,j,k)=mean(data_PER_K_N_rx(:,4));
    rxpck_RF(i,j,k)=mean(data_PER_K_N_rx(:,5));
    uselesspck_RF(i,j,k)=mean(data_PER_K_N_rx(:,6));
    PER_estimate_RF(i,j,k)=mean(data_PER_K_N_rx(:,7));
        end
    end
end


% figure();
% plot(PERs,droppck_RF);
% title('Number of packets dropped');
% hold all;
% grid on;
% figure();
% hold all;
% plot(PERs,rxtime_RF);
% title('Time to receive the file');
% grid on;
% figure();
% hold all;
% plot(PERs,dectime_RF);
% title('Time to decode the file');
% grid on;
% figure();
% hold all;
% plot(PERs,uselesspck_RF);
% title('Number of packets correctly received but useless');
% grid on;

%% Random fountain TX
%sent infile time goodput throughput	PER_estimate PER
%sent infile time goodput throughput	PER_estimate PER	K_TB_SIZE	N_TB_SIZE
data_RF_tx=importdata('./data/data_RF_loc_tx.txt',' ');
PERs=unique(data_RF_tx(:,7));
Ks=unique(data_RF_tx(:,8));
increments=unique(data_RF_tx(find(data_RF_tx(:,8)==Ks(1)),9))-Ks(1);

for i=1:length(PERs)
    for j=1:length(Ks)
        for k=1:length(increments)
            data_PER_K_N_tx=[];
            data_PER_K_N_tx=data_RF_tx(find(data_RF_tx(:,7)==PERs(i) & data_RF_tx(:,8)==Ks(j) & data_RF_tx(:,9)==(Ks(j)+increments(k))),:);
            sent_RF(i,j,k)=mean(data_PER_K_N_tx(:,1));
            time_RF(i,j,k)=mean(data_PER_K_N_tx(:,3));
            efficiency_RF(i,j,k)=mean(data_PER_K_N_tx(:,4)./data_PER_K_N_tx(:,5));
			efficiency_RF_std(i,j,k)=std(data_PER_K_N_tx(:,4)./data_PER_K_N_tx(:,5));
            goodput_RF(i,j,k)=mean(data_PER_K_N_tx(:,4));
			goodput_RF_std(i,j,k)=std(data_PER_K_N_tx(:,4));
            throughput_RF(i,j,k)=mean(data_PER_K_N_tx(:,5));
			throughput_RF_std(i,j,k)=std(data_PER_K_N_tx(:,5));
            PER_estimate_RF(i,j,k)=mean(data_PER_K_N_tx(:,6));
			num_samples_tx_RF(i,j,k) = length(data_PER_K_N_tx);	
        end
    end
end

% figure();
% plot(PERs,sent_RF);
% title('Number of packets sent');
% hold all;
% grid on;
% figure();
% hold all;
% plot(PERs,time_RF);
% title('Time to send all packets');
% grid on;
% figure();
% hold all;
% plot(PERs,throughput_RF);
% grid on;
% hold all;
% plot(PERs,goodput_RF);
% grid on;
% title('Throughput and goodput');
% legend('throughput','goodput')

%% LT RX
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate PER	K_TB_SIZE N_TB_SIZE
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate PER	K_TB_SIZE N_TB_SIZE	faileddec
data_LT_rx=importdata('./data/data_LT_rx_loc_1ms_PER_mode_0.txt',' ');
Nmc=100; %number of Montecarlo trials for each tuple of PER-K-N
PERs=unique(data_LT_rx(:,8));
Ks=unique(data_LT_rx(:,9));
increments=unique(data_LT_rx(find(data_LT_rx(:,9)==Ks(1)),10))-Ks(1);
for i=1:length(PERs)
    for j=1:length(Ks)
        for k=1:length(increments)
    data_PER_K_N_rx=[];
    data_PER_K_N_rx=data_LT_rx(find(data_LT_rx(:,8)==PERs(i) & data_LT_rx(:,9)==Ks(j) & data_LT_rx(:,10)==(Ks(j)+increments(k))),:);
    dectime_LT(i,j,k)=mean(data_PER_K_N_rx(:,1));
    rxtime_LT(i,j,k)=mean(data_PER_K_N_rx(:,2));
    totrxpck_LT(i,j,k)=mean(data_PER_K_N_rx(:,3));
    droppck_LT(i,j,k)=mean(data_PER_K_N_rx(:,4));
    rxpck_LT(i,j,k)=mean(data_PER_K_N_rx(:,5));
    uselesspck_LT(i,j,k)=mean(data_PER_K_N_rx(:,6));
    PER_estimate_LT(i,j,k)=mean(data_PER_K_N_rx(:,7));
        end
    end
end
%% LT TX
%sent infile time goodput throughput	PER_estimate PER	K_TB_SIZE	N_TB_SIZE
data_LT_tx=importdata('./data/data_LT_tx_loc_1ms_PER_mode_0.txt',' ');
PERs=unique(data_LT_tx(:,7));
Ks=unique(data_LT_tx(:,8));
increments=unique(data_LT_tx(find(data_LT_tx(:,8)==Ks(1)),9))-Ks(1);

for i=1:length(PERs)
    for j=1:length(Ks)
        for k=1:length(increments)
            data_PER_K_N_tx=[];
            data_PER_K_N_tx=data_LT_tx(find(data_LT_tx(:,7)==PERs(i) & data_LT_tx(:,8)==Ks(j) & data_LT_tx(:,9)==(Ks(j)+increments(k))),:);
            sent_LT(i,j,k)=mean(data_PER_K_N_tx(:,1));
            time_LT(i,j,k)=mean(data_PER_K_N_tx(:,3));
			efficiency_LT(i,j,k)=mean(data_PER_K_N_tx(:,4)./data_PER_K_N_tx(:,5));
			efficiency_LT_std(i,j,k)=std(data_PER_K_N_tx(:,4)./data_PER_K_N_tx(:,5));
            goodput_LT(i,j,k)=mean(data_PER_K_N_tx(:,4));
			goodput_LT_std(i,j,k)=std(data_PER_K_N_tx(:,4));
            throughput_LT(i,j,k)=mean(data_PER_K_N_tx(:,5));
			throughput_LT_std(i,j,k)=std(data_PER_K_N_tx(:,5));
            PER_estimate_LT(i,j,k)=mean(data_PER_K_N_tx(:,6));
			num_samples_tx_LT(i,j,k) = length(data_PER_K_N_tx);	
        end
    end
end

%% Decoding time for LT
dec_data = importdata('./data/data_LT_rx_loc_100ms.txt',' ');
PERs=unique(dec_data(:,8));
Ks=unique(dec_data(:,9));
increments=unique(dec_data(find(dec_data(:,9)==Ks(1)),10))-Ks(1);
for i=1:length(PERs)
    for j=1:length(Ks)
        for k=1:length(increments)
			data_PER_K_N_rx=[];
			data_PER_K_N_rx=dec_data(find(dec_data(:,8)==PERs(i) & dec_data(:,9)==Ks(j) & dec_data(:,10)==(Ks(j)+increments(k))),:);
			decoding_time_LT(i,j,k) = mean(data_PER_K_N_rx(:,11));
			decoding_time_LT_std(i,j,k) = std(data_PER_K_N_rx(:,11));
			num_samples_rx(i,j,k) = length(data_PER_K_N_rx);
        end
    end
end
%% Comparison RF LT

linewidth = 1.5;

Ks_LT_index=4;
increment_LT_index=1;

figure();
errorbar(PERs, squeeze(goodput_LT(:,Ks_LT_index,increment_LT_index)), ...
	1.96*squeeze(goodput_LT_std(:,Ks_LT_index,increment_LT_index))./sqrt(squeeze(num_samples_tx_LT(:,Ks_LT_index, increment_LT_index))), '-x', 'LineWidth', linewidth);
hold all;
errorbar(PERs, squeeze(goodput_RF(:,1,1)), ...
	1.96*squeeze(goodput_RF_std(:,1,1))./sqrt(squeeze(num_samples_tx_RF(:,1, 1))), '-o', 'LineWidth', linewidth);
grid on
title('Goodput comparison')
xlabel('PER')
ylabel('Goodput [Mbit/s]')
legend('LT','RF')

figure();
hold all;
plot(PERs,squeeze(dectime_LT(:,Ks_LT_index,increment_LT_index)), '-x', 'LineWidth', linewidth);
plot(PERs,squeeze(dectime_RF(:,1,1)), '-o', 'LineWidth', linewidth);
grid on
title('Decoding time comparison')
xlabel('PER')
ylabel('T [s]')
legend('LT','RF')


figure();
hold all;
figure();
errorbar(PERs, squeeze(efficiency_LT(:,Ks_LT_index,increment_LT_index)), ...
	1.96*squeeze(efficiency_LT_std(:,Ks_LT_index,increment_LT_index))./sqrt(squeeze(num_samples_tx_LT(:,Ks_LT_index, increment_LT_index))), '-x', 'LineWidth', linewidth);
hold all;
errorbar(PERs, squeeze(efficiency_RF(:,1,1)), ...
	1.96*squeeze(efficiency_RF_std(:,1,1))./sqrt(squeeze(num_samples_tx_RF(:,1, 1))), '-o', 'LineWidth', linewidth);
grid on
grid on
title('Efficiency comparison')
xlabel('PER')
ylabel('Efficiency')
legend('LT','RF')

mean_dec_time_pp = decoding_time_LT(1, Ks_LT_index, increment_LT_index)/Ks(Ks_LT_index);
mean_dec_time_pp_RF = dectime_RF(1, 1, 1)/Ks_RF(1);