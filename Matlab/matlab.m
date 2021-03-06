%------------------------- 2020b  
%----------------------------------------------STARTUP 
%check COM number
%check baud rate

serialDevice = serialport("COM10", 115200);

% %--------------------------------------------- Set up tile window for multiple figures
t = tiledlayout(1,3);               % >2019b
                                    % (rows, columns)
                                    % 1 node per row
                                    % same reading -> same column
                                    % 6 readings -> 6 columns
                                                                     
% ax1 = nexttile;                     %creates an axes object
%                                     %places it into the next empty tile
%                                     %in the current figure                                                                          
% title('Temperature 1')
% ylabel('C')
% hold on                             %preserved after call of plot()
% ax2 = nexttile;
% title('Humidity 1')
% ylabel('%')
% hold on
% ax3 = nexttile;
% title('Soil Temperature 1')
% ylabel('C')
% hold on
% ax4 = nexttile;
% title('Altitude 1')
% ylabel('m')
% hold on
% ax5 = nexttile;
% title('Pressure 1')
% ylabel('psi')
% hold on
% ax6 = nexttile;
% title('Soil Moisture 1')
% ylabel('mm/mm')
% hold on
% ax7 = nexttile;                     %node 2
% title('Temperature 2')
% ylabel('C')
% hold on 
% ax8 = nexttile;
% title('Humidity 2')
% ylabel('%')
% hold on
% ax9 = nexttile;
% title('Soil Temperature 2')
% ylabel('C')
% hold on
% ax10 = nexttile;
% title('Altitude 2')
% ylabel('m')
% hold on
% ax11 = nexttile;
% title('Pressure 2')
% ylabel('psi')
% hold on
% ax12 = nexttile;
% title('Soil Moisture 2')
% ylabel('mm/mm')
% hold on
ax13 = nexttile;                    %node 3
title('Temperature 3')
ylabel('F')
hold on 
ax14 = nexttile;
title('Humidity 3')
ylabel('%')
hold on
% ax15 = nexttile;
% title('Soil Temperature 3')
% ylabel('C')
% hold on
ax16 = nexttile;
title('Altitude 3')
ylabel('m')
hold on
% ax17 = nexttile;
% title('Pressure 3')
% ylabel('psi')
% hold on
% ax18 = nexttile;
% title('Soil Moisture 3')
% ylabel('mm/mm')
% hold on

%%------------------------- Set time span and interval for data collection
stopTime = '12/01 13:40';           %effectively non-stop

%%----------------------------------------------------------- Collect data
count = 1;
count2 = 1;
count3 = 1;
loops = 0;
%%-------------------------------------------disregard first few frames
%  for i= 1:20
%      readValue = readline(u);
%      pause(0.5);
%  end
%%----------------------------------------------------------- pre-allocate space for array
% time = NaT(1, 100000);
% time2 = NaT(1, 100000);
time3 = NaT(1, 100000);
% T1 = zeros(1, 100000);
% H1 = zeros(1, 100000);
% S1 = zeros(1, 100000);
% A1 = zeros(1, 100000);
% P1 = zeros(1, 100000);
% M1 = zeros(1, 100000);
% T2 = zeros(1, 100000);
% H2 = zeros(1, 100000);
% S2 = zeros(1, 100000);
% A2 = zeros(1, 100000);
% P2 = zeros(1, 100000);
% M2 = zeros(1, 100000);
T3 = zeros(1, 100000);
H3 = zeros(1, 100000);
% S3 = zeros(1, 100000);
A3 = zeros(1, 100000);
% P3 = zeros(1, 100000);
% M3 = zeros(1, 100000);
plotGT = plot(ax13, time3, T3, '-r');
hold on
plotGH = plot(ax14, time3, H3, '-k');
plotGA = plot(ax16, time3, A3, '--b');


while ~isequal(datestr(now,'mm/DD HH:MM'),stopTime)
        
    %time(count) = datetime('now');
    %------------------------------------- 2020b - read received packet 
    %readValue = read(u, 200, "string");             %number of datagram = 1
    readValue = readline(serialDevice);
    %disp('************');

    %------------------------------------------------- extract strings
    %SAMPLE single message frame (sending all readings at once)
    %@ N1 72T 47H 7400S 19A 10150P 554M # (NO SPACE)
	%open -> @
	%ID -> N1, N2, N3,...
	%DHT22 - temp -> T
	%DHT22 - hum -> H
	%DS18B20 -> S
	%BMP180 - alt -> A
	%BMP180 - pres -> P
	%Moisture -> M
	%close -> #
    %STEPS
    %extract b/w @ and # = 38.50
    %extract b/f id
    %extract b/w id and T
    %...
    
   % str_node1 = extractBetween(readValue, "N1","#");
  %  str_node2 = extractBetween(readValue, "N2","#");
    str_node3 = extractBetween(readValue, "N3","#");
    %str_all = extractBetween(readValue, "Message: @","#");

