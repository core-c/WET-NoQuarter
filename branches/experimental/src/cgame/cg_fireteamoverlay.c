/******************************************************************************
***** teh firetams! (sic)
****/

#include "cg_local.h"

/******************************************************************************
***** Defines, constants, etc
****/

static int sortedFireTeamClients[MAX_CLIENTS];

/******************************************************************************
***** Support Routines
****/

int QDECL CG_SortFireTeam( const void *a, const void *b )
{
	clientInfo_t	*ca, *cb;
	int				cna, cnb;

	cna = *(int*)a;
	cnb = *(int*)b;

	ca = &cgs.clientinfo[cna];
	cb = &cgs.clientinfo[cnb];

	// Not on our team, so shove back
	if ( !CG_IsOnSameFireteam( cnb, cg.clientNum ) ) {
		return -1;
	}
	if ( !CG_IsOnSameFireteam( cna, cg.clientNum ) ) {
		return 1;
	}

	// Leader comes first
	if ( CG_IsFireTeamLeader( cna ) ) {
		return -1;
	}
	if ( CG_IsFireTeamLeader( cnb ) ) {
		return 1;
	}

	// Then higher ranks
	if ( ca->rank > cb->rank ) {
		return -1;
	}
	if ( cb->rank > ca->rank ) {
		return 1;
	}

	// Then score
/*	if ( ca->score > cb->score ) {
		return -1;
	}
	if ( cb->score > ca->score ) {
		return 1;
	}*/ // not atm

	return 0;
}



// Sorts client's fireteam by leader then rank
void CG_SortClientFireteam()
{
	int i;

	for(i = 0; i < cgs.maxclients; i++) {
		sortedFireTeamClients[i] = i;
	}

	qsort( sortedFireTeamClients, cgs.maxclients, sizeof(sortedFireTeamClients[0]), CG_SortFireTeam );
}

// Parses fireteam servercommand
void CG_ParseFireteams()
{
	int i, j;
	char* s;
	const char* p;
	int clnts[2];

	for(i = 0; i < cgs.maxclients; i++) {
		cgs.clientinfo[i].fireteamData = NULL;
	}

	for(i = 0; i < MAX_FIRETEAMS; i++) {
		char hexbuffer[11] = "0x00000000";
		p = CG_ConfigString(CS_FIRETEAMS + i);

		j = atoi(Info_ValueForKey(p, "k")); // "id" -> "k"
		if(j == -1) {
			cg.fireTeams[i].inuse = qfalse;
			continue;
		} else {
			cg.fireTeams[i].inuse = qtrue;
			cg.fireTeams[i].ident = j;
		}

		s = Info_ValueForKey(p, "l");
		cg.fireTeams[i].leader = atoi(s);

		s = Info_ValueForKey(p, "c");
		Q_strncpyz(hexbuffer+2, s, 9);
		sscanf(hexbuffer, "%x", &clnts[1]);
		Q_strncpyz(hexbuffer+2, s+8, 9);
		sscanf(hexbuffer, "%x", &clnts[0]);

		for(j = 0; j < cgs.maxclients; j++) {
			if(COM_BitCheck(clnts, j)) {
				cg.fireTeams[i].joinOrder[j] = qtrue;
				cgs.clientinfo[j].fireteamData = &cg.fireTeams[i];
			} else {
				cg.fireTeams[i].joinOrder[j] = qfalse;
			}
		}
	}

	CG_SortClientFireteam();
}

// Fireteam that both specified clients are on, if they both are on the same team
fireteamData_t* CG_IsOnSameFireteam( int clientNum, int clientNum2 )
{
	if(CG_IsOnFireteam(clientNum) == CG_IsOnFireteam(clientNum2)) {
		return CG_IsOnFireteam(clientNum);
	}

	return NULL;
}

// Fireteam that the specified client is a part of
fireteamData_t* CG_IsOnFireteam(int clientNum)
{
	if (cgs.clientinfo[clientNum].team == TEAM_SPECTATOR) return NULL;
	return cgs.clientinfo[clientNum].fireteamData;
}

// Fireteam that specified client is leader of, or NULL if none
fireteamData_t* CG_IsFireTeamLeader( int clientNum ) {
	fireteamData_t* f;

	if(!(f = CG_IsOnFireteam(clientNum))) {
		return NULL;
	}

	if(f->leader != clientNum) {
		return NULL;
	}

	return f ;
}

