// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

#include "cg_local.h"

// jet Pilot - Unified HUD colors
vec4_t	HUD_Background	= { 0.16f,  0.2f,   0.17f, 0.8f };
vec4_t	HUD_Border		= { 0.5f,   0.5f,   0.5f,  0.8f };
vec4_t	HUD_Text		= { 0.625f, 0.625f, 0.6f,  1.0f };
vec4_t	HUD_Progress	= { 0.25f,  0.75f,  0.25f, 0.8f };
float	HUD_Alpha		= 0.8f;

//tclr	   =	{ 0.625f,	0.625f,	0.6f,	1.0f	};


#define STATUSBARHEIGHT 452
char* BindingFromName(const char *cvar);
void Controls_GetConfig( void );
void SetHeadOrigin(clientInfo_t *ci, playerInfo_t *pi);
void CG_DrawOverlays();
int activeFont;

#define TEAM_OVERLAY_TIME 1000

////////////////////////
////////////////////////
////// new hud stuff
///////////////////////
///////////////////////

void CG_Text_SetActiveFont( int font ) {
	activeFont = font;
}

int CG_Text_Width_Ext( const char *text, float scale, int limit, fontInfo_t* font ) {
	int count, len;
	glyphInfo_t *glyph;
	const char *s = text;
	float out, useScale = scale * font->glyphScale;
	
	out = 0;
	if( text ) {
		len = strlen( text );
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}

	return out * useScale;
}

int CG_Text_Width( const char *text, float scale, int limit ) {
	fontInfo_t *font = &cgDC.Assets.fonts[activeFont];

	return CG_Text_Width_Ext( text, scale, limit, font );
}

int CG_Text_Height_Ext( const char *text, float scale, int limit, fontInfo_t* font ) {
 int len, count;
	float max;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;

	useScale = scale * font->glyphScale;
	max = 0;
	if (text) {
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];
	      if (max < glyph->height) {
		      max = glyph->height;
			  }
				s++;
				count++;
			}
		}
	}
	return max * useScale;
}

int CG_Text_Height( const char *text, float scale, int limit ) {
	fontInfo_t *font = &cgDC.Assets.fonts[activeFont];

	return CG_Text_Height_Ext( text, scale, limit, font );
}

void CG_Text_PaintChar_Ext(float x, float y, float w, float h, float scalex, float scaley, float s, float t, float s2, float t2, qhandle_t hShader) {
	w *= scalex;
	h *= scaley;
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader) {
	float w, h;
	w = width * scale;
	h = height * scale;
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint_Centred_Ext( float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font ) {
	x -= CG_Text_Width_Ext( text, scalex, limit, font ) * 0.5f;

	CG_Text_Paint_Ext( x, y, scalex, scaley, color, text, adjust, limit, style, font );
}

void CG_Text_Paint_Ext( float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font ) {
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;

	scalex *= font->glyphScale;
	scaley *= font->glyphScale;

	if (text) {
		const char *s = text;
		trap_R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[(unsigned char)*s];
			if ( Q_IsColorString( s ) ) {
				if( *(s+1) == COLOR_NULL ) {
					memcpy( newColor, color, sizeof(newColor) );
				} else {
					memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
					newColor[3] = color[3];
				}
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			} else {
				float yadj = scaley * glyph->top;
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor( colorBlack );
					CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex) + ofs, y - yadj + ofs, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
					colorBlack[3] = 1.0;
					trap_R_SetColor( newColor );
				}
				CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex), y - yadj, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
				x += (glyph->xSkip * scalex) + adjust;
				s++;
				count++;
			}
		}
		trap_R_SetColor( NULL );
	}
}

void CG_Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style) {
	fontInfo_t *font = &cgDC.Assets.fonts[activeFont];

	CG_Text_Paint_Ext( x, y, scale, scale, color, text, adjust, limit, style, font );
}

// NERVE - SMF - added back in
int CG_DrawFieldWidth (int x, int y, int width, int value, int charWidth, int charHeight ) {
	char	num[16], *ptr;
	int		l;
	int		frame;
	int		totalwidth = 0;

	if ( width < 1 ) {
		return 0;
	}

	// draw number string
	if ( width > 5 ) {
		width = 5;
	}

	switch ( width ) {
	case 1:
		value = value > 9 ? 9 : value;
		value = value < 0 ? 0 : value;
		break;
	case 2:
		value = value > 99 ? 99 : value;
		value = value < -9 ? -9 : value;
		break;
	case 3:
		value = value > 999 ? 999 : value;
		value = value < -99 ? -99 : value;
		break;
	case 4:
		value = value > 9999 ? 9999 : value;
		value = value < -999 ? -999 : value;
		break;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		totalwidth += charWidth;
		ptr++;
		l--;
	}

	return totalwidth;
}

int CG_DrawField (int x, int y, int width, int value, int charWidth, int charHeight, qboolean dodrawpic, qboolean leftAlign ) {
	char	num[16], *ptr;
	int		l;
	int		frame;
	int		startx;

	if ( width < 1 ) {
		return 0;
	}

	// draw number string
	if ( width > 5 ) {
		width = 5;
	}

	switch ( width ) {
	case 1:
		value = value > 9 ? 9 : value;
		value = value < 0 ? 0 : value;
		break;
	case 2:
		value = value > 99 ? 99 : value;
		value = value < -9 ? -9 : value;
		break;
	case 3:
		value = value > 999 ? 999 : value;
		value = value < -99 ? -99 : value;
		break;
	case 4:
		value = value > 9999 ? 9999 : value;
		value = value < -999 ? -999 : value;
		break;
	}

	Com_sprintf (num, sizeof(num), "%i", value);
	l = strlen(num);
	if (l > width)
		l = width;

	// NERVE - SMF
	if ( !leftAlign ) {
		x -= 2 + charWidth*(l);
	}

	startx = x;

	ptr = num;
	while (*ptr && l)
	{
		if (*ptr == '-')
			frame = STAT_MINUS;
		else
			frame = *ptr -'0';

		if ( dodrawpic )
			CG_DrawPic( x,y, charWidth, charHeight, cgs.media.numberShaders[frame] );
		x += charWidth;
		ptr++;
		l--;
	}

	return startx;
}
// -NERVE - SMF

/*
================
CG_Draw3DModel

================
*/
void CG_Draw3DModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles ) {
	refdef_t		refdef;
	refEntity_t		ent;

	CG_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );
	VectorCopy( origin, ent.origin );
	ent.hModel = model;
	ent.customSkin = skin;
	ent.renderfx = RF_NOSHADOW;		// no stencil shadows

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 30;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );
	trap_R_RenderScene( &refdef );
}

/*
==============
CG_DrawKeyModel
==============
*/
void CG_DrawKeyModel( int keynum, float x, float y, float w, float h, int fadetime) {
	qhandle_t		cm;
	float			len;
	vec3_t			origin, angles;
	vec3_t			mins, maxs;

	VectorClear( angles );

	cm = cg_items[keynum].models[0];

	// offset the origin y and z to center the model
	trap_R_ModelBounds( cm, mins, maxs );

	origin[2] = -0.5 * ( mins[2] + maxs[2] );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );

//	len = 0.5 * ( maxs[2] - mins[2] );		
	len = 0.75 * ( maxs[2] - mins[2] );		
	origin[0] = len / 0.268;	// len / tan( fov/2 )

	angles[YAW] = 30 * sin( cg.time / 2000.0 );;

	CG_Draw3DModel( x, y, w, h, cg_items[keynum].models[0], 0, origin, angles);
}

/*
================
CG_DrawTeamBackground

================
*/
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team )
{
	vec4_t		hcolor;

	hcolor[3] = alpha;
	if ( team == TEAM_AXIS ) {
		hcolor[0] = 1;
		hcolor[1] = 0;
		hcolor[2] = 0;
	} else if ( team == TEAM_ALLIES ) {
		hcolor[0] = 0;
		hcolor[1] = 0;
		hcolor[2] = 1;
	} else {
		return;
	}
	trap_R_SetColor( hcolor );
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
	trap_R_SetColor( NULL );
}

/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

#define UPPERRIGHT_X 634
/*
==================
CG_DrawSnapshot
==================
*/
static float CG_DrawSnapshot( float y, qboolean draw  ) {
	char		*s;
	int			w;
	if (draw)
	{
		s = va( "time:%i snap:%i cmd:%i", cg.snap->serverTime, 
			cg.latestSnapshotNum, cgs.serverCommandSequence );
		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

		CG_DrawBigString( UPPERRIGHT_X - w, y + 2, s, 1.0F);
	}

	return y + BIGCHAR_HEIGHT + 4;
}


// jet Pilot - draw ping or average ping
static float CG_DrawPing( float y, qboolean draw )
{
	float		avgPing		= 0;
	static int	lastPing	= 0;
	int			curPing		= cg.snap->ping;
	int			w;
	char		*s;

	if (draw)
	{
    	avgPing = (float)curPing + (float)lastPing;
		avgPing /= 2.0f;

		

		if (cg_drawPing.integer == 1)
		{
			s = va("Ping %d", curPing < 999 ? curPing : 999);
		}
		else 
		{
			s = va("Avg Ping %0.2f", avgPing);
		}

		w = CG_Text_Width_Ext( s, 0.19f, 0, &cgs.media.limboFont1 );

		CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2,				HUD_Background );
		CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, HUD_Border );

		CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, avgPing > 200 ? colorRed : HUD_Text, s, 0, 0, 0, &cgs.media.limboFont1 );	
	}

	return y + 12 + 4;
}

// jet Pilot - draw kill spree counter
static float CG_DrawKillSpree( float y, qboolean draw  )
{
	char		 *s;
	char		 *kills_string;
	char		 *totalKills_string;
	char		 *bounty_string;
	int			 w;
	int			 kills;
	unsigned int bounty;
	static int	 lastKills		= 0;
	static int	 lastKillTime	= 0;
	float		 flashScale		= 0;
	static int   flashTime		= 0;
	static int   totalKills		= 0;
	vec4_t		 flashColor;

	if (draw)
	{
		kills = BG_GetKillSpree(&cg.snap->ps); 
		totalKills = cg.snap->ps.persistant[PERS_KILLS];
		bounty = BG_GetStatBounty(&cg.snap->ps); 	
		
		if ( cg_drawKillSpree.integer & 1 )	// current spree
			kills_string = va("Spree: %d ", kills);
		else 
			kills_string = "";	
		
		if ( cg_drawKillSpree.integer & 2 )	// total kills
			totalKills_string = va("Kills: %d ", totalKills);
		else 
			totalKills_string = "";	

		if ( cg_drawKillSpree.integer & 4 )	// bounty
			bounty_string = va("(%d)", bounty);
		else 
			bounty_string = "";	


		s = va( "%s%s%s", kills_string, totalKills_string,  bounty_string);

	//	s = va( "Kills: %d Total: %d (%d)", kills, totalKills,  bounty);	
		
		w = CG_Text_Width_Ext( s, 0.19f, 0, &cgs.media.limboFont1 );

		CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2,				HUD_Background );
		CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, HUD_Border );

		CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, kills < 0 ? colorRed : HUD_Text, s, 0, 0, 0, &cgs.media.limboFont1 );	

		// kills increased
		if (lastKills < kills)
		{			
			lastKillTime = flashTime = cg.time;
			lastKills = kills;
			Vector4Copy(colorGreen, flashColor);
		}
		else if (lastKills > kills)
		{
			lastKillTime = flashTime = cg.time;
			lastKills = kills;
			Vector4Copy(colorRed, flashColor);
		}

		if (hud_flashTime.integer > 0)
		{
			// if time still not ended
	//		if (cg.time <= flashTime + hud_flashTime.integer)
	//		{
	//		}
			
			flashScale =  cg.time - lastKillTime;

			if (flashScale <= hud_flashTime.integer)
			{
				flashScale = (flashScale) / (hud_flashTime.integer);
				flashScale = 1.0f - flashScale;
//				flashScale = hud_flashTime.integer - flashScale;

				//flashScale /= hud_flashTime.integer;

                _VectorScale(colorWhite, flashScale, flashColor);

				CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 2, flashColor );
			}			
		}
	}

	return y + 16;
}

/*
==================
CG_DrawFPS
==================
*/
#define	MAX_FPS_FRAMES	500

static float CG_DrawFPS( float y, qboolean draw  )
{
	char			*s;	
	static int		previousTimes[MAX_FPS_FRAMES];
	static int		previous;
	static int		index;	
	static int		oldSamples;
	int				frameTime, i, fps, t, total, w, samples;

	if (draw)
	{	// don't use serverTime, because that will be drifting to
		// correct for internet lag changes, timescales, timedemos, etc
		t			= trap_Milliseconds();
		frameTime	= t - previous;
		previous	= t;


		samples = cg_drawFPS.integer;

		if ( samples < 4 )
			samples = 4;
		if ( samples > MAX_FPS_FRAMES )
			samples = MAX_FPS_FRAMES;

		if ( samples != oldSamples )
			index = 0;

		oldSamples = samples;

		previousTimes[index % samples] = frameTime;
		
		index++;


		if ( index > samples )
		{
			// average multiple frames together to smooth changes out a bit
			total = 0;
			
			for ( i = 0 ; i < samples ; i++ ) 
			{
				total += previousTimes[i];
			}

			total = total ? total : 1;


			fps = 1000 * samples / total;

			s = va( "%i FPS", fps );
		} else {
			s = "estimating";
		}

		w = CG_Text_Width_Ext( s, 0.19f, 0, &cgs.media.limboFont1 );
		
		CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, HUD_Background );
		CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, HUD_Border );
		CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, HUD_Text, s, 0, 0, 0, &cgs.media.limboFont1 );			


	}

	return y + 12 + 4;
}

/*
=================
CG_DrawTimer
=================
*/

static float CG_DrawTimer( float y, qboolean draw ) {
	char		*s;
	int			w;
	int			mins, seconds, tens;
	int			msec;
	char		*rt;
	vec4_t		color =				{ 0.625f,	0.625f,	0.6f,	1.0f	};
//	vec4_t		timerBackground =	{ 0.16f,	0.2f,	0.17f,	0.8f	};
//	vec4_t		timerBorder     =	{ 0.5f,		0.5f,	0.5f,	0.5f	};

	if (draw)
	{

		rt = (cgs.gametype != GT_WOLF_LMS && (cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR || cg.snap->ps.pm_flags & PMF_FOLLOW) && cg_drawReinforcementTime.integer > 0) ?
								va("^F%d%s", CG_CalculateReinfTime( qfalse ), ((cgs.timelimit <= 0.0f) ? "" : " ")) : "";

		msec = ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime );

		seconds = msec / 1000;
		mins = seconds / 60;
		seconds -= mins * 60;
		tens = seconds / 10;
		seconds -= tens * 10;

		if(cgs.gamestate != GS_PLAYING) {
			//%	s = va( "%s^*WARMUP", rt );
			s = "^7WARMUP";	// ydnar: don't draw reinforcement time in warmup mode // ^*
			color[3] = fabs(sin(cg.time * 0.002));
		} else if ( msec < 0 && cgs.timelimit > 0.0f) {
			s = va( "^N0:00" );
			color[3] = fabs(sin(cg.time * 0.002));
		} else {
			if(cgs.timelimit <= 0.0f) {
				s = va( "%s", rt);
			} else {
				s = va( "%s^7%i:%i%i", rt, mins, tens, seconds); // ^*
			}

			color[3] = 1.f;
		}

		w = CG_Text_Width_Ext( s, 0.19f, 0, &cgs.media.limboFont1 );

		CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, HUD_Background );//timerBackground );
		CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, HUD_Border );//timerBorder );

		CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, color, s, 0, 0, 0, &cgs.media.limboFont1 );
	}

	return y + 12 + 4;
}

// forty - chruker's speed constants  
#define   SPEED_US_TO_KPH   15.58f  
#define   SPEED_US_TO_MPH   23.44f  
 
/*   
	cg_drawspeed:  
	1 draw ups only  
	2 draw ups (max: xx) only  
	3 draw speedometer only - missing  
	4 draw speedometer and ups (max: xx) - missing  
	5 draw speedometer and ups - missing   
*/  


static float CG_DrawSpeed( float y ) { 
	         char*s; 
	         int     w; 
	         static vec_t highestSpeed, speed; 
	         static int lasttime; 
	         int thistime; 
	         vec4_t tclr                     = { 0.625f, 0.625f, 0.6f, 1.0f }; 

			if(resetmaxspeed) {  
					highestSpeed = 0;  
					resetmaxspeed = qfalse;  
			}  

	         thistime = trap_Milliseconds(); 
	  
	         if(thistime > (lasttime + cg_speedinterval.integer)) { 
	                  
	                 speed = VectorLength(cg.predictedPlayerState.velocity); 
	                  
	                 if (speed > highestSpeed) { 
	                                 highestSpeed = speed; 
	                 } 
	                  
	                 lasttime = thistime; 
	          
	         }  
	  
			 switch (cg_drawspeed.integer) { 
	                 case 1: 
	                         switch(cg_speedunit.integer) { 
	                                 case 0:       
	                                         // Units per second 
	                                         s = va("%.1f UPS", speed); 
	                                         break; 
	                                 case 1: 
	                                         // Kilometers per hour 
	                                         s = va("%.1f KPH", (speed / SPEED_US_TO_KPH)); 
	                                         break; 
	                                 case 2: 
	                                         // Miles per hour 
	                                         s = va("%.1f MPH", (speed / SPEED_US_TO_MPH)); 
	                                         break; 
	                                 default: 
	                                         s = ""; 
	                                         break; 
	                         } 
	                         break; 
	  
	                 case 2: 
	                         switch(cg_speedunit.integer) { 
	                                 case 0:       
	                                         // Units per second 
	                                         s = va("%.1f UPS (%.1f MAX)", speed, highestSpeed); 
	                                         break; 
	                                 case 1: 
	                                         // Kilometers per hour 
	                                         s = va("%.1f KPH (%.1f MAX)", (speed / SPEED_US_TO_KPH), (highestSpeed / SPEED_US_TO_KPH)); 
	                                         break; 
	                                 case 2: 
	                                         // Miles per hour 
	                                         s = va("%.1f MPH (%.1f MAX)", (speed / SPEED_US_TO_MPH), (highestSpeed / SPEED_US_TO_MPH)); 
	                                         break; 
	                                 default: 
	                                         s = ""; 
	                                         break; 
	                         } 
	                         break; 
	  
	                 default: 
	                         s = ""; 
	                         break; 
	         } 
	  
	         w = CG_Text_Width_Ext( s, 0.19f, 0, &cgs.media.limboFont1 ); 
	  
	         CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, HUD_Background ); 
	         CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, HUD_Border ); 
	  
	         CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, tclr, s, 0, 0, 0, &cgs.media.limboFont1 ); 
	  
	         return y + 12 + 4; 
}  




float CG_DrawTime( float y, qboolean draw  )
{
	char	displayTime[12];
	int		w;
	qtime_t tm;
//	vec4_t	timerBackground = { 0.16f, 0.2f, 0.17f, 0.8f };
//	vec4_t	timerBorder     = { 0.5f, 0.5f,	0.5f, 0.5f };
//	vec4_t	tclr			= { 0.625f, 0.625f, 0.6f, 1.0f };

	if (draw)
	{
		trap_RealTime(&tm);
		displayTime[0] = '\0';

		if(cg_drawTime.integer == 1) 
		{
			Q_strcat(displayTime, sizeof(displayTime), 
				va("%d:%02d%s", tm.tm_hour, tm.tm_min,	cg_drawTimeSeconds.integer ? va(":%02d", tm.tm_sec) : ""));		
		} 
		else 
		{
			Q_strcat(displayTime, sizeof(displayTime), va("%d:%02d", 
				((tm.tm_hour == 0 || tm.tm_hour == 12) ? 12 : tm.tm_hour%12), tm.tm_min));

			if(cg_drawTimeSeconds.integer) 
			{
				Q_strcat(displayTime, sizeof(displayTime),	va(":%02d",tm.tm_sec));
			}

			Q_strcat(displayTime, sizeof(displayTime), (tm.tm_hour < 12) ? " am" : " pm");
		}

		w = CG_Text_Width_Ext(displayTime, 0.19f, 0, &cgs.media.limboFont1);

		CG_FillRect( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, HUD_Background);

		CG_DrawRect_FixedBorder( UPPERRIGHT_X - w - 2, y, w + 5, 12 + 2, 1, HUD_Border);

		CG_Text_Paint_Ext( UPPERRIGHT_X - w, y + 11, 0.19f, 0.19f, HUD_Text, displayTime, 0, 0, 0, &cgs.media.limboFont1);
	}

	return y + 12 + 4;
}

// START	xkan, 8/29/2002
int CG_BotIsSelected(int clientNum)
{
	int i;

	for (i=0; i<MAX_NUM_BUDDY; i++)
	{
		if (cg.selectedBotClientNumber[i] == 0)
			return 0;
		else if (cg.selectedBotClientNumber[i] == clientNum)
			return 1;
	}
	return 0;
}
// END		xkan, 8/29/2002

/*
=================
CG_DrawTeamOverlay
=================
*/

int maxCharsBeforeOverlay;

#define TEAM_OVERLAY_MAXNAME_WIDTH	16
#define TEAM_OVERLAY_MAXLOCATION_WIDTH	20

/*
=====================
CG_DrawUpperRight

=====================
*/
static void CG_DrawUpperRight( void ) {
	float	y;

/*	if( !cg_drawFireteamOverlay.integer )
	{
		return;
	}*/ // jaquboss - draw uper right without need for fireteam!!

	y = 20 + 100 + 32;

	// jet Pilot - BUGFIX Implementation of Chruker's fireteam overlay bugfix
	if (cg_drawFireteamOverlay.integer == 1)
	{
		if(CG_IsOnFireteam( cg.clientNum )) 
		{
			rectDef_t rect = { 10, 10, 100, 100 };
			CG_DrawFireTeamOverlay( &rect );
		}
	}	
	else
	{		
	//	CG_DrawTeamOverlay( 0 );
	}

	if( !( cg.snap->ps.pm_flags & PMF_LIMBO ) && ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) &&
		( cgs.autoMapExpanded || ( !cgs.autoMapExpanded && ( cg.time - cgs.autoMapExpandTime < 250.f ) ) ) )
	{
		return;
	}

	if ( cg_drawRoundTimer.integer ) {
		y = CG_DrawTimer( y, qtrue  );
	}

	if ( cg_drawFPS.integer ) {
		y = CG_DrawFPS( y, qtrue  );
	}

	if ( cg_drawPing.integer ) {
		y = CG_DrawPing( y, qtrue );
	}

	if ( cg_drawKillSpree.integer && cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
		y = CG_DrawKillSpree( y, qtrue  );
	}

//	if (cg_testVar.integer ) {
//		y = CG_DrawSmokeColor( y, qtrue  );
//	}

	if ( cg_drawTime.integer ) {
		y = CG_DrawTime( y, qtrue  );
	}
	// forty - speedometer  
	if ( cg_drawspeed.integer ) {  
		y = CG_DrawSpeed( y );  
	}   

	if ( cg_drawSnapshot.integer ) {
		y = CG_DrawSnapshot( y, qtrue  );
	}
}

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/

#define CHATLOC_X hud_drawAltHUD.integer ? 160 + 16 - 40 : 170 + 16 // jaquboss - flags
#define CHATLOC_Y 478
#define CHATLOC_TEXT_X (CHATLOC_X + 0.25f * TINYCHAR_WIDTH)

/*
=================
CG_DrawTeamInfo
=================
*/
static void CG_DrawTeamInfo( void ) {
	int w, h;
	int i, len;
	vec4_t		hcolor;
	int		chatHeight;
	float	alphapercent;
	float	lineHeight = 9.f;

	int chatWidth = 640 - CHATLOC_X - hud_drawAltHUD.integer ? 100 : 150;
 
	if( cg_teamChatHeight.integer < TEAMCHAT_HEIGHT ) {
		chatHeight = cg_teamChatHeight.integer;
	} else {
		chatHeight = TEAMCHAT_HEIGHT;
	}

	if( chatHeight <= 0 ) {
		return; // disabled
	}

	if( cgs.teamLastChatPos != cgs.teamChatPos ) {
		if( cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer ) {
			cgs.teamLastChatPos++;
		}

		h = (cgs.teamChatPos - cgs.teamLastChatPos) * lineHeight;

		w = 0;

		for( i = cgs.teamLastChatPos; i < cgs.teamChatPos; i++ ) {
			len = CG_Text_Width_Ext( cgs.teamChatMsgs[i % chatHeight], 0.2f, 0, &cgs.media.limboFont2 );
			if( len > w ) {
				w = len;
			}
		}
		w *= TINYCHAR_WIDTH;
		w += TINYCHAR_WIDTH * 2;

		for( i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i-- ) {
			alphapercent = 1.0f - (cg.time - cgs.teamChatMsgTimes[i % chatHeight]) / (float)(cg_teamChatTime.integer);
			if( alphapercent > 1.0f ) {
				alphapercent = 1.0f;
			} else if( alphapercent < 0.f ) {
				alphapercent = 0.f;
			}

			// jaquboss added chatter team instead
			//if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS ) {
			if ( cgs.teamChatMsgTeams[i % chatHeight] == TEAM_AXIS )
			{
				hcolor[0] = 1;
				hcolor[1] = 0;
				hcolor[2] = 0;
			}
			else if ( cgs.teamChatMsgTeams[i % chatHeight] == TEAM_ALLIES )
			{

				//} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ) {
				hcolor[0] = 0;
				hcolor[1] = 0;
				hcolor[2] = 1;
			}
			else
			{
				hcolor[0] = 0;
				hcolor[1] = 1;
				hcolor[2] = 0;
			}

			hcolor[3] = 0.33f * alphapercent;

			trap_R_SetColor( hcolor );
			CG_DrawPic( CHATLOC_X, CHATLOC_Y - (cgs.teamChatPos - i)*lineHeight, chatWidth, lineHeight, cgs.media.teamStatusBar );

			hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
			hcolor[3] = alphapercent;
			trap_R_SetColor( hcolor );

// jaquboss - chat icons
			if ( cgs.teamChatMsgTeams[i % chatHeight] == TEAM_AXIS )
			{
				CG_DrawPic( CHATLOC_TEXT_X - 16, CHATLOC_Y - (cgs.teamChatPos - i - 0.9f) * lineHeight - 8, 12, 10,
							cgs.media.axisFlag );
			}
			else if ( cgs.teamChatMsgTeams[i % chatHeight] == TEAM_ALLIES )
			{
				CG_DrawPic( CHATLOC_TEXT_X - 16, CHATLOC_Y - (cgs.teamChatPos - i - 0.9f) * lineHeight - 8, 12, 10,
							cgs.media.alliedFlag );
			}
// done
			CG_Text_Paint_Ext( CHATLOC_TEXT_X, CHATLOC_Y - (cgs.teamChatPos - i - 1) * lineHeight - 1, 0.2f, 0.2f, hcolor,  cgs.teamChatMsgs[i % chatHeight], 0, 0, 0, &cgs.media.limboFont2 );
		}
	}
}

