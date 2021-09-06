% MAKE   Generate static library asr_calibrate_simple from asr_calibrate_simple,
%  asr_process_simple.
% 
% Script generated from project 'asr.prj' on 24-Aug-2021.
% 
% See also CODER, CODER.CONFIG, CODER.TYPEOF, CODEGEN.

%% Create configuration object of class 'coder.EmbeddedCodeConfig'.
cfg = coder.config('lib','ecoder',true);
cfg.TargetLang = 'C++';
cfg.GenerateReport = true;
cfg.ReportPotentialDifferences = false;
cfg.SaturateOnIntegerOverflow = false;
cfg.GenCodeOnly = true;
cfg.TargetLangStandard = 'C++11 (ISO)';
cfg.EnableOpenMP = false;
%% Define argument types for entry-point 'asr_calibrate_simple'.
ARGS = cell(2,1);
ARGS{1} = cell(2,1);
ARGS{1}{1} = coder.typeof(0,[Inf Inf],[1 1]);
ARGS{1}{2} = coder.typeof(0);

%% Define argument types for entry-point 'asr_process_simple'.
ARGS{2} = cell(3,1);
ARGS{2}{1} = coder.typeof(0,[Inf Inf],[1 1]);
ARGS{2}{2} = coder.typeof(0);
ARGS_2_3 = struct;
ARGS_2_3.M = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_2_3.T = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_2_3.B = coder.typeof(0,[1 Inf],[0 1]);
ARGS_2_3.A = coder.typeof(0,[1 Inf],[0 1]);
ARGS_2_3.cov = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_2_3.carry = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_2_3.iir = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_2_3.last_R = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_2_3.last_trivial = coder.typeof(false);
ARGS{2}{3} = coder.typeof(ARGS_2_3);
ARGS{2}{3} = coder.cstructname(ARGS{2}{3},'asr_state_t');

%% Invoke MATLAB Coder.
codegen -config cfg -o asr -package asr.zip asr_calibrate_simple -args ARGS{1} asr_process_simple -args ARGS{2}

