#include "cg_local.h"

#define NUM_PM_STACK_ITEMS	32
#define MAX_VISIBLE_ITEMS	5

#define NUM_PM_STACK_ITEMS_BIG 8 // Gordon: we shouldn't need many of these

typedef struct pmStackItem_s pmListItem_t;
typedef struct pmStackItemBig_s pmListItemBig_t;

struct pmStackItem_s {
	popupMessageType_t		type;
	qboolean				inuse;
	int						time;
	char					message[128];
	char					message2[128];
	int						actionShaderWScale;
	qhandle_t				actionShader;
	qhandle_t				shader;

	vec3_t					color;

	pmListItem_t*			next;
};

struct pmStackItemBig_s {
	popupMessageBigType_t	type;
	qboolean				inuse;
	int						time;
	char					message[128];
	qhandle_t				shader;
	vec3_t					color;

	pmListItemBig_t*		next;
};

pmListItem_t		cg_pmStack[NUM_PM_STACK_ITEMS];
pmListItem_t*		cg_pmOldList;
pmListItem_t*		cg_pmWaitingList;
pmListItemBig_t*	cg_pmWaitingListBig;

pmListItemBig_t		cg_pmStackBig[NUM_PM_STACK_ITEMS_BIG];

/*
const char* cg_skillRewards[SK_NUM_SKILLS][NUM_SKILL_LEVELS-1] = 
{
	{ "Binoculars", "Improved Physical Fitness", "Improved Health", "Trap Awareness" },											// battle sense
	{ "Improved use of Explosive Ammunition", "Improved Dexterity", "Improved Construction and Destruction", "a Flak Jacket" },	// explosives & construction
	{ "Medic Ammo", "Improved Resources", "Full Revive", "Adrenalin Self" },													// first aid
	{ "Improved Resources", "Improved Signals", "Improved Air and Ground Support", "Enemy Recognition" },						// signals
	{ "Improved use of Light Weapon Ammunition", "Faster Reload", "Improved Light Weapon Handling", "Dual-Wield Pistols" },		// light weapons
	{ "Improved Projectile Resources", "Heavy Weapon Proficiency", "Improved Dexterity", "Improved Weapon Handling" },			// heavy weapons
	{ "Improved use of Scoped Weapon Ammunition", "Improved use of Sabotage and Misdirection", "Breath Control", "Assassin" }	// scoped weapons & military intelligence
};
*/

// jet Pilot - new level names
const char* cg_skillRewards[SK_NUM_SKILLS][NUM_SKILL_LEVELS-1] = {
// Battle Sense
	{ "Binoculars", 
	  "Improved Physical Fitness", 
	  "Improved Health", 
	  "Trap Awareness",
	  "Battle Sense Level 5",
	  "Improved Lung Capacity",
	  "Dynamite Prediction",
	  "Danger Awareness",
	  "Battle Sense Level 9",
	},
	
// Engineering
	{ "Additional Explosive Ammunition", 
	  "Faster Arming and Defusing", 
	  "Improved Construction and Demolitions", 
	  "a Flak Jacket",
	  "Advanced Weapon Cooling",						//Nearby teammates' weapons cool faster
	  "Advanced Arming and Defusing",					//Faster speed and less charge in arming and disrarming	dyno, satchel, and landmines	
	  "Advanced Construction and Demolitions",			//Faster speed and less charge in repairing objectives and tossing dyno
	  "Faster Rifle Grenades",							//Rifle nades use less charge bar
      "Engineering Level 9",
	},
// First Aid
	{ "Additional Ammo", 
	  "Improved Resources", 
	  "Full Revive",
	  "Adrenaline Self",
	  "Improved Health Care",							// Nearby healthpacks heal 30hp, not 20
	  "Full Payload",							
	  "Health Pack Delivery",
      "First Aid Level 8",
      "Battlefield Resuscitation",						// Can revive players by kneeling over them and holding use key for 5 seconds
	},	
// Signals and Logistics
	{ "Improved Resources", 							// Ammopacks contain 2x ammo
	  "Improved Signals", 								//
	  "Improved Air and Ground Support", 				//
	  "Enemy Recognition",								//
	  "Inspire Morale",									// All teammates within 512 units recharge stamina at double rate
	  "Ammo Information",
	  "Ammo Pack Delivery",								// Arty and Airstrikes use less charge
      "Advanced Resources",								// Ammopacks contain 3x ammo
      "Signals Level 9",								//
	},
// Light Weapons
	{ "Extra Light Weapon Ammunition",					// start with an extra clip for light weapons
	  "Faster Reload",									// reload takes less time
	  "Improved Light Weapon Handling",					// Spread reduced by 35%
	  "Dual-Wield Pistols",								// can carry "akimbo" pistols in slot 2
	  "Faster Weapon Switching",						// 
	  "Additional Light Weapon Ammunition",				// add another additional clip
	  "Poison Needles",									// poison needles
	  "Knife Throwing",									// throwing knives
      "Pistol Execution",								// Instant kill with any pistol from right behind
	},												
// Heavy Weapons									
	{ "Extra Heavy Weapon Ammunition",					// Carry one additional belt/round 
	  "Reduced Overheating",	 						// Takes longer till mg overheats
	  "Increased Movement Speed",						// Travel less slowly while carrying heavy weapons
	  "SMG Sidearm",									// can use SMG instead of pistol along with heavy weapon
	  "Improved Machine Gun Usage",						// Can reload an MG42/Browning .30cal mid clip/belt
	  "Additional Heavy Weapon Ammo",					// Carry aditional rpg/mortar rounds
      "Improved Strength",								// Heavy weapons use less charge
      "Advanced Weapons Training",						// Can pickup any weapon on the field
	  "Helmet Armor",									// Helmet gets a piece of armor to have 80% protection on first headshot

	},
// Covert Operations
	{ "Extra Scoped Weapon Ammunition", 				// Carry an additional clip 
	  "Improved use of Sabotage and Misdirection",		// 
	  "Improved Accuracy",								// More stability when using a scope
	  "Backstab",										// Instant kill from behind with knife
	  "Improved Disguises",								// Shooting while in disguise uses charge to retain diguise
	  "Stealthy Movement",								// Move at normal speed with no footsteps
      "Recognition Disruption", 
	  "Enemy Communications",							// Can call for medic or ammo in other team's chat and have icon over your head visible to other team
      "Impersonation",									// Disguise name doesn't disappear at close range
	}
};

