h = figure();
title('Phase Diff 4 Chan RX Rel to Chan 0');
set(h,'PaperUnits','centimeters');
set(h,'PaperOrientation','landscape');
set(h,'PaperType','A4');
set(h,'PaperPosition',[0,0,21.0,29.7]);

for i = 1:1
    data = dlmread(['/home/inets/source/phased-array/build/cal/corr-' num2str(i) '.csv'], ';');
    plot(data(:,1), 'r-.o');
    hold on;
    grid on;
%    plot(data(:,2), 'r--o');
    plot(data(:,2), 'g-.o');
%    plot(data(:,4), 'g--o');
    plot(data(:,3), 'b-.o');
%    plot(data(:,6), 'b--o');
endfor
