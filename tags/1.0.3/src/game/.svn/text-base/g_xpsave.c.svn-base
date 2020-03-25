
#include "g_local.h"

g_xpsave_t *g_xpsaves[MAX_XPSAVES];

void G_xpsave_writeconfig() 
{
	fileHandle_t f;
	int len, i, j;
	time_t t;
	int age = 0;

	if(!(g_XPSave.integer & XPSF_ENABLE))  
		return;   
	if(!g_XPSaveFile.string[0])
		return;
	time(&t);
	len = trap_FS_FOpenFile(g_XPSaveFile.string, &f, FS_WRITE);
	if(len < 0) {
		G_Printf("G_xpsave_writeconfig: could not open %s\n",
				g_XPSaveFile.string);
	}
	for(i=0; g_xpsaves[i]; i++) {
		if(!g_xpsaves[i]->time)
			continue;
		age = t - g_xpsaves[i]->time;  
		if(age > g_XPSaveMaxAge.integer) { 
				continue;
		}

		trap_FS_Write("[xpsave]\n", 9, f);
		trap_FS_Write("guid             = ", 19, f);
		G_shrubbot_writeconfig_string(g_xpsaves[i]->guid, f);
		trap_FS_Write("name             = ", 19, f);  
		G_shrubbot_writeconfig_string(g_xpsaves[i]->name, f);
		trap_FS_Write("time             = ", 19, f);
		G_shrubbot_writeconfig_int(g_xpsaves[i]->time, f);

		if(age <= g_XPSaveMaxAge.integer) {  
			for(j=0; j<SK_NUM_SKILLS; j++) {  
					if(g_xpsaves[i]->skill[j] == 0.0f)  
								continue;  
						trap_FS_Write(va("skill[%i]         = ", j),  
								19, f);  
						G_shrubbot_writeconfig_float(  
								g_xpsaves[i]->skill[j], f);  
					}  
			} /*
			if(age <= g_XPSaveMaxAge.integer &&  
					g_xpsaves[i]->killrating != 1600) {   

	  				trap_FS_Write("killrating       = ", 19, f);  
				G_shrubbot_writeconfig_int(g_xpsaves[i]->killrating, f);  
		}  
			if(age <= g_XPSaveMaxAge.integer &&  
						g_xpsaves[i]->playerrating != 1600) {   
	
						trap_FS_Write("playerrating     = ", 19, f);  
						G_shrubbot_writeconfig_int(  
						g_xpsaves[i]->playerrating, f);  
			}  */
			if(age <= g_XPSaveMaxAge.integer && g_xpsaves[i]->muted) {  
				trap_FS_Write("muted            = ", 19, f);  
				G_shrubbot_writeconfig_int(g_xpsaves[i]->muted, f);  
		}  
		trap_FS_Write("\n", 1, f);
	}
	G_Printf("xpsave: wrote %d xpsaves\n", i);
	trap_FS_FCloseFile(f);
}

