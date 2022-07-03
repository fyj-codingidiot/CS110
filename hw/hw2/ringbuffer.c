#include<stdio.h>
#include "ringbuffer.h"

/* Create a new ring buffer. You should allocate memory for its content,
   initialize the read_pos, write_pos and capacity properly. 
   And finally return a pointer to the new ring buffer. */
ring_buffer* ring_buffer_new(){
        size_t i;
        ring_buffer* rb = (ring_buffer*)malloc(sizeof(ring_buffer));
        if(rb == NULL) return NULL;
        rb->capacity = RING_BUFFER_INIT_SIZE; /* init */
        rb->read_pos = 0; /* read_pos is front */
        rb->write_pos = 0; /* write_pos is rear */
        rb->content = (int*)malloc(sizeof(int)*(RING_BUFFER_INIT_SIZE + 1));
        for(i = 0;i<rb->capacity+1;i++){
            rb->content[i] = 0x3f3f3f3f; /* give inf to each val of content[i] */
        }
        return rb;
}
/* destroy the content */
static void destroy_content(ring_buffer* buffer){
    free(buffer->content);
    buffer->content =NULL;
}
/* Destroy the given ring buffer, free all resources you've allocated.
   Finally you should set the buffer pointer to NULL, that is why we pass
   ring_buffer** as parameter, instead of ring_buffer*. */
void ring_buffer_destroy(ring_buffer** buffer){
    if(buffer==NULL) return;
    if((*buffer)==NULL) return; /* judge corner cases */
    if((*buffer)->content==NULL){ /*judge for another corner cases */
        free((*buffer));
        *buffer = NULL;   /* set it to NULL */
    } 
    else{
        free((*buffer)->content);
        (*buffer)->content = NULL; /* set the ptr to NULL */
        free(*buffer);
        *buffer = NULL; 
    }
}
/* Get the size of the ring_buffer */
static size_t size(const ring_buffer* buffer){
    size_t size = (buffer->write_pos-buffer->read_pos+buffer->capacity+1) % (buffer->capacity+1); /*calculate the size */
    return size;
}
/* Returns true if the given ring buffer is empty, false otherwise */
bool ring_buffer_is_empty(const ring_buffer* buffer){
    size_t st;
    if(buffer == NULL) return false; /* judge for corner cases */
    st = size(buffer);
    if(st==0){ /* judge whether it's empty */
        return true;
    }
    else{
        return false; /* return false otherwise */
    }
}
/* Returns true if the given ring buffer is full,false otherwise */
static bool ring_buffer_is_full(const ring_buffer* buffer){
    size_t st=size(buffer);
    if(st == buffer->capacity){ /* judge whether it's full */
        return true;
    }
    else{
        return false; /* return false otherwise */
    }
}
/* Read an element from the ring buffer (i.e., the dequeue operation), *data records 
   the element popped. returns true if the operation succeeded, false otherwise. */
bool ring_buffer_read(ring_buffer* buffer, int* data){
    if(buffer==NULL || data == NULL) return false; /*judge for coner cases */
    if(ring_buffer_is_empty(buffer)){  /* judge whether it's empty */
        return false; 
    }
    else{
        (*data) = buffer->content[buffer->read_pos]; 
        buffer->content[buffer->read_pos] = 0x3f3f3f3f; /* pop it */
        buffer->read_pos = (buffer->read_pos+1)%(buffer->capacity+1); /* update read_pos */
        return true;
    }
    return false;
}
/* Write an element which value is `data` to the ring buffer (i.e., the enqueue operation),
   returns true if the operation succeeded, false otherwise. Note that when the buffer is 
   full, grow its size to make sure the element can be write successfully. The detailed 
   requirements of size growth are in the homework description. */
