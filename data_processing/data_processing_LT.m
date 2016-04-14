clear all;
color_matrix = colormap;
markers = {'+-','o-','*-','.-','x-','s-','d-','^-','v-','>-','<-','p-','h-'};
markersize = 6; % 3 is best for tikz
linewidth = 1.2;
close all hidden;
%% LT RX
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate PER	K_TB_SIZE N_TB_SIZE
%%dectime	rxtime	totrxpck droppck	rxpck	uselesspck PER_estimate PER	K_TB_SIZE N_TB_SIZE	faileddec
data_LT_rx=importdata('./data/data_LT_rx_3.txt',' ');
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

for j=1:length(Ks)
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(droppck_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
        title(['Number of packets dropped K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
	ylabel('Dropped packets [packet]');
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
    
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(rxtime_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
        title(['Time to receive the file K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
	ylabel('T [s]')
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
    
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(dectime_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
        title(['Time to decode the file K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
	ylabel('T [s]');
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
    
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(uselesspck_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize);
        title(['Number of packets correctly received but useless K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
	ylabel('Useless pck [packet]')
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
end
%% LT TX
%sent infile time goodput throughput	PER_estimate PER	K_TB_SIZE	N_TB_SIZE
data_LT_tx=importdata('./data/data_LT_tx_3.txt',' ');
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
            goodput_LT(i,j,k)=mean(data_PER_K_N_tx(:,4));
            throughput_LT(i,j,k)=mean(data_PER_K_N_tx(:,5));
            PER_estimate_LT(i,j,k)=mean(data_PER_K_N_tx(:,6));
        end
    end
end

for j=1:length(Ks)
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(goodput_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
        title(['Goodput K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
	ylabel('Goodput [Mbit/s]')
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
    
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(throughput_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
        title(['Throughput K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
	ylabel('Throughput [Mbit/s]')
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
    
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(PER_estimate_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
        title(['PER estimated @TX K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
	ylabel('PER estimate');
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
    
    figure();
    for k=1:length(increments)
        plot(PERs,squeeze(goodput_LT(:,j,k))./squeeze(throughput_LT(:,j,k)), markers{mod(k, numel(markers)) + 1}, 'Color', color_matrix(mod(k*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
        title(['Efficiency K= ', num2str(Ks(j))]);
        hold all;
    end
    grid on;
    xlabel('PER');
    ylabel('Efficiency');
    str=[cellstr(num2str((Ks(j)+increments), 'N=%-d'))']';
    for i=1:length(str(:,1))
        str_legend(i)=cellstr(strjoin(str(i,:)));
    end
    legend(str_legend');
end


for k = 1:length(increments)
	figure();
	hold all;
	for j = 1:length(Ks)
		plot(PERs, squeeze(goodput_LT(:,j,k))./squeeze(throughput_LT(:,j,k)), markers{mod(j, numel(markers)) + 1}, 'Color', color_matrix(mod(j*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
	end
	title(strcat('Efficiency for N - K = ', num2str(increments(k))));
	hold all;
	grid on;
	xlabel('PER');
	ylabel('Efficiency');
	str=[cellstr(num2str((Ks), 'K=%-d'))']';
	for i=1:length(str(:,1))
		str_legend(i)=cellstr(strjoin(str(i,:)));
	end
	legend(str_legend');
	
	figure();
	hold all;
	for j = 1:length(Ks)
		plot(PERs, squeeze(goodput_LT(:,j,k)), markers{mod(j, numel(markers)) + 1}, 'Color', color_matrix(mod(j*10, size(color_matrix, 1)) + 1,:), ...
				'LineWidth', linewidth, 'MarkerSize', markersize)
	end
	title(strcat('Goodput for N - K = ', num2str(increments(k))));
	hold all;
	grid on;
	xlabel('PER');
	ylabel('Goodput [Mbit/s]');
	str=[cellstr(num2str((Ks), 'K=%-d'))']';
	for i=1:length(str(:,1))
		str_legend(i)=cellstr(strjoin(str(i,:)));
	end
	legend(str_legend');
	
	
end
