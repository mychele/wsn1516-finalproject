clear all;
close all hidden;
%% Random fountain RX
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_est PER
data_RF_rx=importdata('./data_RF_rx.txt',' ');
PERs=[0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8];
for i=1:length(PERs)
    data_PER=[];
    data_PER=data_RF_rx(find(data_RF_rx(:,8)==PERs(i)),:);
    dectime_RF(i)=mean(data_PER(:,1));
    rxtime_RF(i)=mean(data_PER(:,2));
    totrxpck_RF(i)=mean(data_PER(:,3));
    droppck_RF(i)=mean(data_PER(:,4));
    rxpck_RF(i)=mean(data_PER(:,5));
    uselesspck_RF(i)=mean(data_PER(:,6));
    PER_estimate_RF(i)=mean(data_PER(:,7));
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
data_RF_tx=importdata('./data_RF_tx.txt',' ');
PERs=[0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8];
for i=1:length(PERs)
    data_PER=[];
    data_PER=data_RF_tx(find(data_RF_tx(:,7)==PERs(i)),:);
    sent_RF(i)=mean(data_PER(:,1));
    time_RF(i)=mean(data_PER(:,3));
    goodput_RF(i)=mean(data_PER(:,4));
    throughput_RF(i)=mean(data_PER(:,5));
    PER_estimate_RF(i)=mean(data_PER(:,6));
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
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_est PER
data_LT_rx=importdata('./data_LT_rx.txt',' ');
PERs=[0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8];
for i=1:length(PERs)
    data_PER=[];
    data_PER=data_LT_rx(find(data_LT_rx(:,8)==PERs(i)),:);
    dectime_LT(i)=mean(data_PER(:,1));
    rxtime_LT(i)=mean(data_PER(:,2));
    totrxpck_LT(i)=mean(data_PER(:,3));
    droppck_LT(i)=mean(data_PER(:,4));
    rxpck_LT(i)=mean(data_PER(:,5));
    uselesspck_LT(i)=mean(data_PER(:,6));
    PER_estimate_LT(i)=mean(data_PER(:,7));
end

% figure();
% plot(PERs,droppck_LT);
% title('Number of packets dropped');
% hold all;
% grid on;
% figure();
% hold all;
% plot(PERs,rxtime_LT);
% title('Time to receive the file');
% grid on;
% figure();
% hold all;
% plot(PERs,dectime_LT);
% title('Time to decode the file');
% grid on;
% figure();
% hold all;
% plot(PERs,uselesspck_LT);
% title('Number of packets correctly received but useless');
% grid on;

%% LT TX
%sent infile time goodput throughput	PER_estimate PER
data_LT_tx=importdata('./data_LT_tx.txt',' ');
PERs=[0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8];
for i=1:length(PERs)
    data_PER=[];
    data_PER=data_LT_tx(find(data_LT_tx(:,7)==PERs(i)),:);
    sent_LT(i)=mean(data_PER(:,1));
    time_LT(i)=mean(data_PER(:,3));
    goodput_LT(i)=mean(data_PER(:,4));
    throughput_LT(i)=mean(data_PER(:,5));
    PER_estimate_LT(i)=mean(data_PER(:,6));
end

% figure();
% plot(PERs,sent_LT);
% title('Number of packets sent');
% hold all;
% grid on;
% figure();
% hold all;
% plot(PERs,time_LT);
% title('Time to send all packets');
% grid on;
% figure();
% hold all;
% plot(PERs,throughput_LT);
% grid on;
% hold all;
% plot(PERs,goodput_LT);
% grid on;
% title('Throughput and goodput');
% legend('throughput','goodput')

%% Comparison RF LT
figure();
plot(PERs,goodput_LT);
hold all;
plot(PERs,goodput_RF);
grid on
title('Goodput comparison')
legend('LT','RF')
figure();
hold all;
plot(PERs,dectime_LT);
plot(PERs,dectime_RF);
grid on
title('Decoding time comparison')
xlabel('PER')
legend('LT','RF')
figure();
hold all;
plot(PERs,throughput_LT);
plot(PERs,throughput_RF);
grid on
title('Throughput comparison')
xlabel('PER')
legend('LT','RF')
figure();
hold all;
plot(PERs,uselesspck_LT);
plot(PERs,uselesspck_RF);
grid on
title('Number of useless pcks comparison')
xlabel('PER')
legend('LT','RF')
figure();
hold all;
plot(PERs,rxtime_LT);
plot(PERs,rxtime_RF);
grid on
title('Time to receive comparison')
xlabel('PER')
legend('LT','RF')