const char* CG_PickupItemText( int item )
{
	if( bg_itemlist[ item ].giType == IT_HEALTH && bg_itemlist[ item ].quantity > 0)  // jaquboss - so health packs won't tell 0
	{
		if(bg_itemlist[ item ].world_model[2])	
		{	// this is a multi-stage item
			// FIXME: print the correct amount for multi-stage
			return va( "a %s", bg_itemlist[ item ].pickup_name );
		} 
		else
		{
			if ( cg.snap && cg.snap->ps.stats[STAT_TEAMAURA] & AURABIT_MEDIC )
				return va( "%i %s", bg_itemlist[ item ].quantity+10, bg_itemlist[ item ].pickup_name );
			else 
				return va( "%i %s", bg_itemlist[ item ].quantity, bg_itemlist[ item ].pickup_name );
		}
	} 
	else if( bg_itemlist[ item ].giType == IT_TEAM )
	{
		return "an Objective";
	} 
	else
	{
		if( bg_itemlist[ item ].pickup_name[0] == 'a' ||  bg_itemlist[ item ].pickup_name[0] == 'A' )
		{
			return va( "an %s", bg_itemlist[ item ].pickup_name );
		} 
		else
		{
			return va( "a %s", bg_itemlist[ item ].pickup_name );
		}
	}
}

/*
=================
CG_DrawNotify
=================
*/
#define NOTIFYLOC_Y 42 // bottom end
#define NOTIFYLOC_X 0
#define NOTIFYLOC_Y_SP 128

static void CG_DrawNotify( void ) {
	int w, h;
	int i, len;
	vec4_t		hcolor;
	int		chatHeight;
	float	alphapercent;
	char	var[MAX_TOKEN_CHARS];
	float	notifytime = 1.0f;
	int		yLoc;

	return;

	yLoc = NOTIFYLOC_Y;

	trap_Cvar_VariableStringBuffer( "con_notifytime", var, sizeof( var ) );
	notifytime = atof( var ) * 1000;

	if ( notifytime <= 100.f )
		notifytime = 100.0f;

	chatHeight = NOTIFY_HEIGHT;

	if (cgs.notifyLastPos != cgs.notifyPos) {
		if (cg.time - cgs.notifyMsgTimes[cgs.notifyLastPos % chatHeight] > notifytime) {
			cgs.notifyLastPos++;
		}

		h = (cgs.notifyPos - cgs.notifyLastPos) * TINYCHAR_HEIGHT;

		w = 0;

		for (i = cgs.notifyLastPos; i < cgs.notifyPos; i++) {
			len = CG_DrawStrlen(cgs.notifyMsgs[i % chatHeight]);
			if (len > w)
				w = len;
		}
		w *= TINYCHAR_WIDTH;
		w += TINYCHAR_WIDTH * 2;

		if ( maxCharsBeforeOverlay <= 0 )
			maxCharsBeforeOverlay = 80;

		for (i = cgs.notifyPos - 1; i >= cgs.notifyLastPos; i--) {
			alphapercent = 1.0f - ((cg.time - cgs.notifyMsgTimes[i % chatHeight]) / notifytime);
			if (alphapercent > 0.5f)
				alphapercent = 1.0f;
			else 
				alphapercent *= 2;
			
			if (alphapercent < 0.f)
				alphapercent = 0.f;

			hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
			hcolor[3] = alphapercent;
			trap_R_SetColor( hcolor );

			CG_DrawStringExt( NOTIFYLOC_X + TINYCHAR_WIDTH, 
				yLoc - (cgs.notifyPos - i)*TINYCHAR_HEIGHT, 
				cgs.notifyMsgs[i % chatHeight], hcolor, qfalse, qfalse,
				TINYCHAR_WIDTH, TINYCHAR_HEIGHT, maxCharsBeforeOverlay );
		}
	}
}

/*
===============================================================================

LAGOMETER

===============================================================================
*/

#define	LAG_SAMPLES		128


typedef struct {
	int		frameSamples[LAG_SAMPLES];
	int		frameCount;
	int		snapshotFlags[LAG_SAMPLES];
	int		snapshotSamples[LAG_SAMPLES];
	int		snapshotCount;
} lagometer_t;

lagometer_t		lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
*/
void CG_AddLagometerFrameInfo( void ) {
	int			offset;

	offset = cg.time - cg.latestSnapshotTime;
	lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1) ] = offset;
	lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
*/
void CG_AddLagometerSnapshotInfo( snapshot_t *snap ) {
	// dropped packet
	if ( !snap ) {
		lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = -1;
		lagometer.snapshotCount++;
		return;
	}

	if (cg.demoPlayback) {
		static int lasttime = 0;
		// rain - #67 - display snapshot time delta instead of ping when
		// playing a demo, since I can't think of any way to get the
		// real ping
		lagometer.snapshotSamples[lagometer.snapshotCount & (LAG_SAMPLES - 1)] = snap->serverTime - lasttime;
		lasttime = snap->serverTime;
	} else
		lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->ping; 

	lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1) ] = snap->snapFlags;
	lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
static void CG_DrawDisconnect( void ) {
	float		x, y;
	int			cmdNum;
	usercmd_t	cmd;
	const char		*s;
	int			w;  // bk010215 - FIXME char message[1024];

	// OSP - dont draw if a demo and we're running at a different timescale
	if(cg.demoPlayback && cg_timescale.value != 1.0f) return;
	
	// ydnar: don't draw if the server is respawning
	if( cg.serverRespawning )
		return;

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
	trap_GetUserCmd( cmdNum, &cmd );
	if ( cmd.serverTime <= cg.snap->ps.commandTime
		|| cmd.serverTime > cg.time ) {	// special check for map_restart // bk 0102165 - FIXME
		return;
	}

	// also add text in center of screen
	s = CG_TranslateString( "Connection Interrupted" ); // bk 010215 - FIXME
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w/2, 100, s, 1.0F);

	// blink the icon
	if ( ( cg.time >> 9 ) & 1 ) {
		return;
	}

	x = 640 - 48;
	y = 480 - 200;

	CG_DrawPic( x, y, 48, 48, cgs.media.disconnectIcon );
}


#define	MAX_LAGOMETER_PING	900
#define	MAX_LAGOMETER_RANGE	300

/*
==============
CG_DrawLagometer
==============
*/
static void CG_DrawLagometer( void ) {
	int		a, x, y, i;
	float	v;
	float	ax, ay, aw, ah, mid, range;
	int		color;
	float	vscale;

	if ( !cg_lagometer.integer /*|| cgs.localServer*/ ) { // jaquboss, lagometer on local servers
//	if(0) {
		CG_DrawDisconnect();
		return;
	}

	//
	// draw the graph
	//
	x = 640 - 48;
	y = 480 - 200;

	trap_R_SetColor( NULL );
//	CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

	// jaquboss, use this instead
	CG_FillRect( x, y, 48, 48, HUD_Background );
	CG_DrawRect_FixedBorder( x, y, 48, 48, 1, HUD_Border );


	ax = x;
	ay = y;
	aw = 48;
	ah = 48;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	color = -1;
	range = ah / 3;
	mid = ay + range;

	vscale = range / MAX_LAGOMETER_RANGE;

	// draw the frame interpoalte / extrapolate graph
	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.frameCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.frameSamples[i];
		v *= vscale;
		if ( v > 0 ) {
			if ( color != 1 ) {
				color = 1;
				trap_R_SetColor( colorYellow );
			}
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic ( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 2 ) {
				color = 2;
				trap_R_SetColor( colorBlue );
			}
			v = -v;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	// draw the snapshot latency / drop graph
	range = ah / 2;
	vscale = range / MAX_LAGOMETER_PING;

	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.snapshotCount - 1 - a ) & (LAG_SAMPLES - 1);
		v = lagometer.snapshotSamples[i];
		if ( v > 0 ) {
			if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED ) {
				if ( color != 5 ) {
					color = 5;	// YELLOW for rate delay
					trap_R_SetColor( colorYellow );
				}
			} else {
				if ( color != 3 ) {
					color = 3;
					trap_R_SetColor( colorGreen );
				}
			}
			v = v * vscale;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 4 ) {
				color = 4;		// RED for dropped snapshots
				trap_R_SetColor( colorRed );
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	trap_R_SetColor( NULL );

	if ( cg_nopredict.integer 
#ifdef ALLOW_GSYNC
		|| cg_synchronousClients.integer 
#endif // ALLOW_GSYNC
		) {
		CG_DrawBigString( ax, ay, "snc", 1.0 );
	}

	CG_DrawDisconnect();
}


void CG_DrawLivesLeft( void ) {
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		return;
	}

	if( cg.snap->ps.persistant[PERS_RESPAWNS_LEFT] < 0 ) {
		return;
	}

	CG_DrawPic( 4, 360, 48, 24, cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ? cgs.media.hudAlliedHelmet : cgs.media.hudAxisHelmet );

	CG_DrawField( 44, 360, 3, cg.snap->ps.persistant[PERS_RESPAWNS_LEFT], 14, 20, qtrue, qtrue );
}

/*
===============================================================================

CENTER PRINTING

===============================================================================
*/


/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
#define CP_LINEWIDTH 56			// NERVE - SMF

void CG_CenterPrint( const char *str, int y, int charWidth ) {
	char	*s;
	int		i, len;						// NERVE - SMF
	qboolean neednewline = qfalse;		// NERVE - SMF
	int priority = 0;

	// jaquboss - Announcer - we've got an announce
	// change: keep centerprint..
/*	if ( cg.centerPrintAnnouncerTime > cg.time )
		return;*/

	// NERVE - SMF - don't draw if this print message is less important
	if ( cg.centerPrintTime && priority < cg.centerPrintPriority )
		return;

	Q_strncpyz( cg.centerPrint, str, sizeof(cg.centerPrint) );
	cg.centerPrintPriority = priority;	// NERVE - SMF

	// NERVE - SMF - turn spaces into newlines, if we've run over the linewidth
	len = strlen( cg.centerPrint );
	for ( i = 0; i < len; i++ ) {

		// NOTE: subtract a few chars here so long words still get displayed properly
		if ( i % ( CP_LINEWIDTH - 20 ) == 0 && i > 0 )
			neednewline = qtrue;
		if ( cg.centerPrint[i] == ' ' && neednewline ) {
			cg.centerPrint[i] = '\n';
			neednewline = qfalse;
		}
	}
	// -NERVE - SMF

	cg.centerPrintTime = cg.time;
	cg.centerPrintY = y;
	cg.centerPrintCharWidth = charWidth;

	// count the number of lines for centering
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while( *s ) {
		if (*s == '\n')
			cg.centerPrintLines++;
		s++;
	}
}

void CG_PrintAnnouncement( const char *str, int y, int charWidth, int priority)
{
	char		*s;
	int			i;
	int			len;
	static int	lastPriority = 0;
	qboolean	newLine		 = qfalse;

	// don't print if the last message has higher priority than this one
	if (cg.announcementPrintTime && priority < lastPriority)
	{
		return;
	}

	Q_strncpyz(cg.announcement, str, sizeof(cg.announcement));
	cg.announcementPriority = priority;
    
	len = strlen(cg.announcement);

	// split long strings into multiple lines
    for (i = 0; i < len; i++)
	{		
		if ( i % ( CP_LINEWIDTH - 20 ) == 0 && i > 0 )
		{
			newLine = qtrue;
		}

		if ( cg.centerPrint[i] == ' ' && newLine )
		{
			cg.centerPrint[i] = '\n';
			newLine = qfalse;
		}
	}

	cg.announcementPrintTime = cg.time + 2000;
	cg.announcementPrintY	 = y;
	cg.announcementCharWidth = charWidth;

	// count the number of lines for centering
	cg.announcementPrintLines = 1;
	s = cg.announcement;

	while( *s ) 
	{
		if (*s == '\n')
		{
			cg.centerPrintLines++;
		}
		s++;
	}
}

// NERVE - SMF
/*
==============
CG_PriorityCenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void CG_PriorityCenterPrint( const char *str, int y, int charWidth, int priority ) {
	char	*s;
	int		i, len;						// NERVE - SMF
	qboolean neednewline = qfalse;		// NERVE - SMF

	// NERVE - SMF - don't draw if this print message is less important
	if( cg.centerPrintTime && priority < cg.centerPrintPriority )
		return;

	Q_strncpyz( cg.centerPrint, str, sizeof(cg.centerPrint) );
	cg.centerPrintPriority = priority;	// NERVE - SMF

	// NERVE - SMF - turn spaces into newlines, if we've run over the linewidth
	len = strlen( cg.centerPrint );
	for ( i = 0; i < len; i++ ) {

		// NOTE: subtract a few chars here so long words still get displayed properly
		if ( i % ( CP_LINEWIDTH - 20 ) == 0 && i > 0 )
			neednewline = qtrue;
		if ( cg.centerPrint[i] == ' ' && neednewline ) {
			cg.centerPrint[i] = '\n';
			neednewline = qfalse;
		}
	}
	// -NERVE - SMF

	cg.centerPrintTime = cg.time + 2000;
	cg.centerPrintY = y;
	cg.centerPrintCharWidth = charWidth;

	// count the number of lines for centering
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while( *s ) {
		if (*s == '\n')
			cg.centerPrintLines++;
		s++;
	}
}
// -NERVE - SMF

/*
===================
CG_DrawAnnouncement - jet Pilot
===================
*/
static void CG_DrawAnnouncement( void )
{
	char	*start;
	int		l;
	int		x, y, w;
	float	*color;

	if ( !cg.announcementPrintTime )
	{
		return;
	}

	color = CG_FadeColor( cg.announcementPrintTime, 1000 * cg_centertime.value );

	if ( !color ) 
	{
		cg.centerPrintTime		= 0;
		cg.centerPrintPriority  = 0;
		return;
	}

	trap_R_SetColor( color );

	start = cg.announcement;

	y = cg.announcementPrintY - cg.announcementPrintLines * BIGCHAR_HEIGHT / 2;

	while ( 1 ) 
	{
		char linebuffer[1024];

		for ( l = 0; l < CP_LINEWIDTH; l++ )	// NERVE - SMF - added CP_LINEWIDTH
		{
			if ( !start[l] || start[l] == '\n' ) 
			{
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.announcementCharWidth * CG_DrawStrlen( linebuffer );

		x = ( SCREEN_WIDTH - w ) / 2;

		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue, cg.centerPrintCharWidth, (int)(cg.centerPrintCharWidth * 1.5), 0 );

		y += cg.announcementCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}

/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString( void ) {
	char	*start;
	int		l;
	int		x, y, w;
	float	*color;

	if ( !cg.centerPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
	if ( !color ) {
		cg.centerPrintTime = 0;
		cg.centerPrintPriority = 0;
		return;
	}

	trap_R_SetColor( color );

	start = cg.centerPrint;

	y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < CP_LINEWIDTH; l++ ) {			// NERVE - SMF - added CP_LINEWIDTH
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.centerPrintCharWidth * CG_DrawStrlen( linebuffer );

		x = ( SCREEN_WIDTH - w ) / 2;

		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue, cg.centerPrintCharWidth, (int)(cg.centerPrintCharWidth * 1.5), 0 );

		y += cg.centerPrintCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}



/*
================================================================================

CROSSHAIRS

================================================================================
*/

/*
==============
CG_DrawWeapReticle
==============
*/
static void CG_DrawWeapReticle(void) {
	vec4_t	color = {0, 0, 0, 1};
	qboolean fg, garand, k43;

	// DHM - Nerve :: So that we will draw reticle
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback ) {
		garand = (qboolean)(cg.snap->ps.weapon == WP_GARAND_SCOPE);
		k43 = (qboolean)(cg.snap->ps.weapon == WP_K43_SCOPE);
		fg = (qboolean)(cg.snap->ps.weapon == WP_FG42_SCOPE);
	} else {
		fg = (qboolean)(cg.weaponSelect == WP_FG42_SCOPE);
		garand = (qboolean)(cg.weaponSelect == WP_GARAND_SCOPE);
		k43 = (qboolean)(cg.weaponSelect == WP_K43_SCOPE);
	}

	if(fg) {
		// sides
		CG_FillRect (0, 0, 80, 480, color);
		CG_FillRect (560, 0, 80, 480, color);

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 0, 480, 480, cgs.media.reticleShaderSimple );

/*		if(cgs.media.reticleShaderSimpleQ) {
			trap_R_DrawStretchPic( x,	0, w, h, 0, 0, 1, 1, cgs.media.reticleShaderSimpleQ );	// tl
			trap_R_DrawStretchPic( x+w, 0, w, h, 1, 0, 0, 1, cgs.media.reticleShaderSimpleQ );	// tr
			trap_R_DrawStretchPic( x,	h, w, h, 0, 1, 1, 0, cgs.media.reticleShaderSimpleQ );	// bl
			trap_R_DrawStretchPic( x+w, h, w, h, 1, 1, 0, 0, cgs.media.reticleShaderSimpleQ );	// br
		}*/

		// hairs
		CG_FillRect (84, 239, 150, 3, color);	// left
		CG_FillRect (234, 240, 173, 1, color);	// horiz center
		CG_FillRect (407, 239, 150, 3, color);	// right


		CG_FillRect (319, 2,   3, 151, color);	// top center top
		CG_FillRect (320, 153, 1, 114, color);	// top center bot

		CG_FillRect (320, 241, 1, 87, color);	// bot center top
		CG_FillRect (319, 327, 3, 151, color);	// bot center bot
	} else if(garand) {
		// sides
		CG_FillRect (0, 0, 80, 480, color);
		CG_FillRect (560, 0, 80, 480, color);

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 0, 480, 480, cgs.media.reticleShaderSimple );

		// hairs
		CG_FillRect (84, 239, 177, 2, color);	// left
		CG_FillRect (320, 242, 1, 58, color);	// center top
		CG_FillRect (319, 300, 2, 178, color);	// center bot
		CG_FillRect (380, 239, 177, 2, color);	// right
	} else if (k43) {
		// sides
		CG_FillRect (0, 0, 80, 480, color);
		CG_FillRect (560, 0, 80, 480, color);

		// center
		if(cgs.media.reticleShaderSimple)
			CG_DrawPic( 80, 0, 480, 480, cgs.media.reticleShaderSimple );

		// hairs
		CG_FillRect (84, 239, 177, 2, color);	// left
		CG_FillRect (320, 242, 1, 58, color);	// center top
		CG_FillRect (319, 300, 2, 178, color);	// center bot
		CG_FillRect (380, 239, 177, 2, color);	// right
	}
}

