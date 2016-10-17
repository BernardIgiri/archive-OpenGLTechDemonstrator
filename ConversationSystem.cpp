#include "ConversationSystem.h"

ConverseSys::ConverseSys()
{
	m_pLog=NULL;
	memset(m_header,NULL,sizeof(CONVSYS_FILEHEADER_t));
	m_actionList=NULL;
	m_index=0;
	m_isOk=false;
}
ConverseSys::~ConverseSys()
{
	ClearData();
}
ConverseSys::Initialize(CLog *pLog)
{
	m_pLog=pLog;
}
void ConverseSys::LoadFile(const char *fileName)
{
	m_isOk=false;
	if (fileName==NULL)
	{
		m_pLog->lprintf("ConverseSys error: Received NULL filename.");
		return;
	}
	FILE *filePtr;
	filePtr=fopen(fileName, "rb");
	if (filePtr==NULL)
	{
		m_pLog->lprintf("ConverseSys error: Could not open file %s.",fileName);
		return;
	}
	fread(&m_header,sizeof(CONVSYS_FILEHEADER_t),1,filePtr);
	if (m_header.nActions<1)
	{
		m_pLog->lprintf("ConverseSys error: File %s contains invalid data.",fileName);
		return;
	}
	if (m_actionList!=NULL)
		free(m_actionList);
	m_actionList = (CONVSYS_ACTION_t *)malloc(sizeof(CONVSYS_ACTION_t)*m_header.nActions);
	m_index=0;
	bool done=false;
	while (!done)
	{
		int action;
		fread(&action,sizeof(int),1,filePtr);
		m_actionList[index].action=action;
		switch (action)
		{
		case CONVSYS_ACTION_NONE:
			done=true;
			break;
		case CONVSYS_ACTION_ADVSTORY:
			m_actionList[index].data=malloc(sizeof(CONVSYS_ADVSTORY_t));
			fread(m_actionList[index].data,sizeof(CONVSYS_ADVSTORY_t),1,filePtr);
			break;
		case CONVSYS_ACTION_SETGOALS:
			{
				m_actionList[index].data=malloc(sizeof(CONVSYS_SETGOALS_t));				
				CONVSYS_SETGOALS_t* goals=(CONVSYS_SETGOALS_t*)m_actionList[index].data;
				fread(&goals->nGoals,sizeof(int),1,filePtr);
				if (goals->nGoals<1)
				{
					m_pLog->lprintf("ConverseSys error: File %s contains invalid data.",fileName);
				}
				goals->goals =(_CONVSYS_GOAL_t *)malloc(sizeof(_CONVSYS_GOAL_t)*goals->nGoals);
				fread(goals->goals,sizeof(_CONVSYS_GOAL_t)*goals->nGoals,1,filePtr);
			}
			break;
		case CONVSYS_ACTION_CHNGRELATION:
			m_actionList[index].data=malloc(sizeof(CONVSYS_CHNGRELATION_t));
			fread(m_actionList[index].data,sizeof(CONVSYS_CHNGRELATION_t),1,filePtr);
			break;
		case CONVSYS_ACTION_GIVEOBJ:
			m_actionList[index].data=malloc(sizeof(CONVSYS_GIVEOBJ_t));
			fread(m_actionList[index].data,sizeof(CONVSYS_GIVEOBJ_t),1,filePtr);
			break;
		case CONVSYS_ACTION_TAKEOBJ:
			m_actionList[index].data=malloc(sizeof(CONVSYS_TAKEOBJ_t));
			fread(m_actionList[index].data,sizeof(CONVSYS_TAKEOBJ_t),1,filePtr);
			break;
		case CONVSYS_ACTION_SPEECHBRANCH:
			{
				m_actionList[index].data=malloc(sizeof(CONVSYS_SPEAK_t));				
				CONVSYS_SPEAK_t* speech=(CONVSYS_SPEAK_t*)m_actionList[index].data;
				fread(&speech->length,sizeof(int),1,filePtr);
				if (speech->length<1)
				{
					m_pLog->lprintf("ConverseSys error: File %s contains invalid data.",fileName);
				}
				speech->msg =(char *)malloc(sizeof(char)*speech->length);
				fread(speech->msg,sizeof(char)*speech->length,1,filePtr);
			}
				
		default:
			break;
		}
		m_index++;
		if (m_index>=m_header.nActions)
			done=true;
	}

			/*#define CONVSYS_ACTION_NONE			0
#define CONVSYS_ACTION_ADVSTORY		1
#define CONVSYS_ACTION_SETGOALS		2
#define CONVSYS_ACTION_CHNGRELATION	3
#define CONVSYS_ACTION_GIVEOBJ		4
#define CONVSYS_ACTION_TAKEOBJ		5
#define CONVSYS_ACTION_SPEAK		6
#define CONVSYS_ACTION_PLAYSOUND	7
#define CONVSYS_ACTION_ANIMATE		8
#define CONVSYS_ACTION_RANDOMBRANCH	9
#define CONVSYS_ACTION_GOALBRANCH	10
#define CONVSYS_ACTION_RELBRANCH	11
#define CONVSYS_ACTION_STORYBRANCH	12*/
