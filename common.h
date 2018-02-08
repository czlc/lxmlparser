#define LOG_PROCESS_ERROR(Condition)	\
	do									\
	{									\
	if (!(Condition))				\
		{								\
		printf("LOG_PROCESS_ERROR(%s) at line %d in %s\n",	#Condition, __LINE__, __FUNCTION__);	\
		goto Exit0;					\
		}								\
	} while (false)


#define PROCESS_ERROR(Condition)	\
	do									\
	{									\
	if (!(Condition))				\
		{								\
		goto Exit0;					\
		}								\
	} while (false)

static int GetFileSize(FILE *pFile)
{
	long nOffset = ftell(pFile);
	fseek(pFile, 0, SEEK_END);
	long nSize = ftell(pFile);
	fseek(pFile, nOffset, SEEK_SET);

	return nSize;
}