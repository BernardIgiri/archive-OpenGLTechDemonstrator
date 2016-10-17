#include "PlayerStateMachine.h"
#include <memory.h>
#include <stdlib.h>
CPlayerStateMachine::CPlayerStateMachine(CLog *pLog)
{
	m_pLog=pLog;
}
CPlayerStateMachine::~CPlayerStateMachine()
{
	if (NULL!=playerState.playerSaveFile || NULL!=playerState.staticState)
		Shutdown();
}
bool CPlayerStateMachine::Initialize()
{
	static bool isFirstTime = true;
	playerState.actionState = 0.0f;
	playerState.currentHeading = 0.0f;
	playerState.health = 0;
	playerState.lookAngle = 0.0f;
	playerState.magicEnergy = PLYR_MAXCRITSTATVALUE;
	playerState.playerAction = PLYR_ACTION_STAND;
	try
	{
		if (isFirstTime)
		{
			playerState.playerSaveFile = NULL;
			playerState.staticState = NULL;
			playerState.staticState = new plyrStaticState_t;
		}
		else
		{
			if (NULL!=playerState.playerSaveFile)
			{
				free(playerState.playerSaveFile);
				playerState.playerSaveFile=NULL;
			}
			if (NULL!=playerState.staticState)
			{
				delete playerState.staticState;
				playerState.staticState=NULL;
				playerState.staticState = new plyrStaticState_t;
			}
		}
		// equiptment clearing
		playerState.staticState->equiptment.equippedArmorChestArmor=-1;
		playerState.staticState->equiptment.equippedArmorChestLining=-1;
		playerState.staticState->equiptment.equippedArmorGuantlets=-1;
		playerState.staticState->equiptment.equippedArmorHelmets=-1;
		playerState.staticState->equiptment.equippedArmorLeggings=-1;
		playerState.staticState->equiptment.equippedArmorSheilds=-1;
		playerState.staticState->equiptment.equippedArmorSleeves=-1;
		for (int i=0; i<PLYR_NUMBEROFPOCKETS; i++)
			playerState.staticState->equiptment.equippedItem[i]=-1;
		playerState.staticState->equiptment.equippedWeaponLeftHand=-1;
		playerState.staticState->equiptment.equippedWeaponRightHand=-1;
		// inventory clearing
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.armorChestArmor[i].count=0;
			playerState.staticState->inventory.armorChestArmor[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.armorChestArmor[i].wear=0;
		}
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.armorChestLining[i].count=0;
			playerState.staticState->inventory.armorChestLining[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.armorChestLining[i].wear=0;
		}
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.armorGauntlets[i].count=0;
			playerState.staticState->inventory.armorGauntlets[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.armorGauntlets[i].wear=0;
		}
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.armorHelmets[i].count=0;
			playerState.staticState->inventory.armorHelmets[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.armorHelmets[i].wear=0;
		}
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.armorLeggings[i].count=0;
			playerState.staticState->inventory.armorLeggings[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.armorLeggings[i].wear=0;
		}
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.armorSheilds[i].count=0;
			playerState.staticState->inventory.armorSheilds[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.armorSheilds[i].wear=0;
		}
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.armorSleeves[i].count=0;
			playerState.staticState->inventory.armorSleeves[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.armorSleeves[i].wear=0;
		}
		for (i = 0; i<PLYR_ITEMINVENTORYSIZE; i++)
		{
			playerState.staticState->inventory.items[i].count=0;
			playerState.staticState->inventory.items[i].id=0;
		}
		for (i = 0; i<PLYR_MAXPAYLOAD; i++)
		{
			playerState.staticState->inventory.weapons[i].count=0;
			playerState.staticState->inventory.weapons[i].id=PLYR_ITEMTYPE_NONE;
			playerState.staticState->inventory.weapons[i].wear=0;
		}
		// location clearing
		playerState.staticState->location.environmentIndex=0;
		playerState.staticState->location.environmentType=0;
		playerState.staticState->location.positionX=0;
		playerState.staticState->location.positionY=0;
		// stat clearing
		playerState.staticState->stamina=PLYR_MAXCRITSTATVALUE;
		playerState.staticState->stats.agility=0;
		playerState.staticState->stats.magic=0;
		playerState.staticState->stats.speed=0;
		playerState.staticState->stats.spirit=0;
		playerState.staticState->stats.strength=0;
		playerState.staticState->stats.vitality=0;
		// story clearing
		for (i = 0; i<PLYR_NUMBEROFMISSIONGOALS; i++)
			playerState.staticState->story.missionGoal[i]=false;
		for (i = 0; i<PLYR_NUMBEROFRELATIONSHIPS; i++)
			playerState.staticState->story.relationships[i]=0;
		playerState.staticState->story.storyPosition=0;
		playerState.actionKey=false;
	}
	catch(...)
	{
		if (isFirstTime)
			m_pLog->lprintf("The PlayerStateMachine failed to Initialize.");
		else
			m_pLog->lprintf("The PlayerStateMachine failed to Re-Initialize.");
		throw;
	}
	isFirstTime = false;
	return true;
}
void CPlayerStateMachine::Shutdown()
{
	if (NULL!=playerState.playerSaveFile)
		free(playerState.playerSaveFile);
	if (NULL!=playerState.staticState)
		delete playerState.staticState;
}