void CG_PMItemBigSound( pmListItemBig_t* item );

void CG_InitPMGraphics( void ) 
{
	cgs.media.pmImages[PM_DYNAMITE]			= trap_R_RegisterShaderNoMip( /*"gfx/limbo/cm_dynamite"*/"gfx/limbo/dynamite"); // jaquboss, use default shader, we use the one with altered shader for command map
	cgs.media.pmImages[PM_CONSTRUCTION]		= trap_R_RegisterShaderNoMip( "sprites/voiceChat" );
	cgs.media.pmImages[PM_MINES] 			= trap_R_RegisterShaderNoMip( "sprites/voiceChat" );
	cgs.media.pmImages[PM_DEATH] 			= trap_R_RegisterShaderNoMip( "gfx/hud/pm_death" );
	cgs.media.pmImages[PM_MESSAGE]			= trap_R_RegisterShaderNoMip( "sprites/voiceChat" );
	cgs.media.pmImages[PM_OBJECTIVE]		= trap_R_RegisterShaderNoMip( "sprites/objective" );
	cgs.media.pmImages[PM_DESTRUCTION]		= trap_R_RegisterShaderNoMip( "sprites/voiceChat" );
	cgs.media.pmImages[PM_TEAM]				= trap_R_RegisterShaderNoMip( "sprites/voiceChat" );	
	// jet Pilot - new PM types
	cgs.media.pmImages[PM_AMMOPICKUP]		= trap_R_RegisterShaderNoMip( "gfx/2d/ammoboxHint" );	
	cgs.media.pmImages[PM_HEALTHPICKUP]		= trap_R_RegisterShaderNoMip( "gfx/2d/healthHint" );	
	cgs.media.pmImages[PM_GRAPHICOBITUARY]	= trap_R_RegisterShaderNoMip( "gfx/hud/pm_death" );	


	cgs.media.pmImageAlliesConstruct		= trap_R_RegisterShaderNoMip( "gfx/hud/pm_constallied" );
	cgs.media.pmImageAxisConstruct			= trap_R_RegisterShaderNoMip( "gfx/hud/pm_constaxis" );
	cgs.media.pmImageAlliesMine				= trap_R_RegisterShaderNoMip( "gfx/hud/pm_mineallied" );
	cgs.media.pmImageAxisMine				= trap_R_RegisterShaderNoMip( "gfx/hud/pm_mineaxis" );
//	cgs.media.hintKey						= trap_R_RegisterShaderNoMip( "gfx/hud/keyboardkey_old" );
}

