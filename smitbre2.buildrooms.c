#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct Room {
   char type[11];	
   char name[8];
   char** connections[6];	//Will hold the names of connected rooms
};

struct Graph {
   struct Room rooms[8];	//Will hold the 7 randomly generated rooms
};



void generate_rooms(struct Graph& g){
   //Hardcoded names for rooms
   int tuple_time[11] = {};	//Will flag already used names in assignment
   const char* names[10];
   names[0] = "Hollows";
   names[1] = "Caves";
   names[2] = "Grotto";
   names[3] = "Hideout";
   names[4] = "Woods";
   names[5] = "Abyss";
   names[6] = "Sanctuary";
   names[7] = "Lair";
   names[8] = "Labrynth";
   names[9] = "Prairie";


   int itr = 0;
   int index = rand() % 10;
   do{
      if(!tuple_time[index]){
	 strcpy(g.rooms[itr].type, "MID_ROOM");
	 strncpy(g.rooms[itr].name, names[index], 8);	//Set name
	 g.rooms[itr].name[8] = '\0';			//Set terminator
	 tuple_time[index] = 1;			//String has been used, make NULL
         itr++;
      }
      index = rand() % 10;

   }while(itr < 7);

   //Now set the start and end rooms
   index = rand() % 7;
   tuple_time[11] = index;
   strcpy(g.rooms[index].type, "START_ROOM");
   do{
      index = rand() % 7;
      if(tuple_time[11] != index){
	 strcpy(g.rooms[index].type, "END_ROOM");
	 break;
      }
   }while(1);
}

//Create a directory called smitbre2.rooms.<process Id of Rooms Program>
//Generate 7 different room files, each will contain one room->file
//Names can be anything < 8 chars. Must have 10 and only select 7

//Each room will have a type. START_ROOM, END_ROOM, MID_ROOM

//Each room must have 3 outbound connections, 6 at most. Connections must match
//


//File Format is as follows:
/*
ROOM NAME: <room name>
CONNECTION 1: <room name>

...
ROOM TYPE: <room type>
 */

//Generate rooms and then add connections between to fit constraints

//Returns true if all rooms have 3 -> 6 connections, else false
int is_graph_full(){
   //cycle through vertexes
   //If connections are met set flag and return falue	
   int i = 0;
   //for(i < 7; i++) {
   //	//Check if connection is NULL, then throw flag
   //}
}

Room get_random_room(Graph g){
	//Return a random room
	return g.rooms[rand()%7];
}

int can_add_connection_from(Room x){
	//Return true if a connection can be add from Room x, < 6 connections
	//False otherwise
}

int connection_already_exists(Room x, Room y) {
	//Returns true of connections from x->y exists
	//False otherwise
}

void connect_room(Room x, Room y) {
	//Connects Rooms x and y, doest not check
}

int is_same_room(Room x, Room y) {
	//Return true if x and y are the same
}

/*
void add_random_connection(){
   Room A;
   Room B;

   while(1){
      A = get_random_room();

      if (can_add_connection_from(A) == 1)
	 break;
      
   }do{
	 B = get_random_room();
      }
   while(can_add_connection_from(B) == 0 ||  is_same_room(A,B) == 1 || connection_already_exists(A,B) == 1);

   connect_room(A, B);	//Add this connection to real variables due to scoping
   connect_room(B, A);
   

   //Create connections in the graph until full
   while (is_graph_full() == 0) {
	add_random_connection();
   }
}
*/
int main() {
   const int SIZE = 7;
   struct Graph map;
   srand(time(0));

   generate_rooms(map);

   int i = 0; 
   for(i=0;i<7;i++){   
      std::cout << "Name of room["<< i << "]: " <<  map.rooms[i].name << std::endl;
 	std::cout << "   Type: "<<map.rooms[i].type <<std::endl;
   }
}
