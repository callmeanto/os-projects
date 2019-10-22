#include <stdio.h>
#include <dirent.h>

int main(void)
{
    struct dirent *de; // Pointer for directory entry
    DIR *FD;
    struct dirent *in_file;
    FILE *common_file;
    FILE *entry_file;
    char buffer[BUFSIZ];

    /* Openiing common file for writing */
    common_file = fopen("/home/callmeanto/Documents/TRIMESTRE VIII/Operativos/Taller/os-projects/output.txt", "w");
    if (common_file == NULL)
    {
        fprintf(stderr, "Error : Failed to open common_file");

        return 1;
    }

    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir("/home/callmeanto/Documents/TRIMESTRE VIII/Operativos/Taller/tarea-1");

    if (dr == NULL) // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
        return 0;
    }

    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        printf("%s\n", de->d_name);
        entry_file = fopen(de->d_name, "rw");
        if (entry_file == NULL)
        {
            fprintf(stderr, "Error : Failed to open entry file");
            fclose(common_file);

            return 1;
        }

        /* Doing some struf with entry_file : */
        /* For example use fgets */
        while (fgets(buffer, BUFSIZ, entry_file) != NULL)
        {
            fprintf(common_file, "reading file %s", de->d_name);
        }

        /* When you finish with the file, close it */
        fclose(entry_file);
    }

    closedir(dr);
    return 0;
}