void CG_InitPM( void ) {
	memset( &cg_pmStack,	0, sizeof( cg_pmStack ) );
	memset( &cg_pmStackBig, 0, sizeof( cg_pmStackBig ) );

	cg_pmOldList =			NULL;
	cg_pmWaitingList =		NULL;
	cg_pmWaitingListBig =	NULL;
}

#define PM_FADETIME 2500
#define PM_WAITTIME 2000

#define PM_FADETIME_BIG 1000
#define PM_WAITTIME_BIG 3500

int CG_TimeForPopup( popupMessageType_t type ) {
	switch( type ) {
		default:
			return (int)(cg_pmPopupTime.value*1000);
	}
} 

int CG_TimeForBigPopup( popupMessageBigType_t type ) {
	switch( type ) {
		default:
			return (int)(cg_pmBigPopupTime.value*1000);
	}
}

void CG_AddToListFront( pmListItem_t** list, pmListItem_t* item ) {
	item->next = *list;
	*list = item;
}

void CG_UpdatePMLists( void ) {
	pmListItem_t* listItem;
	pmListItem_t* lastItem;
	pmListItemBig_t* listItem2;

	if ((listItem = cg_pmWaitingList)) {
		int t = (CG_TimeForPopup( listItem->type ) + listItem->time);
		if( cg.time > t ) {
			if( listItem->next ) {
				// there's another item waiting to come on, so move to old list
				cg_pmWaitingList = listItem->next;
				cg_pmWaitingList->time = cg.time; // set time we popped up at

				CG_AddToListFront( &cg_pmOldList, listItem );
			} else {
				if( cg.time > t + (cg_pmWaitTime.value*1000) + (cg_pmFadeTime.value*1000) ) {
					// we're gone completely
					cg_pmWaitingList = NULL;
					listItem->inuse = qfalse;
					listItem->next = NULL;
				} else {
					// just sit where we are, no pressure to do anything...
				}
			}
		}
	}

	listItem = cg_pmOldList;
	lastItem = NULL;
	while( listItem ) {
		int t = (CG_TimeForPopup( listItem->type ) + listItem->time + (cg_pmWaitTime.value*1000) + (cg_pmFadeTime.value*1000));
		if( cg.time > t ) {
			// nuke this, and everything below it (though there shouldn't BE anything below us anyway)
			pmListItem_t* next;

			if( !lastItem ) {
				// we're the top of the old list, so set to NULL
				cg_pmOldList = NULL;
			} else {
				lastItem->next = NULL;
			}

			do {
				next = listItem->next;

				listItem->next = NULL;
				listItem->inuse = qfalse;

			} while ((listItem = next));
			

			break;
		}

		lastItem = listItem;
		listItem = listItem->next;
	}


	if ((listItem2 = cg_pmWaitingListBig)) {
		int t = CG_TimeForBigPopup( listItem2->type ) + listItem2->time;
		if( cg.time > t ) {
			if( listItem2->next ) {
				// there's another item waiting to come on, so kill us and shove the next one to the front
				cg_pmWaitingListBig = listItem2->next;
				cg_pmWaitingListBig->time = cg.time; // set time we popped up at

				CG_PMItemBigSound( cg_pmWaitingListBig );

				listItem2->inuse = qfalse;
				listItem2->next = NULL;
			} else {
				if( cg.time > t + (cg_pmWaitTime.value*1000) + (cg_pmFadeTime.value*1000) ) {
					// we're gone completely
					cg_pmWaitingListBig = NULL;
					listItem2->inuse = qfalse;
					listItem2->next = NULL;
				} else {
					// just sit where we are, no pressure to do anything...
				}
			}
		}
	}
}

pmListItemBig_t* CG_FindFreePMItem2( void ) {
	int i = 0;
	for( ; i < NUM_PM_STACK_ITEMS_BIG; i++ ) {
		if( !cg_pmStackBig[i].inuse ) {
			return &cg_pmStackBig[i];
		}
	}

	return NULL;
}

