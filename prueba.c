#include<stdio.h>
#include<string.h>
#include <dirent.h>

#define putc(_ch,_fp) _IO_putc (_ch, _fp)

struct tar_header{
    char name[100];
    char mode[8];
    char owner[20];
    char group[20];
    char size[12];
    char type[1];
};

void fexpand(FILE* f, size_t amount, int value){
    while( amount-- ){
        fputc( value, f );
    }
}

void tar_add(FILE* tar_file, const char* file, const char* internal_name){
    //Get current position; round to a multiple of 512 if we aren't there already
    size_t index = ftell( tar_file );
    size_t offset = index % 512;
    if( offset != 0 ){
        fexpand( tar_file, 512 - offset, 0);
    }
    //Store the index for the header to return to later
    index = ftell( tar_file );
    //Write some space for our header
    fexpand( tar_file, sizeof(struct tar_header), 0 );
    //Write the input file to the tar file
    FILE* input = fopen( file, "rb" );
    if( input == NULL ){
        fprintf( stderr, "Failed to open %s for reading\n", file);
        return;
    }
    //Copy the file content to the tar file
    while( !feof(input) ){
        char buffer[2000];
        size_t read = fread( buffer, 1, 2000, input );
        fwrite( buffer, 1, read, tar_file);
    }
    //Get the end to calculate the size of the file
    size_t end = ftell( tar_file );
    //Round the file size to a multiple of 512 bytes
    offset = end % 512;
    if( end != 0 ){
        fexpand( tar_file, 512 - offset, 0);
    }
    //Fill out a new tar header
    struct tar_header header;
    memset( &header, 0, sizeof( struct tar_header ) );
    snprintf( header.name, 100, "%s", internal_name  );
    snprintf( header.mode, 8, "%s", "rwx" );
    snprintf( header.owner, 20, "%s", "anto" ); 
    snprintf( header.group, 20, "%s", "anto" ); 
    snprintf( header.size, 12, "%011o", end - 512 - index );
    header.type[0] = '0';

    //Calculate the checksum
    size_t checksum = 0;
    int i;
    const unsigned char* bytes = &header;
    for( i = 0; i < sizeof( struct tar_header ); ++i ){
        checksum += bytes[i];
    }


    //Save the new end to return to after writing the header
    end = ftell(tar_file);

    //Write the header
    fseek( tar_file, index, SEEK_SET );
    fwrite( bytes, 1, sizeof( struct tar_header ), tar_file );

    //Return to the end
    fseek( tar_file, end, SEEK_SET );
    fclose( input );
}

void listdir(FILE* tar,const char *name, int indent)
{
    DIR *dir;
    struct dirent *entry;
    char buffer[2000];
    size_t read;
    FILE *holaEze;


    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 4)
        {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);          
            listdir(tar, path, indent + 2);
        }
        else
        {
            if (strcmp(entry->d_name, tar) == 0) continue;

            printf("%*s- %s\n", indent, "", entry->d_name);
            tar_add(tar, entry->d_name, entry->d_name); 
        }
 
    }
    closedir(dir);
}

int main( int argc, char* argv[] ){
    if( argc > 1 ){
        FILE* tar = fopen(argv[1], "wb" );
        char* inputDir = argv[2];

        if( !tar ){
            fprintf( stderr, "Failed to open %s for writing\n", argv[1] );
            return 1;
        }

        listdir(tar,argv[2],0);
        fexpand(tar, 1024, 0);
        fclose(tar);
        return 0;
    }
    fprintf( stderr, "Please specify some file names!\n" );
    return 0;
}