#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments");
        return 1;
    }

    DIR *input_dir = opendir(argv[1]);
    DIR *output_dir = opendir(argv[2]);
    struct dirent *strdir;
    FILE *in_file;
    FILE *out_file;
    char line[1000];
    char input_filepath[273];
    char output_filepath[273];
    char temp;
    if (input_dir == NULL) {
        printf("no input directory found");
        return 1;
    }
    
    if (output_dir == NULL) {
        printf("no output directory found");
        return 1;
    }
    
    while ((strdir = readdir(input_dir)) != NULL) {
        if (strlen(strdir->d_name) >= 4 && strcmp(strdir->d_name + strlen(strdir->d_name) - 4, ".txt") == 0){
        snprintf(input_filepath, sizeof(input_filepath), "%s/%s",argv[1], strdir->d_name);
        in_file = fopen(input_filepath, "r");
        if (in_file == NULL) {
            printf("no file found");
            return 1;
        }
        
        snprintf(output_filepath, sizeof(output_filepath), "%s/%s",argv[2], strdir->d_name);
        out_file = fopen(output_filepath, "w");
        if (out_file == NULL) {
            printf("cannot create output file");
            fclose(in_file);
            return 1;
        }
        
        while (fgets(line, sizeof(line), in_file) != NULL) {
        for (int i = 0; i < strlen(line)/2; i++){  
            temp = line[i];  
            line[i] = line[strlen(line) - i - 1];  
            line[strlen(line) - i - 1] = temp;  
    }  
            fputs(line, out_file);
        }
        
        fclose(in_file);
        fclose(out_file);
        }

    }
    
    printf("end\n");
    closedir(input_dir);
    return 0;
}
