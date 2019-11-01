/*************************************************************************
*Author: Brenden Smith
*Description: Reads in room data that is propogated by the building program.
*	The user will be placed in the START_ROOM and will work towards the
*	END_ROOM. Threading was utilized to access time and date while playing
 * *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

struct Room{
	char name[8];
	char type[11];
	char *connections[6];
	int num_connections;
};

struct Map{
	int current;		//Current room index for graph
	int visited[100];	//Tracks visited areas
	int num_visited;
};

struct Game{
	struct Room graph[7];	//Holds the loaded rooms
	struct Map map;
};


void set_name();

void get_newest_directory(char *direct){
	//Set DIR and stat() pointers
   	DIR *d = opendir(".");
	struct dirent *de;	//Will hold directory entry
	struct dirent *bestd;	//Will hold the most recent
	struct stat holder;
	time_t latest = 0;;	//Most recent time


	//Used to navigate to strictly room folders
	char *starter = "smitbre2.rooms.";
	int logic = 1;

	if(d == NULL){
	   //Failed to open directory
	   printf("Failed to open directory.");
	   return;
	}

	while((de = readdir(d)) != NULL){
	   logic = strncmp(starter, de->d_name, 14); //Compare start of directory
	   if(logic == 0){
	      //This directory contains rooms. Check for most recent
	  
	      if(stat(de->d_name, &holder) == 0){
		 //0 is success
		 //Swap in largest time returned
		 if(holder.st_mtime > latest){
		    strcpy(direct, de->d_name);
	  	    latest = holder.st_mtime;	 
		 }

	      }
	   }
	}
	closedir(d);
	return;
}	

/***********************************************************************
*Description: Reads through the passed folder directory and reads in 
*   all files in thep assed directory. The newest directory is delved
*   into. All files with a _room suffix should be loaded and read into
*   a struct. A pointer to said struct will be returned.
*
************************************************************************/
struct Game* read_files(char *directory){
   //Directory and File declarations
   DIR *d;
   struct dirent *de;
   FILE *f;
   int itr = 0;
   int cc = 0;		//Indexing for connections
   char *str;
   char start[100] = { '\0' };
   strcpy(start,directory);
   strcat(start, "/");   

   char result[200] = {'\0'};

   //Getline
   char *buffer;
   size_t buffsize = 32;
   ssize_t characters;
   buffer = (char *)malloc(buffsize * sizeof(char));
   char connection[50] = { '\0' };

   //Declare struct to return
   struct Game *g = malloc(sizeof (struct Game));

   //Open directory with newest files
   d = opendir(directory);
   
   if(d == NULL){
   	printf("Failed to open directory");
	return;
   }

   //Cycle through the files and store their contents
   while((de = readdir(d)) != NULL){
   	
        //Keep reading in the files
	if(strlen(de->d_name) > 3){
	   strcpy(result, start);		//Reset result 
	   strcat(result, de->d_name);
	 
	   f = fopen(result, "r");

	   if(f == NULL){
	      perror("Error: ");
	 
	   }else{
	      fseek(f, 11, SEEK_SET);		//Position iterator
	      characters = getline(&buffer, &buffsize, f); //Holds room name

	      buffer[strcspn(buffer, "\n")] = 0;	   //Will slice trailing  newline

	      strncpy(g->graph[itr].name, buffer, 8);
	      
	      g->graph[itr].name[8] = '\0';
	        //Nab all the connections
		while((characters = getline(&buffer, &buffsize, f)) != -1){
			      	   
		   //Set connection if line is not room type
		   if(buffer[0] != 'R'){
	 	      sprintf(connection, "%s", buffer+14);	//Move cursor   
		      g->graph[itr].connections[cc] = malloc(sizeof(char[8]));
		      g->graph[itr].connections[cc][8] = '\0';
	
		      strcpy(g->graph[itr].connections[cc], connection);
		      cc++;		//Update count of connections
		   
		   }else{
		      //Set room type and number of connections
		      sprintf(connection, "%s", buffer+11);	//Move cursor
		      
		      strcpy(g->graph[itr].type, connection);
		      g->graph[itr].type[11] = '\0';
		      g->graph[itr].num_connections = cc;
		   }
		}
		cc = 0;		//Reset indexing
	   }
	   fclose(f);
	   itr++;
	}
   }
   closedir(d);	
   free(buffer);
   g->map.num_visited= 1; 
   return g;
}

