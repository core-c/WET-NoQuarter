// cg_syscalls.c -- this file is only included when building a dll
// cg_syscalls.asm is included instead when building a qvm
#include "cg_local.h"

// IRATA: Parts of documentation are taken from http://www.quake3world.com/ubb/Forum4/HTML/005563.html - THX !

static int (QDECL *syscall)( int arg, ... ) = (int (QDECL *)( int, ...))-1;

#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export on
#endif
#endif
void dllEntry( int (QDECL  *syscallptr)( int arg,... ) ) {
	syscall = syscallptr;
}
#if defined(__MACOS__)
#ifndef __GNUC__
#pragma export off
#endif
#endif


/*int PASSFLOAT( float x ) {
	float	floatTemp;
	floatTemp = x;
	return *(int *)&floatTemp;
}*/


#define PASSFLOAT( x ) (*(int*)&x)

void trap_PumpEventLoop( void ) {
	if( !cgs.initing ) {
		return;
	}
	syscall( CG_PUMPEVENTLOOP );
}


void	trap_Print( const char *fmt ) {
	syscall( CG_PRINT, fmt );
}

void	trap_Error( const char *fmt ) {
	syscall( CG_ERROR, fmt );
}

int		trap_Milliseconds( void ) {
	return syscall( CG_MILLISECONDS );
}

void	trap_Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags ) {
	syscall( CG_CVAR_REGISTER, vmCvar, varName, defaultValue, flags );
}

void	trap_Cvar_Update( vmCvar_t *vmCvar ) {
	syscall( CG_CVAR_UPDATE, vmCvar );
}

void	trap_Cvar_Set( const char *var_name, const char *value ) {
	syscall( CG_CVAR_SET, var_name, value );
}

void trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize ) {
	syscall( CG_CVAR_VARIABLESTRINGBUFFER, var_name, buffer, bufsize );
}

void trap_Cvar_LatchedVariableStringBuffer( const char *var_name, char *buffer, int bufsize ) {
	syscall( CG_CVAR_LATCHEDVARIABLESTRINGBUFFER, var_name, buffer, bufsize );
}

int	trap_Argc( void ) {
	return syscall( CG_ARGC );
}

void	trap_Argv( int n, char *buffer, int bufferLength ) {
	syscall( CG_ARGV, n, buffer, bufferLength );
}

void	trap_Args( char *buffer, int bufferLength ) {
	syscall( CG_ARGS, buffer, bufferLength );
}

int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode ) {
	return syscall( CG_FS_FOPENFILE, qpath, f, mode );
}

void	trap_FS_Read( void *buffer, int len, fileHandle_t f ) {
	syscall( CG_FS_READ, buffer, len, f );
}

void	trap_FS_Write( const void *buffer, int len, fileHandle_t f ) {
	syscall( CG_FS_WRITE, buffer, len, f );
}

void	trap_FS_FCloseFile( fileHandle_t f ) {
	syscall( CG_FS_FCLOSEFILE, f );
}

int trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize ) {
	return syscall( CG_FS_GETFILELIST, path, extension, listbuf, bufsize );
}

int trap_FS_Delete( const char *filename ) {
	return syscall( CG_FS_DELETEFILE, filename);
}

void	trap_SendConsoleCommand( const char *text ) {
	syscall( CG_SENDCONSOLECOMMAND, text );
}

void	trap_AddCommand( const char *cmdName ) {
	syscall( CG_ADDCOMMAND, cmdName );
}


/*
===================
trap_RemoveCommand

cmdName: command name

Although this trap is not actually used anywhere in baseq3 game source, a reasonable assumption, based on
function name and parameters, is that this trap removes a command name from the list of command completions.
That is, the inverse of trap_AddCommand().

Unknown: case sensitivity (i.e. if capitalization matters for string matching)
===================
*/
void	trap_RemoveCommand( const char *cmdName ) {
	syscall( CG_REMOVECOMMAND, cmdName );
}


/*
===================
trap_SendClientCommand

===================
*/
void	trap_SendClientCommand( const char *s ) {
	syscall( CG_SENDCLIENTCOMMAND, s );
}

void	trap_UpdateScreen( void ) {
	syscall( CG_UPDATESCREEN );
}

/*void	trap_CM_LoadMap( const char *mapname ) {
	CG_DrawInformation();
	syscall( CG_CM_LOADMAP, mapname );
}*/

int		trap_CM_NumInlineModels( void ) {
	return syscall( CG_CM_NUMINLINEMODELS );
}

clipHandle_t trap_CM_InlineModel( int index ) {
	return syscall( CG_CM_INLINEMODEL, index );
}

clipHandle_t trap_CM_TempBoxModel( const vec3_t mins, const vec3_t maxs ) {
	return syscall( CG_CM_TEMPBOXMODEL, mins, maxs );
}

clipHandle_t trap_CM_TempCapsuleModel( const vec3_t mins, const vec3_t maxs ) {
	return syscall( CG_CM_TEMPCAPSULEMODEL, mins, maxs );
}

