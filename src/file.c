#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"
int open_db_file(char *filename) {
    int fd;
    fd = open(filename, O_RDWR, 0644);

    if(fd == -1){
        return STATUS_ERROR;
    }
    else{
        return fd; 
    }
}


int create_db_file(char *filename) {
    int fd = open_db_file(filename);

    if(fd == STATUS_ERROR){
        fd = open(filename, O_RDWR | O_CREAT, 0644);
            if(fd != STATUS_ERROR){
               printf("Db file has been created\n"); 
            }
            else{
                printf("There is an issue about creating db file\n");
                return -1;
            }

    }
    else{
        printf("There is already a db file named <%s>, opened instead of creating a new one\n", filename);

        
    }
    return fd;
}