// Client, not on a fireteam, not sorted, but on your team
clientInfo_t* CG_ClientInfoForPosition(int pos, int max)
{
	int i, cnt = 0;
	int clientNum;

	for( i = 0; i < cgs.numValidClients && cnt < max; ++i ) {
		clientNum = cgs.validClients[i];
		if(cg.clientNum != clientNum && !CG_IsOnFireteam(clientNum) && cgs.clientinfo[cg.clientNum].team == cgs.clientinfo[clientNum].team ) {
			if(cnt == pos) {
				return &cgs.clientinfo[clientNum];
			}
			cnt++;
		}
	}
	return NULL;
}

// Fireteam, that's on your same team
fireteamData_t* CG_FireTeamForPosition( int pos, int max )
{
	int i, cnt = 0;

	for(i = 0; i < MAX_FIRETEAMS && cnt < max; i++) {
		if(cg.fireTeams[i].inuse && cgs.clientinfo[cg.fireTeams[i].leader].team == cgs.clientinfo[cg.clientNum].team) {
			if(cnt == pos) {
				return &cg.fireTeams[i];
			}
			cnt++;
		}
	}

	return NULL;
}

// Client, not sorted by rank, on CLIENT'S fireteam
clientInfo_t* CG_FireTeamPlayerForPosition(int pos, int max)
{
	int i, cnt = 0;
	fireteamData_t* f = CG_IsOnFireteam(cg.clientNum);
	int clientNum;

	if(!f) {
		return NULL;
	}

	for( i = 0; i < cgs.numValidClients && cnt < max; ++i ) {
		clientNum = cgs.validClients[i];
		if(cgs.clientinfo[cg.clientNum].team == cgs.clientinfo[clientNum].team ) {
			if(!(f == CG_IsOnFireteam(clientNum))) {
				continue;
			}

			if(cnt == pos) {
				return &cgs.clientinfo[clientNum];
			}
			cnt++;
		}
	}
	return NULL;
}


// Client, sorted by rank, on CLIENT'S fireteam
clientInfo_t* CG_SortedFireTeamPlayerForPosition(int pos)//, int max)
{
	int				i;
	int				cnt = 0;
	fireteamData_t* f	= CG_IsOnFireteam(cg.clientNum);

	if(!f) {
		return NULL;
	}

	for(i = 0; i < cgs.maxclients && cnt < MAX_FIRETEAM_MEMBERS; i++) {
		if(!(f == CG_IsOnFireteam(sortedFireTeamClients[i]))) {
			return NULL;
		}

		if(cnt == pos) {
			return &cgs.clientinfo[sortedFireTeamClients[i]];
		}
		cnt++;
	}

	return NULL;
}

/******************************************************************************
***** Main Functions
****/


int SkillNumForClass( int classNum )
{
	switch( classNum ) {
		case PC_SOLDIER:
			return SK_SOLDIER;
		case PC_MEDIC:
			return SK_MEDIC;
		case PC_ENGINEER:
			return SK_ENGINEER;
		case PC_FIELDOPS:
			return SK_SIGNALS;
		case PC_COVERTOPS:
			return SK_COVERTOPS;
		default:
			return SK_BATTLE_SENSE;
	}
}

#define FT_BAR_YSPACING 2.f
#define FT_BAR_HEIGHT	10.f
//#define FT_BOX_WIDTH	301//maxLen + 224	// jP default is 204, increased size for location names
int weaponIconScale(int weap)
{

	if ( cg_weapons[weap].weaponIconScale )
		return cg_weapons[weap].weaponIconScale;

	switch(weap) {
		// weapons with 'wide' icons
		case WP_MP34:
		case WP_THOMPSON:
		case WP_MP40:
		case WP_STEN:
		case WP_STEN_MKII:
		case WP_STG44:
		case WP_BAR:
		case WP_BAR_SET:
		case WP_PANZERFAUST:
		case WP_BAZOOKA:
		case WP_FLAMETHROWER:
		case WP_SHOTGUN:
		case WP_GARAND:
		case WP_FG42:
		case WP_FG42SCOPE:
		case WP_KAR98:
		case WP_GPG40:
		case WP_CARBINE:
		case WP_M7:
		case WP_MOBILE_BROWNING:
		case WP_MOBILE_BROWNING_SET:
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
		case WP_K43:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
		case WP_MORTAR:
		case WP_MORTAR_SET:
		case WP_MORTAR2:
		case WP_MORTAR2_SET:
		case WP_VENOM:
			return 2;
	}

	return 1;
}