int		trap_CM_PointContents( const vec3_t p, clipHandle_t model ) {
	return syscall( CG_CM_POINTCONTENTS, p, model );
}

int		trap_CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles ) {
	return syscall( CG_CM_TRANSFORMEDPOINTCONTENTS, p, model, origin, angles );
}

void	trap_CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask ) {
	syscall( CG_CM_BOXTRACE, results, start, end, mins, maxs, model, brushmask );
}

void	trap_CM_TransformedBoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask,
						  const vec3_t origin, const vec3_t angles ) {
	syscall( CG_CM_TRANSFORMEDBOXTRACE, results, start, end, mins, maxs, model, brushmask, origin, angles );
}

void	trap_CM_CapsuleTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask ) {
	syscall( CG_CM_CAPSULETRACE, results, start, end, mins, maxs, model, brushmask );
}

void	trap_CM_TransformedCapsuleTrace( trace_t *results, const vec3_t start, const vec3_t end,
						  const vec3_t mins, const vec3_t maxs,
						  clipHandle_t model, int brushmask,
						  const vec3_t origin, const vec3_t angles ) {
	syscall( CG_CM_TRANSFORMEDCAPSULETRACE, results, start, end, mins, maxs, model, brushmask, origin, angles );
}

int		trap_CM_MarkFragments( int numPoints, const vec3_t *points,
				const vec3_t projection,
				int maxPoints, vec3_t pointBuffer,
				int maxFragments, markFragment_t *fragmentBuffer ) {
	return syscall( CG_CM_MARKFRAGMENTS, numPoints, points, projection, maxPoints, pointBuffer, maxFragments, fragmentBuffer );
}

// ydnar
void		trap_R_ProjectDecal( qhandle_t hShader, int numPoints, vec3_t *points, vec4_t projection, vec4_t color, int lifeTime, int fadeTime )
{
	syscall( CG_R_PROJECTDECAL, hShader, numPoints, points, projection, color, lifeTime, fadeTime );
}

void		trap_R_ClearDecals( void )
{
	syscall( CG_R_CLEARDECALS );
}


void	trap_S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx ) {
		syscall( CG_S_STARTSOUND, origin, entityNum, entchannel, sfx, 127 /* Gordon: default volume always for the moment*/ );
}

void	trap_S_StartSoundVControl( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx, int volume ) {
		syscall( CG_S_STARTSOUND, origin, entityNum, entchannel, sfx, volume );
}

//----(SA)	added
void	trap_S_StartSoundEx( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx, int flags ) {
	syscall( CG_S_STARTSOUNDEX, origin, entityNum, entchannel, sfx, flags, 127 /* Gordon: default volume always for the moment*/ );
}
//----(SA)	end

void	trap_S_StartSoundExVControl( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx, int flags, int volume ) {
	syscall( CG_S_STARTSOUNDEX, origin, entityNum, entchannel, sfx, flags, volume );
}

void	trap_S_StartLocalSound( sfxHandle_t sfx, int channelNum ) {
	syscall( CG_S_STARTLOCALSOUND, sfx, channelNum, 127 /* Gordon: default volume always for the moment*/ );
}

void	trap_S_ClearLoopingSounds( void ) {
	syscall( CG_S_CLEARLOOPINGSOUNDS );
}

void	trap_S_ClearSounds( qboolean killmusic ) {
	syscall( CG_S_CLEARSOUNDS, killmusic );
}

void	trap_S_AddLoopingSound( const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int volume, int soundTime ) {
	syscall( CG_S_ADDLOOPINGSOUND, origin, velocity, 1250, sfx, volume, soundTime );		// volume was previously removed from CG_S_ADDLOOPINGSOUND.  I added 'range'
}

void	trap_S_AddRealLoopingSound( const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int range, int volume, int soundTime ) {
	syscall( CG_S_ADDREALLOOPINGSOUND, origin, velocity, range, sfx, volume, soundTime );
}

void	trap_S_StopStreamingSound(int entityNum) {
	syscall( CG_S_STOPSTREAMINGSOUND, entityNum );
}

void	trap_S_UpdateEntityPosition( int entityNum, const vec3_t origin ) {
	syscall( CG_S_UPDATEENTITYPOSITION, entityNum, origin );
}

// Ridah, talking animations
int		trap_S_GetVoiceAmplitude( int entityNum ) {
	return syscall( CG_S_GETVOICEAMPLITUDE, entityNum );
}
// done.

void	trap_S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater ) {
	syscall( CG_S_RESPATIALIZE, entityNum, origin, axis, inwater );
}

/*sfxHandle_t	trap_S_RegisterSound( const char *sample, qboolean compressed ) {
	CG_DrawInformation();
	return syscall( CG_S_REGISTERSOUND, sample, compressed );
}*/

int trap_S_GetSoundLength(sfxHandle_t sfx)
{
	return syscall( CG_S_GETSOUNDLENGTH, sfx );
}

// ydnar: for timing looped sounds
int trap_S_GetCurrentSoundTime( void )
{
	return syscall( CG_S_GETCURRENTSOUNDTIME );
}

