#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 100

typedef struct {
  hash_table_node_t* data[MAX_QUEUE_SIZE];
  int front;
  int rear;
} queue_t;

void create_queue(){
    // Initializes a queue by setting the front and rear indices to -1.
    queue_t *q = malloc(sizeof(queue_t));
    q->front = -1;
    q->rear = -1;
}

void destroy_queue(queue_t *q){
    // Frees the memory allocated to the queue.
    free(q);
}

bool is_empty(queue_t *q){
    // Returns true if the queue is empty, false otherwise.
    if (q->front == -1 && q->rear == -1){
        return true;
    }
    else{
        return false;
    }
}

bool is_full(queue_t *q) {
    // Returns true if the queue is full, false otherwise.
    if (q->rear == MAX_QUEUE_SIZE - 1){
        return true;
    }
    else{
        return false;
    }
}

int size(queue_t *q) {
    // Returns the number of elements in the queue.
    if (is_empty(q)){
        return 0;
    }
    else{
        return q->rear - q->front + 1;
    }
}

void enqueue(queue_t *q, hash_table_node_t* x) {
    // Adds an element to the rear of the queue.
    if (is_full(q)){
        printf("Queue is full. Cannot add element.\n");
    }
    else if (is_empty(q)){
        q->front = 0;
        q->rear = 0;
        q->data[q->rear] = x;
    }
    else{
        q->rear++;
        q->data[q->rear] = x;
    }
}

hash_table_node_t* dequeue(queue_t *q) {
    // Removes and returns the element at the front of the queue.
    if (is_empty(q)){
        printf("Queue is empty. Cannot remove element.\n");
        return -1;
    }
    else if (q->front == q->rear){
        int temp = q->data[q->front];
        q->front = -1;
        q->rear = -1;
        return temp;
    }
    else{
        int temp = q->data[q->front];
        q->front++;
        return temp;
    }
}

hash_table_node_t* peek(queue_t *q) {
    // Returns the element at the front of the queue without removing it.
    if (is_empty(q)){
        printf("Queue is empty. Cannot peek.\n");
        return -1;
    }
    else{
        return q->data[q->front];
    }
}

#endif
