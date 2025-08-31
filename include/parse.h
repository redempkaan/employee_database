#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x45444844
#define VERSION 0x1

struct dbheader_t {
	unsigned int magic; // Password like special value, indicates that this is a db file
	unsigned short version; // Version of the db app, to prevent corruption while working with later versions
	unsigned short count; // Total employee count
	unsigned int filesize; // Total file size
};

struct employee_t {
	char name[256];
	char address[256];
	unsigned int hours;
};

int create_db_header(int fd, struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut);
int read_employees(int fd, struct dbheader_t *, struct employee_t **employeesOut);
int output_file(int fd, struct dbheader_t *, struct employee_t *employees);
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring);
int* find_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *name);
int update_employee_hour(struct dbheader_t *dbhdr, struct employee_t* employees, char* name, int *found_employees ,int value);
int delete_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *name, int *found_employees);
int same_name_counter(int *found_employees);


#endif
