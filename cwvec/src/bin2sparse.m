function cooc = bin2sparse(path)
e = bin2oct(path) ;

e(:,1:3) += 1 ; % the indices in bin file start from zero
nrows = max(e(:,1)) ;
ncols = max(e(:,2)) ;
nlayers = max(e(:,3)) ;
cooc = cell(1, nlayers) ;
for i = 1:nlayers
  idx = e(:,3) == i ;
  nnz = sum(idx) ;
  cooc{i} = sparse(e(idx,1), e(idx,2), e(:, 4), nrows, ncols, nnz) ;
end
