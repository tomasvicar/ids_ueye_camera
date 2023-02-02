clc; clear all; close all;

fudus_dir = 'Snímání 01_02_2023';
folder_names = dir('Gacr*');
folder_names = {folder_names(:).name};
fundus_names = dir([fudus_dir '/*.JPG']);
fundus_names = {fundus_names(:).name};
outptu_folder_name= '../Sada_02';



for folder_num = 23:length(folder_names)
    
    folder_name = folder_names{folder_num};
    
    pac_number = split(folder_name,'_');
    pac_number = str2num(pac_number{3});


    get1 = @(x) str2num(x{1});

    L_fundus_name = cellfun(@(x) pac_number == get1(split(x,'_')) && contains(x,'_L_'), fundus_names);
    L_fundus_name = fundus_names{L_fundus_name};
    L_fundus_name = [fudus_dir '\' L_fundus_name ];

    if ~(strcmp(folder_name,'Gacr_02_020_001_dual_m') || strcmp(folder_name,'Gacr_02_020_002_dual_m'))
        R_fundus_name = cellfun(@(x) pac_number == get1(split(x,'_')) && contains(x,'_P_'), fundus_names);
        R_fundus_name = fundus_names{R_fundus_name};
        R_fundus_name = [fudus_dir '\' R_fundus_name ];
    end


    video_name = subdir([folder_name '\*.avi']);
    video_name = video_name(1).name;

    signals_name = subdir([folder_name '\*.txt']);
    signals_name = signals_name(1).name;

    settigs_name = subdir([folder_name '\*.json']);
    settigs_name = settigs_name(1).name;

    goal_dir = [outptu_folder_name '/' folder_name];

    mkdir([goal_dir '/BiosignalAnalysis'])
    mkdir([goal_dir '/ImageAnalysis'])
    mkdir([goal_dir '/Registration'])
    mkdir([goal_dir '/VideoAnalysis'])

    copyfile(L_fundus_name, [goal_dir '/' replace(folder_name,'_dual_m','_L') '.JPG'])


    if ~(strcmp(folder_name,'Gacr_02_020_001_dual_m') || strcmp(folder_name,'Gacr_02_020_002_dual_m'))
        copyfile(R_fundus_name, [goal_dir '/' replace(folder_name,'_dual_m','_R') '.JPG'])
    end

    
    copyfile(video_name, [goal_dir '/' folder_name,'_dual_m' '.avi'])
    copyfile(signals_name, [goal_dir '/' folder_name,'_dual_m' '.txt'])
    copyfile(settigs_name, [goal_dir '/' folder_name,'_dual_m' '.json'])



end



