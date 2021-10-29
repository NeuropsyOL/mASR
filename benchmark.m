infiles=['calib_csv/sme_1_1.xdf_filt.csv'; 'calib_csv/sme_1_4.xdf_filt.csv'; 'calib_csv/sme_1_5.xdf_filt.csv';];
calibdata = load(infiles(1,:));
[M,T,B,A, iirstate]=asr_calibrate_simple(calibdata,100);
state = struct('M',M,'T',T,'B',B,'A',A,'cov',[],'carry',[],'iir',iirstate,'last_R',[],'last_trivial',true);
indata=load(infiles(2,:));
times=zeros(1,numel(1:size(indata,2)/50-1));
tic;
for i=1:size(indata,2)/50-1
    tic;
    chunk=indata(:,50*i:50*(i+1)-1);
    [outdata,state] = asr_process_simple(chunk,100,state);  % apply the processing to the data and update the filter state (as in MATLAB's filter())
    times(i)=toc;
end
mean(times)
[M,T,B,A, iirstate]=asr_calibrate_simple(calibdata,1000);
state = struct('M',M,'T',T,'B',B,'A',A,'cov',[],'carry',[],'iir',iirstate,'last_R',[],'last_trivial',true);
for i=1:size(indata,2)/500-1
    tic;
    chunk=indata(:,500*i:500*(i+1)-1);
    [outdata,state] = asr_process_simple(chunk,1000,state);  % apply the processing to the data and update the filter state (as in MATLAB's filter())
    times(i)=toc;
end
mean(times)