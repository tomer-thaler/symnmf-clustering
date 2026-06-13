from sklearn.metrics import silhouette_score
import sys
import pandas as pd
import numpy as np
from symnmf import makeMatrix, symnmf_process, error
import kmeans

#a function that gets H from symnmf and reurns the lables for silhouette
def cluster_assignment(H): 
    dp_to_cluster = np.argmax(H,axis=1)
    dp_to_cluster=dp_to_cluster.tolist()
    return dp_to_cluster

#a function that assigns each num- AKA vector to an index of a centroid by finding its closect centroid
def findMindis_index(num, centroids_kmeans):
    min_distance = float('inf')
    for i in range(len(centroids_kmeans)): # for centroid in centroids_list
        Euclidean_dis = kmeans.Euclidean_distance(num,centroids_kmeans[i])
        if  Euclidean_dis < min_distance:
            min_distance = Euclidean_dis
            assigned_cluster = i
    return assigned_cluster

#a function that gets data matrix and centroids from kmeans and returns the lables for silhouette using the function findmindis_index
def assign_labels_to_centroids_kmeans(X, centroids_kmeans):
    lables_kmeans = [findMindis_index(num, centroids_kmeans) for num in X]
    return lables_kmeans

if __name__ == "__main__":
    np.random.seed(0)
    k = int(sys.argv[1])
    file_name = sys.argv[2]
    try:
        X = makeMatrix(file_name)
        num_vectors = len(X)
        dim = len(X[0])

        H = symnmf_process(X,num_vectors, k) #symnmf process returns the H of symnmf
        dp_to_cluster = cluster_assignment(H) #lables of symnmf
        centroids_kmeans = kmeans.kmeans(k,num_vectors,dim, 300, file_name) #kmeans returns the centroids
        labels_kmeans = assign_labels_to_centroids_kmeans(X, centroids_kmeans)#lables of kmeans

    except Exception as e:
        error()
    print("nmf: %.4f" % silhouette_score(X, dp_to_cluster))
    print("kmeans: %.4f" % silhouette_score(X,labels_kmeans))



