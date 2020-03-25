/*
 * name:	ui_local.h
 *
 * desc:
 *
 * NQQS:
 *
 */
#ifndef __UI_LOCAL_H__
#define __UI_LOCAL_H__

#include "../game/q_shared.h"
#include "../cgame/tr_types.h"
#include "ui_public.h"
#include "keycodes.h"
#include "../game/bg_public.h"
#ifdef HW_BAN
  #include "../game/bg_hwguid.h"
#endif // HW_BAN
#include "ui_shared.h"

extern vmCvar_t	ui_master;

extern vmCvar_t	ui_cdkey;
extern vmCvar_t	ui_cdkeychecked;
extern vmCvar_t	ui_selectedPlayer;
extern vmCvar_t	ui_selectedPlayerName;
extern vmCvar_t	ui_netSource;
extern vmCvar_t	ui_menuFiles;
extern vmCvar_t	ui_gameType;
extern vmCvar_t	ui_netGameType;
extern vmCvar_t	ui_joinGameType;
extern vmCvar_t	ui_dedicated;
extern vmCvar_t	ui_clipboardName;

// NERVE - SMF - multiplayer cvars
extern vmCvar_t	ui_serverFilterType;
extern vmCvar_t	ui_currentNetMap;
extern vmCvar_t	ui_currentMap;
extern vmCvar_t	ui_mapIndex;

extern vmCvar_t	ui_browserMaster;
extern vmCvar_t	ui_browserGameType;
extern vmCvar_t	ui_browserSortKey;
extern vmCvar_t	ui_browserShowEmptyOrFull;
extern vmCvar_t ui_browserShowPasswordProtected;
extern vmCvar_t	ui_browserShowFriendlyFire;
extern vmCvar_t	ui_browserShowMaxlives;
extern vmCvar_t	ui_browserShowPunkBuster;
extern vmCvar_t ui_browserShowAntilag;
extern vmCvar_t ui_browserShowWeaponsRestricted;
extern vmCvar_t ui_browserShowTeamBalanced;
extern vmCvar_t	ui_browserNQonly;

extern vmCvar_t	ui_serverStatusTimeOut;
extern vmCvar_t	ui_limboOptions;

extern vmCvar_t	ui_isSpectator;
// -NERVE - SMF

extern vmCvar_t	g_gameType;

extern vmCvar_t cl_profile;
extern vmCvar_t cl_defaultProfile;
extern vmCvar_t ui_profile;
extern vmCvar_t ui_currentNetCampaign;
extern vmCvar_t ui_currentCampaign;
extern vmCvar_t ui_campaignIndex;
extern vmCvar_t	ui_currentCampaignCompleted;
extern vmCvar_t ui_blackout;
extern vmCvar_t cg_crosshairAlpha;
extern vmCvar_t cg_crosshairAlphaAlt;
extern vmCvar_t cg_crosshairColor;
extern vmCvar_t cg_crosshairColorAlt;
extern vmCvar_t	cg_crosshairSize;

extern vmCvar_t cl_bypassMouseInput;

//bani
extern vmCvar_t ui_autoredirect;

//
// ui_qmenu.c
//
#define	MAX_EDIT_LINE			256

//
// ui_main.c
//
void			UI_Report();
void			UI_Load();
void			UI_LoadMenus(const char *menuFile, qboolean reset);
void			_UI_SetActiveMenu( uiMenuCommand_t menu );
uiMenuCommand_t	_UI_GetActiveMenu(void);

void			UI_ShowPostGame(qboolean newHigh);
void			UI_LoadArenas(void);
void			UI_LoadCampaigns(void);
mapInfo*		UI_FindMapInfoByMapname( const char* name );
void			UI_ReadableSize( char *buf, int bufsize, int value );
void			UI_PrintTime( char *buf, int bufsize, int time );
void			Text_Paint_Ext( float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font );

void UI_Campaign_f( void );
void UI_ListCampaigns_f( void );

#ifdef HW_BAN
  void UI_ShowHWGuid_f();
  void UI_ShowHWInfo_f();
#endif // HW_BAN

#define GLINFO_LINES		128

// core: image used for the "glossy"-effect on buttons..
qhandle_t	imgBlingbling;
// core: image used for the "fade to black"-effect in the credits-menu..
qhandle_t	imgFadeToBlack;
// core: image used for the "shining"-effect on buttons..
qhandle_t	imgFadeCircle;

