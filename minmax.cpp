#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

int main(){
    int n;
    cout << "Enter no of elements: ";
    cin >> n;;

    vector<int> arr(n);
    for(int i=0; i<n; i++){
        cout << "Enter elememts: ";
        cin >> arr[i];
    }

    int minVal = arr[0];
    int maxVal = arr[0];
    double sum = 0;

    #pragma omp parallel for reduction(min : minVal) reduction(max : maxVal) reduction (+:sum)
    for(int i=0; i<n; i++){
        if(arr[i] < minVal){
            minVal = arr[i];
        }
        if(arr[i] > maxVal){
            maxVal = arr[i];
        }
        sum += arr[i];
    }

    double average = (double) sum / n;

    cout << "\nMinimum value = " << minVal;
    cout << "\nMaximum value = " << maxVal;
    cout << "\nSum = " << sum;
    cout << "\nAverage = " << average << endl;

    return 0;
}