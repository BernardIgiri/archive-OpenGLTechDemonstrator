#include "log.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

	CLog::CLog(char *path)
	{
		szPath = path;				// Save The Path
		logh=NULL;
		logh = fopen(path,"w");			// Open The File, Clear Contents
		if(!logh)				// If There Was An Error, Gripe About It
			MessageBox(NULL,"Log Error","ERROR",MB_OK);
		Close();				// Close The Log
	}

	CLog::~CLog()
	{
		Close();				// Just In Case It Was Somehow Left Open
	}

	void CLog::lprintf(char *text,...)
	{
		//static CTime theTime = CTime::GetCurrentTime();
		//static const char *weekDayMap[]={"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
		static struct tm *newtime;
		static time_t aclock;
		time( &aclock );
		newtime = localtime( &aclock );
		Open();					// Open The Log File For Writing
		if(!logh)				// If There Was An Error, Stop
			return;
		// Create The Buffer
		va_list arglist;
		char buff[1024];
		memset(buff,0,sizeof(buff));

		va_start(arglist,text);
		//int i = sprintf(buff,"%.19s.%hu %s\n",timeline,timebuffer.millitm,&timeline[20]);
		/*int i = sprintf(buff,"%s %02i/%02i/%04i %02i:%02i Second %i",
			weekDayMap[theTime.GetDayOfWeek()],
			theTime.GetMonth(),theTime.GetDay(),theTime.GetYear(),
			theTime.GetHour(),theTime.GetMinute(),theTime.GetSecond());*///int i=0;
		int i = sprintf(buff,"Time: %s",asctime( newtime ));
		vsprintf(buff+i,text,arglist);
		strcat(buff,"\n");
		va_end(arglist);
		fprintf(logh,buff);			// Write To The File
		Close();				// Close The Log
	}
