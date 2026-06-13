
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "symnmf.h"
#define MAX_LINE_LENGTH 1000
#define epsilon 0.0001
#define max_iter 300
#define error_messege "An Error Has Occurred"

/*purpose: do a=b in a way the changing b wont change a*/
void deep_copy_mat(double**a,double**b,int rows, int cols){
    int i,j;
    for (i = 0; i <rows; i++) {
        for (j = 0; j < cols; j++) {
            a[i][j]=b[i][j];
        }
        
    }
}

void print_2d_arr(double ** vectors,int num_vectors,int dimensions){
    int i,j;
    for (i = 0; i < num_vectors; i++) {
        for (j = 0; j < dimensions-1; j++) {
            printf("%.4f,", vectors[i][j]);
        }
        printf("%.4f", vectors[i][dimensions-1]);
        printf("\n");
    }
}

/*purpose: make ret=a-b*/
void subtruct_matrix(double ** a,double ** b,double ** ret,int rows,int cols){
    int i,j;
    for(i=0;i<rows;i++){
        for(j=0;j<cols;j++){
            ret[i][j]=a[i][j]-b[i][j];
        }
    }
}

double square_euclidean_distance(double *vector1, double *vector2, int d) {
    double distance = 0.0,diff;
    int i;
    /* Calculate the sum of squared differences for each dimension*/
    for (i = 0; i < d; i++) {
        diff = vector1[i] - vector2[i];
        distance += diff * diff;
    }
    /*Return the square root of the sum of squared differences*/
    return distance;
}

double square_frobenius_norm(double ** mat, int n,int k){
    double sum=0;
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<k;j++){
            sum+=pow(fabs(mat[i][j]),2);
        }
    }
    return sum;
}

