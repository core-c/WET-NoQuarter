/*
 * core: remake of the cg_popupmessages.c file
 *       This one is for mission related messages only..
 *       It displays messages on top-of-screen.
 */

#include "cg_local.h"

#define NUM_MM_STACK_ITEMS	32
#define MAX_VISIBLE_ITEMS	3

typedef struct mmStackItem_s mmListItem_t;

struct mmStackItem_s {
	popupMessageType_t		type;
	qboolean				inuse;
	int						time;
	char					message[128];
	char					message2[128];
	int						actionShaderWScale;
	qhandle_t				actionShader;
	qhandle_t				shader;

	vec3_t					color;

	mmListItem_t*			next;
};

mmListItem_t		cg_mmStack[NUM_MM_STACK_ITEMS];
mmListItem_t*		cg_mmOldList;
mmListItem_t*		cg_mmWaitingList;

/*
===============
CG_InitMM
===============
*/
void CG_InitMM( void ) {
	memset( &cg_mmStack,	0, sizeof( cg_mmStack ) );

	cg_mmOldList =			NULL;
	cg_mmWaitingList =		NULL;
}

#define MM_FADETIME 2500
#define MM_WAITTIME 2000

/*
===============
CG_TimeForPopupMM
===============
*/
int CG_TimeForPopupMM( popupMessageType_t type ) {
	switch( type ) {
		default:
			return (int)(cg_pmPopupTime.value*1000);
	}
}

/*
===============
CG_AddToListFrontMM
===============
*/
void CG_AddToListFrontMM( mmListItem_t** list, mmListItem_t* item ) {
	item->next = *list;
	*list = item;
}

