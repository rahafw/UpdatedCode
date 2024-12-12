// we adjusted the code to have a larger data size & include race condition variables 

#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <chrono>
using namespace std;

struct Event {
    int start;
    int end;
};

bool compareByEnd(const Event& a, const Event& b) {
    return a.end < b.end;
}


int maxEventsWithRaceCondition(vector<Event>& events, int numThreads) {
    sort(events.begin(), events.end(), compareByEnd);

    // Shared variable without protection
    int globalAttendCount = 0;

    #pragma omp parallel num_threads(numThreads)
    {
       // Thread-local variable
        int localCurrentDay = 0; 

        #pragma omp for schedule(dynamic, 100)
        for (size_t i = 0; i < events.size(); ++i) {
            if (events[i].start >= localCurrentDay) {

                // No synchronization here
                globalAttendCount++;
                localCurrentDay = events[i].end;
            }
        }
    }

    return globalAttendCount; 
}

int main() {
    srand(time(nullptr)); 

    // Dataset size
    const int NUM_EVENTS = 1000000; 
    vector<Event> events;
    for (int i = 0; i < NUM_EVENTS; ++i) {
        int start = rand() % 1000000;
        int end = start + rand() % 100;
        events.push_back({start, end});
    }

    // Fixed number of threads
    int numThreads = 8;

    auto start = chrono::high_resolution_clock::now();
    int result = maxEventsWithRaceCondition(events, numThreads);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    cout << "Maximum Event: " << result << endl;
    cout << "Time: " << duration << " nanoseconds" << endl;

    return 0;
}