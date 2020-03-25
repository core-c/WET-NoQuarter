#ifndef _G_XPSAVE_H
#define _G_XPSAVE_H

#define MAX_XPSAVES 32768

typedef struct {
	char guid[33];
	char name[MAX_NAME_LENGTH];
	int time;
	float skill[SK_NUM_SKILLS];
	int lives;
//	int killrating;
//	int playerrating;
	int muted;
	float hits;
	float team_hits;
} g_xpsave_t;

void G_xpsave_writeconfig();
void G_xpsave_readconfig();
qboolean G_xpsave_add(gentity_t *ent);
qboolean G_xpsave_load(gentity_t *ent);
void G_xpsave_resetxp();
void G_xpsave_cleanup(); 
#endif /* ifndef _G_XPSAVE_H */