/*******************************************************************
*Description: Determines if user input is good/bad/time.
*	Returns -2 for time, -1 for failure. otherwise
*	an inde is returned of the file inside g->graph
*
********************************************************************/
int _get_user_input(struct Game *g){
	int check = 0;
	int index = g->map.current;
   	char name[8] = { '\0' };

   	scanf("%s", name);
	printf("\n");

	//Check for time
	if(strcmp(name, "time") == 0){
		return -2;
	}


	int length = 0;
	int i = 0;
	for(i; i < g->graph[index].num_connections; i++){
	   length = strlen(g->graph[index].connections[i]);
	   
	   if(strncmp(name, g->graph[index].connections[i], length) == 0){
	   	//Correct input, find/return the index in graph
		strncpy(name, g->graph[index].connections[i], length);
		name[8] = '\0';

		//Find in graph and return index
		int j = 0;
		for(j; j < 7; j++){
		   length = strlen(g->graph[j].name);
		   if(strncmp(name, g->graph[j].name, length) == 0){
		      //Return next rooms index	
		      return j;
		   }
		}
	   } 
	}
	//Return -1 as a failure
	return -1;

}

void* get_time(){
	FILE *f;
        time_t t;
	struct tm *info;
	char arg[100] = {'\0'};

	time(&t);
	info = localtime(&t);
        

	//Format time string and write to file
	strftime(arg, 100, "%I:%M%p, %A, %B, %d, %Y", info);
	f = fopen("./currentTime.txt", "w+");
	fputs(arg, f);
	
	printf("%s\n", arg);
	printf("\n");
	fclose(f);
}

int ui(struct Game *g){
   int time = 0;
   int index = g->map.current;	//Make life easier
   char tmp[9];
   char *buffer;
   int boolboi = 0;
   signed int new_room;		//Will recieve index of room traversal

   //Fix up strings
   g->graph[index].connections[0][strcspn(g->graph[index].connections[0], "\n")] = 0;
   printf("CURRENT LOCATION: %s\n", g->graph[index].name);
   
   printf("POSSIBLE CONNECTIONS: %s", g->graph[index].connections[0]);
   
   //Cyce through and print connections
   int i = 1;
   for(i; i < g->graph[index].num_connections; i++){
      g->graph[index].connections[i][strcspn(g->graph[index].connections[i], "\n")] = 0;
      
      printf(", %s", g->graph[index].connections[i]);
   }
   printf(".\n");
   printf("WHERE TO? >");
   do{
	
      //Get index for next room
      new_room = _get_user_input(g);
      
      if(new_room > -1) {
	 //Correct Input
	 //break from this loop
	 boolboi = 1;

      }else if(new_room == -2){
	 //Call thread to get time
	 time = 1;
	 break; 
      }else{
      	//Bad input try again
        printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
	ui(g);
	return;
      }

   }while(boolboi != 1);
  
   if(time == 1){
   	time = 0;
	return -1;
   }

   //Set new current and update map with the new room
   //Need to set current to the graph index not connections
   g->map.current = new_room;
   g->map.visited[g->map.num_visited] = new_room;
   g->map.num_visited++;

   //Check for end case
   if(strncmp(g->graph[g->map.current].type, "END_ROOM", 8) == 0){
      return 1;
   }else{
      return 0;
   }

}


void get_first_room(struct Game *g){
   int i = 0;	
   for(i; i < 7; i++){
      if(g->graph[i].type == "START_ROOM"){
	 g->map.current = i;
	 i = 7;		//Room found
      }
   }
}

void show_path(struct Game *g){
   int i = 0;
   printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
   printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", g->map.num_visited);   
   for(i; i < g->map.num_visited; i++){
   	printf("%s\n", g->graph[g->map.visited[i]].name);
   }
}

void disconnect_graph(struct Game *g){
   int i = 0;	
   for(i; i < 7; i++){
      
      int j = 0;	
      for(j; j < g->graph[i].num_connections; j++){
	 free(g->graph[i].connections[j]);
      }
   }
   free(g);
}

int main(){
   FILE *f;
   struct stat tmp;
   char directory[200];
   int length = 0;
   struct Game *g;
   int logic = 0;
   
   //Find File Directory that is the newest
   get_newest_directory(directory);

   //Read File and store rooms
   g = read_files(directory);

   //Get start room to load. All other rooms will be placed @ runtime
   get_first_room(g);


   //Call ui untill in end room
   do{
      logic = ui(g);

      if(logic == -1){
      	//Thread for time
	pthread_t thread;
	pthread_create(&thread, NULL, get_time, NULL);
	pthread_join(thread,NULL);
      }

   }while(logic <= 0);

   //Display path taken
   show_path(g);
  
   //Deallocate memory for all connections
   disconnect_graph(g);

}
