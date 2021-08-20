function [] = test_asr_calibrate_simple()
%
% test_asr_calibrate_simple.m--
%
% Input arguments: 
%
% Output arguments: 
%
% Other m-files required:   
%
% Example usage:   
%
%
% Developed in Matlab 9.8.0.1359463 (R2020a) Update 1 on PCWIN64
% at University of Oldenburg.
% Sarah Blum (sarah.blum@uol.de), 2020-07-30 11:44
%-------------------------------------------------------------------------

% coder.load('file.mat')
% load test data
%data = csvread('calib_data.csv');
%save('data.mat', 'data');

S = coder.load('data.mat');
srate =250;
state_simple = asr_calibrate_simple(S.data,srate);


% back-to-back test (do simple asr and original asr yield the same results?)
% note: this asr_calibrate version is not rASR, but uses eig() instead of nonlinear_eig()

% %TODO provide same parameters here
% state_original = asr_calibrate(data,250);
% 
% isequal(state_simple, state_original)

[process_data_simple, proc_state_simple] = asr_process_simple(S.data, 250, state_simple);

[process_data_original, proc_state_original] = asr_process(S.data, 250, state_simple);

