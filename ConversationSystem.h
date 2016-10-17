#ifndef __INCLUDED_CONVERSATIONSYSTEM__
#define __INCLUDED_CONVERSATIONSYSTEM__
#include "ConversationSystemDefines.h"
#include "Log.h"

class ConverseSys
{
public:
	ConverseSys();
	~ConverseSys();
	void Initialize(CLog *pLog);
	void LoadFile(const char *fileName);
	void InsertData(CONVSYS_FILEHEADER_t *header,CONVSYS_ACTION_t *actionList);
	bool ProcessData();
	void ClearData();
protected:
	CLog*				  m_pLog;
	CONVSYS_FILEHEADER_t  m_header;
	CONVSYS_ACTION_t*	  m_actionList;
	int					  m_index;
	bool				  m_isOk;
};

#endif