//
// ui_menu.c
//
extern void UI_RegisterCvars( void );
extern void UI_UpdateCvars( void );

//
// ui_connect.c
//
extern void UI_DrawConnectScreen( qboolean overlay );

//
// ui_loadpanel.c
//
extern void UI_DrawLoadPanel( qboolean forcerefresh, qboolean ownerdraw, qboolean uihack );


// new ui stuff
#define MAX_HEADS 64
#define MAX_ALIASES 64
#define MAX_HEADNAME  32
#define MAX_TEAMS 64
#define MAX_GAMETYPES 16
#define MAX_MAPS 512	// kw: increased from 128 // IRATA: 512 maps ...that is huuuuuuge!

#define MAX_ADDRESSLENGTH		64
#define MAX_HOSTNAMELENGTH		22
#define MAX_MAPNAMELENGTH		16
#define MAX_STATUSLENGTH		64
#define MAX_LISTBOXWIDTH		59
#define UI_FONT_THRESHOLD		0.1
#define MAX_DISPLAY_SERVERS		4096
#define MAX_SERVERSTATUS_LINES	128
#define MAX_SERVERSTATUS_TEXT	2048
#define MAX_FOUNDPLAYER_SERVERS	16
#define TEAM_MEMBERS 5

#define MAX_MODS 64
#define MAX_DEMOS 512	// kw: increased from 256
#define MAX_MOVIES 256
#define MAX_PLAYERMODELS 256
#define MAX_SAVEGAMES 256
#define MAX_SPAWNPOINTS 128		// NERVE - SMF
#define MAX_SPAWNDESC	128		// NERVE - SMF
#define MAX_PBLINES		128		// DHM - Nerve
#define MAX_PBWIDTH		42		// DHM - Nerve

#define MAX_PROFILES 64

typedef struct {
	const char *name;
	const char *imageName;
	qhandle_t headImage;
	qboolean female;
} characterInfo;

//----(SA)	added
typedef struct {
	const char	*name;
	qhandle_t	sshotImage;
} savegameInfo;

typedef struct {
	const char *name;
	const char *ai;
	const char *action;
} aliasInfo;

typedef struct {
	const char *teamName;
	const char *imageName;
	const char *teamMembers[TEAM_MEMBERS];
	qhandle_t teamIcon;
	qhandle_t teamIcon_Metal;
	qhandle_t teamIcon_Name;
	int cinematic;
} teamInfo;


typedef struct {
	const char *gameType;
	const char *gameTypeShort;
	int gtEnum;
	const char *gameTypeDescription;
} gameTypeInfo;

typedef struct {
	const char *name;
	const char *dir;
} profileInfo_t;

typedef struct serverFilter_s {
	const char *description;
	const char *basedir;
} serverFilter_t;


typedef struct serverStatus_s {
	int		refreshtime;
	int		sortKey;
	int		sortDir;
	qboolean refreshActive;
	int		currentServer;
	int		displayServers[MAX_DISPLAY_SERVERS];
	int		numDisplayServers;
	int		numPlayersOnServers;
	int		nextDisplayRefresh;
	qhandle_t currentServerPreview;
	int		currentServerCinematic;
	int		motdLen;
	int		motdWidth;
	int		motdPaintX;
	int		motdPaintX2;
	int		motdOffset;
	int		motdTime;
	char	motd[MAX_STRING_CHARS];
} serverStatus_t;

typedef struct {
	char		adrstr[MAX_ADDRESSLENGTH];
	char		name[MAX_ADDRESSLENGTH];
	int			startTime;
	int			serverNum;
	qboolean	valid;
} pendingServer_t;

typedef struct {
	int num;
	pendingServer_t server[MAX_SERVERSTATUSREQUESTS];
} pendingServerStatus_t;

typedef struct {
	char address[MAX_ADDRESSLENGTH];
	char *lines[MAX_SERVERSTATUS_LINES][4];
	char text[MAX_SERVERSTATUS_TEXT];
	char pings[MAX_CLIENTS * 3];
	int numLines;
} serverStatusInfo_t;

typedef struct {
	const char *modName;
	const char *modDescr;
} modInfo_t;