void trap_S_StartBackgroundTrack( const char *intro, const char *loop, int fadeupTime ) {
	syscall( CG_S_STARTBACKGROUNDTRACK, intro, loop, fadeupTime );
}

void trap_S_FadeBackgroundTrack( float targetvol, int time, int num) {
	syscall( CG_S_FADESTREAMINGSOUND, PASSFLOAT(targetvol), time, num);	// 'num' is '0' if it's music, '1' if it's "all streaming sounds"
}

void	trap_S_FadeAllSound( float targetvol, int time, qboolean stopsounds) {
	syscall( CG_S_FADEALLSOUNDS, PASSFLOAT(targetvol), time, stopsounds);
}

int	trap_S_StartStreamingSound( const char *intro, const char *loop, int entnum, int channel, int attenuation ) {
	return syscall( CG_S_STARTSTREAMINGSOUND, intro, loop, entnum, channel, attenuation );
}

/*void	trap_R_LoadWorldMap( const char *mapname ) {
	CG_DrawInformation();
	syscall( CG_R_LOADWORLDMAP, mapname );
}

qhandle_t trap_R_RegisterModel( const char *name ) {
	CG_DrawInformation();
	return syscall( CG_R_REGISTERMODEL, name );
}*/

//----(SA)	added
qboolean trap_R_GetSkinModel( qhandle_t skinid, const char *type, char *name ) {
	return syscall( CG_R_GETSKINMODEL, skinid, type, name);
}

qhandle_t trap_R_GetShaderFromModel( qhandle_t modelid, int surfnum, int withlightmap) {
	return syscall( CG_R_GETMODELSHADER, modelid, surfnum, withlightmap);
}
//----(SA)	end

void	trap_R_ClearScene( void ) {
	syscall( CG_R_CLEARSCENE );
}

void	trap_R_AddRefEntityToScene( const refEntity_t *re ) {
	syscall( CG_R_ADDREFENTITYTOSCENE, re );
}

void	trap_R_AddPolyToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts ) {
	syscall( CG_R_ADDPOLYTOSCENE, hShader, numVerts, verts );
}

void	trap_R_AddPolyBufferToScene( polyBuffer_t* pPolyBuffer ) {
	syscall( CG_R_ADDPOLYBUFFERTOSCENE, pPolyBuffer );
}

// Ridah
void	trap_R_AddPolysToScene( qhandle_t hShader , int numVerts, const polyVert_t *verts, int numPolys ) {
	syscall( CG_R_ADDPOLYSTOSCENE, hShader, numVerts, verts, numPolys );
}


/*
=================
trap_R_AddLightToScene

const vec3_t org: origin of the light - where the light will be.

float intensity: not the exact intensity of the light, but the radius of the light - the light is a sphere-like object
and points within that object are lighted up, and the closer something is to the origin of the light (org) the closer
the light will be to the actual colour data of the light.

float r/g/b: this is the colour data of the light (normalized, so the highest value, 255, is 1.0) - r is the amount of
red light, g green light, and b blue light.

This function will add a light to the game at org, which has a radius of intensity units, and the colour data which is shown in the variables r, g, and b. The highest number
for an RGB index is 255, but in Quake 3 the colour's index is divided by 255 to give a normalized value.

Say I want to make a purple light. The colour data for purple in RGB is 255,0,255. So the normalized values for this colour are:

255/255 = 1.0
0/255 = 0.0
255/255 = 1.0

So to add a purple light at the vector x, and radius 2.4 I would write:
trap_R_AddLightToScene( x, 2.4, 1.0, 0.0, 1.0 );
=================
*/
void	trap_R_AddLightToScene( const vec3_t org, float radius, float intensity, float r, float g, float b, qhandle_t hShader, int flags )
{
	syscall( CG_R_ADDLIGHTTOSCENE, org, PASSFLOAT( radius ), PASSFLOAT( intensity ),
		PASSFLOAT( r ), PASSFLOAT( g ), PASSFLOAT( b ), hShader, flags );
}

//----(SA)
void	trap_R_AddCoronaToScene( const vec3_t org, float r, float g, float b, float scale, int id, qboolean visible) {
	syscall( CG_R_ADDCORONATOSCENE, org, PASSFLOAT(r), PASSFLOAT(g), PASSFLOAT(b), PASSFLOAT(scale), id, visible);
}


//----(SA)
void	trap_R_SetFog( int fogvar, int var1, int var2, float r, float g, float b, float density ) {
	syscall( CG_R_SETFOG, fogvar, var1, var2, PASSFLOAT(r), PASSFLOAT(g), PASSFLOAT(b), PASSFLOAT(density) );
}


void	trap_R_SetGlobalFog( qboolean restore, int duration, float r, float g, float b, float depthForOpaque ) {
	syscall( CG_R_SETGLOBALFOG, restore, duration, PASSFLOAT(r), PASSFLOAT(g), PASSFLOAT(b), PASSFLOAT(depthForOpaque) );
}

