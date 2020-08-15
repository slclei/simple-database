#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//definations for row in the table
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct {
    uint32_t id;
    //additional one is for null
    char username[COLUMN_USERNAME_SIZE+1];
    char email[COLUMN_EMAIL_SIZE+1];
} Row;

//enums for success of failure for meta command
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

//enums for success of failure for prepare step (compiler)
typedef enum {
    PREPARE_SUCCESS,
    PREPAR_NEGATIVE_ID,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG
} PrepareResult;

//statement type, TODO
typedef enum{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

//Execute statement
typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

//statement struct, TODO
typedef struct{
    StatementType type;
    //row to be inserted
    Row row_to_insert;
} Statement;

//a wrapper structer to record input buffer
typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

//Store as many rows in page as possibile
//construct compact representation
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE= size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE=size_of_attribute(Row, email);
const uint32_t ID_OFFSET=0;
const uint32_t USERNAME_OFFSET=ID_OFFSET+ID_SIZE;
const uint32_t EMAIL_OFFSET=USERNAME_OFFSET+USERNAME_SIZE;
const uint32_t ROW_SIZE=ID_SIZE+USERNAME_SIZE+EMAIL_SIZE;

//define table structure pointing to pages of rows, and keep tracks of row number
const uint32_t PAGE_SIZE=4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE/ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct {
    uint32_t num_rows;
    void* pages[TABLE_MAX_PAGES];
} Table;

//initialize table
Table* new_table(){
    Table* table=malloc(sizeof(Table));
    table->num_rows=0;
    for (uint32_t i=0; i<TABLE_MAX_PAGES;i++){
        table->pages[i]=NULL;
    }
    return table;
}

//free table in memory
void free_table(Table* table){
    for (int i=0; i<TABLE_MAX_PAGES;i++){
        free(table->pages[i]);
    }
    free(table);
}

//function convert to the compact representation
void serialize_row(Row* source, void* destination){
    memcpy(destination+ID_OFFSET,&(source->id),ID_SIZE);
    memcpy(destination+USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination+EMAIL_OFFSET, &(source->email),EMAIL_SIZE);
}

//function convert from the compact representation
void deserialize_row(void* source, Row* destination){
    memcpy(&(destination->id), source+ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source+USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source+EMAIL_OFFSET, EMAIL_SIZE);
}

//find the address to read/write in memory for a row number
void* row_slot(Table* table, uint32_t row_num){
    //find the page number
    uint32_t page_num = row_num/ROWS_PER_PAGE;
    //get the page address
    void* page=table->pages[page_num];
    //allocate memory only when it is necessary
    if (page==NULL){
        page=table->pages[page_num]=malloc(PAGE_SIZE);
    }
    //get byte offset in the page
    uint32_t row_offset=row_num%ROWS_PER_PAGE;
    uint32_t byte_offset=row_offset*ROW_SIZE;

    //return target address
    return page+byte_offset;
}

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

//print row
void print_row(Row* row){
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
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
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    //in case of '.exit'
    if (strcmp(input_buffer->buffer,".exit")==0){
        close_input_buffer(input_buffer);
        free_table(table);
        exit(EXIT_SUCCESS);
    }
    //TODO
    //in different cases
    else{
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

//check if input will cause buffer overflow
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement){
    //change statement type to be insert
    statement->type = STATEMENT_INSERT;

    //parse insert commond into 4 sections for latter checking
    char* keyword=strtok(input_buffer->buffer, " ");
    char* id_string=strtok(NULL, " ");
    char* username=strtok(NULL, " ");
    char* email=strtok(NULL, " ");

    //in case of any field is null, return syntax code
    if(id_string==NULL || username==NULL || email==NULL){
        return PREPARE_SYNTAX_ERROR;
    }

    //convert id to int
    int id = atoi(id_string);
    //check if id is negtive
    if (id<0){
        return PREPAR_NEGATIVE_ID;
    }

    //check length for username and email
    if (strlen(username)>COLUMN_USERNAME_SIZE || strlen(email)>COLUMN_EMAIL_SIZE){
        return PREPARE_STRING_TOO_LONG;
    }

    //save each field into statement
    statement->row_to_insert.id=id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

//works as SQL Compiler, to parse command
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement){
    //in case of insert command
    if (strncmp(input_buffer->buffer,"insert", 6)==0){
        //use prepare_insert, instead of scanf, to avoid buffer overflow
        return prepare_insert(input_buffer, statement);
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
//exe insert function
ExecuteResult execute_insert(Statement* statement, Table* table){
    //check if table is full
    if (table->num_rows>=TABLE_MAX_ROWS){
        return EXECUTE_TABLE_FULL;
    }

    //get address of row to be inserted from statement
    Row* row_to_insert=&(statement->row_to_insert);

    //serialize row into end of table
    serialize_row(row_to_insert, row_slot(table, table->num_rows));

    //update row number
    table->num_rows+=1;

    return EXECUTE_SUCCESS;
}

//exe select function
ExecuteResult execute_select(Statement* statement, Table* table){
    //row to store rows tempor
    Row row;

    //a loop to print all row in the table
    for (uint32_t i=0; i<table->num_rows; i++){
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

//exe function to choose which function
ExecuteResult execute_statement(Statement* statement, Table* table){
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
        return execute_select(statement, table);
    }
}

//main function, a infinit loop to show prompt, get a input line, and process the line
int main(int argc, char* argv[]){
    //initialize input buffer
    InputBuffer* input_buffer=new_input_buffer();
    //initizlize table
    Table* table=new_table();

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
            switch (do_meta_command(input_buffer, table)){
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
            case (PREPAR_NEGATIVE_ID):
                printf("ID must be positive.\n");
                continue;
            case (PREPARE_STRING_TOO_LONG):
                printf("String is too long.\n");
                continue;
            case (PREPARE_SYNTAX_ERROR):
                printf("SYntax error. could not parse statement.\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword '%s' \n", input_buffer->buffer);
                continue;
        }

        //TODO exe actual command
        switch(execute_statement(&statement, table)){
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }
    }
}