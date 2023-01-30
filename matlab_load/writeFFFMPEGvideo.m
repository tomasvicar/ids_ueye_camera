function [] = writeFFFMPEGvideo(filename, data, fps)
    
    ffmpeg_path = 'ffmpeg.exe';
    codec = 'ffv1'; %losseles codec
    
%     filename = 'output.avi';
%     data = imread('peppers.png');
%     data = cat(4,data,data,data,data,data);
%     fps = 25;



    if (size(data,3) == 3) && isa(data,'uint8')
        format_in = 'rgb24';
        format_out = 'bgr0'; % superted by ffv1 codec "ffmpeg -h encoder=ffv1 -v quiet" for list of avaliable and "ffmpeg -pix_fmts" says it has all bytes
    elseif (size(data,3) == 1) && isa(data,'uint16')
        format_in = 'gray16';
        format_out = 'gray16le';
    elseif (size(data,3) == 1) && isa(data,'uint8')
        format_in = 'gray8';
        format_out = 'gray8';  
    else
        error('data type not implemented')
    end


    width =size(data,2);
    height = size(data,1);
    n_frames = size(data,4);
    
    
    cmd = [ffmpeg_path, ...
         ' -y' ...
         ' -video_size ', num2str(width),'x', num2str(height), ...
         ' -f rawvideo', ...
         ' -pixel_format ', format_in, ... 
         ' -framerate ', num2str(fps), ...
         ' -i pipe:', ...
         ' -vcodec ', codec...
         ' -pix_fmt ', format_out, ...
         ' ', filename];

%     ' -pix_fmt ', format_out, ...
%     -filter:v "format=yuv420p"
    
    p = java.lang.Runtime.getRuntime().exec(cmd);
    registerFfmpegProcess(cmd, p);
    
    p_stderr = java.io.DataInputStream(p.getErrorStream());
    p_stdin = java.io.DataOutputStream(p.getOutputStream());
    
    
    for frame_num = 1:n_frames
    
        I = data(:,:,:,frame_num);
    
        I = permute(I, ndims(I):-1:1);
        
        I = typecast(I(:), 'uint8');
    
        p_stdin.write(I);
        p_stdin.flush();
        
        p_stderr_bytes_available = p_stderr.available();

        if (p_stderr_bytes_available > 0)
            % Read all the available bytes from stderr stream.
            out = read_BufferedInputStream(p_stderr);
            disp(out);
        end
    end

    p_stderr.close();
    p_stderr = [];

    p_stdin.close();
    p_stdin = [];

    unregisterFfmpegProcess(cmd);

    pause(0.05)

    if ~isfile(filename)
        error('file was not created')
    end
    
    s=dir(filename);
    the_size=s.bytes;
    if the_size < 1000
        error('file was creted almost empty')
    end

end






function registerFfmpegProcess(cmd, p)
    % Manage book keeping of FFmpeg processes for preventing "zombie processes".
    % All FFmpeg commands (cmd) are stored in a "global" Map container.
    % The key-value pair M(cmd) = p, are stored in groot appdata (using setappdata command).
    % Before storing, the function checks if the M(cmd) already exists, and destroy the old process if it does.
    % Note: groot data is used because it's persistent, and not deleted by "clear all" command.
    % Note: The "book keeping" is useful when debugging the class.
    
    persistent is_first_time
    
    if isempty(is_first_time)
        if isappdata(groot, 'FfmpegProcessMap')
            M = getappdata(groot, 'FfmpegProcessMap');
            keySet = keys(M);
    
            % If first time, destroy all "zombie processes".
            for i = 1:length(keySet)
                key = keySet{i};
                proc = M(key);
                proc.destroy();
                warning('Zombie FFmpeg process is destroyed');
            end
    
            rmappdata(groot, 'FfmpegProcessMap');
        end
        is_first_time = false;
    end
    
    if isappdata(groot, 'FfmpegProcessMap')
        M = getappdata(groot, 'FfmpegProcessMap');
    
        if isKey(M, cmd)
            % If cmd is in the map - there is a "zombie processes" that needs to be destroyed.
            proc = M(cmd);
            proc.destroy();
            warning('Zombie FFmpeg process is destroyed');
            remove(M, cmd);
            M(cmd) = p; % Replace value of p (M(cmd) value is the new p).
        end
    else
        % Build new map if not exist.
        M = containers.Map(cmd, p);
    end
    
    % Store map M in groot.
    setappdata(groot, 'FfmpegProcessMap', M);
