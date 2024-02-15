#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define HEADER_SIZE 22

int main(int argc, char *argv[]){

   long delay;
   long volume_scale;
   int index;
   short header[HEADER_SIZE];
   int read_error;
   char *endptr; //Pointer for strtol
   int parameter; //Return value for optind
 
   if (argc < 5 || argc > 7){
       perror("Wrong amount of command line inputs!");
       return 1;
    }
 
   int check = 0;
 
   while((parameter = getopt(argc, argv, "d: v: ")) != -1){
       switch(parameter){
           case 'd':
               if (optarg != NULL) {
                   delay = strtol(optarg,&endptr,10);
                   if (optarg[0] == '-') {
                        check = 1;
                       if (*endptr != '\0'){
                           delay = 8000;
                           optind--;
                       }
                   } else if (*endptr != '\0'){
                        delay = 8000;
                    }
                   if (delay <= 0){
                       delay = 8000;
                   }
               } else {
                   delay = 8000;
                   optind--;
               }
               break;
           case 'v':
               if (optarg != NULL) {
                   volume_scale = strtol(optarg,&endptr,10);
                   if (*endptr != '\0'){
                       volume_scale = 4;
                       if (check == 1){
                            optind--;
                       }
                   }
                   if (volume_scale <= 0){
                       volume_scale = 4;
                   }
               } else {
                   volume_scale = 4;
               }
               break;
           case '?':
               perror("Invalid command");
               return 1;
       }
   }

   short *echo= malloc(sizeof(short)*delay);

   index = optind;
   FILE *input_file = fopen(argv[index],"rb");
   if(input_file == NULL){
        perror("File couldn't be opened");
       return 1;
   }

   index++;
   FILE *output_file = fopen(argv[index], "wb");
   if(output_file == NULL){
       perror("File couldn't be opened");
       return 1;
   }

   read_error = fread(header, sizeof(short), HEADER_SIZE, input_file);
   if(read_error != HEADER_SIZE){
       perror("Problem while reading the file");
       return 1;
   }

   unsigned int *sizeptr = (unsigned int *)(header + 2);
   *sizeptr = *sizeptr + (delay * 2);
   sizeptr = (unsigned int *)(header -2);
   sizeptr = (unsigned int *)(header + 20);
   *sizeptr = *sizeptr + (delay * 2);

   size_t read_write;
   read_write = fwrite(header, sizeof(short), HEADER_SIZE, output_file);
   if(read_write != HEADER_SIZE){
       perror("Problem while writing the file");
       return 1;
   }

   short value;
   short value2;
   short value3;
   int i = 0;
   int error;
   while(i < delay && fread(&value, sizeof(short), 1, input_file)>0){
        echo[i] = value/volume_scale;
        error = fwrite(&value, sizeof(short), 1, output_file);
        if (error != 1){
            perror("Problem while writing the file");
            return 1;
        }
        i++;
   }
   if(i != delay){
        for(int g = 0; g < (delay-i); g++){
            value3 = 0;
            error = fwrite(&value3, sizeof(short), 1, output_file);
            if (error != 1){
                perror("Problem while writing the file");
                return 1;
            }
        }
        for(int s = 0; s<i; s++){
            value3 = echo[s];
            error = fwrite(&value3, sizeof(short), 1, output_file);
            if (error != 1){
                perror("Problem while writing the file");
                return 1;
            }
        }
   }else{
        int j = 0;
        size_t byte_read;
        while((byte_read = fread(&value, sizeof(short),1,input_file))>0){
            value2 = echo[j] + value;
            error = fwrite(&value2, sizeof(short), 1, output_file);
            if (error != 1){
                perror("Problem while writing the file");
                return 1;
            }
            echo[j] = value/volume_scale;
            j++;
            if(j == delay){
                j = 0;
            }
        }
        for(int k = j; k < delay; k++){
            value = echo[k];
            error = fwrite(&value, sizeof(short),1, output_file);
            if (error != 1){
                perror("Problem while writing the file");
                return 1;
            }
        }
        for(int h = 0; h < j; h++){
            value = echo[h];
            error = fwrite(&value, sizeof(short),1, output_file);
            if (error != 1){
                perror("Problem while writing the file");
                return 1;
            }
        }
    }

   fclose(input_file);
   fclose(output_file);
   free(echo);
   return 0;
}
