#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <pthread.h>

#define NUMBER_OF_THREADS 4

using namespace std;

struct thread_data
{
   int thread_id;
   string path;
};
struct thread_data thread_data_array[NUMBER_OF_THREADS];

string datasets[NUMBER_OF_THREADS] = {"dataset_0.csv", "dataset_1.csv", "dataset_2.csv", "dataset_3.csv"};
long int correct_detected[NUMBER_OF_THREADS] = {0};
double low_price_mean[NUMBER_OF_THREADS][8] = {0};
double high_price_mean[NUMBER_OF_THREADS][8] = {0};
double low_price_std[8] = {0};
double high_price_std[8] = {0};
double total_low_mean[8] = {0};
double total_high_mean[8] = {0};
long int num_of_low_price[NUMBER_OF_THREADS] = {0};
long int num_of_high_price[NUMBER_OF_THREADS] = {0};
double high_range[2] = {0, 0};
int done[NUMBER_OF_THREADS] = {0};
int end_calcs = 0;

vector<vector<int>> dataset[NUMBER_OF_THREADS];
int price_threshold = 0;

void read_dataset(vector<vector<int>> &_dataset, string file_path) {
    vector<int> row;
    ifstream data(file_path);
    string line, num;
    getline(data, line);
    while(getline(data, line)){
        stringstream line_stream(line);
        while(getline(line_stream, num, ','))
            row.push_back(stoi(num));
        _dataset.push_back(row);
        row.clear();
    }
    data.close();
}

void labelization(int tid){
    for(int i = 0; i < dataset[tid].size(); i++){
        if(dataset[tid][i][8] < price_threshold)
            dataset[tid][i][8] = 0;
        else
            dataset[tid][i][8] = 1;
    }
}

void calc_mean(int tid){              
    long double low_sum;
    long double high_sum;
    for(int col = 0; col < 8; col++){
        low_sum = 0;
        high_sum = 0;
        num_of_high_price[tid] = 0;
        num_of_low_price[tid] = 0;
        for(int row = 0; row < dataset[tid].size(); row++){
            if(dataset[tid][row][8]){
                high_sum += dataset[tid][row][col];
                num_of_high_price[tid]++;
            }
            else{
                low_sum += dataset[tid][row][col];
                num_of_low_price[tid]++;
            }
        }
        low_price_mean[tid][col] = low_sum / num_of_low_price[tid];
        high_price_mean[tid][col] = high_sum / num_of_high_price[tid];
    }
}

void classifier(int tid){
    high_range[0] = total_high_mean[5] - high_price_std[5];
    high_range[1] = total_high_mean[5] + high_price_std[5];
    for(int i = 0; i < dataset[tid].size(); i++){
        if(dataset[tid][i][5] <= high_range[1] && dataset[tid][i][5] >= high_range[0])
            dataset[tid][i].push_back(1);
        else
            dataset[tid][i].push_back(0);
    }
}

void count_correct_detected(int tid){
    for(int i = 0; i < dataset[tid].size(); i++){
        if(dataset[tid][i][8] == dataset[tid][i][9])
            correct_detected[tid]++;
    }
}

void calc_total_mean(){
    long double low_sum;
    long double high_sum;
    for(int col = 0; col < 8; col++){
        low_sum = 0;
        high_sum = 0;
        for(int i = 0; i < NUMBER_OF_THREADS; i++){
            low_sum += low_price_mean[i][col];
            high_sum += high_price_mean[i][col];
        }
        total_low_mean[col] = low_sum / NUMBER_OF_THREADS;
        total_high_mean[col] = high_sum / NUMBER_OF_THREADS;
    }
}

void calc_std(){
    long double low_sum;
    long double high_sum;
    int num_of_low = 0;
    int num_of_high = 0;
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        num_of_low += num_of_low_price[i];
        num_of_high += num_of_high_price[i];
    }
    for(int col = 0; col < 8; col++){
        low_sum = 0;
        high_sum = 0;
        for(int i = 0; i < NUMBER_OF_THREADS; i++){
            for(int row = 0; row < dataset[i].size(); row++){
                if(dataset[i][row][8])
                    high_sum += pow(dataset[i][row][col] - total_high_mean[col], 2);
                else
                    low_sum += pow(dataset[i][row][col] - total_low_mean[col], 2);
                
            }
        }
        low_price_std[col] = sqrt(low_sum / num_of_low);
        high_price_std[col] = sqrt(high_sum / num_of_high);
    }
}

void calc_accuracy(){
    long int all_correct_detected = 0;
    long int all_samples = 0;
    double accuracy = 0;
    for(int i = 0; i < NUMBER_OF_THREADS; i++){
        all_correct_detected += correct_detected[i];
        all_samples += dataset[i].size();
    }
    accuracy = (((double) all_correct_detected) / all_samples) * 100;
    cout << "Accuracy: " << setprecision(4) << accuracy << "%" << endl;
}

void* thread_functions(void* data){
    struct thread_data* my_data = (struct thread_data*) data;
    int thread_id = my_data->thread_id;
	string thread_path = my_data->path;
    read_dataset(dataset[thread_id], thread_path);
    labelization(thread_id);
    calc_mean(thread_id);
    done[thread_id] = 1;
    while(end_calcs == 0){}
    classifier(thread_id);
    count_correct_detected(thread_id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    price_threshold = stoi(argv[2]);
    int tid, return_code;
    pthread_t threads[NUMBER_OF_THREADS];

    for(tid = 0; tid < NUMBER_OF_THREADS; tid++){
        thread_data_array[tid].thread_id = tid;
		thread_data_array[tid].path = argv[1] + datasets[tid];

		return_code = pthread_create(&threads[tid], NULL, thread_functions, (void*)&thread_data_array[tid]);

		if (return_code){
			cout << "ERROR; return code from pthread_create() is\n";
			exit(-1);
		}
    }

    while (done[0] == 0 || done[1] == 0 || done[2] == 0 || done[3] == 0){}

    calc_total_mean();
    calc_std();
    end_calcs = 1;

    for (tid = 0; tid < NUMBER_OF_THREADS; tid++){
        pthread_join(threads[tid], NULL);
    }
    
    calc_accuracy();

    pthread_exit(NULL);
}