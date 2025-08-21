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
    char* addstring = NULL;
    bool list = false;
    bool newfile = false;
    struct dbheader_t* dbhdr = NULL;
    struct employee_t* employees = NULL;

    while((clf = getopt(argc, argv, "nf:a:l")) != -1){
        switch (clf){
            case 'n':
                newfile = true;
                break;
            
            case 'f':
                filepath = optarg;
                break;
            
            case 'a':
                addstring = optarg;
                break;
            
            case 'l':
                list = true;
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
            }
    else{
        dbfd = open_db_file(filepath);

        if(validate_db_header(dbfd, &dbhdr) != STATUS_SUCCESS){
            printf("Failed to open %s", filepath);
            return -1;
        }

    }

        if(read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS){
            printf("Failed to read employees\n");
            return -1;
            }
        
        if(addstring){
            dbhdr -> count++;
            employees = realloc(employees, (dbhdr -> count) * sizeof(struct employee_t));
            add_employee(dbhdr, employees, addstring);
        }

        if(list){
            list_employees(dbhdr, employees);
        }
    
            output_file(dbfd, dbhdr, employees);

    return 0;
    }
