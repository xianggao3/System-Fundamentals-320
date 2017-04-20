//**DO NOT** CHANGE THE PROTOTYPES FOR THE FUNCTIONS GIVEN TO YOU. WE TEST EACH
//FUNCTION INDEPENDENTLY WITH OUR OWN MAIN PROGRAM.
#include "map_reduce.h"
#include <string.h>
#include <dirent.h>
#include <stdio.h>

//Implement map_reduce.h functions here.
int validateargs(int argc, char** argv){

	int vFlag = 0;
	char* helpMenu = "Usage:  ./mapreduce [h|v] FUNC DIR \
		FUNC    Which operation you would like to run on the data:\
		ana - Analysis of various text files in a directory.\
		stats - Calculates stats on files which contain only numbers.\
		DIR     The directory in which the files are located.\
		Options:\
		-h      Prints this help menu.\
		-v      Prints the map function’s results, stating the file it’s from.";


	if(argc < 2){
		printf("Invalid Arguments");
		return -1;
	}

	for(int vCheck=0 ;vCheck<=argc;vCheck++){		
		if (strcmp(argv[vCheck], "-v")==0){
			vFlag=1;
			printf("-v");
		}
	}

	for(int i=0 ;i<=argc;i++){
		if( strcmp(argv[i],"-h")==0){
			printf("-h");
			printf("%s",helpMenu);
			return 0;	
		}else if (strcmp(argv[i], "ana")==0){
			if(vFlag==0){
				return 1;
			}else {
				return 3;
			}
		}else if (strcmp(argv[i], "stats")==0){
			if(vFlag==0){
				return 2;
			}else {
				return 4;
			}
		}else{ 
			printf("%s",helpMenu);
			return -1;
		}
	}
	return -1;
}	

int nfiles(char* dir){
	int num = -2;
	DIR *directory;
	struct dirent *nextDir;

	directory = opendir(dir);
	if(directory==NULL){
		return -1;
	}	

	while( (nextDir = readdir(directory))!=NULL ){
		printf ("[%s]\n", nextDir->d_name); 									/*just to check the names*/
		num++;
	}
	
	if(num==0){
		printf("No files present in the directory.");
		return 0;
	}
	closedir(directory);
	return num;
}	

int map(char* dir, void* results, size_t size, int (*act)(FILE* f, void* res, char* fn)){

	DIR *directory;
	struct dirent *nextDir;
	int sum=0;

	directory = opendir(dir);

	while( (nextDir = readdir(directory))!= NULL){
		char* filepath;

		memset(results, 0, size*strlen(nextDir->d_name)+1);
		//(char*) results = results + length;
		filepath = strdup(strncat(dir, nextDir->d_name,strlen(nextDir->d_name)+1));

		FILE* fp;
		fp= fopen(filepath, "r");
		if(fp!= NULL){

			//code here for part 4?
			sum += act(fp,results,filepath);

			fclose(fp);
		}else{
			return -1;
		}
	}

	closedir(directory);
	return sum;
}


struct Analysis analysis_reduce(int n,void* results){

	struct Analysis finalStruct;
	finalStruct.filename = NULL;
	finalStruct.lnlen=0;
	for(int x=0;x<128;x++){
		finalStruct.ascii[x]=0;
	}

	for(int i =0; i <n ; i++){
		if(((struct Analysis*)results+i)->lnlen >= finalStruct.lnlen){	//if current struct in results is biggest, make it the finalStruct
			finalStruct.lnlen = ((struct Analysis*)results+i)->lnlen;
			finalStruct.lnno = ((struct Analysis*)results+i)->lnno;
			for(int z=0; z<128; z++){
				finalStruct.ascii[z]+=((struct Analysis*)results+i)->ascii[z];
			}
		}

	}
	
	return finalStruct;
}	

void analysis_print( struct Analysis res, int nbytes, int hist){
	char* file = "File:";
	char* llen = "Longest line length:";
	char* lnum = "Longest line number:";
	char* tbytes= "Total Bytes in directory:";
	char* histo= "Histogram:";

	
	printf("%s %s\n", file, res.filename);
	printf("%s %d\n", llen, res.lnlen);
	printf("%s %d\n", lnum, res.lnno);
	//implement if -v is on
	
	printf("%s %d\n", tbytes, nbytes);
	printf("%s %s\n", file, res.filename);

	printf("%s\n", histo);
	for (int i =0; i<128; i++){
		if(res.ascii[i] >0){
			printf("%d:",i);					//prints number:
			for(int numDashes = 0; numDashes<= res.ascii[i]; numDashes++){	//prints amt of dashes
				printf("-");
			}
		}
	}

}

/*part 2

struct Analysis {
	int ascii[128]; //space to store counts for each ASCII character.
	int lnlen; 		//the longest line’s length
	int lnno;		//the longest line’s line number.
	char* filename;	//the file corresponding to the struct.
};
*/
int analysis( FILE * f, void * res, char * filename){
	int llen=0;
	int llnum=0;
	int bytes=0;
	int current=0;
	int currentLineLen=0;
	int currentLineNum=1;
	struct Analysis* aStruct=((struct Analysis*)res);

	for(int i =0; i<128;i++){	//initialize all to 0
		aStruct->ascii[i]=0;
	}

	while((current = fgetc(f)) != EOF){
		bytes++;
		aStruct->ascii[current]++;
		if(current != 10){
			currentLineLen++;
		}else {
			if(currentLineLen>=llen){
				llen=currentLineLen;
				llnum=currentLineNum;
			}
			currentLineNum++;
			currentLineLen=0;
		}
	}	
	aStruct->filename= filename;		//putting data into the analysis struct
	aStruct->lnlen=llen;
	aStruct->lnno=llnum;
	//aStruct=((struct Analysis*)res);					//putting the aStruct into the res array

	return bytes;
}


/*stats
typedef struct Stats {
int histogram[NVAL];//space to store the count for each number.
int sum;//the sum total of all the numbers in the file.
int n; //the total count of numbers the files.
char* filename;//the file corresponding to the struct.
				//(don’t print for final result)
} Stats;
*/
int stats(FILE* f,void* res,char* filename){
	Stats* statStruct=((Stats*)res);
	int currentNum;
	statStruct->sum=0;
	statStruct->n=0;
	statStruct->filename=filename;

	for(int i =0; i<=NVAL; i++){ 	//clears histogram
		statStruct->histogram[i]=0;
	}

	if(f==NULL){
        return -1;
    }
    while(!feof(f)){				//incr per number read
    	fscanf(f,"%d",&currentNum);
    	statStruct->histogram[currentNum]++;
    	statStruct->sum+=currentNum;
    	statStruct->n++;
    }
    //statStruct=((Stats*)res);		//put into res array
    return 1;

}

Stats stats_reduce(int n, void * results){

	Stats finalStats;
	finalStats.filename=NULL;
	finalStats.n=0;
	finalStats.sum=0;

	for(int x=0;x<NVAL;x++){
		finalStats.histogram[x]=0;
	}
	for(int i =0; i<n; i++){
		finalStats.n += ((Stats*)results+i)->n;
		finalStats.sum += ((Stats*)results+i)->sum;
		for(int z=0; z<NVAL; z++){
				finalStats.histogram[z]+=((Stats*)results+i)->histogram[z];
		}
	}

	return finalStats;
}

void stats_print(Stats res,int hist){

}