void	trap_R_RenderScene( const refdef_t *fd ) {
	syscall( CG_R_RENDERSCENE, fd );
}

// Mad Doctor I, 11/4/2002.
void	trap_R_SaveViewParms(  )
{
	syscall( CG_R_SAVEVIEWPARMS );
}

// Mad Doctor I, 11/4/2002.
void	trap_R_RestoreViewParms(  )
{
	syscall( CG_R_RESTOREVIEWPARMS );
}

void	trap_R_SetColor( const float *rgba ) {
	syscall( CG_R_SETCOLOR, rgba );
}

void	trap_R_DrawStretchPic( float x, float y, float w, float h,
							   float s1, float t1, float s2, float t2, qhandle_t hShader ) {
	syscall( CG_R_DRAWSTRETCHPIC, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(w), PASSFLOAT(h), PASSFLOAT(s1), PASSFLOAT(t1), PASSFLOAT(s2), PASSFLOAT(t2), hShader );
}

void	trap_R_DrawRotatedPic( float x, float y, float w, float h,
							   float s1, float t1, float s2, float t2, qhandle_t hShader, float angle ) {
	syscall( CG_R_DRAWROTATEDPIC, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(w), PASSFLOAT(h), PASSFLOAT(s1), PASSFLOAT(t1), PASSFLOAT(s2), PASSFLOAT(t2), hShader, PASSFLOAT(angle) );
}

void	trap_R_DrawStretchPicGradient(	float x, float y, float w, float h,
										float s1, float t1, float s2, float t2, qhandle_t hShader,
										const float *gradientColor, int gradientType ) {
	syscall( CG_R_DRAWSTRETCHPIC_GRADIENT, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(w), PASSFLOAT(h), PASSFLOAT(s1), PASSFLOAT(t1), PASSFLOAT(s2), PASSFLOAT(t2), hShader, gradientColor, gradientType  );
}

void trap_R_Add2dPolys( polyVert_t* verts, int numverts, qhandle_t hShader ) {
	syscall( CG_R_DRAW2DPOLYS, verts, numverts, hShader );
}


void	trap_R_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs ) {
	syscall( CG_R_MODELBOUNDS, model, mins, maxs );
}

int		trap_R_LerpTag( orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex ) {
	return syscall( CG_R_LERPTAG, tag, refent, tagName, startIndex );
}

void	trap_R_RemapShader( const char *oldShader, const char *newShader, const char *timeOffset ) {
	syscall( CG_R_REMAP_SHADER, oldShader, newShader, timeOffset );
}

void		trap_GetGlconfig( glconfig_t *glconfig ) {
	syscall( CG_GETGLCONFIG, glconfig );
}

void		trap_GetGameState( gameState_t *gamestate ) {
	syscall( CG_GETGAMESTATE, gamestate );
}

#ifdef _DEBUG
//#define FAKELAG
#ifdef FAKELAG
#define	MAX_SNAPSHOT_BACKUP	256
#define	MAX_SNAPSHOT_MASK	(MAX_SNAPSHOT_BACKUP - 1)

static snapshot_t snaps[MAX_SNAPSHOT_BACKUP];
static int curSnapshotNumber;
int snapshotDelayTime;
static qboolean skiponeget;
#endif // FAKELAG
#endif // _DEBUG

void		trap_GetCurrentSnapshotNumber( int *snapshotNumber, int *serverTime ) {
	syscall( CG_GETCURRENTSNAPSHOTNUMBER, snapshotNumber, serverTime );

#ifdef FAKELAG
	{
		char s[MAX_STRING_CHARS];
		int fakeLag;

		trap_Cvar_VariableStringBuffer( "g_fakelag", s, sizeof(s) );
		fakeLag = atoi(s);
		if( fakeLag < 0 )
			fakeLag = 0;

		if( fakeLag ) {
			if( curSnapshotNumber < cg.latestSnapshotNum ) {
				*snapshotNumber = cg.latestSnapshotNum + 1;
				curSnapshotNumber = cg.latestSnapshotNum + 2;	// skip one ahead and we're good to go on the next frame
				skiponeget = qtrue;
			} else
				*snapshotNumber = curSnapshotNumber;
		}
	}
#endif // FAKELAG
}

