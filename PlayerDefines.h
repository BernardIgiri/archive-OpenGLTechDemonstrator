#ifndef __PLAYERDEFINES_INCLUDED__
#define __PLAYERDEFINES_INCLUDED__
#include "basetsd.h"
// INVENTORY DEFINES
#define PLYR_MAXPAYLOAD			20
#define PLYR_ITEMINVENTORYSIZE	30
#define PLYR_NUMBEROFPOCKETS	6 // will be used to hold items for combat use
// ENVIRONMENT DEFINES
#define PLYR_ENVIRONMENTTYPE_WORLDMAP	0
#define PLYR_ENVIRONMENTTYPE_TOWN		1
#define PLYR_ENVIRONMENTTYPE_DUNGEON	2
#define PLYR_ENVIRONMENTTYPE_COMBAT		3
// PLAYER STAT DEFINES
#define PLYR_LEVELGAININCREMENT 4194304 // once level reached stat becomes 1 more
// STORY DEFINES
#define PLYR_NUMBEROFRELATIONSHIPS	5
#define PLYR_NUMBEROFMISSIONGOALS	10
// CRITICAL STATUS DEFINES
#define PLYR_MAXCRITSTATVALUE 10000 // used to calculate percentage, all crit stats are represented as percentages
// PLAYER ACTION DEFINES
#define PLYR_ACTION_STAND		0
#define PLYR_ACTION_WALK		1
#define PLYR_ACTION_RUN			2
#define PLYR_ACTION_ATTACK		100 // + id of attack type
#define PLYR_ACTION_CASTSPELL	200 // + id of spell
// PLAYER ITEM TYPE DEFINES
#define PLYR_ITEMTYPE_NONE			0
#define PLYR_ITEMTYPE_ITEM			1
#define PLYR_ITEMTYPE_WEAPON		2
#define PLYR_ITEMTYPE_ARCHESTLINE	3
#define PLYR_ITEMTYPE_ARCHESTARMOR	4
#define PLYR_ITEMTYPE_ARSLEEVES		5
#define PLYR_ITEMTYPE_ARGUANTLETES	6
#define PLYR_ITEMTYPE_ARLEGGINGS	7
#define PLYR_ITEMTYPE_ARHELMET		8
#define PLYR_ITEMTYPE_ARSHEILD		9
#define PLYR_ITEMTYPE_WEAPONLHAND	10
#define PLYR_ITEMTYPE_WEAPONRHAND	PLYR_ITEMTYPE_WEAPON

// GAME TYPE DEFINES
typedef UINT32 plyrStat_t;
typedef INT16 plyrCritStat_t;
typedef signed char plyrRelationship_t;
typedef unsigned int plyrItemID_t;

// STRUCTURE DEFINES
typedef struct
{
	plyrItemID_t	id;	// id number of object
	int				count;	// amount of object owned
	plyrCritStat_t	wear; // percentage worn the object is (100% = broken)


} plyrEquiptmentState_t;

typedef struct
{
	plyrItemID_t id;	// id number of object
	int			 count;	// amount of object owned
} plyrItemState_t;

typedef struct
{ // id of poss
	plyrEquiptmentState_t armorChestLining[PLYR_MAXPAYLOAD];
	plyrEquiptmentState_t armorChestArmor[PLYR_MAXPAYLOAD];
	plyrEquiptmentState_t armorSleeves[PLYR_MAXPAYLOAD];
	plyrEquiptmentState_t armorGauntlets[PLYR_MAXPAYLOAD];
	plyrEquiptmentState_t armorLeggings[PLYR_MAXPAYLOAD];
	plyrEquiptmentState_t armorHelmets[PLYR_MAXPAYLOAD];
	plyrEquiptmentState_t armorSheilds[PLYR_MAXPAYLOAD];

	plyrEquiptmentState_t weapons[PLYR_MAXPAYLOAD];

	plyrItemState_t items[PLYR_ITEMINVENTORYSIZE];
} plyrInventoryState_t;

typedef struct
{
	plyrStat_t vitality;	// player resistance to physical damage
	plyrStat_t spirit;	// player resistance to magic
	plyrStat_t strength;	// player's physical strength
	plyrStat_t speed;		// player's speed at weilding weapons
	plyrStat_t agility;	// player's ability to dodge and evade
	plyrStat_t magic;		// player's ability to cast magic spells
} plyrStats_t;

