function state = asr_calibrate_simple(X,srate)
%
% asr_calibrate_simple.m--
%
% A simplified version of asr_calibrate.m (part of clean_rawdata toolbox) in terms of input
% parameters and used functions.
% This function is the basis for an automatic translation to C++ using Matlab Coder.
% 
%
%
% Developed in Matlab 9.8.0.1359463 (R2020a) Update 1 on PCWIN64
% at University of Oldenburg.
% Sarah Blum (sarah.blum@uol.de), 2020-07-30 11:26
%-------------------------------------------------------------------------

%% define and set variables
[C,S] = size(X);
cutoff = 3;
blocksize = 10;
[B,A] = deal([1.7587013141770287 -4.3267624394458641  5.7999880031015953 ...
    -6.2396625463547508  5.3768079046882207 -3.7938218893374835 ...
    2.1649108095226470 -0.8591392569863763  0.2569361125627988],...
    [1.0000000000000000 -1.7008039639301735  1.9232830391058724 ...
    -2.0826929726929797  1.5982638742557307 -1.0735854183930011 ...
    0.5679719225652651 -0.1886181499768189  0.0572954115997261]);
window_len = 0.1;
window_overlap = 0.5;
max_dropout_fraction = 0.1;
min_clean_fraction = 0.3;
X(~isfinite(X(:))) = 0;
mu = zeros(1,C);
sig = zeros(1,C);

% apply the signal shaping filter and initialize the IIR filter state
[X,iirstate] = filter(B,A,double(X),[],2)
X = X';

% calculate the sample covariance matrices U (averaged in blocks of blocksize successive samples)
%U = zeros(length(1:blocksize:S),C*C);
% U1 = zeros(length(1:blocksize:S),C*C);
% for k=1:blocksize
%     range = min(S,k:blocksize:(S+k-1));
%     %U = U + reshape(bsxfun(@times,reshape(X(range,:),[],1,C),reshape(X(range,:),[],C,1)),size(U));
%     U1 = U1 + reshape(reshape(X(range,:),[],1,C) .* reshape(X(range,:),[],C,1), size(U1));
% end

