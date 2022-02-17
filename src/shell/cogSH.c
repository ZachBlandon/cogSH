#define SIZE 256

#include "../include/cogSH.h"

//functions to handle keywords
void keywordExit(char*);
void keywordCd(char*);
void keywordAlias(char*);
void keywordUnalias(char*);
void keywordVar(char*);
void keywordUnset(char*);


//global variables used in keyword functions
int exitNum = 0;	
char dir[SIZE];
extern struct aliasStruct* aliasTable;
struct aliasStruct* alias = NULL;
int aliasCount = 0;
extern struct varStruct* varTable;
struct varStruct* var = NULL;
int varCount = 0;

	
int main()
{
	char inputBuffer[SIZE];
	char* cmdBuffer;
	char* promptStr = { "cogSH> " };
	int aliasFlag = FALSE;
	
	getcwd(dir, SIZE);
	
	FILE* fp;
	//TODO fix path
	if ((fp = fopen("etc/.cogshrc", "r")) != NULL)
	{	//check for PS1= in .cogshrc
		searchFile(fp, &promptStr, "PS1=");
	}
	
	//main loop
	while (1)
	{
		if (!aliasFlag) //if inputBuffer does not contain an alias value
		{
			//print prompt and get input from user
			printf("%s", promptStr);
			fgets(inputBuffer, SIZE, stdin);
		}
		else
		if (aliasFlag++ > TRUE) //if second time aliasFlag has been looped
		{
			inputBuffer[0] = '\0'; //make inputBuffer empty so no keywords are found
		}
		
		//find keyword exit
		if (findStr(inputBuffer, &cmdBuffer, "exit", TRUE))
		{
			keywordExit(cmdBuffer);		
			break; 
		}
		
		//find PS1= keyword and replace promptStr with user input
		if (findStr(inputBuffer, &promptStr, "PS1=", FALSE))
		{
			continue;
		}

		//find export keyword
		if (findStr(inputBuffer, &cmdBuffer, "export", TRUE))
		{	
			//if export is alone
			if (cmdBuffer[0] == '\0')
			{	//print var list
				for (var = varTable; var != NULL; var = (struct varStruct*) (var->hh.next))
				{
					printf("var %s=\"%s\"\n", var->name, var->value);
				}
				continue;
			}
			
			//if PS1= follows export then place what follows in promptStr
			if (findStr(cmdBuffer, &promptStr, "PS1=", FALSE))
			{
				continue;
			}
			
			//if an = is found after export then check for variables
			if (strstr(cmdBuffer, "=") != NULL)
			{
				keywordVar(cmdBuffer);
				continue;
			}
		}
		
		//find comment symbol
		if (findStr(inputBuffer, &cmdBuffer, "#", FALSE))
		{
			continue;
		}
		
		//find pwd keyword
		if (findStr(inputBuffer, &cmdBuffer, "pwd", TRUE))
		{
			printf("%s\n", dir); 
			continue;
		}
		
		//find cd keyword
		if (findStr(inputBuffer, &cmdBuffer, "cd", TRUE))
		{
			keywordCd(cmdBuffer);
			continue;
		}
		
		//find ls keyword
		if (findStr(inputBuffer, &cmdBuffer, "ls", TRUE))
		{		
			ls(cmdBuffer);
			continue;
		}
		
		//find cp keyword
		if (findStr(inputBuffer, &cmdBuffer, "cp", TRUE))
		{
			cp(cmdBuffer);
			continue;
		}
		
		//find alias keyword
		if (findStr(inputBuffer, &cmdBuffer, "alias", TRUE))
		{
			keywordAlias(cmdBuffer);
			continue;
		}
		
		//find unalias keyword
		if (findStr(inputBuffer, &cmdBuffer, "unalias", TRUE))
		{
			keywordUnalias(cmdBuffer);
			continue;
		}
		
		//find var keyword
		if (strstr(inputBuffer, "=") != NULL)
		{
			inputBuffer[strlen(inputBuffer) - 1] = '\0';
			keywordVar(inputBuffer);			
			continue;
		}
		
		//find unset keyword
		if (findStr(inputBuffer, &cmdBuffer, "unset", TRUE))
		{
			keywordUnset(cmdBuffer);
			continue;
		}
		
		if (!aliasFlag)	//if aliasFlag is false
		{
			inputBuffer[strlen(inputBuffer) - 1] = '\0'; //null terminate inputBuffer so it can be printed without newline
			
			if ((alias = findAlias(inputBuffer)) != NULL) //if input is an alias
			{					//copy alias into inputBuffer
				memcpy(inputBuffer, alias->value, strlen(alias->value) + 1);
				aliasFlag = TRUE;	//mark that input buffer contains an alias
				continue;	//continue
			}
		}
		
		aliasFlag = FALSE;	//if alias flag is true set it to false
		if (inputBuffer[0] !='\0')	//if inputBuffer is not empty
		{
			if (strstr(inputBuffer, "$") == NULL) //if $ is not found
			{
				printf("%s\n", inputBuffer); //print input
			}
			else  							//else if $ is found
			{		//replace var name with value and print to console if var name is valid
			//TODO check for vars first and expand into cmdBuffer 
			//NOTE: very quickly added functionality to see one var expanded
				int start = strstr(inputBuffer, "$") - inputBuffer;
				if (start == 0 || inputBuffer[start - 1] == ' ')
				{
					int end;
					if (strstr(inputBuffer + start, " ") != NULL)
					{
						end = strstr(inputBuffer + start, " ") - inputBuffer;
					}
					else
					{
						end = strlen(inputBuffer);
					}
					
					inputBuffer[end] = '\0';					
					if ((var = findVar(inputBuffer + start + 1)) != NULL)
					{
						inputBuffer[start] = '\0';
						printf("%s%s %s\n", inputBuffer, var->value, inputBuffer + end + 1);
					}
					else
					{
						printf("%s%s\n", inputBuffer, inputBuffer + end + 1);
					}
				}
				else
				{
					printf("%s\n", inputBuffer);
				}
			}
		}
		
	}
	
	return exitNum;
}


