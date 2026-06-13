def Euclidean_distance(p,q):
# p and q are lists of floats in which the length of the list show us the d
    sum = 0
    for i in range(len(p)):
        sum += pow((p[i]-q[i]), 2)
    return pow(sum, 0.5)


def kmeans_data(n,d, input_data):
    input_data_list = [] # every xi has \n at the end
    with open(input_data, 'r') as f:
        for j in range(n):
            xi = f.readline()
            list_xi = xi.split(",")
            last_place_no_n =list_xi[-1] 
            list_xi[-1] = last_place_no_n[:-1]

            for i in range(d):
                list_xi[i] = float(list_xi[i])
            input_data_list.append(list_xi)
    return input_data_list

def kmeans(k,n,d,max_iter, input_data):
        
    nums_list = kmeans_data(n, d, input_data)

    centroids_list = []
    for j1 in range(k):
        centroids_list.append(nums_list[j1])

    i = 0
    Epsilon = 0.0001
    
    while True:   
        max_dis_centroids = float('-inf')

        kluster_list = []
        for j2 in range(k):
            kluster_list.append([])
        
        for num in nums_list:

            min_distance = float('inf')
            for j3 in range(len(centroids_list)): # for centroid in centroids_list
                Euclidean_dis = Euclidean_distance(num,centroids_list[j3])
                if  Euclidean_dis < min_distance:
                    min_distance = Euclidean_dis
                    assigned_kluster = j3
            kluster_list[assigned_kluster].append(num)
        

         #step 4 update centroids
        for j4 in range(len(centroids_list)): # for centroid in centroids_list
            new_centroid = []
            for index in range(d):
                num_d = 0
                for num in kluster_list[j4]:
                    num_d += num[index]
                new_centroid.append((num_d)/(len(kluster_list[j4])))
            dis_centroid = Euclidean_distance(centroids_list[j4], new_centroid)
            centroids_list[j4] = new_centroid

            if max_dis_centroids < dis_centroid :
                max_dis_centroids = dis_centroid
        i = i+1

        #step 5
        if i < max_iter and Epsilon <= max_dis_centroids:
            continue
        else:
            break

    return centroids_list

