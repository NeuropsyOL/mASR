calibdata = load('mASR-testdata/calib.csv');
[M,T,B,A, iirstate]=asr_calibrate_simple(calibdata,500);
state = struct('M',M,'T',T,'B',B,'A',A,'cov',[],'carry',[],'iir',iirstate,'last_R',[],'last_trivial',true);
writematrix(M,'mASR-testdata/M.csv');
writematrix(T,'mASR-testdata/T.csv');
writematrix(B,'mASR-testdata/B.csv');
writematrix(A,'mASR-testdata/A.csv');
writematrix(iirstate,'mASR-testdata/iirstate.csv');
indata=load('mASR-testdata/proc.csv');
[outdata,~] = asr_process_simple(indata,500,state);  % apply the processing to the data and update the filter state (as in MATLAB's filter())
writematrix(outdata, 'mASR-testdata/proc.out.csv');
