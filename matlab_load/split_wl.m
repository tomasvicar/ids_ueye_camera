clc; clear all; close all;

file_names = subdir('Sada_02/*m.avi');
file_names = {file_names(:).name};


for file_num = 3 : length(file_names)
    
    file_name = file_names{file_num};
    [video,fps] = readFFFMPEGvideo(file_name, 'gray8', 1, 1, false);
    
    data = readtable(replace(file_name, '.avi', '.txt'),'Delimiter',';');
    % data = readtable('../Gacr_02_020_002_dual_m_01_02_2023_14_52_42/Gacr_02_020_002_dual_m_01_02_2023_14_52_42.txt','Delimiter',';');
    % data = readtable('../Tom_test_01_02_2023_13_52_45/Tom_test_01_02_2023_13_52_45.txt','Delimiter',';');
%     ecg = data.Var3;
    triger = data.Var5;
%     plot(ecg)
    
    
    [~,frame_positions_time] = findpeaks( diff(triger),'MinPeakHeight',10000,'MinPeakDistance', (1000/40)*0.6 );
    
    plot(triger);
    hold on
    plot(frame_positions_time,32820*ones(1,length(frame_positions_time)),'*');
    hold off
    
    frame_positions_time(frame_positions_time < 500) = [];

    if size(video,4) ~= length(frame_positions_time)
        disp(['frames ' num2str(size(video,4))])
        disp(['trigers ' num2str(length(frame_positions_time))])
        error('nesedi')
    end
    
    
    wl1_video = video(:,:,1,1:2:end);
    wl2_video = video(:,:,1,2:2:end);
    



%     imshow4(wl1_video)
    % imshow4(video)

    writeFFFMPEGvideo([replace(file_name, '.avi', '') '_wl1.avi'], wl1_video, fps/2);

    writeFFFMPEGvideo([replace(file_name, '.avi', '') '_wl2.avi'], wl2_video, fps/2);

end