pmListItem_t* CG_FindFreePMItem( void ) {
	pmListItem_t* listItem;
	pmListItem_t* lastItem;

	int i = 0;
	for( ; i < NUM_PM_STACK_ITEMS; i++ ) {
		if( !cg_pmStack[i].inuse ) {
			return &cg_pmStack[i];
		}
	}

	// no totally free items, so just grab the last item in the oldlist
	if ((lastItem = listItem = cg_pmOldList)) {
		while( listItem->next ) {
			lastItem = listItem;
			listItem = listItem->next;			
		}

		if( lastItem == cg_pmOldList ) {
			cg_pmOldList = NULL;
		} else {
			lastItem->next = NULL;
		}

		listItem->inuse = qfalse;

		return listItem;
	} else {
		// there is no old list... PANIC!
		return NULL;
	}
}

void CG_AddPMItem( popupMessageType_t type, const char* message, qhandle_t shader, vec3_t color ) 
{
	pmListItem_t* listItem;
	char* end;

	if( !message || !*message ) {
		return;
	}
	if (type < 0 || type >= PM_NUM_TYPES) {
		CG_Printf("Invalid popup type: %d\n", type);
		return;
	}

	listItem = CG_FindFreePMItem();

	if( !listItem ) {
		return;
	}

	if( shader ) {
		listItem->shader = shader;
	} else {
		listItem->shader = cgs.media.pmImages[type];
	}

	listItem->inuse = qtrue;
	listItem->type = type;
	Q_strncpyz( listItem->message,  message, sizeof( cg_pmStack[0].message ) );
	Q_strncpyz( listItem->message2, "", sizeof(cg_pmStack[0].message2) );
	listItem->actionShader = 0;
// jaquboss - colored obituaries
	listItem->color[0] = listItem->color[1] = listItem->color[2] = 1.f;
	if ( color != NULL )
		VectorCopy (color, listItem->color);

	// rain - moved this: print and THEN chop off the newline, as the
	// console deals with newlines perfectly.  We do chop off the newline
	// at the end, if any, though.
	if (listItem->message[strlen(listItem->message) - 1] == '\n')
		listItem->message[strlen(listItem->message) - 1] = 0;

	trap_Print( va( "%s\n", listItem->message ) );

	// rain - added parens
	while ((end = strchr(listItem->message, '\n'))) {
		*end = '\0';
	}

	// rain - don't eat popups for empty lines
	if (*listItem->message == '\0')
		return;

	if( !cg_pmWaitingList ) {
		cg_pmWaitingList = listItem;
		listItem->time = cg.time;
	} else {
		pmListItem_t* loop = cg_pmWaitingList;
		while( loop->next ) {
			loop = loop->next;
		}

		loop->next = listItem;
	}
}

// jet Pilot - new PM item, with 2 messages separated by an icon
void CG_AddPMItem2( popupMessageType_t type, const char* msg1, qhandle_t actionShader, const char* msg2,  int actionShaderWScale, vec3_t color)
{
	pmListItem_t	*listItem;
	char			*end;

	if( !msg1 || !*msg1) 
	{
	//	CG_Printf("DEBUG: no message \n" );
		return;
	}

	if (!msg2 || !*msg2)
	{
	//	CG_Printf("DEBUG: no message2 \n" );
		return;
	}

	if (type < 0 || type >= PM_NUM_TYPES) 
	{
		CG_Printf("Invalid popup type: %d\n", type);
		return;
	}

	listItem = CG_FindFreePMItem();

	if( !listItem ) {
	//	CG_Printf("DEBUG: no free PMITEM \n" );
		return;
	}

	listItem->shader = -1;

	if ( !actionShader ){
	//	CG_Printf("DEBUG: no actionShader \n" );
		return;
	}

	if ( !actionShaderWScale ){
	//	CG_Printf("DEBUG: no wScale \n" );
		return;
	}

	listItem->actionShaderWScale = actionShaderWScale;
	listItem->actionShader = actionShader;


	listItem->color[0] = listItem->color[1] = listItem->color[2] = 1.f;
	if ( color != NULL )
		VectorCopy (color, listItem->color);

	listItem->inuse = qtrue;
	listItem->type  = type;
	Q_strncpyz( listItem->message, msg1, sizeof( cg_pmStack[0].message ) );
	Q_strncpyz( listItem->message2, msg2, sizeof( cg_pmStack[0].message2 ) );

	// rain - moved this: print and THEN chop off the newline, as the
	// console deals with newlines perfectly.  We do chop off the newline
	// at the end, if any, though.
	if (listItem->message[strlen(listItem->message) - 1] == '\n')
		listItem->message[strlen(listItem->message) - 1] = 0;

	if (listItem->message[strlen(listItem->message2) - 1] == '\n')
		listItem->message[strlen(listItem->message2) - 1] = 0;

	// rain - added parens
	while ((end = strchr(listItem->message, '\n'))) {
		*end = '\0';
	}
	while ((end = strchr(listItem->message2, '\n'))) {
		*end = '\0';
	}

	// rain - don't eat popups for empty lines
	if (*listItem->message == '\0'){
	//	CG_Printf("DEBUG: empty message \n" );
		return;
	}
	if (*listItem->message2 == '\0'){
	//	CG_Printf("DEBUG: empty message2 \n" );
		return;
	}

	if( !cg_pmWaitingList ) {
		cg_pmWaitingList = listItem;
		listItem->time = cg.time;
	} else {
		pmListItem_t* loop = cg_pmWaitingList;
		while( loop->next ) {
			loop = loop->next;
		}

		loop->next = listItem;
	}
}