/*
==============
CG_DrawMortarReticle
==============
*/
static void CG_DrawMortarReticle( void ) {
	vec4_t	color = { 1.f, 1.f, 1.f, .5f };
	vec4_t	color_back = { 0.f, 0.f, 0.f, .25f };
	vec4_t	color_extends = { .77f, .73f, .1f, 1.f };
	vec4_t	color_lastfire = { .77f, .1f, .1f, 1.f };
	//vec4_t	color_firerequest = { .23f, 1.f, .23f, 1.f };
	vec4_t	color_firerequest = { 1.f, 1.f, 1.f, 1.f };
	float	offset, localOffset;
	int		i, min, majorOffset, val, printval, fadeTime;
	char	*s;
	float	angle, angleMin, angleMax;
	qboolean hasRightTarget, hasLeftTarget;

	// Background
	CG_FillRect( 136, 236, 154, 38, color_back );
	CG_FillRect( 290, 160, 60, 208, color_back );
	CG_FillRect( 350, 236, 154, 38, color_back );

	// Horizontal bar

	// bottom
	CG_FillRect( 140, 264, 150, 1, color);	// left
	CG_FillRect( 350, 264, 150, 1, color);	// right

	// 10 units - 5 degrees
	// total of 360 units
	// nothing displayed between 150 and 210 units
	// 360 / 10 = 36 bits, means 36 * 5 = 180 degrees
	// that means left is cg.predictedPlayerState.viewangles[YAW] - .5f * 180
	angle = 360 - AngleNormalize360(cg.predictedPlayerState.viewangles[YAW] - 90.f);

	offset = (5.f / 65536) * ((int)(angle * (65536 / 5.f)) & 65535);
	min = (int)(AngleNormalize360(angle - .5f * 180) / 15.f) * 15;
	majorOffset = (int)(floor((int)floor(AngleNormalize360(angle - .5f * 180)) % 15) / 5.f );

	for( val = i = 0; i < 36; i++ ) {
		localOffset = i * 10.f + (offset * 2.f);

		if( localOffset >= 150 && localOffset <= 210 ) {
			if( i % 3 == majorOffset)
				val++;
			continue;
		}

		if( i % 3 == majorOffset) {
			printval = min - val * 15 + 180;
			
			// rain - old tertiary abuse was nasty and had undefined result
			if (printval < 0)
				printval += 360;
			else if (printval >= 360)
				printval -= 360;

			s = va( "%i", printval );
			//CG_Text_Paint_Ext( 140 + localOffset - .5f * CG_Text_Width_Ext( s, .15f, 0, &cgs.media.limboFont1 ), 244, .15f, .15f, color, s, 0, 0, 0, &cgs.media.limboFont1 );
			//CG_FillRect( 140 + localOffset, 248, 1, 16, color);
			CG_Text_Paint_Ext( 500 - localOffset - .5f * CG_Text_Width_Ext( s, .15f, 0, &cgs.media.limboFont1 ), 244, .15f, .15f, color, s, 0, 0, 0, &cgs.media.limboFont1 );
			CG_FillRect( 500 - localOffset, 248, 1, 16, color);
			val++;
		} else {
			//CG_FillRect( 140 + localOffset, 256, 1, 8, color);
			CG_FillRect( 500 - localOffset, 256, 1, 8, color);			
		}
	}

	// the extremes
	// 30 degrees plus a 15 degree border
	angleMin = AngleNormalize360(360 - (cg.pmext.mountedWeaponAngles[YAW] - 90.f) - (30.f + 15.f));
	angleMax = AngleNormalize360(360 - (cg.pmext.mountedWeaponAngles[YAW] - 90.f) + (30.f + 15.f));

	// right
	localOffset = (AngleNormalize360(angle - angleMin) / 5.f ) * 10.f;
	//CG_FillRect( 320 + localOffset, 252, 2, 18, color_extends);
	CG_FillRect( 320 - localOffset, 252, 2, 18, color_extends);

	// left
	localOffset = (AngleNormalize360(angleMax - angle) / 5.f ) * 10.f;
	//CG_FillRect( 320 - localOffset, 252, 2, 18, color_extends);
	CG_FillRect( 320 + localOffset, 252, 2, 18, color_extends);

	// last fire pos
	fadeTime = 0;
	if( (cg.lastFiredWeapon == WP_MORTAR_SET || cg.lastFiredWeapon == WP_MORTAR2_SET) && cg.mortarImpactTime >= -1 ) {
		fadeTime = cg.time - (cg.predictedPlayerEntity.muzzleFlashTime + 5000);

		if( fadeTime < 3000 ) {
			float lastfireAngle;

			if( fadeTime > 0 ) {
				color_lastfire[3] = 1.f - (fadeTime/3000.f);
			}

			lastfireAngle = AngleNormalize360(360 - (cg.mortarFireAngles[YAW] - 90.f));

			localOffset = ( ( AngleSubtract( angle, lastfireAngle ) ) / 5.f ) * 10.f;
			//CG_FillRect( 320 + localOffset, 252, 2, 18, color_lastfire);
			CG_FillRect( 320 - localOffset, 252, 2, 18, color_lastfire);
		}
	}

	// mortar attack requests
	hasRightTarget = hasLeftTarget = qfalse;
	for( i = 0; i < MAX_CLIENTS; i++ ) {
		int requestFadeTime = cg.time - (cg.artilleryRequestTime[i] + 25000);

		if( requestFadeTime < 5000 ) {
			vec3_t dir;
			float yaw;
			float attackRequestAngle;

			VectorSubtract( cg.artilleryRequestPos[i], cg.predictedPlayerEntity.lerpOrigin, dir );

			// ripped this out of vectoangles
			if( dir[1] == 0 && dir[0] == 0 ) {
				yaw = 0;
			} else {
				if( dir[0] ) {
					yaw = ( atan2 ( dir[1], dir[0] ) * 180 / M_PI );
				}
				else if ( dir[1] > 0 ) {
					yaw = 90;
				}
				else {
					yaw = 270;
				}
				if ( yaw < 0 ) {
					yaw += 360;
				}
			}

			if( requestFadeTime > 0 ) {
				color_firerequest[3] = 1.f - (requestFadeTime/5000.f);
			}

			attackRequestAngle = AngleNormalize360(360 - (yaw - 90.f));

			yaw = AngleSubtract( attackRequestAngle, angleMin );

			if( yaw < 0 ) {
				if( !hasLeftTarget ) {
					//CG_FillRect( 136 + 2, 236 + 38 - 6, 4, 4, color_firerequest );

					trap_R_SetColor( color_firerequest );
					CG_DrawPic( 136 + 2, 236 + 38 - 10 + 1, 8, 8, cgs.media.ccMortarTargetArrow );
					trap_R_SetColor( NULL );

					hasLeftTarget = qtrue;
				}
			} else if( yaw > 90 ) {
				if( !hasRightTarget ) {
					//CG_FillRect( 350 + 154 - 6, 236 + 38 - 6, 4, 4, color_firerequest );

					trap_R_SetColor( color_firerequest );
					CG_DrawPic( 350 + 154 - 10, 236 + 38 - 10 + 1, -8, 8, cgs.media.ccMortarTargetArrow );
					trap_R_SetColor( NULL );

					hasRightTarget = qtrue;
				}
			} else {
				localOffset = ( ( AngleSubtract( angle, attackRequestAngle ) ) / 5.f ) * 10.f;
				//CG_FillRect( 320 + localOffset - 3, 264 - 3, 6, 6, color_firerequest );

				trap_R_SetColor( color_firerequest );
 				//CG_DrawPic( 320 + localOffset - 8, 264 - 8, 16, 16, cgs.media.ccMortarTarget );
				CG_DrawPic( 320 - localOffset - 8, 264 - 8, 16, 16, cgs.media.ccMortarTarget );
				trap_R_SetColor( NULL );
			}
		}
	}

 	/*s = va( "%.2f (%i / %i)",AngleNormalize360(angle - .5f * 180), majorOffset, min );
	CG_Text_Paint( 140, 224, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f",AngleNormalize360(angle) );
	CG_Text_Paint( 320 - .5f * CG_Text_Width( s, .25f, 0), 224, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f", AngleNormalize360(angle + .5f * 180) );
	CG_Text_Paint( 500 - CG_Text_Width( s, .25f, 0 ), 224, .25f, color, s, 0, 0, 0 );*/

	// Vertical bar

	// sides
	CG_FillRect( 295, 164, 1, 200, color);	// left
	CG_FillRect( 345, 164, 1, 200, color);	// right

	// 10 units - 2.5 degrees
	// total of 200 units
	// 200 / 10 = 20 bits, means 20 * 2.5 = 50 degrees
	// that means left is cg.predictedPlayerState.viewangles[PITCH] - .5f * 50
	angle = AngleNormalize180(360 - (cg.predictedPlayerState.viewangles[PITCH] - 60));

	offset = (2.5f / 65536) * ((int)(angle * (65536 / 2.5f)) & 65535);
	min = floor((angle + .5f * 50) / 10.f) * 10;
	majorOffset = (int)(floor((int)((angle + .5f * 50) * 10.f) % 100) / 25.f );

	for( val = i = 0; i < 20; i++ ) {
		localOffset = i * 10.f + (offset * 4.f);

		/*if( localOffset >= 150 && localOffset <= 210 ) {
			if( i % 3 == majorOffset)
				val++;
			continue;
		}*/

		if( i % 4 == majorOffset ) {
			printval = min - val * 10;
			
			// rain - old tertiary abuse was nasty and had undefined result
			if (printval <= -180)
				printval += 360;
			else if (printval >= 180)
				printval -= 180;

			s = va( "%i", printval );
			CG_Text_Paint_Ext( 320 - .5f * CG_Text_Width_Ext( s, .15f, 0, &cgs.media.limboFont1 ), 164 + localOffset + .5f * CG_Text_Height_Ext( s, .15f, 0, &cgs.media.limboFont1 ), .15f, .15f, color, s, 0, 0, 0, &cgs.media.limboFont1 );
			CG_FillRect( 295 + 1, 164 + localOffset, 12, 1, color);
			CG_FillRect( 345 - 12, 164 + localOffset, 12, 1, color);
			val++;
		} else {
			CG_FillRect( 295 + 1, 164 + localOffset, 8, 1, color);
			CG_FillRect( 345 - 8, 164 + localOffset, 8, 1, color);
		}
	}

	// the extremes
	// 30 degrees up
	// 20 degrees down
	angleMin = AngleNormalize180(360 - (cg.pmext.mountedWeaponAngles[PITCH] - 60)) - 20.f;
	angleMax = AngleNormalize180(360 - (cg.pmext.mountedWeaponAngles[PITCH] - 60)) + 30.f;

	// top
	localOffset = angleMax - angle;
	if( localOffset < 0 )
		localOffset = 0;
	localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
	if( localOffset < 100 ) {
		CG_FillRect( 295 - 2, 264 - localOffset, 6, 2, color_extends);
		CG_FillRect( 345 - 4 + 1, 264 - localOffset, 6, 2, color_extends);
	}

	// bottom
	localOffset = angle - angleMin;
	if( localOffset < 0 )
		localOffset = 0;
	localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
	if( localOffset < 100 ) {
		CG_FillRect( 295 - 2, 264 + localOffset, 6, 2, color_extends);
		CG_FillRect( 345 - 4 + 1, 264 + localOffset, 6, 2, color_extends);
	}

	// last fire pos
	if( (cg.lastFiredWeapon == WP_MORTAR_SET || cg.lastFiredWeapon == WP_MORTAR2_SET) && cg.mortarImpactTime >= -1 ) {
		if( fadeTime < 3000 ) {
			float lastfireAngle;

			lastfireAngle = AngleNormalize180(360 - (cg.mortarFireAngles[PITCH] - 60));

			if( lastfireAngle > angle ) {
				localOffset = lastfireAngle - angle;
				if( localOffset < 0 )
					localOffset = 0;
				localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
				if( localOffset < 100 ) {
					CG_FillRect( 295 - 2, 264 - localOffset, 6, 2, color_lastfire);
					CG_FillRect( 345 - 4 + 1, 264 - localOffset, 6, 2, color_lastfire);
				}
			} else {
				localOffset = angle - lastfireAngle;
				if( localOffset < 0 )
					localOffset = 0;
				localOffset = (AngleNormalize360(localOffset) / 2.5f ) * 10.f;
				if( localOffset < 100 ) {
					CG_FillRect( 295 - 2, 264 + localOffset, 6, 2, color_lastfire);
					CG_FillRect( 345 - 4 + 1, 264 + localOffset, 6, 2, color_lastfire);
				}
			}
		}
	}
 
	/*s = va( "%.2f (%i / %i)", angle + .5f * 50, majorOffset, min );
	CG_Text_Paint( 348, 164, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f",angle );
	CG_Text_Paint( 348, 264, .25f, color, s, 0, 0, 0 );
	s = va( "%.2f", angle - .5f * 50 );
	CG_Text_Paint( 348, 364, .25f, color, s, 0, 0, 0 );*/
}

/*
==============
CG_DrawBinocReticle
==============
*/
static void CG_DrawBinocReticle(void) {
	// an alternative.  This gives nice sharp lines at the expense of a few extra polys
	vec4_t	color;
	color[0] = color[1] = color[2] = 0;
	color[3] = 1;

	if(cgs.media.binocShaderSimple)
		CG_DrawPic( 0, 0, 640, 480, cgs.media.binocShaderSimple );

	CG_FillRect (146, 239, 348, 1, color);

	CG_FillRect (188, 234, 1, 13, color);	// ll
	CG_FillRect (234, 226, 1, 29, color);	// l
	CG_FillRect (274, 234, 1, 13, color);	// lr
	CG_FillRect (320, 213, 1, 55, color);	// center
	CG_FillRect (360, 234, 1, 13, color);	// rl
	CG_FillRect (406, 226, 1, 29, color);	// r
	CG_FillRect (452, 234, 1, 13, color);	// rr
}

void CG_FinishWeaponChange(int lastweap, int newweap); // JPW NERVE
void CG_DrawCompassIcon( float x, float y, float w, float h, vec3_t origin, vec3_t dest, qhandle_t shader, float dstScale, int baseSize );

static void CG_DrawDanger( void ){
	snapshot_t	*snap;
	int	i;
	vec4_t col={1.f,1.f,1.f,0.5f };

	if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
		snap = cg.nextSnap;
	} else {
		snap = cg.snap;
	}

	if ( !(cgs.clientinfo[cg.snap->ps.clientNum].skillBits[SK_BATTLE_SENSE] & (1<<8)) )
		return;

	for ( i = 0; i < snap->numEntities; i++ ) {
		centity_t *cent = &cg_entities[ snap->entities[ i ].number ];
		int	weap;
		vec3_t v1;
		float len, maxDist;

		if( cent->currentState.eType != ET_MISSILE )
			continue;

		weap = cent->currentState.weapon;

		if(!BG_WeaponInWolfMP(weap))
			continue;

		switch( weap ){
		default:
			continue;
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
			maxDist = 320.f;
			break;
		}

		VectorCopy( cent->lerpOrigin, v1 );
		VectorSubtract( cg.predictedPlayerState.origin, v1, v1 );
		len = VectorLength( v1 );

		if ( len > maxDist )
			continue;

		col[3] = col[0] = 1 - (len / maxDist);
		col[2] = col[1] = len / maxDist;

		trap_R_SetColor( col );

		CG_DrawCompassIcon( 320-70, 240-70, 140, 140, cg.predictedPlayerState.origin, cent->lerpOrigin, cgs.media.nadeDanger[weap == WP_GRENADE_LAUNCHER? 1 : 0], 0.75f, 32);

	}

	trap_R_SetColor( NULL );


}

/*
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair(void) 
{
	float		w, h;
	qhandle_t	hShader;
	float		f;
	float		x, y;
	int			weapnum;		// DHM - Nerve

	if ( cg.renderingThirdPerson )
		return;

	// using binoculars
	if(cg.zoomedBinoc) {
		CG_DrawBinocReticle();
		return;
	}

	// DHM - Nerve :: show reticle in limbo and spectator
	if ( (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg.demoPlayback )
		weapnum = cg.snap->ps.weapon;
	else
		weapnum = cg.weaponSelect;


	switch(weapnum) {

		// weapons that get no reticle
		case WP_NONE:	// no weapon, no crosshair
			if(cg.zoomedBinoc)
				CG_DrawBinocReticle();

			if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR )
				return;
			break;

		// special reticle for weapon
		case WP_FG42_SCOPE:
		case WP_GARAND_SCOPE:
		case WP_K43_SCOPE:
			if(!BG_PlayerMounted(cg.snap->ps.eFlags)) {
				// JPW NERVE -- don't let players run with rifles -- speed 80 == crouch, 128 == walk, 256 == run
					if (VectorLengthSquared(cg.snap->ps.velocity) > SQR(127)) {
						if( cg.snap->ps.weapon == WP_FG42_SCOPE ) {
							CG_FinishWeaponChange( WP_FG42_SCOPE, WP_FG42 );
						}
						if( cg.snap->ps.weapon == WP_GARAND_SCOPE ) {
							CG_FinishWeaponChange( WP_GARAND_SCOPE, WP_GARAND );
						}
						if( cg.snap->ps.weapon == WP_K43_SCOPE ) {
							CG_FinishWeaponChange( WP_K43_SCOPE, WP_K43 );
						}
					}
				
				// OSP
				if(cg.mvTotalClients < 1 || cg.snap->ps.stats[STAT_HEALTH] > 0)
					CG_DrawWeapReticle();

				return;
			}
			break;
		default:
			break;
	}

	if( cg.predictedPlayerState.eFlags & EF_PRONE_MOVING ) {
		return;
	}

	// FIXME: spectators/chasing?
	if( (cg.predictedPlayerState.weapon == WP_MORTAR_SET  || cg.predictedPlayerState.weapon == WP_MORTAR2_SET) && cg.predictedPlayerState.weaponstate != WEAPON_RAISING ) {
		CG_DrawMortarReticle();
		return;
	}

	CG_DrawDanger();

	if ( cg_drawCrosshair.integer < 0 )	//----(SA)	moved down so it doesn't keep the scoped weaps from drawing reticles
		return;

	// no crosshair while leaning
	// jet Pilot - Allow crosshair while leaning
//	if( cg.snap->ps.leanf ) {
//		return;
//	}

	// TAT 1/10/2003 - Don't draw crosshair if have exit hintcursor
	if (cg.snap->ps.serverCursorHint >= HINT_EXIT && cg.snap->ps.serverCursorHint <= HINT_NOEXIT )
		return;

	// jet Pilot - render scene onto scope lens
	// jet Pilot - FIXME - crashes on gerbalblaste's pc	
	if (r_dynamicTextures.integer)
	{
		static int texid = 0;
	//	int dx, dy, ds;

	//	ds = r_dynamicSize.integer;

		if( !texid && texid != -1) 
		{
			texid = trap_R_GetTextureId( "textures/effects/envmap_slate.tga");//"gfx/jetPilot/jetblank.jpg");
		}
		else
		{	
			trap_R_RenderToTexture( texid, 451, 294, 256, 256 ); // 0, 0, 256, 256			
		}
	}

	// set color based on health
	if ( cg_crosshairHealth.integer ) {
		vec4_t		hcolor;

		CG_ColorForHealth( hcolor );
		trap_R_SetColor( hcolor );
	} 
	else
	{
		trap_R_SetColor(cg.xhairColor);
	}

	w = h = cg_crosshairSize.value;

	// RF, crosshair size represents aim spread
	f = (float)((cg_crosshairPulse.integer == 0) ? 0 : cg.snap->ps.aimSpreadScale / 255.0);
	w *= ( 1 + f*2.0 );
	h *= ( 1 + f*2.0 );
	
	x = cg_crosshairX.integer;
	y = cg_crosshairY.integer;
	CG_AdjustFrom640( &x, &y, &w, &h );

	hShader = cgs.media.crosshairShader[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ];

	trap_R_DrawStretchPic( x + 0.5 * (cg.refdef_current->width - w), y + 0.5 * (cg.refdef_current->height - h), w, h, 0, 0, 1, 1, hShader );

	if ( cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] )
	{
		w = h = cg_crosshairSize.value;
		x = cg_crosshairX.integer;
		y = cg_crosshairY.integer;
		CG_AdjustFrom640( &x, &y, &w, &h );

		if(cg_crosshairHealth.integer == 0) 
		{
			trap_R_SetColor(cg.xhairColorAlt);
		}

		trap_R_DrawStretchPic( x + 0.5 * (cg.refdef_current->width - w), y + 0.5 * (cg.refdef_current->height - h), w, h, 0, 0, 1, 1, cg.crosshairShaderAlt[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ] );
	}
}

static void CG_DrawNoShootIcon( void ) {
	float x, y, w, h;
	float *color;

	if( cg.predictedPlayerState.eFlags & EF_PRONE && 
	   (cg.snap->ps.weapon == WP_PANZERFAUST || cg.snap->ps.weapon == WP_BAZOOKA) ) 
	{
		trap_R_SetColor( colorRed );
	}
	else if ( cg.crosshairClientNoShoot 
				// xkan, 1/6/2003 - don't shoot friend or civilian
				|| cg.snap->ps.serverCursorHint == HINT_PLYR_NEUTRAL
				|| cg.snap->ps.serverCursorHint == HINT_PLYR_FRIEND) {
		color = CG_FadeColor( cg.crosshairClientTime, 1000 );

		if ( !color ) {
			trap_R_SetColor( NULL );
			return;
		} else {
			trap_R_SetColor( color );
		}
	} else {
		return;
	}

	w = h = 48.f;

	x = cg_crosshairX.integer + 1;
	y = cg_crosshairY.integer + 1;
	CG_AdjustFrom640( &x, &y, &w, &h );

	trap_R_DrawStretchPic( x + 0.5 * (cg.refdef_current->width - w), y + 0.5 * (cg.refdef_current->height - h), w, h, 0, 0, 1, 1, cgs.media.friendShader );
}

/*
=================
CG_ScanForCrosshairMine
=================

 jet Pilot
 from forty's FortyMod
 forty - mine id.

*/

void CG_ScanForCrosshairMine(centity_t *cent) 
{
   trace_t      trace;
   vec3_t      start, end;

   VectorCopy( cg.refdef.vieworg, start );
   VectorMA( start, 512, cg.refdef.viewaxis[0], end );   

   CG_Trace( &trace, start, NULL, NULL, end, -1, MASK_SOLID );

   if(
   Square(trace.endpos[0] - cent->currentState.pos.trBase[0]) < 256 &&
   Square(trace.endpos[1] - cent->currentState.pos.trBase[1]) < 256 &&
   Square(trace.endpos[2] - cent->currentState.pos.trBase[2]) < 256 )
   {
      cg.crosshairMine = cent->currentState.otherEntityNum;
      cg.crosshairMineTime = cg.time;
   }
 } 


void CG_ScanForCrosshairDynamite(centity_t *cent) 
{
   trace_t      trace;
   vec3_t      start, end;

   VectorCopy( cg.refdef.vieworg, start );
   VectorMA( start, 512, cg.refdef.viewaxis[0], end );   

   CG_Trace( &trace, start, NULL, NULL, end, -1, MASK_SOLID );

   if(
   Square(trace.endpos[0] - cent->currentState.pos.trBase[0]) < 256 &&
   Square(trace.endpos[1] - cent->currentState.pos.trBase[1]) < 256 &&
   Square(trace.endpos[2] - cent->currentState.pos.trBase[2]) < 256 )
   {
	  cg.crosshairClientNum = cent->currentState.number;
	  cg.crosshairDynamite = cent->currentState.clientNum;
	  cg.crosshairClientTime = cg.time;
   }
 } 


/*
=================
CG_ScanForCrosshairEntity
=================

Returns the distance to the entity

*/
static float CG_ScanForCrosshairEntity( void ) {
	trace_t		trace;
	vec3_t		start, end;
	float		dist;
	centity_t*	cent;

	VectorCopy( cg.refdef.vieworg, start );
	VectorMA( start, 8192, cg.refdef.viewaxis[0], end );	//----(SA)	changed from 8192

	cg.crosshairClientNoShoot = qfalse;

	CG_Trace( &trace, start, NULL, NULL, end, cg.snap->ps.clientNum, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_ITEM );

	// How far from start to end of trace?
	dist = VectorDistance( start, trace.endpos );


	if ( trace.entityNum >= MAX_CLIENTS ) {
		if( cg_entities[trace.entityNum].currentState.eFlags & EF_TAGCONNECT ) {
			trace.entityNum = cg_entities[trace.entityNum].tagParent;
		}

		// is a tank with a healthbar
		// this might have some side-effects, but none right now as the script_mover is the only one that sets effect1Time
		if( ( cg_entities[trace.entityNum].currentState.eType == ET_MOVER && cg_entities[trace.entityNum].currentState.effect1Time ) ||
			cg_entities[trace.entityNum].currentState.eType == ET_CONSTRUCTIBLE_MARKER ) {
			// update the fade timer
			cg.crosshairClientNum = trace.entityNum;
			cg.crosshairClientTime = cg.time;
			cg.identifyClientRequest = cg.crosshairClientNum;
		}

		// Default: We're not looking at a client
		cg.crosshairNotLookingAtClient = qtrue;
		
		return dist;
	}

	// Reset the draw time for the SP crosshair
	cg.crosshairSPClientTime = cg.time;

	// Default: We're not looking at a client
	cg.crosshairNotLookingAtClient = qfalse;


	// update the fade timer
	cg.crosshairClientNum = trace.entityNum;
	cg.crosshairClientTime = cg.time;
	
	if ( cg.crosshairClientNum != cg.snap->ps.identifyClient && cg.crosshairClientNum != ENTITYNUM_WORLD ) {
		cg.identifyClientRequest = cg.crosshairClientNum;
	}

	cent = &cg_entities[cg.crosshairClientNum];

	if( cent && cent->currentState.powerups & (1 << PW_OPS_DISGUISED) ) {
		if(cgs.clientinfo[cg.crosshairClientNum].team == cgs.clientinfo[cg.clientNum].team) {
			cg.crosshairClientNoShoot = qtrue;
		}
	}

	return dist;
}



#define CH_KNIFE_DIST		48	// from g_weapon.c
#define CH_LADDER_DIST		100
#define CH_WATER_DIST		100
#define CH_BREAKABLE_DIST	64
#define CH_DOOR_DIST		96

#define CH_DIST				100 //128		// use the largest value from above

/*
==============
CG_CheckForCursorHints
	concept in progress...
==============
*/
void CG_CheckForCursorHints( void ) {
	trace_t		trace;
	vec3_t		start, end;
	centity_t	*tracent;
	vec3_t		pforward, eforward;
	float		dist = 0;

	if ( cg.renderingThirdPerson )
		return;

	if(cg.snap->ps.serverCursorHint) {	// server is dictating a cursor hint, use it.
		cg.cursorHintTime = cg.time;
		cg.cursorHintFade = 500;	// fade out time
		cg.cursorHintIcon = cg.snap->ps.serverCursorHint;
		cg.cursorHintValue = cg.snap->ps.serverCursorHintVal;
		return;
	}

	// From here on it's client-side cursor hints.  So if the server isn't sending that info (as an option)
	// then it falls into here and you can get basic cursorhint info if you want, but not the detailed info
	// the server sends.

	// the trace
	VectorCopy( cg.refdef_current->vieworg, start );
	VectorMA( start, CH_DIST, cg.refdef_current->viewaxis[0], end );

//	CG_Trace( &trace, start, vec3_origin, vec3_origin, end, cg.snap->ps.clientNum, MASK_ALL &~CONTENTS_MONSTERCLIP);
	CG_Trace( &trace, start, vec3_origin, vec3_origin, end, cg.snap->ps.clientNum, MASK_PLAYERSOLID);

	if(trace.fraction == 1){
		// jaquboss might be water
		if ( (CG_PointContents( trace.endpos, -1 ) & CONTENTS_WATER) && !(CG_PointContents( cg.refdef.vieworg, -1 ) & CONTENTS_WATER) && !(cg.snap->ps.powerups[PW_BREATHER])) {	// jaquboss - was only on servercode
			if(dist <= CH_WATER_DIST) {
				cg.cursorHintIcon = HINT_WATER;
				cg.cursorHintTime = cg.time;
				cg.cursorHintFade = 500;
				cg.cursorHintValue = 0;
			}
		}
		return;
	}

	dist = trace.fraction * CH_DIST;

	tracent = &cg_entities[ trace.entityNum ];

	// Arnout: invisible entities don't show hints
	if( trace.entityNum >= MAX_CLIENTS &&
		( tracent->currentState.powerups == STATE_INVISIBLE ||
		tracent->currentState.powerups == STATE_UNDERCONSTRUCTION ) ) {
		return;
	}



	//
	// world
	//
	if(trace.entityNum == ENTITYNUM_WORLD) {
		if (/*(trace.contents & CONTENTS_WATER)*/ (CG_PointContents( trace.endpos, -1 ) & CONTENTS_WATER) && !(CG_PointContents( cg.refdef.vieworg, -1 ) & CONTENTS_WATER) && !(cg.snap->ps.powerups[PW_BREATHER])) {	// jaquboss - was only on servercode
			if(dist <= CH_WATER_DIST) {
				cg.cursorHintIcon = HINT_WATER;
				cg.cursorHintTime = cg.time;
				cg.cursorHintFade = 500;
				cg.cursorHintValue = 0;
			}
		} else if( (trace.surfaceFlags & SURF_LADDER) && !(cg.snap->ps.pm_flags & PMF_LADDER) ) {
			if(dist <= CH_LADDER_DIST) {
				cg.cursorHintIcon = HINT_LADDER;
				cg.cursorHintTime = cg.time;
				cg.cursorHintFade = 500;
				cg.cursorHintValue = 0;
			}
		}


	} else if(trace.entityNum < MAX_CLIENTS ) { // people

		// knife
		if( trace.entityNum < MAX_CLIENTS &&	(cg.snap->ps.weapon == WP_KNIFE || 	cg.snap->ps.weapon == WP_KNIFE_KABAR || 
			(IS_PISTOL_ALL(cg.snap->ps.weapon) && cgs.clientinfo[cg.snap->ps.clientNum].skillBits[SK_LIGHT_WEAPONS] & (1<<9)) )) 
		{
			if(dist <= CH_KNIFE_DIST)
			{
				float /*w,*/ dp;
				AngleVectors (cg.snap->ps.viewangles,	pforward, NULL, NULL);
				AngleVectors (tracent->lerpAngles,		eforward, NULL, NULL);

				dp = DotProduct( eforward, pforward );

		
				if( DotProduct( eforward, pforward ) > 0.6f )	// from behind(-ish)
				{	
					//clientInfo_t *ci;
//					cg.clientNum
					//ci = &cgs.clientinfo [ cg.clientNum ];
					switch (cg_entities[ cg.clientNum ].currentState.weapon)
					{
						case WP_LUGER:
						case WP_SILENCER:
						case WP_AKIMBO_LUGER:
						case WP_AKIMBO_SILENCEDLUGER:
							cg.cursorHintIcon = HINT_LUGER;
							break;											
						case WP_COLT:
						case WP_SILENCED_COLT:
						case WP_AKIMBO_COLT:
						case WP_AKIMBO_SILENCEDCOLT:						
							cg.cursorHintIcon = HINT_PISTOL;
							break;
						default:
							cg.cursorHintIcon = HINT_KNIFE;
							break;
					}
					
					cg.cursorHintTime = cg.time;
					cg.cursorHintFade = 500;
					cg.cursorHintValue = 0;
				}
			}
		}
	}
}

