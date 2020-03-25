#ifndef _G_XPSAVE_H
#define _G_XPSAVE_H

// Lucel: XP save structure. We read/write this directly to disk.
//
// We don't care about padding issues as the structure will be padded
// consistently on the same machine - if we want to share these across different
// platforms then we might need to deal with padding.

#define XP_SAVE_SIGNATURE 		0x0ACED00D
#define XP_SAVE_CUR_VER			0
#define XP_SAVE_FILE_EXT		".xp"
#define MAX_GUID_LENGTH			32

typedef struct {
	int			signature;					// Check it's a valid save file
	int			version;					// Check file version. Allow for backwards compatibility later on.
	time_t		timestamp;					// Timestamp of xp save file
	char		netname[MAX_NETNAME];		// Store the player name
	float		skillpoints[SK_NUM_SKILLS];	// skillpoints
} g_xpsave_t;

void		G_xpsave_writestats();
qboolean	G_xpsave_add(gentity_t *ent);
qboolean	G_xpsave_load(gentity_t *ent);
void		G_xpsave_resetxp();
void		G_xpsave_init_game();

#endif /* ifndef _G_XPSAVE_H */
