#ifndef FINDSTR_C
#define FINDSTR_C

#define TRUE 1
#define FALSE 0


//find find_str in inputBuffer and place whatever comes after it into cmdBuffer
int findStr(const char* inputBuffer, char** cmdBuffer, const char* find_str, short spaceTerminated)
{
	if (inputBuffer != NULL && inputBuffer[0] != '\0')
	{	
		char* start; //location that find_str is found at in inputBuffer
		int length; //length of inputBuffer from after find_str until \n
		int find_length = strlen(find_str); //length of find_str

		if ((start = strstr(inputBuffer, find_str)) != NULL) //if find_str is found in inputBuffer
		{		
			//if find_str is not its own word and return 0
			if (spaceTerminated && start[find_length] != ' ' && start[find_length] != '\n' && start[find_length] != '\0')
			{
				return 0;
			}
		
			int i = 0;
			while (inputBuffer[i] == ' ') //set i to first non-space character
			{
				i++;
			}
			
			if (start - (inputBuffer + i) == 0) //if find_str is found at the first non-space character in inputBuffer
			{
				if (spaceTerminated) //if not space terminated
				{
					while (start[find_length] == ' ') //set find_length to first non-space character
					{
						find_length++;
					}
				}
				
				//set length to length of inputBuffer from end of find_str to \n while ignoring space
				if (strstr(inputBuffer, "\n") == NULL)
				{
					length = strlen(inputBuffer) + 1;
				}
				else
				{
					length = strstr(inputBuffer, "\n") - inputBuffer;
				}
				
				length -= find_length;
				
				//malloc space to cmdBuffer and copy chars after find_str and before \n from inputBuffer into cmdBuffer, then set terminating \0 at the end
				*cmdBuffer = (char*)malloc(sizeof(char) * (length + 1));
				memcpy(*cmdBuffer, start + find_length, length);
				*(*cmdBuffer + length) = '\0';
				
				return 1;
			}
		}
	}
	
	return 0;
}


#endif