end



function unregisterFfmpegProcess(cmd)
        % Unregister a process - function should be executed after FFmpeg process ends.
        
        if ~isappdata(groot, 'FfmpegProcessMap')
            %warning('unregisterFfmpegProcess is executed but process is not registered');
            return
        end

        M = getappdata(groot, 'FfmpegProcessMap');

        if ~isKey(M, cmd)
            %warning('unregisterFfmpegProcess is executed but process is not registered');
            return
        end

        % Remove key from the map.
        remove(M, cmd);

        if isempty(M)
            % Map is empty - remove it from groot.
            rmappdata(groot, 'FfmpegProcessMap');
        else
            % Store updated map in groot.
            setappdata(groot, 'FfmpegProcessMap', M);        
        end
end





function out = read_BufferedInputStream(input_stream)
    % The simple JAVA syntax input_stream.read(byte[]) is not supported by MATLAB.
    % We need to use a reticulately complicated solution for reading byes from the stream...
    % The solution was posted by Benjamin Davis on 17 Feb 2020.

    num_available = input_stream.available();
    %short circuit out if none available
    %do not try to read, or it will block
    if num_available == 0
        out = '';
        return;
    end
    %save the reflection method object between calls
    persistent m_read
    if isempty(m_read)
        %build the reflection object
        %we are going to lookup BufferedInputStream.read(byte[], int, int)
        %using reflection API
        getMethod_args = javaArray('java.lang.Class',3);

        %this rather cryptic syntax is used for byte[]
        %since it is not possible to use java.lang.Byte[].TYPE
        %See:
        %https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.3.2
        byteArrayName = '[B';

        %these are the vararg list to getMethod
        getMethod_args(1) = java.lang.Class.forName(byteArrayName);
        getMethod_args(2) = java.lang.Integer.TYPE;
        getMethod_args(3) = java.lang.Integer.TYPE;

        %use reflection to get the method object
        m_read = input_stream.getClass().getMethod('read', getMethod_args);
    end
    
    %save the current buffer size and array of arguments to read()
    persistent buf_size read_args
    if isempty(buf_size)
        MIN_SIZE = 1024; %you could set this to whatever you want
        %make the buffer large enough to eat all available characters in the
        %stream
        buf_size = max(MIN_SIZE, num_available);

        %Note that this will fail:
        %   read_args = javaArray('java.lang.Object', 3)
        %   read_args(1) = zeros(1,buf_size,'int8')
        %So we instead use an ArrayList which will then be converted to
        %Object[] for the call to invoke.
        read_args = java.util.ArrayList();
        %this will become a byte[] in the ArrayList
        read_args.add(zeros(1,buf_size,'int8'));
        %arg for read start offset
        read_args.add(int32(0));
        %arg for read length
        read_args.add(int32(buf_size));
    end
    
    %Update the buffer to be larger if the input stream content grows
    if num_available > buf_size
        buf_size = num_available;
        read_args.set(0, zeros(1,buf_size,'int8'));
        read_args.set(2, int32(buf_size));
    end
    
    %Here is the magic, when read_args is unpacked, the byte[] reference in the first
    %element is passed
    n_read = m_read.invoke(input_stream, read_args.toArray());
    %so now we can go back to the original ArrayList and read out the contents
    out = char(read_args.get(0));
    out = out(:)'; %make row vector
    out = out(1:n_read); %trim to indicated size
end