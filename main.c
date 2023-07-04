//
// Created by Russ Sobti on 4/21/23.
//

#include <unistd.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
// Struct that represents an inode
struct inode_entry
{
    int inode_number;
    char type;
    struct entry *entries;
    int num_entries;
};

// Struct that represents an entry in the inode file
struct entry
{
    int reference_inode;
    char name[32]; // store the name of the file
};

// Utility Function
char *uint32_to_str(uint32_t i)
{
    int length = snprintf(NULL, 0, "%lu", (unsigned long)i); // pretend to print to a string to determine length
    char *str = calloc(1, length + 1);                       // allocate space for the actual string
    snprintf(str, length + 1, "%lu", (unsigned long)i);      // print to string
    return str;
}

int ls(int cwd, struct inode_entry *inodes_list)
{
    // get the inode of the current directory
    struct inode_entry *current_directory = &inodes_list[cwd];
    // loop through the entries in the current directory
    for (int i = 0; i < current_directory->num_entries; i++)
    {
        // print the name of the entry
        printf("%d %s\n", current_directory->entries[i].reference_inode, current_directory->entries[i].name);
    }
    return cwd;
}
int cd(int cwd, struct inode_entry *inodes_list, char *directory)
{
    // get the inode of the current directory
    struct inode_entry *current_directory = &inodes_list[cwd];
    // loop to the entries in the current directory
    for (int i = 0; i < current_directory->num_entries; i++)
    {
        // check if the entry is the directory we are looking for
        if (strcmp(current_directory->entries[i].name, directory) == 0)
        {
            uint32_t target = current_directory->entries[i].reference_inode;
            // check if the entry is a directory
            if (inodes_list[target].type != 'd')
            {
                printf("Error: Not a directory\n");
                return cwd;
            }
            // return the inode number of the directory
            return current_directory->entries[i].reference_inode;
        }
    }
    return 0;
}
int mkdir(int cwd, struct inode_entry *inodes_list, char *directory, int num_inodes)
{
    // get the appropriate index for the new inode
    int inode_index = num_inodes;
    num_inodes++;
    // create the new directory
    struct inode_entry *new_directory = &inodes_list[inode_index];
    // set the inode number
    new_directory->inode_number = inode_index;
    // set the type
    new_directory->type = 'd';
    // set the number of entries
    new_directory->num_entries = 2;
    // malloc the entries
    new_directory->entries = calloc(1, sizeof(struct entry) * 2);
    // create the . entry
    struct entry *dot_entry = &new_directory->entries[0];
    // set the reference inode
    dot_entry->reference_inode = inode_index;
    // set the name
    strncpy(dot_entry->name, ".", 32);
    // create the .. entry
    struct entry *dotdot_entry = &new_directory->entries[1];
    // set the reference inode
    dotdot_entry->reference_inode = cwd;
    // set the name
    strncpy(dotdot_entry->name, "..", 32);
    // add the new directory to inode list
    inodes_list[inode_index] = *new_directory;

    // get the inode of the current directory
    struct inode_entry *current_directory = &inodes_list[cwd];
    // add the new directory to the current directory
    current_directory->entries = realloc(current_directory->entries, sizeof(struct entry) * (current_directory->num_entries + 1));
    // create the new entry
    struct entry *new_entry = &current_directory->entries[current_directory->num_entries];
    // set the reference inode
    new_entry->reference_inode = inode_index;
    // set the name
    strncpy(new_entry->name, directory, 32);
    // increment the number of entries
    current_directory->num_entries++;
    // return the cwd
    return cwd;
}

