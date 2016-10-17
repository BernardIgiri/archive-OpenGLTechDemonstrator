#ifndef __LOGGINGSYSTEM_INCLUDED__
#define __LOGGINGSYSTEM_INCLUDED__
#include <stdio.h>
class CLog
	{
	public:
		CLog(char *path);			// Creates The System
		~CLog();				// Destroys The System

		void lprintf(char *text,...);		// Logs To The System

	private:
		// Opens The Log
		void Open() { if(!logh) logh = fopen(szPath,"a"); };
		// Closes The Log
		void Close() { if(logh!=NULL) { fclose(logh); logh = NULL; } }; 
	public:
		char *szPath;				// Log Filepath
		FILE *logh;				// Log Handle
	};
#endif