SCM = (1/S) * (X' * X);

% get the mixing matrix M
%M = sqrtm(real(reshape(block_geometric_median(U1/blocksize),C,C)));
M = sqrtm(real(SCM));

% window length for calculating thresholds
N = round(window_len*srate);

% get the threshold matrix T
%[V,D] = rasr_nonlinear_eigenspace_simple(M, C);
[V,D] = eig(M);
[D, order] = sort(reshape(diag(D),1,C));
V = V(:,order); 
X = abs(X*V);

% for c = C:-1:1
%     % compute RMS amplitude for each window...
%     rms = X(:,c).^2;
%     %rms = sqrt(sum(rms(bsxfun(@plus,round(1:N*(1-window_overlap):S-N),(0:N-1)')))/N);
%     rms = sqrt(sum(rms(round(1:N*(1-window_overlap):S-N) + (0:N-1)'))/N);
%     % fit a distribution to the clean part
%     [mu(c),sig(c)] = fit_eeg_distribution(rms,min_clean_fraction,max_dropout_fraction);
% end


% note to myself: matrix automatically expands parts of the multiplications here, so that the
% dimensions of the vectors and matrices magically match. For the code translation, we explicitly
% compute the index range manually, avoiding the automatic expansion.
myrange = round(1:N*(1-window_overlap):S-N);% + (0:N-1)';
matrix_range = zeros(N, length(myrange));
% do the automatic expansion manually
for i = 1: length(myrange)
    one_column = myrange(i) + (0:N-1);
    matrix_range(:,i) = one_column;
end

for c = C:-1:1
    % compute RMS amplitude for each window...
    rms = X(:,c).^2;
    rms_chan = sqrt(sum(rms(matrix_range))/N);
    % fit a distribution to the clean part
    [mu(c),sig(c)] = fit_eeg_distribution(rms_chan,min_clean_fraction,max_dropout_fraction);
end

T = diag(mu + cutoff*sig)*V';

% initialize the remaining filter state
state = struct('M',M,'T',T,'B',B,'A',A,'cov',[],'carry',[],'iir',iirstate,'last_R',[],'last_trivial',true);

sprintf('%f',T(1,1))



function [mu,sig,alpha,beta] = fit_eeg_distribution(X,min_clean_fraction,max_dropout_fraction,quants,step_sizes,beta)
% Estimate the mean and standard deviation of clean EEG from contaminated data.
% [Mu,Sigma,Alpha,Beta] = fit_eeg_distribution(X,MinCleanFraction,MaxDropoutFraction,FitQuantiles,StepSizes,ShapeRange)
%
% This function estimates the mean and standard deviation of clean EEG from a sample of amplitude
% values (that have preferably been computed over short windows) that may include a large fraction
% of contaminated samples. The clean EEG is assumed to represent a generalized Gaussian component in
% a mixture with near-arbitrary artifact components. By default, at least 25% (MinCleanFraction) of
% the data must be clean EEG, and the rest can be contaminated. No more than 10%
% (MaxDropoutFraction) of the data is allowed to come from contaminations that cause lower-than-EEG
% amplitudes (e.g., sensor unplugged). There are no restrictions on artifacts causing
% larger-than-EEG amplitudes, i.e., virtually anything is handled (with the exception of a very
% unlikely type of distribution that combines with the clean EEG samples into a larger symmetric
% generalized Gaussian peak and thereby "fools" the estimator). The default parameters should be
% fine for a wide range of settings but may be adapted to accomodate special circumstances.
%
% The method works by fitting a truncated generalized Gaussian whose parameters are constrained by
% MinCleanFraction, MaxDropoutFraction, FitQuantiles, and ShapeRange. The alpha and beta parameters
% of the gen. Gaussian are also returned. The fit is performed by a grid search that always finds a
% close-to-optimal solution if the above assumptions are fulfilled.
%
% In:
%   X : vector of amplitude values of EEG, possible containing artifacts
%       (coming from single samples or windowed averages)
%
%   MinCleanFraction : Minimum fraction of values in X that needs to be clean
%                      (default: 0.25)
%
%   MaxDropoutFraction : Maximum fraction of values in X that can be subject to
%                        signal dropouts (e.g., sensor unplugged) (default: 0.1)
%
%   FitQuantiles : Quantile range [lower,upper] of the truncated generalized Gaussian distribution
%                  that shall be fit to the EEG contents (default: [0.022 0.6])
%
%   StepSizes : Step size of the grid search; the first value is the stepping of the lower bound
%               (which essentially steps over any dropout samples), and the second value
%               is the stepping over possible scales (i.e., clean-data quantiles)
%               (default: [0.01 0.01])
%
%   ShapeRange : Range that the clean EEG distribution's shape parameter beta may take (default:
%                1.7:0.15:3.5)
%
% Out:
%   Mu : estimated mean of the clean EEG distribution
%
%   Sigma : estimated standard deviation of the clean EEG distribution
%
%   Alpha : estimated scale parameter of the generalized Gaussian clean EEG distribution (optional)
%
%   Beta : estimated shape parameter of the generalized Gaussian clean EEG distribution (optional)

% assign defaults
quants = [0.022 0.6]; 
step_sizes = [0.01 0.01]; 
beta = 1.7:0.15:3.5; 
zbounds = cell(1,length(beta));
rescale = zeros(1,length(beta));
opt_lu = zeros(1,2);
opt_bounds = zeros(1,2);
opt_beta = zeros(1,length(beta));

% sort data so we can access quantiles directly
X = double(sort(X(:)));
n = length(X);

% calc z bounds for the truncated standard generalized Gaussian pdf and pdf rescaler
for b=1:length(beta)    
    zbounds{b} = sign(quants-1/2).*gammaincinv(sign(quants-1/2).*(2*quants-1),1/beta(b)).^(1/beta(b)); %#ok<*AGROW>
    rescale(b) = beta(b)/(2*gamma(1/beta(b)));
end

% determine the quantile-dependent limits for the grid search
lower_min = min(quants);                    % we can generally skip the tail below the lower quantile
max_width = diff(quants);                   % maximum width is the fit interval if all data is clean
min_width = min_clean_fraction*max_width;   % minimum width of the fit interval, as fraction of data

% get matrix of shifted data ranges
%X = X(bsxfun(@plus,(1:round(n*max_width))',round(n*(lower_min:step_sizes(1):lower_min+max_dropout_fraction))));
% replaced with:
%X = X((1:round(n*max_width))' + round(n*(lower_min:step_sizes(1):lower_min+max_dropout_fraction)));
% this cannot be translated by matlab coder due to mismatch in dimension size (and automatic
% expansion), so let's unroll this:
x = 1;
r = round(n*(lower_min:step_sizes(1):lower_min+max_dropout_fraction));
X_sub = zeros(round(n*max_width), length(r));
% columns
for mw = 1 : round(n*max_width)
    X_sub(mw,:) = X(r);
    r = r+1;
end
%X1 = X(1,:);
X1 = X_sub(1,:);

% again, unroll automatic expansion
%X = X - X1; which is X_norm = X_sub - X1
X_norm = ones(size(X_sub,1), size(X_sub,2));
for i = 1: length(X_sub)
    for j = 1: length(X1)
        X_norm(i,j) = X_sub(i,j) - X1(j);
    end
end

opt_val = Inf;
% for each interval width...
for m = round(n*(max_width:-step_sizes(2):min_width))
    % scale and bin the data in the intervals
    nbins = round(3*log2(1+m/2));
    %H = bsxfun(@times,X(1:m,:),nbins./X(m,:));
    H =  X(1:m,:) .* (nbins./X(m,:));
    logq = log(histc(H,[0:nbins-1,Inf]) + 0.01);
    
    % for each shape value...
    for b=1:length(beta)
        bounds = zbounds{b};
        % evaluate truncated generalized Gaussian pdf at bin centers
        x = bounds(1)+(0.5:(nbins-0.5))/nbins*diff(bounds);
        p = exp(-abs(x).^beta(b))*rescale(b); p=p'/sum(p);
        
        % calc KL divergences
        %kl = sum(bsxfun(@times,p,bsxfun(@minus,log(p),logq(1:end-1,:)))) + log(m);
        kl = sum(p .* (log(p) - logq(1:end-1,:))) + log(m);
        
        % update optimal parameters
        [min_val,idx] = min(kl);
        if min_val < opt_val
            opt_val = min_val;
            opt_beta = beta(b);
            opt_bounds = bounds;
            opt_lu = [X1(idx) X1(idx)+X(m,idx)];
        end
    end
end

% recover distribution parameters at optimum
alpha = (opt_lu(2)-opt_lu(1))/diff(opt_bounds);
mu = opt_lu(1)-opt_bounds(1)*alpha;
beta = opt_beta;

% calculate the distribution's standard deviation from alpha and beta
sig = sqrt((alpha^2)*gamma(3/beta)/gamma(1/beta));

mu = real(mu);
sig = real(sig);
alpha = real(alpha);
beta = real(beta);