void keywordExit(char* cmdBuffer)
{
	//if exit is followed by a number
	if (cmdBuffer[0] >= '0' && cmdBuffer[0] <= '9')
	{
		exitNum = cmdBuffer[0] - '0';
		int temp = 1;
		// make the number into an int to use as return value
		while (cmdBuffer[temp] != '\0' && cmdBuffer[temp] >= '0' && cmdBuffer[temp] <= '9')
		{
			exitNum *= 10;
			exitNum += cmdBuffer[temp] - '0';
			temp++;
		}				
	}
	
	return;
}


void keywordCd(char* cmdBuffer)
{
	if (chdir(cmdBuffer) == 0) //if no errors then change pwd
	{
		getcwd(dir, SIZE);
	}
	else						//else
	{
		if (cmdBuffer[0] == '\0') //if cd is alone then change pwd to home dir
		{
			chdir(getenv("HOME"));
			getcwd(dir, SIZE);
		}
		else					//else
		if (cmdBuffer[0] == '~') //if cd is followed by ~	
		{
			if (cmdBuffer[1] != '\0') //check if ~ is alone
			{	
				if (cmdBuffer[1] != ' ') //if its not give apppropriate error
				{
					printf("cogSH: cd: %s: Not a directory\n", cmdBuffer);
					return;
				}
				
				int temp = 1;
				while (cmdBuffer[temp] == ' ')
				{
					temp++;
				}
				
				if (cmdBuffer[temp] != '\0')
				{
					printf("cogSH: cd: Too many arguments\n");
					return;
				}
			}
			
			chdir(getenv("HOME")); //if it is alone change pwd to home dir
			getcwd(dir, SIZE);					
		}
		else 
		{
			//and if (chdir(cmdBuffer) == 0) gave error then print error
			printf("cogSH: cd: %s: No such file or directory\n", cmdBuffer);
		}				
	}
	
	return;
}


void keywordAlias(char* cmdBuffer)
{
	char* tempStr = NULL;
	
	if (cmdBuffer[0] == '\0') //if alias is alone
	{
		for (alias = aliasTable; alias != NULL; alias = (struct aliasStruct*) (alias->hh.next))
		{	//print alias list
			printf("alias %s=\'%s\'\n", alias->name, alias->value);
		}
		return;
	}
	else									
	{	
		char* token = prectok(cmdBuffer, '\'', ' ');
		
		while (token != NULL)
		{
			int temp = 0;		
			if (strstr(token, "=") != NULL)
			{
				temp = strstr(token, "=") - token;
			}	
			
			if (temp != 0)	//check that = is in a valid spot
			{
				if (++aliasCount > MAX_ALIAS) //if max number of aliases has been reached 
				{								//give error
					printf("cogSH: alias: Maximum number of aliases exceeded\n");
					return;
				}
				else
				if (temp > ALIAS_SIZE)		//if before = is too long
				{							//give error
					printf("cogSH: alias: Alias name exceeds [%d] characters\n", ALIAS_SIZE - 1);
				}
				else				
				if ((strlen(token) - temp) > SIZE)	//if after = is too long
				{									//give error
					printf("cogSH: alias: Alias value exceeds [%d] characters\n", SIZE - 1);
				}
				else 			//else if no errors
				{	
					int flag = 0;
					if (strstr(token, "'") != NULL)  //if a single quote is in the token
					{
						token[strlen(token) - 1] = '\0'; //replacec closing quote with '\0'
						flag = 1;	//and flag that this token has single quotes
					}
						
					//put name of alias in temp array
					tempStr = (char*)malloc(sizeof(char) * (temp + 1));
					memcpy(tempStr, token, temp);
					tempStr[temp] = '\0';
					
					//add alias to list using name in temp arr and value as offset from main arr
					if (addAlias(tempStr, token + temp + 1 + flag)) //add an alias
					{
						aliasCount--;	//decrement aliasCount if an existing alias was overwritten
					}
				}
			}
			else	//if no = is found, or if its in an invalid spot
			{
				printf("cogSH: alias: %s: Not found\n", token);
			}
									
			token = prectok(NULL, '\'', ' '); //get next token
		}
	}
	
	if (tempStr != NULL)
	{
		free(tempStr);
	}
	
	return;
}


