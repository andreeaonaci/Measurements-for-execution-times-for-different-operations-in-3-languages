#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <process.h>

#define numThreads 1
#define numThreadContextSwitch 1
volatile int flag = 0;
HANDLE threadNumbers[numThreadContextSwitch];
int allThreadsCompleted = 0;
int threadContextSwitched = 0;
volatile int flagCPU = 0;
CRITICAL_SECTION mutex;
HANDLE start_event, done_event;

unsigned __stdcall threadFunctions(void* arg) {
    return 0;
}

unsigned __stdcall ThreadFunctionSwitchingContext(void* arg) {
    int ThreadNumber = *(int*)(arg);
    LARGE_INTEGER start_time, end_time, freq;
    QueryPerformanceCounter(&start_time);
    while (!flag);
    EnterCriticalSection(&mutex);
    threadContextSwitched = 1;
    SetEvent(start_event);
    LeaveCriticalSection(&mutex);
    QueryPerformanceCounter(&end_time);
    QueryPerformanceFrequency(&freq);
    double migrationTimeMs = ((double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart) * 1000.0;
    printf("Thread context switching time for thread no %d: %lf milliseconds\n", ThreadNumber, migrationTimeMs);
    return 0;
}

unsigned __stdcall ThreadFunctionMigration(void* arg) {
    while (!flagCPU);
    LARGE_INTEGER start_time, end_time, freq;
    QueryPerformanceCounter(&start_time);
    HANDLE currentThread = GetCurrentThread();
    DWORD_PTR affinityMask = 1;
    SetThreadAffinityMask(currentThread, affinityMask);
    QueryPerformanceCounter(&end_time);
    QueryPerformanceFrequency(&freq);
    double migration_time_ms = ((double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart) * 1000.0;
    printf("Thread migration time: %lf milliseconds\n", migration_time_ms);

    return 0;
}

int arraySizeStaticAccess = 1000000;
int arrayStaticAccess[1000000];

#define INITIAL_CAPACITY 1000000

typedef struct {
    int *array;
    int size;
    int capacity;
} DynamicArray;

void initDynamicArray(DynamicArray *arr) {
    arr->array = (int *)malloc(INITIAL_CAPACITY * sizeof(int));
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
}

void addToDynamicArray(DynamicArray *arr, int value) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->array = (int *)realloc(arr->array, arr->capacity * sizeof(int));
        if (arr->array == NULL) {
            printf("Memory reallocation failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    arr->array[arr->size++] = value;
}

void freeDynamicArray(DynamicArray *arr) {
    free(arr->array);
    arr->array = NULL;
    arr->size = arr->capacity = 0;
}

int main() {
    LARGE_INTEGER start_time, end_time, freq;
    double computed_time;

    //-------------------------Memory Allocation------------------------------------------

    int arraySizeMemoryAllocation = 1000000;
    QueryPerformanceCounter(&start_time);
    int* arrayMemoryAllocation = (int*)malloc(arraySizeMemoryAllocation * sizeof(int));
    QueryPerformanceCounter(&end_time);
    QueryPerformanceFrequency(&freq);
    computed_time = ((double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart) * 1000.0;
    printf("Time for memory allocation in milliseconds: %lf\n", computed_time);

    //-------------------------Memory Static Access------------------------------------------

    for (int i = 0; i < arraySizeStaticAccess; i++) {
        arrayStaticAccess[i] = i;
    }

    QueryPerformanceCounter(&start_time);
    for (int i = 0; i < arraySizeStaticAccess; i++) {
        int elementStatic = arrayStaticAccess[i];
    }
    QueryPerformanceCounter(&end_time);
    computed_time = ((double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart) * 1000.0;
    printf("Static memory access took %lf milliseconds.\n", computed_time);

    //-------------------------Memory Dynamic Access------------------------------------------

    double elapsedMillisecondsDynamicAccess;
    DynamicArray arrayDynamicAccess; //define it as a list
    initDynamicArray(&arrayDynamicAccess);

    QueryPerformanceCounter(&start_time);

    for (int i = 0; i < 1000000; i++) {
        addToDynamicArray(&arrayDynamicAccess, i);
    }

    QueryPerformanceCounter(&end_time);
    computed_time = ((double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart) * 1000.0;
    printf("Dynamic memory access took %f milliseconds.\n", computed_time);

    freeDynamicArray(&arrayDynamicAccess);

    //-------------------------Thread Creation and Execution------------------------------------------

    InitializeCriticalSection(&mutex);
    start_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    done_event = CreateEvent(NULL, FALSE, FALSE, NULL);

    HANDLE threads[numThreads];

    QueryPerformanceCounter(&start_time);
    for (int i = 0; i < numThreads; i++) {
        uintptr_t threadID;
        threads[i] = (HANDLE)_beginthreadex(NULL, 0, threadFunctions, NULL, 0, (unsigned*)&threadID);
    }

    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    QueryPerformanceCounter(&end_time);
    computed_time = ((double)(end_time.QuadPart - start_time.QuadPart) / freq.QuadPart) * 1000.0;
    printf("Thread creation and execution took %lf milliseconds.\n", computed_time);

    //-------------------------Thread Context Switching------------------------------------------

    HANDLE threadsContextSwitching[numThreadContextSwitch];

    for (int i = 0; i < numThreadContextSwitch; i++) {
        threadNumbers[i] = (HANDLE)(i);
        uintptr_t threadID;
        threadsContextSwitching[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFunctionSwitchingContext, &threadNumbers[i], 0, (unsigned*)&threadID);
    }

    flag = 1;
    SetEvent(start_event);
    WaitForMultipleObjects(numThreadContextSwitch, threadsContextSwitching, TRUE, INFINITE);

    //-------------------------Thread Migration------------------------------------------

    HANDLE thread_id = (HANDLE)_beginthreadex(NULL, 0, ThreadFunctionMigration, NULL, 0, NULL);

    flagCPU = 1;
    WaitForSingleObject(thread_id, INFINITE);
    return 0;
}
