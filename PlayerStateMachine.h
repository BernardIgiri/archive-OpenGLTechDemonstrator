////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************

  PLAYER STATE MACHINE CLASS DEFINITION

***********************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __PLAYERSTATEMACHINE_INCLUDED__
#define __PLAYERSTATEMACHINE_INCLUDED__
#include "PlayerDefines.h"
#include "log.h"
class CPlayerStateMachine
{
public:
	CPlayerStateMachine(CLog *pLog);
	~CPlayerStateMachine();
	bool Initialize();
	void Shutdown();
	bool LoadPlayerData(char *filename);
	bool SavePlayerData();

	void SetActionKey(bool state);
	void CheckActionKey(bool &state);

	void ActionMovePlayer(float direction,bool run);
	void ActionSetPos(plyrGeographicState_t *newLocation);
	void ActionSetStoryPosition(unsigned int position);
	void ActionSetMissionGoal(int goal, bool state);
	void ActionEquipItem(int itemType,int inventoryIndex,int itemNum=-1);
	void ActionGiveItem(int itemType,plyrItemID_t id);
	void ActionTakeItem(int itemType,plyrItemID_t id);
	void ActionAlterRelationship(int relationship, int change);
	void ActionAlterCritStats(int healthChange,int magicChange);
	void ActionRaiseStats(plyrStats_t *statIncrements);
	void GetStoryState(plyrStoryState_t *story);
	void GetMotionState(plyrVolatileState_t *volState);
	void GetPosition(plyrGeographicState_t *position);
	void GetStats(plyrCOMFullStats_t *stats);
	void GetEquipData(plyrCOMEquipData_t *equipped);
protected:
	plyrVolatileState_t playerState;
	CLog				*m_pLog;
};
#endif
/*

  Setting Data Directly may result in errors when the systems setting the new
  data do so from origin copies that are different versions.

*/