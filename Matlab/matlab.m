%------------------------- 2020b  
%----------------------------------------------STARTUP 
%check COM number
%check baud rate

u = serialport("COM3", 115200);

%%---------------------------------------------- Set up the figure window
%time = now;
T = 0;
H = 0;
DST = 0;
%TVOC = 0;

% %--------------------------------------------- Set up tile window for multiple figures
t = tiledlayout(3,1);               % >2019b
                                    % (rows, columns)
                                                                     
ax1 = nexttile;                     %creates an axes object
                                    %places it into the next empty tile
                                    %in the current figure                                                                          
title('Temperature')
ylabel('F')
hold on                             %preserved after call of plot()

ax2 = nexttile;
title('Humidity')
ylabel('%')
hold on

ax3 = nexttile;
title('Soil Temperature')
ylabel('F')
hold on
% 
% ax4 = nexttile;
% title('TVOC')
% ylabel('ppb')
% hold on

%%------------------------- Set time span and interval for data collection
stopTime = '12/01 13:40';           %effectively non-stop



%%----------------------------------------------------------- Collect data
count = 1;
loops = 0;
%%-------------------------------------------disregard first few frames
%  for i= 1:20
%      readValue = readline(u);
%      pause(0.5);
%  end
while ~isequal(datestr(now,'mm/DD HH:MM'),stopTime)
        
    time(count) = datetime('now');
    %------------------------------------- 2020b - read received packet 
    %readValue = read(u, 200, "string");             %number of datagram = 1
    readValue = readline(u);

    %------------------------------------------------- extract strings
    
    %SAMPLE single message frame (sending all readings at once)
    %24.03T38.50RH400C0VO
    %extract before O = whole message w/o garbage at the end
    %extract before T = 24.03
    %extract b/w T and R = 38.50
    %...
   
    str_all = extractBetween(readValue, "Message: @","#");

    str_T = extractBefore(str_all, "T");
    str_H = extractBetween(str_all, "T", "H");
    str_DST = extractBetween(str_all, "H", "S");
    %str_TVOC = extractBetween(str_all, "C", "V");
    
    %-------------------------------------------------- convert to numbers
    try
        T(count) = str2double(str_T);
        H(count) = str2double(str_H);
        DST(count) = str2double(str_DST);
        if (T(count)~=0)
            try
                plot(ax1, time,T, '--or');             %auto-target current figure
                plot(ax2, time,H, '-k');
                plot(ax3, time,DST, '--b');
                count = count +1;
            catch
            end
        end
    catch
    end

    
    %TVOC(count) = str2double(str_TVOC);   
    
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
delete(u)