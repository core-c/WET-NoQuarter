/*
 * name:	ui_loadpanel.c
 *
 * desc:
 *
 * NQQS:
 *
 */

#include "ui_local.h"
#include "ui_shared.h"

qboolean	bg_loadscreeninited = qfalse;
fontInfo_t	bg_loadscreenfont1;
fontInfo_t	bg_loadscreenfont2;

void UI_LoadPanel_RenderHeaderText( panel_button_t* button );
void UI_LoadPanel_RenderLoadingText( panel_button_t* button );
void UI_LoadPanel_RenderPercentageMeter( panel_button_t* button );
void UI_LoadPanel_DownloadingBarText( panel_button_t* button );
void UI_LoadPanel_RenderDownloadingBar( panel_button_t* button );
void UI_LoadPanel_RenderDownloadingBarBg( panel_button_t* button );

panel_button_text_t missiondescriptionTxt = {
	0.2f, 0.2f,
	{ 0.0f, 0.0f, 0.0f, 1.f },
	0, 0,
	&bg_loadscreenfont2,
};

panel_button_text_t missiondescriptionHeaderTxt = {
	0.2f, 0.2f,
	{ 0.0f, 0.0f, 0.0f, 0.8f },
	 0,ITEM_ALIGN_CENTER,
	&bg_loadscreenfont2,
};

panel_button_text_t campaignpheaderTxt = {
	0.2f, 0.2f,
	{ 1.0f, 1.0f, 1.0f, 0.6f },
	0, 0,
	&bg_loadscreenfont2,
};

panel_button_text_t campaignpTxt = {
	0.35f, 0.35f,
	{ 1.0f, 1.0f, 1.0f, 0.6f },
	0, 0,
	&bg_loadscreenfont2,
};

panel_button_text_t loadScreenMeterBackTxt = {
	0.22f, 0.22f,
	{ 0.1f, 0.1f, 0.1f, 0.8f },
	 0, ITEM_ALIGN_CENTER,
	&bg_loadscreenfont2,
};