typedef struct {
	displayContextDef_t uiDC;
	int newHighScoreTime;
	int newBestTime;
	int showPostGameTime;
	qboolean newHighScore;
	qboolean demoAvailable;
	qboolean soundHighScore;

	int characterCount;
	int botIndex;
	characterInfo characterList[MAX_HEADS];

	int aliasCount;
	aliasInfo aliasList[MAX_ALIASES];

	int teamCount;
	teamInfo teamList[MAX_TEAMS];

	int numGameTypes;
	gameTypeInfo gameTypes[MAX_GAMETYPES];

	int numJoinGameTypes;
	gameTypeInfo joinGameTypes[MAX_GAMETYPES];

	int redBlue;
	int playerCount;
	int myTeamCount;
	int teamIndex;
	int playerRefresh;
	int playerIndex;
	int playerNumber;
	qboolean teamLeader;
	char playerNames[MAX_CLIENTS][MAX_NAME_LENGTH*2];
	qboolean playerMuted[MAX_CLIENTS];
	int playerRefereeStatus[MAX_CLIENTS];
	char teamNames[MAX_CLIENTS][MAX_NAME_LENGTH];
	int teamClientNums[MAX_CLIENTS];

	int mapCount;
	mapInfo mapList[MAX_MAPS];

	int campaignCount;
	campaignInfo_t campaignList[MAX_CAMPAIGNS];

	cpsFile_t campaignStatus;

	profileInfo_t profileList[MAX_PROFILES];
	int profileCount;
	int profileIndex;

	int skillIndex;

	modInfo_t modList[MAX_MODS];
	int modCount;
	int modIndex;

	const char *demoList[MAX_DEMOS];
	int demoCount;
	int demoIndex;

	const char *movieList[MAX_MOVIES];
	int movieCount;
	int movieIndex;
	int previewMovie;

	//----(SA)	added
	savegameInfo	savegameList[MAX_SAVEGAMES];
	int				savegameCount;
	int				savegameIndex;

	serverStatus_t	serverStatus;

	// for the showing the status of a server
	char			serverStatusAddress[MAX_ADDRESSLENGTH];
	serverStatusInfo_t serverStatusInfo;
	int				nextServerStatusRefresh;

	// to retrieve the status of server to find a player
	pendingServerStatus_t pendingServerStatus;
	char			findPlayerName[MAX_STRING_CHARS];
	char			foundPlayerServerAddresses[MAX_FOUNDPLAYER_SERVERS][MAX_ADDRESSLENGTH];
	char			foundPlayerServerNames[MAX_FOUNDPLAYER_SERVERS][MAX_ADDRESSLENGTH];
	int				currentFoundPlayerServer;
	int				numFoundPlayerServers;
	int				nextFindPlayerRefresh;

	int				currentCrosshair;
	int				startPostGameTime;
	sfxHandle_t		newHighScoreSound;

	int				q3HeadCount;
	char			q3HeadNames[MAX_PLAYERMODELS][64];
	qhandle_t		q3HeadIcons[MAX_PLAYERMODELS];
	int				q3SelectedHead;

	int				effectsColor;

	qboolean		inGameLoad;

	int				selectedObjective;

	int				activeFont;

	const char		*glInfoLines[GLINFO_LINES];
	int				numGlInfoLines;

	vec4_t			xhairColor;
	vec4_t			xhairColorAlt;

	qhandle_t		passwordFilter;
	qhandle_t		friendlyFireFilter;
	qhandle_t		maxLivesFilter;
	qhandle_t		punkBusterFilter;
	qhandle_t		weaponRestrictionsFilter;
	qhandle_t		antiLagFilter;
	qhandle_t		teamBalanceFilter;
	qhandle_t		nqFilter;

	qhandle_t		campaignMap;

} uiInfo_t;

extern uiInfo_t uiInfo;