void CG_DrawFireTeamOverlay( rectDef_t* rect )
{
	int				x 			= rect->x;
	int				y 			= rect->y + 1;	// +1, jitter it into place in 1024 :)
	int				i;
	int				boxWidth	= 106;//164;//301;
	char			buffer[64];
	float			h			= 16;			// 12 + 2 + 2
	clientInfo_t*	ci			= NULL;
	fireteamData_t* f			= NULL;
	static vec4_t 	clr1 		= { 0.0f,	0.0f,	0.0f,	0.8f }; // box itselfs - { 0.16f,	0.2f,	0.17f,	0.8f }
	static vec4_t 	clr2 		= { 0.0f,	0.0f,	0.0f,	0.2f }; // not selected
	static vec4_t 	clr3 		= { 0.25f,	0.0f,	0.0f,	0.6f }; // selected member
//	vec2_t			loc;
	int				bestWidth	= -1;
	char			*locStr[MAX_FIRETEAM_MEMBERS];

	memset( locStr, 0, sizeof(locStr));

	// assign fireteam data, and early out if not on one
	if(!(f = CG_IsOnFireteam( cg.clientNum ))) {
		return;
	}

	// sort the fireteam
	for(i = 0; i < MAX_FIRETEAM_MEMBERS; i++) {
		ci = CG_SortedFireTeamPlayerForPosition( i );

		if (!ci) break;

		h += 12.f;
	}

	// First get name and location width, also store location names
	for(i = 0; i < MAX_FIRETEAM_MEMBERS; i++) {
		int		locwidth;
		int		namewidth;
		vec3_t	origin;

		ci = CG_SortedFireTeamPlayerForPosition( i );

		// Make sure it's valid
		if(!ci) break;

		origin[0] = ci->location[0];
		origin[1] = ci->location[1];
		origin[2] = ci->location[2];

		locStr[i] = CG_BuildLocationString( ci->clientNum, origin, LOC_FTEAM );

		if( !locStr[i][1] || !*locStr[i] )
			locStr[i] = "";

		locwidth = CG_Text_Width_Ext( locStr[i], 0.2f, 0, &cgs.media.limboFont2 );

		// Meyer - changed back
		if ( cg_fixedFTeamSize.integer ) {
			namewidth = 102;
		}
		else {
			namewidth = CG_Text_Width_Ext(ci->name, 0.2f, 17, &cgs.media.limboFont2 ); // jaquboss - changed

			if ( ci->health == 0 )
				namewidth += 7;
		}

		if ( (locwidth + namewidth) > bestWidth)
			bestWidth = locwidth + namewidth;
	}

	boxWidth +=	bestWidth;

	CG_DrawRect( x,		y,		boxWidth,	  h,		1,	 HUD_Border);
	CG_FillRect( x + 1, y + 1,	boxWidth - 2, h - 2,        HUD_Background);

	x += 2;
	y += 2;

	CG_FillRect( x,		y,		boxWidth - 4, 12,	clr1 );

	Com_sprintf(buffer, 64, "Fireteam: %s", bg_fireteamNames[f->ident] );
	Q_strupr( buffer );
	CG_Text_Paint_Ext( x + 3, y + FT_BAR_HEIGHT, .19f, .19f, HUD_Text, buffer, 0, 0, 0, &cgs.media.limboFont1 );

	x += 2;

	for(i = 0; i < MAX_FIRETEAM_MEMBERS; i++) {
		int		curWeap;

		y += FT_BAR_HEIGHT + FT_BAR_YSPACING;
		x = rect->x + 2;

		// Grab a pointer to the current player
		ci = CG_SortedFireTeamPlayerForPosition( i );

		// Make sure it's valid
		if(!ci) break;

		// hilight selected players
		if( ci->selected )
			CG_FillRect( x, y + FT_BAR_YSPACING, boxWidth - 4, FT_BAR_HEIGHT, clr3 );
		else
			CG_FillRect( x, y + FT_BAR_YSPACING, boxWidth - 4, FT_BAR_HEIGHT, clr2 );

		x += 4;

		// jet Pilot - draw class icon in fireteam overlay
		CG_DrawPic( x, y, 12, 12, cgs.media.skillPics[SkillNumForClass(ci->cls)]);
		x += 14;

		// core: draw the mute-icon in the fireteam overlay..
		if ( ci->muted ) {
			CG_DrawPic( x, y, 12, 12, cgs.media.muteIcon );
			x += 14;
		}
		// core: ..or else draw objective icon (if they are carrying one) in fireteam overlay..
		else if( ci->powerups & ((1 << PW_REDFLAG) | (1 << PW_BLUEFLAG)) ) {
			CG_DrawPic( x, y, 12, 12, cgs.media.objectiveShader );
			x += 14;
		}
		// core: ..or else draw the disguised icon  in fireteam overlay..
		else if ( ci->powerups & (1<<PW_OPS_DISGUISED) ) {
			CG_DrawPic( x, y, 12, 12, ci->team == TEAM_AXIS ? cgs.media.alliedUniformShader : cgs.media.axisUniformShader );
			x += 14;
		}
		// core: ..otherwise draw rank icon in fireteam overlay
		else {
			if (ci->rank > 0) CG_DrawPic( x, y, 12, 12, rankicons[ ci->rank ][  ci->team == TEAM_AXIS ? 1 : 0 ][0].shader );
			x += 14;
		}

		// draw the player's name
		CG_Text_Paint_Ext( x, y + FT_BAR_HEIGHT, .2f, .2f, HUD_Text, ci->name, 0, 17, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont2 );

		// add space
		if ( cg_fixedFTeamSize.integer )
			x += 115;
		else
			x += 14 + CG_Text_Width_Ext(ci->name, 0.2f, 17, &cgs.media.limboFont2 ); // jaquboss - changed

		// jet Pilot - Draw the player's weapon icon
		curWeap	= cg_entities[ ci->clientNum ].currentState.weapon;

		if ( cg_weapons[ curWeap ].weaponIcon[0] ) // jaquboss - do not try to draw nothing
			CG_DrawPic( x, y, weaponIconScale(curWeap) * 10, 10, cg_weapons[ curWeap ].weaponIcon[0] );
		else if (  cg_weapons[ curWeap ].weaponIcon[1] )
			CG_DrawPic( x, y, weaponIconScale(curWeap) * 10, 10, cg_weapons[ curWeap ].weaponIcon[1] );

		x += 24;

		if( ci->health > 80 ){
			CG_Text_Paint_Ext(x, y + FT_BAR_HEIGHT,  .2f, .2f, HUD_Text,	va("%i", ci->health < 0 ? 0 : ci->health ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont2 );
		} else if( ci->health > 0 ) {
			CG_Text_Paint_Ext(x, y + FT_BAR_HEIGHT,  .2f, .2f, colorYellow, va("%i", ci->health < 0 ? 0 : ci->health ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont2 );
		} else if ( ci->health == 0 ){
			CG_Text_Paint_Ext( x, y + FT_BAR_HEIGHT, .2f, .2f, ((cg.time % 500) > 250)  ? colorWhite : colorRed, "*", 0, 17, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont2 );
			x += 7;
			CG_Text_Paint_Ext(x, y + FT_BAR_HEIGHT,  .2f, .2f, ((cg.time % 500) > 250)  ? colorRed : colorWhite, "0", 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont2 );
			x -= 7;
		} else {
			CG_Text_Paint_Ext(x, y + FT_BAR_HEIGHT,  .2f, .2f,  colorRed ,	"0", 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont2 );
		}
		// Meyer - Set hard limit on width
		x += 24;

		// jaquboss - pants meyer this was incredibly wrong
		CG_Text_Paint_Ext( x, y + FT_BAR_HEIGHT,  .2f, .2f, HUD_Text, locStr[i] , 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont2 );

	}
}

qboolean CG_FireteamGetBoxNeedsButtons()
{
	if ( cgs.applicationEndTime > cg.time ) {
		if( cgs.applicationClient < 0 ) {
			return qfalse;
		}
		return qtrue;
	}

	if ( cgs.invitationEndTime > cg.time ) {
		if( cgs.invitationClient < 0 ) {
			return qfalse;
		}
		return qtrue;
	}

	if ( cgs.propositionEndTime > cg.time ) {
		if( cgs.propositionClient < 0 ) {
			return qfalse;
		}
		return qtrue;
	}

	return qfalse;
}

const char* CG_FireteamGetBoxText() {
	if ( cgs.applicationEndTime > cg.time )
	{
		if ( cgs.applicationClient == -1 ) {
			return "Sent";
		}

		if ( cgs.applicationClient == -2 ) {
			return "Failed";
		}

		if ( cgs.applicationClient == -3 ) {
			return "Accepted";
		}

		if ( cgs.applicationClient == -4 ) {
			return "Sent";
		}

		if( cgs.applicationClient < 0 ) {
			return NULL;
		}

		return va("Accept application from %s?", cgs.clientinfo[cgs.applicationClient].name);
	}

	if ( cgs.invitationEndTime > cg.time ) {
		if ( cgs.invitationClient == -1 ) {
			return "Sent";
		}

		if ( cgs.invitationClient == -2 ) {
			return "Failed";
		}

		if ( cgs.invitationClient == -3 ) {
			return "Accepted";
		}

		if ( cgs.invitationClient == -4 ) {
			return "Sent";
		}

		if( cgs.invitationClient < 0 ) {
			return NULL;
		}

		return va("Accept invitiation from %s?", cgs.clientinfo[cgs.invitationClient].name);
	}

	if ( cgs.propositionEndTime > cg.time ) {
		if ( cgs.propositionClient == -1 ) {
			return "Sent";
		}

		if ( cgs.propositionClient == -2 ) {
			return "Failed";
		}

		if ( cgs.propositionClient == -3 ) {
			return "Accepted";
		}

		if ( cgs.propositionClient == -4 ) {
			return "Sent";
		}

		if( cgs.propositionClient < 0 ) {
			return NULL;
		}

		return va( "Accept %s's proposition to invite %s to join your fireteam?", cgs.clientinfo[cgs.propositionClient2].name, cgs.clientinfo[cgs.propositionClient].name);
	}

	return NULL;
}

qboolean CG_FireteamHasClass( int classnum, qboolean selectedonly )
{
	fireteamData_t* ft;
	int i;
	int clientNum;

	if( !(ft = CG_IsOnFireteam( cg.clientNum )) ) {
		return qfalse;
	}

	for( i = 0; i < cgs.numValidClients; ++i ) {
		clientNum = cgs.validClients[i];
		if( ft != CG_IsOnFireteam(clientNum) ) {
			continue;
		}

		if( cgs.clientinfo[clientNum].cls != classnum ) {
			continue;
		}

		if( selectedonly && !cgs.clientinfo[clientNum].selected ) {
			continue;
		}

		return qtrue;
	}
	return qfalse;
}

const char* CG_BuildSelectedFirteamString( void )
{
	char buffer[256];
	clientInfo_t* ci;
	int cnt = 0;
	int i;

	*buffer = '\0';
	for( i = 0; i < MAX_FIRETEAM_MEMBERS; i++ ) {
		ci = CG_SortedFireTeamPlayerForPosition( i );
		if( !ci ) {
			break;
		}

		if( !ci->selected ) {
			continue;
		}

		cnt++;

		Q_strcat( buffer, sizeof( buffer ), va( "%i ", ci->clientNum ) );
	}

	if( cnt == 0 ) {
		return "0";
	}

	if( !cgs.clientinfo[cg.clientNum].selected ) {
		Q_strcat( buffer, sizeof( buffer ), va( "%i ", cg.clientNum ) );
		cnt++;
	}

	return va( "%i %s", cnt, buffer );
}

float CG_FireteamOverlayHeight( void )
{
	float	h = 0;

	if ( cg_drawFireteamOverlay.integer ) {
		if( CG_IsOnFireteam( cg.clientNum ) ) {
			int		i,j;
			int		clientNum;

			h = 16.f; // 12 + 2 + 2
			for( i=j=0; i<cgs.numValidClients && j<MAX_FIRETEAM_MEMBERS; ++i ) {
				clientNum = cgs.validClients[i];
				if ( CG_IsOnSameFireteam(clientNum, cg.clientNum) != NULL ) j++;
			}
			h += j * (FT_BAR_HEIGHT + FT_BAR_YSPACING);
		}
	}
	return h;
}
