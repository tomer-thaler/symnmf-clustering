# ifndef SYMNMF_H_
# define SYMNMF_H_

double ** sym_c(double ** vectors,int num_vectors,int dimensions);
double ** ddg_c(double ** vectors,int num_vectors,int dimensions);
double ** norm_c(double ** vectors,int num_vectors,int dimensions);
double ** symnmf_c(double ** W,double ** H0,int k,int n);


# endif