int CG_CrosshairClientMaxHealth ( void ) {
	int maxHealth = 100;
	int i;

	for( i = 0; i < MAX_CLIENTS; i++ ) {
		if( !cgs.clientinfo[i].infoValid ) {
			continue;
		}

		if( cgs.clientinfo[i].team != cgs.clientinfo[cg.snap->ps.clientNum].team ) {
			continue;
		}

		if( cgs.clientinfo[i].cls != PC_MEDIC ) {
			continue;
		}

		maxHealth += 10;

		if( maxHealth >= 125 ) {
			maxHealth = 125;
			break;
		}
	}

	if( cgs.clientinfo[ cg.crosshairClientNum ].skillBits[SK_BATTLE_SENSE] & (1<<3) ) {
		maxHealth += 15;
	}

	if( cgs.clientinfo[ cg.crosshairClientNum ].cls == PC_MEDIC ) {
		maxHealth *= 1.12f;
	}

	return maxHealth;
}

int SkillNumForClass( int classNum );

/*
=====================
CG_DrawCrosshairNames
=====================
*/
static void CG_DrawCrosshairNames( void ) {
	float		*color;
	char		*name;
	const char	*s, *playerRank;
	int			playerHealth = 0;
	int			maxHealth = 1;
	vec4_t		c, bgcolor;
	float		barFrac, dist, w;
	qboolean	drawStuff = qfalse;
	qboolean	isTank = qfalse;
	int			playerClass;
	qboolean	colorNames=(qboolean)(cg_drawCrosshairNames.integer & 2);
	qboolean	rankIcon=(qboolean)(cg_drawCrosshairNames.integer & 4); 
	qboolean	transparentBar=qfalse;
	vec4_t		trans={ 1.f, 1.f, 1.f, .3f };
	vec4_t		transBg={ 1.f, 1.f, 1.f, .125f };
	vec4_t		transTx={ 1.f, 1.f, 1.f, .5f };

	if ( cg_drawCrosshair.integer < 0 ) {
		return;
	}

	// jet Pilot - forty - mine id's
	if ( cg.crosshairMine > -1 ) 
	{
		color = CG_FadeColor( cg.crosshairMineTime, 1000 );
		s = va("%s^7\'s landmine", cgs.clientinfo[cg.crosshairMine].name);
		w = CG_DrawStrlen( s ) * (SMALLCHAR_WIDTH-2);
		CG_DrawStringExt(320 - w / 2, 170, s, colorWhite, !colorNames, !colorNames,(SMALLCHAR_WIDTH-2), (SMALLCHAR_HEIGHT-2), 0);
		cg.crosshairMine = -1;
		return;
	} 

	// scan the known entities to see if the crosshair is sighted on one
	dist = CG_ScanForCrosshairEntity();

	if ( cg.renderingThirdPerson )
		return;

	// draw the name of the player being looked at
	color = CG_FadeColor( cg.crosshairClientTime, 1000 );


	trap_R_SetColor( NULL );

	if ( !color ) {
		return;
	}

	// NERVE - SMF
	if ( cg.crosshairClientNum > MAX_CLIENTS ) {
		if ( !cg_drawCrosshairNames.integer ) {
			return;
		}

		if( cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR ) {
			if( cg_entities[cg.crosshairClientNum].currentState.eType == ET_MOVER && cg_entities[cg.crosshairClientNum].currentState.effect1Time ) {
				isTank = qtrue;

				playerHealth = cg_entities[cg.crosshairClientNum].currentState.dl_intensity;
				maxHealth = 255;

				s = Info_ValueForKey( CG_ConfigString( CS_SCRIPT_MOVER_NAMES ), va( "%i", cg.crosshairClientNum ) );
				if( !*s ) {
					return;
				}

				w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
				CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );
			} else if( cg_entities[cg.crosshairClientNum].currentState.eType == ET_CONSTRUCTIBLE_MARKER ) {
				s = Info_ValueForKey( CG_ConfigString( CS_CONSTRUCTION_NAMES ), va( "%i", cg.crosshairClientNum ) );
				if( *s ) {
					w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
					CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );
				}
				return;
			} else if (cg.crosshairDynamite > -1 ) {

				if ( Distance( cg.predictedPlayerState.origin, cg_entities[cg.crosshairClientNum].lerpOrigin) > Square( 64 ) )
					return;

				if ( cg_entities[cg.crosshairClientNum].currentState.teamNum % 4 == cgs.clientinfo[cg.snap->ps.clientNum].team )
				{
					s = va("%s^7\'s dynamite", cgs.clientinfo[cg.crosshairDynamite].name);
					w = CG_DrawStrlen( s ) * (SMALLCHAR_WIDTH-2);
					CG_DrawStringExt(320 - w / 2, 170, s, colorWhite, !colorNames, !colorNames, (SMALLCHAR_WIDTH-2), (SMALLCHAR_HEIGHT-2), 0);
				} else {
					s = "Dynamite";
					w = CG_DrawStrlen( s ) * (SMALLCHAR_WIDTH-2);
					CG_DrawStringExt(320 - w / 2, 170, s, transTx, qtrue, qtrue, (SMALLCHAR_WIDTH-2), (SMALLCHAR_HEIGHT-2), 0);
				}

				isTank = qtrue;

				// not armed, draw just name
				if ( cg_entities[cg.crosshairClientNum].currentState.teamNum >= 4 ||
					!(cgs.clientinfo[cg.snap->ps.clientNum].skillBits[SK_BATTLE_SENSE] & (1<<7)) )
					return;

				transparentBar = qtrue;

				playerHealth = 30000 - (cg.time - cg_entities[cg.crosshairClientNum].currentState.effect1Time);
				maxHealth = 30000;
				cg.crosshairDynamite = -1;
			}
		}

		if( !isTank ) {
			return;
		}
	} else if( cgs.clientinfo[cg.crosshairClientNum].team != cgs.clientinfo[cg.snap->ps.clientNum].team ) {
		if( (cg_entities[cg.crosshairClientNum].currentState.powerups & (1 << PW_OPS_DISGUISED)) && cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR) {

			if((cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR &&
				cgs.clientinfo[cg.snap->ps.clientNum].skillBits[SK_BATTLE_SENSE] & (1<<4) && 
				(cgs.clientinfo[cg.snap->ps.clientNum].cls == PC_FIELDOPS ||
				(cgs.jp_keepAwards & JP_KEEP_RECOGNITION)))
				&& !(cgs.clientinfo[ cg.crosshairClientNum ].skillBits[SK_COVERTOPS] & (1<<7)) ){
					vec4_t redcol = { 1, 0, 0, 0.8f };
					s = CG_TranslateString( "Disguised Enemy!" );
					w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
					CG_DrawSmallStringColor( 320 - w / 2, 170, s, redcol );
					return;
			} else if( dist > 512 || cgs.clientinfo[ cg.crosshairClientNum ].skillBits[SK_COVERTOPS] & (1<<9) ) {
				qboolean drawRank = qfalse;
				float x = 0;

				if ( !cg_drawCrosshairNames.integer ) {
					return;
				}

				drawStuff = qtrue;

				name = cgs.clientinfo[ cg.crosshairClientNum ].disguiseName;
				playerClass = ( cg_entities[ cg.crosshairClientNum ].currentState.powerups >> PW_OPS_CLASS_1) & 6;
				
				if ( rankIcon ){
					s = va( "%s^7", name );
				} else {
					playerRank = cgs.clientinfo[ cg.crosshairClientNum ].team != TEAM_AXIS ? rankNames_Axis[cgs.clientinfo[cg.crosshairClientNum].disguiseRank] : rankNames_Allies[cgs.clientinfo[cg.crosshairClientNum].disguiseRank];
					s = va( "%s %s^7", playerRank, name );
				}

				w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;

				if ( rankIcon && cgs.clientinfo[ cg.crosshairClientNum ].disguiseRank >= 1 ){
					drawRank = qtrue;
					x += 21/2; // 5 for a gap, 16 for icon
				}

				// class 
				x += 21/2; // 5 for a gap, 16 for icon
				CG_DrawPic( 320 - x - w/2, 170, 16, 16, cgs.media.skillPics[SkillNumForClass(playerClass)]);

				// rank
				if ( drawRank )
				{
					CG_DrawPic( 320 - x/2 - w/2, 170, 16, 16, rankicons[ cgs.clientinfo[ cg.crosshairClientNum ].disguiseRank ][ cgs.clientinfo[ cg.crosshairClientNum ].team != TEAM_AXIS ? 1 : 0 ][0].shader );
				}

				// name
				CG_DrawStringExt(320 + x - w/2, 170, s, colorWhite, !colorNames, !colorNames, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0);


				// set the health
				if( cg.crosshairClientNum == cg.snap->ps.identifyClient ) {
					playerHealth = cg.snap->ps.identifyClientHealth;
				} else {
					playerHealth = cgs.clientinfo[ cg.crosshairClientNum ].health;
				}

				maxHealth = 100;
			} else {
				// rain - #480 - don't show the name after you look away, should this be a disguised covert
				cg.crosshairClientTime = 0;
				return;
			}
		} else {
			return;
		}
	}

	if ( !cg_drawCrosshairNames.integer ) {
		return;
	}

	// we only want to see players on our team
	if ( !isTank &&	!( cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR && cgs.clientinfo[ cg.crosshairClientNum ].team != cgs.clientinfo[cg.snap->ps.clientNum].team ) ) 
	{
		qboolean drawRank = qfalse;
		float x = 0.f;

		drawStuff = qtrue;

		// determine player class
		playerClass = cg_entities[ cg.crosshairClientNum ].currentState.teamNum ;
		name = cgs.clientinfo[ cg.crosshairClientNum ].name;
		
		if ( rankIcon ){
			s = va( "%s^7", name );
		} else {
			playerRank = cgs.clientinfo[cg.crosshairClientNum].team == TEAM_AXIS ? rankNames_Axis[cgs.clientinfo[cg.crosshairClientNum].rank] : rankNames_Allies[cgs.clientinfo[cg.crosshairClientNum].rank];
			s = va( "%s %s^7", playerRank, name );
		}

		w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;

		if ( rankIcon && cgs.clientinfo[ cg.crosshairClientNum ].rank >= 1 ){
			drawRank = qtrue;
			x += 21/2; // 5 for a gap, 16 for icon
		}

		// class 
		x += 21/2; // 5 for a gap, 16 for icon
		CG_DrawPic( 320 - x - w/2, 170, 16, 16, cgs.media.skillPics[SkillNumForClass(playerClass)]);

		// rank
		if ( drawRank )
		{
			CG_DrawPic( 320 - x + 21 - w/2, 170, 16, 16, rankicons[ cgs.clientinfo[ cg.crosshairClientNum ].rank ][ cgs.clientinfo[ cg.crosshairClientNum ].team == TEAM_AXIS ? 1 : 0 ][0].shader );
		}

		// name
		CG_DrawStringExt(320 + x - w/2, 170, s, colorWhite, !colorNames, !colorNames, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 0);

		// set the health
		if( cg.crosshairClientNum == cg.snap->ps.identifyClient ) {
			playerHealth = cg.snap->ps.identifyClientHealth;
		} else {
			playerHealth = cgs.clientinfo[ cg.crosshairClientNum ].health;
		}

		maxHealth = CG_CrosshairClientMaxHealth();
	}

	barFrac = (float)playerHealth / maxHealth;

	if ( barFrac > 1.0 )
		barFrac = 1.0;
	else if ( barFrac < 0 )
		barFrac = 0;

	c[0] = 1.0f;
	c[1] = c[2] = barFrac;
	c[3] = (0.25 + barFrac * 0.5) * color[3];

	Vector4Set( bgcolor, 1.f, 1.f, 1.f, .25f * color[3] );

	if ( transparentBar )
	{
		trans[3] = trans[3] * color[3];
		transBg[3] = transBg[3] * color[3];
		CG_FilledBar( 320 - 77 / 2, 200-7, 77, 7, trans, NULL, transBg, barFrac, 16 );
	}
	else
	{
#if	0 // draw health cross
		if ( !isTank ){
			CG_DrawPic( 320 - (110+18) / 2, 187, 16, 16, cgs.media.healthHintShader );
			CG_FilledBar( 320 - (110-18) / 2, 190, 110, 10, c, NULL, bgcolor, barFrac, 16 );
		} else {
#endif
			CG_FilledBar( 320 - 110 / 2, 190, 110, 10, c, NULL, bgcolor, barFrac, 16 );
#if	0 // draw health cross
		}
#endif
	}

	if ( !isTank && cg.snap->ps.stats[STAT_IDENTIFYAMMO] > -1 &&
		cgs.clientinfo[cg.snap->ps.clientNum].cls == PC_FIELDOPS &&
		cgs.clientinfo[cg.snap->ps.clientNum].skillBits[SK_SIGNALS] & (1<<6) ) {
		
		barFrac = cg.snap->ps.stats[STAT_IDENTIFYAMMO] / 255.f;

		c[0] = 0.25f;
		c[1] = 0.75f;
		c[2] = 0.25f;
		c[3] = 0.25 * color[3];

		transBg[3] = transBg[3] * color[3];

		CG_DrawPic( 320 - (90+22) / 2, 204, 16, 16, cgs.media.ammoHintShader );
		CG_FilledBar( 320 - (90-16) / 2, 208, 90, 8, c, NULL, transBg, barFrac, 16 );
	
		if ( cg.snap->ps.stats[STAT_IDENTIFYCLIP] > -1 ){
			barFrac = cg.snap->ps.stats[STAT_IDENTIFYCLIP] / 255.f;

			c[0] = 0.5f;
			c[1] = 1.0f;
			c[2] = 0.5f;
			c[3] = 0.25 * color[3];
			Vector4Set( bgcolor, 1.f, 1.f, 1.f, .25f * color[3] );
			CG_FilledBar( 320 - (85-16) / 2, 213, 85, 2, c, NULL, NULL, barFrac, 0 );
		}
	}

	trap_R_SetColor( NULL );
}



//==============================================================================

/*
=================
CG_DrawSpectator
=================
*/
static void CG_DrawSpectator(void)
{
	CG_DrawBigString( 320 - 9 * 8, 440, CG_TranslateString( "SPECTATOR" ), 1.f );
}

/*
=================
CG_DrawVote
=================
*/
static void CG_DrawVote(void) {
	char	*s;
	char str1[32], str2[32];
	float color[4] = { 1, 1, 0, 1 };
	int		sec;

	if( cgs.complaintEndTime > cg.time && !cg.demoPlayback && cg_complaintPopUp.integer > 0 && cgs.complaintClient >= 0 ) 
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = va( CG_TranslateString( "File complaint against ^7%s^3 for team-killing?" ), cgs.clientinfo[cgs.complaintClient].name);
		CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}

	if( cgs.applicationEndTime > cg.time && cgs.applicationClient >= 0 )
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = va( CG_TranslateString( "Accept ^7%s^3's application to join your fireteam?" ), cgs.clientinfo[cgs.applicationClient].name);
		CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}

	if( cgs.propositionEndTime > cg.time && cgs.propositionClient >= 0)
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = va( CG_TranslateString( "Accept ^7%s^3's proposition to invite ^7%s^3 to join your fireteam?" ), cgs.clientinfo[cgs.propositionClient2].name, cgs.clientinfo[cgs.propositionClient].name);
		CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}

	if ( cgs.invitationEndTime > cg.time && cgs.invitationClient >= 0 ) 
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = va( CG_TranslateString( "Accept ^7%s^3's invitation to join their fireteam?" ), cgs.clientinfo[cgs.invitationClient].name);
		CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}

	if ( cgs.autoFireteamEndTime > cg.time && cgs.autoFireteamNum == -1 ) 
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = "Make Fireteam private?";
		CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}

	if ( cgs.autoFireteamCreateEndTime > cg.time && cgs.autoFireteamCreateNum == -1 )
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = "Create a Fireteam?";
		CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}
	
	if ( cgs.autoFireteamJoinEndTime > cg.time && cgs.autoFireteamJoinNum == -1 ) 
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		s = "Join a Fireteam?";
		CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

		s = va( CG_TranslateString( "Press '%s' for YES, or '%s' for No" ), str1, str2 );
		CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}
	

	if( cgs.voteTime ) 
	{
		Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
		Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );

		// play a talk beep whenever it is modified
		if( cgs.voteModified ) {
			cgs.voteModified = qfalse;
		}

		sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
		if( sec < 0 ) {
			sec = 0;
		}

		if( !Q_stricmpn( cgs.voteString, "kick", 4 ) ) 
		{
			if( strlen( cgs.voteString ) > 5 ) {
				int nameindex;
				char buffer[ 128 ];
				Q_strncpyz( buffer, cgs.voteString + 5, sizeof( buffer ) );
				Q_CleanStr( buffer );

				for( nameindex = 0; nameindex < MAX_CLIENTS; nameindex++ ) 
				{
					if( !cgs.clientinfo[ nameindex ].infoValid ) 
					{
						continue;
					}

					if( !Q_stricmp( cgs.clientinfo[ nameindex ].cleanname, buffer ) ) 
					{
						if( cgs.clientinfo[ nameindex ].team != TEAM_SPECTATOR && cgs.clientinfo[ nameindex ].team != cgs.clientinfo[ cg.clientNum ].team )
						{
							return;
						}
					}
				}
			}
		}

		if ( !(cg.snap->ps.eFlags & EF_VOTED) ) 
		{
			s = va( CG_TranslateString( "VOTE(%i): %s" ), sec, cgs.voteString);
			CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

			if( cgs.clientinfo[cg.clientNum].team != TEAM_AXIS && cgs.clientinfo[cg.clientNum].team != TEAM_ALLIES ) 
			{
				s = CG_TranslateString( "Cannot vote as Spectator" );
			}
			else
			{
				s = va( CG_TranslateString( "YES(%s):%i, NO(%s):%i" ), str1, cgs.voteYes, str2, cgs.voteNo );
			}
			CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 60 );
			return;
		}
		else 
		{
			s = va( CG_TranslateString( "YOU VOTED ON: %s" ), cgs.voteString);
			CG_DrawStringExt( 8, 200, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );

			s = va( CG_TranslateString( "Y:%i, N:%i" ), cgs.voteYes, cgs.voteNo );
			CG_DrawStringExt( 8, 214, s, color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 20 );
			return;
		}
	}

	if( cgs.complaintEndTime > cg.time && !cg.demoPlayback && cg_complaintPopUp.integer > 0 && cgs.complaintClient < 0 )
	{
		if( cgs.complaintClient == -1 ) 
		{
			s = "Your complaint has been filed";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}
		if( cgs.complaintClient == -2 )
		{
			s = "Complaint dismissed";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}
		if( cgs.complaintClient == -3 ) 
		{
			s = "Server Host cannot be complained against";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}
		if( cgs.complaintClient == -4 )
		{
			s = "You were team-killed by the Server Host";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}
	}

	if( cgs.applicationEndTime > cg.time && cgs.applicationClient < 0 )
	{
		if( cgs.applicationClient == -1 )
		{
			s = "Your application has been submitted";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.applicationClient == -2 )
		{
			s = "Your application failed";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.applicationClient == -3 ) 
		{
			s = "Your application has been approved";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.applicationClient == -4 ) 
		{
			s = "Your application reply has been sent";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}
	}

	if( cgs.propositionEndTime > cg.time && cgs.propositionClient < 0)
	{
		if( cgs.propositionClient == -1 )
		{
			s = "Your proposition has been submitted";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.propositionClient == -2 )
		{
			s = "Your proposition was rejected";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.propositionClient == -3 )
		{
			s = "Your proposition was accepted";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.propositionClient == -4 )
		{
			s = "Your proposition reply has been sent";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}
	}

	if( cgs.invitationEndTime > cg.time && cgs.invitationClient < 0 )
	{
		if( cgs.invitationClient == -1 )
		{
			s = "Your invitation has been submitted";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.invitationClient == -2 ) 
		{
			s = "Your invitation was rejected";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.invitationClient == -3 )
		{
			s = "Your invitation was accepted";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.invitationClient == -4 )
		{
			s = "Your invitation reply has been sent";
			CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qfalse, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
			return;
		}

		if( cgs.invitationClient < 0 ) 
		{
			return;
		}
	}

	if( (cgs.autoFireteamEndTime > cg.time && cgs.autoFireteamNum == -2) || (cgs.autoFireteamCreateEndTime > cg.time && cgs.autoFireteamCreateNum == -2) || (cgs.autoFireteamJoinEndTime > cg.time && cgs.autoFireteamJoinNum == -2)) 
	{
		s = "Response Sent";
		CG_DrawStringExt( 8, 200, CG_TranslateString( s ), color, qtrue, qtrue, TINYCHAR_WIDTH - 2, TINYCHAR_HEIGHT - 2, 80 );
		return;
	}
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission( void )
{
	// End-of-level autoactions
	if(!cg.demoPlayback)
	{
		static int doScreenshot = 0, doDemostop = 0;

		if(!cg.latchAutoActions) 
		{
			cg.latchAutoActions = qtrue;

			if(cg_autoAction.integer & AA_SCREENSHOT) 
			{
				doScreenshot = cg.time + 1000;
			}

			if(cg_autoAction.integer & AA_STATSDUMP)
			{
				CG_dumpStats_f();
			}

			if((cg_autoAction.integer & AA_DEMORECORD) &&
			  ((cgs.gametype == GT_WOLF_STOPWATCH && cgs.currentRound == 0) ||
			    cgs.gametype != GT_WOLF_STOPWATCH))
			{
				doDemostop = cg.time + 5000;	// stats should show up within 5 seconds
			}
		}

		if(doScreenshot > 0 && doScreenshot < cg.time) 
		{
			CG_autoScreenShot_f();
			doScreenshot = 0;
		}

		if(doDemostop > 0 && doDemostop < cg.time)
		{
			trap_SendConsoleCommand("stoprecord\n");
			doDemostop = 0;
		}
	}

	// Intermission view
	CG_Debriefing_Draw();

/*	cg.scoreFadeTime = cg.time;
	CG_DrawScoreboard();
*/
}

/*
=================
CG_ActivateLimboMenu

NERVE - SMF
=================
*/
static void CG_ActivateLimboMenu(void) {
/*	static qboolean latch = qfalse;
	qboolean test;

	// should we open the limbo menu (make allowances for MV clients)
	test = ((cg.snap->ps.pm_flags & PMF_LIMBO) ||
			( (cg.mvTotalClients < 1 && (
				(cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR) ||
				(cg.warmup))
			  )
			&& cg.snap->ps.pm_type != PM_INTERMISSION ) );


	// auto open/close limbo mode
	if(cg_popupLimboMenu.integer && !cg.demoPlayback) {
		if(test && !latch) {			
			CG_LimboMenu_f();
			latch = qtrue;
		} else if(!test && latch && cg.showGameView) {
			CG_EventHandling(CGAME_EVENT_NONE, qfalse);
			latch = qfalse;
		}
	}*/
}

/*
=================
CG_DrawSpectatorMessage
=================
*/
static void CG_DrawSpectatorMessage( void ) 
{
	const char *str, *str2;
	float x, y;
	static int lastconfigGet = 0;

	// jet Pilot - moved to calling function
//	if ( !cg_descriptiveText.integer )
//		return;

	if ( !( cg.snap->ps.pm_flags & PMF_LIMBO || cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) )
		return;

	if(cg.time - lastconfigGet > 1000) 
	{
		Controls_GetConfig();

		lastconfigGet = cg.time;
	}

	x = (cg.snap->ps.pm_flags & PMF_LIMBO) ? 170 : 80;
	y = 408;

	y -= 2 * TINYCHAR_HEIGHT;

	str2 = BindingFromName( "openlimbomenu" );
	if ( !Q_stricmp( str2, "(openlimbomenu)" ) ) {
		str2 = "ESCAPE";
	}
	str = va( CG_TranslateString( "Press %s to open Limbo Menu" ), str2 );
	CG_DrawStringExt( 8, 150, str, colorWhite, qtrue, qtrue, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );

	str2 = BindingFromName( "+attack" );
	str = va( CG_TranslateString( "Press %s to follow next player" ), str2 );
	CG_DrawStringExt( 8, 164, str, colorWhite, qtrue, qtrue, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );

#ifdef MV_SUPPORT
	if ( cgs.mvAllowed ){
		str2 = BindingFromName( "mvactivate" );
		str = va( CG_TranslateString( "Press %s to %s multiview mode" ), str2, ((cg.mvTotalClients > 0) ? "disable" : "activate") );   // CHRUKER: b0xx
		CG_DrawStringExt( 8, 190, str, colorWhite, qtrue, qtrue, MINICHAR_WIDTH, MINICHAR_HEIGHT, 0 );   // CHRUKER: b0xx
		y += TINYCHAR_HEIGHT;
	}
#endif 
}