int touch(int cwd, struct inode_entry *inodes_list, char *file, int num_inodes)
{
    // get the appropriate index for the new inode
    int inode_index = num_inodes;
    num_inodes++;
    // create the new directory
    struct inode_entry *new_inode = &inodes_list[inode_index];
    // set the inode number
    new_inode->inode_number = inode_index;
    // set the type
    new_inode->type = 'f';
    // set the number of entries
    new_inode->num_entries = 1;
    // malloc the entries
    new_inode->entries = calloc(1, sizeof(struct entry));
    // create the entry for the file, which has the filename as the name
    struct entry *file_entry = &new_inode->entries[0];
    // set the reference inode
    file_entry->reference_inode = inode_index;
    // set the name
    strncpy(file_entry->name, file, 32);
    // add the new file to inode list
    inodes_list[inode_index] = *new_inode;
    // get the inode of the current directory
    struct inode_entry *current_directory = &inodes_list[cwd];
    // add the new file to the current directory
    current_directory->entries = realloc(current_directory->entries, sizeof(struct entry) * (current_directory->num_entries + 1));
    // create the new entry
    struct entry *new_entry = &current_directory->entries[current_directory->num_entries];
    // set the reference inode
    new_entry->reference_inode = inode_index;
    // set the name
    strncpy(new_entry->name, file, 32);
    // increment the number of entries
    current_directory->num_entries++;
    // return the cwd
    return cwd;
}
int export_fs(struct inode_entry *inodes_list)
{
    // find out how many inodes there are
    int num_inodes = 0;
    while (1)
    {
        if (inodes_list[num_inodes].type == 'f' || inodes_list[num_inodes].type == 'd')
        {
            num_inodes++;
        }
        else
        {
            break;
        }
    }
    // create the inodes_list file
    FILE *inodes_list_file = fopen("inodes_list", "wb");
    // write the inode numbers as int32 and a character representing the type
    for (int i = 0; i < num_inodes; i++)
    {
        fwrite(&inodes_list[i].inode_number, sizeof(uint32_t), 1, inodes_list_file);
        fwrite(&inodes_list[i].type, sizeof(char), 1, inodes_list_file);
    }

    // Create inode files
    for (int i = 0; i < num_inodes; i++)
    {
        // create the inode file
        char *inode_file_name = uint32_to_str(inodes_list[i].inode_number);
        FILE *inode_file = fopen(inode_file_name, "wb");
        free(inode_file_name);
        // Write to these files, based on whether they are a file or directory
        if (inodes_list[i].type == 'd')
        {
            for (int j = 0; j < inodes_list[i].num_entries; j++)
            {
                // write the reference inode
                fwrite(&inodes_list[i].entries[j].reference_inode, 4, 1, inode_file);
                char name[32];
                strncpy(name, inodes_list[i].entries[j].name, 32);
                // write the name
                fwrite(name, 32, 1, inode_file);
            }
        }
        else
        {
            // create a string of max size 32 to hold the name
            char name[32];
            strncpy(name, inodes_list[i].entries[0].name, 32);
            // write the name
            for (int j = 0; j < 32; j++)
            {
                // check if the character is null
                if (name[j] == '\0')
                {
                    // write the null character
                    fwrite(&name[j], 1, 1, inode_file);
                    break;
                }
                // write the character
                fwrite(&name[j], 1, 1, inode_file);
            }
        }
        // free the inode entries
        free(inodes_list[i].entries);
        // close the file
        fclose(inode_file);
    }
    // close the file
    fclose(inodes_list_file);
    // free the inodes list
    free(inodes_list);
    return 0;
}