qboolean	trap_GetSnapshot( int snapshotNumber, snapshot_t *snapshot ) {
#ifndef FAKELAG
	return syscall( CG_GETSNAPSHOT, snapshotNumber, snapshot );
#else
	{
		char s[MAX_STRING_CHARS];
		int fakeLag;

		if( skiponeget)
		syscall( CG_GETSNAPSHOT, snapshotNumber, snapshot );

		trap_Cvar_VariableStringBuffer( "g_fakelag", s, sizeof(s) );
		fakeLag = atoi(s);
		if( fakeLag < 0 )
			fakeLag = 0;

		if( fakeLag ) {
			int i;
			int realsnaptime, thissnaptime;

			// store our newest usercmd
			curSnapshotNumber++;
			memcpy( &snaps[curSnapshotNumber & MAX_SNAPSHOT_MASK], snapshot, sizeof(snapshot_t) );

			// find a usercmd that is fakeLag msec behind
			i = curSnapshotNumber & MAX_SNAPSHOT_MASK;
			realsnaptime = snaps[i].serverTime;
			i--;
			do {
				thissnaptime = snaps[i & MAX_SNAPSHOT_MASK].serverTime;

				if( realsnaptime - thissnaptime > fakeLag ) {
					// found the right one
					snapshotDelayTime = realsnaptime - thissnaptime;
                    snapshot = &snaps[i & MAX_SNAPSHOT_MASK];
					//*snapshotNumber = i & MAX_SNAPSHOT_MASK;
					return qtrue;
				}

				i--;
			} while ( (i & MAX_SNAPSHOT_MASK) != (curSnapshotNumber & MAX_SNAPSHOT_MASK) );

			// didn't find a proper one, just use the oldest one we have
			snapshotDelayTime = realsnaptime - thissnaptime;
			snapshot = &snaps[(curSnapshotNumber - 1) & MAX_SNAPSHOT_MASK];
			//*snapshotNumber = (curSnapshotNumber - 1) & MAX_SNAPSHOT_MASK;
			return qtrue;
		} else
			return syscall( CG_GETSNAPSHOT, snapshotNumber, snapshot );
	}
#endif // FAKELAG
}

qboolean	trap_GetServerCommand( int serverCommandNumber ) {
	return syscall( CG_GETSERVERCOMMAND, serverCommandNumber );
}

int			trap_GetCurrentCmdNumber( void ) {
	return syscall( CG_GETCURRENTCMDNUMBER );
}

qboolean	trap_GetUserCmd( int cmdNumber, usercmd_t *ucmd ) {
	return syscall( CG_GETUSERCMD, cmdNumber, ucmd );
}

void		trap_SetUserCmdValue( int stateValue, int flags, float sensitivityScale, int mpIdentClient ) {
	syscall( CG_SETUSERCMDVALUE, stateValue, flags, PASSFLOAT(sensitivityScale), mpIdentClient );
}

void		trap_SetClientLerpOrigin( float x, float y, float z ) {
	syscall( CG_SETCLIENTLERPORIGIN, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(z) );
}

void		testPrintInt( char *string, int i ) {
	syscall( CG_TESTPRINTINT, string, i );
}

void		testPrintFloat( char *string, float f ) {
	syscall( CG_TESTPRINTFLOAT, string, PASSFLOAT(f) );
}

int trap_MemoryRemaining( void ) {
	return syscall( CG_MEMORY_REMAINING );
}

/*
=================
trap_Key_IsDown

keynum: A key keynum

Returns true is the indicated key is held down, returns false if not held down (released).

The values of keynum range from 0 to 255, inclusive. For the most part, a key's keynum corresponds to its character's ASCII value (e.g. the 'A' key is 65).

I have a list of keynames used by the bind command with their keynums which I may append at a later date. In the meantime, if you want to know them now, you can do this:

Create a keynums.cfg with a pattern like:
bind 0x00 Key 0x00
bind 0x01 Key 0x01
...
bind 0xfe Key 0xfe
bind 0xff Key 0xff

(case matters for the hex values, and a scripting language to automate this really helps)

Back up your q3config.cfg, run Q3, "/exec keynums.cfg; bindlist; condump keynums.txt; quit"

In the file "keynums.txt" you should now have lines like:
UPARROW "Key 0x84"

There's your keynum list.


(Generating keynums.cfg in GNU bash:
for i in $(seq 0 255); do x=$(printf "%02x" $i); echo bind 0x$x Key 0x$x; done > keynums.cfg
)
=================
*/
qboolean trap_Key_IsDown( int keynum ) {
	return syscall( CG_KEY_ISDOWN, keynum );
}

/*
=================
trap_Key_GetCatcher

Returns a bitmask representing active key catchers.

Bitmask values are:
KEYCATCH_CONSOLE
KEYCATCH_UI
KEYCATCH_MESSAGE
KEYCATCH_CGAME

(see also trap_Key_SetCatcher())

To check if CGAME key-catching is active:
if (trap_Key_GetCatcher() & KEYCATCH_UI) {
	// happy happy
}
=================
*/
int trap_Key_GetCatcher( void ) {
	return syscall( CG_KEY_GETCATCHER );
}

qboolean trap_Key_GetOverstrikeMode( void ) {
	return syscall( CG_KEY_GETOVERSTRIKEMODE );
}

void trap_Key_SetOverstrikeMode( qboolean state ) {
	syscall( CG_KEY_SETOVERSTRIKEMODE, state );
}

// binding MUST be lower case
void trap_Key_KeysForBinding( const char* binding, int* key1, int* key2 ) {
	syscall( CG_KEY_BINDINGTOKEYS, binding, key1, key2 );
}


