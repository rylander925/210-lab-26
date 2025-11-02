/*
COMSC-210 | Lab 26 | Rylan Der
IDE Used: Visual Studio Code 
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <chrono>
#include <string>
using namespace std;
using namespace chrono;

const int DEFAULT_SPACING = 15;
enum RACETYPE {READ, SORT, INSERT, DELETE};
enum DATATYPE {LIST, VECTOR, SET};

microseconds Read(list<string>& list, ifstream& input);
microseconds Read(set<string>& set, ifstream& input);
microseconds Read(vector<string>& vect, ifstream& input);

microseconds TimeSort(vector<string>& vector);
microseconds TimeSort(list<string>& list);

microseconds TimeInsert(vector<string>& vector, int index, string value);
microseconds TimeInsert(set<string>& set, string value);
microseconds TimeInsert(list<string>& list, int index, string value);

microseconds TimeDelete(vector<string>& vector, int index);
microseconds TimeDelete(set<string>& set, int index);
microseconds TimeDelete(list<string>& list, int index);

vector<microseconds> ReadRace(list<string>& list, vector<string>& vect, set<string>& set, string filename);
vector<microseconds> SortRace(list<string>& list, vector<string>& vect);
vector<microseconds> InsertRace(list<string>&list, vector<string>& vect, set<string>& set, string value);
vector<microseconds> DeleteRace(list<string>&list, vector<string>& vect, set<string>& set);

void OutputRace(vector<vector<microseconds>> race, string raceName, int spacing = DEFAULT_SPACING);
void OutputRace(vector<string> names, int spacing = DEFAULT_SPACING);

int main() {
    const string FILENAME = "codes.txt";
    const int TESTS = 15;
    /* 3D array of stored times:
            Outer vector stores results of each race, ordered from 0-3:
                Read, Sort, Insert, Delete
            Next inner vector stores results for each data type, ordered from 0-2:
                list, vector, set
            Final inner layer stores each time */
    vector<vector<vector<microseconds>>> races(4, vector<vector<microseconds>>(3));

    //2D array to store the results of each race
    vector<microseconds> race(3);
    
    list<string> list;
    set<string> set;
    vector<string> vect;

    //Run each test race multiple times and save results to 3D array
    //Yielded more consistent times than looping within each race function
    for (int i = 0; i < TESTS; i++) {
        //Runs races and store in 3D array
        race = ReadRace(list, vect, set, FILENAME);
        for (int dataType = 0; dataType < race.size(); dataType++) {
            races.at(READ).at(dataType).push_back(race.at(dataType));
        }
        
        race = InsertRace(list, vect, set, "TESTCODE");
        for (int dataType = 0; dataType < race.size(); dataType++) {
            races.at(INSERT).at(dataType).push_back(race.at(dataType));
        }
        race = DeleteRace(list, vect, set);
        for (int dataType = 0; dataType < race.size(); dataType++) {
            races.at(DELETE).at(dataType).push_back(race.at(dataType));
        }
        
        race = SortRace(list, vect);
        for (int dataType = 0; dataType < race.size(); dataType++) {
            races.at(SORT).at(dataType).push_back(race.at(dataType));
        } 
    }

    //Output header rows
    cout << "Number of simulations: " << TESTS << endl;
    OutputRace({"Operation", "List", "Vector", "Set"});
    
    //Outputs table contents
    vector<string> raceNames = {"Read", "Sort", "Insert", "Delete"};
    for (int raceType = 0; raceType < races.size(); raceType++) {
        OutputRace(races.at(raceType), raceNames.at(raceType));
    }
        
    return 0;
}

/**
 * Outputs race names as a header table row
 * @param names     Names to be displayed above table columns
 * @param spacing   Spacing between table columns 
 */
void OutputRace(vector<string> names, int spacing) {
    //Output titles for table columns
    for (string name : names) {
        cout << left << setw(spacing) << name;
    }
    cout << endl;
}

/**
 * Averages race durations and outputs as a table row: name followed by durations separated by spaces
 * @param race      2D list of durations for list, vector, and set
 * @param raceName  Name to be displayed to the left of the durations
 * @param spacing   Spacing between table columns 
 */