int main(int argc, char *argv[])
{
    // Check if the number of arguments is correct
    if (argc != 2)
    {
        printf("Error: Incorrect number of arguments\n");
        return 1;
    }

    // check if the dir exists
    if (access(argv[1], F_OK) == -1)
    {
        printf("Error: Directory does not exist\n");
        return 1;
    }
    // change to the directory of interest
    chdir(argv[1]);
    // read the inode list from the file
    FILE *fp = fopen("inodes_list", "rb");
    if (fp == NULL)
    {
        printf("Error: Could not open file\n");
        return 1;
    }
    // read the number of inodes
    uint32_t num_inodes = 0;
    // malloc an array of inodes, with a max of 1024 inodes supported
    struct inode_entry *inodes_list = calloc(1, sizeof(struct inode_entry) * 1024);
    for (int i = 0; i < 1024; i++)
    {
        inodes_list[i].type = '\0'; // Assigning a default value, such as null character '\0'
        // Initialize other members of the Inode struct if necessary
        inodes_list[i].entries = NULL;
    }

    // create a pointer to store reads into
    int *temp_pointer = calloc(1, 5);
    while (fread(temp_pointer, 5, 1, fp))
    {

        inodes_list[num_inodes].inode_number = temp_pointer[0];
        inodes_list[num_inodes].type = temp_pointer[1];
        // increment the number of inodes
        num_inodes++;
    }
    fclose(fp);
    free(temp_pointer);
    // Now, we need to read the inode files and find their contents
    for (int i = 0; i < num_inodes; i++)
    {
        // First, convert the inode number to a string
        char *filename = uint32_to_str(inodes_list[i].inode_number);
        // open the file
        FILE *fp;
        fp = fopen(filename, "rb");
        free(filename);
        if (fp == NULL)
        {
            printf("Error: Could not open file\n");
            return 1;
        }
        // hold the number of entries
        uint32_t num_entries = 0;
        // malloc an array of entries
        struct entry *entries_list = calloc(1, sizeof(struct entry) * 1024);
        for (int k = 0; k < 1024; k++)
        {
            entries_list[k].name[0] = '\0'; // Assigning a default value, such as null character '\0'
            entries_list[k].reference_inode = -1;
        }
        if (inodes_list[i].type == 'd')
        {

            // create a pointer to store reads into
            int *temp_pointer = calloc(1, 36);
            // read the entries, 36 bytes at a time
            while (fread(temp_pointer, 36, 1, fp))
            {
                // add the entry to the list
                entries_list[num_entries].reference_inode = temp_pointer[0]; // first 32-bit integer is the inode number
                // next 32 bytes are the name of the file
                char *name = calloc(1, 32);
                name[0] = '\0';
                // copy the name of the file
                strncpy(name, (char *)&temp_pointer[1], 32);
                // assign the name to the entry
                strncpy(entries_list[num_entries].name, name, 32);
                //
                // increment the number of entries
                num_entries++;
                // reallocate the entry list
                entries_list = realloc(entries_list, sizeof(struct entry) * (num_entries + 1));
                // free the name
                free(name);
            }
            fclose(fp);
            free(temp_pointer);
        }
        else
        {
            // read 32 bytes
            int *temp_pointer = calloc(1, 36);
            fread(temp_pointer, 36, 1, fp);
            // add the entry to the list
            // inode number is the inode number
            entries_list[num_entries].reference_inode = i;
            // next 32 bytes are the name of the file
            entries_list[num_entries].name[0] = '\0'; // null terminate for safety with strncpy
            // set the name from temp_pointer
            strncpy(entries_list[num_entries].name, (char *)&temp_pointer[1], 32);
            free(temp_pointer);
        }
        // assign the entries list to the inode
        inodes_list[i].entries = entries_list;
        inodes_list[i].num_entries = num_entries;
    }

    int cwd = 0;

    while (1)
    {
        // Loop for user input
        char input[100];
        char command[100];
        char argument[100];
        input[0] = '\0';
        command[0] = '\0';
        argument[0] = '\0';
        printf("> ");
        fgets(input, 100, stdin);
        sscanf(input, "%s %s", command, argument);

        if (strcmp(command, "ls") == 0)
        {
            ls(cwd, inodes_list);
        }
        else if (strcmp(command, "cd") == 0)
        {
            cwd = cd(cwd, inodes_list, argument);
        }
        else if (strcmp(command, "mkdir") == 0)
        {
            cwd = mkdir(cwd, inodes_list, argument, num_inodes);
            num_inodes++;
        }
        else if (strcmp(command, "touch") == 0)
        {
            cwd = touch(cwd, inodes_list, argument, num_inodes);
            num_inodes++;
            //            printf("num inodes: %d\n", num_inodes);
        }
        else if (strcmp(command, "exit") == 0)
        {
            export_fs(inodes_list);
            break;
        }
        else
        {
            printf("Error: Invalid command\n");
            printf("command: %s\n", command);
        }
    }

    return 0;
}