float CG_CalculateReinfTime_Float( qboolean menu ) {
	team_t team; 
	int dwDeployTime; 

	if( menu ) {
		if(cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) {
			team = cgs.ccSelectedTeam == 0 ? TEAM_AXIS : TEAM_ALLIES;
		} else {
			team = cgs.clientinfo[cg.clientNum].team;
		}
	} else {
		team = cgs.clientinfo[cg.snap->ps.clientNum].team;
	}

	dwDeployTime = (team == TEAM_AXIS) ? cg_redlimbotime.integer : cg_bluelimbotime.integer;
	return (1 + (dwDeployTime - ((cgs.aReinfOffset[team] + cg.time - cgs.levelStartTime) % dwDeployTime)) * 0.001f);
}

int CG_CalculateReinfTime( qboolean menu ) {
	return((int)CG_CalculateReinfTime_Float( menu ));
}


/*
=================
CG_DrawLimboMessage
=================
*/

#define INFOTEXT_STARTX	8

static void CG_DrawLimboMessage( void ) 
{
	float color[4] = { 1, 1, 1, 1 };
	const char *str; 
	playerState_t *ps;
	int y = 118;


	ps = &cg.snap->ps;

	if ( ps->stats[STAT_HEALTH] > 0 ) {
		return;
	}

	if( cg.snap->ps.pm_flags & PMF_LIMBO || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR )
	{
		return;
	}

	if( cg_descriptiveText.integer ) 
	{
		/*if ( ps->eFlags & EF_PLAYDEAD )
			str = CG_TranslateString( "You are playing dead." );
		else*/
			str = CG_TranslateString( "You are wounded and waiting for a medic." );

		CG_DrawTinyStringColor( INFOTEXT_STARTX, y, str, color );
		y += 12;

		if( cgs.gametype == GT_WOLF_LMS && !( ps->eFlags & EF_PLAYDEAD ) ) {
			trap_R_SetColor( NULL );
			return;
		}

		/*if ( ps->eFlags & EF_PLAYDEAD )
			str = CG_TranslateString( va("Press %s to stand up", BindingFromName( "playdead" )) );
		else*/
			str = CG_TranslateString( "Press JUMP to go into reinforcement queue." );

		CG_DrawTinyStringColor( INFOTEXT_STARTX, 130, str, color );
		y += 12;
	}
	// removed else so playdead will end here
	if( cgs.gametype == GT_WOLF_LMS /*|| ps->eFlags & EF_PLAYDEAD */) 
	{
		trap_R_SetColor( NULL );
		return;
	}

	// JPW NERVE
	str = (ps->persistant[PERS_RESPAWNS_LEFT] == 0) ? CG_TranslateString("No more reinforcements this round.") : va(CG_TranslateString("Reinforcements deploy in %d seconds."), CG_CalculateReinfTime( qfalse ));

	CG_DrawTinyStringColor( INFOTEXT_STARTX, y, str, color );
	y += 12;
	// jpw

	trap_R_SetColor( NULL );
}
// -NERVE - SMF

/*
=================
CG_DrawFollow
=================
*/
static qboolean CG_DrawFollow(void)
{
	char deploytime[128];

	// MV following info for mainview
	if(CG_ViewingDraw()) {
		return(qtrue);
	}

	if(!(cg.snap->ps.pm_flags & PMF_FOLLOW)) {
		return(qfalse);
	}

	// Spectator view teamflags
	if(cg.snap->ps.clientNum != cg.clientNum && 
		cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) {

		if(cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES) {
			CG_DrawPic( INFOTEXT_STARTX, 118 - 12, 18, 12, cgs.media.alliedFlag);
		} else {
			CG_DrawPic( INFOTEXT_STARTX, 118 - 12, 18, 12, cgs.media.axisFlag);
		}
	}

	// if in limbo, show different follow message 
	if(cg.snap->ps.pm_flags & PMF_LIMBO) {
		if(cgs.gametype != GT_WOLF_LMS) {
			if( cg.snap->ps.persistant[PERS_RESPAWNS_LEFT] == 0 ) {
				if( cg.snap->ps.persistant[PERS_RESPAWNS_PENALTY] >= 0 ) {
					int deployTime = (cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_AXIS) ? cg_redlimbotime.integer : cg_bluelimbotime.integer;

					deployTime *= 0.001f;

					sprintf(deploytime, CG_TranslateString("Bonus Life! Deploying in %d seconds"), CG_CalculateReinfTime(qfalse) + cg.snap->ps.persistant[PERS_RESPAWNS_PENALTY] * deployTime );
				} else {
					sprintf(deploytime, CG_TranslateString("No more deployments this round"));
				}
			} else {
				sprintf(deploytime, CG_TranslateString("Deploying in %d seconds"), CG_CalculateReinfTime(qfalse));
			}

			CG_DrawStringExt(INFOTEXT_STARTX, 118, deploytime, colorWhite, qtrue, qtrue, SMALLCHAR_WIDTH-1, SMALLCHAR_HEIGHT-2, 80);
		}

		// Don't display if you're following yourself
		if(cg.snap->ps.clientNum != cg.clientNum) {
			sprintf(deploytime, "(%s %s %s [%s])", CG_TranslateString("Following"),
					cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES ? rankNames_Allies[cgs.clientinfo[cg.snap->ps.clientNum].rank] : rankNames_Axis[cgs.clientinfo[cg.snap->ps.clientNum].rank],
					cgs.clientinfo[cg.snap->ps.clientNum].name,
					BG_ClassnameForNumber(cgs.clientinfo[cg.snap->ps.clientNum].cls));

			CG_DrawStringExt( INFOTEXT_STARTX, 136, deploytime, colorWhite, qtrue, qtrue, SMALLCHAR_WIDTH-1, SMALLCHAR_HEIGHT-2, 80);
		}
	} else {
		CG_DrawStringExt( INFOTEXT_STARTX, 118, CG_TranslateString("Following"), colorWhite, qtrue, qtrue, (BIGCHAR_WIDTH/2)-1, BIGCHAR_HEIGHT-2, 0 );

		CG_DrawStringExt( 84, 118, va("%s %s [%s]",
					cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_ALLIES ? rankNames_Allies[cgs.clientinfo[cg.snap->ps.clientNum].rank] : rankNames_Axis[cgs.clientinfo[cg.snap->ps.clientNum].rank],
					cgs.clientinfo[cg.snap->ps.clientNum].name, BG_ClassnameForNumber(cgs.clientinfo[cg.snap->ps.clientNum].cls)),
					colorWhite, qtrue, qtrue, BIGCHAR_WIDTH/2, BIGCHAR_HEIGHT, 0);
	}

	return(qtrue);
}


/*
=================
CG_DrawWarmup
=================
*/
static void CG_DrawWarmup( void ) {
	int			w;
	int			sec;
	int			cw;
	const char	*s, *s1, *s2;

	sec = cg.warmup;
	if(!sec) {
		if((cgs.gamestate == GS_WARMUP && !cg.warmup) || cgs.gamestate == GS_WAITING_FOR_PLAYERS) {
			cw = 10-1;

			if ( CG_ConfigString( CS_CONFIGNAME )[0] )
			{
				s1 = va( "^3Config:^7%s^7", CG_ConfigString( CS_CONFIGNAME ));
				w = CG_DrawStrlen( s1 );
				CG_DrawStringExt(320 - w * 12/2, 162, s1, colorWhite, qfalse, qtrue, 12, 16, 0);
			}

			s1 = va( CG_TranslateString( "^3WARMUP:^7 Waiting on ^2%i^7 %s" ), cgs.minclients, cgs.minclients == 1 ? "player" : "players" );
			w = CG_DrawStrlen( s1 );
			CG_DrawStringExt(320 - w * 12/2, 188, s1, colorWhite, qfalse, qtrue, 12, 18, 0);

			if(!cg.demoPlayback && cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR &&
			  (!(cg.snap->ps.pm_flags & PMF_FOLLOW) || (cg.snap->ps.pm_flags & PMF_LIMBO))) {
				char str1[32];
				Q_strncpyz( str1, BindingFromName( "ready" ), 32 );
				if( !Q_stricmp( str1, "(?" "?" "?)" ) ) {
					s2 = CG_TranslateString( "Type ^3\\ready^7 in the console to start" ); // ^*
				} else {
					s2 = va( "Press ^3%s^7 to start", str1 ); // ^*
					s2 = CG_TranslateString( s2 );
				}				
				w = CG_DrawStrlen( s2 );
				CG_DrawStringExt(320 - w * cw/2, 208, s2, colorWhite, qfalse, qtrue, cw, ((int)(cw * 1.5))-2, 0);
			}
	
/*	if ( !sec ) {
		if ( cgs.gamestate == GS_WAITING_FOR_PLAYERS ) {
			cw = 10;

			s = CG_TranslateString( "Game Stopped - Waiting for more players" );

			w = CG_DrawStrlen( s );
			CG_DrawStringExt( 320 - w * 6, 120, s, colorWhite, qfalse, qtrue, 12, 18, 0 );

			if( cg_gameType.integer != GT_WOLF_LMS ) {
			s1 = va( CG_TranslateString( "Waiting for %i players" ), cgs.minclients );
			s2 = CG_TranslateString( "or call a vote to start the match" );

			w = CG_DrawStrlen( s1 );
			CG_DrawStringExt( 320 - w * cw/2, 160, s1, colorWhite, 
				qfalse, qtrue, cw, (int)(cw * 1.5), 0 );

			w = CG_DrawStrlen( s2 );
			CG_DrawStringExt( 320 - w * cw/2, 180, s2, colorWhite, 
				qfalse, qtrue, cw, (int)(cw * 1.5), 0 );
			}
*/
			return;
		}

		return;
	}

	sec = ( sec - cg.time ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}

		s = va( "%s %i", CG_TranslateString( "(WARMUP) Match begins in:" ), sec + 1 );

	w = CG_DrawStrlen( s );
	CG_DrawStringExt( 320 - w * 6, 120, s, colorYellow, qfalse, qtrue, 12, 18, 0 );

	// NERVE - SMF - stopwatch stuff
	s1 = "";
	s2 = "";

	if ( cgs.gametype == GT_WOLF_STOPWATCH ) {
		const char	*cs;
		int		defender;

		s = va( "%s %i", CG_TranslateString( "Stopwatch Round" ), cgs.currentRound + 1 );

		cs = CG_ConfigString( CS_MULTI_INFO );
		defender = atoi( Info_ValueForKey( cs, "defender" ) );

		if ( !defender ) {
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Axis team";
					s2 = "Keep the Allies from beating the clock!";
				}
				else {
					s1 = "You are on the Axis team";
				}
			}
			else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Allied team";
					s2 = "Try to beat the clock!";
				}
				else {
					s1 = "You are on the Allied team";
				}
			}
		}
		else {
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Axis team";
					s2 = "Try to beat the clock!";
				}
				else {
					s1 = "You are on the Axis team";
				}
			}
			else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ) {
				if ( cgs.currentRound == 1 ) {
					s1 = "You have been switched to the Allied team";
					s2 = "Keep the Axis from beating the clock!";
				}
				else {
					s1 = "You are on the Allied team";
				}
			}
		}

		if ( strlen( s1 ) )
			s1 = CG_TranslateString( s1 );

		if ( strlen( s2 ) )
			s2 = CG_TranslateString( s2 );

		cw = 10-1;

		w = CG_DrawStrlen( s );
		CG_DrawStringExt( 320 - w * cw/2, 140, s, colorWhite, 
			qfalse, qtrue, cw, ((int)(cw * 1.5))-2, 0 );

		w = CG_DrawStrlen( s1 );
		CG_DrawStringExt( 320 - w * cw/2, 160, s1, colorWhite, 
			qfalse, qtrue, cw, ((int)(cw * 1.5))-2, 0 );

		w = CG_DrawStrlen( s2 );
		CG_DrawStringExt( 320 - w * cw/2, 180, s2, colorWhite, 
			qfalse, qtrue, cw, ((int)(cw * 1.5))-2, 0 );
	}
}

//==================================================================================

/*
=================
CG_DrawFlashFade
=================
*/
static void CG_DrawFlashFade( void ) {
	static int lastTime;
	int elapsed, time;
	vec4_t col;
	qboolean fBlackout = (!CG_IsSinglePlayer() && int_ui_blackout.integer > 0);

	if (cgs.fadeStartTime + cgs.fadeDuration < cg.time) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	} else if (cgs.fadeAlphaCurrent != cgs.fadeAlpha) {
		elapsed = (time = trap_Milliseconds()) - lastTime;	// we need to use trap_Milliseconds() here since the cg.time gets modified upon reloading
		lastTime = time;
		if (elapsed < 500 && elapsed > 0) {
			if (cgs.fadeAlphaCurrent > cgs.fadeAlpha) {
				cgs.fadeAlphaCurrent -= ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent < cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			} else {
				cgs.fadeAlphaCurrent += ((float)elapsed/(float)cgs.fadeDuration);
				if (cgs.fadeAlphaCurrent > cgs.fadeAlpha)
					cgs.fadeAlphaCurrent = cgs.fadeAlpha;
			}
		}
	}

	// OSP - ugh, have to inform the ui that we need to remain blacked out (or not)
	if(int_ui_blackout.integer == 0) {
		if(cg.mvTotalClients < 1 && cg.snap->ps.powerups[PW_BLACKOUT] > 0) {
			trap_Cvar_Set("ui_blackout", va("%d", cg.snap->ps.powerups[PW_BLACKOUT]));
		}
	} else if(cg.snap->ps.powerups[PW_BLACKOUT] == 0 || cg.mvTotalClients > 0) {
		trap_Cvar_Set("ui_blackout", "0");
	}

	// now draw the fade
	if(cgs.fadeAlphaCurrent > 0.0 || fBlackout) {
		VectorClear( col );
		col[3] = (fBlackout) ? 1.0f : cgs.fadeAlphaCurrent;
//		CG_FillRect( -10, -10, 650, 490, col );
		CG_FillRect( 0, 0, 640, 480, col );	// why do a bunch of these extend outside 640x480?

		//bani - #127 - bail out if we're a speclocked spectator with cg_draw2d = 0
		if( cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR && !cg_draw2D.integer ) {
			return;
		}

		// OSP - Show who is speclocked
		if(fBlackout) {
			int i, nOffset = 90;
			char *str, *format = "The %s team is speclocked!";
			char *teams[TEAM_NUM_TEAMS] = { "??", "AXIS", "ALLIES", "???" };
			float color[4] = { 1, 1, 0, 1 };

			for(i=TEAM_AXIS; i<=TEAM_ALLIES; i++) {
				if(cg.snap->ps.powerups[PW_BLACKOUT] & i) {
					str = va(format, teams[i]);
					CG_DrawStringExt(INFOTEXT_STARTX, nOffset, str, color, qtrue, qfalse, 10, 10, 0);
					nOffset += 12;
				}
			}
		}
	}
}



/*
==============
CG_DrawFlashZoomTransition
	hide the snap transition from regular view to/from zoomed

  FIXME: TODO: use cg_fade?
==============
*/
static void CG_DrawFlashZoomTransition(void) {
	vec4_t	color;
	float	frac;
	int		fadeTime=400;

	if (!cg.snap)
		return;


	if(cg.snap->ps.stats[STAT_STUN] && !cg.renderingThirdPerson ) {
		frac = cg.snap->ps.stats[STAT_STUN]/(float)800;
		if ( frac > 0.66f ) frac = 0.66f;
		if ( frac < 0 ) frac = 0;
		Vector4Set( color, 1.f, 0.8f, 0.75f, frac );
		CG_FillRect( -10, -10, 650, 490, color );
	}

	if( BG_PlayerMounted( cg.snap->ps.eFlags ) ) {
		// don't draw when on mg_42
		// keep the timer fresh so when you remove yourself from the mg42, it'll fade
		cg.zoomTime = cg.time;
		return;
	}

	if ( cg.renderingThirdPerson )
		return;


	frac = cg.time - cg.zoomTime;

	if(frac < fadeTime) {
		frac = frac/(float)fadeTime;
		Vector4Set( color, 0, 0, 0, 1.0f - frac );
		CG_FillRect( -10, -10, 650, 490, color );
	}
}



/*
=================
CG_DrawFlashDamage
=================
*/
static void CG_DrawFlashDamage( void ) {
	vec4_t		col;
	float		redFlash;

	if (!cg.snap)
		return;

	if (cg.v_dmg_time > cg.time) {
		redFlash = fabs(cg.v_dmg_pitch * ((cg.v_dmg_time - cg.time) / DAMAGE_TIME));

		// blend the entire screen red
		if (redFlash > 5)
			redFlash = 5;

		VectorSet( col, 0.2, 0, 0 );
		col[3] =  0.7 * (redFlash/5.0) * ((cg_bloodFlash.value > 1.0) ? 1.0 :
												(cg_bloodFlash.value < 0.0) ? 0.0 :
																			  cg_bloodFlash.value);

		CG_FillRect( -10, -10, 650, 490, col );
	}
}


/*
=================
CG_DrawFlashFire
=================
*/
static void CG_DrawFlashFire( void ) {
	vec4_t		col={1,1,1,1};
	float		alpha, max, f;

	if (!cg.snap)
		return;

	if ( cg.renderingThirdPerson ) {
		return;
	}

	if (!cg.snap->ps.onFireStart) {
		cg.v_noFireTime = cg.time;
		return;
	}

	alpha = (float)((FIRE_FLASH_TIME-1000) - (cg.time - cg.snap->ps.onFireStart))/(FIRE_FLASH_TIME-1000);
	if (alpha > 0) {
		if (alpha >= 1.0) {
			alpha = 1.0;
		}

		// fade in?
		f = (float)(cg.time - cg.v_noFireTime)/FIRE_FLASH_FADEIN_TIME;
		if (f >= 0.0 && f < 1.0)
			alpha = f;

		max = 0.5 + 0.5*sin((float)((cg.time/10)%1000)/1000.0);
		if (alpha > max)
			alpha = max;
		col[0] = alpha;
		col[1] = alpha;
		col[2] = alpha;
		col[3] = alpha;
		trap_R_SetColor( col );
		CG_DrawPic( -10, -10, 650, 490, cgs.media.viewFlashFire[(cg.time/50)%16] );
		trap_R_SetColor( NULL );

		trap_S_AddLoopingSound( cg.snap->ps.origin, vec3_origin, cgs.media.flameSound, (int)(255.0*alpha), 0 );
		trap_S_AddLoopingSound( cg.snap->ps.origin, vec3_origin, cgs.media.flameCrackSound, (int)(255.0*alpha), 0 );
	} else {
		cg.v_noFireTime = cg.time;
	}
}

static void CG_DrawFlashPoison( void ) {

	if (!cg.snap)
		return;

	if (cg.snap->ps.eFlags & EF_POISONED) {
		CG_DrawPic( 0, 0, 640, 480, cgs.media.poisonOverlay );
	}
}


/*
==============
CG_DrawFlashBlendBehindHUD
	screen flash stuff drawn first (on top of world, behind HUD)
==============
*/
static void CG_DrawFlashBlendBehindHUD(void) {
	CG_DrawFlashZoomTransition();
	CG_DrawFlashFade();
}


/*
=================
CG_DrawFlashBlend
	screen flash stuff drawn last (on top of everything)
=================
*/
static void CG_DrawFlashBlend( void ) {
	// Gordon: no flash blends if in limbo or spectator, and in the limbo menu
	if( (cg.snap->ps.pm_flags & PMF_LIMBO || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) && cg.showGameView ) {
		return;
	}

	CG_DrawFlashFire();
	CG_DrawFlashDamage();
	CG_DrawFlashPoison();
}

// NERVE - SMF
/*
=================
CG_DrawObjectiveInfo
=================
*/
#define OID_LEFT	10
#define OID_TOP		360

void CG_ObjectivePrint( const char *str, int charWidth ) {
	char	*s;
	int		i, len;						// NERVE - SMF
	qboolean neednewline = qfalse;		// NERVE - SMF

	if( cg.centerPrintTime ) {
		return;
	}

	s = CG_TranslateString( str );

	Q_strncpyz( cg.oidPrint, s, sizeof(cg.oidPrint) );

	// NERVE - SMF - turn spaces into newlines, if we've run over the linewidth
	len = strlen( cg.oidPrint );
	for ( i = 0; i < len; i++ ) {

		// NOTE: subtract a few chars here so long words still get displayed properly
		if ( i % ( CP_LINEWIDTH - 20 ) == 0 && i > 0 )
			neednewline = qtrue;
		if ( cg.oidPrint[i] == ' ' && neednewline ) {
			cg.oidPrint[i] = '\n';
			neednewline = qfalse;
		}
	}
	// -NERVE - SMF

	cg.oidPrintTime = cg.time;
	cg.oidPrintY = OID_TOP;
	cg.oidPrintCharWidth = charWidth;

	// count the number of lines for oiding
	cg.oidPrintLines = 1;
	s = cg.oidPrint;
	while( *s ) {
		if (*s == '\n')
			cg.oidPrintLines++;
		s++;
	}
}

static void CG_DrawObjectiveInfo( void ) {
	char	*start;
	int		l;
	int		x, y, w,h;
	int		x1, y1, x2, y2;
	float	*color;
	vec4_t	backColor;
	backColor[0] = 0.2f;
	backColor[1] = 0.2f;
	backColor[2] = 0.2f;
	backColor[2] = 1.f;

	if ( !cg.oidPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.oidPrintTime, 250 );
	if ( !color ) {
		cg.oidPrintTime = 0;
		return;
	}

	trap_R_SetColor( color );

	start = cg.oidPrint;

// JPW NERVE
	//	y = cg.oidPrintY - cg.oidPrintLines * BIGCHAR_HEIGHT / 2;
	y = 400 - cg.oidPrintLines*BIGCHAR_HEIGHT/2; 

	x1 = 319;
	y1 = y - 2;
	x2 = 321;
// jpw

	// first just find the bounding rect
	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < CP_LINEWIDTH; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.oidPrintCharWidth * CG_DrawStrlen( linebuffer ) + 10;
// JPW NERVE
		if (320 - w/2 < x1) {
			x1 = 320 - w/2;
			x2 = 320 + w/2;
		}

/*
		if ( x1 + w > x2 )
			x2 = x1 + w;
*/
		x = 320 - w/2;
// jpw
		y += cg.oidPrintCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	x2 = x2 + 4;
	y2 = y - cg.oidPrintCharWidth * 1.5 + 4;

	h = y2 - y1; // JPW NERVE

	VectorCopy( color, backColor );
	backColor[3] = 0.5 * color[3];
	trap_R_SetColor( backColor );

	CG_DrawPic( x1, y1, x2 - x1, y2 - y1, cgs.media.teamStatusBar );

	VectorSet( backColor, 0, 0, 0 );
	CG_DrawRect( x1, y1, x2 - x1, y2 - y1, 1, backColor );

	trap_R_SetColor( color );

	// do the actual drawing
	start = cg.oidPrint;
//	y = cg.oidPrintY - cg.oidPrintLines * BIGCHAR_HEIGHT / 2;
	y = 400 - cg.oidPrintLines*BIGCHAR_HEIGHT/2; // JPW NERVE


	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < CP_LINEWIDTH; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.oidPrintCharWidth * CG_DrawStrlen( linebuffer );
		if ( x1 + w > x2 )
			x2 = x1 + w;

		x = 320 - w/2; // JPW NERVE

		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue,
			cg.oidPrintCharWidth, (int)(cg.oidPrintCharWidth * 1.5), 0 );

		y += cg.oidPrintCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}