/**************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////

									A C T I O N S

///////////////////////////////////////////////////////////////////////////////////////
**************************************************************************************/
// Action Key Routines
/**************************************************************************************

  Sets Action Key State.

**************************************************************************************/
void CPlayerStateMachine::SetActionKey(bool state)
{
	playerState.actionKey=state;
}
/**************************************************************************************

  Gets Action Key State.

**************************************************************************************/
void CPlayerStateMachine::CheckActionKey(bool &state)
{
	state=playerState.actionKey;
}

/**************************************************************************************

  Moves the player in specified direction with specified animation (run/walk)

**************************************************************************************/
void CPlayerStateMachine::ActionMovePlayer(float direction, bool run)
{
	if (playerState.playerAction!=PLYR_ACTION_WALK&&
		playerState.playerAction!=PLYR_ACTION_RUN&&
		playerState.playerAction!=PLYR_ACTION_STAND&&
		playerState.actionState>0.0f&&
		playerState.actionState<1.0f) // if doing a non-move action and not yet done, then do nothing until act is done
		return;
	if (direction==playerState.currentHeading&&
		playerState.playerAction==(unsigned)(run ? PLYR_ACTION_RUN : PLYR_ACTION_WALK)&&
		playerState.actionState<1.0f) // if same action then ignore
		return;
	playerState.currentHeading=direction;
	playerState.actionState=0.0f;
	playerState.playerAction=(run ? PLYR_ACTION_RUN : PLYR_ACTION_WALK);
}
/**************************************************************************************

  Changes the position of the player.

**************************************************************************************/
void CPlayerStateMachine::ActionSetPos(plyrGeographicState_t *newLocation)
{
	memcpy(&playerState.staticState->location,newLocation,sizeof(plyrGeographicState_t));
}
/**************************************************************************************

  Changes Current Placement in story.

**************************************************************************************/
void CPlayerStateMachine::ActionSetStoryPosition(unsigned int position)
{
	playerState.staticState->story.storyPosition=position;
}
/**************************************************************************************

  Sets a mission goal.

**************************************************************************************/
void CPlayerStateMachine::ActionSetMissionGoal(int goal,bool state)
{
	playerState.staticState->story.missionGoal[goal]=state;
}
/**************************************************************************************

  Alters player relationship.

**************************************************************************************/
void CPlayerStateMachine::ActionAlterRelationship(int relationship,int change)
{
	playerState.staticState->story.relationships[relationship]+=(plyrRelationship_t)change;
}
/**************************************************************************************

  Equips Requested Item.

**************************************************************************************/
void CPlayerStateMachine::ActionEquipItem(int itemType,int inventoryIndex,int itemNum)
{
	switch (itemType)
	{
	case PLYR_ITEMTYPE_ITEM:
		if (itemNum<0)
			return;
		playerState.staticState->equiptment.equippedItem[itemNum]=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_WEAPONRHAND:
		playerState.staticState->equiptment.equippedWeaponRightHand=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_WEAPONLHAND:
		playerState.staticState->equiptment.equippedWeaponLeftHand=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_ARCHESTLINE:
		playerState.staticState->equiptment.equippedArmorChestLining=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_ARCHESTARMOR:
		playerState.staticState->equiptment.equippedArmorChestArmor=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_ARSLEEVES:
		playerState.staticState->equiptment.equippedArmorSleeves=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_ARGUANTLETES:
		playerState.staticState->equiptment.equippedArmorGuantlets=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_ARLEGGINGS:
		playerState.staticState->equiptment.equippedArmorLeggings=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_ARHELMET:
		playerState.staticState->equiptment.equippedArmorHelmets=inventoryIndex;
		break;
	case PLYR_ITEMTYPE_ARSHEILD:
		playerState.staticState->equiptment.equippedArmorSheilds=inventoryIndex;
		break;
	default:
		return;
		break;
	}
}
/**************************************************************************************

  Adds an item to the player inventory.

**************************************************************************************/
void CPlayerStateMachine::ActionGiveItem(int itemType,plyrItemID_t id)
{
	if (itemType==PLYR_ITEMTYPE_NONE)
		return;
	if (itemType==PLYR_ITEMTYPE_ITEM)
	{
		for (int i=0;i<PLYR_ITEMINVENTORYSIZE;i++)
		{
			if (playerState.staticState->inventory.items[i].id==id)
			{
				playerState.staticState->inventory.items[i].count++;
				return;
			}
			else if (playerState.staticState->inventory.items[i].id==0)
			{
				playerState.staticState->inventory.items[i].id=id;
				playerState.staticState->inventory.items[i].count=1;
				/////// missing ACCESS DATA - for mission goals !!!!!!!!
				return;
			}
		}
		return;
	}
	plyrEquiptmentState_t *array=NULL;
	switch (itemType)
	{
	case PLYR_ITEMTYPE_WEAPON:
		array=playerState.staticState->inventory.weapons;
		break;
	case PLYR_ITEMTYPE_ARCHESTLINE:
		array=playerState.staticState->inventory.armorChestLining;
		break;
	case PLYR_ITEMTYPE_ARCHESTARMOR:
		array=playerState.staticState->inventory.armorChestArmor;
		break;
	case PLYR_ITEMTYPE_ARSLEEVES:
		array=playerState.staticState->inventory.armorSleeves;
		break;
	case PLYR_ITEMTYPE_ARGUANTLETES:
		array=playerState.staticState->inventory.armorGauntlets;
		break;
	case PLYR_ITEMTYPE_ARLEGGINGS:
		array=playerState.staticState->inventory.armorLeggings;
		break;
	case PLYR_ITEMTYPE_ARHELMET:
		array=playerState.staticState->inventory.armorHelmets;
		break;
	case PLYR_ITEMTYPE_ARSHEILD:
		array=playerState.staticState->inventory.armorSheilds;
		break;
	default:
		return;
		break;
	}
	for (int i=0;i<PLYR_MAXPAYLOAD;i++)
	{
		if (array[i].id==id)
		{
			array[i].count++;
			return;
		}
		else if (array[i].id==0)
		{
			array[i].id=id;
			array[i].count=1;
			array[i].wear=0;
			/////// missing ACCESS DATA - for mission goals !!!!!!!!
			return;
		}
	}
}
/**************************************************************************************

  Removes an item from the player inventory.

**************************************************************************************/
void CPlayerStateMachine::ActionTakeItem(int itemType,plyrItemID_t id)
{
	if (itemType==PLYR_ITEMTYPE_NONE||id==0)
		return;
	if (itemType==PLYR_ITEMTYPE_ITEM)
	{
		for (int i=0;i<PLYR_ITEMINVENTORYSIZE;i++)
		{
			if (playerState.staticState->inventory.items[i].id==id)
			{
				if (playerState.staticState->inventory.items[i].count<1)
					playerState.staticState->inventory.items[i].count=1;
				playerState.staticState->inventory.items[i].count--;
				if (playerState.staticState->inventory.items[i].count<1)
				{
					/////// missing ACCESS DATA - for mission goals !!!!!!!!
					playerState.staticState->inventory.items[i].id=0;
					playerState.staticState->inventory.items[i].count=0;
				}
				return;
			}
			else if (playerState.staticState->inventory.items[i].id==0)
				return;
		}
		return;
	}
	plyrEquiptmentState_t *array=NULL;
	switch (itemType)
	{
	case PLYR_ITEMTYPE_WEAPON:
		array=playerState.staticState->inventory.weapons;
		break;
	case PLYR_ITEMTYPE_ARCHESTLINE:
		array=playerState.staticState->inventory.armorChestLining;
		break;
	case PLYR_ITEMTYPE_ARCHESTARMOR:
		array=playerState.staticState->inventory.armorChestArmor;
		break;
	case PLYR_ITEMTYPE_ARSLEEVES:
		array=playerState.staticState->inventory.armorSleeves;
		break;
	case PLYR_ITEMTYPE_ARGUANTLETES:
		array=playerState.staticState->inventory.armorGauntlets;
		break;
	case PLYR_ITEMTYPE_ARLEGGINGS:
		array=playerState.staticState->inventory.armorLeggings;
		break;
	case PLYR_ITEMTYPE_ARHELMET:
		array=playerState.staticState->inventory.armorHelmets;
		break;
	case PLYR_ITEMTYPE_ARSHEILD:
		array=playerState.staticState->inventory.armorSheilds;
		break;
	default:
		return;
		break;
	}
	for (int i=0;i<PLYR_MAXPAYLOAD;i++)
	{
		if (array[i].id==id)
		{
			if (array[i].count<1)
				array[i].count=1;
			array[i].count--;
			if (array[i].count<1)
			{
				/////// missing ACCESS DATA - for mission goals !!!!!!!!
				array[i].id=0;
				array[i].wear=0;
				array[i].count=0;
			}
			return;
		}
		else if (array[i].id==0)
			return;
	}
}
/**************************************************************************************

  Changes Player Health and Magic Levels.

**************************************************************************************/
void CPlayerStateMachine::ActionAlterCritStats(int healthChange,int magicChange)
{
	if (((int)playerState.magicEnergy+magicChange)<0)
		playerState.magicEnergy=0;
	else
		playerState.magicEnergy=(plyrCritStat_t)((int)playerState.magicEnergy+magicChange);
	if (((int)playerState.health+healthChange)<0)
		playerState.health=0;
	else
		playerState.health=(plyrCritStat_t)((int)playerState.health+healthChange);
}
/**************************************************************************************

  Changes Player Stats.

**************************************************************************************/
void CPlayerStateMachine::ActionRaiseStats(plyrStats_t *statIncrements)
{
	playerState.staticState->stats.agility += statIncrements->agility;
	playerState.staticState->stats.magic += statIncrements->magic;
	playerState.staticState->stats.speed += statIncrements->speed;
	playerState.staticState->stats.spirit += statIncrements->spirit;
	playerState.staticState->stats.strength += statIncrements->strength;
	playerState.staticState->stats.vitality += statIncrements->vitality;
}

