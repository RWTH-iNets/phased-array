h = figure();
title('Phase Diff 4 Chan RX Rel to Chan 0');
set(h,'PaperUnits','centimeters');
set(h,'PaperOrientation','landscape');
set(h,'PaperType','A4');
set(h,'PaperPosition',[0,0,21.0,29.7]);

for i = 1:10
    data = dlmread(['/home/inets/source/phased-array/build/RxCal-' num2str(i) '.txt'], ';');
    plot(data(:,1) / 60.0, (data(:,5)/pi)*180.0, 'r-.o');
    hold on;
    grid on;
    plot(data(:,1) / 60.0, (data(:,6)/pi)*180.0, 'g--o');
    plot(data(:,1) / 60.0, (data(:,7)/pi)*180.0, 'b:*');
endfor

print(h,'pdf','meas_results.pdf');

%figure();

%plot(data(:,1) / 60.0, data(:,11), 'r:*');
%hold on;
%grid on;
%plot(data(:,1) / 60.0, data(:,12), 'g:*');
%plot(data(:,1) / 60.0, data(:,13), 'b:*');
%plot(data(:,1) / 60.0, data(:,14), 'y:*');

%plot(data2(:,1) / 60.0, (data2(:,2)/pi)*180.0, 'y');
%plot(data2(:,1) / 60.0, (data2(:,3)/pi)*180.0, 'c');
%plot(data2(:,1) / 60.0, (data2(:,4)/pi)*180.0, 'k');



