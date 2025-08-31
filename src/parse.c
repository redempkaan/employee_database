#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
    int i;

    printf("    (NAME)       (ADDRESS)       (HOURS)\n");

    for(i = 0; i < dbhdr -> count; i++){
        printf("[%d] %s       %s       %d\n", i + 1, employees[i].name, employees[i].address, employees[i].hours);
    }

}

int same_name_counter(int *found_employees){
    int i = 1;

    while(found_employees[i] != 0){
        i++;
    }

    return i;
}

int delete_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *name, int *found_employees){

    int input_employee_id = 1;
    int same_name_count = same_name_counter(found_employees);
    int delete_employee_index;
    int elements_to_move;
    struct employee_t* employees_temp = *employees;
    struct employee_t* temp;

    if(same_name_count > 1){ // Counting employees with the same name
        printf("There are more than 1 employee with the given name, please specify index\n");
        scanf("%d", &input_employee_id); // Reading an index to select an employee to delete
    }

    if((input_employee_id > same_name_count) || (input_employee_id <= 0)){
        printf("Invalid index\n"); // Index control
        return STATUS_ERROR;
    }

    delete_employee_index = found_employees[input_employee_id - 1];
    elements_to_move = (dbhdr -> count) - delete_employee_index - 1;

    if(elements_to_move > 0){ // Counting employees that must be replaced
        memmove(&employees_temp[delete_employee_index], &employees_temp[delete_employee_index + 1], elements_to_move * sizeof(struct employee_t));
        
    }
    dbhdr -> count--;

    if((dbhdr -> count) > 0){
        temp = realloc(employees_temp, (dbhdr -> count) * sizeof(struct employee_t)); // Reallocating temp array
    }

    if(temp == NULL){
        printf("Error: Failed to shrink memory\n");
        dbhdr -> count++;
        return STATUS_ERROR;
    }
    else{
        *employees = temp; // Copying the reallocated array to the main array
    }

    printf("Employee %s has been deleted\n", name);

    return STATUS_SUCCESS;
}

int update_employee_hour(struct dbheader_t *dbhdr, struct employee_t *employees, char* name, int *found_employees, int value){

    int same_name_count= 1;
    int input_employee_id = 1;
    int old_hour_value;

    same_name_count = same_name_counter(found_employees); // Same name counting again

    if(same_name_count > 1){
        printf("There are more than 1 employee with the given name, please specify index\n");
        scanf("%d", &input_employee_id);
    }

    input_employee_id--;

    if(((input_employee_id - 1) >= same_name_count) || (input_employee_id < 0)){
        printf("Invalid index\n"); // Index control
        return STATUS_ERROR;
    }
    else{
        old_hour_value = employees[found_employees[input_employee_id]].hours;
        employees[input_employee_id].hours = value;
        printf("Employee %s's hour value has been changed (%d -> %d)\n", employees[found_employees[input_employee_id]].name, old_hour_value, value);
    }

    return STATUS_SUCCESS;
}

int* find_employee(struct dbheader_t* dbhdr, struct employee_t* employees, char* name){
    int i;
    int fe_index = 0;

    int* found_employees;

    found_employees = (int *) calloc(dbhdr -> count, sizeof(int));

    for(i = 0; i < (dbhdr -> count); i++){

        if(!strcmp(employees[i].name, name)){
            found_employees[fe_index++] = i; // Storing the found employees with the given name's indexes
        }
    }
    

    if(fe_index > 0){
        printf("Found %d employees:\n", fe_index);
        printf("    (NAME)       (ADDRESS)       (HOURS)\n");


    for(i = 0; i < fe_index; i++){
        printf("[%d] %s       %s       %d\n", i + 1, employees[found_employees[i]].name, employees[found_employees[i]].address, employees[found_employees[i]].hours);

    }
        return found_employees;
    }

    else{
        printf("Employee %s Not found\n", name);

        return NULL;
    }

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

    char* name = strtok(addstring, ",");

    char* addr = strtok(NULL, ",");

    char* hours = strtok(NULL, ",");

    strncpy(employees[dbhdr -> count - 1].name, name, sizeof(employees[dbhdr -> count - 1].name));

    strncpy(employees[dbhdr -> count - 1].address, addr, sizeof(employees[dbhdr -> count - 1].address));

    employees[dbhdr -> count - 1].hours = atoi(hours);

    printf("Added employee: %s %s %u\n", employees[0].name, employees[0].address, employees[0].hours);



    return STATUS_SUCCESS;

}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
    int e_count = dbhdr -> count;
    int i;

    struct employee_t* employees = calloc(e_count, sizeof(struct employee_t));

    if(employees == -1){
        printf("Malloc failed\n");
        return STATUS_ERROR;
    }

    read(fd, employees, e_count * sizeof(struct employee_t)); // Reading the employees from the file descriptor to a employees array

    for(i = 0; i < e_count; i++){
        employees[i].hours = ntohl(employees[i].hours); // Unpacking the hours value
    }

    *employeesOut = employees;

    return STATUS_SUCCESS;


}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    int i, realcount, realfilesize;

    if(fd < 0){
        printf("Invalid fd\n");
        return STATUS_ERROR;
    }

    realcount = dbhdr -> count;
    realfilesize = sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount);

    // Packing all header values to provide endian-safeness
    dbhdr -> magic = htonl(dbhdr -> magic);
    dbhdr -> filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr -> count = htons(dbhdr -> count);
    dbhdr -> version = htons(dbhdr -> version);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbhdr, sizeof(struct dbheader_t));

    for(i = 0; i < realcount; i++){ // Packing employees hours also
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    if(ftruncate(fd, realfilesize) == -1){ // Truncating the db file
        perror("Truncating failed\n");
    }


    return STATUS_SUCCESS;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {

    if(fd < 0){
        printf("Invalid fd\n");
        return STATUS_ERROR;
    }

    struct dbheader_t* header;

    header = calloc(1, sizeof(struct dbheader_t));
    if(header == -1){
        printf("Memory allocation for header failed\n");
        return STATUS_ERROR;
    }
    
    lseek(fd, 0, SEEK_SET); // Sets the file indicator position to beginning of the file
    
    if(read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)){
        perror("header read");
        free(header);
        return STATUS_ERROR;
    }
    // Unpacking all header values
    header -> version = ntohs(header -> version);
    header -> filesize = ntohl(header -> filesize);
    header -> magic = ntohl(header -> magic);
    header -> count = ntohs(header -> count);

    if(header -> version != VERSION){
        printf("Improper header version\n");
        free(header);
        return STATUS_ERROR;
    }

    if(header -> magic != HEADER_MAGIC){
        printf("Invalid file type (magic)\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat;

    fstat(fd, &dbstat); // Checking if the file size in header matches with the actual file size of the db file

    if(header -> filesize != dbstat.st_size){
        printf("Corrupted db file\n");
        free(header);
        return STATUS_ERROR;
    } 

    *headerOut = header;

    printf("Header validaton was successful\n");
    return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
    struct dbheader_t* header;
    struct employee_t* employees = NULL;

    header = calloc(1, sizeof(struct dbheader_t));

    if(header == -1){
        printf("Malloc failed to create header\n");
        return STATUS_ERROR;
    }

    header -> magic = HEADER_MAGIC;
    header -> version = VERSION;
    header -> count = 0;
    header -> filesize = sizeof(struct dbheader_t);

    *headerOut = header;

    output_file(fd, header, employees);

    printf("Header creation was successful\n");

    return STATUS_SUCCESS;
	
}


