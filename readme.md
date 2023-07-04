# File Representation
The inode file corresponding to a plain file (type f) in your simulated filesystem should contain the full file name stored as ASCII text, followed by a single newline character. The following is an example of the content of inode file 4, representing a file with name file.tex:

amigler@csc$ xxd -c 36 4
00000000: 6669 6e61 6c2e 7465 780a                                                                   final.tex.

## Requirement 1
Your program must take one command-line argument (a string) specifying the name of a directory that holds the simulated file system (referenced as the file system directory below). Assuming that fs is such a directory, your program may be run as follows:
```shell
fs_simulator fs
```
Your program must verify that the specified directory actually exists.



## Requirement 2
Your program must load, from the file system directory (consider using chdir once for this), the inodes_list file. This file includes an entry for each inode currently in use. Each such entry will include the inode number (of type uint32_t, as within a directory) and an indicator (a single character, d or f) of the file type. These entries are stored as the binary representations of each piece of data, so you will want to use read/fread to extract the data from the file. Verify that these indicators are valid. Any inode numbers outside of the range from 0 to the maximum (exclusive) or with invalid indicators should be reported as invalid and ignored.



## Requirement 3
With the simulation initialized, the user will start in the directory at inode 0. If this inode is not a directory (or is not in use), then the program should report an error and terminate. The program will need to track the current working directory (initially 0).



## Requirement 4
Your program must take user commands as input from stdin. The commands and their behavior are listed below. If the user attempts to run any other command, or if a command is provided with the wrong number of arguments, then report an appropriate error and wait for the next command.

Limit: The use of file names in the commands discussed below must be restricted in length to properly interact with the represented directories. In particular, if your program allows a user to enter an arbitrarily long name (e.g., when executing mkdir), then it will need to truncate the name to the maximum length of 32 (specified earlier).

exit - (or EOF; ^D in Unix). Save the state of the inode list (see the Requirement 5) and exit the program.
cd <name> - Change to the specified directory (by name) from the current directory. The name must be given. Your program does not need to support paths (i.e., cd lab2 is valid, but cd lab2/task1 is not). If the specified name is not a directory (does not exist or is a regular file), then report the error and do not change the current working directory.
ls - List the contents (names and inodes) of the current working directory.
mkdir <name> - Create a new directory (in the current directory) with the given name. Doing so will require that there is an available inode and that the contents of this directory are updated. Moreover, the file corresponding to this new inode will need to be created with the default entries of . and ... Report an error and continue if there is already an entry in the current directory with this name.
touch <name> - Create a regular file (in the current directory) with the given name. Doing so will require that there is an available inode and that the contents of this directory are updated. The real file corresponding to this new inode will need to be created; for simulation/debugging purposes the name of the file should be written to the real file (corresponding to the inode number). If there is already an entry with this name, then this command will do nothing.


## Requirement 5
On exit, your program must update the inodes_list file in the file system directory to reflect any modifications to the inodes.



## Utility Function
It is likely that your program will need to convert an integer into a string. There are a few ways to do this (especially given that the maximum number of digits that will be printed for this program is known to be small), but the following is a common approach.
```c


char *uint32_to_str(uint32_t i)
{
int length = snprintf(NULL, 0, "%lu", (unsigned long)i);       // pretend to print to a string to determine length
char* str = checked_malloc(length + 1);                        // allocate space for the actual string
snprintf(str, length + 1, "%lu", (unsigned long)i);            // print to string

return str;
}
```

# Deliverables
Source Code - Push all relevant source code and an appropriate Makefile to your repository.