typedef struct
{
	bool missionGoal[PLYR_NUMBEROFMISSIONGOALS]; // list of acheived goals
	plyrRelationship_t relationships[PLYR_NUMBEROFRELATIONSHIPS]; // list of the states of important relationships
	unsigned int storyPosition; // current position in the story
} plyrStoryState_t;

typedef struct
{
	int equippedArmorChestLining; // index of equipped Item -1 if none
	int equippedArmorChestArmor; // index of equipped Item -1 if none
	int equippedArmorSleeves; // index of equipped Item -1 if none
	int equippedArmorGuantlets; // index of equipped Item -1 if none
	int equippedArmorLeggings; // index of equipped Item -1 if none
	int equippedArmorHelmets; // index of equipped Item -1 if none
	int equippedArmorSheilds; // index of equipped Item -1 if none

	int equippedWeaponRightHand; // index of equipped Item -1 if none
	int equippedWeaponLeftHand; // index of equipped Item -1 if none

	int equippedItem[PLYR_NUMBEROFPOCKETS]; // index of equipped Item -1 if none
} plyrEquippedState_t;

typedef struct
{
	unsigned int positionX; // location on current map
	unsigned int positionY; // location on current map

	unsigned int environmentIndex; // index of the environment player is located in
	unsigned int environmentType; // type of enviorment player is located
} plyrGeographicState_t;

typedef struct
{
	plyrStoryState_t	  story;
	plyrStats_t			  stats;
	plyrInventoryState_t  inventory;
	plyrEquippedState_t	  equiptment;
	plyrGeographicState_t location;	
	plyrCritStat_t		  stamina; // reduces player performance as it drops
} plyrStaticState_t; // contains all the data that the save file will need

typedef struct
{
	plyrStaticState_t		*staticState; // stores player save data
	float					currentHeading; // direction player is facing in degrees
	float					lookAngle; // direction player is look in degrees on X axis
	plyrCritStat_t			health; // player health percentage when at zero player dies
	plyrCritStat_t			magicEnergy; // percent magic energy left
	unsigned int			playerAction; // current action of player
	float					actionState; // current percentage complete of action
	char					*playerSaveFile; // filename player loaded from
	bool					actionKey; // true if action key is depressed
} plyrVolatileState_t; // contain all the player related data the game will need access to

/// COMMUNICATION STRUCTURES
typedef struct
{
	plyrStats_t		stats;
	plyrCritStat_t	health; // player health percentage when at zero player dies
	plyrCritStat_t	magicEnergy; // percent magic energy left
} plyrCOMFullStats_t;
typedef struct
{
	plyrInventoryState_t  inventory;
	plyrEquippedState_t	  equiptment;
} plyrCOMEquipData_t;

/// NON PLAYER DATA STUCTURES TYPES AND DEFINES

