#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "mergesort.c"
#include "multiThreadSorter.h"
#include <stdio.h> 
#include <dirent.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

int lineSize = 1024;
int pathLength = 512;
int filterColumnIsAlpha = false;
char* filterColumn;
int sum = 0;

pid_t parent;

char* trimwhitespace(char* str);
void* sort(void *voidData);


void* search(void *voidData) {
	
    char** args = voidData;

	char* name;
	char* outdir;

	strcpy(name,args[0]);
	strcpy(outdir,args[1]); 

    DIR *dir;
    struct dirent *entry;

    /*
    struct LLNode* head = (struct LLNode*)malloc(sizeof(struct LLNode));
    struct LLNode* ptr = head;
    */

    if (!(dir = opendir(name)))
        return 0;

    while ((entry = readdir(dir)) != NULL) {
    	DIR *tstdir;

    	char * filePath = malloc(strlen(name)+strlen(entry->d_name)+2);
		filePath[0] = '\0';   // ensures the memory is an empty string
	    strcat(filePath,name);
	    strcat(filePath,"/");
	    strcat(filePath,entry->d_name);
	    //printf("File: %s\n", new_str);
    	tstdir = opendir(filePath);
    	pid_t cpid;
		if(tstdir){
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name); 
            //search(path);

            char* tmpdir = (char*) malloc(sizeof(outdir)+3); 
            if(strcmp(outdir, "") != 0){
            	strcat(tmpdir,"../");
            	strcat(tmpdir,outdir);
            }
            else
            	strcat(tmpdir,outdir);
            
            pthread_t id;
            sum += 1;
    			
			char args[2][pathLength];
			strcpy(args[0],path);
			strcpy(args[1],tmpdir);

			//int (*search_ptr)(void *) = &search;

			pthread_create(&id,NULL,&search,args);
			printf("%d,", (int)id);
			fflush(stdout);

        } 
        else {

        	char* fileName = entry->d_name;
    		int len = strlen(fileName);
			const char *last_four = &fileName[len-4];
        	if(len < 4) //file name not long enough to be csv
        		continue;

    		if(strcmp(last_four, ".csv") == 0 && fileName[0] != '.'){

            	pthread_t id;
            	sum += 1;
    			
				char* outPath = malloc(strlen(fileName)+7);
    			if(strcmp(outdir, "") != 0){ 
    				strcpy(outPath, outdir);
    			}
    			else
    				strcpy(outPath, filePath);
    			strcpy(&outPath[strlen(outPath)-4], "sorted.csv");

    			char args[2][pathLength];
    			strcpy(args[0],filePath);
    			strcpy(args[1],outPath);

    			//int (*sort_ptr)(void *) = &sort;

    			pthread_create(&id,NULL, &sort,args);
    			printf("%d,", (int)id);
    			fflush(stdout);
    		}
    		
        }
        /*
        ptr -> id = cpid;
        ptr -> next = (struct LLNode*)malloc(sizeof(struct LLNode)); 
        ptr = ptr -> next;
        */
        //free(tstdir);
    }

	closedir(dir);

	return NULL;
    /*
    while ((parent = wait(&status)) > 0);
    if(parent == getpid()){
    	while ((parent = wait(&status)) > 0);
    	printf("\nTotal number of processes: %d\n", count);
    }
    */
}

int main(int argc, char** argv) {

	parent = getpid();

	printf("Initial PID: %d\n", parent);
	
	if(argc < 3) {
		printf("Error: incorrect number of arguments\n");
		return 1;
	}

	int i;
	const char* indir = (char*) malloc(124*sizeof(char));
	const char* outdir = (char*) malloc(124*sizeof(char));

	indir = ".";
	outdir = "";

	for(i = 1; i<argc; i+=2) {
		if(strcmp(argv[i],"-c") == 0)
			filterColumn = strdup(argv[i+1]);
		
		if(strcmp(argv[i],"-d") == 0)
			//in directory
			//printf("indir: %s", argv[i+1]);
			indir = strdup(argv[i+1]);

		if(strcmp(argv[i],"-o") == 0)
			//out directory
			//printf("outdir: %s", argv[i+1]);
			outdir = strdup(argv[i+1]);

	}
	printf("PIDS of all child processes: ");
	fflush(stdout);
	char args[2][pathLength];
    strcpy(args[0],indir);
    strcpy(args[1],outdir);

	search(args);

	printf("\nTotal number of processes: %d\n", sum);

	return 0;
}
/////////////////////////////////////////////////////////////
int rowNums(char* filePath){
  int count = 0;
  FILE* fp;
  char line[lineSize];
  
  if((fp = fopen(filePath, "r")) == NULL) {
    printf("Error: cant open file: %s", filePath);
    return -1;
  }
  
  while(fgets(line, lineSize, fp) != NULL) {
    count++;
  }
  fclose(fp);
  
  return count - 1;
}

////////////////////////////////////////////////////////////

