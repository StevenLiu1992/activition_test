#include <vector>
#include <fstream>
#include <thread>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;


#define toDigit(c) (c-'0')

vector<string> dictionary;

struct password{
	int row;
	int count;
	vector<vector <char>> wheels;
}WHEEL;

bool readDictionary(string dir){
	float secs;
	cout << "read file... " << endl;
	ifstream dictionary_file(dir);
	if (!dictionary_file){
		cout << "Error opening dictionary file" << endl;
		return false;
	}
	string temp;

	clock_t startRead = clock();

	while (getline(dictionary_file, temp)){
		//read the file to memony
		dictionary.push_back(temp);
	}

	secs = (float)(clock() - startRead) / CLOCKS_PER_SEC;
	cout << "read file: " << secs << endl;
	dictionary_file.close();

	return true;
}



bool readWheels(string dir){
	ifstream wheel_file(dir);
	if (!wheel_file){
		cout << "Error opening wheel file" << endl;
		return false;
	}

	string temp;
	WHEEL.row = 0;
	WHEEL.count = 0;

	getline(wheel_file, temp);
	//get the number of wheel
	
	for (unsigned int i = 0; i < temp.size(); i++){
		WHEEL.row *= 10;
		WHEEL.row += toDigit(temp[i]);
	}

	getline(wheel_file, temp);
	//get the number of character per wheel
	for (unsigned int i = 0; i < temp.size(); i++){
		WHEEL.count *= 10;
		WHEEL.count += toDigit(temp[i]);
	}

	WHEEL.wheels.resize(WHEEL.row, vector<char>(WHEEL.count, 0));

	int i = 0;
	while (getline(wheel_file, temp)){
		for (int j = 0; j < WHEEL.count; j++){
			WHEEL.wheels[i][j] = temp[j];
		}
		i++;
	}

	return true;
}



bool searchWord(int current_row, int current_index, string word){
	if (current_index == word.size()){
		//find it
		return true;
	}
	if (current_row > (WHEEL.row - word.size() + current_index)){
		//rest rows less than the rest of word size
		return false;
	}
	int i;
	for (i = 0; i < WHEEL.count; i++){
		//for each character of this row
		if (WHEEL.wheels[current_row][i] == word[current_index] ||
			(WHEEL.wheels[current_row][i] - 'A' + 'a') == word[current_index]){
			//go to the next character
			return searchWord(current_row + 1, current_index + 1, word);
		}
	}
	if (i == WHEEL.count){
		//cant find same character in current row
		return false;
	}

}


void dosearch(int index, int start, int end,int con[]){
	//for multi-threading,
	//from start to end inside the dictionary,
	//record count in con[index]
	for (int i = start; i <= end; i++){
		string word = dictionary[i];
		
		for (int j = 0; j < WHEEL.row; j++){
			if ((WHEEL.row - j) >= word.size()){

				if (searchWord(j, 0, word)){
					con[index]++;
					cout << word << endl;
				}
			}
		}
	}
}


#define MT
void main(int argc, char *argv[]){
	if (argc != 3){
		cout << "error input.." << endl;
		return;
	}
	readDictionary(argv[1]);
	readWheels(argv[2]);

	int count = 0;
	clock_t startSort = clock();
#ifdef MT
#define MAX_CORE 16
	int con[MAX_CORE] = { 0 };
	thread multiThreadTest[MAX_CORE];
	
	//get the amount of core
	unsigned int thread_size = std::thread::hardware_concurrency();

	int number = dictionary.size() / thread_size;
	
	for (int i = 0; i < thread_size; i++){
		if (i != (thread_size - 1)){
			int start = i * number;
			int end = start + number - 1;
			multiThreadTest[i] = std::thread(dosearch, i, start, end, con);
		}
		else{
			int start = i*number;
			int end = dictionary.size() - 1;
			multiThreadTest[i] = std::thread(dosearch, i, start, end, con);

		}
	}

	for (int i = 0; i < thread_size; i++){

		multiThreadTest[i].join();
		count += con[i];
	}
	cout << "thread size: " << thread_size << endl;
#endif
#ifndef MT
	for (vector<string>::iterator it = dictionary.begin(); it != dictionary.end(); ++it){
		string word = *it;
		int i;

		for (i = 0; i < WHEEL.row; i++){
			if ((WHEEL.row - i) >= word.size()){

				if (searchWord(i, 0, word)){	
					count++;
					cout << word << endl;
					
				}	
			}
		}
	}
#endif

	float secs = (float)(clock() - startSort) / CLOCKS_PER_SEC;
	cout << "Found " << count << " words"<< endl;
	cout << "search word time: " << secs << endl;
	
	system("PAUSE");
}