/*Function to multiply two matrices*/
void multiplyMatrices(double **mat1, double **mat2, double **result, int rows1, int cols1, int cols2) {
    /*Initialize the result matrix with zeros*/
    int i,j,k;
    for (i = 0; i < rows1; ++i) {
        for (j = 0; j < cols2; ++j) {
            result[i][j] = 0;
        }
    }

    /*matrix multiplication*/
    for (i = 0; i < rows1; ++i) {
        for (j = 0; j < cols2; ++j) {
            for (k = 0; k < cols1; ++k) {
                result[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}

/*given a number of vectors and dimensions creates space for them using calloc*/
double **allocate_array(int n, int d) {
    double **arr = (double **)calloc(n , sizeof(double *));
    int i,j;
    if (arr == NULL) {
        return NULL;
    }
    for (i = 0; i < n; i++) {
        arr[i] = (double *)calloc(d , sizeof(double));
        if (arr[i] == NULL) {
            /*Free previously allocated memory*/
            for (j = 0; j < i; j++) {
                free(arr[j]);
            }
            free(arr);
            return NULL;
        }
    }
    return arr;
}

/*purpose: make B= A transpose*/
void transpose(double **A, double **B, int n, int k) {
    int i,j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            B[j][i] = A[i][j];
        }
    }
}

/*Function to free memory allocated for vectors*/
void free_vectors(double **vectors, int num_vectors) {
    /*Free dynamically allocated memory*/
    int i;
    for (i = 0; i < num_vectors; i++) {
        free(vectors[i]);
    }
    free(vectors);
}

/*function for similarity matrix*/
double ** sym_c(double ** vectors,int num_vectors,int dimensions){
    /*Allocate memory for sym mat*/
    double ** sym_mat = allocate_array(num_vectors,num_vectors);
    int i,j;
    double pow;
    if (sym_mat == NULL) {
        return NULL;
    }

    /*sparing the computation by filling the mat this way*/
    for(i =0;i<num_vectors;i++){
        for(j=i;j<num_vectors;j++){
            if(i==j){
                sym_mat[i][j]=0;
            }
            else{
                pow=square_euclidean_distance(vectors[i],vectors[j],dimensions);
                pow=pow/2.0;
                pow=-pow;
                sym_mat[i][j]=exp(pow);

            }
        }
    }
    /*filling the bottom left part, the matrix is symmetric*/
    for(i =1;i<num_vectors;i++){
        for(j=0;j<i;j++){
            sym_mat[i][j]=sym_mat[j][i];     
        }
    }
    return sym_mat;
}

/*function that makes an empty mat (ddg_mat) of size n*n be the ddg, given the similarity matrix*/
void ddg_from_sym(double ** sym_mat, double ** ddg_mat, int num_vectors){
    int i,j;
    double sum;
    for (i=0;i<num_vectors;i++){
        sum=0;
        for(j=0;j<num_vectors;j++){
            sum+=sym_mat[i][j];
        }
        ddg_mat[i][i]=sum;
    }
}


/*function for diagonal degree matrix*/
double ** ddg_c(double ** vectors,int num_vectors,int dimensions){
    double ** sym_mat=sym_c(vectors,num_vectors,dimensions);
    double ** ddg_mat = allocate_array(num_vectors,num_vectors);
    if(sym_mat==NULL || ddg_mat == NULL){
        return NULL;
    }
    ddg_from_sym(sym_mat,ddg_mat,num_vectors);
    free_vectors(sym_mat,num_vectors);
    return ddg_mat;
}

/*a function that gets a diagonal matrix A and return A in the power of minus half*/
void power_minus_half(double ** mat, int num_vectors){
    int i;
    for(i=0;i<num_vectors;i++){
        mat[i][i]=pow(mat[i][i],-0.5);  
    }
}

/*function for normalized similarity matrix*/
double ** norm_c(double ** vectors,int num_vectors,int dimensions){
    double ** sym_mat=sym_c(vectors,num_vectors,dimensions);
    double ** ddg_mat = allocate_array(num_vectors,num_vectors);
    double ** res=allocate_array(num_vectors,num_vectors);
    double ** res1=allocate_array(num_vectors,num_vectors);

    if(sym_mat==NULL || ddg_mat == NULL || res == NULL||res1 == NULL){
        return NULL;
    }
    ddg_from_sym(sym_mat,ddg_mat,num_vectors); /*does ddg mat= the ddg based on sym*/
    power_minus_half(ddg_mat,num_vectors); /*raises ddg_mat in the power of half*/
    multiplyMatrices(ddg_mat,sym_mat,res,num_vectors,num_vectors,num_vectors); /*does res=current ddg*sym_mat*/
    multiplyMatrices(res,ddg_mat,res1,num_vectors,num_vectors,num_vectors); /*does res1=res*current ddg*/
    free_vectors(sym_mat,num_vectors);
    free_vectors(ddg_mat,num_vectors);
    free_vectors(res,num_vectors);
    return res1;
}

/*function for the whole symnmf process, returning H*/
/*needs to get: W-normalized sym mat, initial H, k, num of vectors*/
double ** symnmf_c(double ** W,double ** H0,int k,int n){
    double diff=42.0; /*initial value greater than epsilon*/
    double ** old_H=H0;
    double ** new_H;
    double ** numerator_mat;
    double ** denominator_mat0;
    double ** denominator_mat;
    double ** old_H_transpose;
    double ** diff_mat;
    int cnt=0,i,j;
    
    while(diff>=epsilon && cnt<max_iter){ 
        /*we will define numerator and denominator matrixes for simplicity of computations*/
        numerator_mat=allocate_array(n,k);
        denominator_mat0=allocate_array(n,n);
        denominator_mat=allocate_array(n,k);
        old_H_transpose=allocate_array(k,n);
        if(numerator_mat==NULL || denominator_mat==NULL || denominator_mat0==NULL||old_H_transpose==NULL){
            return NULL;
        }
        transpose(old_H,old_H_transpose,n,k);
        multiplyMatrices(W,old_H,numerator_mat,n,n,k);
        multiplyMatrices(old_H,old_H_transpose,denominator_mat0,n,k,n);
        multiplyMatrices(denominator_mat0,old_H,denominator_mat,n,n,k);

        /* now, numerator mat holds W * old H
        and denominator mat holds old H *old H transpose * old H
        and now we can create the new H */
        new_H=allocate_array(n,k);
        if(new_H==NULL){
            return NULL;
        }
        for(i=0;i<n;i++){
            for(j=0;j<k;j++){
                new_H[i][j]=old_H[i][j]*(0.5+0.5*(numerator_mat[i][j]/denominator_mat[i][j]));
            }
        }

        /*now we will update diff, using a new matrix representing the matrix new_H-old_H*/
        diff_mat=allocate_array(n,k);
        subtruct_matrix(new_H,old_H,diff_mat,n,k);
        diff=square_frobenius_norm(diff_mat,n,k);
        deep_copy_mat(old_H,new_H,n,k); /*does old_H=new_H*/
        cnt++;

        /*memory freeing*/
        free_vectors(numerator_mat,n);
        free_vectors(denominator_mat0,n);
        free_vectors(denominator_mat,n);
        free_vectors(old_H_transpose,k);
        free_vectors(new_H,n);
        /*free_vectors(diff_mat,n); new*/
    }
    return old_H;
}

/*Function to read vectors from file*/
int read_vectors_from_file(const char *filename, double ***vectors, int *num_vectors, int *dimensions) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *token;
    int dim,i;
    *num_vectors = 0;
    *dimensions = 0;
    /*Open the file*/
    file = fopen(filename, "r");
    if (file == NULL) {
        return 1;
    }

    /*Determine the number of vectors and dimensions*/
    while (fgets(line, sizeof(line), file)!=NULL) {
        (*num_vectors)++; /*Increment the number of vectors*/

        /*Tokenize the line using comma as delimiter to determine dimensions*/
        token = strtok(line, ",");
        dim = 0;
        while (token != NULL) {
            dim++;
            token = strtok(NULL, ",");
        }

        /*Update dimensions if necessary*/
        if (dim > *dimensions)
            *dimensions = dim;
    }

    /*Reset file pointer to beginning of file*/
    fseek(file, 0, SEEK_SET);

    /*Allocate memory for storing vectors using the helper function*/
    *vectors = allocate_array(*num_vectors, *dimensions);
    if (*vectors == NULL) {
        fclose(file);
        return 1;
    }

    /*Read each line from the file and store vectors*/
    for (i = 0; i < *num_vectors; i++) {
        if (fgets(line, sizeof(line), file)==NULL){
            fclose(file);
            return 1;
        }
        else{
            char *token = strtok(line, ",");
            int dim = 0;
            while (token != NULL) {
                (*vectors)[i][dim++] = atof(token);
                token = strtok(NULL, ",");
            }
        }
    }

    /*Close the file*/
    fclose(file);
    return 0;
}

/*purpose: simplify error printing*/
int error(void){
    printf(error_messege);
    return 1;
}

int main(int argc, char *argv[]) {
    /*no need to validate arguments*/
    char *goal = argv[1];
    char *filename = argv[2];
    double **vectors = NULL;
    double ** sym_mat = NULL;
    double ** ddg_mat = NULL;
    double ** norm_mat = NULL;
    int num_vectors, dimensions;

    /*Read vectors from file*/
    int file_failure_indicator=read_vectors_from_file(filename, &vectors, &num_vectors, &dimensions);
    if(argc!=3){
        return error();
    }
    if(file_failure_indicator==1){ /*f_f_i=1 iff the reading has failed*/
        return error();
    }

    /*now, vectors conatains the nums, num_vectors and dimensions is init.
    now the real deal, by goal, do what needed */
    if (strcmp(goal, "sym") == 0) /*if goal is sym*/
    {
        /*lets get sym mat*/
        sym_mat=sym_c(vectors,num_vectors,dimensions);
        if(sym_mat==NULL){
            return error();
        }
        print_2d_arr(sym_mat,num_vectors,num_vectors);
        free_vectors(sym_mat,num_vectors);
    }

    if(strcmp(goal, "ddg") == 0){ /*if goal is ddg*/
        /*lets get ddg mat*/
        ddg_mat=ddg_c(vectors,num_vectors,dimensions);
        if(ddg_mat==NULL){
            return error();
        }
        print_2d_arr(ddg_mat,num_vectors,num_vectors);
        free_vectors(ddg_mat,num_vectors);
    }

    if(strcmp(goal, "norm") == 0){/*if goal is norm*/
        /*lets get norm mat*/
        norm_mat=norm_c(vectors,num_vectors,dimensions);
        if(norm_mat==NULL){
            return error();
        }
        print_2d_arr(norm_mat,num_vectors,num_vectors);
        free_vectors(norm_mat,num_vectors);
    }
    /*Free memory allocated for vectors*/
    free_vectors(vectors, num_vectors);
    return 0;
}