void OutputRace(vector<vector<microseconds>> race, string raceName, int spacing) {
    //output race name
    cout << left << setw(spacing) << raceName;

    //followed by entries
    for (vector<microseconds> durations : race) {
        //calculate the average duration of the race
        long long averageDuration = 0;
        for (microseconds duration : durations) {
            averageDuration += duration.count();
        }
        //if the duration is negative, it has been set to a fixed value, so set to -1
        if (averageDuration < 0) {
            averageDuration = -1;
        } else {
            //otherwise calculate average as normal
            //note integer division; since durations are generally long, will not matter much
            averageDuration /= durations.size();
        }

        //output result
        cout << left << setw(spacing) << averageDuration;
    }
    cout << endl;
}

/**
 * Returns durations of deletion operations for a list, vector, and a set
 * @param testList List to delete from
 * @param vect     Vector to delete from
 * @param set      Set to delete from
 * @return Vector containing durations of tests, ordered list, vector, set
 */
vector<microseconds> DeleteRace(list<string>&testList, vector<string>& vect, set<string>& set) {
    //Run timers for each delete operation and return times as an array
    return vector<microseconds>
        {   TimeDelete(testList, testList.size() / 2),
            TimeDelete(vect, vect.size() / 2),
            TimeDelete(set, set.size() / 2)
        };
}

/**
 * Returns durations of insertion operations for a list, vector, and a set
 * @param testList List to insert to
 * @param vect     Vector to insert to
 * @param set      Set to insert to
 * @param value    Value to insert
 * @return Vector containing durations of tests, ordered list, vector, set
 */
vector<microseconds> InsertRace(list<string>&testList, vector<string>& vect, set<string>& set, string value) {
    //Run timers and return as an array
    return vector<microseconds>
        {   TimeInsert(testList, testList.size() / 2, value),
            TimeInsert(vect, vect.size() / 2, value),
            TimeInsert(set, value)
        };
}

/**
 * Returns resulting durations from sorting race between a list and vector. Third duration (for a set) is -1.
 * @param list  List to sort
 * @param vect  Vector to sort
 * @return Vector of sorting durations, ordered list, vector, set
 */
vector<microseconds> SortRace(list<string>& list, vector<string>& vect) {
    //Run timers and return as an array. The third parameter is set to negative one for a set
    return vector<microseconds>
        {
            TimeSort(list), 
            TimeSort(vect), 
            microseconds(-1) //set sort time to -1 for the set
        };
}

/**
 * Run race for read operations on given list, vector, and set
 * Returns vector of durations, ordered [0] list, [1] vector, [2] set
 * @param testList      List to read to
 * @param testSet       Set to read to
 * @param testVector    Vector to read to
 * @param filename      File to read data from
 * @return Vector of durations in microseconds, ordered list, vector, set
 */
vector<microseconds> ReadRace(list<string>& testList, vector<string>& testVector, set<string>& testSet, string filename) {

    vector<microseconds> durations(3);
    
    //Verify file opens properly
    ifstream infile;
    infile.open(filename);
    if (!infile.is_open()) {
        cout << "Error opening file " << filename << endl;
        throw ios_base::failure("File open error");
    }

    //Time read operation for the list
    //Use a dummy list for repeat tests
    durations.at(LIST) = Read(testList, infile); 

    //Reset file stream to beginning
    infile.clear();
    infile.seekg(0);

    //Time read operation for vector
    durations.at(VECTOR) = Read(testVector, infile);

    infile.clear();
    infile.seekg(0);
    
    //Time read operation for set
    durations.at(SET) = Read(testSet, infile);

    infile.close();

    return durations;
}

/**
 * Time how long it takes to read data into a list
 * @param list List to read data to
 * @param input Input stream to read from
 * @return Duration in microseconds 
 */
