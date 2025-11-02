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

microseconds Read(list<string>& list, ifstream& input);
microseconds Read(set<string>& set, ifstream& input);
microseconds Read(vector<string>& vect, ifstream& input);

//modify to 2d array to store multiple times
vector<microseconds> TimeSort(vector<string>& vector, int tests);
vector<microseconds> TimeSort(list<string>& list, int tests);

//modify to 2d array to store multiple times
vector<microseconds> TimeInsert(vector<string>& vector, int index, string value, int tests);
vector<microseconds> TimeInsert(set<string>& set, string value, int tests);
vector<microseconds> TimeInsert(list<string>& list, int index, string value, int tests);

//modify to 2d array to store multiple times
vector<microseconds> TimeDelete(vector<string>& vector, int index, int tests);
vector<microseconds> TimeDelete(set<string>& set, int index, int tests);
vector<microseconds> TimeDelete(list<string>& list, int index, int tests);

//modify to 2d array to store multiple times
vector<vector<microseconds>> ReadRace(list<string>& list, vector<string>& vect, set<string>& set, string filename, int tests);
vector<vector<microseconds>> SortRace(list<string>& list, vector<string>& vect, int tests);
vector<vector<microseconds>> InsertRace(list<string>&list, vector<string>& vect, set<string>& set, string value, int tests);
vector<vector<microseconds>> DeleteRace(list<string>&list, vector<string>& vect, set<string>& set, int tests);

void OutputRace(vector<microseconds> durations, string raceName, int spacing = DEFAULT_SPACING);
void OutputRace(vector<string> names, int spacing = DEFAULT_SPACING);

int main() {
    const string FILENAME = "codes.txt";
    const int TESTS = 10;
    const int READ_TESTS = 1;
    const int SORT_TESTS = 1;
    const int INSERTION_TESTS = 1;
    const int DELETION_TESTS = 1;
    const int SPACING = 15;

    list<string> list;
    set<string> set;
    vector<string> vect;

    /*
        3D array of stored times:
            Outer vector stores results of each race, ordered from 0-3:
                Read, Sort, Insert, Delete
            Next inner vector stores results for each data type, ordered from 0-2:
                list, vector, set
            Final inner layer stores each time
                 
     */
    vector<vector<vector<microseconds>>> races(4);
    for (int i = 0; i < TESTS; i++) {
        //Runs races and store in 3D array
        races.at(0) = ReadRace(list, vect, set, FILENAME, READ_TESTS);
        //races.at(1) = SortRace(list, vect, SORT_TESTS);
        //races.at(2) = InsertRace(list, vect, set, "TESTCODE", INSERTION_TESTS);
        //races.at(3) = DeleteRace(list, vect, set, DELETION_TESTS);

        OutputRace({"Operation", "List", "Vector", "Set"});

        for (int raceType = 0; raceType < races.size(); raceType++) {
            switch (raceType) {
                case 0:
                    cout << setw(SPACING) <<  left << "Read:";
                    break; 
                case 1:
                    cout << setw(SPACING) <<  left << "Sort:";
                    break;
                case 2:
                    cout << setw(SPACING) <<  left << "Insert:";
                    break;
                case 3:
                    cout << setw(SPACING) <<  left << "Delete:";
                    break;
            }
            for (int datatype = 0; datatype < races.at(raceType).size(); datatype++) {
                //average out durations for each test
                long long averageDuration = 0;
                cout << "{";
                for (int i = 0; i < races.at(raceType).at(datatype).size(); i++) {
                    long long duration = races.at(raceType).at(datatype).at(i).count();
                    cout << duration << ", ";
                    averageDuration += duration;
                }
                cout << "}";

                //if the duration is negative, it has been set to a fixed value, so set to -1
                if (averageDuration < 0) {
                    averageDuration = -1;
                } else {
                    //otherwise calculate average as normal
                    //note integer division; since durations are generally long, will not matter much
                    averageDuration /= races.at(raceType).at(datatype).size();
                }
                cout << setw(SPACING) << left << averageDuration;
            }
            cout << endl;
        }
    }

    return 0;
}

/* syntax examples:
auto start = high_resolution_clock::now()
auto end = high_resolution_clock::now()
auto duration = duration_cast<microseconds>(end - start)
duration.count() references elapsed microseconds
*/

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
 * Outputs race durations as a table row: name followed by durations separated by spaces
 * @param durations List of durations, ordered list, vector, set
 * @param raceName  Name to be displayed to the left of the durations
 * @param spacing   Spacing between table columns 
 */
void OutputRace(vector<microseconds> durations, string raceName, int spacing) {
    //output race name
    cout << left << setw(spacing) << raceName;

    //followed by entries
    for (microseconds duration : durations) {
        cout << left << setw(spacing) << duration.count();
    }
    cout << endl;
}

/**
 * Returns durations of deletion operations for a list, vector, and a set
 * @param testList List to delete from
 * @param vect     Vector to delete from
 * @param set      Set to delete from
 * @param tests    Number of times to repeat test
 * @return Vector containing durations of tests, ordered list, vector, set
 */
