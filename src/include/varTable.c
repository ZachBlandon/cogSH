#ifndef VARTABLE_C
#define VARTABLE_C

#include "uthash.h"

#define MAX_VAR 100
#define VAR_SIZE 32


//struct to hold var names and values and facilitate quick and easy CRUD
struct varStruct 
{
	char name[VAR_SIZE]; //name of var
	char value[SIZE];	  //value that var holds
	UT_hash_handle hh;	  // required by uthash to make struct hashable
};


struct varStruct *varTable = NULL; //base pointer to hash table


//delete a var with char* name in varTable
int deleteVar(char* name)
{
	struct varStruct* vs;

	HASH_FIND_STR(varTable, name, vs); //find var
	
	if (vs != NULL)						//if var exists
	{
		HASH_DEL(varTable, vs);		//delete var from table
		free(vs);						//free struct memory
		return 1;					//returns 1 if var deleted
	}
	
	return 0;						//returns 0 if no var found
}


//add a var to varTable
int addVar(char* name, char* value)
{
	int rtn = deleteVar(name); //if var with char* name is found then delete it	
	
	struct varStruct* vs; //create new varStruct and place name and values into it
	vs = (struct varStruct*)malloc(sizeof(struct varStruct));
	strcpy(vs->name, name);
	strcpy(vs->value, value);

	HASH_ADD_STR(varTable, name, vs); //add to hash table
	
	return rtn; //returns 1 if var overwritten, 0 if not
}


//find a var with char* name in varTable
struct varStruct* findVar(char* name)
{
	struct varStruct* vs;
	HASH_FIND_STR(varTable, name, vs); //find var and place varStruct into as
	
	//return struct if var with char* name is found, else returns NULL
	return vs;
}


//delele all elements from varTable and free used memory
void deleteAllVars()
{
	struct varStruct* vs, *temp;
	
	HASH_ITER(hh, varTable, vs, temp) //uthash macro to iterate through varTable
	{
		HASH_DEL(varTable, vs);	//delete element from varTable
		free(vs);					//free struct memory
	}
}


#endif
