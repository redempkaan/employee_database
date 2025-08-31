#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("\nSUPPORTED FUNCTIONS:\n\nCREATE: %1$s -n -f <database_file> (Creates and opens a new db file named <database_file>)\n\nOPEN: %1$s -f <database_file> (Opens the specified db file and validates header)\n\nLIST: %1$s -f <database_file> -l (Lists the employees in the specified db file)\n\nADD: %1$s -f <database_file> -a <Employee_name,Employee_address,Employee_hours> (Adds the given employee to specified db file)\n\nSEARCH: %1$s -f <database_file> -s <employee_name> (Searches the employee in the specified db file and lists if it exists)\n\nUPDATE: %1$s -f <database_file> -s <employee_name> -u <new_employee_hour> (Searches the employee in the db file; if it exists, changes its hours value to the specified new value)\n\nDELETE: %1$s -f <database_file> -s <employee_name> -d (Searches the employee in the db file; if it exists, removes it)\n\n", argv[0]);
}

int main(int argc, char *argv[]) {
    int dbfd;
	int clf;
    char* filepath = NULL;
    char* addstring = NULL;
    char* name = NULL;
    int update_hour_value = 0;
    int *found_employees = NULL;
    bool list = false;
    bool newfile = false;
    bool delete = false;
    struct dbheader_t* dbhdr = NULL;
    struct employee_t* employees = NULL;

    while((clf = getopt(argc, argv, "nf:a:ls:u:d")) != -1){
        switch (clf){  
            case 'n': // New file flag set
                newfile = true;
                break;
            
            case 'f':
                filepath = optarg;
                break;
            
            case 'a': // Add string copied
                addstring = optarg;
                break;
            
            case 'l': // List flag set
                list = true;
                break;
            
            case 's': // The specified name copied
                name = optarg;
                break;
            
            case 'u': // New hour value copied
                update_hour_value = atoi(optarg);
                break;

            case 'd': // Deletion flag set
                delete = true;
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
        dbfd = create_db_file(filepath); // Creating new db file
            if(dbfd == STATUS_ERROR){
                return -1; 
            }
            else{
                if(create_db_header(dbfd, &dbhdr) == STATUS_ERROR){ // Creating new db header right after
                    return -1;
                }
                else if(validate_db_header(dbfd, &dbhdr) != STATUS_SUCCESS){  // And validate its header
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
        
        if(addstring){ // If addstring is set, reallocating the employee array and going to add function
            dbhdr -> count++;
            employees = realloc(employees, (dbhdr -> count) * sizeof(struct employee_t));
            add_employee(dbhdr, employees, addstring);
        }

        if(list){
            list_employees(dbhdr, employees);
        }

        if(name != NULL){ //Found employees' indexes is stored in an array as we can have two or more employees with the same name in a db file
            found_employees = find_employee(dbhdr, employees, name);
        }

        if(update_hour_value != 0 && found_employees != NULL){ // Before going to update function, search function is running in the upper block
            update_employee_hour(dbhdr, employees, name, found_employees, update_hour_value);
        }

        if(found_employees != NULL && delete){ // Again search function is running before delete function
            delete_employee(dbhdr, &employees, name, found_employees);
        }
    
        output_file(dbfd, dbhdr, employees); // Outputting the file
        
        
        

    return 0;
    }