//int sort(char* filePath, char* fileName, char* filterColumn){
void* sort(void *voidData){
	
	char** args = voidData;
	char* inPath;
	char* outPath;

	strcpy(inPath,args[0]);
	strcpy(outPath,args[1]); 

	int filterIndex = -1;
	//number of rows in movie database
	int rownums = rowNums(inPath);
	//printf("fileName: %s, rownums: %d\n", inPath, rownums);
	
	char line[lineSize];

	int colnums = 0;
	
		//Read first line and figure out which header column to filter by
	FILE* fp;
	FILE* outFile;
	  
	fp = fopen(inPath, "r");
	outFile = fopen(outPath, "w+");

	if(!outFile){
	    printf("Error: out directory %s does not exist", outPath);
	    return NULL;
	}

	fgets(line, lineSize, fp);
  
	char* tmp = strdup(line);
	bool first = true;
	char* tok;

	while ((tok = strsep(&tmp, ",")) != NULL) {  
		
    	int p = 0;
		for(p=strlen(tok)-1; p>0; p--){
			if(tok[p] == '\n' || tok[p] == '\r')
				tok[p] = '\0';
			else 
				break;
		}

		if(!first)
			fprintf(outFile,",");
		fprintf(outFile,"%s", tok);
		first = false;
		
    	if(strcmp(filterColumn,tok) == 0){
    		filterIndex = colnums;
    	}
    	colnums++;
		
	}

	fprintf(outFile,"\n");   
	/*
	if(filterIndex == -1) 
		printf("\nError: Column provided as argument does not exist in csv\n");
	*/
	//column argument given doesnt exist in the csv
	if(filterIndex == -1){
		printf("\nError: Column provided as argument does not exist in csv\n");
		fclose(fp);    
    	fclose(outFile);
		return NULL;
	}
	if(rownums == 0 || colnums == 0){
		printf("Error: Empty csv");
		fclose(fp);    
    	fclose(outFile);
		return NULL;
	}
	if(colnums != 28){
		printf("Error: Incorrect number of columns");
	}


	//All rows but the first are saved into a 2D-array call database
	char* database[rownums][colnums];
	int rowCounter = 0;

	struct Node array[rownums]; 
  
	while (fgets(line, lineSize, fp)) {

		if(rowCounter > rownums){
			printf("Error: Incorrect number of rows\n");
			fclose(fp);    
    		fclose(outFile);
			return NULL;
		}

		char* tmp = strdup(line);
    	
	    int colCounter = 0;
	    int paren = 0;
	    int comma = 0;
	   	int i = 0; 
	   	int j = 0;

	   	char* tok = (char*) malloc(1000*sizeof(char));
	   	
	   	while(i<(strlen(tmp))){
	   		
	   		if(colCounter == colnums-1){
		    	int p = 0;
          for(p=strlen(tok)-1; p>0; p--){
					if(tok[p] == '\n' || tok[p] == '\r')
						tok[p] = '\0';
					else 
						break;
				}
			}

			if(i+1 == strlen(tmp)){
   				tok[j] = tmp[i];
   				tok[j+1] = '\0';		
				database[rowCounter][colCounter] = strdup(tok);
				break;
	   		}
			
	   		if(tmp[i] == '\n' || tmp[i] == '\r'){

	   			tok[j] = '\0';		
				database[rowCounter][colCounter] = strdup(tok);
				comma = 0;		
	   			break;
	   		}


	   		if(tmp[i] == '"')
	   			paren = (paren+1)%2;

	   		if(tmp[i] != ','){
	   			tok[j] = tmp[i];
	   			comma = 0;
	   		}
	   		else{
	   			if(comma == 1){
	   				tok[j] = '\0';   			
		    		database[rowCounter][colCounter] = strdup(tok);
					j = -1;		
		        	colCounter++;
		        	j++;
	   				i++;
		        	continue;
	   			}

	   			comma = 1;

	   			if(paren == 1){
	   				tok[j] = tmp[i];
	   				i++; j++;
	   				continue;
	   			}
				
	   			tok[j] = '\0';   			
	    		database[rowCounter][colCounter] = strdup(tok);
				j = -1;		
	        	colCounter++;

	   		}
	   		j++;
	   		i++;
	   		
	   	} 

   		if(isalpha(database[rowCounter][filterIndex][0])){
    		filterColumnIsAlpha = true;
   		}

	    char* processed = strdup(database[rowCounter][filterIndex]);

        array[rowCounter] = (struct Node) {rowCounter, trimwhitespace(processed)};
        rowCounter++;
        
    }

    if(rowCounter != rownums){
  		printf("Error: Incorrect number of rows\n");
  		fclose(fp);    
    	fclose(outFile);
  		return NULL;
	  }
      
    //sorting struct array
    mergeSort(array, 0, rownums-1, filterColumnIsAlpha);
  
    int j = 0;
    for(j=0; j<rownums; j++) {

    	int rowid = array[j].rowID;	
	    fprintf(outFile,"%s", database[rowid][0]);	

      int k = 0;
    	for(k=1; k<colnums; k++) {
	    	fprintf(outFile,",%s", database[rowid][k]);
	    	//clear memory
	    	free(database[rowid][k]);
    	}
    	fprintf(outFile,"\n");
    }
    
    fclose(fp);    
    fclose(outFile);

    return NULL;
}

//remove leading/trailing whitespace
char* trimwhitespace(char* str) {
	char* end;

	// Trim leading space
	while(isspace((unsigned char)*str)) {
  		str++;
  	}

  	if(*str == 0) {
    	return str;
	}

  	// Trim trailing space
  	end = str + strlen(str) - 1;
  	while(end > str && isspace((unsigned char)*end)) {
  		end--;
  	}

  	end[1] = '\0';

  	return str;
}