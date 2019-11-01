/********************************************************************
*AUthor: Brenden Smith
*Description: Will create a local directory appended by the process
	ID. The directory will get 7 room files that are randomly
	processed.
*********************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	//For getppid()
#include <sys/stat.h>	//mkdir

struct Room {
   char type[11];	
   char name[8];
   char* connections[6];	//Will hold the names of connected rooms
   int num_connections;
};

struct Graph {
   struct Room rooms[8];	//Will hold the 7 randomly generated rooms
};



void generate_rooms(struct Graph *g){
   //Hardcoded names for rooms
   int tuple_time[11] = {};	//Will flag already used names in assignment
   const char* names[10];
   names[0] = "Hollows";
   names[1] = "Caves";
   names[2] = "Grotto";
   names[3] = "Hideout";
   names[4] = "Woods";
   names[5] = "Abyss";
   names[6] = "Camp";
   names[7] = "Lair";
   names[8] = "Labs";
   names[9] = "Prairie";


   //Allocate arrays to hold connections

   int itr = 0;
   int index = rand() % 10;
   do{
      if(!tuple_time[index]){
	 strcpy(g->rooms[itr].type, "MID_ROOM");
	 strncpy(g->rooms[itr].name, names[index], 8);	//Set name
	 g->rooms[itr].name[8] = '\0';			//Set terminator
	 tuple_time[index] = 1;			//String has been used, make NULL
         g->rooms[itr].num_connections = 0;	//Set initial connections
	 itr++;
      }
      index = rand() % 10;

   }while(itr < 7);

   //Now set the start and end rooms
   index = rand() % 7;
   tuple_time[11] = index;
   strcpy(g->rooms[index].type, "START_ROOM");
   
   do{
      index = rand() % 7;
      if(tuple_time[11] != index){
	 strcpy(g->rooms[index].type, "END_ROOM"); 
	 break;
      }
   }while(1);
}


//Generate rooms and then add connections between to fit constraints

//Returns true if all rooms have 3 -> 6 connections, else false
int is_graph_full(struct Graph *g){
   //cycle through vertexes
   //If connections are met set continue until end and return true	
   int i = 0;
   for(i = 0;i < 7; i++) {
   	//Check if connection is NULL, then throw flag
	if(g->rooms[i].num_connections < 3) {
		return 0;
	}
   }
   return 1;
}

struct Room* get_random_room(struct Graph* g){
	//Return a random room
	return &g->rooms[rand()%7];
}

int can_add_connection_from(struct Room x){
	//Return true if a connection can be add from Room x, < 6 connections
	//False otherwise
   	if(x.num_connections < 6)
		return 1;
	else
	   return 0;
}

int connection_already_exists(struct Room x, struct Room y) {
	//Returns true of connections from x->y exists
	//False otherwise
	int i = 0;
	for(i=0;i < x.num_connections; i++){
		if(strcmp(x.connections[i], y.name) == 0)
		   return 1;
	}
	return 0;
}

void connect_room(struct Room *x, struct Room *y) {
	//Connects Rooms x and y, doest not check
	//Increments sizeing for both rooms
   	x->connections[x->num_connections] = malloc(sizeof(char[8]));
   	strcpy(x->connections[x->num_connections], y->name);
	x->num_connections++;

	//Place connection on room y
	y->connections[y->num_connections] = malloc(sizeof(char[8]));
	strcpy(y->connections[y->num_connections], x->name);
	y->num_connections++;
	
}

int is_same_room(struct Room x, struct Room y) {
	//Return true if x and y are the same
	if (strcmp(x.name, y.name) == 0) {
		return 1;
	}
	else{
		return 0;
	}
}


void add_random_connection(struct Graph *g){
   struct Room *A = malloc(sizeof(struct Room));
   struct Room *B = malloc(sizeof(struct Room));

   while(1){
      A = get_random_room(g);
      if (can_add_connection_from(*A) == 1)
	 break;
      
   }do{
	 B = get_random_room(g);
      }
   while(can_add_connection_from(*B) == 0 ||  is_same_room(*A,*B) == 1 || connection_already_exists(*A, *B) == 1);

   connect_room(A, B);	//Add this connection to real variables due to scoping

   //Create connections in the graph until full
   while (is_graph_full(g) == 0) {
	add_random_connection(g);
   }
}


void write_to_file(struct Graph g){
   //Get pid to append to directory
   int pid = getpid();
   char buffer[12];
   snprintf(buffer, 12, "%d", pid);
  
   //Start appending directory path 
   FILE *f;
   char final[200] = "./smitbre2.rooms.";
   
   strcat(final,buffer);
   strcat(final,"/");
   char tmp [8];	//Stores room name
   char ptr[200];	//ptr to file specific path

   //Make directory and store destination in ptr
   mkdir(final, ACCESSPERMS);
   
   //Cycle through all rooms in graph and write to individual files 
   int i;
   for(i=0;i<7;i++){
      //Concetenate new file directory for f to open
      strcpy(ptr,final);      
      strcpy(tmp, g.rooms[i].name);
      strcat(ptr, tmp);
      strcat(ptr,"_room");
   	   
      //Work around permission settings on the servers
      f = fopen(ptr, "w");
      chmod(ptr, 0777);

      if (!f){
	perror("fopen");
        return;
      }
      
      fprintf(f,"ROOM NAME: %s\n",g.rooms[i].name);
      
      //loop through the rooms connections to put in the file
      int j;
      for(j=0; j < g.rooms[i].num_connections; j++){
	fprintf(f,"CONNECTION %i: %s\n", j+1,g.rooms[i].connections[j]); 
      }
      
      fprintf(f,"ROOM TYPE: %s\n", g.rooms[i].type);
      
      //Close file
      fclose(f);
   }
}

void free_connections(struct Graph *m){
   int i = 0;	
   for(i; i < 7; i++){
	int j = 0;
	for(j; j < m->rooms[i].num_connections; j++){
		free(m->rooms[i].connections[j]);
	}	
   }
}

int main() {
   const int SIZE = 7;
   struct Graph map;
   srand(time(0));

   //Initializes values and sets random names
   generate_rooms(&map);

   //Sets up connections between rooms
   add_random_connection(&map);

   write_to_file(map);
   free_connections(&map);
}