/*
===============
CG_UpdateMMLists
===============
*/
void CG_UpdateMMLists( void ) {
	mmListItem_t* listItem;
	mmListItem_t* lastItem;

	if ((listItem = cg_mmWaitingList)) {
		int t = (CG_TimeForPopupMM( listItem->type ) + listItem->time);
		if( cg.time > t ) {
			if( listItem->next ) {
				// there's another item waiting to come on, so move to old list
				cg_mmWaitingList = listItem->next;
				cg_mmWaitingList->time = cg.time; // set time we popped up at

				CG_AddToListFrontMM( &cg_mmOldList, listItem );
			} else {
				if( cg.time > t + (cg_pmWaitTime.value*1000) + (cg_pmFadeTime.value*1000) ) {
					// we're gone completely
					cg_mmWaitingList = NULL;
					listItem->inuse = qfalse;
					listItem->next = NULL;
				} else {
					// just sit where we are, no pressure to do anything...
				}
			}
		}
	}

	listItem = cg_mmOldList;
	lastItem = NULL;
	while( listItem ) {
		int t = (CG_TimeForPopupMM( listItem->type ) + listItem->time + (cg_pmWaitTime.value*1000) + (cg_pmFadeTime.value*1000));
		if( cg.time > t ) {
			// nuke this, and everything below it (though there shouldn't BE anything below us anyway)
			mmListItem_t* next;

			if( !lastItem ) {
				// we're the top of the old list, so set to NULL
				cg_mmOldList = NULL;
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
}

/*
===============
CG_FindFreeMMItem
===============
*/
mmListItem_t* CG_FindFreeMMItem( void ) {
	mmListItem_t* listItem;
	mmListItem_t* lastItem;

	int i = 0;
	for( ; i < NUM_MM_STACK_ITEMS; i++ ) {
		if( !cg_mmStack[i].inuse ) {
			return &cg_mmStack[i];
		}
	}

	// no totally free items, so just grab the last item in the oldlist
	if ((lastItem = listItem = cg_mmOldList)) {
		while( listItem->next ) {
			lastItem = listItem;
			listItem = listItem->next;
		}

		if( lastItem == cg_mmOldList ) {
			cg_mmOldList = NULL;
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

/*
===============
CG_FindFreeMMItem

 redeye - checks the type of the message to be printed against cg_popupMessageFilter
===============
*/
static qboolean CG_CheckMsgFilter(popupMessageType_t type)
{
	switch (type) {
		case PM_DYNAMITE:
		case PM_CONSTRUCTION:
		case PM_MINES:
		case PM_OBJECTIVE:
		case PM_DESTRUCTION:

			if (cg_popupMessageFilter.integer & POPUP_FILTER_OBJECTIVE) {
				return qtrue;
			}
			break;

		case PM_TEAM:

			if (cg_popupMessageFilter.integer & POPUP_FILTER_TEAMJOIN) {
				return qtrue;
			}
			break;

		case PM_AMMOPICKUP:
		case PM_HEALTHPICKUP:

			if (cg_popupMessageFilter.integer & POPUP_FILTER_PICKUPS) {
				return qtrue;
			}
			break;

//		case PM_DEATH: // obituaries/death messages are filtered in CG_Obituary()

		default:
			return qfalse; // don't filter by default
	}
	return qfalse;
}

/*
===============
CG_AddMMItem
===============
*/
void CG_AddMMItem( popupMessageType_t type, const char* message, qhandle_t shader, vec3_t color, int weapon )
{
	mmListItem_t*	listItem;
	char*			end;
	qboolean		hasShader;

	if( !message || !*message ) {
		return;
	}
	if (type < 0 || type >= PM_NUM_TYPES) {
		CG_Printf("Invalid mission-popup type: %d\n", type);
		return;
	}

	// redeye - filter popup messages
	if (CG_CheckMsgFilter(type)) {
		trap_Print(va("%s\n", message)); // when filtered, log it to the console
		return;
	}

	listItem = CG_FindFreeMMItem();

	if( !listItem ) {
		return;
	}

	// destruction types have a weapon defined
	hasShader = qfalse;
	if ( type == PM_DESTRUCTION && weapon > WP_NONE && weapon < MAX_WEAPONS ) {
		if ( cg_weapons[ weapon ].weaponIcon[1] ) {
			listItem->shader = cg_weapons[ weapon ].weaponIcon[1];
			listItem->actionShaderWScale = weaponIconScale(weapon);
			hasShader = qtrue;
		}
	}
	if ( hasShader == qfalse ) {
		if( shader ) {
			listItem->shader = shader;
		}
		else {
			listItem->shader = cgs.media.pmImages[type];
		}
		listItem->actionShaderWScale = 1;
	}

	listItem->inuse = qtrue;
	listItem->type = type;
	Q_strncpyz( listItem->message,  message, sizeof( cg_mmStack[0].message ) );
	Q_strncpyz( listItem->message2, "", sizeof(cg_mmStack[0].message2) );
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

	// core: do not write obituary popups into console..
	// we'll get double kill-messages otherwise.
	if ( type != PM_DEATH ) {
		trap_Print( va( "%s\n", listItem->message ) );
	}

	// rain - added parens
	while ((end = strchr(listItem->message, '\n'))) {
		*end = '\0';
	}

	// rain - don't eat popups for empty lines
	if (*listItem->message == '\0')
		return;

	if( !cg_mmWaitingList ) {
		cg_mmWaitingList = listItem;
		listItem->time = cg.time;
	}
	else {
		mmListItem_t* loop = cg_mmWaitingList;
		while( loop->next ) {
			loop = loop->next;
		}

		loop->next = listItem;
	}
}

#define MM_ICON_SIZE_NORMAL 20
#define MM_ICON_SIZE_SMALL 12

/*
===============
CG_MMPosY
===============
*/
float CG_MMPosY( void ) {
	float y = 10;
	// core: check if a fireteam panel is displayed..
	//       The mission-messages come below that panel.
	y += CG_FireteamOverlayHeight();
	if ( y>10 ) y += 10; // Y-spacing..
	return y;
}

/*
===============
CG_MMHeight
===============
*/
float CG_MMHeight( void ) {
	int size = ( cg_drawSmallPopupIcons.integer )? MM_ICON_SIZE_SMALL : MM_ICON_SIZE_NORMAL;
	return ( MAX_VISIBLE_ITEMS * (size + 2) );
}

/*
===============
CG_DrawMMItems
===============
*/
void CG_DrawMMItems( void )
{
	vec4_t			colour = { 0.f, 0.f, 0.f, 1.f };
	vec4_t			colourText = { 1.f, 1.f, 1.f, 1.f };
	float			t;
	int				i, size, altSize, j;
	mmListItem_t*	listItem = cg_mmOldList;
	float			x = 10;
	float			y = CG_MMPosY();
	float			w = 0;
	float			scale;
	float			y_offset = 12.0f;

	scale = ( cg_smallFont.integer & SMALLFONT_MISSIONPOPUPS )? 0.16f : 0.2f;

	if( cg_drawSmallPopupIcons.integer ) {
		size = altSize = MM_ICON_SIZE_SMALL;
		y_offset = 8.0f;
	}
	else {
		size = altSize = MM_ICON_SIZE_NORMAL;
	}

	if( !cg_mmWaitingList ) {
		return;
	}

	t = cg_mmWaitingList->time + CG_TimeForPopupMM( cg_mmWaitingList->type ) + (cg_pmFadeTime.value*1000);
	if( cg.time > t ) {
		colourText[3] = colour[3] = 1 - ((cg.time - t) / (cg_pmFadeTime.value*1000));
	}

	if ( cg_mmWaitingList->shader > 0){
		// colorize
		for (i=0; i<3; i++ )  {
			colourText[i] = cg_mmWaitingList->color[i];
		}
		trap_R_SetColor( colourText );
		// draw
		CG_DrawPic( x, y, size*cg_mmWaitingList->actionShaderWScale, size, cg_mmWaitingList->shader );
		// decolorize
		for (i=0; i<3; i++ )  {
			colourText[i] = 1.f;
		}
		trap_R_SetColor( NULL );
	}
	else {
		size = 0;
	}

	CG_Text_Paint_Ext( x + size*2 + 2, y + y_offset, scale, scale, colourText, cg_mmWaitingList->message, 0, 0, 0, &cgs.media.limboFont2 );

	// jet Pilot - expanded PopupMessages
	if (cg_mmWaitingList->message2[0]) {
		w = CG_Text_Width_Ext( cg_mmWaitingList->message, scale, 0, &cgs.media.limboFont2 );

		for (i=0; i<3; i++ )  {
			colourText[i] = cg_mmWaitingList->color[i];
		}
		trap_R_SetColor( colourText );

		CG_DrawPic( x + w + size*2 + 8, y, altSize*cg_mmWaitingList->actionShaderWScale, altSize, cg_mmWaitingList->actionShader );

		for (i=0; i<3; i++ )  {
			colourText[i] = 1.f;
		}
		trap_R_SetColor( NULL );

		CG_Text_Paint_Ext( x + size*2 + (altSize*cg_mmWaitingList->actionShaderWScale) + 16 + w, y + y_offset, scale, scale, colourText, cg_mmWaitingList->message2, 0, 0, 0, &cgs.media.limboFont2 );
	}

	for( i = 0; i < MAX_VISIBLE_ITEMS-1 && listItem; i++, listItem = listItem->next ) {
		size = altSize = ( cg_drawSmallPopupIcons.integer )? MM_ICON_SIZE_SMALL : MM_ICON_SIZE_NORMAL;

		y += size + 2;

		t = listItem->time + CG_TimeForPopupMM( listItem->type ) + (cg_pmWaitTime.value*1000);
		if( cg.time > t ) {
			colourText[3] = colour[3] = 1 - ((cg.time - t) / (cg_pmFadeTime.value*1000));
		}
		else {
			colourText[3] = colour[3] = 1.f;
		}

		if ( listItem->shader > 0){
			// colorize
			for (j=0; j<3; j++ )  {
				colourText[j] = listItem->color[j];
			}
			trap_R_SetColor( colourText );
			// draw
			CG_DrawPic( x, y, size*listItem->actionShaderWScale, size, listItem->shader );
			// decolorize
			for (j=0; j<3; j++ )  {
				colourText[j] = 1.f;
			}
			trap_R_SetColor( NULL );
		}
		else {
			size = 0;
		}

		CG_Text_Paint_Ext( x + size*2 + 2, y + y_offset, scale, scale, colourText, listItem->message, 0, 0, 0, &cgs.media.limboFont2 );

		// jet Pilot - expanded PopupMessages
		if (listItem->message2[0]) {
			w = CG_Text_Width_Ext( listItem->message, scale, 0, &cgs.media.limboFont2 );

			for (j=0; j<3; j++ )  {
				colourText[j] = listItem->color[j];
			}
			trap_R_SetColor( colourText );

			CG_DrawPic( x + size*2 + w + 8, y, altSize*listItem->actionShaderWScale, altSize, listItem->actionShader );

			for (j=0; j<3; j++ )  {
				colourText[j] = 1.f;
			}
			trap_R_SetColor( NULL );

			CG_Text_Paint_Ext( 4 + size*2 + (altSize*listItem->actionShaderWScale) + 16 + w, y + y_offset, scale, scale, colourText, listItem->message2, 0, 0, 0, &cgs.media.limboFont2 );
		}

	}
}

#define TXTCOLOR_OBJ "^O"

/*
===============
CG_GetMMItemText
===============
*/
const char* CG_GetMMItemText( centity_t* cent ) {
	switch( cent->currentState.effect1Time ) {
		case PM_DYNAMITE:
			switch( cent->currentState.effect2Time ) {
				case 0:
					return va( "%sPlanted at %s by ^7%s%s.", TXTCOLOR_OBJ, CG_ConfigString( CS_OID_TRIGGERS + cent->currentState.effect3Time ), cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ );
				case 1:
					return va( "%sDefused at %s by ^7%s%s.", TXTCOLOR_OBJ, CG_ConfigString( CS_OID_TRIGGERS + cent->currentState.effect3Time ), cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ );
			}
			break;
		case PM_CONSTRUCTION:
			switch( cent->currentState.effect2Time ) {
				case -1:
					return CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time );
				case 0:
					return va( "%s%s has been constructed by ^7%s%s.", TXTCOLOR_OBJ, Q_strCapitalize((char *)CG_ConfigString(CS_OID_TRIGGERS + cent->currentState.effect3Time)), cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ );
				case 1:
					return va( "%s%s has been repaired by ^7%s%s.", TXTCOLOR_OBJ, Q_strCapitalize((char *)CG_ConfigString(CS_OID_TRIGGERS + cent->currentState.effect3Time)), cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ );
			}
			break;
		case PM_DESTRUCTION:
			if ( cgs.clientinfo[cent->currentState.clientNum].infoValid == qtrue ) {
				switch( cent->currentState.effect2Time ) {
					case 0:
						return va( "%s%s has been damaged by ^7%s%s.", TXTCOLOR_OBJ, Q_strCapitalize((char *)CG_ConfigString(CS_OID_TRIGGERS + cent->currentState.effect3Time)), cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ );
					case 1:
						return va( "%s%s has been destroyed by ^7%s%s.", TXTCOLOR_OBJ, Q_strCapitalize((char *)CG_ConfigString(CS_OID_TRIGGERS + cent->currentState.effect3Time)), cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ );
				}
			}
			else {
				switch( cent->currentState.effect2Time ) {
					case 0:
						return va( "%s%s has been damaged.", TXTCOLOR_OBJ, Q_strCapitalize((char *)CG_ConfigString(CS_OID_TRIGGERS + cent->currentState.effect3Time)) );
					case 1:
						return va( "%s%s has been destroyed.", TXTCOLOR_OBJ, Q_strCapitalize((char *)CG_ConfigString(CS_OID_TRIGGERS + cent->currentState.effect3Time)) );
				}
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

                    locStr = CG_GetLocationMsg(-1, origin);
					if ( !(cg_locations.integer & LOC_KEEPUNKNOWN) ){
						if (!Q_stricmp( locStr, "Unknown")){
							locStr = BG_GetLocationString( loc );
							locValid = qfalse; // don't draw it twice..
						}
					}
					if ( cg_locations.integer & LOC_SHOWCOORDS_LANDMINES && locValid ) {
						Q_strcat( locStr, 64, va(" ^3(%s)", BG_GetLocationString( loc )) ); // append a location
					}
				}
				else {
					locStr = BG_GetLocationString( loc );
				}

				if( !locStr || !*locStr )
				locStr = "";

				return va( "%sSpotted by ^7%s%s at %s", TXTCOLOR_OBJ, cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ, BG_GetLocationString( cent->currentState.origin ) );
			}
			break;
		case PM_OBJECTIVE:
			switch( cent->currentState.density ) {
				case 0: // team_WOLF_redflag/team_WOLF_blueflag
					return va( "^7%s%s has stolen %s%s!", cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ, CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time ), TXTCOLOR_OBJ );
				case 1: // team_WOLF_redflag/team_WOLF_blueflag
					if ( cent->currentState.clientNum != -1 && Q_stricmpn(cgs.clientinfo[cent->currentState.clientNum].name, "", 64) ) {
						return va( "^7%s%s has returned %s%s!", cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ, CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time ), TXTCOLOR_OBJ );
					}
					else {
						return va( "%s%s have returned %s%s!", TXTCOLOR_OBJ, cent->currentState.effect2Time == TEAM_ALLIES ? "Allies" : "Axis", CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time ), TXTCOLOR_OBJ );
					}
				case 2: // team_WOLF_redflag/team_WOLF_blueflag
					return va( "^7%s%s has secured %s%s!", cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ, CG_ConfigString( CS_STRINGS + cent->currentState.effect3Time ), TXTCOLOR_OBJ );
				case 3: //core: touching a team_WOLF_checkpoint
					return va( "^7%s%s captured the flag!", cgs.clientinfo[cent->currentState.clientNum].name, TXTCOLOR_OBJ );
			}
			break;
	}

	return NULL;
}

/*
===============
CG_PlayMMItemSound
===============
*/
void CG_PlayMMItemSound( centity_t *cent )
{
	switch( cent->currentState.effect1Time ) {
		case PM_DYNAMITE:
			switch( cent->currentState.effect2Time ) {
				case 0: // planted
					if( cent->currentState.teamNum == TEAM_AXIS ) { // axis dynamite

						if ( cg_altHQVoice.integer )
							CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team != TEAM_ALLIES ? "axis_hq_dynamite_planted" : "allies_hq_dynamite_planted_axis", NULL, -1, qtrue );
						else
							CG_SoundPlaySoundScript( "axis_hq_dynamite_planted", NULL, -1, qtrue );
					}
					else {// allied dynamite

						if ( cg_altHQVoice.integer )
							CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team != TEAM_AXIS ? "allies_hq_dynamite_planted" : "axis_hq_dynamite_planted_allies", NULL, -1, qtrue );
						else
							CG_SoundPlaySoundScript( "allies_hq_dynamite_planted", NULL, -1, qtrue );
					}
					break;
				case 1: // defused
					if( cent->currentState.teamNum == TEAM_AXIS ) { // axis dynamite

						if ( cg_altHQVoice.integer )
							CG_SoundPlaySoundScript( cgs.clientinfo[cg.clientNum].team != TEAM_ALLIES ? "axis_hq_dynamite_defused" : "allies_hq_dynamite_defused_axis", NULL, -1, qtrue );
						else
							CG_SoundPlaySoundScript( "axis_hq_dynamite_defused", NULL, -1, qtrue );
					}
					else { // allied dynamite

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
				}
				else {
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

/*
===============
CG_GetMMItemIcon
===============
*/
qhandle_t CG_GetMMItemIcon( centity_t* cent ) {
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
