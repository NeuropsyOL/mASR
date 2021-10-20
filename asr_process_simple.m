function [outdata,outstate] = asr_process_simple(data,srate,state)
% asr_calibrate_simple.m--
%
% A simplified version of asr_process.m (part of clean_rawdata toolbox) in terms of input
% parameters and used functions.
% This function is the basis for an automatic translation to C++ using Matlab Coder.
%
%
%
% Developed in Matlab 9.8.0.1359463 (R2020a) Update 1 on PCWIN64
% at University of Oldenburg.
% Sarah Blum (sarah.blum@uol.de), 2020-09-01 11:26
%-------------------------------------------------------------------------

windowlen = 0.1;
windowlen = max(windowlen,1.5*size(data,1)/srate);
lookahead = windowlen/2;
stepsize = 4;



[C,S] = size(data);
maxdims=C;
N = round(windowlen*srate);
P = round(lookahead*srate)
[T,M,A,B] = deal(state.T,state.M,state.A,state.B);

% initialize prior filter state by extrapolating available data into the past (if necessary)
if isempty(state.carry)
    state.carry = repmat(2*data(:,1),1,P) - data(:,1+mod(((P+1):-1:2)-1,S)); 
end
data = [state.carry data];

% set non-finite to zero. Equivalent to data(~isfinite(data(:))) = 0 but
% generates simpler code
for k=1:size(data,1)
    for j=1:size(data,2)
        if ~isfinite(data(k,j))
            data(k,j)=0;
        end
    end
end

% Do not split data up, we specialize for small chunks
range=1:S;
% get spectrally shaped data X for statistics computation (range shifted by lookahead)
[X,state.iir] = filter(B,A,double(data(:,range+P)),state.iir,2);

% for simplified version, estimate covariance matrix easier
Xcov = (1/S) * (X * X');

% extract the subset of time points at which we intend to update
update_at = min(stepsize:stepsize:(size(X,2)+stepsize-1),size(X,2));

% if there is no previous R (from the end of the last chunk), we estimate it right at the first sample
if isempty(state.last_R)
    update_at = [1 update_at];
    state.last_R = eye(C);
end

% do the reconstruction in intervals of length stepsize (or shorter if at the end of a chunk)
last_n = 0;
for j = 1:length(update_at)
    
    % do a PCA to find potential artifact components
    [V,D] = eig(Xcov);
    % just in case. The decomposition will return complex values in case of bad (too small,
    % ..) data. This should not happen with real EEG data, but with test data, we go the safe
    % way here
    V = real(V); D = real(D);
    [D,order] = sort(reshape(diag(D),1,C));
    V = V(:,order);
    
    % determine which components to keep (variance below directional threshold or not admissible for rejection)
    keep = D<sum((T*V).^2) | (1:C)<(C-maxdims);
    trivial = all(keep);
    % update the reconstruction matrix R (reconstruct artifact components using the mixing matrix)
    if ~trivial
        %R_old = real(M*pinv(bsxfun(@times,keep',V'*M))*V');
        R = real(M*pinv(keep'.*(V'*M)))*V';
    else
        R = eye(C);
    end
    % apply the reconstruction to intermediate samples (using raised-cosine blending)
    n = update_at(j);
    if ~trivial || ~state.last_trivial
        subrange = range((last_n+1):n);
        blend = real((1-cos(pi .* (1:(n-last_n))/(n-last_n)))/2);
        
        % for Coder translation, we need to unroll the expansions
        
        % first, get current segment of data
        current_data = data(:, subrange);
        left = ones(size(current_data));
        right = ones(size(current_data));
        % compute left side of original expression
        R_times_range = ones(size(current_data));
        for col = 1: size(current_data,2)
            R_times_range(:,col) = R * current_data(:, col);
        end
        left = blend .* R_times_range;
        
        % compute right side of original expression
        last_R_times_range = ones(size(current_data));
        for col = 1: size(current_data,2)
            last_R_times_range(:,col) = state.last_R * current_data(:, col);
        end
        right = (1-blend) .* last_R_times_range;
        
        % assign newly computed data to the original range
        data(:, subrange) = left + right;
    end
    [last_n,state.last_R,state.last_trivial] = deal(n,R,trivial);
end

% carry the look-ahead portion of the data over to the state (for successive calls)
state.carry = [state.carry data(:,(end-P+1):end)];
state.carry = state.carry(:,(end-P+1):end);
% finalize outputs
outdata = data(:,1:(end-P));
outstate = state;