microseconds Read(list<string>& list, ifstream& input) {
    auto start = high_resolution_clock::now();
    //Read all data from input stream and add to list
    string value;
    while (getline(input, value)) {
        list.push_back(value);
    }
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to read data into a set
 * @param set   Set to read data to
 * @param input Input stream to read from
 * @return Duration in microseconds
 */
microseconds Read(set<string>& set, ifstream& input) {
    auto start = high_resolution_clock::now();

    //Read all data from input stream and add to set
    string value;
    while (getline(input, value)) {
        set.insert(value);
    }

    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to read data into a vector
 * @param vector Vector to read data to
 * @param input  Input stream to read from
 * @return Duration in microseconds
 */
microseconds Read(vector<string>& vector, ifstream& input) {
    auto start = high_resolution_clock::now();

    //Read all data from input stream and add to vector
    string value;
    while (getline(input, value)) {
        vector.push_back(value);
    }

    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start);
}

/**
 * Time sort function of a list
 * @param l     List to sort
 * @return Duration in microseconds
 */
microseconds TimeSort(list<string>& l) {
    auto start = high_resolution_clock::now();
    l.sort();
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}

/**
 * Time sort function of a vector
 * @param vector Vector to sort
 * @return Duration in microseconds
 */
microseconds TimeSort(vector<string>& vect) {
    auto start = high_resolution_clock::now();
    sort(vect.begin(), vect.end());
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to add a value to a set
 * @param set   Set to insert a value to
 * @param value Value to insert into set
 * @return Durations in microseconds
 */
microseconds TimeInsert(set<string>& set, string value) {
    //start timer and insert
    auto start = high_resolution_clock::now();
    set.insert(value);
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to insert a value at the specified index
 * @param vect   Vector to insert value to
 * @param index  Index to insert value to
 * @param value  Value to insert
 * @return Duration in microseconds
 */
microseconds TimeInsert(vector<string>& vect, int index, string value) {
    //time insert operation
    auto start = high_resolution_clock::now();
    vect.insert(vect.begin() + index, value);
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to insert a value at the specified position. 
 * Does not track the time needed to iterate through the list to retrieve the iterator at the proper position.
 * @param l     List to insert value to
 * @param index Index to insert value to
 * @param value Value to insert
 * @return Duration in microseconds
 */
microseconds TimeInsert(list<string>& l, int index, string value) {
    //iterate through list to the specified location before starting timer
    list<string>::iterator location = l.begin();
    for (int i = 0; i < index; i++) 
        location++;

    //start timer and time insert operations based on number of tests
    auto start = high_resolution_clock::now();
    l.insert(location, value);
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to delete a value from a set
 * @param testSet Set to delete a value from
 * @param index   Index to delete
 * @return Duration in microseconds
 * @note Does not check for an empty set
 */
microseconds TimeDelete(set<string>& testSet, int index) {
    //iterate through set to the specified location before starting timer
    set<string>::iterator location = testSet.begin();
    for (int i = 0; i < index; i++) location++;

    //start timer and time delete operations based on number of tests
    auto start = high_resolution_clock::now();
    testSet.erase(location);
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to delete a value at the specified index
 * @param vect   Vector to delete a value from
 * @param index  Index to delete a value from
 * @return Duration in microseconds
 * @note Does not check whether vector is empty
 */
microseconds TimeDelete(vector<string>& vect, int index) {
    //start timer and time delete operations based on number of tests
    auto start = high_resolution_clock::now();
    vect.erase(vect.begin() + index);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start);
}

/**
 * Time how long it takes to delete a value at the specified position. 
 * Does not track the time needed to iterate through the list to retrieve the iterator at the proper position.
 * @param l     List to delete value of
 * @param index Index to delete
 * @return Durations in microseconds
 * @note Does not check whether list is empty
 */
microseconds TimeDelete(list<string>& l, int index) {
    //iterate through list to the specified location; does not start timer
    list<string>::iterator location = l.begin();
    for (int i = 0; i < index; i++) location++;

    //starts timer and times delete operations
    auto start = high_resolution_clock::now();
    l.erase(location);
    auto end = high_resolution_clock::now();

    return duration_cast<microseconds>(end - start);
}