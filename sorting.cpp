#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

void sequentialBubble(vector<int> &arr){
    int n = arr.size();
    for(int i=0; i<n; i++){
        for(int j=0; j<n-i-1; j++){
            if(arr[j] > arr[j+1]){
                swap(arr[j], arr[j+1]);
            }
        }
    }
}

void parallelBubble(vector<int> &arr){
    int n = arr.size();

    for(int i=0; i<n; i++){
        if(i % 2 == 0){
            #pragma omp parallel for
            for(int j=0; j<n-1; j++){
                if(arr[j] > arr[j+1]){
                    swap(arr[j], arr[j+1]);
                }
            }
        }
        else{
            
            #pragma omp parallel for
            for(int j=1; j<n-1; j++){
                if(arr[j] > arr[j+1]){
                    swap(arr[j], arr[j+1]);
                }
            }
        }
    }
}

void merge(vector<int> &a, int l, int m, int r){
    int i = l;
    int j = m + 1;

    vector<int> temp;

    while(i <= m && j <= r){
        if(a[i] <= a[j]) temp.push_back(a[i++]);
        else temp.push_back(a[j++]); 
    }

    while(i <= m) temp.push_back(a[i++]);
    while(j <= r) temp.push_back(a[j++]);

    for(int k=l; k<=r; k++){
        a[k] = temp[k-l];
    }
}

void sequentialMerge(vector<int> &a, int l, int r){
    if(l >= r) return;

    int m = (l + r) / 2;

    sequentialMerge(a, l, m);
    sequentialMerge(a, m+1, r);
    merge(a, l, m, r);
}

void parallelMerge(vector<int> &a, int l, int r){
    if(r - l < 1000){
        sequentialMerge(a, l, r);
        return;
    }

    int m = (l + r) / 2;

    #pragma omp task shared(a)
    parallelMerge(a, l, m);
    
    #pragma omp task shared(a)
    parallelMerge(a, m+1, r);
    
    #pragma omp taskwait
    merge(a, l, m, r);
}

int main(){
    int n = 100000;
    vector<int> arr(n);
    srand(42); // fixed seed so results are reproducible
    for(int i = 0; i < n; i++){
        arr[i] = rand() % 100000;
    }
    vector<int> arr1 = arr, arr2 = arr, arr3 = arr, arr4 = arr;

    double start, end;

    start = omp_get_wtime();
    sequentialBubble(arr1);
    end = omp_get_wtime();
    cout << "Sequential Bubble: "<< end - start << " seconds";
    
    start = omp_get_wtime();
    parallelBubble(arr2);
    end = omp_get_wtime();
    cout << "Parallel Bubble: "<< end - start << " seconds";
    
    start = omp_get_wtime();
    sequentialMerge(arr3, 0, n-1);
    end = omp_get_wtime();
    cout << "Sequential Merge: "<< end - start << " seconds";

    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        parallelMerge(arr4, 0, n-1);
    }
    end = omp_get_wtime();
    cout << "Parallel Merge: " << end - start << " seconds";

    return 0;
    
}