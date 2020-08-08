#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//a wrapper structer to record input buffer
typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

//initialize a new inputbuffer
InputBuffer* new_input_buffer(){
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    input_buffer->buffer=NULL;
    input_buffer->buffer_length=0;
    input_buffer->input_length=0;

    return input_buffer;
}

//print prompt function
void print_prompt(){
    printf("db > ");
}

//get new input line
void read_input(InputBuffer* input_buffer){
    //call getline to record new line, and store line and buffer size into input_buffer
    ssize_t bytes_read= getline(&(input_buffer->buffer), &(input_buffer->buffer_length),stdin);

    //in case of wrong input, exit
    if (bytes_read<=0){
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    //ignore trailing newline
    input_buffer->input_length=bytes_read-1;
    input_buffer->buffer[bytes_read-1]=0;
}

//free allocated space
void close_input_buffer(InputBuffer* input_buffer){
    free(input_buffer->buffer);
    free(input_buffer);
}


//main function, a infinit loop to show prompt, get a input line, and process the line
int main(int argc, char* argv[]){
    //initialize input buffer
    InputBuffer* input_buffer=new_input_buffer();

    //infinit loop until input is '.exit'
    //loop is used to show prompt, get a new input line, and process the line
    while (1) {
        //print prompt
        print_prompt();
        //get a new input line, and store that in the input buffer
        read_input(input_buffer);

        //exit with input of '.exit'
        if(strcmp(input_buffer->buffer,".exit")==0) {
            //free the space allocated
            close_input_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        }
        //otherwise, process the input line
        else{
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);
        }
    }
}