void CG_PMItemBigSound( pmListItemBig_t* item ) {
	if( !cg.snap ) {
		return;
	}

	switch( item->type ) {
		case PM_RANK:
			trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.media.sndRankUp );
			break;
		case PM_SKILL:
			trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.media.sndSkillUp );
			break;
		default:
			break;
	}
}

void CG_AddPMItemBig( popupMessageBigType_t type, const char* message, qhandle_t shader, vec3_t color ) {
	pmListItemBig_t* listItem = CG_FindFreePMItem2();
	if( !listItem ) {
		return;
	}

	if( shader ) {
		listItem->shader = shader;
	} else {
		listItem->shader = cgs.media.pmImages[type];
	}

	listItem->inuse = qtrue;
	listItem->type = type;
	listItem->next = NULL;
	Q_strncpyz( listItem->message, message, sizeof( cg_pmStackBig[0].message ) );

// jaquboss - colored obituaries
	listItem->color[0] = listItem->color[1] = listItem->color[2] = 1.f;

	if ( color != NULL )
		VectorCopy (color, listItem->color);

	if( !cg_pmWaitingListBig ) {
		cg_pmWaitingListBig = listItem;
		listItem->time = cg.time;

		CG_PMItemBigSound( listItem );
	} else {
		pmListItemBig_t* loop = cg_pmWaitingListBig;
		while( loop->next ) {
			loop = loop->next;
		}

		loop->next = listItem;
	}
}

#define PM_ICON_SIZE_NORMAL 20
#define PM_ICON_SIZE_SMALL 12

