function [] = UT_test_asr_calibrate()
%
% test_asr_calibrate.m--
% This is the Matlab counterpart to the Java tests of ASR_Calibrate.cpp. The ASR_calibrate function
% has been translated from Matlab to CPP using Matlab Coder, the resulting CPP codebase has been
% integrated in an Android library project, which creates an AAR file to include the ASR method as a
% library in other Android applications such as SCALA and so on.
% In the library project located at
% C:\Users\sarah\Documents\PhD\MATLAB_Coder\rASR_to_C\AndroidStudioProjects\ASR_Calibrate_CPP\,
% a set of unit tests is performed in Java that test the CPP generated code and (more important
% even) the Java wrapper using the JNI that allows the call of native functions from Java.
%
%
% Developed in Matlab 9.8.0.1359463 (R2020a) Update 1 on PCWIN64
% at University of Oldenburg.
% Sarah Blum (sarah.blum@uol.de), 2020-09-25 14:27
%-------------------------------------------------------------------------

% in case we are not here:
%cd('C:\Users\sarah\Documents\PhD\MATLAB_Coder\rASR_to_C\AndroidStudioProjects\MatlabDataAndScripts');

% load test data that is also used in Java project
data = csvread('calib_250hz_24ch.csv');
srate = 250;
state = asr_calibrate_simple(data,srate);
state.T(1);

% create small test data
csvwrite('smalldata.csv', data(1:3, 1:500))

srate = 250;
state_small = asr_calibrate_simple(data(1:3, 1:500),srate);
state_small.T(1)



% write the single contents of the state struct into different csv files
csvwrite('state_M.csv', state.M);
csvwrite('state_T.csv', state.T);


% even smaller testcase
tinydata = randn(2,10);
csvwrite('tiny.csv', tinydata)
% this can not run fully, set breakpoint at filter
state_tiny = asr_calibrate_simple(tinydata,srate);