/*
=================
catcher: Key catchers bitmask

Redirects key events to different module(s).

Normally, key events are interpreted by the engine, affected by the key bindings. This trap allows you redirect key events to other components.

Valid catchers are:
KEYCATCH_CONSOLE: the console
KEYCATCH_UI: the ui module
KEYCATCH_MESSAGE: the little message editor thingy
KEYCATCH_CGAME: the cgame module

These are bitmasked values, meaning you can combine them with the OR operator (|), as in (KEYCATCH_UI|KEYCATCH_CGAME), but I recommend against trying multiple catchers (it may get confusing).

As a mods programmer, you may be most interested in KEYCATCH_CGAME, and on this I will concentrate this entry.

--- The Keyboard ---

When key-catching is enabled (trap_Key_SetCatcher(KEYCATCH_CGAME);), upon a change in key state (gets pressed or gets released), the engine calls the vmMain() function in cg_main.c with the following parameters:
command = CG_KEY_EVENT
arg0 = keynum
arg1 = 1 if pressed, 0 if released.

In baseq3 game source, this event is already handled and delegated to CG_KeyEvent(), which is empty.

The values for keynum are the same as for trap_Key_IsDown().

Key catching is disabled by calling with a parameter of 0 (as in, no catchers). Key catching is also terminated if the user presses the ESC key, which means you won't receive events on the ESC key.

--- The Mouse ---

When key-catching is enabled, mouse movement is also redirected to the specified module. When the engine detects mouse movement, it calls vmMain() with the following parameters:
command = CG_MOUSE_EVENT
arg0 = relative X position
arg1 = relative Y position

The game source for baseq3 delegates this event to CG_MouseEvent().

Pointer Device Movement - Reported Value:
Left - Negative X
Right - Positive X
Up - Negative Y
Down - Positive Y

The engine only reports mouse movement -- if the mouse doesn't move, there is no mouse event. The engine reports mouse buttons and wheels (if any) as key events, for binding purposes.

Cvars do not influence the reported values.

=================
*/
void trap_Key_SetCatcher( int catcher ) {
	syscall( CG_KEY_SETCATCHER, catcher );
}


/*
=================
trap_Key_GetKey

binding: command string presumably bound to a key.

Returns keynum of the key that has the binding specified by binding. Case-sensitive.

Unknown: what happens when many keys have this binding.
=================
*/
int trap_Key_GetKey( const char *binding ) {
	return syscall( CG_KEY_GETKEY, binding );
}


int trap_PC_AddGlobalDefine( char *define ) {
	return syscall( CG_PC_ADD_GLOBAL_DEFINE, define );
}

int trap_PC_LoadSource( const char *filename ) {
	return syscall( CG_PC_LOAD_SOURCE, filename );
}

int trap_PC_FreeSource( int handle ) {
	return syscall( CG_PC_FREE_SOURCE, handle );
}

int trap_PC_ReadToken( int handle, pc_token_t *pc_token ) {
	return syscall( CG_PC_READ_TOKEN, handle, pc_token );
}

int trap_PC_SourceFileAndLine( int handle, char *filename, int *line ) {
	return syscall( CG_PC_SOURCE_FILE_AND_LINE, handle, filename, line );
}

int trap_PC_UnReadToken( int handle ) {
	return syscall( CG_PC_UNREAD_TOKEN, handle );
}

void	trap_S_StopBackgroundTrack( void ) {
	syscall( CG_S_STOPBACKGROUNDTRACK );
}

int trap_RealTime(qtime_t *qtime) {
	return syscall( CG_REAL_TIME, qtime );
}

void trap_SnapVector( float *v ) {
	syscall( CG_SNAPVECTOR, v );
}

#if 0
/*
=================
trap_CIN_PlayCinematic

arg0 - the name of the .RoQ file to load.
xpos - the X coordinate on-screen for the left edge of the cinematic display.
ypos - the Y coordinate on-screen for the top edge of the cinematic display.
width - the screen width of the cinematic display.
height - the screen height of the cinematic display.

bits - combinations of the following flags:
# CIN_system
# CIN_loop
# CIN_hold
# CIN_silent
# CIN_shader
OR them together to use multiple. eg. CIN_loop|CIN_silent

This function loads up and begins playing an ROQ file at the coordinates given.
If the CIN_hold flag is specified, the ROQ is loaded in a paused state.
The function returns a handle to the cinematic instance or zero on failure.
=================
*/
int trap_CIN_PlayCinematic( const char *arg0, int xpos, int ypos, int width, int height, int bits) {
	return syscall(CG_CIN_PLAYCINEMATIC, arg0, xpos, ypos, width, height, bits);
}


/*
=================
trap_CIN_StopCinematic

handle - the handle returned by a previous call to trap_CIN_PlayCinematic

Stops playing the cinematic and ends it. should always return FMV_EOF
cinematics must be stopped in reverse order of when they are started
=================
*/
e_status trap_CIN_StopCinematic(int handle) {
	return syscall(CG_CIN_STOPCINEMATIC, handle);
}


