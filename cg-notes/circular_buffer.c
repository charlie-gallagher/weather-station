#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

/* Circular buffer ----------------------------------------------- */

typedef struct WeatherObs {
    int data;
    struct WeatherObs* next;
} WeatherObs;

typedef struct CircularBuffer {
    WeatherObs* head;       // Points to the oldest element
    WeatherObs* tail;       // Points to the newest element
    int size;         // Current number of elements
    int capacity;     // Max number of elements
    pthread_mutex_t lock;
} CircularBuffer;


CircularBuffer* create_buffer(int capacity) {
    CircularBuffer* cb = (CircularBuffer*)malloc(sizeof(CircularBuffer));
    cb->head = NULL;
    cb->tail = NULL;
    cb->size = 0;
    cb->capacity = capacity;
    pthread_mutex_init(&cb->lock, NULL);  // Initialize mutex
    return cb;
}

bool is_full(CircularBuffer* cb) {
    return cb->size == cb->capacity;
}

bool is_empty(CircularBuffer* cb) {
    return cb->size == 0;
}

void push(CircularBuffer* cb, int value) {
    pthread_mutex_lock(&cb->lock);

    WeatherObs* new_node = (WeatherObs*)malloc(sizeof(WeatherObs));
    new_node->data = value;

    if (is_full(cb)) {
        WeatherObs* old_head = cb->head;
        cb->head = cb->head->next;
        if (cb->tail == old_head)
            cb->tail = cb->tail->next;
        free(old_head);
        cb->size--;
    }

    new_node->next = NULL;

    if (is_empty(cb)) {
        cb->head = cb->tail = new_node;
        new_node->next = new_node;
    } else {
        new_node->next = cb->head;
        cb->tail->next = new_node;
        cb->tail = new_node;
    }

    cb->size++;

    pthread_mutex_unlock(&cb->lock);
}

int pop(CircularBuffer* cb) {
    pthread_mutex_lock(&cb->lock);

    if (is_empty(cb)) {
        pthread_mutex_unlock(&cb->lock);
        fprintf(stderr, "Error: Buffer is empty.\n");
        exit(EXIT_FAILURE);
    }

    WeatherObs* temp = cb->head;
    int value = temp->data;

    if (cb->size == 1) {
        cb->head = cb->tail = NULL;
    } else {
        cb->head = cb->head->next;
        cb->tail->next = cb->head;
    }

    free(temp);
    cb->size--;

    pthread_mutex_unlock(&cb->lock);
    return value;
}

void free_buffer(CircularBuffer* cb) {
    pthread_mutex_lock(&cb->lock);
    while (!is_empty(cb)) {
        WeatherObs* temp = cb->head;
        cb->head = cb->head->next;
        free(temp);
        cb->size--;
    }
    pthread_mutex_unlock(&cb->lock);
    pthread_mutex_destroy(&cb->lock);
    free(cb);
}

void print_buffer(CircularBuffer* cb) {
    pthread_mutex_lock(&cb->lock);

    if (is_empty(cb)) {
        printf("Buffer is empty.\n");
        pthread_mutex_unlock(&cb->lock);
        return;
    }

    WeatherObs* current = cb->head;
    printf("Buffer contents: ");
    for (int i = 0; i < cb->size; i++) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");

    pthread_mutex_unlock(&cb->lock);
}





/* End circular buffer ------------------------------------ */

#define BUFFER_SIZE 16
CircularBuffer *cb;

typedef struct {
	int buffer_writes;
} ThreadArgs;



void *thr_write_buffer(void *arg) {
	ThreadArgs *decoded_args = ((ThreadArgs *)arg);
	printf("Writing %d items to the buffer\n", decoded_args->buffer_writes);
	int i;
	for (i = 0; i < decoded_args->buffer_writes; i++) {
		push(cb, i);
	}
	return((void *)0);
}


int main(void) {
	int err;
	pthread_t tid1;
	void *tret;
	ThreadArgs thread1_args = {1000};
	cb = create_buffer(BUFFER_SIZE);

	err = pthread_create(&tid1, NULL, thr_write_buffer, &thread1_args);
	if (err != 0) {
		printf("Can't create thread 1\n");
		exit(1);
	}
	sleep(0.001);
	print_buffer(cb);
	sleep(0.001);
	print_buffer(cb);
	sleep(0.001);
	print_buffer(cb);
	sleep(0.001);
	print_buffer(cb);
	sleep(0.001);
	print_buffer(cb);
	pthread_join(tid1, tret);

	free_buffer(cb);
	return(0);
}


