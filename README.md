# Employee Database CLI

This project is a simple command-line tool written in C that stores and manages employee information in a custom binary file.

## Features

-   Create and initialize a new database file.
-   Add new employees (name, address, hours) to the database.
-   List all employees in the database.
-   Search for employees by name.
-   Update the working hours of an existing employee.
-   Delete an employee from the database.
-   Manage cases where multiple employees have the same name.

## Compiling the Project

Run the makefile script

```bash
make
```

This command will create an executable file named `dbview` in `/bin/` and also creates a db file named `mynewdb.db`.

## Usage

**NOTE:** The database file name must be specified with the `-f` parameter for all operations.

### Create a New Database

Creates a new database file and writes the header information.

```bash
./bin/dbview -n -f <database_file>
```

### List Employees

Lists all employees in the database.

```bash
./bin/dbview -f <database_file> -l
```

### Add an Employee

Adds a new employee to the database. Employee information must be provided in quotes and separated by commas.

```bash
./bin/dbview -f <database_file> -a "Name,Address,Hours"
```
**Example:**
```bash
./bin/dbview -f mynewdb.db -a "John Doe,123 Main St,40"
```

### Search for an Employee

Searches for and lists employees with the specified name.

```bash
./bin/dbview -f <database_file> -s <employee_name>
```

### Update an Employee's Hours

Searches for the specified employee and updates their working hours to the new value. If multiple employees have the same name, it will ask which one to update.

```bash
./bin/dbview -f <database_file> -s <employee_name> -u <new_hour_value>
```

### Delete an Employee

Searches for the specified employee and removes them from the database. If multiple employees have the same name, it will ask which one to delete.

```bash
./bin/dbview -f <database_file> -s <employee_name> -d
```

## File Format

The database uses a custom binary format:
1.  **Header (`dbheader_t`):** Located at the beginning of the file. It contains information such as a magic number, version, total employee count, and file size.
2.  **Employee Records (`employee_t`):** Immediately following the header, employee records are stored