/*
=================
trap_CIN_RunCinematic

handle a handle returned by a previous call to trap_CIN_PlayCinematic

Will run a frame of the cinematic but will not draw it. Will return FMV_EOF if the end of the cinematic has been reached.
=================
*/
e_status trap_CIN_RunCinematic(int handle) {
	return syscall(CG_CIN_RUNCINEMATIC, handle);
}


/*
===================
trap_CIN_DrawCinematic

handle - a handle returned by a previous call to trap_CIN_PlayCinematic

This function renders the current frame of the cinematic to the screen with the current position coordinates.
===================
*/
void trap_CIN_DrawCinematic (int handle) {
	syscall(CG_CIN_DRAWCINEMATIC, handle);
}


/*
===================
trap_CIN_SetExtents

handle - a handle returned by a previous call to trap_CIN_PlayCinematic

x - the new X coordinate on-screen of the left edge of the cinematic.
y - the new Y coordinate on-screen of the top edge of the cinematic.
w - the new width on-screen of the cinematic.
h - the new height on-screen of the cinematic.

Allows you to resize the animation dynamically
===================
*/
void trap_CIN_SetExtents (int handle, int x, int y, int w, int h) {
	syscall(CG_CIN_SETEXTENTS, handle, x, y, w, h);
}
#endif


qboolean trap_GetEntityToken( char *buffer, int bufferSize ) {
	return syscall( CG_GET_ENTITY_TOKEN, buffer, bufferSize );
}

//----(SA)	added
// bring up a popup menu
extern void	Menus_OpenByName(const char *p);

//void trap_UI_Popup( const char *arg0) {
void trap_UI_Popup( int arg0 ) {
	syscall(CG_INGAME_POPUP, arg0);
}

void trap_UI_ClosePopup( const char *arg0) {
	syscall(CG_INGAME_CLOSEPOPUP, arg0);
}

void trap_Key_GetBindingBuf( int keynum, char *buf, int buflen ) {
	syscall( CG_KEY_GETBINDINGBUF, keynum, buf, buflen );
}

void trap_Key_SetBinding( int keynum, const char *binding ) {
	syscall( CG_KEY_SETBINDING, keynum, binding );
}

void trap_Key_KeynumToStringBuf( int keynum, char *buf, int buflen ) {
	syscall( CG_KEY_KEYNUMTOSTRINGBUF, keynum, buf, buflen );
}

void trap_TranslateString( const char *string, char *buf ) {
	syscall( CG_TRANSLATE_STRING, string, buf );
}
// -NERVE - SMF

// Media register functions
#ifdef _DEBUG

#if 0 // show loading time
#define DEBUG_REGISTERPROFILE_INIT int dbgTime = trap_Milliseconds();
#define DEBUG_REGISTERPROFILE_EXEC(f,n) if( developer.integer ) CG_Printf("%s : loaded %s in %i msec\n", f, n, trap_Milliseconds()-dbgTime );
#else // always show hunkmegs for debug builds
#define DEBUG_REGISTERPROFILE_INIT int size = trap_MemoryRemaining();
#define DEBUG_REGISTERPROFILE_EXEC(f,n) if ( size-trap_MemoryRemaining() > 0 ) CG_WriteToLog("%s : %s %i\n", f, n, size-trap_MemoryRemaining() );
#endif

sfxHandle_t	trap_S_RegisterSound( const char *sample, qboolean compressed ) {
	sfxHandle_t snd;
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	snd = syscall( CG_S_REGISTERSOUND, sample, qfalse /* compressed */ );
	if(!*sample) {
		Com_Printf("^1Warning: Null Sample filename\n");
	}
	if(snd == 0) {
		Com_Printf("^1Warning: Failed to load sound: %s\n", sample);
	}
	DEBUG_REGISTERPROFILE_EXEC("trap_S_RegisterSound",sample)
	trap_PumpEventLoop();
	return snd;
}

qhandle_t trap_R_RegisterModel( const char *name ) {
	qhandle_t handle;
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	handle = syscall( CG_R_REGISTERMODEL, name );
	DEBUG_REGISTERPROFILE_EXEC("trap_R_RegisterModel",name)
	trap_PumpEventLoop();
	return handle;
}

qhandle_t trap_R_RegisterSkin( const char *name ) {
	qhandle_t handle;
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	handle = syscall( CG_R_REGISTERSKIN, name );
	DEBUG_REGISTERPROFILE_EXEC("trap_R_RegisterSkin",name)
	trap_PumpEventLoop();
	return handle;
}

qhandle_t trap_R_RegisterShader( const char *name ) {
	qhandle_t handle;
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	handle = syscall( CG_R_REGISTERSHADER, name );
	DEBUG_REGISTERPROFILE_EXEC("trap_R_RegisterShader",name)
	trap_PumpEventLoop();
	return handle;
}

qhandle_t trap_R_RegisterShaderNoMip( const char *name ) {
	qhandle_t handle;
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	handle = syscall( CG_R_REGISTERSHADERNOMIP, name );
	trap_PumpEventLoop();
	DEBUG_REGISTERPROFILE_EXEC("trap_R_RegisterShaderNpMip", name);
	return handle;
}

