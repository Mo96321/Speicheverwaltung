#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// Constans
#define MEMORY_SIZE 1024
#define PARTITION_SIZE 128
#define NUM_PARTITIONS (MEMORY_SIZE / PARTITION_SIZE)

// Enums for management types and allocation types
typedef enum { STATIC, DYNAMIC, BUDDY } ManagementType;
typedef enum { FIRST_FIT, NEXT_FIT, BEST_FIT } AllocationType;

// Struct for memory blocks
typedef struct MemoryBlock {
    int size;
    struct MemoryBlock * next;
    
} MemoryBlock;

// Global variables
ManagementType management_type;
AllocationType allocation_type;
MemoryBlock* static_partitions[NUM_PARTITIONS];
int static_partition_usage[NUM_PARTITIONS];
MemoryBlock* dynamic_free_list;
MemoryBlock* dynamic_allocated_list;
MemoryBlock* buddy_free_list[10];
FILE* log_file = NULL;

// Signal handler for graceful termination
void handle_signal(int signal) {
    if (log_file) {
        fclose(log_file);
    }
    exit(EXIT_SUCCESS);
}

// Function to initialize static memory system
void initialize static system() {
    for (int i = 0; i < NUM_PARTITION; i++) {
        static_partitions[i] = malloc(PARTITION_SIZE);
        static_partition_usage[i] = 0;
    }
}

//Function to allocate static memory
void* allocate_static memory(int size){
    if (size > PARTITION_SIZE){
        print("Error: Requested size too Large for static partitioning.\n");
        return NULL;
    }
    for (int i = 0 < NUM_PARTITION; i++){
        if(!static_partition_usage[i]){
            static_partition_usage[i] = 1;
            if (log_file) {
                fprint(log_file, "Allocate %d bytes in static partiton %d\n", size, i);
            }
            return static_partitions[i];
        }
    }
    print("Error: No free static partiton available.\n");
    return NULL;
}

// Function to free static memory
void free_static_memory(void* ptr){
    for (int i = 0; i< NUM_PARTITIONS; i++) {
        if (static_partitions[i] == ptr) {
            static_partition_usage[i] = 0;
            if (log_file) {
                fprintf(log_file, "Freed static memory in partition %d\n", i);
            }
            return;
        }
    }
    printf("Error: Invalid static memory adress.\n");
    
}

// Initialize dynamic memory system
void initialize_dynamic_system() {
    dynamic_free_list = malloc(sizeof(MemoryBlock));
    dynamic_free_list->size = MEMORY_SIZE;
    dynamic_free_list->next = NULL;
}

// 

void * allocate_dynamic_memory(int size) {
    MemoryBlock *prev = NULL, *current = dynamic_free_list, *best_fit = NULL, *best_fit_prev = NULL;
    while (current) {
        if (current->size >= size){
            if (allocation_type == FIRST_FIT) {
                break;
            } else if (allocation_type == BEST_FIT) {
                if (!best_fit || current->size < best_fit->size) {
                    best_fit = current;
                    best_fit_prev = prev;
                }
            }
        }
        prev = current;
        current = current->next;
    }
    if (allocation_type == BEST_FIT) {
        current = best_fit;
        prev = best_fit_prev;
    }
    if (!current || current->size < size) {
        printf("Error: No suitable memory block found.\n");
        return NULL;
    }
    if (current->size == size) {
        if (prev) {
            prev->next = current->next;
        } else {
            dynamic_free_list = current->next;
        }
    } else {
        MemoryBlock *new_block = (MemoryBlock *)((char *)current + size);
        new_block->size = current->size - size;
        new_block->next = current->next;
        if (prev) {
            prev->next = new_block;
        } else {
            dynamic_free_list = new_block;
        }
        current->size = size;
    }
    current->next = dynamic_allocated_list;
    dynamic_allocated_list = current;
    if (log_file) {
        fprintf(log_file, "Allocated %d bytes dynamically\n", size);
    }
    return (void *)(current + 1);
}