void CG_DrawPMItems( void )
{
	vec4_t	colour = { 0.f, 0.f, 0.f, 1.f };
	vec4_t	colourText = { 1.f, 1.f, 1.f, 1.f };
	float	t;
	int		i, size, altSize;
	pmListItem_t* listItem = cg_pmOldList;
	float	y = 360;
	float	w = 0;

	if( cg_drawSmallPopupIcons.integer ) 
	{
		size = altSize = PM_ICON_SIZE_SMALL;

		y += 4;
	} 
	else 
	{
		size = altSize = PM_ICON_SIZE_NORMAL;
	}

	if( cg.snap->ps.persistant[PERS_RESPAWNS_LEFT] >= 0 ) {
		y -= 20;
	}


	if ( hud_drawAltHUD.integer > 0 )
		y -= 20; 


	if( !cg_pmWaitingList ) {
		return;
	}

	t = cg_pmWaitingList->time + CG_TimeForPopup( cg_pmWaitingList->type ) + (cg_pmFadeTime.value*1000);
	if( cg.time > t ) {
		colourText[3] = colour[3] = 1 - ((cg.time - t) / (cg_pmFadeTime.value*1000));
	}

// colorize
	if ( cg_pmWaitingList->shader > 0){
		for (i=0; i<3; i++ )  {
			colourText[i] = cg_pmWaitingList->color[i];
		}
		trap_R_SetColor( colourText );
	// draw
		CG_DrawPic( 4, y, size, size, cg_pmWaitingList->shader );
	// decolorize
		for (i=0; i<3; i++ )  {
			colourText[i] = 1.f;
		}
		trap_R_SetColor( NULL );
	} else {
		size = 0;
	}

	CG_Text_Paint_Ext( 4 + size + 2, y + 12, 0.2f, 0.2f, colourText, cg_pmWaitingList->message, 0, 0, 0, &cgs.media.limboFont2 );

	// jet Pilot - expanded PopupMessages
	if (cg_pmWaitingList->message2[0])
	{
		w = CG_Text_Width_Ext( cg_pmWaitingList->message, 0.2f, 0, &cgs.media.limboFont2 );
	
		for (i=0; i<3; i++ )  {
			colourText[i] = cg_pmWaitingList->color[i];
		}
		trap_R_SetColor( colourText );
		
		CG_DrawPic( 4 + w + size + 8, y, altSize*cg_pmWaitingList->actionShaderWScale, altSize, cg_pmWaitingList->actionShader );

		for (i=0; i<3; i++ )  {
			colourText[i] = 1.f;
		}
		trap_R_SetColor( NULL );
		
		CG_Text_Paint_Ext( 4 + size + (altSize*cg_pmWaitingList->actionShaderWScale) + 16 + w, y + 12, 0.2f, 0.2f, colourText, cg_pmWaitingList->message2, 0, 0, 0, &cgs.media.limboFont2 );
	}

	for( i = 0; i < 4 && listItem; i++, listItem = listItem->next ) {
		int			j;

		if( cg_drawSmallPopupIcons.integer ) {
			size = altSize = PM_ICON_SIZE_SMALL;
		} else {
			size = altSize = PM_ICON_SIZE_NORMAL;
		}

		y -= size + 2;

		t = listItem->time + CG_TimeForPopup( listItem->type ) + (cg_pmWaitTime.value*1000);
		if( cg.time > t ) {
			colourText[3] = colour[3] = 1 - ((cg.time - t) / (cg_pmFadeTime.value*1000));
		} else {
			colourText[3] = colour[3] = 1.f;
		}

	// colorize
		if ( listItem->shader > 0){

			for (j=0; j<3; j++ )  {
				colourText[j] = listItem->color[j];
			}
			trap_R_SetColor( colourText );
		// draw
			CG_DrawPic( 4, y, size, size, listItem->shader );
		// decolorize
			for (j=0; j<3; j++ )  {
				colourText[j] = 1.f;
			}
			trap_R_SetColor( NULL );
		} else {
			size = 0;
		}

		CG_Text_Paint_Ext( 4 + size + 2, y + 12, 0.2f, 0.2f, colourText, listItem->message, 0, 0, 0, &cgs.media.limboFont2 );
		// jet Pilot - expanded PopupMessages
		if (listItem->message2[0])
		{
			w = CG_Text_Width_Ext( listItem->message, 0.19f, 0, &cgs.media.limboFont2 );
		
			for (j=0; j<3; j++ )  {
				colourText[j] = listItem->color[j];
			}
			trap_R_SetColor( colourText );

			CG_DrawPic( 4 + size + w + 8, y, altSize*listItem->actionShaderWScale, altSize, listItem->actionShader );
	
			for (j=0; j<3; j++ )  {
				colourText[j] = 1.f;
			}
			trap_R_SetColor( NULL );
			
			CG_Text_Paint_Ext( 4 + size + (altSize*listItem->actionShaderWScale) + 16 + w, y + 12, 0.2f, 0.2f, colourText, listItem->message2, 0, 0, 0, &cgs.media.limboFont2 );
	

		}
	}
}

void CG_DrawPMItemsBig( void ) {
	vec4_t colour = { 0.f, 0.f, 0.f, 1.f };
	vec4_t colourText = { 1.f, 1.f, 1.f, 1.f };
	float t;
	float y = 270;
	float w;
	int	  i;

	if( !cg_pmWaitingListBig ) {
		return;
	}

	t = cg_pmWaitingListBig->time + CG_TimeForBigPopup( cg_pmWaitingListBig->type ) + (cg_pmWaitTime.value*1000);
	if( cg.time > t ) {
		colourText[3] = colour[3] = 1 - ((cg.time - t) / (cg_pmFadeTime.value*1000));
	}

// colorize
	for (i=0; i<3; i++ )  {
		colourText[i] = cg_pmWaitingListBig->color[i];
	}
	trap_R_SetColor( colourText );
// draw
	CG_DrawPic( 640 - 56, y, 48, 48, cg_pmWaitingListBig->shader );
// decolorize
	for (i=0; i<3; i++ )  {
		colourText[i] = 1.f;
	}
	trap_R_SetColor( NULL );


	w = CG_Text_Width_Ext( cg_pmWaitingListBig->message, 0.22f, 0, &cgs.media.limboFont2 );
	CG_Text_Paint_Ext( 640 - 4 - w, y + 56, 0.22f, 0.24f, colourText, cg_pmWaitingListBig->message, 0, 0, 0, &cgs.media.limboFont2 );
}

