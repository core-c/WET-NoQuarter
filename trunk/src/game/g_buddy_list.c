/*
 * name:		g_buddy_list.c
 *
 * desc:
 *
 */

#include "g_local.h"

void G_RemoveFromAllIgnoreLists( int clientNum ) {
	int i;

	for( i = 0; i < g_maxclients.integer; i++) {
		COM_BitClear( level.clients[i].sess.ignoreClients, clientNum );
	}
}
