#ifndef PRECTOK_C
#define PRECTOK_C


//function to get tokens from a string with precedence
//returns a char* of the token if found, else NULL
char* prectok(const char* s, char d1, char d2)
{	
	static char* str = NULL;
	static int start = 0;
	static int end = 0;
	static int length = 0;
	
	char* rtnStr;
	int open = 0;
	
	if (s != NULL)
	{	//copy char* s arg to local static str
		length = strlen(s);
		
		str = (char*)malloc(sizeof(char) * length + 1);
		memcpy(str, s, length);
		str[length] = '\0';			
		
		start = end = open = 0;
	}
	
	if (str != NULL)
	{
		while (1)
		{
			if (str[end] == d1) //check for delimeter with precedence
			{
				open ? open-- : open++;	//keep track of opening and closing predence delimeter
			}
			else
			if (!open && (str[end] == d2 || str[end] == '\0')) //if delimiter with precedence is not open, then check for secondary delimeter
			{	
				if (start == end)
				{
					start = ++end;
					continue;
				}

				//copy from start offset to end offset into a temp arr if secondary delimeter is found and primary is not open
				rtnStr = (char*)malloc(sizeof(char) * (end - start + 1));
				memcpy(rtnStr, str + start, end - start);
				rtnStr[end] = '\0';
				
				if (str[end] == '\0') //if end of string is reached reset all vars
				{
					str = NULL;
					open = 0;
					length = 0;
					
					if (start == end)
					{
						end = start = 0;
						
						return NULL;
					}
					
					end = -1;
				}
				
				end = start = end + 1;	
				
				return rtnStr;		//return token if found
			}
			
			if (end++ > length)	//ensure that end is not out of bounds
			{
				break;
			}
		}	
	}
	
	return NULL;	//return NULL if string is NULL or primary delimeter is not closed
}


#endif