const char* CG_GetPMItemText( centity_t* cent ) {
	switch( cent->currentState.effect1Time ) {
		case PM_DYNAMITE:
			switch( cent->currentState.effect2Time ) {
				case 0:
					return va( "Planted at %s.", CG_ConfigString( CS_OID_TRIGGERS + cent->currentState.effect3Time ) );
				case 1:
					return va( "Defused at %s.", CG_ConfigString( CS_OID_TRIGGERS + cent->currentState.effect3Time ) );
			}
			break;
		case PM_CONSTRUCTION:
			switch( cent->currentState.effect2Time ) {
				case -1:
					return CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time );
				case 0:
					return va( "%s has been constructed.", CG_ConfigString( CS_OID_TRIGGERS + cent->currentState.effect3Time ) );
			}
			break;
		case PM_DESTRUCTION:
			switch( cent->currentState.effect2Time ) {
				case 0:
					return va( "%s has been damaged.", CG_ConfigString( CS_OID_TRIGGERS + cent->currentState.effect3Time ) );
				case 1:
					return va( "%s has been destroyed.", CG_ConfigString( CS_OID_TRIGGERS + cent->currentState.effect3Time ) );
			}
			break;
		case PM_MINES:
			// CHRUKER: b009 - Prevent spectators from being informed when a mine is spotted 
			if( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR ) { 
				return NULL; 
			}
			if( cgs.clientinfo[cg.clientNum].team == cent->currentState.effect2Time ) {
				return NULL;
			}

			{
				vec2_t	loc;
				char *locStr = NULL;

				loc[0] =  cent->currentState.origin[0];
				loc[1] =  cent->currentState.origin[1];

				if (cg_locations.integer & LOC_LANDMINES){
					vec3_t	origin;
					qboolean locValid = qtrue;

					origin[0] =  cent->currentState.origin[0];
					origin[1] =  cent->currentState.origin[1];
					origin[2] =  cent->currentState.origin[2];

					locStr = CG_GetLocationMsg(origin);
					if ( !(cg_locations.integer & LOC_KEEPUNKNOWN) ){
						if (!Q_stricmp( locStr, "Unknown")){
							locStr = BG_GetLocationString( loc );
							locValid = qfalse; // don't draw it twice..
						}
					}
					if ( cg_locations.integer & LOC_SHOWCOORDS_LANDMINES && locValid )
					{
						Q_strcat( locStr, 64, va(" ^3(%s)", BG_GetLocationString( loc )) ); // append a location
					}
				} else {
					locStr = BG_GetLocationString( loc );
				}

				if( !locStr || !*locStr )
				locStr = "";
				
				return va( "Spotted by %s^7 at %s", cgs.clientinfo[cent->currentState.effect3Time].name, BG_GetLocationString( cent->currentState.origin ) );
			}
		case PM_OBJECTIVE:
			switch( cent->currentState.density ) {
				case 0:
					return va( "%s have stolen %s!", cent->currentState.effect2Time == TEAM_ALLIES ? "Allies" : "Axis", CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time ));
				case 1:
					return va( "%s have returned %s!", cent->currentState.effect2Time == TEAM_ALLIES ? "Allies" : "Axis", CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time ));
			}
			break;
		case PM_TEAM:
			switch( cent->currentState.density ) {
				case 0: // joined
					{
						const char* teamstr = NULL;
						switch( cent->currentState.effect2Time ) {
							case TEAM_AXIS:
								teamstr = "Axis team";
								break;
							case TEAM_ALLIES:
								teamstr = "Allied team";
								break;
							default:
								teamstr = "Spectators";
								break;
						}

						return va( "%s^7 has joined the %s^7!", cgs.clientinfo[cent->currentState.effect3Time].name, teamstr );
					}
				case 1:
					return va( "%s^7 disconnected", cgs.clientinfo[cent->currentState.effect3Time].name );
			}
	}

	return NULL;
}

