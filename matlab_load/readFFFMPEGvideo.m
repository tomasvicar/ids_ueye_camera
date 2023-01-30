function [data,fps] = readFFFMPEGvideo(filename, format, bytes_per_pixel, color_channels, cast_to_uint16)

ffmpeg_path = 'ffmpeg.exe';
ffprobe_path = 'ffprobe.exe';

%     filename = 'Gacr_01_001_01_580_m_short.avi';
%     format = 'rgb24';
%     bytes_per_pixel = 3;
%     color_channels = 3;
%     cast_to_uint16 = false;

%     filename = 'Gacr_01_001_01_580_m_short_16bit.avi';
%     format = 'gray16';
%     bytes_per_pixel = 2;
%     color_channels = 1;
%     cast_to_uint16 = true;

    
    [metadata] = getMetaData(filename,ffprobe_path);


    cmd = [ffmpeg_path, ...
         ' -y', ...
         ' -i ', filename, ...
         ' -f rawvideo', ...
         ' -pix_fmt ', format, ...
         ' pipe:'];
    
    p = java.lang.Runtime.getRuntime().exec(cmd);
    registerFfmpegProcess(cmd, p);


    p_stderr = java.io.DataInputStream(p.getErrorStream());
    p_stdin = java.io.DataInputStream(p.getInputStream());

    n = metadata.Width*metadata.Height*metadata.NumberOfFrames * bytes_per_pixel;
    data = zeros(1,n,'int8');
    index = 1;
    empty_counter = 0;
    counter = 0;
    while index < n
        out = read_BufferedInputStream(p_stdin);
        data(index:index + length(out) - 1) = out;
        index = index + length(out);
        if isempty(out)
            empty_counter = empty_counter + 1;
        else
            empty_counter = 0;
        end
        if empty_counter > 100000
            error('reading problem')
        end
        counter = counter +1;
        if mod(counter,100) ==0
            p_stderr_bytes_available = p_stderr.available();
            if (p_stderr_bytes_available > 0)
                out = read_BufferedInputStream(p_stderr);
                disp(char(out));
            end
        end
    end



    if cast_to_uint16
        data = typecast(data, 'uint16');
    else
        data = typecast(data, 'uint8');
    end

    data = reshape(data,[color_channels, metadata.Width, metadata.Height, metadata.NumberOfFrames]);

    data = permute(data, [3,2,1,4]);
%     imshow(data(:,:,:,2))

    p_stderr.close();
    p_stderr = [];

    p_stdin.close();
    p_stdin = [];

    unregisterFfmpegProcess(cmd);
    fps = metadata.FrameRate;

end

function [metadata] = getMetaData(filename,ffprobe_path)
     % get metadata using FFPROBE
     out = evalc(['!' ffprobe_path ' -show_streams ' filename]);
     out(strfind(out, '=')) = [];
     % map FFPROBE output to their corresponding class variables
     keys = {'nb_frames', 'width', 'height', 'r_frame_rate'};
     keysField = {'NumberOfFrames', 'Width', 'Height', 'FrameRate'};
     metadata = struct();
     for idx = 1:length(keys)
        key = keys{idx};
        Index = strfind(out, key);
        metadata.(keysField{idx}) = sscanf(out(Index(1) + length(key):end), '%g', 1);
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
        MIN_SIZE = 32768; %you could set this to whatever you want
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

%     out = char(read_args.get(0));
    out = read_args.get(0);

    out = out(:)'; %make row vector
    out = out(1:n_read); %trim to indicated size
end