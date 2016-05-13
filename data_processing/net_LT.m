clear all;
color_matrix = colormap;
markers = {'+-','o-','*-','.-','x-','s-','d-','^-','v-','>-','<-','p-','h-'};
markersize = 6; % 3 is best for tikz
linewidth = 1.2;
close all hidden;
%% LT RX
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate PER	K_TB_SIZE N_TB_SIZE
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate PER	K_TB_SIZE N_TB_SIZE	faileddec
data_LT_rx=importdata('./data/data_LT_net_rx.txt',' ');
PERs=unique(data_LT_rx(:,8));
Ks=unique(data_LT_rx(:,9));
increments=unique(data_LT_rx(find(data_LT_rx(:,9)==Ks(1)),10))-Ks(1);
for i=1:length(PERs)
    for j=1:length(Ks)
        for k=1:length(increments)
			data_PER_K_N_rx=[];
			data_PER_K_N_rx=data_LT_rx(find(data_LT_rx(:,8)==PERs(i) & data_LT_rx(:,9)==Ks(j) & data_LT_rx(:,10)==(Ks(j)+increments(k))),:);
			dectime_LT(i,j,k)=mean(data_PER_K_N_rx(:,1));
			dectime_LT_std(i,j,k)=std(data_PER_K_N_rx(:,1));
			rxtime_LT(i,j,k)=mean(data_PER_K_N_rx(:,2));
			rxtime_LT_std(i,j,k)=std(data_PER_K_N_rx(:,2));
			totrxpck_LT(i,j,k)=mean(data_PER_K_N_rx(:,3));
			totrxpck_LT_std(i,j,k)=std(data_PER_K_N_rx(:,3));
			droppck_LT(i,j,k)=mean(data_PER_K_N_rx(:,4));
			droppck_std(i,j,k)=std(data_PER_K_N_rx(:,4));
			rxpck_LT(i,j,k)=mean(data_PER_K_N_rx(:,5));
			rxpck_LT_std(i,j,k)=std(data_PER_K_N_rx(:,5));
			uselesspck_LT(i,j,k)=mean(data_PER_K_N_rx(:,6));
			uselesspck_LT_std(i,j,k)=std(data_PER_K_N_rx(:,6));
			PER_estimate_LT(i,j,k)=mean(data_PER_K_N_rx(:,7));
			PER_estimate_LT_std(i,j,k)=std(data_PER_K_N_rx(:,7));
			num_samples_rx(i,j,k) = length(data_PER_K_N_rx);
		end
    end
end
 
%% LT TX
%sent infile time goodput throughput	PER_estimate PER	K_TB_SIZE	N_TB_SIZE
data_LT_tx=importdata('./data/data_LT_net_tx.txt',' ');
PERs=unique(data_LT_tx(:,7));
Ks=unique(data_LT_tx(:,8));
increments=unique(data_LT_tx(find(data_LT_tx(:,8)==Ks(1)),9))-Ks(1);

for i=1:length(PERs)
    for j=1:length(Ks)
        for k=1:length(increments)
            data_PER_K_N_tx=[];
            data_PER_K_N_tx=data_LT_tx(find(data_LT_tx(:,7)==PERs(i) & data_LT_tx(:,8)==Ks(j) & data_LT_tx(:,9)==(Ks(j)+increments(k))),:);
            sent_LT(i,j,k)=mean(data_PER_K_N_tx(:,1));
            sent_LT_std(i,j,k)=std(data_PER_K_N_tx(:,1));
			time_LT(i,j,k)=mean(data_PER_K_N_tx(:,3));
			time_LT_std(i,j,k)=std(data_PER_K_N_tx(:,3));
			efficiency_LT(i,j,k)=mean(data_PER_K_N_tx(:,4)./data_PER_K_N_tx(:,5));
			efficiency_LT_std(i,j,k)=std(data_PER_K_N_tx(:,4)./data_PER_K_N_tx(:,5));
			goodput_LT(i,j,k)=mean(data_PER_K_N_tx(:,4));
			goodput_LT_std(i,j,k)=std(data_PER_K_N_tx(:,4));
			throughput_LT(i,j,k)=mean(data_PER_K_N_tx(:,5));
			throughput_LT_std(i,j,k)=std(data_PER_K_N_tx(:,5));
			PER_estimate_LT(i,j,k)=mean(data_PER_K_N_tx(:,6));
			PER_estimate_LT_std(i,j,k)=std(data_PER_K_N_tx(:,6));
			num_samples_tx(i,j,k) = length(data_PER_K_N_tx);
		end
    end
end

figure()
title('Goodput on Wi-Fi connection');
for j=1:length(Ks)
	errorbar(increments, squeeze(goodput_LT(:,j,:)), 1.96*squeeze(goodput_LT_std(:,j,:))./sqrt(squeeze(num_samples_tx(:,j, :))), markers{mod(j, numel(markers)) + 1}, 'Color', color_matrix(mod(j*10, size(color_matrix, 1)) + 1,:), ...
		'LineWidth', linewidth, 'MarkerSize', markersize)
	hold all;	
end
xlabel('N-K');
ylabel('Mbit/s');
str=[cellstr(num2str((Ks), 'K=%-d'))']';
for i=1:length(str(:,1))
	str_legend(i)=cellstr(strjoin(str(i,:)));
end
legend(str_legend');
grid on

figure()
title('Goodput on Wi-Fi connection');
for j=1:length(Ks)
	errorbar(increments, squeeze(throughput_LT(:,j,:)), 1.96*squeeze(throughput_LT_std(:,j,:))./sqrt(squeeze(num_samples_tx(:,j, :))), markers{mod(j, numel(markers)) + 1}, 'Color', color_matrix(mod(j*10, size(color_matrix, 1)) + 1,:), ...
		'LineWidth', linewidth, 'MarkerSize', markersize)
	hold all;	
end
xlabel('N-K');
ylabel('Mbit/s');
str=[cellstr(num2str((Ks), 'K=%-d'))']';
for i=1:length(str(:,1))
	str_legend(i)=cellstr(strjoin(str(i,:)));
end
legend(str_legend');
grid on

figure()
title('Efficiency on Wi-Fi connection');
for j=1:length(Ks)
	errorbar(increments, squeeze(efficiency_LT(:,j,:)), 1.96*squeeze(efficiency_LT_std(:,j,:))./sqrt(squeeze(num_samples_tx(:,j, :))), markers{mod(j, numel(markers)) + 1}, 'Color', color_matrix(mod(j*10, size(color_matrix, 1)) + 1,:), ...
		'LineWidth', linewidth, 'MarkerSize', markersize)
	hold all;	
end
xlabel('N-K');
%ylabel('Mbit/s');
str=[cellstr(num2str((Ks), 'K=%-d'))']';
for i=1:length(str(:,1))
	str_legend(i)=cellstr(strjoin(str(i,:)));
end
legend(str_legend');
grid on