bool ring_buffer_write(ring_buffer* buffer, const int data){
    if(buffer ==NULL) return false; /*judge for corner cases */
    if(!ring_buffer_is_full(buffer)){
        buffer->content[buffer->write_pos] = data;
        buffer->write_pos = (buffer->write_pos+1) %(buffer->capacity+1); /*update write_pos */ 
        return true;
    }
    else{
        if(buffer->capacity<1024){ /*judging*/
            int index ;
            int* new_content = (int*)malloc(sizeof(int)*(buffer->capacity*RING_BUFFER_GROW_FACTOR1+1)); /* apply for more spaces */
            if(new_content==NULL) return false; /*judge for corner cases */
            else{
                size_t i; 
                for(i=0;i<buffer->capacity*RING_BUFFER_GROW_FACTOR1+1;i++){
                    new_content[i] = 0x3f3f3f3f;   /* given inf */
                }
                index=0;
                while(!ring_buffer_is_empty(buffer)){
                    int temp = 0;
                    ring_buffer_read(buffer,&temp); /* read one int */
                    new_content[index] = temp;
                    index+=1;    /*update*/
                }
                buffer->read_pos = 0; /* reintiliaze */
                buffer->write_pos = index+1;
                destroy_content(buffer);
                buffer->content = new_content;
                buffer->content[index] = data; /*finally*/
                buffer->capacity = buffer->capacity*RING_BUFFER_GROW_FACTOR1;
                return true;
            }
        }
        else if(buffer->capacity>=1024){  /*judging*/
            int index ;
            int* new_content = (int*)malloc(sizeof(int)*(buffer->capacity*RING_BUFFER_GROW_FACTOR2+1));/* apply for more spaces */
            if(new_content==NULL) return false;/*judge for coner cases */
            else{
                size_t i;
               for(i=0;i<buffer->capacity*RING_BUFFER_GROW_FACTOR2+1;i++){
                    new_content[i] = 0x3f3f3f3f; /* given inf */
                }
                index =0;
                while(!ring_buffer_is_empty(buffer)){
                    int temp = 0;
                    ring_buffer_read(buffer,&temp);/* read one int */
                    new_content[index] = temp;
                    index+=1;/*update*/
                }
                buffer->read_pos = 0;/* reintiliaze */
                buffer->write_pos = index+1;
                destroy_content(buffer);
                buffer->content = new_content;
                buffer->content[index] = data;/*finally*/
                buffer->capacity = buffer->capacity*RING_BUFFER_GROW_FACTOR2;
                return true;
            }
        }
    }
    return false;
}
/* Read "rdsize" elements from the ring buffer, and fill them to "data".
   Notice that you should only return true when this operation succeeds.
   If there are not enough elements in the buffer, just DO NOTHING and 
   return false. DO NOT modify the ring buffer and data for such cases. */
bool ring_buffer_read_multi(ring_buffer* buffer, size_t rdsize, int* data){
    size_t st ; /* get size */
    if(buffer == NULL || data == NULL) return false;/* judge for coner cases */
    st = size(buffer);
    if(st<rdsize) return false;
    else if(ring_buffer_is_empty(buffer) || rdsize == 0) return false;/* judge for coner cases */
    else{
    size_t i;
    for(i = 0;i<rdsize;i++){
        int temp = 0;
        ring_buffer_read(buffer,&temp); /* update */
        data[i] = temp;
    }
    return true; /* return true if it success */
    }
    return false;
}
/* Write "wrtsize" elements to the ring buffer. The values to write are 
   provided in "data". Returns true if succeeds, false otherwise. Don't 
   forget to grow your buffer size properly. */
bool ring_buffer_write_multi(ring_buffer* buffer, size_t wrtsize, const int* data){
    if(buffer == NULL || data == NULL || wrtsize == 0) return false; /* judge for coner cases */
    else {
        size_t i;
        for(i=0;i<wrtsize;i++){ /*begin*/
            int temp= data[i]; 
            ring_buffer_write(buffer,temp); /* write one int to rb */
        }
        return true; /* return true if success */ 
    }
    return false;
}
/* For every element in the ring buffer, apply "func" to it. For example,
   Your buffer currently contains 3 elements 1, 2, 4. If the map function is to 
   "add five", then their values should be 6, 7, 9 after calling this function. */
bool ring_buffer_map(ring_buffer* buffer, map_func func){
     size_t i;
    if(buffer == NULL || func == NULL) return false; /* judge for coner cases */
    for(i=0;i<buffer->capacity+1;i++){
        if(buffer->content[i]!=0x3f3f3f3f){ /*judge whether it's empty */
            int temp = buffer->content[i];
            int val = func(temp); /* apply to func */
            buffer->content[i] = val;
        }
    }
    return true; /* return true if success */
}

