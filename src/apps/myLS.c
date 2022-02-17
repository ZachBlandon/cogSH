#ifndef MYLS_C
#define MYLS_C

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/findStr.c"

#define DIR_SIZE 512

//function to list specified directory, if no directory is specified it lists cwd
void ls(char* args)
{
		char* cmdBuffer = NULL;
		int flag = 0;

		if (findStr(args, &cmdBuffer, "-a", TRUE)) //check if ls is followed by -a flag
		{
			flag = 1;					//if it is, remaining args are put in cmdBuffer
		}
		else									//else
		{
			cmdBuffer = (char*)malloc(sizeof(char) * strlen(args) + 1); //copy args to cmdBuffer
			memcpy(cmdBuffer, args, strlen(args) + 1);			
		}
		
		struct dirent* dirStruct;	//struct to hold directory information
		struct stat statStruct;		//struct to hold file information
		char dirBuffer[DIR_SIZE];	//abslolute path to current file/dir
		int length;					//length of dirBuffer
		DIR* dir;					//pointer to directory
		
		if (cmdBuffer == NULL || cmdBuffer[0] == '\0')	//if no args are passed
		{
			dir = opendir(getcwd(dirBuffer, DIR_SIZE));	//put cwd in dirBuffer
			length = strlen(dirBuffer);
			dirBuffer[length++] = '/';
		}
		else									//else if args are passed
		{
			if ((dir = opendir(cmdBuffer)) != NULL)	//if the arg is a valid path
			{
				getcwd(dirBuffer, DIR_SIZE);	//make absolute path and put into dirBuffer
				length = strlen(dirBuffer);
				memcpy(dirBuffer + length + 1, cmdBuffer, strlen(cmdBuffer));
				dirBuffer[length] = '/';
				length = strlen(dirBuffer);
				dirBuffer[length++] = '/';
			}
			else							//else if arg is invalid path
			{						//give error message
				printf("ls: cannot access \'%s\': No such file or directory\n", cmdBuffer);
				return;
			}
		}
		
		while((dirStruct = readdir(dir)) != NULL) //while files remain in the directory
		{				//avoid printing '.' and '..' dirs
			if (!strcmp(dirStruct->d_name, ".") || !strcmp(dirStruct->d_name, ".."))
			{
				continue;
			}
			
			if (flag)	//if [-a] was passed
			{			//append filename to path
				memcpy(dirBuffer + length, dirStruct->d_name, dirStruct->d_reclen);
				stat(dirBuffer, &statStruct); //and get file stats
				
				//TODO fix -a for non-current dir
				switch((statStruct.st_mode & S_IFMT)) //switch on file mode
				{
					case S_IFREG:
						printf("[r] ");	//print [r] for regular files
						break;
					case S_IFDIR:
						printf("[d] "); //print [d] for directories
						break;
					case S_IFLNK:
						printf("[l] "); //print [l] for links
						break;
					default:
						printf("[o] "); //print [o] for other
						break;
				}
			}

			printf("%s\n", dirStruct->d_name); //print filename
		}

		closedir(dir);	//close DIR* dir
		free(cmdBuffer); //free cmdBuffer
		return;
}


#endif
