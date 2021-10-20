infiles=['calib_csv/sme_1_1.xdf_filt.csv'; 'calib_csv/sme_1_2.xdf_filt.csv'; 'calib_csv/sme_1_3.xdf_filt.csv'; 'calib_csv/sme_1_4.xdf_filt.csv'; 'calib_csv/sme_1_5.xdf_filt.csv';'calib_csv/sme_1_6.xdf_filt.csv';
    'calib_csv/sme_2_1.xdf_filt.csv'; 'calib_csv/sme_2_2.xdf_filt.csv'; 'calib_csv/sme_2_3.xdf_filt.csv'; 'calib_csv/sme_2_4.xdf_filt.csv'; 'calib_csv/sme_2_5.xdf_filt.csv'; 'calib_csv/sme_2_6.xdf_filt.csv';
    'calib_csv/sme_3_1.xdf_filt.csv'; 'calib_csv/sme_3_2.xdf_filt.csv'; 'calib_csv/sme_3_3.xdf_filt.csv'; 'calib_csv/sme_3_4.xdf_filt.csv'; 'calib_csv/sme_3_5.xdf_filt.csv'; 'calib_csv/sme_4_1.xdf_filt.csv';
    'calib_csv/sme_4_2.xdf_filt.csv'; 'calib_csv/sme_4_3.xdf_filt.csv'];
calibdata = load(infiles(1,:));
[M,T,B,A, iirstate]=asr_calibrate_simple(calibdata,250);
state = struct('M',M,'T',T,'B',B,'A',A,'cov',[],'carry',[],'iir',iirstate,'last_R',[],'last_trivial',true);
for i=2:size(infiles,1)
    indata=load(infiles(i,:));
    [outdata,~] = asr_process_simple(indata,250,state);  % apply the processing to the data and update the filter state (as in MATLAB's filter())
    fprintf("%ix%i -> %ix%i %.5f \n",size(indata,1),size(indata,2),size(outdata,1),size(outdata,2),outdata(1,1));
    
    writematrix(outdata,[infiles(i,1:end-4) '.out.csv']);
end