panel_button_t loadScreenMap = {
	"gfx/loading/camp_map",
	NULL,
	{ 0, 0, 440, 480 },	// shouldn't this be square??
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t loadScreenBack = {
	"gfx/loading/camp_side",
	NULL,
	{ 440, 0, 200, 480 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t loadingPanelText = {
	NULL,
	NULL,
	{ 460, 72, 160, 244 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&missiondescriptionTxt,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	UI_LoadPanel_RenderLoadingText,
	NULL,
};

panel_button_t campaignPanelText = {
	NULL,
	NULL, //"CONNECTING...",
	{ 470, 33, 152, 232 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&campaignpTxt,			/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	UI_LoadPanel_RenderHeaderText,
	NULL,
};


panel_button_t loadScreenMeterBack = {
	"gfx/loading/progressbar_back",
	NULL,
	{ 440+26, 480-30+1, 200-56, 20 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,
	NULL,
	NULL,
	UI_LoadPanel_RenderDownloadingBarBg,
	NULL,
};

panel_button_t downloadScreenMeterBack = {
	"gfx/loading/progressbar",
	NULL,
	{ 440+26, 480-30+1, 200-56, 20 },
	{ 1, 255, 0, 0, 255, 0, 0, 0 },
	NULL,
	NULL,
	NULL,
	UI_LoadPanel_RenderDownloadingBar,
	NULL,
};
panel_button_t downloadScreenMeterBackText = {
	NULL,
	NULL,
	{ 440+28, 480-28+12+1, 200-56-2, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&loadScreenMeterBackTxt,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	UI_LoadPanel_DownloadingBarText,
	NULL,
};


panel_button_t* loadpanelButtons[] = {
	&loadScreenMap, &loadScreenBack,

	&loadingPanelText, &campaignPanelText,

	&loadScreenMeterBack, &downloadScreenMeterBack, &downloadScreenMeterBackText,

	NULL,
};


/*
================
CG_DrawConnectScreen
================
*/
static qboolean connect_ownerdraw;
void UI_DrawLoadPanel( qboolean forcerefresh, qboolean ownerdraw, qboolean uihack ) {
	static qboolean inside = qfalse;

	if( inside ) {
		if( !uihack && trap_Cvar_VariableValue( "ui_connecting" ) ) {
			trap_Cvar_Set( "ui_connecting", "0" );
		}
		return;
	}

	connect_ownerdraw = ownerdraw;

	inside = qtrue;

	if( !bg_loadscreeninited ) {
		trap_R_RegisterFont( "ariblk", 27, &bg_loadscreenfont1 );
		trap_R_RegisterFont( "courbd", 30, &bg_loadscreenfont2 );

		BG_PanelButtonsSetup( loadpanelButtons );
		C_PanelButtonsSetup( loadpanelButtons, Cui_WideXoffset() );	// core: convert to possible widescreen coordinates..

		bg_loadscreeninited = qtrue;
	}

	BG_PanelButtonsRender( loadpanelButtons );
	/*
	if( forcerefresh ) {
		//trap_UpdateScreen();
	}
	*/
	if( !uihack && trap_Cvar_VariableValue( "ui_connecting" ) ) {
		trap_Cvar_Set( "ui_connecting", "0" );
	}

	inside = qfalse;
}

/*
================
Downloading
================
*/
qboolean Downloading ( void ){
	uiClientState_t	cstate;
	char			downloadName[MAX_INFO_VALUE];

	trap_GetClientState( &cstate );
	trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );

	if( ( cstate.connState == CA_DISCONNECTED || cstate.connState == CA_CONNECTED ) && *downloadName )
		return qtrue;

	return qfalse;
}

/*
================
UI_LoadPanel_RenderDownloadingBarBg
================
*/
void UI_LoadPanel_RenderDownloadingBarBg ( panel_button_t* button ) {

	if ( Downloading() ) {
		BG_PanelButtonsRender_Img( button );
	}
}

/*
================
UI_LoadPanel_DownloadingBarText
================
*/
void UI_LoadPanel_DownloadingBarText( panel_button_t* button ) {
	int downloadSize, downloadCount;
	char dlSizeBuf[64], totalSizeBuf[64];

	if ( !Downloading() )
		return;

	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );

	if ( !downloadSize )
		return;

	UI_ReadableSize( dlSizeBuf,		sizeof dlSizeBuf,		downloadCount );
	UI_ReadableSize( totalSizeBuf,	sizeof totalSizeBuf,	downloadSize );


	Text_Paint_Ext( button->rect.x,
		button->rect.y,
		button->font->scalex,
		button->font->scaley,
		button->font->colour,
		va( "%s/%s(%d%%)", dlSizeBuf, totalSizeBuf,  (int)( (float)downloadCount * 100.0f / (float)downloadSize ) ),
		0, 0, 0,
		button->font->font );
}

/*
================
UI_LoadPanel_RenderDownloadingBar
================
*/
void UI_LoadPanel_RenderDownloadingBar( panel_button_t* button ) {
	int downloadSize, downloadCount;
	float frac;
	float x,y,w,h;

	if ( !Downloading() )
		return;

	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );

	if ( !downloadSize ) {
		return;
	}

	frac = (float)downloadCount/(float)downloadSize;

	if( frac < 0.f )
		frac = 0.f;
	if( frac > 1.f )
		frac = 1.f;

	x = button->rect.x;
	y = button->rect.y;
	w = button->rect.w;
	h = button->rect.h;

	UI_AdjustFrom640( &x, &y, &w, &h );

	trap_R_DrawStretchPic( x, y, w * frac, h, 0, 0, frac, 1, button->hShaderNormal );
}

#define STARTANGLE 40

/*
================
UI_LoadPanel_RenderPercentageMeter
================
*/
void UI_LoadPanel_RenderPercentageMeter( panel_button_t* button ) {
	float hunkfrac = 0.f;
	float w= button->rect.w, h= button->rect.h;
	vec2_t org = {button->rect.x, button->rect.y};
	polyVert_t verts[4];

	AdjustFrom640( &org[0], &org[1], &w, &h );
	SetupRotatedThing( verts, org, w, h, DEG2RAD((180-STARTANGLE) - ((180-(2*STARTANGLE)) * hunkfrac)) );

	trap_R_Add2dPolys( verts, 4, button->hShaderNormal );
}

/*
================
MiniAngleToAxis
================
*/
void MiniAngleToAxis( vec_t angle, vec2_t axes[2] ) {
	axes[0][0] = (vec_t)sin( -angle );
	axes[0][1] = -(vec_t)cos( -angle );

	axes[1][0] = -axes[0][1];
	axes[1][1] = axes[0][0];
}

/*
================
SetupRotatedThing
================
*/
void SetupRotatedThing( polyVert_t* verts, vec2_t org, float w, float h, vec_t angle ) {
	vec2_t axes[2];

	MiniAngleToAxis( angle, axes );

	verts[0].xyz[0] = org[0] - (w * 0.5f) * axes[0][0];
	verts[0].xyz[1] = org[1] - (w * 0.5f) * axes[0][1];
	verts[0].xyz[2] = 0;
	verts[0].st[0] = 0;
	verts[0].st[1] = 1;
	verts[0].modulate[0] = 255;
	verts[0].modulate[1] = 255;
	verts[0].modulate[2] = 255;
	verts[0].modulate[3] = 255;

	verts[1].xyz[0] = verts[0].xyz[0] + w * axes[0][0];
	verts[1].xyz[1] = verts[0].xyz[1] + w * axes[0][1];
	verts[1].xyz[2] = 0;
	verts[1].st[0] = 1;
	verts[1].st[1] = 1;
	verts[1].modulate[0] = 255;
	verts[1].modulate[1] = 255;
	verts[1].modulate[2] = 255;
	verts[1].modulate[3] = 255;

	verts[2].xyz[0] = verts[1].xyz[0] + h * axes[1][0];
	verts[2].xyz[1] = verts[1].xyz[1] + h * axes[1][1];
	verts[2].xyz[2] = 0;
	verts[2].st[0] = 1;
	verts[2].st[1] = 0;
	verts[2].modulate[0] = 255;
	verts[2].modulate[1] = 255;
	verts[2].modulate[2] = 255;
	verts[2].modulate[3] = 255;

	verts[3].xyz[0] = verts[2].xyz[0] - w * axes[0][0];
	verts[3].xyz[1] = verts[2].xyz[1] - w * axes[0][1];
	verts[3].xyz[2] = 0;
	verts[3].st[0] = 0;
	verts[3].st[1] = 0;
	verts[3].modulate[0] = 255;
	verts[3].modulate[1] = 255;
	verts[3].modulate[2] = 255;
	verts[3].modulate[3] = 255;
}

/*
================
UI_LoadPanel_RenderHeaderText
================
*/
void UI_LoadPanel_RenderHeaderText( panel_button_t* button ) {
	uiClientState_t	cstate;
	char			downloadName[MAX_INFO_VALUE];

	trap_GetClientState( &cstate );

	trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );

	if( ( cstate.connState == CA_DISCONNECTED || cstate.connState == CA_CONNECTED ) && *downloadName ) {
		button->text = "DOWNLOADING...";
	}
	else {
		button->text = "CONNECTING...";
	}

	BG_PanelButtonsRender_Text( button );
}

#define ESTIMATES 80
const char *UI_DownloadInfo( const char *downloadName ) {

	static int	tleEstimates[ESTIMATES] = { 60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
											60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
											60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,
											60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60 };
	static int	tleIndex = 0;

	char dlSizeBuf[64], totalSizeBuf[64];

	const char *s, *ds;
	int downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	int downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );
	int downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );

	if( downloadSize > 0 ) {
		ds = va( "%s (%d%%)", downloadName, (int)( (float)downloadCount * 100.0f / (float)downloadSize ) );
	}
	else {
		ds = downloadName;
	}

	UI_ReadableSize( dlSizeBuf,		sizeof dlSizeBuf,		downloadCount );
	UI_ReadableSize( totalSizeBuf,	sizeof totalSizeBuf,	downloadSize );

	if( downloadCount < 4096 || !downloadTime ) {
		s = va( "%s\n\n estimating...", downloadName);
		return s;
	}
	else {
		char xferRateBuf[64], dlTimeBuf[64];
		int xferRate;

		if( ( uiInfo.uiDC.realTime - downloadTime ) / SECONDS_1 ) {
			xferRate = downloadCount / ( ( uiInfo.uiDC.realTime - downloadTime ) / SECONDS_1 );
		}
		else {
			xferRate = 0;
		}
		UI_ReadableSize( xferRateBuf, sizeof xferRateBuf, xferRate );

		// Extrapolate estimated completion time
		if( downloadSize && xferRate ) {
			int n = downloadSize / xferRate; // estimated time for entire d/l in secs
			int timeleft = 0, i;

			// We do it in K (/1024) because we'd overflow around 4MB
			tleEstimates[ tleIndex ] = (n - (((downloadCount/1024) * n) / (downloadSize/1024)));
			tleIndex++;
			if( tleIndex >= ESTIMATES )
				tleIndex = 0;

			for( i = 0; i<ESTIMATES; i++ )
				timeleft += tleEstimates[ i ];

			timeleft /= ESTIMATES;

			UI_PrintTime( dlTimeBuf, sizeof dlTimeBuf, timeleft );
		}
		else {
			dlTimeBuf[0] = '\0';
		}

		if( xferRate ) {
			s = va( "%s\n\n %s(%s/s)", downloadName, dlTimeBuf, xferRateBuf);
		}
		else {
			s = va( "%s\n\n estimating...", downloadName );
		}

		return s;
	}

    return "";
}

void UI_LoadPanel_RenderLoadingText( panel_button_t* button )
{
	uiClientState_t	cstate;
	char				downloadName[MAX_INFO_VALUE];
	char				buff[2560];
	static connstate_t	lastConnState;
	static char			lastLoadingText[MAX_INFO_VALUE];
	char				*p, *s = "";
	float				y;
#ifndef _DEBUG
	char 				versionStr[512];

	trap_Cvar_VariableStringBuffer( "version", versionStr, sizeof(versionStr) );
	if ( strstr( versionStr, "2.55") || strstr( versionStr, "2.56")) {
		Com_Error(ERR_FATAL, va("Invalid ET version: %s\nInstall 2.60 or greater.\n", versionStr ));
		return;
	}
#endif

	trap_GetClientState( &cstate );

	Com_sprintf( buff, sizeof(buff), "Connecting to:\n %s^*\n\n%s", cstate.servername, Info_ValueForKey( cstate.updateInfoString, "motd" ) );

	trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );

	if ( lastConnState > cstate.connState ) {
		lastLoadingText[0] = '\0';
	}
	lastConnState = cstate.connState;

	if( !connect_ownerdraw ) {
		if( !trap_Cvar_VariableValue( "ui_connecting" ) ) {
			switch( cstate.connState ) {
			case CA_CONNECTING:
				s = va( trap_TranslateString( "Awaiting connection...%i" ), cstate.connectPacketCount );
				break;
			case CA_CHALLENGING:
				s = va( trap_TranslateString( "Awaiting challenge...%i" ), cstate.connectPacketCount );
				break;
			case CA_DISCONNECTED:
			case CA_CONNECTED:
				if( *downloadName || cstate.connState == CA_DISCONNECTED ) {
					s = (char *)UI_DownloadInfo( downloadName );
				} else {
					s = trap_TranslateString( "Awaiting gamestate..." );
				}
				break;
			case CA_LOADING:
			case CA_PRIMED:
			default:
				break;
			}
		}
		else if ( trap_Cvar_VariableValue( "ui_dl_running" ) ) {
			// only toggle during a disconnected download
			s = (char *)UI_DownloadInfo( downloadName );
		}

		Q_strcat( buff, sizeof(buff), va( "\n\n%s^*", s ) );

		if( cstate.connState < CA_CONNECTED && *cstate.messageString )
			Q_strcat( buff, sizeof(buff), va( "\n\n%s^*", cstate.messageString ) );
	}

	if ( Downloading() )
		Com_sprintf( buff, sizeof(buff), va("Connecting to:\n %s^*\n\n Needed paks: \n %s", cstate.servername, UI_Cvar_VariableString("com_missingFiles")) );


	BG_FitTextToWidth_Ext( buff, button->font->scalex, button->rect.w, sizeof(buff), button->font->font );

	y = button->rect.y + 12;

	s = p = buff;

	while( *p ) {
		if( *p == '\n' ) {
			*p++ = '\0';
			Text_Paint_Ext( button->rect.x + 4, y, button->font->scalex, button->font->scaley, button->font->colour, s, 0, 0, 0, button->font->font );
			y += 8;
			s = p;
		}
		else {
			p++;
		}
	}

	if ( !Downloading() )
		return;

	Q_strncpyz( buff, (char *)UI_DownloadInfo( downloadName ), sizeof(buff) );
	BG_FitTextToWidth_Ext( buff, 0.2f, 155, sizeof(buff), &bg_loadscreenfont2 );

	y = 358;
	s = p = buff;

	while( *p ) {
		if( *p == '\n' ) {
			*p++ = '\0';
			Text_Paint_Ext( 465, y, 0.2f, 0.2f, colorWhite, s, 0, 0, 0, &bg_loadscreenfont2 );
			y += 8;
			s = p;
		}
		else {
			p++;
		}
	}
}