extern void			UI_Init( void );
extern void			UI_Shutdown( void );
extern void			UI_KeyEvent( int key );
extern void			UI_MouseEvent( int dx, int dy );
extern void			UI_Refresh( int realtime );
extern qboolean		UI_ConsoleCommand( int realTime );
extern float		UI_ClampCvar( float min, float max, float value );
extern void			UI_DrawNamedPic( float x, float y, float width, float height, const char *picname );
extern void			UI_DrawHandlePic( float x, float y, float w, float h, qhandle_t hShader );
extern void			UI_FillRect( float x, float y, float width, float height, const float *color );
extern void			UI_DrawRect( float x, float y, float width, float height, const float *color );
extern void			UI_DrawTopBottom(float x, float y, float w, float h);
extern void			UI_DrawSides(float x, float y, float w, float h);
extern void			UI_UpdateScreen( void );
extern void			UI_SetColor( const float *rgba );
extern void			UI_LerpColor(vec4_t a, vec4_t b, vec4_t c, float t);
extern void			UI_DrawBannerString( int x, int y, const char* str, int style, vec4_t color );
// extern float		UI_ProportionalSizeScale( int style );
// extern void			UI_DrawProportionalString( int x, int y, const char* str, int style, vec4_t color );
extern int			UI_ProportionalStringWidth( const char* str );
extern void			UI_DrawString( int x, int y, const char* str, int style, vec4_t color );
extern void			UI_DrawChar( int x, int y, int ch, int style, vec4_t color );
extern qboolean 	UI_CursorInRect (int x, int y, int width, int height);
extern void			UI_AdjustFrom640( float *x, float *y, float *w, float *h );
extern void			UI_DrawTextBox (int x, int y, int width, int lines);
extern qboolean		UI_IsFullscreen( void );
extern void			UI_SetActiveMenu( uiMenuCommand_t menu );
// extern void			UI_PushMenu ( menuframework_s *menu );
extern void			UI_PopMenu (void);
extern void			UI_ForceMenuOff (void);
extern char			*UI_Argv( int arg );
extern char			*UI_Cvar_VariableString( const char *var_name );
extern void			UI_Refresh( int time );
extern void			UI_KeyEvent( int key );

//
// ui_syscalls.c
//
void			trap_Print( const char *string );
void			trap_Error( const char *string );
int				trap_Milliseconds( void );
void			trap_Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
void			trap_Cvar_Update( vmCvar_t *vmCvar );
void			trap_Cvar_Set( const char *var_name, const char *value );
float			trap_Cvar_VariableValue( const char *var_name );
void			trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void			trap_Cvar_LatchedVariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void			trap_Cvar_SetValue( const char *var_name, float value );
void			trap_Cvar_Reset( const char *name );
void			trap_Cvar_Create( const char *var_name, const char *var_value, int flags );
void			trap_Cvar_InfoStringBuffer( int bit, char *buffer, int bufsize );
int				trap_Argc( void );
void			trap_Argv( int n, char *buffer, int bufferLength );
void			trap_Cmd_ExecuteText( int exec_when, const char *text );	// don't use EXEC_NOW!
void			trap_AddCommand( const char *cmdName );
int				trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void			trap_FS_Read( void *buffer, int len, fileHandle_t f );
void			trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void			trap_FS_FCloseFile( fileHandle_t f );
int				trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize );
int				trap_FS_Delete(const char *filename);
qhandle_t		trap_R_RegisterModel( const char *name );
qhandle_t		trap_R_RegisterSkin( const char *name );
qhandle_t		trap_R_RegisterShaderNoMip( const char *name );
void			trap_R_ClearScene( void );
void			trap_R_AddRefEntityToScene( const refEntity_t *re );
void			trap_R_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts );
void			trap_R_AddLightToScene( const vec3_t org, float radius, float intensity, float r, float g, float b, qhandle_t hShader, int flags );
void			trap_R_AddCoronaToScene( const vec3_t org, float r, float g, float b, float scale, int id, qboolean visible);
void			trap_R_RenderScene( const refdef_t *fd );
void			trap_R_SetColor( const float *rgba );
void			trap_R_Add2dPolys( polyVert_t* verts, int numverts, qhandle_t hShader );
void			trap_R_DrawStretchPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader );
void			trap_R_DrawRotatedPic( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader, float angle );
void			trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );
void			trap_UpdateScreen( void );
int				trap_CM_LerpTag( orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex );
void			trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum );
sfxHandle_t		trap_S_RegisterSound( const char *sample, qboolean compressed );
void			trap_Key_KeynumToStringBuf( int keynum, char *buf, int buflen );
void			trap_Key_GetBindingBuf( int keynum, char *buf, int buflen );
void			trap_Key_KeysForBinding( const char* binding, int* key1, int* key2 );
void			trap_Key_SetBinding( int keynum, const char *binding );
qboolean		trap_Key_IsDown( int keynum );
qboolean		trap_Key_GetOverstrikeMode( void );
void			trap_Key_SetOverstrikeMode( qboolean state );
void			trap_Key_ClearStates( void );
int				trap_Key_GetCatcher( void );
void			trap_Key_SetCatcher( int catcher );
void			trap_GetClipboardData( char *buf, int bufsize );
void			trap_GetClientState( uiClientState_t *state );
void			trap_GetGlconfig( glconfig_t *glconfig );
int				trap_GetConfigString( int index, char* buff, int buffsize );
int				trap_LAN_GetServerCount( int source );			// NERVE - SMF
int				trap_LAN_GetLocalServerCount( void );
void			trap_LAN_GetLocalServerAddressString( int n, char *buf, int buflen );
int				trap_LAN_GetGlobalServerCount( void );
void			trap_LAN_GetGlobalServerAddressString( int n, char *buf, int buflen );
int				trap_LAN_GetPingQueueCount( void );
void			trap_LAN_ClearPing( int n );
void			trap_LAN_GetPing( int n, char *buf, int buflen, int *pingtime );
void			trap_LAN_GetPingInfo( int n, char *buf, int buflen );
int				trap_MemoryRemaining( void );

