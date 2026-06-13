import sys
import symnmfapi as symnmf
import pandas as pd
import numpy as np
np.random.seed(0)

def error():
    print("An Error Has Occurred")
    sys.exit()

#a function that gets from the user the arg k, goal, filename and parses it 
def get_args():
    if (len(sys.argv) != 4):
        error()
    
    k = sys.argv[1]
    k = int(k)
    goal = sys.argv[2]
    file_name = sys.argv[3]

    return [k, goal, file_name]


def makeMatrix(filename):
    return pd.read_csv(filename, header=None).values.tolist()


def printMatrix(matrix):
    for vector in matrix:
        formatted_vector = ["%.4f" % cordinate for cordinate in vector]
        vector_str = ",".join(formatted_vector)
        print(vector_str)

#initializing H0 by te instuctions
def initializeH(W,num_vectors,k):
    W = np.array(W) ## np 2d array of W
    m = np.mean(W)
    H = np.random.uniform(0,  2 * np.sqrt(m / k), size=(num_vectors, k))
    H = H.tolist()
    return H

#a function that combines all the commands needed in order to get the sumnmf H
def symnmf_process(X,num_vectors, k):
    W = symnmf.norm(X)
    H0 = initializeH(W, num_vectors, k)
    H = symnmf.symnmf(W, H0, k)
    return H


if __name__ == "__main__":
    args = get_args()
    try:
        X = makeMatrix(args[2])
    except Exception as e:
            error()

    num_vectors = len(X)
    k = args[0]

    if args[1] == "sym":
        try:
            A = symnmf.sym(X)
            printMatrix(A)
        except Exception as e:
            error()

    if args[1] == "ddg":
        try:
            D = symnmf.ddg(X)
            printMatrix(D)
        except Exception as e:
            error()

    if args[1] == "norm":
        try:
            W = symnmf.norm(X)
            printMatrix(W)
        except Exception as e:
            error()
    
    if args[1] == "symnmf":
        try: 
            H = symnmf_process(X,num_vectors, k)
            printMatrix(H)
        except Exception as e:
            error()