%     str_T1 = extractBefore(str_node1, "T");                 %node 1
%     str_H1 = extractBetween(str_node1, "T", "H");
%     str_S1 = extractBetween(str_node1, "H", "S");
%     str_A1 = extractBetween(str_node1, "S", "A");
% %     str_P1 = extractBetween(str_node1, "A", "P");
% %     str_M1 = extractBetween(str_node1, "P", "M");
%     str_T2 = extractBefore(str_node2, "T");                 %node 2
%     str_H2 = extractBetween(str_node2, "T", "H");
%     str_S2 = extractBetween(str_node2, "H", "S");
%     str_A2 = extractBetween(str_node2, "S", "A");
% %     str_P2 = extractBetween(str_node2, "A", "P");
% %     str_M2 = extractBetween(str_node2, "P", "M");
    str_T3 = extractBefore(str_node3, "T");                 %node 3
    str_H3 = extractBetween(str_node3, "T", "H");
%    str_S3 = extractBetween(str_node3, "H", "S");
    str_A3 = extractBetween(str_node3, "S", "A");
%     str_P3 = extractBetween(str_node3, "A", "P");
%     str_M3 = extractBetween(str_node3, "P", "M");
%     disp('************');
    
    %-------------------------------------------------- convert to numbers
%     try                                                 %node 1
%         T1(count) = str2double(str_T1);                
%         H1(count) = str2double(str_H1);
%         S1(count) = str2double(str_S1);
%         A1(count) = str2double(str_A1);
% %         P1(count) = str2double(str_P1);
% %         M1(count) = str2double(str_M1);
%         if (T1(count)~=0)
%             try
%                 plot(ax1, time,T1, '--or');             
%                 plot(ax2, time,H1, '-k');
%                 plot(ax3, time,S1, '--b');
%                 plot(ax4, time,A1, '--b');
% %                 plot(ax5, time,P1, '--b');
% %                 plot(ax6, time,M1, '--b');
%                 count = count +1;
%             catch
%             end
%         end
%     catch
%     end
%     try                                                 %node 2
%          T2(count2) = str2double(str_T2);                     
%          H2(count2) = str2double(str_H2);
%          S2(count2) = str2double(str_S2);
%          A2(count2) = str2double(str_A2);
% %         P2(count2) = str2double(str_P2);
% %         M2(count2) = str2double(str_M2);
%          if (T2(count2)~=0)
%               try
%                  time2(count2) = datetime('now');
%                  count2 = count2 +1;
%                  plot(ax7, time2, T2, '--or');            
%                  plot(ax8, time2, H2, '-k');
%                  plot(ax9, time2, S2, '--b');
%                  plot(ax10, time2, A2, '--b');
% %                 plot(ax11, time2, P2, '--b');
% %                 plot(ax12, time2, M2, '--b');
%                  
%              catch
%              end
%          end
%     catch
%     end
     try                                                 %node 3
         T3(count3) = str2double(str_T3);                     
         H3(count3) = str2double(str_H3);
   %      S3(count3) = str2double(str_S3);
         A3(count3) = str2double(str_A3);
%          P2(count3) = str2double(str_P3);
%          M2(count3) = str2double(str_M3);
         if (T3(count3)~=0)
             try
                 time3(count3) = datetime('now');
                 count3 = count3 +1;
                 
%                  plot(ax13, time3, T3, '-r');            
%                  plot(ax14, time3, H3, '-k');
%   %               plot(ax15, time3, S3, '--b');
%                  plot(ax16, time3, A3, '--b');
% %                  plot(ax17, time3, P3, '--b');
% %                  plot(ax18, time3, M3, '--b');

                set(plotGT,'XData',time3,'YData',T3);
                set(plotGH,'XData',time3,'YData',H3);
                set(plotGA,'XData',time3,'YData',A3);
                 
             catch
             end
         end
    catch
    end
    
    %---------------------------------------------------------update plots
%     if (T(count)~=0)
%         try
%             plot(ax1, time,T, '--or');             %auto-target current figure
%             plot(ax2, time,H, '-k');               
%             plot(ax3, time,DST, '--b');          
%             count = count +1;
%         catch
%         end
%     end
    %plot(ax4, time,TVOC, '-m');           %auto-target current figure
    
    pause(0.1);
    %count = count +1;
    
    %--------------------------------------------- Specify no. of readings
%     loops = loops + 1;
%     
%     if loops >= 10          %loops = no. of readings
%         %-------------------------------------- write to file
%         maTrix = [T;RH;CO2eq;TVOC];
%         writematrix(maTrix,'data_1.csv')
%         break
%     end
end
%% Clean up the object
delete(serialDevice)