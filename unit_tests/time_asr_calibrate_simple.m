function [] = time_asr_calibrate_simple()

S = coder.load('data.mat');
srate =100;
[M, T, B, A, iirstate] = asr_calibrate_simple(S.data,srate);
state_simple = struct('M',M,'T',T,'B',B,'A',A,'cov',[],'carry',[],'iir',iirstate,'last_R',[],'last_trivial',true);
dt=[];
size(S.data)
S.data=S.data(:,1:150)
for i=1:10
    tic;
    [process_data_simple, proc_state_simple] = asr_process_simple(S.data, 250, state_simple);
    t1=toc;
    dt=[dt t1];
end
dt=[];
for i=1:10
    tic;
    [process_data_simple, proc_state_simple] = asr_process(S.data, 250, state_simple);
    t1=toc;
    dt=[dt t1];
end