vector<vector<microseconds>> DeleteRace(list<string>&testList, vector<string>& vect, set<string>& set, int tests) {
    //Run timers for each delete operation and return times as an array
    return vector<vector<microseconds>>
        {   TimeDelete(testList, testList.size() / 2, tests),
            TimeDelete(vect, vect.size() / 2, tests),
            TimeDelete(set, set.size() / 2, tests)
        };
}

/**
 * Returns durations of insertion operations for a list, vector, and a set
 * @param testList List to insert to
 * @param vect     Vector to insert to
 * @param set      Set to insert to
 * @param value    Value to insert
 * @param tests    Number of times to repeat test
 * @return Vector containing durations of tests, ordered list, vector, set
 */
vector<vector<microseconds>> InsertRace(list<string>&testList, vector<string>& vect, set<string>& set, string value, int tests) {
    //Run timers and return as an array
    return vector<vector<microseconds>>
        {   TimeInsert(testList, testList.size() / 2, value, tests),
            TimeInsert(vect, vect.size() / 2, value, tests),
            TimeInsert(set, value, tests)
        };
}

/**
 * Returns resulting durations from sorting race between a list and vector. Third duration (for a set) is -1.
 * @param list  List to sort
 * @param vect  Vector to sort
 * @param tests Number of times to repeat test
 * @return Vector of sorting durations, ordered list, vector, set
 */
vector<vector<microseconds>> SortRace(list<string>& list, vector<string>& vect, int tests) {
    //Run timers and return as an array. The third parameter is set to negative one for a set
    return vector<vector<microseconds>>
        {
            TimeSort(list, tests), 
            TimeSort(vect, tests), 
            vector<microseconds>(tests, static_cast<microseconds>(-1)) //initialize a dummy vector and set all times to -1
        };
}

/**
 * Run race for read operations on given list, vector, and set
 * Returns vector of durations, ordered [0] list, [1] vector, [2] set
 * @param testList      List to read to
 * @param testSet       Set to read to
 * @param testVector    Vector to read to
 * @param filename      File to read data from
 * @param tests         Number of times to repeat tests
 * @return Vector of durations in microseconds, ordered list, vector, set
 */