/**************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////

								G E T    C O M M A N D S

///////////////////////////////////////////////////////////////////////////////////////
**************************************************************************************/

/**************************************************************************************

  Copies Motion State data to specified volState structure.

**************************************************************************************/
void CPlayerStateMachine::GetMotionState(plyrVolatileState_t *volState)
{
	memcpy(volState,&playerState,sizeof(plyrVolatileState_t));
	volState->staticState=NULL; // block pointer acess
	volState->playerSaveFile=NULL; // block pointer acess
}
/**************************************************************************************

  Gets Current Geographic Location of Player.

**************************************************************************************/
void CPlayerStateMachine::GetPosition(plyrGeographicState_t *position)
{
	memcpy(position,&playerState.staticState->location,sizeof(plyrGeographicState_t));
}
/**************************************************************************************

  Gets Current Player Stats.

**************************************************************************************/
void CPlayerStateMachine::GetStats(plyrCOMFullStats_t *stats)
{
	memcpy(&stats->stats,&playerState.staticState->stats,sizeof(plyrCOMFullStats_t));
	stats->health=playerState.health;
	stats->magicEnergy=playerState.magicEnergy;
}
/**************************************************************************************

  Gets Current Player Story State.

**************************************************************************************/
void CPlayerStateMachine::GetStoryState(plyrStoryState_t *story)
{
	memcpy(story,&playerState.staticState->story,sizeof(plyrStoryState_t));
}
/**************************************************************************************

  Gets Current Player Inventory and Equiptment State.

**************************************************************************************/
void CPlayerStateMachine::GetEquipData(plyrCOMEquipData_t *equipped)
{
	memcpy(&equipped->equiptment,&playerState.staticState->equiptment,sizeof(plyrEquippedState_t));
	memcpy(&equipped->inventory,&playerState.staticState->inventory,sizeof(plyrInventoryState_t));
}