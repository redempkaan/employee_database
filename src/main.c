#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database_file>\n", argv[0]);
}

int main(int argc, char *argv[]) {
    int dbfd;
	int clf;
    char* filepath = NULL;
    bool newfile = false;
    struct dbheader_t* dbhdr = NULL;
    struct employee_t* employees = NULL;

    while((clf = getopt(argc, argv, "nf:")) != -1){
        switch (clf){
            case 'n':
                newfile = true;
                break;
            
            case 'f':
                filepath = optarg;
                break;

            case '?':
                printf("Unknown flag\n");
                break;
        }
    }

    if(filepath == NULL){
        printf("No filepath specified\n");
        print_usage(argv);
        return -1;
    }

    printf("DB File : %s\n", filepath);

    if(newfile == true){
        dbfd = create_db_file(filepath);
            if(dbfd == STATUS_ERROR){
                return -1; 
            }
            else{
                if(create_db_header(dbfd, &dbhdr) == STATUS_ERROR){
                    return -1;
                }
                else if(validate_db_header(dbfd, &dbhdr) != STATUS_SUCCESS){
                    return -1;
                    }
                }
                printf("Header validation completed\n");
            }
    
            //output_file(dbfd, dbhdr);
    return 0;
    }
