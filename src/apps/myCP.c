#ifndef MYCP_C
#define MYCP_C

#define PATH_SIZE 128

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/prectok.c"


//function to copy contents of one file to another
static void copyFile(char* src_path, char* dest_path)
{
	FILE* src, *dest;
	
	if ((src = fopen(src_path, "r")) == NULL) //check if src can be opened
	{
		printf("cp: Error opening file '%s'\n", src_path); //if not print error
	}
	else
	if ((dest = fopen(dest_path, "w")) == NULL) //check if dest can be opened
	{
		printf("cp: Error opening file '%s'\n", dest_path); //if not print error
		fclose(src); 								//and close src
	}
	else												//if no errors
	{
		char temp;
		while((temp = fgetc(src)) != EOF)		//loop through src
		{
			fputc(temp, dest);					//annd copy contents into dest
		}
		
		fclose(src);						//close both
		fclose(dest);
	}
	
	return;
}


//function to copy one file to another or many files to a directory
//NOTE: -r is not implemented for dir to dir copy
void cp(char* args)
{
	int count = 8, index = 0;	//count is max number of additional args
	char* files[count]; 		//index to keep track of how many args there are
								
	char* token = prectok(args, '\'', ' '); //tokenize args on space and single quote
	
	while(token != NULL)	//loop while tokens remain
	{	
		if (index < count)	//if in bounds
		{		//copy token into files array
			files[index] = (char*)malloc(sizeof(char) * (strlen(token) + 1));
			memcpy(files[index], token, strlen(token));
			files[index++][strlen(token)] = '\0';
		}
		else	//else if not in bounds
		{
			//TODO dynamically increase arr size
			printf("cp: Too many arguements\n");
			return;
		}
		
		token = prectok(NULL, '\'', ' '); //get next token
	}
	
	struct stat st;
	stat(files[index - 1], &st);	
	if (index <= 2)		//if two or less args are given
	{
		if (index == 0)	//if no args
		{					//give error
			printf("cp: Missing source and destination file operands\n");
			return;
		}
		else
		if (index == 1)	//if only one arg is given
		{					//print error message
			printf("cp: Missing destination file operand after '%s'\n", files[0]);
			free(files[0]);
			return;
		}
		
		struct stat temp;
		stat(files[0], &temp);
		
		if (!S_ISDIR(temp.st_mode) && !S_ISDIR(st.st_mode)) //base case where neither file is a dir
		{		
			if (!S_ISREG(temp.st_mode))	//if source file is not a regular file
			{								//give error
				printf("cp: Cannot stat '%s': No such file or directory\n", files[0]);
			}
			else				
			if (!strcmp(files[0], files[1]))	//if both files are same
			{									//give error
				printf("cp: '%s' and '%s' are the same file\n", files[0], files[1]);
			}						
			else					//if no errors
			{					//copy file
				copyFile(files[0], files[1]);
			}				//if dest file does not exist, one is made
		}
		else
		if (!S_ISDIR(temp.st_mode) && S_ISDIR(st.st_mode)) //if source is not a dir but dest is
		{
			if (S_ISREG(temp.st_mode))	//and source is a regular file
			{
				int tempOff = 0;
				int len = strlen(files[0]) - 1;
				int destLen = strlen(files[1]);
				
				//loop through source backwards to get file name
				while (len - tempOff >= 0 && files[0][len - tempOff] != '/' && files[0][len - tempOff] != ' ')
				{
					tempOff++;
				}
				
				//append file name to dest folder name
				char* tempStr = (char*)malloc(sizeof(char) * (destLen + tempOff + 1));
				memcpy(tempStr, files[1], destLen);
				tempStr[destLen] = '/';
				memcpy(tempStr + destLen + 1, files[0] + ((len + 1) - tempOff), tempOff);
				tempStr[destLen + tempOff + 1] = '\0';
				
				copyFile(files[0], tempStr);	//copy file
				
				free(tempStr);
			}
			else	//if source is not a regular file
			{		//give error
				printf("cp: Cannot stat '%s': No such file or directory\n", files[0]);
			}
		}
		else	//if both source and dest are dirs
		if (S_ISDIR(temp.st_mode) && S_ISDIR(st.st_mode))
		{
			//TODO dir to dir copy
			printf("cp: -r not specified: Omitting directory '%s'\n", files[0]);
			printf("cp: -r flag is not yet implemented\n");
		}
		else	//if source is dir and dest is file
		{		//give error
			printf("cp: Cannot overwrite non-directory '%s' with directory '%s'\n", files[1], files[0]);
		}
	}
	else //else if more than two files
	if (!S_ISDIR(st.st_mode))	//if final arg is not a dir
	{			//give error
		printf("cp: Target '%s' is not a directory\n", files[index - 1]);
	}
	else	//else if more than two args and final arg is a dir
	{	
		for (int i = 0; i < index - 1; i++) //loop through args
		{
			stat(files[i], &st);		
			if (S_ISDIR(st.st_mode))	//if arg is dir skip unless -r is present
			{
				//TODO dir to dir copy
				printf("cp: -r not specified: Omitting directory '%s'\n", files[i]);
				printf("cp: -r flag is not yet implemented\n");
				continue;
			}
			else
			if (S_ISREG(st.st_mode))	// if arg is regular file
			{
				int tempOff = 0;
				int len = strlen(files[i]) - 1;
				int destLen = strlen(files[index - 1]);
				
				//get file name 
				while (len - tempOff >= 0 && files[i][len - tempOff] != '/' && files[i][len - tempOff] != ' ')
				{
					tempOff++;
				}
				
				//and append to folder path
				char* tempStr = (char*)malloc(sizeof(char) * (destLen + tempOff + 1));
				memcpy(tempStr, files[index - 1], destLen);
				tempStr[destLen] = '/';
				memcpy(tempStr + destLen + 1, files[i] + ((len + 1) - tempOff), tempOff);
				tempStr[destLen + tempOff + 1] = '\0';
				
				copyFile(files[i], tempStr); //and copy file
				
				free(tempStr);
			}
		}
	}
	
	//loop through files and free each arg
	for (int i = 0; i < index; i++)
	{
		free(files[i]);
	}
}


#endif