// jaquboss - from RTCW - todo sort the stupid coloring it had
void CG_DrawObjectiveIcons() {
	float x, y, w, h;// JPW NERVE
	const char *s, *s2, *buf;
	int i, num;
	vec4_t hcolor = { 0.2f, 0.2f, 0.2f, 1.f };

	x = 5;
	y = 68;
	w = 24;
	h = 14;

	s = CG_ConfigString( CS_MULTI_INFO );
	buf = Info_ValueForKey( s, "numobjectives" );

	if ( buf && atoi( buf ) ) { // if text
		num = atoi( buf );

		VectorSet( hcolor, 0.3f, 0.3f, 0.3f );
		hcolor[3] = 0.7f*cg_HUDAlpha.value; // JPW NERVE
		CG_DrawRect( x-1, y-1, w+2, ( h + 4 ) * num - 4 + 2, 1, hcolor );

		VectorSet( hcolor, 1.0f, 1.0f, 1.0f );
		hcolor[3] = 0.2f * cg_HUDAlpha.value; // JPW NERVE
		trap_R_SetColor( hcolor );
		CG_DrawPic( x, y, w , ( h + 4 ) * num - 4, cgs.media.teamStatusBar );
		trap_R_SetColor( NULL );

		s2 = CG_ConfigString( CS_MULTI_OBJECTIVE );

		for ( i = 0; i < num; i ++ ) { // loop
			int axisStatus=0;
			int alliedStatus=0;

			axisStatus = atoi(Info_ValueForKey( s2, va( "x%i", i+1 )));
			alliedStatus = atoi(Info_ValueForKey( s2, va( "a%i", i+1 )));

			trap_R_SetColor( HUD_Background );
			CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );

			VectorSet( hcolor, 1, 1, 1 );
			hcolor[3] = 0.7f; 
			trap_R_SetColor( hcolor );

			if ( axisStatus == 1 && alliedStatus == 1) {
				CG_DrawPic( x, y, w, h, trap_R_RegisterShaderNoMip( "ui/assets/gerusa_flag.tga" ) );
			}
			else if ( axisStatus == 1  ) {
				CG_DrawPic( x, y, w, h, trap_R_RegisterShaderNoMip( "ui/assets/ger_flag.tga" ) );
			}
			else if ( alliedStatus == 1 ) {
				CG_DrawPic( x, y, w, h, trap_R_RegisterShaderNoMip( "ui/assets/usa_flag.tga" ) );
			}

			y += h + 4;
		} // loop
	} // text
			trap_R_SetColor( NULL );
}
// -NERVE - SMF

//==================================================================================

void CG_DrawTimedMenus() {
	if (cg.voiceTime) {
		int t = cg.time - cg.voiceTime;
		if ( t > 2500 ) {
			Menus_CloseByName("voiceMenu");
			trap_Cvar_Set("cl_conXOffset", "0");
			cg.voiceTime = 0;
		}
	}
}

/*
=================
CG_Fade
=================
*/
void CG_Fade( int r, int g, int b, int a, int time, int duration ) {

	// incorporate this into the current fade scheme

	cgs.fadeAlpha = (float)a / 255.0f;
	cgs.fadeStartTime = time;
	cgs.fadeDuration = duration;

	if (cgs.fadeStartTime + cgs.fadeDuration <= cg.time) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	}
	return;


	if ( time <= 0 ) {	// do instantly
		cg.fadeRate = 1;
		cg.fadeTime = cg.time - 1;	// set cg.fadeTime behind cg.time so it will start out 'done'
	} else {
		cg.fadeRate = 1.0f / time;
		cg.fadeTime = cg.time + time;
	}

	cg.fadeColor2[ 0 ] = ( float )r / 255.0f;
	cg.fadeColor2[ 1 ] = ( float )g / 255.0f;
	cg.fadeColor2[ 2 ] = ( float )b / 255.0f;
	cg.fadeColor2[ 3 ] = ( float )a / 255.0f;
}

/*
=================
CG_ScreenFade
=================
*/
static void CG_ScreenFade( void ) {
	int		msec;
	int		i;
	float	t, invt;
	vec4_t	color;

	if ( !cg.fadeRate ) {
		return;
	}

	msec = cg.fadeTime - cg.time;
	if ( msec <= 0 ) {
		cg.fadeColor1[ 0 ] = cg.fadeColor2[ 0 ];
		cg.fadeColor1[ 1 ] = cg.fadeColor2[ 1 ];
		cg.fadeColor1[ 2 ] = cg.fadeColor2[ 2 ];
		cg.fadeColor1[ 3 ] = cg.fadeColor2[ 3 ];

		if ( !cg.fadeColor1[ 3 ] ) {
			cg.fadeRate = 0;
			return;
		}

		CG_FillRect( 0, 0, 640, 480, cg.fadeColor1 );

	} else {
		t = ( float )msec * cg.fadeRate;
		invt = 1.0f - t;

		for( i = 0; i < 4; i++ ) {
			color[ i ] = cg.fadeColor1[ i ] * t + cg.fadeColor2[ i ] * invt;
		}

		if ( color[ 3 ] ) {
			CG_FillRect( 0, 0, 640, 480, color );
		}
	}
}


/*
=================
CG_DrawCompassIcon

NERVE - SMF
=================
*/
void CG_DrawCompassIcon( float x, float y, float w, float h, vec3_t origin, vec3_t dest, qhandle_t shader, float dstScale, int baseSize ) {
	float angle, pi2 = M_PI * 2;
	vec3_t v1, angles;
	float len;

	VectorCopy( dest, v1 );
	VectorSubtract( origin, v1, v1 );
	len = VectorLength( v1 );
	VectorNormalize( v1 );
	vectoangles( v1, angles );

	if ( v1[0] == 0 && v1[1] == 0 && v1[2] == 0 )
		return;

	angles[YAW] = AngleSubtract( cg.predictedPlayerState.viewangles[YAW], angles[YAW] );

	angle = ( ( angles[YAW] + 180.f ) / 360.f - ( 0.50 / 2.f ) ) * pi2;


	w /= 2;
	h /= 2;

	x += w;
	y += h;

	w = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

	x = x + ( cos( angle ) * w );
	y = y + ( sin( angle ) * w );

	len = 1 - min( 1.f, len / (2000.f*dstScale) );


	CG_DrawPic( x - (baseSize * len + 4)/2, y - (baseSize * len + 4)/2, baseSize * len + 8, baseSize * len + 8, shader );
#ifdef SQUARE_COMPASS
	} else {
		int iconWidth, iconHeight;
		// START Mad Doc - TDF
		// talk about fitting a square peg into a round hole...
		// we're now putting the compass icons around the square automap instead of the round compass

		while (angle < 0)
			angle += pi2;

		while (angle >= pi2)
			angle -= pi2;


		x = x + w/2;
		y = y + h/2;
		w /= 2;// = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

		if ( (angle >= 0) && (angle < M_PI/4.0))
		{
			x += w;
			y += w * tan(angle);

		}
		else if ( (angle >= M_PI/4.0) && (angle < 3.0 * M_PI / 4.0) )
		{
			x += w / tan(angle);
			y += w;
		}
		else if ( (angle >= 3.0 * M_PI / 4.0) && (angle < 5.0 * M_PI / 4.0) )
		{
			x -= w;
			y -= w * tan(angle);
		}
		else if ( (angle >= 5.0 * M_PI / 4.0) && (angle < 7.0 * M_PI / 4.0) )
		{
			x -= w / tan(angle);
			y -= w;
		}
		else
		{
			x += w;
			y += w * tan(angle);

		}

		len = 1 - min( 1.f, len / 2000.f );
		iconWidth = 14 * len + 4; // where did this calc. come from?
		iconHeight = 14 * len + 4; 

		// adjust so that icon is always outside of the map
		if ( (angle > 5.0*M_PI/4.0) && (angle < 2*M_PI) )
		{

			y -= iconHeight;
		} 

		if ((angle >= 3.0*M_PI/4.0) && (angle <= 5.0*M_PI/4.0) )
		{
			x -= iconWidth;
		}
	
		
		CG_DrawPic( x, y, iconWidth, iconHeight, shader );


		// END Mad Doc - TDF

	}
#endif
}

/*
=================
CG_DrawNewCompass
=================
*/
static void CG_DrawNewCompass( void ) {
	float basex, basey;
	float basew, baseh;
	snapshot_t	*snap;
	float angle;
	int i;
	static float lastangle = 0;
	static float anglespeed = 0;
	float diff;

	if ( cg.nextSnap && !cg.nextFrameTeleport && !cg.thisFrameTeleport ) {
		snap = cg.nextSnap;
	} else {
		snap = cg.snap;
	}

	// Arnout: bit larger
	basex = 520 - 16;
	basey = 20 - 16;
	basew = 100 + 32;
	baseh = 100 + 32;

	CG_DrawAutoMap();

	if( cgs.autoMapExpanded ) {
		if( cg.time - cgs.autoMapExpandTime < 100.f ) {
			basey -= ( ( cg.time - cgs.autoMapExpandTime ) / 100.f ) * 128.f;
		} else {
			//basey -= 128.f;
			return;
		}
	} else {
		if( cg.time - cgs.autoMapExpandTime <= 150.f ) {
			//basey -= 128.f;
			return;
		} else if( ( cg.time - cgs.autoMapExpandTime > 150.f ) && ( cg.time - cgs.autoMapExpandTime < 250.f ) ) {

			basey = ( basey - 128.f ) + ( ( cg.time - cgs.autoMapExpandTime - 150.f ) / 100.f ) * 128.f;
		} /*else {
			rectDef_t compassHintRect =	{ 640 - 22, 128, 20, 20 };

			CG_DrawKeyHint( &compassHintRect, "+mapexpand" );
		}*/ // jaquboss not needed
	}


	CG_DrawPic( basex + 4, basey + 4, basew - 8, baseh - 8, cgs.media.compassShader );

	angle = ( cg.predictedPlayerState.viewangles[YAW] + 180.f ) / 360.f - ( 0.125f );	
	diff = AngleSubtract( angle * 360, lastangle * 360 ) / 360.f;
	anglespeed /= 1.08f;
	anglespeed += diff * 0.01f;
	if( Q_fabs(anglespeed) < 0.00001f ) {
		anglespeed = 0;
	}
	lastangle += anglespeed;
	CG_DrawRotatedPic( basex + 4, basey + 4, basew - 8, baseh - 8, cgs.media.compass2Shader, lastangle );

	// draw voice chats
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		centity_t *cent = &cg_entities[i];

		if ( cg.predictedPlayerState.clientNum == i || !cgs.clientinfo[i].infoValid || cg.predictedPlayerState.persistant[PERS_TEAM] != cgs.clientinfo[i].team )
			continue;

		// also draw revive icons if cent is dead and player is a medic
		if ( cent->voiceChatSpriteTime < cg.time ) {
			continue;
		}

		if ( cgs.clientinfo[i].health <= 0 ) {
			// reset
			cent->voiceChatSpriteTime = cg.time;
			continue;
		}

		CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->lerpOrigin, cent->voiceChatSprite, 1.f, 14 );
	}

	// draw revive medic icons
	if ( cg.predictedPlayerState.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
		for ( i = 0; i < snap->numEntities; i++ ) {
			entityState_t *ent = &snap->entities[i];

			if ( ent->eType != ET_PLAYER )
				continue;

			if ( ( ent->eFlags & EF_DEAD ) && ent->number == ent->clientNum ) {
				if ( !cgs.clientinfo[ent->clientNum].infoValid || cg.predictedPlayerState.persistant[PERS_TEAM] != cgs.clientinfo[ent->clientNum].team )
					continue;

				CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, ent->pos.trBase, cgs.media.medicReviveShader, 1.f, 14 );
			}
		}
	}

	/*if ( cg.predictedPlayerState.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
		for ( i = 0; i < snap->numEntities; i++ ) {
			centity_t *cent = &cg_entities[ snap->entities[ i ].number ];

			if ( cent->currentState.eType != ET_EXPLOSIVE_INDICATOR ) {
				continue;
			}

			if ( cent->currentState.teamNum == 1 && cg.predictedPlayerState.persistant[PERS_TEAM] == TEAM_AXIS )
				continue;
			else if ( cent->currentState.teamNum == 2 && cg.predictedPlayerState.persistant[PERS_TEAM] == TEAM_ALLIES )
				continue;

			CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->lerpOrigin, cgs.media.compassDestroyShader );
		}
	}*/

	/*for ( i = 0; i < snap->numEntities; i++ ) {
		centity_t *cent = &cg_entities[ snap->entities[ i ].number ];

		if( cent->currentState.eType != ET_WAYPOINT ) {
			continue;
		}

		// see if the waypoint owner is someone that you accept waypoints from
		if( !CG_IsOnSameFireteam( cg.clientNum, cent->currentState.clientNum )) {  // TODO: change to fireteam
				continue;
		}

		switch( cent->currentState.frame ) {
		case WAYP_ATTACK: CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->currentState.pos.trBase, cgs.media.waypointCompassAttackShader, 1.f, 14 ); break;
		case WAYP_DEFEND: CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->currentState.pos.trBase, cgs.media.waypointCompassDefendShader, 1.f, 14 ); break;
		case WAYP_REGROUP: CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, cent->currentState.pos.trBase, cgs.media.waypointCompassRegroupShader, 1.f, 14 ); break;
		}
	}*/

	for ( i = 0; i < snap->numEntities; i++ ) {
		entityState_t *ent = &snap->entities[i];

		if ( ent->eType != ET_PLAYER ) {
			continue;
		}

		if ( ent->eFlags & EF_DEAD ) {
			continue;
		}

		if ( !cgs.clientinfo[ent->clientNum].infoValid || cg.predictedPlayerState.persistant[PERS_TEAM] != cgs.clientinfo[ent->clientNum].team ) {
			continue;
		}
			
		if(!CG_IsOnSameFireteam( cg.clientNum, ent->clientNum )) {
			continue;
		}

		CG_DrawCompassIcon( basex, basey, basew, baseh, cg.predictedPlayerState.origin, ent->pos.trBase, cgs.media.buddyShader, 1.f, 14 );
	}
}

static int CG_PlayerAmmoValue( int *ammo, int *clips, int *akimboammo ) {
	centity_t		*cent;
	playerState_t	*ps;
	int				weap;
	qboolean		skipammo = qfalse;

	*ammo = *clips = *akimboammo = -1;

	if( cg.snap->ps.clientNum == cg.clientNum )
		cent = &cg.predictedPlayerEntity;
	else
		cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	weap = cent->currentState.weapon;

	if ( !weap )
		return weap;

	switch(weap) {		// some weapons don't draw ammo count text
		case WP_AMMO:
		case WP_MEDKIT:
		//case WP_KNIFE:
		//case WP_KNIFE_KABAR:
		case WP_PLIERS:
		case WP_SMOKE_MARKER:
		case WP_DYNAMITE:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_SMOKE_BOMB:
		case WP_BINOCULARS:
		//case WP_FIREBOLT:
			return weap;
		case WP_KNIFE:
		case WP_KNIFE_KABAR:
			if ( !(cgs.clientinfo[cg.snap->ps.clientNum].skillBits[SK_LIGHT_WEAPONS] & (1<<8)) )
				return weap;
		case WP_LANDMINE:
		case WP_MEDIC_SYRINGE:
		case WP_MEDIC_ADRENALINE:
		case WP_POISON_SYRINGE:
		case WP_GRENADE_LAUNCHER:
		case WP_GRENADE_PINEAPPLE:
		case WP_FLAMETHROWER:
		case WP_MORTAR:
		case WP_MORTAR_SET:
		case WP_MORTAR2:
		case WP_MORTAR2_SET:
		case WP_PANZERFAUST:	
		case WP_BAZOOKA:	// jet Pilot - use clips?
			skipammo = qtrue;
			break;

		default:
			break;
	}

	if( cg.snap->ps.eFlags & EF_MG42_ACTIVE || cg.snap->ps.eFlags & EF_MOUNTEDTANK ) {
		return WP_MOBILE_MG42;
	}

	// total ammo in clips
	*clips = cg.snap->ps.ammo[BG_FindAmmoForWeapon(weap)];

	// current clip
	*ammo = ps->ammoclip[BG_FindClipForWeapon(weap)];

	if( IS_AKIMBO_WEAPON( weap ) ) {
		*akimboammo = ps->ammoclip[BG_FindClipForWeapon(BG_AkimboSidearm(weap))];
	} else {
		*akimboammo = -1;
	}

	if( weap == WP_LANDMINE ) {
		if( !cgs.gameManager ) {
			*ammo = 0;
		} else {
			if( cgs.clientinfo[ps->clientNum].team == TEAM_AXIS ) {
				*ammo = cgs.gameManager->currentState.otherEntityNum;
			} else {
				*ammo = cgs.gameManager->currentState.otherEntityNum2;
			}
		}
	} else if( weap == WP_MORTAR2 || weap == WP_MORTAR2_SET || weap == WP_MORTAR || weap == WP_MORTAR_SET || weap == WP_PANZERFAUST || weap == WP_BAZOOKA) {
		*ammo += *clips;
	}
	
	if( skipammo ) {
		*clips = -1;
	}

	return weap;
}

#define HEAD_TURNTIME 10000
#define HEAD_TURNANGLE 20
#define HEAD_PITCHANGLE 2.5
static void CG_DrawPlayerStatusHead( void ) 
{
	hudHeadAnimNumber_t anim;
	rectDef_t headRect =		{ 44, 480 - 92, 62, 80 };
	bg_character_t* character = CG_CharacterForPlayerstate( &cg.snap->ps );
	bg_character_t* headcharacter = BG_GetCharacter( cgs.clientinfo[ cg.snap->ps.clientNum ].team, cgs.clientinfo[ cg.snap->ps.clientNum ].cls );
	animation_t	animations[MAX_HD_ANIMATIONS];
	int			i, team, rank;
	int			cls = 0;
	refdef_t		refdef;
	refEntity_t		head, hat, marmor, mrank;
	vec3_t			origin, mins, maxs, angles;
	float			len, x, y, w, h;
	int				hatIndex = ACC_HAT;

	if( !character || !headcharacter ) 
		return;

	anim = cg.idleAnim;

	for ( i = 0 ; i < MAX_HD_ANIMATIONS ; i++ ) {
		animations[i] = character->hudheadanimations[i];
	}

	if( cg.weaponFireTime > 500 ) 
	{
		anim = HD_ATTACK;
	}
	else if( cg.time - cg.lastFiredWeaponTime < 500 )
	{
		anim = HD_ATTACK_END;
	} 
	else if( cg.time - cg.painTime < ( animations[ HD_PAIN ].numFrames * animations[ HD_PAIN ].frameLerp) )
	{
		anim = HD_PAIN;
	} 
	else if( cg.time > cg.nextIdleTime ) 
	{ 
		cg.nextIdleTime = cg.time + 7000 + rand() % 1000;
		if( cg.snap->ps.stats[ STAT_HEALTH ] < 40 ) 
		{
			cg.idleAnim = (rand() % (HD_DAMAGED_IDLE3 - HD_DAMAGED_IDLE2 + 1)) + HD_DAMAGED_IDLE2;
		}
		else 
		{
			cg.idleAnim = (rand() % (HD_IDLE8 - HD_IDLE2 + 1)) + HD_IDLE2;
		}

		cg.lastIdleTimeEnd = cg.time + animations[ cg.idleAnim ].numFrames * animations[ cg.idleAnim ].frameLerp;
	}


	if( cg.time > cg.lastIdleTimeEnd ) {
		if( cg.snap->ps.stats[ STAT_HEALTH ] < 40 ) {
			cg.idleAnim = HD_DAMAGED_IDLE1;
		} else {
			cg.idleAnim = HD_IDLE1;
		}
	}
	
	if ( cg.snap->ps.powerups[PW_OPS_DISGUISED] ){
		if( cg.snap->ps.powerups[PW_OPS_CLASS_1] )
			cls |= 1;
		if( cg.snap->ps.powerups[PW_OPS_CLASS_2] )
			cls |= 2;
		if( cg.snap->ps.powerups[PW_OPS_CLASS_3] )
			cls |= 4;

		team = cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
		rank = cgs.clientinfo[cg.snap->ps.clientNum].disguiseRank;
	} else {
		cls = cgs.clientinfo[cg.snap->ps.clientNum].cls;
		team = cgs.clientinfo[cg.snap->ps.clientNum].team;
		rank = cgs.clientinfo[cg.snap->ps.clientNum].rank;
	}


	if ( rank >= NUM_EXPERIENCE_LEVELS-1 && !cg.snap->ps.powerups[PW_HELMETSHIELD] )
		hatIndex = ACC_MOUTH2;

	trap_R_SaveViewParms();

	x = headRect.x;
	y = headRect.y;
	w = headRect.w;
	h = headRect.h;

	CG_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	refdef.rdflags = RDF_NOWORLDMODEL;
	AxisClear( refdef.viewaxis );

	refdef.fov_x = 8;
	refdef.fov_y = 10;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	trap_R_ClearScene();

	trap_R_ModelBounds( headcharacter->hudhead, mins, maxs );

	origin[2] = -0.7 * ( mins[2] + maxs[2] );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );

	// calculate distance so the head nearly fills the box
	// assume heads are taller than wide
	len = 3.5f * ( maxs[2] - mins[2] );		
	origin[0] = len / tan( 20 / 2 ); 

	angles[PITCH] = 0;
	angles[YAW] = 180; // todo look after hit dir?
	angles[ROLL] = 0;

	memset( &head, 0, sizeof( head ) );
	AnglesToAxis( angles, head.axis );
	VectorCopy( origin, head.origin );

	head.hModel = headcharacter->hudhead;
	head.customSkin = headcharacter->hudheadskin;
	head.renderfx = RF_NOSHADOW | RF_FORCENOLOD | RF_MINLIGHT;		// no stencil shadows

	CG_HudHeadAnimation( headcharacter, &cg.predictedPlayerEntity.pe.hudhead, &head.oldframe, &head.frame, &head.backlerp, anim, &cgs.clientinfo[cg.snap->ps.clientNum] );

	if( !(cg.snap->ps.eFlags & EF_HEADSHOT) ) 
	{
		memset( &mrank, 0, sizeof( mrank ) ); 
		memset( &hat, 0, sizeof( hat ) );
		memset( &marmor, 0, sizeof( marmor ) );
		
		hat.hModel = character->accModels[hatIndex];
		hat.customSkin = character->accSkins[hatIndex];

		hat.renderfx = RF_NOSHADOW | RF_FORCENOLOD | RF_MINLIGHT;		// no stencil shadows
		
		CG_PositionEntityOnTag( &hat, &head, "tag_mouth", 0, NULL);

		if ( cg.snap->ps.powerups[PW_HELMETSHIELD] )
		{
			marmor.hModel = character->accModels[ACC_SHIELD];
			marmor.customSkin = character->accSkins[ACC_SHIELD];
			marmor.renderfx = hat.renderfx;
			CG_PositionEntityOnTag( &marmor, &head, "tag_mouth", 0, NULL);

		} else if( rank && hatIndex == ACC_HAT  )
		{
			mrank.hModel = character->accModels[ ACC_RANK ];
			mrank.customShader = rankicons[ rank ][ team == TEAM_AXIS ? 1 : 0 ][1].shader; 
			mrank.renderfx = RF_NOSHADOW | RF_FORCENOLOD;		// no stencil shadows
			CG_PositionEntityOnTag( &mrank, &head, "tag_mouth", 0, NULL);
		}		
	}

	trap_R_AddRefEntityToScene( &head );

	if( !(cg.snap->ps.eFlags & EF_HEADSHOT) ) {
		trap_R_AddRefEntityToScene( &hat );

		if ( cg.snap->ps.powerups[PW_HELMETSHIELD] ) {
			trap_R_AddRefEntityToScene( &marmor );
		} else if( rank && hatIndex == ACC_HAT ) {
			trap_R_AddRefEntityToScene( &mrank );
		}

	}

	if( cg.snap->ps.powerups[PW_ADRENALINE] && ( cgs.adrenaline & ADF_EYEGLOW ) ) {
		head.customSkin = cgs.media.adrenalineEyesSkin;
		trap_R_AddRefEntityToScene( &head );
	}

	if( cg.snap->ps.stats[ STAT_HEALTH ] < 60 ) {
		if( cg.snap->ps.stats[ STAT_HEALTH ] < 5 ) {
			head.customSkin = cgs.media.hudDamagedStates[3];
		} else if( cg.snap->ps.stats[ STAT_HEALTH ] < 20 ) {
			head.customSkin = cgs.media.hudDamagedStates[2];
		} else if( cg.snap->ps.stats[ STAT_HEALTH ] < 40 ) {
			head.customSkin = cgs.media.hudDamagedStates[1];
		} else {
			head.customSkin = cgs.media.hudDamagedStates[0];
		}
		head.customShader = 0;
		trap_R_AddRefEntityToScene( &head );
	}

	trap_R_RenderScene( &refdef );
	trap_R_RestoreViewParms();

}

static void CG_DrawPlayerHealthBar( rectDef_t *rect ) {
	vec4_t bgcolour =	{	1.f,	1.f,	1.f,	0.3f	};
	vec4_t colour =		{	0.1f,	1.0f,	0.1f,	0.5f	};

	int flags = 1|4|16|64;
	float frac;

	if( cg.snap->ps.powerups[PW_ADRENALINE] && !(cgs.adrenaline & ADF_NOHEALTH) ) 
	{
		float msec = cg.snap->ps.powerups[PW_ADRENALINE] - cg.time;

		if( msec < 0 ) {
			msec = 0;
		} else {
			Vector4Average( colour, colorWhite, .5f + sin(.2f * sqrt(msec) * 2 * M_PI) * .5f, colour);
		}
	} 
	else 
	{
		CG_ColorForHealth( colour );
		colour[3] = 0.5f;
	}


	if( cgs.clientinfo[ cg.snap->ps.clientNum ].cls == PC_MEDIC ) {
		frac = cg.snap->ps.stats[STAT_HEALTH] / ( (float) cg.snap->ps.stats[STAT_MAX_HEALTH] * 1.12f );
	} else {
		frac = cg.snap->ps.stats[STAT_HEALTH] / (float) cg.snap->ps.stats[STAT_MAX_HEALTH];
	}


	CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, colour, NULL, bgcolour, frac, flags );

	trap_R_SetColor( NULL );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
	CG_DrawPic( rect->x, rect->y + rect->h + 4, rect->w, rect->w, cgs.media.hudHealthIcon );
}