void trap_R_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font) {
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	syscall(CG_R_REGISTERFONT, fontName, pointSize, font );
	DEBUG_REGISTERPROFILE_EXEC("trap_R_RegisterFont",fontName)
	trap_PumpEventLoop();
}

void	trap_CM_LoadMap( const char *mapname ) {
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	syscall( CG_CM_LOADMAP, mapname );
	DEBUG_REGISTERPROFILE_EXEC("trap_CM_LoadMap",mapname)
	trap_PumpEventLoop();
}

void	trap_R_LoadWorldMap( const char *mapname ) {
	DEBUG_REGISTERPROFILE_INIT
	CG_DrawInformation( qtrue );
	syscall( CG_R_LOADWORLDMAP, mapname );
	DEBUG_REGISTERPROFILE_EXEC("trap_R_LoadWorldMap",mapname)
	trap_PumpEventLoop();
}
#else
sfxHandle_t	trap_S_RegisterSound( const char *sample, qboolean compressed ) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	return syscall( CG_S_REGISTERSOUND, sample, qfalse /* compressed */ );
}

qhandle_t trap_R_RegisterModel( const char *name ) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	return syscall( CG_R_REGISTERMODEL, name );
}

qhandle_t trap_R_RegisterSkin( const char *name ) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	return syscall( CG_R_REGISTERSKIN, name );
}

/*
=================
trap_R_RegisterShader

const char *name: pointer to the name of the shader (?)

This function registers the name shader and gives it a unique qhandle_t.
=================
*/
qhandle_t trap_R_RegisterShader( const char *name ) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	return syscall( CG_R_REGISTERSHADER, name );
}

qhandle_t trap_R_RegisterShaderNoMip( const char *name ) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	return syscall( CG_R_REGISTERSHADERNOMIP, name );
}

void trap_R_RegisterFont(const char *fontName, int pointSize, fontInfo_t *font) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	syscall(CG_R_REGISTERFONT, fontName, pointSize, font );
}

void	trap_CM_LoadMap( const char *mapname ) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	syscall( CG_CM_LOADMAP, mapname );
}

void	trap_R_LoadWorldMap( const char *mapname ) {
	CG_DrawInformation( qtrue );
	trap_PumpEventLoop();
	syscall( CG_R_LOADWORLDMAP, mapname );
}
#endif // _DEBUG

qboolean trap_R_inPVS( const vec3_t p1, const vec3_t p2 ) {
	return syscall( CG_R_INPVS, p1, p2 );
}

void trap_GetHunkData( int* hunkused, int* hunkexpected ) {
	syscall( CG_GETHUNKDATA, hunkused, hunkexpected );
}

#define __ET_25X_SUPPORT__

#ifdef __ET_25X_SUPPORT__
qboolean isET260( void ){
	char versionStr[256];

	trap_Cvar_VariableStringBuffer( "version", versionStr, 256 );

	if (!Q_stricmp(versionStr, va("ET 2.60 %s Mar 10 2005", CPUSTRING)))
		return qtrue;

	if (!Q_stricmp(versionStr, va("ET 2.60b %s May  8 2006", CPUSTRING)))
		return qtrue;

#if defined(__MACOS__)
	//if (!Q_stricmp(versionStr, "ET 2.60d OSX-universal JAN 20 2007"))
		return qtrue;
#endif

	return qfalse;
}
#endif

//zinx - binary message channel
void trap_SendMessage( char *buf, int buflen ) {
#ifdef __ET_25X_SUPPORT__
	if (isET260())
#endif
	syscall( CG_SENDMESSAGE, buf, buflen );
}

messageStatus_t trap_MessageStatus( void ) {
#ifdef __ET_25X_SUPPORT__
	if (isET260())
#endif
	return syscall( CG_MESSAGESTATUS );
#ifdef __ET_25X_SUPPORT__
	return MESSAGE_EMPTY;
#endif
}

//bani - dynamic shaders
qboolean trap_R_LoadDynamicShader( const char *shadername, const char *shadertext ) {
#ifdef __ET_25X_SUPPORT__
	if (isET260())
#endif
	return syscall( CG_R_LOADDYNAMICSHADER, shadername, shadertext );
#ifdef __ET_25X_SUPPORT__
	return qfalse;
#endif
}

// fretn - render to texture
void trap_R_RenderToTexture( int textureid, int x, int y, int w, int h ) {
#ifdef __ET_25X_SUPPORT__
	if (isET260())
#endif
	syscall( CG_R_RENDERTOTEXTURE, textureid, x, y, w, h );
}

int trap_R_GetTextureId( const char *name ) {
#ifdef __ET_25X_SUPPORT__
	if (isET260())
#endif
	return syscall( CG_R_GETTEXTUREID, name );
#ifdef __ET_25X_SUPPORT__
	return -1;
#endif
}

// bani - sync rendering
void trap_R_Finish( void ) {
#ifdef __ET_25X_SUPPORT__
	if (isET260())
#endif
	syscall( CG_R_FINISH );
}
