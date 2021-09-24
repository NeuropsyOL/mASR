% The general calling convention of the method is:
% 
% 1)  Calibrate the parameters using the asr_calibrate function and some reasonably clean 
%     calibration data, e.g., resting-state EEG, as in the following code snippet. 
%     The recommended data length is ca. 1 minute or longer, the absolute minimum would be ca. 15 seconds. 
%     There are optional parameters as documented in the function, in particular a tunable threshold parameter 
%     governing the aggressiveness of the cleaning (although the defaults are sensible for testing purposes).

calibdata = randn(20,10000); % simulating 20-channel, 100-second random data at 100 Hz
[M,T,B,A, iirstate]=asr_calibrate_simple(calibdata,100);
state = struct('M',M,'T',T,'B',B,'A',A,'cov',[],'carry',[],'iir',iirstate,'last_R',[],'last_trivial',true);
while 1
   newchunk = randn(20,50);    % here using 0.5-second chunks; the chunk length is near-arbitrary and may vary, but transitions will be sharper for very short chunks.
   fprintf("-------------\n");
   size(newchunk)
   [cleanchunk,state] = asr_process_simple(newchunk,100,state);  % apply the processing to the data and update the filter state (as in MATLAB's filter())
   size(cleanchunk)
end
