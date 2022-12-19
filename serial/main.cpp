#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip>

using namespace std;

void read_dataset(vector<vector<int>> &dataset, string file_path) {
    vector<int> row;
    ifstream data(file_path);
    string line, num;
    getline(data, line);
    while(getline(data, line)){
        stringstream line_stream(line);
        while(getline(line_stream, num, ','))
            row.push_back(stoi(num));
        dataset.push_back(row);
        row.clear();
    }
    data.close();
}

void labelization(vector<vector<int>> &dataset, int price_threshold){
    for(int i = 0; i < dataset.size(); i++){
        if(dataset [i][8] < price_threshold)
            dataset [i][8] = 0;
        else
            dataset [i][8] = 1;
    }
}

void calc_mean(vector<vector<int>> &dataset, double low_price_mean[], double high_price_mean[],
                long int &num_of_low_price, long int &num_of_high_price){
    long double low_sum;
    long double high_sum;
    for(int col = 0; col < 8; col++){
        low_sum = 0;
        high_sum = 0;
        num_of_low_price = 0;
        num_of_high_price = 0;
        for(int row = 0; row < dataset.size(); row++){
            if(dataset[row][8]){
                high_sum += dataset[row][col];
                num_of_high_price++;
            }
            else{
                low_sum += dataset[row][col];
                num_of_low_price++;
            }
        }
        low_price_mean[col] = low_sum / num_of_low_price;
        high_price_mean[col] = high_sum / num_of_high_price;
    }
}

void calc_std(vector<vector<int>> &dataset, double low_price_std[], double high_price_std[],
              double low_price_mean[], double high_price_mean[],
              int num_of_low_price, int num_of_high_price){
    long double low_sum;
    long double high_sum;
    for(int col = 0; col < 8; col++){
        low_sum = 0;
        high_sum = 0;
        for(int row = 0; row < dataset.size(); row++){
            if(dataset[row][8])
                high_sum += pow(dataset[row][col] - high_price_mean[col], 2);
            else
                low_sum += pow(dataset[row][col] - low_price_mean[col], 2);
        }
        low_price_std[col] = sqrt(low_sum / num_of_low_price);
        high_price_std[col] = sqrt(high_sum / num_of_high_price);
    }
}

void classifier(vector<vector<int>> &dataset, double high_range[], double grLiveArea_mean, double grLiveArea_std){
    high_range[0] = grLiveArea_mean - grLiveArea_std;
    high_range[1] = grLiveArea_mean + grLiveArea_std;
    for(int i = 0; i < dataset.size(); i++){
        if(dataset[i][5] <= high_range[1] && dataset[i][5] >= high_range[0])
            dataset[i].push_back(1);
        else
            dataset[i].push_back(0);
    }
}

void calc_accurancy(vector<vector<int>> dataset){
    long int correct_detected = 0;
    double accuracy = 0;
    long int all_samples = dataset.size();
    for(int i = 0; i < all_samples; i++){
        if(dataset[i][8] == dataset[i][9])
            correct_detected++;
    }
    accuracy = (((double) correct_detected) / all_samples) * 100;
    cout << "Accuracy: " << setprecision(4) << accuracy << "%" << endl;
}

int main(int argc, char *argv[]) {
    vector<vector<int>> dataset;
    string file_name = "dataset.csv";
    double low_price_mean[8] = {0};
    double low_price_std[8] = {0};
    double high_price_mean[8] = {0};
    double high_price_std[8] = {0};
    double high_range[2] = {0, 0};
    long int num_of_low_price = 0;
    long int num_of_high_price = 0;
    string file_path = argv[1] + file_name;
    int price_threshold = stoi(argv[2]);
    read_dataset(dataset, file_path);
    labelization(dataset, price_threshold);
    calc_mean(dataset, low_price_mean, high_price_mean, num_of_low_price, num_of_high_price);
    calc_std(dataset, low_price_std, high_price_std, low_price_mean, high_price_mean, num_of_low_price, num_of_high_price);
    classifier(dataset, high_range, high_price_mean[5], high_price_std[5]);
    calc_accurancy(dataset);
    return 0;  
}