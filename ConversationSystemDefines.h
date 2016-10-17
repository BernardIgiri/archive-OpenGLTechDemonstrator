#ifndef __INCLUDED_CONVERSATIONSYSTEMDEFINES__
#define __INCLUDED_CONVERSATIONSYSTEMDEFINES__
#define CONVSYS_ACTION_NONE			0
#define CONVSYS_ACTION_ADVSTORY		1
#define CONVSYS_ACTION_SETGOALS		2
#define CONVSYS_ACTION_CHNGRELATION	3
#define CONVSYS_ACTION_GIVEOBJ		4
#define CONVSYS_ACTION_TAKEOBJ		5
#define CONVSYS_ACTION_PLAYSOUND	6
#define CONVSYS_ACTION_ANIMATE		7
#define CONVSYS_ACTION_SPEECHBRANCH	8
#define CONVSYS_ACTION_RANDOMBRANCH	9
#define CONVSYS_ACTION_GOALBRANCH	10
#define CONVSYS_ACTION_RELBRANCH	11
#define CONVSYS_ACTION_STORYBRANCH	12

typedef struct
{
	int nActions;
	CONVSYS_STORYBRANCH_t storyBranch;
} CONVSYS_FILEHEADER_t;

typedef struct
{
	int action;
	void *data;
} CONVSYS_ACTION_t;

typedef struct
{
	int storyPos;
} CONVSYS_ADVSTORY_t;

typedef struct
{
	int nGoals;
	_CONVSYS_GOAL_t *goals;
} CONVSYS_SETGOALS_t;

typedef struct _CONVSYS_GOAL_t
{
	int goal;
	char status;
} CONVSYS_GOAL_t;

typedef struct
{
	int change;
} CONVSYS_CHNGRELATION_t;

typedef struct
{
	int objType;
	int objID;
} CONVSYS_GIVEOBJ_t;

typedef CONVSYS_GIVEOBJ_t CONVSYS_TAKEOBJ_t;

typedef struct
{
	int length;
	char *str;
} CONVSYS_STRING_t;

typedef CONVSYS_STRING_t;

typedef CONVSYS_STRING_t CONVSYS_PLAYSOUND_t;

typedef struct
{
	int aniID;
} CONVSYS_ANIMATE_t;

typedef struct
{
	int percent;
	int lnHeads;
	int lnTails;
} CONVSYS_RANDOMBRANCH_t;

typedef struct
{
	CONVSYS_SETGOALS_t reqGoals;
	int lnTrue;
	int lnFalse;
} CONVSYS_GOALBRANCH_t;

typedef struct
{
	int lVal;
	int hVal;
	int lnLow;
	int lnHigh;
} CONVSYS_RELBRANCH_t;

typedef struct
{
	int nConditions;
	CONVSYS_STORYCONDITION_t *conditions;
} CONVSYS_STORYBRANCH_t;

typedef struct
{
	int lVal;
	int hVal;
	int lnTrue; // line to go to if between
} CONVSYS_STORYCONDITION_t;

typedef struct
{
	CONVSYS_STRING_t msg;
	int nReplies;
	CONVSYS_REPLY_t *pReplies;
} CONVSYS_SPEECHBRANCH_t;

typedef struct
{
	int lnTrue;
	CONVSYS_STRING_t msg;
} CONVSYS_REPLY_t

#endif