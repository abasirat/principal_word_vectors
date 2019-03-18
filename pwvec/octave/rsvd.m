%
%
% This program is written by Ali Basirat ali.basirat@lingfil.uu.se as part of the 
% project Principla Word Vectors at http://urn.kb.se/resolve?urn=urn:nbn:se:uu:diva-353866
% You are allowed to modify or distribute it if you keep this header part
% 
%

function [U S V Q]  = rsvd_1(A, k, q, K)
%% A - E[A] ~ USV'
%% The expectation is over the column vectors in A
[Q R] = randomized_subspace_iteration_1(A, K, q);

%%%%%%%%%%%%%%%%%%%%
%% centring step
E = full(mean(A,2)) ;
[Q R] = qrupdate(Q, R, E, ones(size(R,2),1)) ;
B = bsxfun(@minus, Q'*A, Q'*E) ;
%%%%%%%%%%%%%%%%%%%%

% B = Q'*A  ;
[U S V] = svd(B, 'econ') ;
U = Q*U ;
k = min(k, size(U,2)) ;
U = U(:,1:k) ;
S = S(1:k , 1:k) ;
V = V(:, 1:k) ;

endfunction