vector<vector<microseconds>> ReadRace(list<string>& testList, vector<string>& testVector, set<string>& testSet, string filename, int tests) {
    vector<vector<microseconds>> durations(3);
    
    //Verify file opens properly
    ifstream infile;
    infile.open(filename);
    if (!infile.is_open()) {
        cout << "Error opening file " << filename << endl;
        throw ios_base::failure("File open error");
    }
    
    for (int i = 0; i < tests; i++) {
        list<string> dummyList;
        vector<string> dummyVect;
        set<string> dummySet;

        //Time read operation for the list
        //Use a dummy list for repeat tests
        durations.at(0).push_back(Read((i == 0 ? testList : dummyList), infile)); 
        //Reset file stream to beginning
        infile.clear();
        infile.seekg(0);


        //Time read operation for vector
        durations.at(1).push_back(Read((i == 0) ? testVector : dummyVect, infile));
        infile.clear();
        infile.seekg(0);
        
        //Time read operation for set
        durations.at(2).push_back(Read((i == 0) ? testSet : dummySet, infile));
        infile.clear();
        infile.seekg(0);
    }
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

    cout << "Read list: " << duration_cast<microseconds>(end - start).count() << endl;

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
 * @param tests Number of times to repeat test
 * @return Durations in microseconds
 */
vector<microseconds> TimeSort(list<string>& l, int tests) {
    //vector stores results of each test
    vector<microseconds> durations;

    //sort using a dummy variable for all but last test
    for (int i = 0; i < tests - 1; i++) {
        //create a dummy list for multiple tests
        list<string> testList = l;

        auto start = high_resolution_clock::now();
        testList.sort();
        auto end = high_resolution_clock::now();

        durations.push_back(duration_cast<microseconds>(end - start));
    }

    //sort using actual list for final test
    auto start = high_resolution_clock::now();
    l.sort();
    auto end = high_resolution_clock::now();
    durations.push_back(duration_cast<microseconds>(end - start));

    return durations;
}

/**
 * Time sort function of a vector
 * @param vector Vector to sort
 * @param tests  Number of times to repeat test
 * @return Durations in microseconds
 */
vector<microseconds> TimeSort(vector<string>& vect, int tests) {
    //vector stores results of each test
    vector<microseconds> durations;

    //test with a dummy vector for all but last test
    for (int i = 0; i < tests - 1; i++) {
        //initialize a dummy vector
        vector<string> testVect = vect;

        //time sort operation for dummy tests
        auto start = high_resolution_clock::now();
        sort(testVect.begin(), testVect.end());
        auto end = high_resolution_clock::now();

        durations.push_back(duration_cast<microseconds>(end - start));
    }

    //sort the actual vector for the final test
    auto start = high_resolution_clock::now();
    sort(vect.begin(), vect.end());
    auto end = high_resolution_clock::now();

    durations.push_back(duration_cast<microseconds>(end - start));

    return durations;
}

/**
 * Time how long it takes to add a value to a set
 * @param set   Set to insert a value to
 * @param value Value to insert into set
 * @param tests Number of times to repeat test
 * @return Durations in microseconds
 */
vector<microseconds> TimeInsert(set<string>& set, string value, int tests) {
    //vector to store durations
    vector<microseconds> durations;
    
    for(int i = 0; i < tests; i++) {
        //start timer and insert
        auto start = high_resolution_clock::now();
        set.insert(value);
        auto end = high_resolution_clock::now();
        durations.push_back(duration_cast<microseconds>(end - start));
    }

    return durations;
}

/**
 * Time how long it takes to insert a value at the specified index
 * @param vect   Vector to insert value to
 * @param index  Index to insert value to
 * @param value  Value to insert
 * @param tests  Number of times to repeat test
 * @return Durations in microseconds
 */
vector<microseconds> TimeInsert(vector<string>& vect, int index, string value, int tests) {
    //Use a vector to store durations of each test
    vector<microseconds> durations;

    for(int i = 0; i < tests; i++) {
        //time insert operations based on # of tests
        auto start = high_resolution_clock::now();
        vect.insert(vect.begin() + index, value);
        auto end = high_resolution_clock::now();
        durations.push_back(duration_cast<microseconds>(end - start));
    }

    return durations;
}

/**
 * Time how long it takes to insert a value at the specified position. 
 * Does not track the time needed to iterate through the list to retrieve the iterator at the proper position.
 * @param l     List to insert value to
 * @param index Index to insert value to
 * @param value Value to insert
 * @param tests Number of times to repeat test
 * @return Durations in microseconds
 */
vector<microseconds> TimeInsert(list<string>& l, int index, string value, int tests) {
    //iterate through list to the specified location before starting timer
    list<string>::iterator location = l.begin();
    for (int i = 0; i < index; i++) 
        location++;

    //Use a vector to store durations of each test
    vector<microseconds> durations;
    
    for (int i = 0; i < tests; i++) {
        //start timer and time insert operations based on number of tests
        auto start = high_resolution_clock::now();
        l.insert(location, value);
        auto end = high_resolution_clock::now();
        durations.push_back(duration_cast<microseconds>(end - start));
    }
    return durations;
}

/**
 * Time how long it takes to delete a value from a set
 * @param testSet Set to delete a value from
 * @param index   Index to delete
 * @param tests   Number of times to repeat test
 * @return Durations in microseconds
 * @note Does not check for an empty set
 */
vector<microseconds> TimeDelete(set<string>& testSet, int index, int tests) {
    
    //Use a vector to store durations
    vector<microseconds> durations;
    
    
    for(int i = 0; i < tests; i++) {
        //iterate through set to the specified location before starting timer
        set<string>::iterator location = testSet.begin();
        for (int i = 0; i < index; i++) location++;

        //start timer and time delete operations based on number of tests
        auto start = high_resolution_clock::now();
        testSet.erase(location);
        auto end = high_resolution_clock::now();
        durations.push_back(duration_cast<microseconds>(end - start));
    }
    return durations;
}

/**
 * Time how long it takes to delete a value at the specified index
 * @param vect   Vector to delete a value from
 * @param index  Index to delete a value from
 * @param tests  Number of times to repeat test
 * @return Durations in microseconds
 * @note Does not check whether vector is empty
 */
vector<microseconds> TimeDelete(vector<string>& vect, int index, int tests) {
    //Use vector to store durations
    vector<microseconds> durations;

    for(int i = 0; i < tests; i++) {
        //start timer and time delete operations based on number of tests
        auto start = high_resolution_clock::now();
        vect.erase(vect.begin() + index);
        auto end = high_resolution_clock::now();
        durations.push_back(duration_cast<microseconds>(end - start));
    }
    return durations;
}

/**
 * Time how long it takes to delete a value at the specified position. 
 * Does not track the time needed to iterate through the list to retrieve the iterator at the proper position.
 * @param l     List to delete value of
 * @param index Index to delete
 * @param tests Number of times to repeat test
 * @return Durations in microseconds
 * @note Does not check whether list is empty
 */
vector<microseconds> TimeDelete(list<string>& l, int index, int tests) {
    
    //Use a vector to store durations
    vector<microseconds> durations;
    
    for (int i = 0; i < tests; i++) {
        //iterate through list to the specified location; does not start timer
        list<string>::iterator location = l.begin();
        for (int i = 0; i < index; i++) location++;

        //starts timer and times delete operations
        auto start = high_resolution_clock::now();
        l.erase(location);
        auto end = high_resolution_clock::now();
        durations.push_back(duration_cast<microseconds>(end - start));
    }
    return durations;
}