// jet Pilot  - draw the breath bar! Thanks to bacon from the splashdamage forums for this 
static void CG_DrawBreathBar( rectDef_t *rect ) 
{ 
   vec4_t bgcolour =	{   1.f,    1.f,    1.f,    0.3f   }; 
   vec4_t colour =      {   0.1f,   0.1f,   1.0f,   0.5f   }; 
   vec4_t colourlow =   {   1.0f,   0.1f,   0.1f,   0.5f   }; 
   vec_t* color = colour; 
   int flags = 1|4|16|64; 
   float frac;
   
   	if ( cgs.clientinfo[cg.snap->ps.clientNum].skillBits[SK_BATTLE_SENSE] & (1<<6) )
		frac = (cg.waterundertime - cg.time) / (float)HOLDBREATHTIME_LONG; 
	else
		frac = (cg.waterundertime - cg.time) / (float)HOLDBREATHTIME; 

   if( frac < 0.25 ) { 
      color = colourlow; 
   } 

   CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, color, NULL, bgcolour, frac, flags ); 

   trap_R_SetColor( NULL ); 
   CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
   CG_DrawPic( rect->x, rect->y + rect->h + 4, rect->w, rect->w, cgs.media.waterHintShader );
}

static void CG_DrawStaminaBar( rectDef_t *rect ) 
{
	vec4_t bgcolour =	{	1.f,	1.f,	1.f,	0.3f	};
	vec4_t colour =		{	0.1f,	1.0f,	0.1f,	0.5f	};
	vec4_t colourlow =	{	1.0f,	0.1f,	0.1f,	0.5f	};
	vec_t* color = colour;
	int flags = 1|4|16|64;
	float frac = cg.pmext.sprintTime / (float)SPRINTTIME;

	if( cg.snap->ps.powerups[PW_ADRENALINE] && !(cgs.adrenaline & ADF_NOSTAMINA) ) 
	{
		if ( cg.snap->ps.pm_flags & PMF_FOLLOW ) 
		{
			Vector4Average( colour, colorWhite, sin(cg.time*.005f), colour);
		}
		else 
		{
			float msec = cg.snap->ps.powerups[PW_ADRENALINE] - cg.time;

			if( msec < 0 ) {
				msec = 0;
			} else {
				Vector4Average( colour, colorWhite, .5f + sin(.2f * sqrt(msec) * 2 * M_PI) * .5f, colour);
			}
		}
	} 
	else 
	{
		if( frac < 0.25 ) 
		{
			color = colourlow;
		}
	}

	CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, color, NULL, bgcolour, frac, flags );

	trap_R_SetColor( NULL );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );
	CG_DrawPic( rect->x, rect->y + rect->h + 4, rect->w, rect->w, cgs.media.hudSprintIcon);
}

qboolean CG_CheckDisguise( float frac )
{
	int		 covertopsLevel = cgs.clientinfo[cg.clientNum].skill[SK_COVERTOPS];

	switch ( cg.snap->ps.weapon )
	{
		case WP_SMOKE_BOMB:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_BINOCULARS:	
		case WP_MEDIC_ADRENALINE: 
			return qtrue;			
		default:	
			break;				
	}

	if (frac > disguiseTestCost[covertopsLevel])
		return qtrue;

	return qfalse;
}

static void CG_DrawWeapRecharge( rectDef_t *rect ) {
	float		barFrac, chargeTime;
	int			weap, flags;
	qboolean	fade = qfalse;
	vec4_t	orange = { 1.0f, 0.5f, 0.0f, 1.0f };
	vec4_t	yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	vec4_t	red = { 1.0f, 0.0f, 0.0f, 1.0f };
	vec4_t	bgcolor = { 1.0f, 1.0f, 1.0f, 0.25f };
	vec4_t	color;

	flags = 1|4|16;

	weap = cg.snap->ps.weapon;

//	if( !(cg.snap->ps.eFlags & EF_ZOOMING) ) {
//		if ( weap != WP_PANZERFAUST && weap != WP_DYNAMITE && weap != WP_MEDKIT && weap != WP_SMOKE_GRENADE && weap != WP_PLIERS && weap != WP_AMMO ) {
//			fade = qtrue;
//		}
//	}

	// Draw power bar
	if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
		chargeTime = cg.engineerChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_MEDIC ) {
		chargeTime = cg.medicChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_FIELDOPS ) {
		chargeTime = cg.ltChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else if( cg.snap->ps.stats[ STAT_PLAYER_CLASS ] == PC_COVERTOPS ) {
		chargeTime = cg.covertopsChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	} else {
		chargeTime = cg.soldierChargeTime[cg.snap->ps.persistant[PERS_TEAM]-1];
	}

	barFrac = (float)(cg.time - cg.snap->ps.classWeaponTime) / chargeTime;
	if( barFrac > 1.0 ) {
		barFrac = 1.0;
	}

	color[0] = 1.0f;
	color[1] = color[2] = barFrac;
	color[3] = 0.25 + barFrac*0.5;

	if ( fade ) {
		bgcolor[3] *= 0.4f;
		color[3] *= 0.4;
	}

// jaquboss -  we can't use pm, that's sad.. 
	{
	 int weapon;
	 qboolean charge=qtrue;

		if ( cg.snap->ps.weapon == WP_MORTAR2 || cg.snap->ps.weapon == WP_MORTAR || cg.snap->ps.weapon == WP_CARBINE || cg.snap->ps.weapon == WP_KAR98 ) // display alt charge instead
		{
			weapon = weapAlts[cg.snap->ps.weapon];
		} else {
			weapon = cg.snap->ps.weapon;
		}

		if ((cg.time - cg.snap->ps.classWeaponTime) < (chargeTime * BG_ChargeUsage(weapon, cgs.clientinfo[cg.snap->ps.clientNum].skillBits)) ) 
		{ // red like a soviet
			color[0] = 1;
			color[1] = color[2] = 0; 
			color[3] = 1;
			charge = qfalse;
		}
		if (!CG_CheckDisguise( barFrac ) && cg.snap->ps.powerups[PW_OPS_DISGUISED] )
		{
			if ( charge )
			color[0] = 0;

			color[2] = 1; // blue..
			color[1] = 0; 
			color[3] = 1;
		}

	}
	

	CG_FilledBar( rect->x, rect->y + (rect->h * 0.1f), rect->w, rect->h * 0.84f, color, NULL, bgcolor, barFrac, flags );

	trap_R_SetColor( NULL );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.hudSprintBar );

	if ( cg.snap->ps.ammo[WP_ARTY] & NO_AIRSTRIKE )
	{
		if ( cg.snap->ps.ammo[WP_ARTY] & NO_ARTY )
			trap_R_SetColor( red );
		else 
			trap_R_SetColor( yellow );
	} else if ( cg.snap->ps.ammo[WP_ARTY] & NO_ARTY ) {
			trap_R_SetColor( orange );
	}

	CG_DrawPic( rect->x + (rect->w * 0.25f) - 1, rect->y + rect->h + 4, (rect->w * 0.5f) + 2, rect->w + 2, cgs.media.hudPowerIcon );

	if ( cg.snap->ps.ammo[WP_ARTY] != 0 )
	trap_R_SetColor( NULL );
}

extern pmove_t *pm;

static void CG_DrawPlayerStatus( void ) 
{
	int				value, value2, value3;
	char			buffer[32];
	int				weap;
	playerState_t	*ps;
	rectDef_t		rect;
//	vec4_t			colorFaded = { 1.f, 1.f, 1.f, 0.3f };
	pmove_t			*pm;


	pm = &cg_pmove;

	ps = &cg.snap->ps;
	
	// Draw weapon icon and overheat bar
	rect.x = 640 - 82;
	rect.y = 480 - 56;
	rect.w = 60;
	rect.h = 32;
	CG_DrawWeapHeat( &rect, HUD_HORIZONTAL );
	if( cg.mvTotalClients < 1 && cg_drawWeaponIconFlash.integer == 0 ) {
		CG_DrawPlayerWeaponIcon(&rect, qtrue, ITEM_ALIGN_RIGHT, &colorWhite);
	} else {
		int ws = (cg.mvTotalClients > 0) ? cgs.clientinfo[cg.snap->ps.clientNum].weaponState : BG_simpleWeaponState(cg.snap->ps.weaponstate);
		CG_DrawPlayerWeaponIcon(&rect, (ws != WSTATE_IDLE), ITEM_ALIGN_RIGHT, ((ws == WSTATE_SWITCH) ? &colorWhite : (ws == WSTATE_FIRE) ? &colorRed : &colorYellow));
	}

	// Draw ammo
	weap = CG_PlayerAmmoValue( &value, &value2, &value3 );
	if( value3 >= 0 ) {
		Com_sprintf( buffer, sizeof(buffer), "%i|%i/%i", value3, value, value2 );
		CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, &cgs.media.limboFont1 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 );
//		CG_DrawPic( 640 - 2 * ( 12 + 2 ) - 16 - 4, 480 - 1 * ( 16 + 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
	} else if( value2 >= 0 ) {
		Com_sprintf( buffer, sizeof(buffer), "%i/%i", value, value2 );
		CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, &cgs.media.limboFont1 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 );
//		CG_DrawPic( 640 - 2 * ( 12 + 2 ) - 16 - 4, 480 - 1 * ( 16 + 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
	} else if( value >= 0 ) {
		Com_sprintf( buffer, sizeof(buffer), "%i", value );
		CG_Text_Paint_Ext( 640 - 22 - CG_Text_Width_Ext( buffer, .25f, 0, &cgs.media.limboFont1 ), 480 - 1 * ( 16 + 2 ) + 12 - 4, .25f, .25f, colorWhite, buffer, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 );
//		CG_DrawPic( 640 - 2 * ( 12 + 2 ) - 16 - 4, 480 - 1 * ( 16 + 2 ) - 4, 16, 16, cgs.media.SPPlayerInfoAmmoIcon );
	}


// ==
	rect.x = 24;
	rect.y = 480 - 92;
	rect.w = 12;
	rect.h = 72;
	CG_DrawPlayerHealthBar( &rect );
// ==

// ==
	rect.x = 4;
	rect.y = 480 - 92;
	rect.w = 12;
	rect.h = 72;
	if ( BG_GetConditionValue( cg.snap->ps.clientNum, ANIM_COND_UNDERWATER, qtrue ) ) // jaquboss - looks weird but it is only place we know for waterlevel
	{
		CG_DrawBreathBar( &rect );	
	}
	else
	{							   
		CG_DrawStaminaBar( &rect );
	}

// ==

// ==
	rect.x = 640 - 16;
	rect.y = 480 - 92;
	rect.w = 12;
	rect.h = 72;
	CG_DrawWeapRecharge( &rect );
// ==
}
/*
static void CG_DrawSkillBar( float x, float y, float w, float h, int skill ) {
	int i;
	int skillMax;
	float blockheight; //= ( h - 4 ) / (float)(NUM_SKILL_LEVELS - 1);
	float draw_y;
	vec4_t colour;
	float x1, y1, w1, h1;

	
	// jet Pilot - Draw Skill Bar

	blockheight = ( h - 4 ) / (float)(skillMax - 1);
	
	draw_y = y + h - blockheight;


//	for( i = 0; i < NUM_SKILL_LEVELS - 1; i++ ) {
	for( i = 0; i < skillMax; i++ ) {
		if( i >= skill ) {
			Vector4Set( colour, 1.f, 1.f, 1.f, .15f );
		} else {
			Vector4Set( colour, 0.f, 0.f, 0.f, .4f );
		}

		CG_FillRect( x, draw_y, w, blockheight, colour );

		if( i < skill ) {
			x1 = x;
			y1 = draw_y;
			w1 = w;
			h1 = blockheight;
			CG_AdjustFrom640( &x1, &y1, &w1, &h1 );

			trap_R_DrawStretchPic( x1, y1, w1, h1, 0, 0, 1.f, 0.5f, cgs.media.limboStar_roll );
		}

		CG_DrawRect_FixedBorder( x, draw_y, w, blockheight, 1, colorBlack );
//		CG_DrawPic( x, draw_y, w, blockheight, cgs.media.hudBorderVert2 );
		draw_y -= ( blockheight + 1 );
	}
}
*/
#define SKILL_ICON_SIZE		14

#define SKILLS_X 112
#define SKILLS_Y 20

#define SKILL_BAR_OFFSET	(2*SKILL_BAR_X_INDENT)
#define SKILL_BAR_X_INDENT	0
#define SKILL_BAR_Y_INDENT	6

#define SKILL_BAR_WIDTH		( SKILL_ICON_SIZE - SKILL_BAR_OFFSET )
#define SKILL_BAR_X			( SKILL_BAR_OFFSET + SKILL_BAR_X_INDENT + SKILLS_X )
#define SKILL_BAR_X_SCALE	( SKILL_ICON_SIZE + 2 )
#define SKILL_ICON_X		( SKILL_BAR_OFFSET + SKILLS_X )
#define SKILL_ICON_X_SCALE	( SKILL_ICON_SIZE + 2 )
#define SKILL_BAR_Y			( SKILL_BAR_Y_INDENT - SKILL_BAR_OFFSET - SKILLS_Y )
#define SKILL_BAR_Y_SCALE	( SKILL_ICON_SIZE + 2 )
#define SKILL_ICON_Y		(- ( SKILL_ICON_SIZE + 2 ) - SKILL_BAR_OFFSET - SKILLS_Y )

skillType_t CG_ClassSkillForPosition( clientInfo_t* ci, int pos ) {
	switch( pos ) {
		case 0:
			return BG_ClassSkillForClass(ci->cls);
		case 1:
			return SK_BATTLE_SENSE;
		case 2:
			// jet Pilot - draw soldier level if using a heavy weapon instead of light weapons icon
			if ((pm && ( pm->ps->persistant[PERS_HWEAPON_USE] || pm->ps->eFlags & EF_MOUNTEDTANK) ) && ci->cls != PC_SOLDIER) // jaquboss - checking for pm
				return SK_SOLDIER;			
			return SK_LIGHT_WEAPONS;
	}

	return SK_BATTLE_SENSE;
}

/* CG_DrawSkillLevels() 

Draws active powers the player currently has

*/

void CG_DrawPlayerRank()
{
	playerState_t	*ps;
	clientInfo_t	*ci;
	
	ps = &cg.snap->ps;
	ci = &cgs.clientinfo[ ps->clientNum ];	

	CG_FillRect(hud_rankX.integer, hud_rankY.integer, 32, 32, HUD_Background );
	CG_DrawRect_FixedBorder(hud_rankX.integer, hud_rankY.integer, 32, 32, 1, HUD_Border );

	// fixme not showing up
	if (ci->rank > 0) 
        CG_DrawPic(hud_rankX.integer, hud_rankY.integer, 48, 48, rankicons[ ci->rank ][  ci->team == TEAM_AXIS ? 1 : 0 ][0].shader);
}
//extern int skillLevels[];
void CG_DrawSkillLevels()
{
	playerState_t	*ps;
	clientInfo_t	*ci;
	skillType_t		skill;	
	int				i;
	int				x, y;
	float			prog;						// progress toward next level
	int				need, have;	
/*	int				nextLvl[NUM_SKILL_LEVELS] = { 20, 50, 90, 140, 200, 270, 350, 440, 540, -1 };
	int				curLvl[NUM_SKILL_LEVELS]  = {  0, 20, 50,  90, 140, 200, 270, 350, 440, 540 };
*/
	const char		*str;
//	float			w;
//	vec_t*			clr;
	//static int		lastXP = 0;			// Jaybird - cleaning warnings
	//static qboolean	statsUD = qfalse;	// Jaybird - cleaning warnings

	x  = hud_skillBarX.integer;
	y  = hud_skillBarY.integer;

	ps = &cg.snap->ps;
	ci = &cgs.clientinfo[ ps->clientNum ];

	// draw player's health
	str = va( "HP %i", cg.snap->ps.stats[STAT_HEALTH] );
	CG_Text_Paint_Ext( x, 480 - 4, 0.25f, 0.25f, colorWhite, str,  0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 );

	// no xp aquired in LMS games, nothing else to do here
	if (cgs.gametype == GT_WOLF_LMS) return;
	
	// update the individual skill xp counts if xp has changed since last frame
	//  jaquboss, this does problems would be better to do it in diffirent way
/*	if( lastXP != cg.snap->ps.stats[STAT_XP] && cgs.gamestats.requestTime < cg.time)
	{
		cgs.gamestats.requestTime = cg.time + 4000;
		trap_SendClientCommand(va("sgstats %d", cg.snap->ps.clientNum));
	}
*/	
	// draw the player's total xp, yellow if it changed in the last second, white otherwise
	//clr = (cg.time - cg.xpChangeTime < 1000 ) ? colorYellow : colorWhite;

	str = va( "XP %i", cg.snap->ps.stats[STAT_XP] );
	CG_Text_Paint_Ext( x + 80 - 110 , 480 - 4 - 95, 0.25f, 0.25f, (cg.time - cg.xpChangeTime < 1000 ) ? colorYellow : colorWhite, str, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 ); // jaquboss - moved a bit on Meyerinchains request
	
	// draw background & border 
	CG_FillRect(            x,  y, 36, 72, HUD_Background );		
	CG_DrawRect_FixedBorder(x,	y, 36, 72, 1, HUD_Border );

	if (ci->rank > 0)		// jP - include rank icon if rank is > 0
	{
		CG_DrawPic(x + 4, y, 28, 28, rankicons[ ci->rank ][ ci->team == TEAM_AXIS ? 1 : 0 ][0].shader);
	}	
	y += 26;
	
//	else
//	{
//		CG_FillRect(x, y, 36, 46, HUD_Background );
//		CG_DrawRect_FixedBorder(x, y, 36, 46, 1, HUD_Border );
//	}

	// jP - draw skill levels and progress bars
	for (i = 0; i < 3; i++)
	{
		int	skillpoints;	

		skill = CG_ClassSkillForPosition( ci, i );

		switch ( skill ){
		case SK_BATTLE_SENSE:
			skillpoints = cg.snap->ps.ammo[VERYBIGEXPLOSION];
		break;
		case SK_LIGHT_WEAPONS:
			skillpoints = cg.snap->ps.ammo[WP_DUMMY_MG42];
		break;
		case SK_HEAVY_WEAPONS:
			skillpoints = cg.snap->ps.ammoclip[VERYBIGEXPLOSION];
		break;
		default:
			skillpoints = cg.snap->ps.ammoclip[WP_DUMMY_MG42];
			break;
		}


		if (ci->skill[skill] < NUM_SKILL_LEVELS - 1 && !cg.xpOrder )
		{
			need = skillLevels[skill][ci->skill[skill]+1] - skillLevels[skill][ci->skill[skill]];//nextLvl[ci->skill[skill]] - curLvl[ci->skill[skill]];
			have = skillpoints - skillLevels[skill][ci->skill[skill]];//curLvl[ci->skill[skill]];
			prog = (float)have / need;
			
			CG_FilledBar(x + 1, y + 6 +(i * (SKILL_ICON_SIZE)), 34, 8, colorRed, colorGreen, NULL, prog, 256 ); //BAR_LERP_COLOR
			CG_DrawRect_FixedBorder(x + 1, y+ 6 +(i * (SKILL_ICON_SIZE)), 34, 8, 1, HUD_Border );
		}


	//	CG_Text_Paint(x + 4, y+2 + (i * (SKILL_ICON_SIZE)),.19f, colorWhite, va("%f", prog), 0, 0, ITEM_TEXTSTYLE_SHADOWED); 
		CG_DrawPic(x + 4,  y + 2 + (i * (SKILL_ICON_SIZE)), SKILL_ICON_SIZE,     SKILL_ICON_SIZE,   cgs.media.skillPics[skill] );
		CG_DrawPic(x + 20, y + 2 + (i * (SKILL_ICON_SIZE)), SKILL_ICON_SIZE - 2, SKILL_ICON_SIZE-2, cgs.media.numberShaders[ci->skill[skill]]);				
	}	
}

/* CG_DrawActivePowerups() 

Draws active powers the player currently has

*/

#define ICONSIZE 24
#define OFFSETX ( icons >= 3 ? 44 + ICONSIZE + 4 : 44 )
#define OFFSETY ( ((icons >= 6 ? icons - 6 : (icons >= 3 ? icons - 3 : icons)) * (ICONSIZE+4)) +388)
#define OFFSETY2(int)(floor((float)icons / (float)blockHeight))

extern pmove_t	*pm;


void CG_DrawActivePowerups(void) // int x, int y, int blockHeight )
{	
	clientInfo_t	*ci;
	playerState_t	*ps;

	int 			icons		= 0;
	int				cls			= 0;						// clas a covert is disguised as
	int				i, tmp		= 0;
	int				x, y, blockHeight;
	static int		first		= 1;
	static int		offsetX[16];
	static int 		offsetY[16];
	static int 		lastX		= 0;
	static int 		lastY		= 0;
	static int 		lastbH		= 0;	
	static int		iconSize	= ICONSIZE;	

	x = hud_powerupX.integer;
	y = hud_powerupY.integer;
	blockHeight = hud_powerupRows.integer;

	ps = &cg.snap->ps;
	ci = &cgs.clientinfo[ ps->clientNum ];


	if (x != lastX || y != lastY || lastbH != blockHeight || iconSize != hud_powerupSize.integer || first)
	{
		if (blockHeight <= 0) 
		{
			blockHeight = 3;
		}

		first		= 0;
		iconSize	= hud_powerupSize.integer;
		lastX		= x;
		lastY		= y;
		lastbH		= blockHeight;

		if (iconSize < 8)
		{
			iconSize = 8;
			trap_Cvar_Set("hud_powerupSize", "8");
		}

		//if (lastbH != blockHeight)
		{
			for (i = 0; i < 15; i++)
			{				
				while ((i+1) - tmp > blockHeight)
				{
					tmp += blockHeight;
				}
				offsetY[i] = y + iconSize * (i - tmp);
				offsetX[i] = x + iconSize * (int)(floor((float)i / (float)blockHeight));
			}			
		}
	}
// Visibility Icon - ( Carrying Objective or Wearing Disguise )
	// jP - draw objective icon if we are carrying one
	if( ps->powerups[PW_REDFLAG] || ps->powerups[PW_BLUEFLAG] ) 
	{
		trap_R_SetColor( NULL );	
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.objectiveShader );
		icons++;
	} 
	else if ( ps->powerups[PW_OPS_DISGUISED] )  // Disguised?
	{	// cannot have a disguise while carrying an objective
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, ps->persistant[PERS_TEAM] == TEAM_AXIS ? cgs.media.alliedUniformShader : cgs.media.axisUniformShader );
		
		// jP - Show what class a covert is disguised as	
		// jaquboss, use playerstate to stop delay
		if( ps->powerups[PW_OPS_CLASS_1] )
			cls |= 1;
		if( ps->powerups[PW_OPS_CLASS_2] )
			cls |= 2;
		if( ps->powerups[PW_OPS_CLASS_3] )
			cls |= 4;

		CG_DrawStringExt(offsetX[icons], offsetY[icons], va( "%s", BG_ShortClassnameForNumber(cls)), colorWhite, qfalse, qtrue, SMALLCHAR_WIDTH -4, SMALLCHAR_HEIGHT-4, 0);		
		icons++;
	}
// Teamplay Auras - effect icons
	if ( ps->stats[STAT_TEAMAURA] & AURABIT_MEDIC ) 
	{
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.hudCaduceus );
		icons++;
	}

	if ( ps->stats[STAT_TEAMAURA] & AURABIT_ENGINEER )
	{
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.hudEngineer );
		icons++;
	}

	if ( ps->stats[STAT_TEAMAURA] & AURABIT_FIELDOPS)
	{
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.hudMorale[ps->persistant[PERS_TEAM]-1] );
		icons++;
	}

// Equipment Icons
	// Helmet
	if ( !(ps->eFlags & EF_HEADSHOT) )
	{
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.hudHelmet[ps->persistant[PERS_TEAM]-1] );
		icons++;
	}

	// Flak Jacket
	if ( ps->powerups[PW_FLAKJACKET] )	
	{
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.hudFlakJacket[ps->persistant[PERS_TEAM]-1] );	
		icons++;		
	}
	if ( ps->powerups[PW_INVULNERABLE] )
	{
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.spawnInvincibleShader );	
		icons++;
	}

	// Adrenaline
	if (COM_BitCheck( cg.snap->ps.weapons, WP_MEDIC_ADRENALINE ) || ps->powerups[PW_ADRENALINE])
	{
		vec4_t	dRed = { 0.5f, 0.f, 0.f, 1.f };
		qboolean colorSet=qfalse;

		if ((cg.time - ps->classWeaponTime) < (cg.medicChargeTime[ps->persistant[PERS_TEAM]-1] * BG_ChargeUsage(WP_MEDIC_ADRENALINE, cgs.clientinfo[ps->clientNum].skillBits)) ) 
		{
			if ( ps->powerups[PW_ADRENALINE] )
				trap_R_SetColor( dRed );
			else
				trap_R_SetColor( colorDkGrey );

			colorSet = qtrue;
		} else {
			if ( ps->powerups[PW_ADRENALINE] )
			{
				trap_R_SetColor( colorRed );
				colorSet = qtrue;
			}
		}
		
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.hudAdrenaline );

		if ( colorSet )
			trap_R_SetColor( NULL );

		icons++;
	}
	// Binoculars
	if (cg.snap->ps.stats[STAT_KEYS] & (1 << INV_BINOCS) ) 
	{
		CG_DrawPic( offsetX[icons], offsetY[icons], iconSize, iconSize, cgs.media.hudBinoculars );
		icons++;
	}

}

