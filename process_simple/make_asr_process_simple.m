% MAKE_ASR_PROCESS_SIMPLE   Generate static library asr_process_simple from
%  asr_process_simple.
% 
% Script generated from project 'asr_process_simple.prj' on 20-Aug-2021.
% 
% See also CODER, CODER.CONFIG, CODER.TYPEOF, CODEGEN.

%% Create configuration object of class 'coder.CodeConfig'.
cfg = coder.config('lib','ecoder',false);
cfg.GenerateReport = true;
cfg.ReportPotentialDifferences = false;
cfg.DynamicMemoryAllocationInterface = 'C++';
cfg.TargetLang = 'C++';
cfg.RowMajor = true;
cfg.EnableOpenMP = false;
cfg.GenCodeOnly = true;
cfg.TargetLangStandard = 'C++03 (ISO)';
cfg.HardwareImplementation.ProdHWDeviceType = 'Generic->Unspecified (assume 32-bit Generic)';
cfg.HardwareImplementation.TargetHWDeviceType = 'Generic->Unspecified (assume 32-bit Generic)';
cfg.HardwareImplementation.ProdIntDivRoundTo = 'Zero';
cfg.HardwareImplementation.TargetIntDivRoundTo = 'Zero';

%% Define argument types for entry-point 'asr_process_simple'.
ARGS = cell(1,1);
ARGS{1} = cell(3,1);
ARGS{1}{1} = coder.typeof(0,[Inf Inf],[1 1]);
ARGS{1}{2} = coder.typeof(0);
ARGS_1_3 = struct;
ARGS_1_3.M = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_1_3.T = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_1_3.B = coder.typeof(0,[1 Inf],[0 1]);
ARGS_1_3.A = coder.typeof(0,[1 Inf],[0 1]);
ARGS_1_3.cov = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_1_3.carry = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_1_3.iir = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_1_3.last_R = coder.typeof(0,[Inf Inf],[1 1]);
ARGS_1_3.last_trivial = coder.typeof(false);
ARGS{1}{3} = coder.typeof(ARGS_1_3);

%% Invoke MATLAB Coder.
codegen -config cfg asr_process_simple -args ARGS{1}