void G_xpsave_readconfig()
{
	g_xpsave_t *x = g_xpsaves[0];
	int xc = 0;
	fileHandle_t f;
	int i;
	int len;
	char *cnf, *cnf2;
	char *t;
	qboolean xpsave_open;
	float skill;

	if(!(g_XPSave.integer & XPSF_ENABLE))  
		return;    
	if(!g_XPSaveFile.string[0])
		return;
	len = trap_FS_FOpenFile(g_XPSaveFile.string, &f, FS_READ) ; 
	if(len < 0) {
		G_Printf("readconfig: could not open xpsave file %s\n",
			g_XPSaveFile.string);
		return;
	}
	cnf = malloc(len+1);
	cnf2 = cnf;
	trap_FS_Read(cnf, len, f);
	*(cnf + len) = '\0';
	trap_FS_FCloseFile(f);
	
	G_xpsave_cleanup(); 
	
	t = COM_Parse(&cnf);
	xpsave_open = qfalse;
	while(*t) {
		if(!Q_stricmp(t, "[xpsave]")) {
			
			if(xpsave_open)
				g_xpsaves[xc++] = x;
			xpsave_open = qfalse;
		}

		if(xpsave_open) {
			if(!Q_stricmp(t, "guid")) {
				G_shrubbot_readconfig_string(&cnf, 
					x->guid, sizeof(x->guid)); 
			}
			else if(!Q_stricmp(t, "name")) {  
				G_shrubbot_readconfig_string(&cnf,   
						x->name, sizeof(x->name));   
			}
			else if(!Q_stricmp(t, "time")) {
				G_shrubbot_readconfig_int(&cnf, &x->time);
			}
			else if(!Q_stricmpn(t, "skill[", 6)) {
				for(i=0; i<SK_NUM_SKILLS; i++) {
					if(Q_stricmp(t, va("skill[%i]", i)))
						continue;		
					G_shrubbot_readconfig_float(&cnf, &skill);
					x->skill[i] = skill;
					break;
				}
			}
/*			else if(!Q_stricmp(t, "killrating")) {
				G_shrubbot_readconfig_int(&cnf, &x->killrating);
			}
			else if(!Q_stricmp(t, "playerrating")) {  
				G_shrubbot_readconfig_int(&cnf,  
						&x->playerrating);  
			}*/
			else if(!Q_stricmp(t, "muted")) {
				G_shrubbot_readconfig_int(&cnf, &x->muted);
			}
			else {
				G_Printf("xpsave: [xpsave] parse error near "
					"%s on line %d\n", 
					t, 
					COM_GetCurrentParseLine());
			}
		}

		if(!Q_stricmp(t, "[xpsave]")) {
			if(xc >= MAX_XPSAVES) {
				G_Printf("xpsave: error MAX_XPSAVES exceeded");
				return;
			}
			x = malloc(sizeof(g_xpsave_t));
			x->guid[0] = '\0';
			x->name[0] = '\0';
			x->lives = -999;
//			x->killrating = 1600;
//			x->playerrating = 1600; 
			for(i=0; i<SK_NUM_SKILLS; i++) {  
					x->skill[i] = 0.0f;  
			}
			 x->muted = qfalse;  
			x->hits = 0;  
			x->team_hits = 0;  
			x->time = 0;  
			xpsave_open = qtrue;
		}
		t = COM_Parse(&cnf);
	}
	if(xpsave_open)
		g_xpsaves[xc++] = x;
	free(cnf2);
	G_Printf("xpsave: loaded %d xpsaves\n", xc);
}

qboolean G_xpsave_add(gentity_t *ent)
{
	int i = 0;
	int j = 0; 
	char userinfo[MAX_INFO_STRING];
	char *s;
	char guid[33];
	char name[MAX_NAME_LENGTH] = {""};
	int clientNum;
	qboolean found = qfalse;
	g_xpsave_t *x = g_xpsaves[0];
	time_t t;

	if(!(g_XPSave.integer & XPSF_ENABLE))  
		return qfalse;  
	if(!ent || !ent->client)
		return qfalse;
	if(!time(&t))
		return qfalse;
	if(ent->client->pers.connected != CON_CONNECTED)  
			return qfalse; 

	clientNum = ent - g_entities;
	trap_GetUserinfo(clientNum, userinfo, sizeof(userinfo));
	s = Info_ValueForKey(userinfo, "cl_guid");
	Q_strncpyz(guid, s, sizeof(guid));
	//SanitizeString(ent->client->pers.netname, name, qtrue);
	Q_strncpyz(name, ent->client->pers.netname, sizeof(name));
		
	if(!guid[0] || strlen(guid) != 32)
		return qfalse;

	for(i=0; g_xpsaves[i]; i++) {
		if(!Q_stricmp(g_xpsaves[i]->guid, guid)) {
			x = g_xpsaves[i];
			found = qtrue;
			break;
		}
	}
	if(!found) {
		if(i == MAX_XPSAVES) {
			G_Printf("xpsave: cannot save. MAX_XPSAVES exceeded");
			return qfalse;
		}
		x = malloc(sizeof(g_xpsave_t));
					x->guid[0] = '\0';
					x->name[0] = '\0';
					x->lives = -999;  
//					x->killrating = 1600;
//					x->playerrating = 1600;
	                for(j=0; j<SK_NUM_SKILLS; j++) {  
							x->skill[j] = 0.0f;  
	                }  
	                x->muted = qfalse;  
	                x->hits = 0;  
					x->team_hits = 0;  
					x->time = 0;
					g_xpsaves[i] = x;
	}
	Q_strncpyz(x->guid, guid, sizeof(x->guid));
	Q_strncpyz(x->name, name, sizeof(x->name)); 
	x->time = t;
	for(i=0; i<SK_NUM_SKILLS; i++) {
		x->skill[i] = ent->client->sess.skillpoints[i];
	}
	x->lives = -999;
       	if(ent->client->maxlivescalced)
		x->lives = ent->client->ps.persistant[PERS_RESPAWNS_LEFT] - 1;	
//	x->killrating = ent->client->sess.overall_killrating;
//	x->playerrating = ent->client->sess.playerrating;
	x->muted = ent->client->sess.muted;
	        // tjw: if this person has been muted by g_censor, do not  
			//      save the mute.  
		if(ent->client->sess.auto_mute_time != -1) {  
				x->muted = qfalse;  
		}  
		else {  
				x->muted = ent->client->sess.muted;  
		}  
	x->hits = ent->client->sess.hits;
	x->team_hits = ent->client->sess.team_hits;
	return qtrue;
}

