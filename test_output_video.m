clc;clear all;close all;
file_name = "C:\Users\vicar\Desktop\ids_ueye_camera\ids_ueye_camera\sdk\samples\source\peak\csharp\simple_live_windows_forms\out\build\x64-Debug\output\bin\x86_64\Debug\test.avi";





v = VideoReader(file_name);
while hasFrame(v)
    frame = readFrame(v);
end