static void CG_DrawPlayerStats( void ) 
{
	playerState_t		*ps;
	clientInfo_t		*ci;
	skillType_t			skill;
	int					i;

	const char*			str;
	float				w;

	vec_t				*clr;

	ps = &cg.snap->ps;
	ci = &cgs.clientinfo[ ps->clientNum ];

	// Draw the player's skill levels
	if (!hud_drawAltHUD.integer)	
	{
		str = va( "HP %i", cg.snap->ps.stats[STAT_HEALTH] );
		w   = CG_Text_Width_Ext( str, 0.25f, 0, &cgs.media.limboFont1 );

		CG_Text_Paint_Ext( SKILLS_X - 18 - w, 480 - 4, 0.25f, 0.25f, colorWhite, str,  0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 );

		// only draw health in LMS games
		if (cgs.gametype == GT_WOLF_LMS) return;

		for( i = 0; i < 3; i++ ) 
		{
			skill = CG_ClassSkillForPosition( ci, i );
			
			CG_FillRect(i * SKILL_ICON_X_SCALE + SKILL_ICON_X -2, 458 + SKILL_ICON_Y, SKILL_ICON_SIZE +4, 38, HUD_Background );
		    
			CG_DrawPic( i * SKILL_ICON_X_SCALE + SKILL_ICON_X +1, 460 + SKILL_ICON_Y, SKILL_ICON_SIZE-2, SKILL_ICON_SIZE-2, cgs.media.numberShaders[ci->skill[skill]]);
			CG_DrawPic( i * SKILL_ICON_X_SCALE + SKILL_ICON_X   , 480 + SKILL_ICON_Y, SKILL_ICON_SIZE, SKILL_ICON_SIZE, cgs.media.skillPics[skill] );
		}
		CG_DrawRect_FixedBorder(SKILL_ICON_X - 2, 458 + SKILL_ICON_Y, (3 * SKILL_ICON_SIZE) + 8, 38, 1, HUD_Border);

		// draw the player's total xp, yellow if it changed in the last second, white otherwise
		clr = (cg.time - cg.xpChangeTime < 1000) ? colorYellow : colorWhite;

		str = va( "XP %i", cg.snap->ps.stats[STAT_XP] );
		w = CG_Text_Width_Ext( str, 0.25f, 0, &cgs.media.limboFont1 );
		CG_Text_Paint_Ext( SKILLS_X + 48 - w, 480 - 4, 0.25f, 0.25f, clr, str, 0, 0, ITEM_TEXTSTYLE_SHADOWED, &cgs.media.limboFont1 );		

		// draw the objective icon if the player is carrying one
		if( ps->powerups[PW_REDFLAG] || ps->powerups[PW_BLUEFLAG] ) 
		{
			trap_R_SetColor( NULL );
			CG_DrawPic( 640 - 40, 480 - 140, 36, 36, cgs.media.objectiveShader );
		}
		// or.... draw the uniform icon if the player is wearing one
		else if (ps->powerups[PW_OPS_DISGUISED])
		{
			CG_DrawPic( 640 - 40, 480 - 140, 36, 36, ps->persistant[PERS_TEAM] == TEAM_AXIS ? cgs.media.alliedUniformShader : cgs.media.axisUniformShader );
		}
	}
	else
	{
		CG_DrawSkillLevels();
		
		if (hud_drawPowerups.integer)		// draw powerup icons
		{
			CG_DrawActivePowerups(); 
		}
		else
		{
			// draw the objective icon if the player is carrying one
			if( ps->powerups[PW_REDFLAG] || ps->powerups[PW_BLUEFLAG] ) 
			{
				trap_R_SetColor( NULL );
				CG_DrawPic( 640 - 40, 480 - 140, 36, 36, cgs.media.objectiveShader );
			}
			// or.... draw the uniform icon if the player is wearing one
			else if (ps->powerups[PW_OPS_DISGUISED])
			{
				CG_DrawPic( 640 - 40, 480 - 140, 36, 36, ps->persistant[PERS_TEAM] == TEAM_AXIS ? cgs.media.alliedUniformShader : cgs.media.axisUniformShader );
			}
		}
	}
}

static char statsDebugStrings[6][512];
static int statsDebugTime[6];
static int statsDebugTextWidth[6];
static int statsDebugPos;

void CG_InitStatsDebug( void )
{
	memset( &statsDebugStrings, 0, sizeof(statsDebugStrings) );
	memset( &statsDebugTime, 0, sizeof(statsDebugTime) );
	statsDebugPos = -1;
}

void CG_StatsDebugAddText( const char *text )
{
	if( cg_debugSkills.integer ) {
		statsDebugPos++;

		if( statsDebugPos >= 6 )
			statsDebugPos = 0;

		Q_strncpyz( statsDebugStrings[statsDebugPos], text, 512 );
		statsDebugTime[statsDebugPos] = cg.time;
		statsDebugTextWidth[statsDebugPos] = CG_Text_Width_Ext( text, .15f, 0, &cgs.media.limboFont2 );

		CG_Printf( "%s\n", text );
	}
}

static void CG_DrawStatsDebug( void )
{
	int textWidth = 0;
	int i, x, y, w, h;

	if( !cg_debugSkills.integer )
		return;

	for( i = 0; i < 6; i++ ) {
		if( statsDebugTime[i] + 9000 > cg.time ) {
			if( statsDebugTextWidth[i] > textWidth )
				textWidth = statsDebugTextWidth[i];
		}
	}

	w = textWidth + 6;
	h = 9;
	x = 640 - w;
	y = (480 - 5 * ( 12 + 2 ) + 6 - 4) - 6 - h;	// don't ask

	i = statsDebugPos;

	do {
		vec4_t colour;

		if( statsDebugTime[i] + 9000 <= cg.time ) {
			break;
		}

        colour[0] = colour[1] = colour[2] = .5f;
		if( cg.time - statsDebugTime[i] > 5000 )
			colour[3] = .5f - .5f * ( ( cg.time - statsDebugTime[i] - 5000 ) / 4000.f );
		else
			colour[3] = .5f ;
		CG_FillRect( x, y, w, h, colour );

		colour[0] = colour[1] = colour[2] = 1.f;
		if( cg.time - statsDebugTime[i] > 5000 )
			colour[3] = 1.f - ( ( cg.time - statsDebugTime[i] - 5000 ) / 4000.f );
		else
			colour[3] = 1.f ;
		CG_Text_Paint_Ext( 640.f - 3 - statsDebugTextWidth[i], y + h - 2, .15f, .15f, colour, statsDebugStrings[i], 0, 0, ITEM_TEXTSTYLE_NORMAL, &cgs.media.limboFont2 );

		y -= h;

		i--;
		if( i < 0 )
			i = 6 - 1;
	} while( i != statsDebugPos );
}

//bani
void CG_DrawDemoRecording( void ) {
	char status[1024];
	char demostatus[128];
	char wavestatus[128];

	if( !cl_demorecording.integer && !cl_waverecording.integer ) {
		return;
	}

	if( !cg_recording_statusline.integer ) {
		return;
	}

	if( cl_demorecording.integer ) {
		Com_sprintf( demostatus, sizeof( demostatus ), " demo %s: %ik ", cl_demofilename.string, cl_demooffset.integer / 1024 );
	} else {
		strncpy( demostatus, "", sizeof( demostatus ) );
	}

	if( cl_waverecording.integer ) {
		Com_sprintf( wavestatus, sizeof( demostatus ), " audio %s: %ik ", cl_wavefilename.string, cl_waveoffset.integer / 1024 );
	} else {
		strncpy( wavestatus, "", sizeof( wavestatus ) );
	}

	Com_sprintf( status, sizeof( status ), "RECORDING%s%s", demostatus, wavestatus );

	CG_Text_Paint_Ext( 5, cg_recording_statusline.integer, 0.2f, 0.2f, colorWhite, status, 0, 0, 0, &cgs.media.limboFont2 );
}

void CG_DrawExpandedAutoMap( void );

void CG_DrawOnScreenNames ( void )
{
	int	i;
	specName_t *spcNm;
	vec4_t	white={1.0f,1.0f,1.0f,1.0f};
	trace_t	tr;
	vec3_t mins={-4,-4,-4};
	vec3_t maxs={4,4,4};

	for ( i=0; i< cg.specOnScreenNamesNum;i++)
	{
		spcNm = &cg.specOnScreenNames[i];

		if ( !spcNm )
			break;

		CG_Trace( &tr, cg.refdef.vieworg, mins, maxs, spcNm->origin, -1, CONTENTS_SOLID);

			if( tr.fraction < 1.0f )
				continue;

		CG_Text_Paint_Ext( spcNm->x, spcNm->y, spcNm->scale, spcNm->scale, white, spcNm->text, 0, 0, 0, &cgs.media.limboFont1 );

		memset(spcNm, 0, sizeof(spcNm) ); // ok drawn, reset it 
	}

	cg.specOnScreenNamesNum = 0;

}
// jaquboss, done for adrenaline ( whole screen red )

/*void CG_BlendColor( void )
{
	vec4_t color={0.0f, 0.0f ,0.0f, 1.0f };

	color[0] = 1.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;

	trap_R_SetColor( color );

	CG_DrawPic(0,0,SCREEN_WIDTH,SCREEN_HEIGHT, cgs.media.onScreenColorShader );

	trap_R_SetColor( NULL );

}*/

/*
=================
CG_Draw2D
=================
*/
void CG_DrawAnnouncer( void );

static void CG_Draw2D( void ) 
{


	CG_ScreenFade();
	jP_SetHUDColors(); // jet Pilot - TODO - only call this if values have changed

	if ( cg.snap->ps.pm_type == PM_INTERMISSION )
	{
		CG_DrawIntermission();
		return;
	} 
	else 
	{
		if( cgs.dbShowing ) 
			CG_Debriefing_Shutdown();
	}

	if ( cg.editingSpeakers ) 
	{
		CG_SpeakerEditorDraw();
		return;
	}

	CG_DrawOnScreenNames();

	//bani - #127 - no longer cheat protected, we draw crosshair/reticle in non demoplayback
	if ( cg_draw2D.integer == 0 ) {
		if( cg.demoPlayback )
			return;
		CG_DrawCrosshair();
		CG_DrawFlashFade();
		return;
	}

	if( !cg.cameraMode ) {
		CG_DrawFlashBlendBehindHUD();

		if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {


			CG_DrawSpectator();
			CG_DrawCrosshair();
			CG_DrawCrosshairNames();

			// NERVE - SMF - we need to do this for spectators as well
			CG_DrawTeamInfo();
		} else {
			// don't draw any status if dead
			if ( cg.snap->ps.stats[STAT_HEALTH] > 0 || (cg.snap->ps.pm_flags & PMF_FOLLOW) ) {

				CG_DrawCrosshair();
				CG_DrawCrosshairNames();
				CG_DrawNoShootIcon();

			}

			CG_DrawTeamInfo();

			if ( cg_drawStatus.integer ) {
				Menu_PaintAll();
				CG_DrawTimedMenus();
			}
		}

		CG_DrawVote();

		CG_DrawLagometer();
	}

	// don't draw center string if scoreboard is up
	if ( !CG_DrawScoreboard() ) 
	{
		if( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
			rectDef_t rect;

			if( cg.snap->ps.stats[STAT_HEALTH] > 0 ) 
			{				
				// jet Pilot - optionally draw the player's face on the HUD
				if (cg_drawHUDHead.integer && !hud_drawAltHUD.integer) 
					CG_DrawPlayerStatusHead();
				CG_DrawPlayerStatus();
				CG_DrawPlayerStats();
			}

			CG_DrawLivesLeft();

			// Cursor hint
			rect.w = rect.h = 48;
			rect.x = .5f * SCREEN_WIDTH - .5f * rect.w;
			rect.y = 260;
			CG_DrawCursorhint( &rect );

			// Stability bar
			rect.x = 50;
			rect.y = 208;
			rect.w = 10;
			rect.h = 64;
			CG_DrawWeapStability( &rect );

			// Stats Debugging
			CG_DrawStatsDebug();
		}

		if (!cg_paused.integer) 
			CG_DrawUpperRight();

		CG_DrawCenterString();
		CG_DrawAnnouncement();
		CG_DrawPMItems();
		CG_DrawPMItemsBig();

		CG_DrawFollow();
		CG_DrawWarmup();

		CG_DrawNotify();

		CG_DrawObjectiveInfo();

		if ( cg_descriptiveText.integer )
			CG_DrawSpectatorMessage();

		CG_DrawLimboMessage();

		// jaquboss from RTCW
		if (cg_drawObjIcons.integer)
			CG_DrawObjectiveIcons();

		if ( cg_drawCompass.integer ) 
			CG_DrawNewCompass();
		else
			CG_DrawExpandedAutoMap();

	} else {
		if(cgs.eventHandling != CGAME_EVENT_NONE) {
			trap_R_SetColor( NULL );
			CG_DrawPic( cgDC.cursorx-14, cgDC.cursory-14, 32, 32, cgs.media.cursorIcon);
		}
	}


	if( cg.showFireteamMenu )
		CG_Fireteams_Draw();

	CG_DrawAnnouncer();

	// Info overlays
	CG_DrawOverlays();

	// OSP - window updates
	CG_windowDraw();

	// Ridah, draw flash blends now
	CG_DrawFlashBlend();

	CG_DrawDemoRecording();
}

// NERVE - SMF
void CG_StartShakeCamera( float p ) {
	cg.cameraShakeScale = p;

	cg.cameraShakeLength = 1000 * (p*p);
	cg.cameraShakeTime = cg.time + cg.cameraShakeLength;
	cg.cameraShakePhase = crandom()*M_PI; // start chain in random dir
}

void CG_ShakeCamera() {
	float x, val;

	if ( cg.time > cg.cameraShakeTime ) {
		cg.cameraShakeScale = 0; // JPW NERVE all pending explosions resolved, so reset shakescale
		return;
	}
	
	// JPW NERVE starts at 1, approaches 0 over time
	x = (cg.cameraShakeTime - cg.time) / cg.cameraShakeLength;

	if ( cg_rtcwShake.integer ){
		// up/down
		val = sin(M_PI * 8 * x + cg.cameraShakePhase) * x * 4.5f * cg.cameraShakeScale;
		cg.refdefViewAngles[0] += val; 

		// left/right
		val = sin(M_PI * 15 * x + cg.cameraShakePhase) * x * 4.0f * cg.cameraShakeScale;
		cg.refdefViewAngles[1] += val;
	} else {
		// move
		val = sin( M_PI * 7 * x + cg.cameraShakePhase ) * x * 4.0f * cg.cameraShakeScale;
		cg.refdef.vieworg[ 2 ] += val;
		val = sin( M_PI * 13 * x + cg.cameraShakePhase ) * x * 4.0f * cg.cameraShakeScale;
		cg.refdef.vieworg[ 1 ] += val;
		val = cos( M_PI * 17 * x + cg.cameraShakePhase ) * x * 4.0f * cg.cameraShakeScale;
		cg.refdef.vieworg[ 0 ] += val;
	}

	AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
}
// -NERVE - SMF

void CG_Coronas ( void ){
	int			i;

	if(cg_coronas.integer == 0)
		return;

	for ( i = 0 ; i < cg.numCoronas ; i++ ) {
		trace_t		tr;
		float		dot, dist;
		vec3_t		dir;
		qboolean	visible = qfalse,
					behind = qfalse,
					toofar = qfalse;

		if (!trap_R_inPVS( cg.refdef_current->vieworg, cgs.corona[i].org ) )
			continue;

		VectorSubtract(cg.refdef_current->vieworg, cgs.corona[i].org, dir);
		dist = VectorNormalize2(dir, dir);

		if(dist > cg_coronafardist.integer) 
			toofar = qtrue;

		dot = DotProduct(dir, cg.refdef_current->viewaxis[0] );

		if(dot>=-0.6)
			behind = qtrue;		

		if(cg_coronas.integer == 2) {	// if set to '2' trace everything
			behind = qfalse;
			toofar = qfalse;
		}

		if(!behind && !toofar) {
			CG_Trace( &tr, cg.refdef_current->vieworg, NULL, NULL, cgs.corona[i].org, -1, MASK_SOLID|CONTENTS_BODY );

			if(tr.fraction == 1)
				visible = qtrue;

			trap_R_AddCoronaToScene( cgs.corona[i].org, cgs.corona[i].color[0], cgs.corona[i].color[1], cgs.corona[i].color[2], cgs.corona[i].scale, i, visible);
		}
	}
}

void CG_DrawMiscGamemodels( void ) {
	int i, j;
	refEntity_t ent;
	int drawn = 0;

	memset( &ent, 0, sizeof( ent ) );

	ent.reType = RT_MODEL;
	ent.nonNormalizedAxes =	qtrue;
	
	// ydnar: static gamemodels don't project shadows
	ent.renderfx = RF_NOSHADOW;
	
	for( i = 0; i < cg.numMiscGameModels; i++ ) {
		if( cgs.miscGameModels[i].radius ) {
			if( CG_CullPointAndRadius( cgs.miscGameModels[i].org, cgs.miscGameModels[i].radius ) ) {
 				continue;
			}
		}

		if( !trap_R_inPVS( cg.refdef_current->vieworg, cgs.miscGameModels[i].org ) ) {
			continue;
		}

		VectorCopy( cgs.miscGameModels[i].org, ent.origin );
		VectorCopy( cgs.miscGameModels[i].org, ent.oldorigin );
		VectorCopy( cgs.miscGameModels[i].org, ent.lightingOrigin );

		for( j = 0; j < 3; j++ ) {
			VectorCopy( cgs.miscGameModels[i].axes[j], ent.axis[j] );
		}
		ent.hModel = cgs.miscGameModels[i].model;
		
		trap_R_AddRefEntityToScene( &ent );

		drawn++;
	}
}

/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void CG_DrawActive( stereoFrame_t stereoView ) {
	float		separation;
	vec3_t		baseOrg;

	// optionally draw the info screen instead
	if ( !cg.snap ) {
		CG_DrawInformation( qfalse );
		return;
	}

	switch ( stereoView ) {
	case STEREO_CENTER:
		separation = 0;
		break;
	case STEREO_LEFT:
		separation = -cg_stereoSeparation.value / 2;
		break;
	case STEREO_RIGHT:
		separation = cg_stereoSeparation.value / 2;
		break;
	default:
		separation = 0;
		CG_Error( "CG_DrawActive: Undefined stereoView" );
	}


	// clear around the rendered view if sized down
	CG_TileClear();

	// offset vieworg appropriately if we're doing stereo separation
	VectorCopy( cg.refdef_current->vieworg, baseOrg );
	if ( separation != 0 ) {
		VectorMA( cg.refdef_current->vieworg, -separation, cg.refdef_current->viewaxis[1], cg.refdef_current->vieworg );
	}

	cg.refdef_current->glfog.registered = 0;	// make sure it doesn't use fog from another scene

	CG_ActivateLimboMenu();

	if ( cg.showGameView ) {
 		float x, y, w, h;
 		x = LIMBO_3D_X;
 		y = LIMBO_3D_Y;
 		w = LIMBO_3D_W;
 		h = LIMBO_3D_H;

 		CG_AdjustFrom640( &x, &y, &w, &h );

 		cg.refdef_current->x = x;
 		cg.refdef_current->y = y;
 		cg.refdef_current->width = w;
 		cg.refdef_current->height = h;

 		CG_Letterbox( (LIMBO_3D_W/640.f)*100, (LIMBO_3D_H/480.f)*100, qfalse );
	}

	CG_ShakeCamera();		// NERVE - SMF

	// Gordon
	CG_PB_RenderPolyBuffers();

	// Gordon
	CG_DrawMiscGamemodels();

	CG_Coronas();

	if( !(cg.limboEndCinematicTime > cg.time && cg.showGameView) ) {
		trap_R_RenderScene( cg.refdef_current );
	}

	// restore original viewpoint if running stereo
	if ( separation != 0 ) {
		VectorCopy( baseOrg, cg.refdef_current->vieworg );
	}

	if( !cg.showGameView ) {
		// draw status bar and other floating elements
		CG_Draw2D();
	} else {
		CG_LimboPanel_Draw();
	}
}
// Jaquboss, this need to be done totaly diffirently
void CG_DrawMissileCamera(rectDef_t *rect ) 
{
	float			x, y, w, h;
	refdef_t		refdef;
	vec3_t			forward;
	centity_t		*cent;
	entityState_t	*s1;


	if ( !cg.latestMissile  )
		return;

	// Save out the old render info so we don't kill the LOD system here
	trap_R_SaveViewParms();

	cent = cg.latestMissile;
	s1	 = &cent->currentState;

	memset( &refdef, 0, sizeof( refdef_t ) );
	memcpy(refdef.areamask, cg.snap->areamask, sizeof(refdef.areamask));

	cg.subscene = qtrue;	// jaquboss, to see entities in satchel view again

	x = rect->x;
	y = rect->y;
	w = rect->w;
	h = rect->h;

	CG_AdjustFrom640( &x, &y, &w, &h );
	memset( &refdef, 0, sizeof( refdef ) );
	AxisClear( refdef.viewaxis );

	refdef.fov_x = cg.refdef_current->fov_x;
	refdef.fov_y = cg.refdef_current->fov_y;
	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;
	refdef.time = cg.time;

	VectorCopy (cent->lerpOrigin ,refdef.vieworg);
	cent->lerpAngles[2]	= 0;
	AnglesToAxis (cent->lerpAngles ,refdef.viewaxis);
	AngleVectors(cent->lerpAngles , forward, NULL, NULL );


	VectorMA(refdef.vieworg, 32, forward, refdef.vieworg); // push a bit forward

	cg.refdef_current = &refdef;

	trap_R_ClearScene();

	CG_SetupFrustum();
	CG_DrawSkyBoxPortal(qfalse);

	if(!cg.hyperspace) {
		CG_AddPacketEntities();
		CG_AddMarks();
		CG_AddParticles();
		CG_AddLocalEntities();

		CG_AddSmokeSprites();
		CG_AddAtmosphericEffects();

		CG_AddFlameChunks();
		CG_AddTrails();		// this must come last, so the trails dropped this frame get drawn
		
		// jaquboss	
		CG_PB_RenderPolyBuffers();
		CG_DrawMiscGamemodels();
		CG_Coronas();

	}

	refdef.time = cg.time;
	trap_SetClientLerpOrigin(refdef.vieworg[0], refdef.vieworg[1], refdef.vieworg[2]);

	trap_R_RenderScene(&refdef);

	cg.refdef_current = &cg.refdef;
	cg.subscene = qfalse;		

	// grain shader
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cgs.media.tv_grain);


	// Reset the view parameters
	trap_R_RestoreViewParms();


}
// note: single line only!
void CG_DrawAnnouncer( void )
{
	int		x, y, w, h;
	float	scale, fade;
	vec4_t	color;


	if( !cg_announcer.integer || cg.centerPrintAnnouncerTime <= cg.time ) 
		return;


	fade = (float)(cg.centerPrintAnnouncerTime - cg.time)/cg.centerPrintAnnouncerDuration;

	color[0] = cg.centerPrintAnnouncerColor[0];
	color[1] = cg.centerPrintAnnouncerColor[1];
	color[2] = cg.centerPrintAnnouncerColor[2]; 

	switch( cg.centerPrintAnnouncerMode ){
		default:
		case ANNOUNCER_NORMAL:
			color[3] = fade;
			break;
		case ANNOUNCER_SINE:
			color[3] = sin( M_PI * fade );
			break;
		case ANNOUNCER_INVERSE_SINE:
			color[3] = 1-sin( M_PI * fade );
			break;
	} 
	
	scale = (1.1f - color[3]) * cg.centerPrintAnnouncerScale;

	h = CG_Text_Height_Ext( cg.centerPrintAnnouncer, scale, 0, &cgs.media.limboFont1 );

	y = (SCREEN_HEIGHT - h) / 2;

	w = CG_Text_Width_Ext(cg.centerPrintAnnouncer, scale, 0, &cgs.media.limboFont1 );

	x = ( SCREEN_WIDTH - w ) / 2;

	CG_Text_Paint_Ext( x, y, scale, scale , color, cg.centerPrintAnnouncer, 0, 0, 0, &cgs.media.limboFont1 );
}

void CG_AddAnnouncer(char *text, sfxHandle_t sound, float scale, int duration, float r, float g, float b, int mode)
{

	if ( !cg_announcer.integer )
		return;

	if ( sound )
		trap_S_StartLocalSound( sound, CHAN_ANNOUNCER );

	if ( text ){
		cg.centerPrintAnnouncer = text;
		cg.centerPrintAnnouncerTime = cg.time + duration;
		cg.centerPrintAnnouncerScale = scale; 
		cg.centerPrintAnnouncerDuration = duration; 
		cg.centerPrintAnnouncerColor[0] = r;
		cg.centerPrintAnnouncerColor[1] = g;
		cg.centerPrintAnnouncerColor[2] = b;
		cg.centerPrintAnnouncerMode = mode;
	}


}