// NERVE - SMF - multiplayer traps
qboolean		trap_LAN_UpdateVisiblePings( int source );
void			trap_LAN_MarkServerVisible(int source, int n, qboolean visible);
void			trap_LAN_ResetPings(int n);
void			trap_LAN_SaveCachedServers();
int				trap_LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 );
void			trap_LAN_GetServerAddressString( int source, int n, char *buf, int buflen );
void 			trap_LAN_GetServerInfo( int source, int n, char *buf, int buflen );
int				trap_LAN_AddServer(int source, const char *name, const char *addr);
void			trap_LAN_RemoveServer(int source, const char *addr);
int				trap_LAN_GetServerPing( int source, int n );
int				trap_LAN_ServerIsVisible( int source, int n);
int				trap_LAN_ServerStatus( const char *serverAddress, char *serverStatus, int maxLen );
void			trap_LAN_SaveCachedServers();
void			trap_LAN_LoadCachedServers();
qboolean		trap_LAN_ServerIsInFavoriteList( int source, int n );

// IRATA_ remove in future ?
void			trap_SetPbClStatus( int status );								// DHM - Nerve
void			trap_SetPbSvStatus( int status );								// TTimo


// -NERVE - SMF
void			trap_GetCDKey( char *buf, int buflen );
void			trap_SetCDKey( char *buf );
void			trap_R_RegisterFont(const char *pFontname, int pointSize, fontInfo_t *font);
void			trap_S_StopBackgroundTrack( void );
void			trap_S_StartBackgroundTrack( const char *intro, const char *loop, int fadeupTime);
void			trap_S_FadeAllSound( float targetvol, int time, qboolean stopsound );
int				trap_CIN_PlayCinematic( const char *arg0, int xpos, int ypos, int width, int height, int bits);
e_status		trap_CIN_StopCinematic(int handle);
e_status		trap_CIN_RunCinematic (int handle);
void			trap_CIN_DrawCinematic (int handle);
void			trap_CIN_SetExtents (int handle, int x, int y, int w, int h);
int				trap_RealTime(qtime_t *qtime);
void			trap_R_RemapShader( const char *oldShader, const char *newShader, const char *timeOffset );
qboolean		trap_VerifyCDKey( const char *key, const char *chksum);
qboolean		trap_GetLimboString( int index, char *buf );			// NERVE - SMF
void			trap_CheckAutoUpdate( void );							// DHM - Nerve
void			trap_GetAutoUpdate( void );								// DHM - Nerve

void			trap_openURL( const char *url ); // TTimo
void			trap_GetHunkData( int* hunkused, int* hunkexpected );

char*			trap_TranslateString( const char *string );				// NERVE - SMF - localization


const char* UI_DescriptionForCampaign( void );
const char* UI_NameForCampaign( void );

#ifdef JAQU_WIP
// jaquboss this is something we have to transmit over network, set and use on client..
// will be in bg_public.h when it's done
	playerCharacter_t	customizedChar[NUM_PLAYER_CLASSES][2];
	playerInfo_t		renderedPlayerInfo[NUM_PLAYER_CLASSES][2];
	animScriptData_t	ui_animScriptData;

void UI_PreparePlayerRender( rectDef_t *rect, int playerClass, int playerTeam );
void UI_RegisterPlayerClasses( void );
#endif

displayContextDef_t *DC;

// core: 4:3 aspectratio is the default for this game engine..
#define RATIO43		(4.0f/3.0f)
#define RPRATIO43	(1/RATIO43)


#endif
