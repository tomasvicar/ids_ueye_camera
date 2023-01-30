clc; clear all; close all;


[video,fps] = readFFFMPEGvideo('../test_roman_30_01_2023_10_44_58/test_roman_30_01_2023_10_44_58.avi', 'gray8', 1, 1, false);
% writeFFFMPEGvideo('xxx.avi', video, fps);


data = readtable('../test_roman_30_01_2023_10_44_58/test_roman_30_01_2023_10_44_58.txt','Delimiter',';');
ecg = data.Var3;
triger = data.Var5;


[~,frame_positions_time] = findpeaks( diff(triger),'MinPeakHeight',10000,'MinPeakDistance', (1000/25)*0.6 );

plot(triger);
hold on
plot(frame_positions_time,32820*ones(1,length(frame_positions_time)),'*');
hold off


disp(['frames ' num2str(size(video,4))])
disp(['trigers ' num2str(length(frame_positions_time))])


wl1_video = video(:,:,1,1:2:end);
wl2_video = video(:,:,1,2:2:end);

imshow4(wl1_video)
% imshow4(video)