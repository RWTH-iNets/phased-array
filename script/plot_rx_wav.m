data = dlmread('rx0.txt', ';');
h = figure();

plot(data(:,1) / 60.0, data(:,2), 'r-x');
hold on;
grid on;
plot(data(:,1) / 60.0, data(:,4), 'b-.o');

%set(h,'PaperUnits','inches');
%set(h,'PaperOrientation','landscape');
%set(h,'papertype','a4');
%set(h,'PaperPosition',[0,0,8.3,11.7]);
%print(h,'pdf','meas_results.pdf');
