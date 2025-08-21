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

    printf("(NAME)       (ADDRESS)       (HOURS)\n");

    for(i = 0; i < dbhdr -> count; i++){
        printf("[%d] %s       %s       %d\n", i + 1, employees[i].name, employees[i].address, employees[i].hours);
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

    read(fd, employees, e_count * sizeof(struct employee_t));

    for(i = 0; i < e_count; i++){
        printf("R: %s %s\n", employees[i].name, employees[i].address);
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;

    return STATUS_SUCCESS;


}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    int i, realcount;

    if(fd < 0){
        printf("Invalid fd\n");
        return STATUS_ERROR;
    }

    realcount = dbhdr -> count;

    dbhdr -> magic = htonl(dbhdr -> magic);
    dbhdr -> filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
    dbhdr -> count = htons(dbhdr -> count);
    dbhdr -> version = htons(dbhdr -> version);
    

    lseek(fd, 0, SEEK_SET);

    write(fd, dbhdr, sizeof(struct dbheader_t));

    for(i = 0; i < realcount; i++){
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    return 1;
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
    
    lseek(fd, 0, SEEK_SET);
    
    if(read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)){
        perror("header read");
        free(header);
        return STATUS_ERROR;
    }

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

    fstat(fd, &dbstat);

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


