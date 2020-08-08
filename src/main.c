#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//enums for success of failure for meta command
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

//enums for success of failure for prepare step (compiler)
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

//statement type, TODO
typedef enum{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

//statement struct, TODO
typedef struct{
    StatementType type;
} Statement;

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

//wrapper for more commands
//TODO
MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    //in case of '.exit'
    if (strcmp(input_buffer->buffer,".exit")==0){
        exit(EXIT_SUCCESS);
    }
    //TODO
    //in different cases
    else{
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

//works as SQL Compiler, to parse command
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement){
    //in case of insert command
    if (strncmp(input_buffer->buffer,"insert", 6)==0){
        statement->type=STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    //in case of select command
    if (strcmp(input_buffer->buffer,"select")==0){
        statement->type=STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    //return unrecognize otherwise
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

//works as virtal machine, to exe command
void execute_statment(Statement* statement){
    //accroding to result from compiler, exe in different manner
    //TODO with real action
    switch (statement->type){
        //TODO
        case (STATEMENT_INSERT):
            printf("for insert.\n");
            break;
        //TODO
        case (STATEMENT_SELECT):
            printf("for select.\n");
            break;
    }
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

        //for input starting with '.', deal with meta command
        if (input_buffer->buffer[0]=='.'){
            //process the input
            switch (do_meta_command(input_buffer)){
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s' \n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        //to exe with success compile, or go to next loop if not
        switch (prepare_statement(input_buffer, &statement)){
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword '%s' \n", input_buffer->buffer);
                continue;
        }

        //TODO exe actual command
        execute_statment(&statement);
        printf("Executed.\n");
    }
}