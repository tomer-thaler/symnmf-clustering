#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdlib.h>
#include "symnmf.h"

static void free_2d_arr(double** arr,int n){
    int i;
    for(i=0;i<n;i++){
        free(arr[i]);
    }
    free(arr);

}

static double** allocate_2d_array(int n,int d) { /*check returns*/
    /*Allocate memory for the array of pointers*/
    int i;
    int j;
    double **arr = (double **)malloc(n * sizeof(double *));
    if (arr == NULL) {
        printf("An Error Has Occurred"); /*free all mem*/
        /*Memory allocation failed*/
        return NULL;
    }

    /*Allocate memgory for inner array*/
    
    for (i = 0; i < n; i++) {
        arr[i] = (double *)malloc(d * sizeof(double));
        if (arr[i] == NULL) {
            printf("An Error Has Occurred"); /*free all mem*/
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

static double **convert_Py2dlist_to_C2darray(PyObject *py2dlist, int n, int d){
    double **C2darray;
    int i,j ;
    PyObject *py2dlist_i_place;
    PyObject *Pyfloat;
    double double_num;

    C2darray = allocate_2d_array(n,d);
    if (C2darray == NULL){
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }

    else{
    for (i = 0; i < n ; i++){
        py2dlist_i_place = PyList_GetItem(py2dlist,i);
        for (j = 0; j < d; j++){
            Pyfloat = PyList_GetItem(py2dlist_i_place,j); /*copy the py number in he j index of the i centroid*/
            double_num = PyFloat_AsDouble(Pyfloat); /*parse this item to double in c*/
            C2darray[i][j] = double_num; /* put this num in malloc array in c*/
        }
    }}
    return C2darray;
}

static PyObject *convert_C2darray_to_Py2dlist(double **C2darray, int n, int d){
    PyObject *Py2dlist;
    PyObject *Py2dlist_i_place;
    int i,j;
    double double_num;
    PyObject *py_float;

    Py2dlist = PyList_New(n);
    for (i = 0; i < n; i++){
        Py2dlist_i_place = PyList_New(d);
        for (j = 0; j < d; j++){
            double_num = C2darray[i][j];
            py_float = Py_BuildValue("d", double_num);
            PyList_SetItem(Py2dlist_i_place, j, py_float);

        }
        PyList_SetItem(Py2dlist, i,Py2dlist_i_place);
    }
    return Py2dlist;

}


static PyObject* sym(PyObject *self, PyObject *args)
{
    PyObject *matrixsym;

    /*variables we use in order to return the value */
    int num_vectors;
    int dim;
    double **X;
    double **Sym;
    PyObject *pySym; /*returned value*/

    if(!PyArg_ParseTuple(args, "O",&matrixsym)) {
        return NULL; 
    }

    num_vectors = (int)PyList_Size(matrixsym);
    dim = (int)PyList_Size(PyList_GetItem(matrixsym, 0));

    /* converts from pyobject to 2d array in C*/
    X = convert_Py2dlist_to_C2darray(matrixsym ,num_vectors, dim);

    /* uses the sym_c func in order to calculate the similarity matrix */
    Sym = sym_c(X, num_vectors, dim);

    /* converts from 2d array in C pyobject*/
    pySym = convert_C2darray_to_Py2dlist(Sym, num_vectors,num_vectors);

    /* free memory*/
    free_2d_arr(X,num_vectors);
    free_2d_arr(Sym,num_vectors);

    return pySym;

}


static PyObject* ddg(PyObject *self, PyObject *args)
{

    PyObject *matrixddg;


 /*variables we use in order to return the value */
    int num_vectors;
    int dim;
    double **X;
    double **Ddg;
    PyObject *pyDdg; /*returned value*/


    if(!PyArg_ParseTuple(args, "O",&matrixddg)) {
        
        return NULL; 
    }

    num_vectors = (int)PyList_Size(matrixddg);
    dim = (int)PyList_Size(PyList_GetItem(matrixddg, 0));


    /* converts from pyobject to 2d array in C*/
    X = convert_Py2dlist_to_C2darray(matrixddg ,num_vectors, dim);

    /* uses the ddg_c func in order to calculate the diagonal matrix */
    Ddg = ddg_c(X, num_vectors, dim);

    /* converts from 2d array in C pyobject*/
    pyDdg = convert_C2darray_to_Py2dlist(Ddg, num_vectors,num_vectors);

    /* free memory*/
    free_2d_arr(X,num_vectors);
    free_2d_arr(Ddg,num_vectors);

    return pyDdg;
}

static PyObject* norm(PyObject *self, PyObject *args){
    PyObject *matrixnorm;

    /*variables we use in order to return the value */
    int num_vectors;
    int dim;
    double **X;
    double **Norm;
    PyObject *pyNorm; /*returned value*/

    if(!PyArg_ParseTuple(args, "O",&matrixnorm)) {

        return NULL; 
    }

    num_vectors = (int)PyList_Size(matrixnorm);
    dim = (int)PyList_Size(PyList_GetItem(matrixnorm, 0));
    
/* converts from pyobject to 2d array in C*/
    X = convert_Py2dlist_to_C2darray(matrixnorm ,num_vectors, dim);

    /* uses the norm_c func in order to calculate the normal matrix */
    Norm = norm_c(X, num_vectors, dim);

    /* converts from 2d array in C pyobject*/
    pyNorm = convert_C2darray_to_Py2dlist(Norm, num_vectors,num_vectors);

    /* free memory*/
    free_2d_arr(X,num_vectors);
    free_2d_arr(Norm,num_vectors);

    return pyNorm;

}


static PyObject* symnmf(PyObject *self, PyObject *args)
{
    PyObject *W;
    PyObject *H0;
    int k;

    /*variables we use in order to return the value */
    int num_vectors;
    double **W_c;
    double **H0_c;
    double **H;
    PyObject *pyH; /*returned value*/

    if(!PyArg_ParseTuple(args, "OOi",&W, &H0, &k)) {
        return NULL; 
    }

    num_vectors = (int)PyList_Size(W);

/* converts from pyobject to 2d array in C*/
    W_c = convert_Py2dlist_to_C2darray(W ,num_vectors, num_vectors);
    H0_c = convert_Py2dlist_to_C2darray(H0, num_vectors, k);


    /* uses the symnmf_c func in order to calculate the matrix */
    H = symnmf_c(W_c,H0_c,k, num_vectors);

    /* converts from 2d array in C pyobject*/
    pyH = convert_C2darray_to_Py2dlist(H, num_vectors,k);

    /* free memory*/
    free_2d_arr(W_c,num_vectors);
    free_2d_arr(H0_c,num_vectors);
    return pyH;

}

static PyMethodDef symnmfMethods[] = {
    {"sym",                   /* the Python method name that will be used */
      (PyCFunction) sym, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           
      PyDoc_STR("gets a matrix, num of vectors and dimension and calculates and outputs the similarity matrix")}, /*  The docstring for the function */
    
    {"ddg",                   
      (PyCFunction) ddg,
      METH_VARARGS,         
      PyDoc_STR("gets a matrix, num of vectors and dimension and calculates and outputs the Diagonal Degree Matrix")},
     
     {"norm",                   
      (PyCFunction) norm,
      METH_VARARGS,         
      PyDoc_STR("gets a matrix, num of vectors and dimension and calculates and outputs the normalized similarity matrix")},

    {"symnmf",                   
      (PyCFunction) symnmf,
      METH_VARARGS,         
      PyDoc_STR("gets a matrix, num of vectors and k and computes symNMF and outputs H")},
    {NULL, NULL, 0, NULL}};

    static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmfapi", /* name of module */
    NULL, 
    -1, 
    symnmfMethods 
};

PyMODINIT_FUNC PyInit_symnmfapi(void)
{
    PyObject *m;
    m = PyModule_Create(&symnmfmodule);
    if (!m) {
        return NULL;
    }
    return m;
}