void keywordUnalias(char* cmdBuffer)
{
	char* token;

	if (cmdBuffer[0] != '\0') //if unalias is followed by args
	{
		if (cmdBuffer[0] == '-') //check for flags
		{
			if (cmdBuffer[1] == 'a') //if flag is [-a]
			{
				deleteAllAlias();	//then delete all aliases
				return;
			}
			else
			if (cmdBuffer[1] != '\0') //else if - is not alone print usage msg
			{
				printf("unalias: usage: unalias [-a] name [name ...]\n");
				return;
			}
		}
		
		token = strtok(cmdBuffer, " ");	//tokenize what comes after unalias
		
		while (token != NULL)				//loop through tokens
		{
			if (!deleteAlias(token)) //delete an alias if its valid and if the name is invalid
			{
				printf("cogSH: unalias: %s: Not found\n", token); //print error msg
			}
			else					//else if name is valid and alias was deleted
			{
				 aliasCount--;		//decrement aliasCount
			}
			
			token = strtok(NULL, " "); //get next token
		}
	}
	else			//if unalias is not followed by args then print usage msg
	{
		printf("unalias: usage: unalias [-a] name [name ...]\n");
	}
	
	return;
}


void keywordVar(char* cmdBuffer)
{
	char* tempStr = NULL;
	char* token = prectok(cmdBuffer, '"', ' ');
	
	while(token != NULL)
	{
		int temp = 0;		
		if (strstr(token, "=") != NULL)
		{
			temp = strstr(token, "=") - token;
		}	
		
		if (temp != 0)	//check that = is in a valid spot
		{
			if (++varCount > MAX_VAR) //if max number of variables has been reached 
			{								//give error
				printf("cogSH: var: Maximum number of variables exceeded\n");
			}
			else
			if (temp > VAR_SIZE)		//if before = is too long
			{							//give error
				printf("cogSH: var: Variable name exceeds [%d] characters\n", VAR_SIZE - 1);
			}
			else				
			if ((strlen(token) - temp) > SIZE)	//if after = is too long
			{									//give error
				printf("cogSH: var: Variable value exceeds [%d] characters\n", SIZE - 1);
			}
			else
			{
				int flag = 0;
				if (strstr(token, "\"") != NULL)  //if a double quote is in the token
				{
					token[strlen(token) - 1] = '\0'; //replace closing quote with '\0'
					flag = 1;	//and flag that this token has double quotes
				}
					
				//put name of alias in temp array
				tempStr = (char*)malloc(sizeof(char) * (temp + 1));
				memcpy(tempStr, token, temp);
				tempStr[temp] = '\0';
				
				//add var to list using name in temp arr and value as offset from main arr
				if (addVar(tempStr, token + temp + 1 + flag)) //add a var
				{
					varCount--;	//decrement varCount if an existing alias was overwritten
				}
			}					
		}
		else
		{
			printf("cogSH: var: %s: Not found\n", token);
		}
		
		token = prectok(NULL, '"', ' ');
	}
	
	if (tempStr != NULL)
	{
		free(tempStr);
	}
	
	return;
} 


void keywordUnset(char* cmdBuffer)
{
	char* token;

	if (cmdBuffer[0] != '\0') //if unset is followed by args
	{
		if (cmdBuffer[0] == '-') //check for flags
		{
			if (cmdBuffer[1] == 'a') //if flag is [-a]
			{
				deleteAllVars();	//then delete all variables
				return;
			}
			else
			if (cmdBuffer[1] != '\0') //else if - is not alone print usage msg
			{
				printf("unset: usage: unset [-a] name [name ...]\n");
				return;
			}
		}
		
		token = strtok(cmdBuffer, " ");	//tokenize what comes after unset
		
		while (token != NULL)				//loop through tokens
		{
			if (!deleteVar(token)) //delete a var if its valid and if the name is invalid then
			{
				printf("cogSH: unset: %s: Not found\n", token); //print error msg
			}
			else					//else if name is valid and var was deleted
			{
				 varCount--;		//decrement varCount
			}
			
			token = strtok(NULL, " "); //get next token
		}
	}
	else			//if unset is not followed by args then print usage msg
	{
		printf("unset: usage: unset [-a] name [name ...]\n");
	}
	
	return;
}