void CG_PlayPMItemSound( centity_t *cent )
{
	switch( cent->currentState.effect1Time ) {
		case PM_DYNAMITE:
			switch( cent->currentState.effect2Time ) {
				case 0: // planted
					if( cent->currentState.teamNum == TEAM_AXIS ) // axis dynamite
					{
						if ( cg_altHQVoice.integer )
							CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team != TEAM_ALLIES ? "axis_hq_dynamite_planted" : "allies_hq_dynamite_planted_axis", NULL, -1, qtrue );
						else
							CG_SoundPlaySoundScript( "axis_hq_dynamite_planted", NULL, -1, qtrue );
					}
					else // allied dynamite
					{
						if ( cg_altHQVoice.integer )
							CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team != TEAM_AXIS ? "allies_hq_dynamite_planted" : "axis_hq_dynamite_planted_allies", NULL, -1, qtrue );
						else
							CG_SoundPlaySoundScript( "allies_hq_dynamite_planted", NULL, -1, qtrue );
					}
					break;
				case 1: // defused
					if( cent->currentState.teamNum == TEAM_AXIS ) // axis dynamite
					{	
						if ( cg_altHQVoice.integer )
							CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team != TEAM_ALLIES ? "axis_hq_dynamite_defused" : "allies_hq_dynamite_defused_axis", NULL, -1, qtrue );
						else
							CG_SoundPlaySoundScript( "axis_hq_dynamite_defused", NULL, -1, qtrue );
					}
					else // allied dynamite
					{
						if ( cg_altHQVoice.integer )
							CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team != TEAM_AXIS ? "allies_hq_dynamite_defused" : "axis_hq_dynamite_defused_allies", NULL, -1, qtrue );
						else
							CG_SoundPlaySoundScript( "allies_hq_dynamite_defused", NULL, -1, qtrue );
					}
					break;
			}
			break;
		case PM_MINES:
			// CHRUKER: b009 - Prevent spectators from being informed when a mine is spotted 
			if( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR ) { 
				break;
			}
			if( cgs.clientinfo[cg.clientNum].team != cent->currentState.effect2Time ) {
				// inverted teams
				if( cent->currentState.effect2Time == TEAM_AXIS ) {
					CG_SoundPlaySoundScript( "allies_hq_mines_spotted", NULL, -1, qtrue );
				} else {
					CG_SoundPlaySoundScript( "axis_hq_mines_spotted", NULL, -1, qtrue );
				}
			}
			break;
		case PM_OBJECTIVE:
			switch( cent->currentState.density ) {
				case 0:
					if( cent->currentState.effect2Time == TEAM_AXIS )
						CG_SoundPlaySoundScript( "axis_hq_objective_taken", NULL, -1, qtrue );
					else
						CG_SoundPlaySoundScript( "allies_hq_objective_taken", NULL, -1, qtrue );
					break;
				case 1:
					if( cent->currentState.effect2Time == TEAM_AXIS )
						CG_SoundPlaySoundScript( "axis_hq_objective_secure", NULL, -1, qtrue );
					else
						CG_SoundPlaySoundScript( "allies_hq_objective_secure", NULL, -1, qtrue );
					break;
			}
			break;
		default:
			break;
	}
}

qhandle_t CG_GetPMItemIcon( centity_t* cent ) {
	switch( cent->currentState.effect1Time ) {
		case PM_CONSTRUCTION:
			if( cent->currentState.density == TEAM_AXIS ) {
				return cgs.media.pmImageAxisConstruct;
			}
			return cgs.media.pmImageAlliesConstruct;
		case PM_MINES:
			if( cent->currentState.effect2Time == TEAM_AXIS ) {
				return cgs.media.pmImageAlliesMine;
			}
			return cgs.media.pmImageAxisMine;
		default:
			return cgs.media.pmImages[cent->currentState.effect1Time];
	}

	return 0;
}



void CG_DrawKeyHint( rectDef_t* rect, const char* binding ) {
/*	int k1, k2;
	char buffer[256];
	char k[2] = { 0, 0 };
	float w;

	trap_Key_KeysForBinding( binding, &k1, &k2 );

	if( k1 != -1 ) {
		trap_Key_KeynumToStringBuf( k1, buffer, 256 );
		if( strlen( buffer ) != 1 ) {
			if( k2 != -1 ) {
				trap_Key_KeynumToStringBuf( k2, buffer, 256 );
				if( strlen( buffer ) == 1 ) {
					*k = toupper( *buffer );
				}
			}
		} else {
			*k = toupper( *buffer );
		}
	}

	if( !*k ) {
		return;
	}

	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hintKey );

	w = CG_Text_Width_Ext( k, 0.2f, 0, &cgs.media.limboFont1 );
	CG_Text_Paint_Ext( rect->x + ((rect->w - w) * 0.5f), rect->y + 14, 0.2f, 0.2f, colorWhite, k, 0, 0, 0, &cgs.media.limboFont1 );*/
}
