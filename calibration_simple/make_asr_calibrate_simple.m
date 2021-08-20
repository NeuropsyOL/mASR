% ASR_CALIBRATE_SIMPLE_SCRIPT   Generate static library asr_calibrate_simple
%  from asr_calibrate_simple.
% 
% Script generated from project 'asr_calibrate_simple.prj' on 20-Aug-2021.
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

%% Define argument types for entry-point 'asr_calibrate_simple'.
ARGS = cell(1,1);
ARGS{1} = cell(2,1);
ARGS{1}{1} = coder.typeof(0,[Inf Inf],[1 1]);
ARGS{1}{2} = coder.typeof(0);

%% Invoke MATLAB Coder.
codegen -config cfg asr_calibrate_simple -args ARGS{1}

