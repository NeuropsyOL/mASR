function [] = UT_asr_calibrate_simple()
%
% UT_asr_calibrate_simple.m--
%
% Unit test for generated C++ code from asr_calibrate
%
% Developed in Matlab 9.8.0.1359463 (R2020a) Update 1 on PCWIN64
% at University of Oldenburg.
% Sarah Blum (sarah.blum@uol.de), 2020-08-13 16:25
%-------------------------------------------------------------------------

Y = coder.ceval('asr_calibrate_simple.cpp')