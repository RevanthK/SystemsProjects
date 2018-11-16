#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "mergesort.c"
#include "simpleCSVsorter.h"

int lineSize = 1024;
int filterColumnIsAlpha = false;

char* trimwhitespace(char* str);

int main(int argc, char** argv) {

	if(argc < 3) {
		printf("Error: incorrect number of arguments\n");
		return 1;
	}

	//sorting column
	char* filterColumn;
	int filterIndex = -1;

	//number of rows in movie database
	int rownums = 5043;

	for(int i = 1; i<argc; i+=2) {
		if(strcmp(argv[i],"-c") == 0)
			filterColumn = argv[i+1];
		
		if(strcmp(argv[i],"-r") == 0)
			rownums = atoi(argv[i+1])-1;	
	}

	char line[lineSize];

	int colnums = 0;
	
	//Read first line and figure out which header column to filter by
    fgets(line, lineSize, stdin);

	char* tmp = strdup(line);
	bool first = true;
	char* tok;

	while ((tok = strsep(&tmp, ",")) != NULL) {

		for(int p=strlen(tok)-1; p>0; p--){
			if(tok[p] == '\n' || tok[p] == '\r')
				tok[p] = '\0';
			else 
				break;
		}
		if(!first)
			printf(",");
		printf("%s", tok);
		first = false;
    	if(strcmp(filterColumn,tok) == 0){
    		filterIndex = colnums;
    	}
    	colnums++;
	}
	printf("\n");

	//column argument given doesnt exist in the csv
	if(filterIndex == -1){
		printf("Error: Column provided as argument does not exist in csv");
		return 1;
	}
	if(rownums == 0 || colnums == 0){
		printf("Error: Empty csv");
		return 1;
	}

	//All rows but the first are saved into a 2D-array call database
	char* database[rownums][colnums];
	int rowCounter = 0;

	struct Node array[rownums]; 


	while (fgets(line, lineSize, stdin)) {

		if(rowCounter > rownums){
			printf("Error: Incorrect number of rows\n");
			return 1;
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
		    	for(int p=strlen(tok)-1; p>0; p--){
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
		return 1;
	}
    //sorting struct array
    mergeSort(array, 0, rownums-1, filterColumnIsAlpha);
  
    for(int j=0; j<rownums; j++) {

    	int rowid = array[j].rowID;	
	    printf("%s", database[rowid][0]);	

    	for(int k=1; k<colnums; k++) {
	    	printf(",%s", database[rowid][k]);
	    	//clear memory
	    	free(database[rowid][k]);
    	}
    	printf("\n");
    }

    //Bonus part 1: finding frequency

    /*
    int maxValue = 0;
    for(int a = 0; a<rownums; a++){
    	if(atoi(database[a][27]) > maxValue)
    		maxValue = atoi(database[rowid][27]);
    }


    int numBuckets = maxValue/rownums;
    int frequency[numBuckets];

	for(int a = 0; a<rownums; a++){
    	frequency[atoi(database[a][27])]++;
    }    

    for(int a = 0; a<numBuckets; a++)
    	printf("%d,", frequency[a]);
    */

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
