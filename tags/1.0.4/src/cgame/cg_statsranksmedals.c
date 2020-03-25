#include "cg_local.h"

rankicon_t rankicons[NUM_EXPERIENCE_LEVELS][2][2] = {
	{ 
		{
			{ 0, "gfx/hud/ranks/rank1", 128, 128 },  
			{ 0, "models/players/temperate/common/rank1", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank1", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank1", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank2", 128, 128 },  
			{ 0, "models/players/temperate/common/rank2", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank2", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank2", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank3", 128, 128 },  
			{ 0, "models/players/temperate/common/rank3", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank3", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank3", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank4", 128, 128 },  
			{ 0, "models/players/temperate/common/rank4", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank4", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank4", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank5", 128, 128 },  
			{ 0, "models/players/temperate/common/rank5", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank5", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank5", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank6", 128, 128 },  
			{ 0, "models/players/temperate/common/rank6", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank6", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank6", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank7", 128, 128 },  
			{ 0, "models/players/temperate/common/rank7", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank7", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank7", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank8", 128, 128 },  
			{ 0, "models/players/temperate/common/rank8", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank8", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank8", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank9", 128, 128 },  
			{ 0, "models/players/temperate/common/rank9", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank9", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank9", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank10", 128, 128 },  
			{ 0, "models/players/temperate/common/rank10", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank10", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank10", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank11", 128, 128 },  
			{ 0, "models/players/temperate/common/rank11", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank11", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank11", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank12", 128, 128 },  
			{ 0, "models/players/temperate/common/rank12", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank12", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank12", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank13", 128, 128 },  
			{ 0, "models/players/temperate/common/rank13", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank13", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank13", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank14", 128, 128 },  
			{ 0, "models/players/temperate/common/rank14", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank14", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank14", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank15", 128, 128 },  
			{ 0, "models/players/temperate/common/rank15", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank15", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank15", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank16", 128, 128 },  
			{ 0, "models/players/temperate/common/rank16", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank16", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank16", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank17", 128, 128 },  
			{ 0, "models/players/temperate/common/rank17", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank17", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank17", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank18", 128, 128 },  
			{ 0, "models/players/temperate/common/rank18", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank18", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank18", 128, 128 } 
		},
	},
	{ 
		{
			{ 0, "gfx/hud/ranks/rank19", 128, 128 },  
			{ 0, "models/players/temperate/common/rank19", 128, 128 } 
		},
		{
			{ 0, "gfx/hud/ranks/xrank19", 128, 128 },  
			{ 0, "models/players/temperate/common/xrank19", 128, 128 } 
		},
	},
};

void CG_LoadRankIcons( void ) {
	int i;

	for( i = 1; i < NUM_EXPERIENCE_LEVELS; i++ ) {
		rankicons[i][0][0].shader = trap_R_RegisterShaderNoMip( rankicons[i][0][0].iconname );
		rankicons[i][1][0].shader = trap_R_RegisterShaderNoMip( rankicons[i][1][0].iconname );
		rankicons[i][0][1].shader = trap_R_RegisterShaderNoMip( rankicons[i][0][1].iconname );
		rankicons[i][1][1].shader = trap_R_RegisterShaderNoMip( rankicons[i][1][1].iconname );
	}
}
