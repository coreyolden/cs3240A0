/*
Corey Oldenberg
CS 3240
2/9/2018
A1
This program should read a csv file that was provided and create structs. Each line is a song seperated
 with commas and containing several sections that we do not need. The program will read each line and
 create a struct which will be populated by applicable information. The structs will then be sorted and placed
 into a binary file. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


/* This will create a struct named songs which will be filled with important information */
typedef struct _songs {
	char *artist;
	char *songname;
	char *albumname;
	float duration;
	int yearofrelease;
	double hotness;
} songs;

int sortsongs(const void *song1, const void *song2)
{
	songs **one = (songs **)song1;
	songs **two = (songs **)song2;
	char *firstsong = (*one)->songname;	/*get first song name*/
	char *secondsong = (*two)->songname;	/*get second song name*/
	
	return strcmp(firstsong, secondsong); /*return positive or negative*/
}


char *segmentstrip(char *segment){
	if(segment[0]=='"'){ /*if first character is a parentheses*/
		for(int i = 0; i<strlen(segment); i++){
			segment[i]=segment[i+1];	 /*rewrite the string so it has no leding parentheses*/
		};
	};
	if(segment[strlen(segment)-1]=='"'){	/*if segment ends in a parentheses*/
		segment[strlen(segment)-1] = 0;
	};
	return segment;
}



void printsonginfo(songs **arr, int songlookup){/*print the information from the song*/
printf("\nSong name: %s\nArtist name: %s   Album name: %s\n",arr[songlookup]->songname,arr[songlookup]->artist,arr[songlookup]->albumname);
printf("Duration: %f  Hotness: %lf  Year of release: %d\n\n",arr[songlookup]->duration,arr[songlookup]->hotness,arr[songlookup]->yearofrelease);
}

void binarysearch(char *song, songs **arr, int first, int last){
	if(last >=first){
		int middle = (first+last)/2;
		char *middlesong = arr[middle]->songname;
		int songcompare = strcmp(middlesong,song);
		if(songcompare == 0){
			 printsonginfo(arr,middle);}
		else if(songcompare > 0){
			binarysearch(song, arr, first, middle-1);}
		else if(songcompare <0){
			 binarysearch(song,arr,middle+1,last);};
		}else{ /*iflast is less than first then you went though the array but its not there*/
			printf("that song is not in the array\n\n");
		};
}
songs **createsong(char *line, songs **arr, int numofsongs){
	//printf("printed line %s\n%d\n\n",line, numofsongs);
	char *newline= malloc(512);
	newline = line;
	int segmentnumber = 0;
	int length = 100;
	char *segment = malloc(512);
	if(numofsongs>0){
	arr = realloc(arr, (numofsongs * sizeof(songs))); /*reallocate the memory to be the number of songs we have */
	}
	arr[numofsongs] = malloc(sizeof(songs));
	while (segmentnumber < 19)
	{
		segment = strsep(&newline, ","); /*splits the line at a demlimiter*/
		if (segmentnumber == 3)
		{
			segmentstrip(segment);
			arr[numofsongs]->albumname = malloc(length);
			strncpy(arr[numofsongs]->albumname, segment, length);
		}
		else if (segmentnumber == 8)
		{
			segmentstrip(segment);
			arr[numofsongs]->artist = malloc(length);
			strncpy(arr[numofsongs]->artist, segment, length);
		}
		else if (segmentnumber == 10)
		{
			arr[numofsongs]->duration = atof(segment);
		}
		else if (segmentnumber == 14)
		{
			arr[numofsongs]->hotness = atof(segment);
		}
		else if (segmentnumber == 17)
		{
			segmentstrip(segment);
			arr[numofsongs]->songname = malloc(length);
			strncpy(arr[numofsongs]->songname, segment, length);
		}
		else if (segmentnumber == 18)
		{
			arr[numofsongs]->yearofrelease = atoi(segment);
		};
		segmentnumber++;
	}
	segmentnumber = 0;
	
	return arr;
}




void readfile(){
	songs **arr = malloc(sizeof(songs));
	FILE *csvfile;						 /* initialize a FILE object called csvfile */
	csvfile = fopen("SongCSV.csv", "r"); /* open the provided csv file */

	char *buffer = malloc(4096);
	buffer[4095]= '\0';
	char *savesong = malloc(512);
	char *line = malloc(512);
	char *leftover = malloc(512);
	int countfrom = 0;
	int countto = 0;
	int songsaved = 0;
	int end = 0;
	int firstline = 0;
	int numofsongs = 0;
	while (read(fileno(csvfile), buffer, 4095)>0){
		while (countto < 4095)
		{
			if(buffer[countto]=='\0'){
				end = 1;
				break;
			}
			if (buffer[countto] == '\n')
			{
				if (songsaved == 1)
				{
					songsaved = 0;
					strncpy(leftover, buffer, countto-countfrom);
					line = strcat(savesong, leftover);
					memset(leftover, '\0', 512);
				}
				else
				{
					strncpy(line, buffer+countfrom, countto-countfrom);
					
				}
				if(firstline == 1){
					arr = createsong(line, arr, numofsongs);
					numofsongs++;
				 
				}else{
					firstline=1;
				}
				memset(line, '\0', 512);
				countfrom = countto;

			}
			countto++;
		}
		
		if(buffer[countfrom]!='\0' && end == 0){
			memset(savesong, 0, 512);
			strncpy(savesong, buffer+countfrom, 4095-countfrom);
			songsaved=1;
			
		}
		memset(buffer, '\0', 4095);
		countfrom = 0;
		countto = 0;
	}
	free(buffer);
	free(line);
	fclose(csvfile);
	
	qsort(arr, numofsongs, sizeof(songs *), sortsongs); /*sort the array*/

	int file = creat("database.bin", 0644);
	for(int i =0; i<numofsongs; i++){

		char *artist;
		char *songname;
		char *albumname;
		float duration;
		int yearofrelease;
		double hotness;

		write(file, &arr[i]->artist, 100);
		write(file, &arr[i]->songname, 100);
		write(file, &arr[i]->albumname, 100);
		write(file, &arr[i]->duration, sizeof(float));
		write(file, &arr[i]->yearofrelease, sizeof(int));
		write(file, &arr[i]->hotness, sizeof(double));
	}

}



int main(int argc, char *argv[]){
	//readfile();
	
	char *input=malloc(256); /* array for user input*/
	char *exitstring = "ZZZ"; /*array to compare for exit condition*/
	for(;;){
			printf("Search for a song or enter \"ZZZ\" to exit program\n");
			fgets(input,256,stdin); /*get user input*/
			input[strlen(input)-1] = 0; /*gets rid of the newline key from submitting input*/
			if (strcmp(input,exitstring)==0){
				break;
			}else{
			/*The next bit of code will get the number of songs*/
					int numofsongs =0;
					for(int i = 1; arr[i]!=NULL; i++){numofsongs = i;};
					numofsongs--;
					binarysearch(input, arr, 0, numofsongs);
				};
}

}
