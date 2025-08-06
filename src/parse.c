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

}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {

}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {

}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
    if(fd < 0){
        printf("Invalid fd\n");
        return STATUS_ERROR;
    }
    dbhdr -> magic = htonl(dbhdr -> magic);
    dbhdr -> count = htons(dbhdr -> count);
    dbhdr -> version = htons(dbhdr -> version);
    dbhdr -> filesize = htonl(dbhdr -> filesize);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbhdr, sizeof(struct dbheader_t));

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

    if(header -> version != 0x1){
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
    header -> version = 0x1;
    header -> count = 0;
    header -> filesize = sizeof(struct dbheader_t);

    *headerOut = header;

    output_file(fd, header, employees);

    printf("Header creation was succesful\n");

    return STATUS_SUCCESS;
	
}