qboolean G_xpsave_load(gentity_t *ent)
{
	int i;
	qboolean found = qfalse;
	char userinfo[MAX_INFO_STRING];
	char *guid;
	int clientNum;
	g_xpsave_t *x = g_xpsaves[0];
	time_t t;
	char agestr	[MAX_STRING_CHARS];
	char desc[64];
	int age;
	
	// tjw: even if xpsave is not enabled, we need to initialize  
	//      XPSave_lives
	if(!ent || !ent->client)
		return qfalse;
	
	if(!(g_XPSave.integer & XPSF_ENABLE))  
		return qfalse;    
	if(!time(&t))  
			return qfalse;  
	
	desc[0] = '\0'; 
	clientNum = ent - g_entities;
	trap_GetUserinfo(clientNum, userinfo, sizeof(userinfo));
	guid = Info_ValueForKey(userinfo, "cl_guid");
	
	for(i=0; g_xpsaves[i]; i++) {
		if(!Q_stricmp(g_xpsaves[i]->guid, guid)) {
			found = qtrue;
			x = g_xpsaves[i];
			break;
		}
	}
	if(!found)
		return qfalse;

        age = t - x->time;  
		if(age > g_XPSaveMaxAge.integer) {
			return qfalse;
		}  
		 
		if(age <= g_XPSaveMaxAge.integer) {  
			for(i=0; i<SK_NUM_SKILLS; i++) {  
					ent->client->sess.skillpoints[i] = x->skill[i];  
					ent->client->sess.startxptotal += x->skill[i];  
		}  
		ent->client->ps.stats[STAT_XP] =   
			(int)ent->client->sess.startxptotal;  
					Q_strcat(desc, sizeof(desc), "XP/");
					if((g_XPDecay.integer & XPDF_ENABLE) &&  
							!(g_XPDecay.integer & XPDF_NO_DISCONNECT_DECAY)) {  
							G_XPDecay(ent, age, qtrue);  
					}  

		}

	if(x->lives != -999 && x->lives < 0) {
		ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = x->lives;
	}
	ent->client->XPSave_lives = x->lives;  
/*	if(age <= g_XPSaveMaxAge.integer) {  
	ent->client->sess.overall_killrating = x->killrating;  
	ent->client->sess.playerrating = x->playerrating;  
			Q_strcat(desc, sizeof(desc), "killrating/playerrating/");  
	}  */
	if(age <= g_XPSaveMaxAge.integer && x->muted) {    
		ent->client->sess.muted = qtrue;  
		ent->client->sess.auto_mute_time = -1;  
		CP("print \"^5You've been muted by XPSave\n\"" );
	Q_strcat(desc, sizeof(desc), "mute/");  
	}  

	ent->client->sess.hits = x->hits;
	ent->client->sess.team_hits = x->team_hits;
	G_CalcRank(ent->client);
	BG_PlayerStateToEntityState(&ent->client->ps,
			&ent->s,
			level.time,
			g_fixedphysics.integer ? qfalse : qtrue);
	// tjw: trim last / from desc  
	if(strlen(desc))  
			desc[strlen(desc)-1] = '\0';  
	G_shrubbot_duration(age, agestr, sizeof(agestr));   
	CP(va(  
			"print \"^3server: loaded stored ^7%s^3 state from %s ago\n\"",  
			desc,  
			agestr));
	return qtrue;
}
/*
void G_xpsave_clear()
{
	int i;
	for(i=0; g_xpsaves[i]; i++) {
		free(g_xpsaves[i]);
		g_xpsaves[i] = NULL;
	}
	G_xpsave_writeconfig();
}
*/
void G_xpsave_resetxp()  
{  
	int i,j;  
		for(i=0; g_xpsaves[i]; i++) {  
				for(j=0; j<SK_NUM_SKILLS; j++) {  
							g_xpsaves[i]->skill[j] = 0.0f;  
				}  
						g_xpsaves[i]->hits = 0;  
						g_xpsaves[i]->team_hits = 0; 
						//g_xpsaves[i]->killrating = 1600;
						//g_xpsaves[i]->playerrating = 1600;
		}  
}

void G_xpsave_cleanup()  
{  
	int i = 0;

	for(i=0; g_xpsaves[i]; i++) {  
			free(g_xpsaves[i]);  
				g_xpsaves[i] = NULL;  
	}
}  