// WEAPON TYPE DEFINES
#define WEAPONTYPES_SINGLEHANDEDSWORD	0
#define WEAPONTYPES_DOUBLEHANDEDSWORD	1
#define WEAPONTYPES_HANDANDAHALFSWORD	2
#define WEAPONTYPES_SINGLEHANDEDAXE		3
#define WEAPONTYPES_DOUBLEHANDEDAXE		4
#define WEAPONTYPES_SINGLEHANDEDHAMMER	5
#define WEAPONTYPES_DOUBLEHANDEDHAMMER	6
// ARMOR TYPE DEFINES
#define ARMOR_TYPES_CHESTLINING	0
#define ARMOR_TYPES_CHESTARMOR	1
#define ARMOR_TYPES_SLEEVES		2
#define ARMOR_TYPES_GUANTLETS	3
#define ARMOR_TYPES_LEGGINGS	4
#define ARMOR_TYPES_HELMET		5
#define ARMOR_TYPES_SHEILD		6
// ITEM EFFECT TYPE DEFINES
#define ITEM_EFFECTTYPE_RAISEBYINT			0
#define ITEM_EFFECTTYPE_RAISEBYPERCENT		1
#define ITEM_EFFECTTYPE_LOWERBYINT			2
#define ITEM_EFFECTTYPE_LOWERBYPERCENT		3
#define ITEM_EFFECTTYPE_TEMP_RAISEBYINT		4
#define ITEM_EFFECTTYPE_TEMP_RAISEBYPERCENT	5
#define ITEM_EFFECTTYPE_TEMP_LOWERBYINT		6
#define ITEM_EFFECTTYPE_TEMP_LOWERBYPERCENT	7
// ITEM AFFECTED STAT DEFINES
#define ITEM_AFFECTEDSTAT_VITALITY		0
#define ITEM_AFFECTEDSTAT_SPIRIT		1
#define ITEM_AFFECTEDSTAT_STRENGTH		2
#define ITEM_AFFECTEDSTAT_SPEED			3
#define ITEM_AFFECTEDSTAT_AGILITY		4
#define ITEM_AFFECTEDSTAT_MAGIC			5
#define ITEM_AFFECTEDSTAT_STAMINA		6
#define ITEM_AFFECTEDSTAT_HEALTH		7
#define ITEM_AFFECTEDSTAT_MAGICENERGY	8
// Equiptment Data File Data Types
#define EQUIPTMENTDATAFILE_ID	82
#define EQUIPTMENTDATAFILE_DATATYPE_WEAPONS	0
#define EQUIPTMENTDATAFILE_DATATYPE_ARMOR	1
#define EQUIPTMENTDATAFILE_DATATYPE_ITEMS	2
// Weapons
typedef struct
{
	char			*renderFile; // filename of file with rendiring info
	int				missionGoal; // mission goal index associated with item -1 if none
	char			*name;
	char			*description;
	plyrItemID_t	id; // id number of object
	plyrItemID_t	weaponType; // weapon type id
	int				statMass; // used to calculate swing velocity
	int				statWeight;
	int				statLength; // used to calculate reach of weapon
	int				statDurability; // used to calculate wear on weapon
	int				statHardness; // used to calculate effect of weapon
} plyrWeaponLibraryData_t;

typedef struct
{
	size_t			renderFilenameLength;
	int				missionGoal; // mission goal index associated with item -1 if none
	size_t			nameLength;
	size_t			descriptionLength;
	plyrItemID_t	id; // id number of object
	plyrItemID_t	weaponType; // weapon type id
	int				statMass; // used to calculate swing velocity
	int				statWeight;
	int				statLength; // used to calculate reach of weapon
	int				statDurability; // used to calculate wear on weapon
	int				statHardness; // used to calculate effect of weapon
} weaponDataFileDataHeader_t;
// Armor
typedef struct
{
	char			*renderFile; // filename of file with rendiring info
	int				missionGoal; // mission goal index associated with item -1 if none
	char			*name;
	char			*description;
	plyrItemID_t	id; // id number of object
	plyrItemID_t	armorType; // armor type id
	int				statMass; // used to calculate affect on player motion
	int				statWeight;
	int				statArea; // measure of percent body coverage
	int				statDurability; // used to calculate wear on armor
} plyrArmorLibraryData_t;

typedef struct
{
	size_t			renderFilenameLength;
	int				missionGoal; // mission goal index associated with item -1 if none
	size_t			nameLength;
	size_t			descriptionLength;
	plyrItemID_t	id; // id number of object
	plyrItemID_t	armorType; // armor type id
	int				statMass; // used to calculate affect on player motion
	int				statWeight;
	int				statArea; // measure of percent body coverage
	int				statDurability; // used to calculate wear on armor
} armorDataFileDataHeader_t;
// Items
typedef struct
{
	plyrItemID_t	affectedStat; // id of affected stat
	plyrItemID_t	effectOnStat; // id of type of effect on stat
	int				potency;	  // degree of effect
} itemEffect_t;

typedef struct
{
	char			*renderFile; // filename of file with rendiring info
	int				missionGoal; // mission goal index associated with item -1 if none
	char			*name;
	char			*description;
	plyrItemID_t	id; // id number of object
	int				numberOfEffects; // amount of effects of item
	itemEffect_t	*effectsList; // list of item effects
} plyrItemLibraryData_t;

typedef struct
{
	size_t			renderFilenameLength;
	int				missionGoal; // mission goal index associated with item -1 if none
	size_t			nameLength;
	size_t			descriptionLength;
	plyrItemID_t	id; // id number of object
	int				numberOfEffects; // amount of effects of item
} itemDataFileDataHeader_t;
// Equiptment Data File Header
typedef struct
{
	UINT8	equiptmentDataFileID; // ID code for Equiptment Data Files
	int		objectCount; // number of objects in the file
	int		dataType; // type of data in file
} equiptmentDataFileHeader_t;
#endif