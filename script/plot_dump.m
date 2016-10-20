h = figure();
title('RX Signals');
set(h,'PaperUnits','centimeters');
set(h,'PaperOrientation','landscape');
set(h,'PaperType','A4');
set(h,'PaperPosition',[0,0,21.0,29.7]);

for i = 2:2
    data = dlmread(['/home/inets/source/phased-array/build/cal/rx_dump-' num2str(i) '.csv'], ';');
    plot(data(:,1), 'r');
    hold on;
    grid on;
    plot(data(:,3), 'g');
    plot(data(:,5), 'b');
    plot(data(:,7), 'm');
endfor

%print(h,'pdf','meas_results.pdf');
