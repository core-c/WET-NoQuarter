/*
 * name:	cg_debriefing.c
 *
 * desc:
 *
 * NQQS:
 *
 * Notes:
 * - cgs.DBmode is the flag for the shown panel
 * - button->data[0] is defined in the panel buttons (see definitions *ListScroll, 4th field, 1st entry)
 *   cgsDBmode != button->data[0] see TODOs
 *
 */
#include "cg_local.h"

team_t CG_Debriefing_FindWinningTeam( void );
team_t CG_Debriefing_FindOveralWinningTeam( void );
team_t CG_Debriefing_FindWinningTeamForPos( int pos );

int QDECL CG_SortPlayersByXP( const void *a, const void *b );

#define DB_MASTER_FONT &cgs.media.limboFont2

panel_button_text_t debriefTitleFont = {
	0.3f, 0.3f,
	{ 1.f, 1.f, 1.f, 0.8f },
	0, ITEM_ALIGN_CENTER,
	DB_MASTER_FONT,
};

panel_button_text_t debriefHeadingFont = {
	0.24f, 0.24f,
	{ 1.f, 1.f, 1.f, 0.8f },
	0, 0,
	DB_MASTER_FONT,
};

panel_button_text_t debriefListFont = {
	0.20f, 0.22f,
	{ 1.f, 1.f, 1.f, 0.8f },
	0, 0,
	DB_MASTER_FONT,
};

panel_button_text_t debriefPlayerHeadingSmallerFont = {
	0.2f, 0.2f,
	{ 0.6f, 0.6f, 0.6f, 1.f },
	0, 0,
	DB_MASTER_FONT,
};

#define DB_RANK_X	213 + 4
#define DB_NAME_X	DB_RANK_X	+ 28
#define DB_TIME_X	DB_NAME_X + 180
#define DB_KILLS_X	DB_TIME_X	+ 48
#define DB_DEATHS_X	DB_KILLS_X	+ 48
#define DB_XP_X		DB_DEATHS_X	+ 56
#define DH_HEADING_Y 60

panel_button_t debriefTitleBack = {
	"white",
	NULL,
	{ 10, 4, 620, 20 },
	{ 1, 41, 51, 43, 204, 0, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefTitleBackBorderLower = {
	NULL,
	NULL,
	{ 10, 24, 620, 200 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefTitleBackLower = {
	"white",
	NULL,
	{ 10, 24, 620, 200 },
	{ 1, 0, 0, 0, 153, 0, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefTitleBackBorder = {
	NULL,
	NULL,
	{ 10, 4, 620, 20 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefTitle = {
	NULL,
	NULL,
	{ 10, 20, 620, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefTitleFont,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	CG_DebriefingTitle_Draw,
	NULL,
};

panel_button_t debriefPlayerHeaderBackBorderLower = {
	NULL,
	NULL,
	{ 10, 260-14, 196, 110 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefPlayerHeaderBackLower = {
	"white",
	NULL,
	{ 10, 260-14, 196, 110 },
	{ 1, 0, 0, 0, 153, 0, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefPlayerHeaderBack = {
	"white",
	NULL,
	{ 10, 240-14, 196, 20 },
	{ 1, 41, 51, 43, 204, 0, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefPlayerHeaderBackBorder = {
	NULL,
	NULL,
	{ 10, 240-14, 196, 20 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefPlayerSkillsBackBorderLower = {
	NULL,
	NULL,
	{ 210, 260-14, 80, 110 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefPlayerSkillsBackLower = {
	"white",
	NULL,
	{ 210, 260-14, 80, 110 },
	{ 1, 0, 0, 0, 153, 0, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefPlayerSkillsBack = {
	"white",
	NULL,
	{ 210, 240-14, 80, 20 },
	{ 1, 41, 51, 43, 204, 0, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t debriefPlayerSkillsBackBorder = {
	NULL,
	NULL,
	{ 210, 240-14, 80, 20 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,		/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};


panel_button_t debriefPlayerWeaponStatsHeader = {
	NULL,
	"Weapon Stats",
	{ 18, 260, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerHeadingSmallerFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefPlayerWeaponStatsNameHeader = {
	NULL,
	"Name",
	{ 18, 274, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerHeadingSmallerFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefPlayerWeaponStatsShotsHeader = {
	NULL,
	"Shots",
	{ 78, 274, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerHeadingSmallerFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefPlayerWeaponStatsHitsHeader = {
	NULL,
	"Hits",
	{ 118, 274, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerHeadingSmallerFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefPlayerWeaponStatsKillsHeader = {
	NULL,
	"Kills",
	{ 148, 274, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerHeadingSmallerFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefPlayerWeaponStatsList = {
	NULL,
	NULL,
	{ 18, 274, 164 + 12, 80 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerHeadingSmallerFont,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	CG_DebriefingPlayerWeaponStats_Draw,
	NULL,
};

panel_button_t debriefPlayerWeaponStatsListScroll = {
	NULL,
	NULL,
	{ 18 + 164 + 12, 274, 12, 80 },
	{ 1, 0, 0, 0, 0, 0, 0, 0 },
	NULL,		/* font		*/
	CG_Debriefing_Scrollbar_KeyDown,	/* keyDown	*/
	CG_Debriefing_Scrollbar_KeyUp,		/* keyUp	*/
	CG_Debriefing_Scrollbar_Draw,
	NULL,
};


panel_button_t debriefTitleWindow = {
	NULL,
	NULL,
	{ 10, 4, 620, 22 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_Debreifing2_MissionTitle_Draw,
	NULL,
};

panel_button_t debriefMissionTitleWindow = {
	NULL,
	NULL,
	{ 10, 30, 193, 240 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_PanelButtonsRender_Window,
	NULL,
};

panel_button_t debriefMissionImage = {
	NULL,
	NULL,
	{ 16, 46, 181, 161 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_Debreifing2_Mission_Draw,
	NULL,
};

panel_button_t debriefMissionMaps = {
	NULL,
	NULL,
	{ 12, 210, 189, 60 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	CG_Debriefing2_Maps_KeyDown,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_Debreifing2_Maps_Draw,
	NULL,
};

panel_button_t debriefMissionAwardsWindow = {
	NULL,
	"ROLL OF HONOR",
	{ 213, 30, 417, 240 },
	{ ITEM_ALIGN_CENTER, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_PanelButtonsRender_Window,
	NULL,
};

panel_button_t debriefMissionAwardsList = {
	NULL,
	NULL,
	{ 215, 44, 413, 220 },
	{ ITEM_ALIGN_CENTER, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_Debreifing2_Awards_Draw,
	NULL,
};

panel_button_t debriefMissionAwardsListScroll = {
	NULL,
	NULL,
	{ 616, 45, 12, 223 },
	{ 3, 0, 0, 0, 0, 0, 0, 0 },
	NULL,		/* font		*/
	CG_Debriefing_Scrollbar_KeyDown,	/* keyDown	*/
	CG_Debriefing_Scrollbar_KeyUp,		/* keyUp	*/
	CG_Debriefing_Scrollbar_Draw,
	NULL,
};

panel_button_t debriefMissionStatsWindow = {
	NULL,
	"MISSION STATS",
	{ 10, 280-6, 620, 70+12 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_PanelButtonsRender_Window,
	NULL,
};

panel_button_t debriefMissionStatsHeaders = {
	NULL,
	NULL,
	{ 16, 298, 608, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_Debriefing2TeamSkillHeaders_Draw,
	NULL,
};

panel_button_t debriefMissionStatsWinner = {
	NULL,
	NULL,
	{ 16, 314, 608, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_Debriefing2TeamSkillXP_Draw,
	NULL,
};

panel_button_t debriefMissionStatsLoser = {
	NULL,
	NULL,
	{ 16, 330, 608, 16 },
	{ 1, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_Debriefing2TeamSkillXP_Draw,
	NULL,
};


panel_button_t debriefPlayerListWindow = {
	NULL,
	"PLAYERS",
	{ 213, 30, 417, 326 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_PanelButtonsRender_Window,
	NULL,
};

panel_button_text_t debriefPlayerListFont = {
	0.2f, 0.2f,
	{ 0.6f, 0.6f, 0.6f, 1.f },
	0, 0,
	DB_MASTER_FONT,
};

panel_button_t debriefHeadingRank = {
	NULL,
	"Rank",
	{ DB_RANK_X, DH_HEADING_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefHeadingName = {
	NULL,
	"Name",
	{ DB_NAME_X, DH_HEADING_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

#if 0
panel_button_t debriefHeadingMedals = {
	NULL,
	"Medals",
	{ DB_MEDALS_X, DH_HEADING_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};
#endif // 0

panel_button_t debriefHeadingTime = {
	NULL,
	"Time",
	{ DB_TIME_X, DH_HEADING_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefHeadingXP = {
	NULL,
	"XP",
	{ DB_XP_X, DH_HEADING_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	CG_DebriefingXPHeader_Draw,
	NULL,
};

panel_button_t debriefHeadingKills = {
	NULL,
	"Kills",
	{ DB_KILLS_X, DH_HEADING_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefHeadingDeaths = {
	NULL,
	"Deaths",
	{ DB_DEATHS_X, DH_HEADING_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t debriefPlayerList = {
	NULL,
	NULL,
	{ DB_RANK_X, DH_HEADING_Y, 640 - 8 - 16 - DB_RANK_X - 16, 292 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerListFont,				/* font		*/
	CG_DebriefingPlayerList_KeyDown,	/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_DebriefingPlayerList_Draw,
	NULL,
};

panel_button_t debriefPlayerListScroll = {
	NULL,
	NULL,
	{ 640 - 8 - 16, DH_HEADING_Y, 12, 292 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,		/* font		*/
	CG_Debriefing_Scrollbar_KeyDown,	/* keyDown	*/
	CG_Debriefing_Scrollbar_KeyUp,		/* keyUp	*/
	CG_Debriefing_Scrollbar_Draw,
	NULL,
};


panel_button_text_t debriefPlayerInfoFont = {
	0.2f, 0.2f,
	{ 0.6f, 0.6f, 0.6f, 1.f },
	0, 0,
	DB_MASTER_FONT,
};

panel_button_t debriefPlayerInfoWindow = {
	NULL,
	"PLAYER STATS",
	{ 10, 30, 198, 326 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,	/* font		*/
	NULL,	/* keyDown	*/
	NULL,	/* keyUp	*/
	CG_PanelButtonsRender_Window,
	NULL,
};

panel_button_t debriefPlayerInfoName = {
	NULL,
	NULL,
	{ 14, 56, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerInfoFont,	/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	CG_Debriefing_PlayerName_Draw,
	NULL,
};

panel_button_t debriefPlayerInfoRank = {
	NULL,
	NULL,
	{ 74, 70, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerInfoFont,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	CG_Debriefing_PlayerRank_Draw,
	NULL,
};

panel_button_t debriefPlayerInfoMedals = {
	NULL,
	NULL,
	{ 74, 84, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerInfoFont,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	CG_Debriefing_PlayerMedals_Draw,
	NULL,
};

panel_button_t debriefPlayerInfoTime = {
	NULL,
	NULL,
	{ 74, 98, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerInfoFont,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	CG_Debriefing_PlayerTime_Draw,
	NULL,
};

panel_button_t debriefPlayerInfoXP = {
	NULL,
	NULL,
	{ 74, 110, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerInfoFont,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	CG_Debriefing_PlayerXP_Draw,
	NULL,
};

panel_button_t debriefPlayerInfoACC = {
	NULL,
	NULL,
	{ 74, 122, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerInfoFont,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	CG_Debriefing_PlayerACC_Draw,
	NULL,
};

panel_button_t debriefPlayerInfoHitRegions = {
	NULL,
	NULL,
	{ 154, 98, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefPlayerInfoFont,	/* font		*/
	NULL,						/* keyDown	*/
	NULL,						/* keyUp	*/
	CG_Debriefing_PlayerHitRegions_Draw,
	NULL,
};

#define PLAYERHEADER_SKILLS( number )			\
	panel_button_t debriefPlayerInfoSkills##number = {		\
	NULL,										\
	NULL,										\
	{ 18 + (100 * (number%2)), 140 + 13 + (number/2 * 24), 12, 12 },		\
	{ number, 0, 0, 0, 0, 0, 0, 0 },			\
	&debriefPlayerInfoFont,	/* font		*/		\
	NULL,	/* keyDown	*/						\
	NULL,	/* keyUp	*/						\
	CG_Debriefing_PlayerSkills_Draw,			\
	NULL,										\
}

PLAYERHEADER_SKILLS( 0 );
PLAYERHEADER_SKILLS( 1 );
PLAYERHEADER_SKILLS( 2 );
PLAYERHEADER_SKILLS( 3 );
PLAYERHEADER_SKILLS( 4 );
PLAYERHEADER_SKILLS( 5 );
PLAYERHEADER_SKILLS( 6 );

panel_button_t* debriefPanelButtons[] = {
	&debriefTitleWindow,

	&debriefPlayerListWindow, &debriefPlayerList, &debriefPlayerListScroll,

	&debriefHeadingRank, &debriefHeadingName,
#if 0
	&debriefHeadingMedals,
#endif // 0
	&debriefHeadingTime, &debriefHeadingXP, &debriefHeadingKills, &debriefHeadingDeaths,

	&debriefPlayerInfoWindow, &debriefPlayerInfoName, &debriefPlayerInfoRank, &debriefPlayerInfoMedals, &debriefPlayerInfoTime, &debriefPlayerInfoXP, &debriefPlayerInfoACC, &debriefPlayerInfoHitRegions,

	&debriefPlayerInfoSkills0,
	&debriefPlayerInfoSkills1,
	&debriefPlayerInfoSkills2,
	&debriefPlayerInfoSkills3,
	&debriefPlayerInfoSkills4,
	&debriefPlayerInfoSkills5,
	&debriefPlayerInfoSkills6,

	&debriefPlayerWeaponStatsHeader, &debriefPlayerWeaponStatsNameHeader, &debriefPlayerWeaponStatsShotsHeader, &debriefPlayerWeaponStatsHitsHeader, &debriefPlayerWeaponStatsKillsHeader,
	&debriefPlayerWeaponStatsList, &debriefPlayerWeaponStatsListScroll,

	NULL
};


panel_button_t teamDebriefOutcome = {
	"white",
	NULL,
	{ 14, 26, 432, 224 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	CG_TeamDebriefingOutcome_Draw,
	NULL,
};


panel_button_t teamDebriefMapListBackBorderLower = {
	NULL,
	NULL,
	{ 460, 22, 170, 232 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefMapListBackLower = {
	"white",
	NULL,
	{ 460, 22, 170, 232 },
	{ 1, 0, 0, 0, 153, 0, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefMapListBackBorder = {
	NULL,
	NULL,
	{ 460, 4, 170, 20 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefMapListBack = {
	"white",
	NULL,
	{ 460, 4, 170, 20 },
	{ 1, 41, 51, 43, 204, 0, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefMapList = {
	NULL,
	NULL,
	{ 470, 196, 130, 50 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&debriefListFont,					/* font		*/
	CG_TeamDebriefingMapList_KeyDown,	/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_TeamDebriefingMapList_Draw,
	NULL,
};

panel_button_t teamDebriefMapListScroll = {
	NULL,
	NULL,
	{ 606, 198, 12, 50 },
	{ 2, 0, 0, 0, 0, 0, 0, 0 },
	NULL,					/* font		*/
	CG_Debriefing_Scrollbar_KeyDown,	/* keyDown	*/
	CG_Debriefing_Scrollbar_KeyUp,		/* keyUp	*/
	CG_Debriefing_Scrollbar_Draw,
	NULL,
};

panel_button_t teamDebriefMapShot = {
	NULL,
	NULL,
	{ 464, 28, 170-8, 170-8 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	CG_TeamDebriefingMapShot_Draw,
	NULL,
};

panel_button_text_t teamDebriefBigTitle = {
	0.32f, 0.32f,
	{ 1.f, 1.f, 1.f, 0.8f },
	0, 0,
	DB_MASTER_FONT,
};

panel_button_text_t teamDebriefTitleSmall = {
	0.24f, 0.24f,
	{ 1.f, 1.f, 1.f, 0.8f },
	0, ITEM_ALIGN_CENTER,
	DB_MASTER_FONT,
};

panel_button_text_t teamDebriefTitle = {
	0.28f, 0.28f,
	{ 1.f, 1.f, 1.f, 0.8f },
	0, 0,
	DB_MASTER_FONT,
};

panel_button_t teamDebriefMapWinnerText = {
	NULL,
	NULL,
	{ 10, 273, 620, 20 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&teamDebriefBigTitle,			/* font		*/
	NULL,								/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_TeamDebriefingMapWinner_Draw,
	NULL,
};


panel_button_t teamDebriefMapWinnerBackBorderLower = {
	NULL,
	NULL,
	{ 10, 277, 620, 77 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefMapWinnerBackLower = {
	"white",
	NULL,
	{ 10, 277, 620, 77 },
	{ 1, 0, 0, 0, 153, 0, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefMapWinnerTextBackBorder = {
	NULL,
	NULL,
	{ 10, 258, 620, 20 },
	{ 1, 127, 127, 127, 255, 1, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefMapWinnerTextBack = {
	"white",
	NULL,
	{ 10, 258, 620, 20 },
	{ 1, 41, 51, 43, 204, 0, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t teamDebriefAxisXPText = {
	NULL,
	"Axis",
	{ 24, 320, 470, 200 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&teamDebriefBigTitle,			/* font		*/
	NULL,							/* keyDown	*/
	NULL,							/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t teamDebriefAlliesXPText = {
	NULL,
	"Allies",
	{ 24, 340, 470, 200 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&teamDebriefBigTitle,			/* font		*/
	NULL,							/* keyDown	*/
	NULL,							/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

#define TDB_SKILL_TITLES_XP( number, title, x )				\
panel_button_t teamDebriefSkillXPText_##number = {			\
	NULL,													\
	title,													\
	{ 100 + (number * 65 ), 304 - (x * 12), 20, 200 },		\
	{ 0, 0, 0, 0, 0, 0, 0, 0 },								\
	&teamDebriefTitleSmall,		/* font		*/				\
	NULL,					/* keyDown	*/					\
	NULL,					/* keyUp	*/					\
	BG_PanelButtonsRender_Text,								\
	NULL,													\
}

TDB_SKILL_TITLES_XP( 0, "Battle Sense", 0 );
TDB_SKILL_TITLES_XP( 1, "Engineer",		1 );
TDB_SKILL_TITLES_XP( 2, "Medic",		0 );
TDB_SKILL_TITLES_XP( 3, "Field Ops",	1 );
TDB_SKILL_TITLES_XP( 4, "Light Weapons",0 );
TDB_SKILL_TITLES_XP( 5, "Soldier",		1 );
TDB_SKILL_TITLES_XP( 6, "Covert Ops",	0 );
TDB_SKILL_TITLES_XP( 7, "Total",		1 );

#define TDB_SKILL_AXIS_XP( number )							\
panel_button_t teamDebriefSkillXPText0_##number = {			\
	NULL,													\
	NULL,													\
	{ 110 + (number * 65 ), 320, 470, 200 },				\
	{ 0, number, 0, 0, 0, 0, 0, 0 },						\
	&teamDebriefTitle,		/* font		*/					\
	NULL,					/* keyDown	*/					\
	NULL,					/* keyUp	*/					\
	CG_TeamDebriefingTeamSkillXP_Draw,						\
	NULL,													\
}

#define TDB_SKILL_ALLIES_XP( number )						\
panel_button_t teamDebriefSkillXPText1_##number = {			\
	NULL,													\
	NULL,													\
	{ 110 + (number * 65 ), 340, 470, 200 },				\
	{ 1, number, 0, 0, 0, 0, 0, 0 },						\
	&teamDebriefTitle,		/* font		*/					\
	NULL,					/* keyDown	*/					\
	NULL,					/* keyUp	*/					\
	CG_TeamDebriefingTeamSkillXP_Draw,						\
	NULL,													\
}

TDB_SKILL_AXIS_XP( 0 );
TDB_SKILL_AXIS_XP( 1 );
TDB_SKILL_AXIS_XP( 2 );
TDB_SKILL_AXIS_XP( 3 );
TDB_SKILL_AXIS_XP( 4 );
TDB_SKILL_AXIS_XP( 5 );
TDB_SKILL_AXIS_XP( 6 );
TDB_SKILL_AXIS_XP( 7 );

TDB_SKILL_ALLIES_XP( 0 );
TDB_SKILL_ALLIES_XP( 1 );
TDB_SKILL_ALLIES_XP( 2 );
TDB_SKILL_ALLIES_XP( 3 );
TDB_SKILL_ALLIES_XP( 4 );
TDB_SKILL_ALLIES_XP( 5 );
TDB_SKILL_ALLIES_XP( 6 );
TDB_SKILL_ALLIES_XP( 7 );

panel_button_t* teamDebriefPanelButtons[] = {
	&debriefTitleWindow, &debriefMissionTitleWindow, &debriefMissionAwardsWindow, &debriefMissionImage, &debriefMissionMaps, &debriefMissionAwardsList, &debriefMissionAwardsListScroll,

	&debriefMissionStatsWindow, &debriefMissionStatsWinner, &debriefMissionStatsLoser, &debriefMissionStatsHeaders,

	NULL
};


panel_button_text_t chatPanelButtonFont = {
	0.20f, 0.20f,
	{ 1.f, 1.f, 1.f, 0.8f },
	0, ITEM_ALIGN_CENTER,
	DB_MASTER_FONT,
};

panel_button_text_t chatPanelButtonFontRed = {
	0.20f, 0.20f,
	{ 1.f, 0.f, 0.f, 0.8f },
	0, ITEM_ALIGN_CENTER,
	DB_MASTER_FONT,
};


panel_button_t chatPanelWindow = {
	NULL,
	"CHAT",
	{ 10, 480-120, 620, 110 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,			/* font		*/
	NULL,			/* keyDown	*/
	NULL,			/* keyUp	*/
	CG_PanelButtonsRender_Window,
	NULL,
};

panel_button_t chatPanelText = {
	NULL,
	NULL,
	{ 18, 480-34, 640 - 36, TEAMCHAT_HEIGHT },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,					/* font		*/
	NULL,					/* keyDown	*/
	NULL,					/* keyUp	*/
	CG_Debriefing_ChatBox_Draw,
	NULL,
};

panel_button_t chatPanelNextButton = {
	NULL,
	"MORE",
	{ 640-10-60-4, 480-30, 60, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,								/* font		*/
	CG_Debriefing_NextButton_KeyDown,	/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_Debriefing_NextButton_Draw,
	NULL,
};

panel_button_t chatPanelHTMLButton = {
	NULL,
	"^1REPORT",
	{ 640-10-60-4-60-4, 480-30, 60, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,		/* font	*/
	NULL,		/* keyDown	*/
	NULL,		/* keyUp	*/
	CG_Debriefing_HTMLButton_Draw,
	NULL,
};

panel_button_t chatPanelQCButton = {
	NULL,
	"QUICK CHAT",
	{ 640-10-60-4-60-4-80-4, 480-30, 80, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,								/* font		*/
	CG_Debriefing_QCButton_KeyDown,		/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_PanelButtonsRender_Button,
	NULL,
};

panel_button_t chatPanelReadyButton = {
	NULL,
	"READY",
	{ 640-10-60-4-60-4-80-4-60-4, 480-30, 60, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,		/* font		*/
	CG_Debriefing_ReadyButton_KeyDown,		/* keyDown	*/
	NULL,		/* keyUp	*/
	CG_Debriefing_ReadyButton_Draw,
	NULL,
};

panel_button_t chatTypeButton = {
	NULL,
	NULL,
	{ 10+4, 480-30, 80, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,		/* font		*/
	CG_Debriefing_ChatButton_KeyDown,		/* keyDown	*/
	NULL,		/* keyUp	*/
	CG_Debriefing_ChatButton_Draw,
	NULL,
};

panel_button_t charPanelEditSurround = {
	NULL,
	NULL,
	{ 10+4+80+4, 480-30, 252, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,				/* font		*/
	NULL,				/* keyDown	*/
	NULL,				/* keyUp	*/
	CG_PanelButtonsRender_Button,
	NULL,
};

panel_button_t charPanelEdit = {
	NULL,
	"chattext",
	{ 10+4+80+4+8, 480-34, 236, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&chatPanelButtonFont,				/* font		*/
	NULL, /*BG_PanelButton_EditClick,*/			/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_Debriefing_ChatEdit_Draw,
	CG_Debriefing_ChatEditFinish,
};

// MAPVOTE
#define DB_MAPNAME_X	15
#define DB_MAPVOTE_X	(DB_MAPNAME_X + 200)
#define DB_MAPVOTE_Y	(56 + 10)
#define DB_MAPVOTE_X2	(620 - 192 - 96 - 20)
#define DB_MAPVOTE_Y2	(326 + 30 - 192 - 16)

qboolean CG_MapVoteList_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		int pos	= ((cgs.cursorY - DB_MAPVOTE_Y - 2) / 12) + cgs.dbMapVoteListOffset;
		if( pos < 0 || pos >= cgs.dbNumMaps ) {
			return qfalse;
		}
		if( pos != cgs.dbSelectedMap ) {
			cgs.dbSelectedMap = pos;
		}
		return qtrue;
	}
	return qfalse;
}

qboolean CG_MapVote_VoteButton_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		if( !cg.snap ) {
			return qfalse;
		}

		if ( cgs.dbMapMultiVote ) {
			return qfalse;
		}

		if ( cgs.dbSelectedMap != -1 ) {
			trap_SendClientCommand( va("mapvote %d", cgs.dbMapID[cgs.dbSelectedMap]) );
			cgs.dbMapVotedFor[0] = cgs.dbSelectedMap;
			return qtrue;
		}
	}

	return qfalse;
}

void CG_MapVote_MultiVoteButton_Draw( panel_button_t* button )
{
	const char* str;
	vec4_t clrTxtBck = { 0.6f, 0.6f, 0.6f, 1.0f };

	if ( !cg.snap ) {
		return;
	}

	if ( !cgs.dbMapMultiVote ) {
		return;
	}

	if ( cgs.dbMapVotedFor[button->data[7]-1] != -1 ) {
		str = va("^3%d: ^3RE-VOTE", 4 - button->data[7]);
	} else {
		str = va("^3%d: ^7VOTE", 4 - button->data[7]);
	}

	if ( cgs.dbMapVotedFor[button->data[7]-1] != -1 ) {
		CG_Text_Paint_Ext( button->rect.x + button->rect.w + 10,
			button->rect.y + (3*(button->rect.h/4)),
			.20f, .20f, clrTxtBck,
			cgs.dbMapDispName[cgs.dbMapVotedFor[button->data[7]-1]],
			0, 0, 0, DB_MASTER_FONT );
	}

	CG_PanelButtonsRender_Button_Ext( &button->rect, str );
	return;
}

void CG_MapVoteList_Draw( panel_button_t* button )
{
	int i;
	float y = button->rect.y + 12;
	float y2 = DB_MAPVOTE_Y;
	qhandle_t pic;

	CG_Text_Paint_Ext( 290, y2, button->font->scalex,
				button->font->scaley, button->font->colour,
				( cgs.mapVoteMapY <= 0 ? "" :
				va("Map %d of %d", cgs.mapVoteMapX + 1, cgs.mapVoteMapY)),
				0, 0, 0, button->font->font );
	y2 += 15;

	for( i = 0; i + cgs.dbMapVoteListOffset < cgs.dbNumMaps && i < 17; i++ ) {
		if ( strlen( cgs.dbMaps[i + cgs.dbMapVoteListOffset] ) < 1 ) {
			break;
		}
		if( cgs.dbSelectedMap == i + cgs.dbMapVoteListOffset ) {
			vec4_t clr = { 1.f, 1.f, 1.f, 0.3f };
			CG_FillRect( button->rect.x, y - 10, 250, 12, clr );
			pic = trap_R_RegisterShaderNoMip( va( "levelshots/%s.tga", cgs.dbMaps[i + cgs.dbMapVoteListOffset]));
			if ( pic ) {
				CG_DrawPic( DB_MAPVOTE_X2, DB_MAPVOTE_Y2, 96, 177.0f/233.0f * 96, pic);
			}
			pic = trap_R_RegisterShaderNoMip( va( "levelshots/%s_cc.tga", cgs.dbMaps[i + cgs.dbMapVoteListOffset]));
			if ( pic ) {
				CG_DrawPic( 620-192-10, DB_MAPVOTE_Y2, 192, 192, pic);
			}
			CG_Text_Paint_Ext( DB_MAPVOTE_X2, y2, button->font->scalex,
					button->font->scaley, button->font->colour,
					va("Last Played             : %s",
						(cgs.dbMapLastPlayed[i+cgs.dbMapVoteListOffset] == -1 ? "Never" : va("%d maps ago",
						cgs.dbMapLastPlayed[i+cgs.dbMapVoteListOffset]))),
					0, 0, 0, button->font->font );
			y2 += 15;
			CG_Text_Paint_Ext( DB_MAPVOTE_X2, y2, button->font->scalex,
					button->font->scaley, button->font->colour,
					va("Total Accumulated Votes : %d", cgs.dbMapTotalVotes[i+cgs.dbMapVoteListOffset]),
					0, 0, 0, button->font->font );
		}
		CG_Text_Paint_Ext( DB_MAPNAME_X + 12, y, button->font->scalex,
				button->font->scaley, button->font->colour,
				cgs.dbMapDispName[i + cgs.dbMapVoteListOffset],
				0, 30, 0, button->font->font );
		CG_Text_Paint_Ext( DB_MAPVOTE_X + 10 + 10, y, button->font->scalex,
				button->font->scaley, button->font->colour,
				va("%d", cgs.dbMapVotes[i + cgs.dbMapVoteListOffset]),
				0, 0, 0, button->font->font );
		y += 12;
	}
	return;
}

void CG_MapVote_VoteButton_Draw( panel_button_t* button )
{
	const char* str;
	vec4_t clrTxtBck = { 0.6f, 0.6f, 0.6f, 1.0f };

	if ( !cg.snap ) {
		return;
	}

	if ( cgs.dbMapMultiVote ) {
		return;
	}

	if ( cg.snap->ps.eFlags & EF_VOTED ) {
		str = "^1RE-VOTE";
	} else {
		str = "^3VOTE";
	}

	if ( cg.snap->ps.eFlags & EF_VOTED ) {
		CG_Text_Paint_Ext( button->rect.x + button->rect.w + 10,
			button->rect.y + (3*(button->rect.h/4)),
			.20f, .20f, clrTxtBck,
			cgs.dbMapDispName[cgs.dbMapVotedFor[0]],
			0, 0, 0, DB_MASTER_FONT );
	}
	CG_PanelButtonsRender_Button_Ext( &button->rect, str );
	return;
}

qboolean CG_MapVote_MultiVoteButton_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		if( !cg.snap ) {
			return qfalse;
		}

		if ( !cgs.dbMapMultiVote ) {
			return qfalse;
		}

		if ( cgs.dbSelectedMap != -1 ) {
			int i;
			int arrIdx = button->data[7] - 1;

			for ( i = 0; i < 3; i++ ) {
				if ( arrIdx == i )
					continue;
				if ( cgs.dbMapVotedFor[i] == cgs.dbSelectedMap ) {
					CG_Printf("^3Can't vote for the same map twice\n");
					return qfalse;
				}
			}

			trap_SendClientCommand(va("mapvote %d %d",
					cgs.dbMapID[cgs.dbSelectedMap],
					arrIdx + 1));
			cgs.dbMapVotedFor[arrIdx] = cgs.dbSelectedMap;
			return qtrue;
		}
	}

	return qfalse;
}

panel_button_t mapVoteWindow = {
	NULL,
	"MAP VOTE",
	{ 10, 30, 620, 326 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,								/* font		*/
	NULL,								/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_PanelButtonsRender_Window,
	NULL,
};

panel_button_text_t mapVoteFont = {
	0.2f, 0.2f,
	{ 0.6f, 0.6f, 0.6f, 1.f },
	0, 0,
	DB_MASTER_FONT,
};

panel_button_t mapVoteHeadingName = {
	NULL,
	"Name",
	{ DB_MAPNAME_X + 10, DB_MAPVOTE_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&mapVoteFont,						/* font		*/
	NULL,								/* keyDown	*/
	NULL,								/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t mapVoteHeadingVotes = {
	NULL,
	"Votes",
	{ DB_MAPVOTE_X + 10, DB_MAPVOTE_Y, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&mapVoteFont,						/* font		*/
	NULL,								/* keyDown	*/
	NULL,								/* keyUp	*/
	BG_PanelButtonsRender_Text,
	NULL,
};

panel_button_t mapVoteNamesList = {
	NULL,
	NULL,
	{ DB_MAPNAME_X + 10, DB_MAPVOTE_Y, 250, 17*12 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	&mapVoteFont,						/* font		*/
	CG_MapVoteList_KeyDown,				/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_MapVoteList_Draw,
	NULL,
};

panel_button_t mapVoteNamesListScroll = {
	NULL,
	NULL,
	{ DB_MAPVOTE_X + 10 + 52, DB_MAPVOTE_Y + 2, 12, 17*12 },
	{ 4, 0, 0, 0, 0, 0, 0, 0 },
	NULL,								/* font		*/
	CG_Debriefing_Scrollbar_KeyDown,	/* keyDown	*/
	CG_Debriefing_Scrollbar_KeyUp,		/* keyUp	*/
	CG_Debriefing_Scrollbar_Draw,
	NULL,
};

panel_button_t mapVoteButton = {
	NULL,
	"^3VOTE",
	{ DB_MAPNAME_X + 10, 296 - 10 + 2, 64, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	NULL,								/* font		*/
	CG_MapVote_VoteButton_KeyDown,		/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_MapVote_VoteButton_Draw,
	NULL,
};

panel_button_t mapVoteButton1 = {
	NULL,
	"^3VOTE #1",
	{ DB_MAPNAME_X + 10, 296 - 10 + 2, 64, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 3 },
	NULL,								/* font		*/
	CG_MapVote_MultiVoteButton_KeyDown,	/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_MapVote_MultiVoteButton_Draw,
	NULL,
};

panel_button_t mapVoteButton2 = {
	NULL,
	"^3VOTE #2",
	{ DB_MAPNAME_X + 10, 296 + 10 + 2, 64, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 2 },
	NULL,								/* font		*/
	CG_MapVote_MultiVoteButton_KeyDown,	/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_MapVote_MultiVoteButton_Draw,
	NULL,
};

panel_button_t mapVoteButton3 = {
	NULL,
	"^3VOTE #3",
	{ DB_MAPNAME_X + 10, 296 + 30 + 2, 64, 16 },
	{ 0, 0, 0, 0, 0, 0, 0, 1 },
	NULL,								/* font		*/
	CG_MapVote_MultiVoteButton_KeyDown,	/* keyDown	*/
	NULL,								/* keyUp	*/
	CG_MapVote_MultiVoteButton_Draw,
	NULL,
};

panel_button_t mapVoteBorder1 = {
	NULL,
	NULL,
	{ DB_MAPVOTE_X2 - 10, DB_MAPVOTE_Y - 12, 620 - DB_MAPVOTE_X2 + 10, DB_MAPVOTE_Y2 - DB_MAPVOTE_Y - 4 },
	{ 1, 255, 255, 255, 40, 1, 0, 0 },
	NULL,								/* font		*/
	NULL,								/* keyDown	*/
	NULL,								/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t mapVoteBorder2 = {
	NULL,
	NULL,
	{ DB_MAPVOTE_X2 - 10, DB_MAPVOTE_Y2 - 10 + 2, 620 - DB_MAPVOTE_X2 + 10, 370 - DB_MAPVOTE_Y2 - 2 - 10 },
	{ 1, 255, 255, 255, 40, 1, 0, 0 },
	NULL,								/* font		*/
	NULL,								/* keyDown	*/
	NULL,								/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};

panel_button_t mapVoteBorder3 = {
	NULL,
	NULL,
	{ 20, DB_MAPVOTE_Y - 12, DB_MAPVOTE_X2 - 40 + 2, 370 - DB_MAPVOTE_Y - 10 + 2 },
	{ 1, 255, 255, 255, 40, 1, 0, 0 },
	NULL,								/* font		*/
	NULL,								/* keyDown	*/
	NULL,								/* keyUp	*/
	BG_PanelButtonsRender_Img,
	NULL,
};
// MAPVOTE END

void CG_Debriefing_ChatEdit_Draw( panel_button_t* button ) {
//	qboolean useCvar = button->data[0] ? qfalse : qtrue;
	int offset = -1;
	char buffer[256 + 1];
	const char* cs;

	trap_Cvar_VariableStringBuffer( button->text, buffer, sizeof(buffer) );

	if( (cg.time / 1000) % 2 ) {
		if( trap_Key_GetOverstrikeMode() ) {
			Q_strcat( buffer, sizeof(buffer), "^7|" );
		}
		else {
			Q_strcat( buffer, sizeof(buffer), "^7_" );
		}
	}
	else {
		Q_strcat( buffer, sizeof(buffer), " " );
	}

	do {
		offset++;
		if( buffer + offset  == '\0' ) {
			break;
		}
	} while( CG_Text_Width_Ext( buffer + offset, button->font->scalex, 0, button->font->font ) > button->rect.w );

	switch( cgs.dbChatMode ) {
		case 0:
			cs = va( "^7%s", buffer + offset );
			break;
		case 1:
			cs = va( "^5%s", buffer + offset );
			break;
		case 2:
			cs = va( "^3%s", buffer + offset );
			break;
		default:
			cs = "";
			break;
	}


	CG_Text_Paint_Ext( button->rect.x, button->rect.y + button->rect.h, button->font->scalex, button->font->scaley, button->font->colour, cs, 0, 0, button->font->style, button->font->font );
}

void CG_Debriefing_ChatBox_Draw( panel_button_t* button ) {
	int w, h;
	vec4_t		hcolor;


	int chatWidth = button->rect.w;
	int chatHeight = button->rect.h;

	if( cgs.teamLastChatPos != cgs.teamChatPos ) {
		int i, len;
		float	lineHeight = 9.f;

		h = (cgs.teamChatPos - cgs.teamLastChatPos) * TINYCHAR_HEIGHT;

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
			if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_AXIS ) {
				hcolor[0] = 1;
				hcolor[1] = 0;
				hcolor[2] = 0;
			}
			else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_ALLIES ) {
				hcolor[0] = 0;
				hcolor[1] = 0;
				hcolor[2] = 1;
			}
			else {
				hcolor[0] = 0;
				hcolor[1] = 1;
				hcolor[2] = 0;
			}

			hcolor[3] = 0.33f;

			trap_R_SetColor( hcolor );
			CG_DrawPic( button->rect.x, button->rect.y - (cgs.teamChatPos - i)*lineHeight, chatWidth, lineHeight, cgs.media.teamStatusBar );

			trap_R_SetColor( NULL );

			if( cgs.teamChatMsgTeams[i % chatHeight] == TEAM_AXIS ) {
				CG_DrawPic( button->rect.x, button->rect.y - (cgs.teamChatPos - i - 1)*lineHeight - 8, 12, 10, cgs.media.axisFlag );
			}
			else if( cgs.teamChatMsgTeams[i % chatHeight] == TEAM_ALLIES ) {
				CG_DrawPic( button->rect.x, button->rect.y - (cgs.teamChatPos - i - 1)*lineHeight - 8, 12, 10, cgs.media.alliedFlag );
			}

			CG_Text_Paint_Ext( button->rect.x + 12, button->rect.y - (cgs.teamChatPos - i - 1)*lineHeight, 0.2f, 0.2f, colorWhite,  cgs.teamChatMsgs[i % chatHeight], 0, 0, 0, &cgs.media.limboFont2 );
		}
	}
}

panel_button_t* chatPanelButtons[] = {
	&chatPanelWindow, &chatPanelText,

	&chatPanelNextButton, &chatPanelQCButton, &chatTypeButton, &chatPanelReadyButton,

	&charPanelEditSurround, &charPanelEdit,

	NULL
};

// MAPVOTE
panel_button_t* mapVoteButtons[] = {
	&debriefTitleWindow, &mapVoteWindow, &mapVoteHeadingName, &mapVoteHeadingVotes,
	&mapVoteBorder1, &mapVoteBorder2, &mapVoteBorder3,
	&mapVoteNamesListScroll, &mapVoteNamesList, &mapVoteButton,
	&mapVoteButton1, &mapVoteButton2, &mapVoteButton3,
	NULL
};

void CG_ChatPanel_Setup( void )
{
	BG_PanelButtonsSetup( chatPanelButtons );
	BG_PanelButtonsSetup( teamDebriefPanelButtons );
	BG_PanelButtonsSetup( debriefPanelButtons );
	// MAPVOTE TODO: only active for gametype 6 ?
	BG_PanelButtonsSetup( mapVoteButtons );
}

void CG_Debriefing_Startup( void )
{
	const char *s, *buf;

	cgs.dbShowing = qtrue;
	cgs.dbAccuraciesRecieved = qfalse;
	cgs.dbWeaponStatsRecieved = qfalse;
	cgs.dbPlayerKillsDeathsRecieved = qfalse;

	cgs.dbLastRequestTime = 0;
	cgs.dbSelectedClient = cg.clientNum;
	// MAPVOTE
	cgs.dbSelectedMap = -1;
	cgs.dbMapListReceived = qfalse;
	cgs.dbMapVotedFor[0] = -1;
	cgs.dbMapVotedFor[1] = -1;
	cgs.dbMapVotedFor[2] = -1;
	// MAPVOTE END

	cgs.dbAwardsParsed = qfalse;

	s = CG_ConfigString( CS_MULTI_MAPWINNER );
	buf = Info_ValueForKey( s, "winner" );

	trap_Cvar_Set( "chattext", "" );

	if ( atoi( buf ) == -1 ) {
	}
	else if ( atoi( buf ) ) {
		trap_S_StartLocalSound( trap_S_RegisterSound( "sound/music/allies_win.wav", qtrue ), CHAN_LOCAL_SOUND );
	}
	else {
		trap_S_StartLocalSound( trap_S_RegisterSound( "sound/music/axis_win.wav", qtrue ), CHAN_LOCAL_SOUND );
	}

	cgs.dbMode = 0;

	if ( cgs.gametype != GT_WOLF_MAPVOTE ) {
		cgs.dbMode = 1;
	}
}

void CG_Debriefing_Shutdown( void )
{
	cgs.dbShowing = qfalse;
}

void CG_Debriefing_InfoRequests( void )
{
	if( cgs.dbLastRequestTime && (cg.time - cgs.dbLastRequestTime) < 1000 ) {
		return;
	}
	cgs.dbLastRequestTime = cg.time;

	// MAPVOTE
	if ( !cgs.dbMapListReceived && cgs.gametype == GT_WOLF_MAPVOTE ) {
		trap_SendClientCommand("immaplist");
		return;
	}

	if( !cgs.dbPlayerKillsDeathsRecieved ) {
		trap_SendClientCommand( "impkd" );
		return;
	}

	if( !cgs.dbAccuraciesRecieved ) {
		trap_SendClientCommand( "imwa" );
		return;
	}

	if( !cgs.dbWeaponStatsRecieved ) {
		trap_SendClientCommand( va( "imws %i", cgs.dbSelectedClient ) );
		return;
	}

	// if nothing else is pending, ask for scores
	// core: ..but do not poll during intermission (scores will not change anyway)..
	if( !cgs.dbLastScoreRequest || (!cg.intermissionStarted && (cg.time - cgs.dbLastScoreRequest) > 1000) ) {
		cgs.dbLastScoreRequest = cg.time;
		trap_SendClientCommand( "score" );
	}
}

qboolean CG_Debriefing_Draw( void )
{
	int i;

	if( !cgs.dbShowing ) {
		CG_Debriefing_Startup();
	}

	CG_Debriefing_InfoRequests();

//	CG_FillRect( 0, 0, 640, 480, colorBlack );

	if( trap_Key_GetCatcher() & KEYCATCH_UI ) {
		return qtrue;
	}

	if( !trap_Key_GetCatcher() ) {
		trap_Key_SetCatcher( KEYCATCH_CGAME );
	}

	switch( cgs.dbMode ) {
		case 0: // vote
			BG_PanelButtonsRender( mapVoteButtons );
			BG_PanelButtonsRender( chatPanelButtons );
			CG_DrawPic( cgDC.cursorx, cgDC.cursory, 32, 32, cgs.media.cursorIcon );
			break;
		case 1: // player list
			CG_DrawScoreboard();

			BG_PanelButtonsRender( chatPanelButtons );

			CG_DrawPic( cgDC.cursorx, cgDC.cursory, 32, 32, cgs.media.cursorIcon );
			break;
		case 2:	// awards
			BG_PanelButtonsRender( teamDebriefPanelButtons );
			BG_PanelButtonsRender( chatPanelButtons );

			CG_DrawPic( cgDC.cursorx, cgDC.cursory, 32, 32, cgs.media.cursorIcon );

			break;
		case 3:	// campaign
			for( i = 0 ; i < cgs.maxclients; i++ ) {
				cgs.dbSortedClients[i] = i;
			}

			qsort( cgs.dbSortedClients, cgs.maxclients, sizeof(int), CG_SortPlayersByXP );

			BG_PanelButtonsRender( debriefPanelButtons );

			BG_PanelButtonsRender( chatPanelButtons );

			CG_DrawPic( cgDC.cursorx, cgDC.cursory, 32, 32, cgs.media.cursorIcon );
			break;
	}

	return qtrue;
}

qboolean CG_DebriefingPlayerList_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		int pos	= (( cgs.cursorY - DH_HEADING_Y ) / 12) + cgs.dbPlayerListOffset;
		if( pos < 0 || pos >= cgs.maxclients ) {
			return qfalse;
		}

		pos = cgs.dbSortedClients[pos];

		if( !cgs.clientinfo[pos].infoValid ) {
			return qfalse;
		}


		CG_Debrieing_SetSelectedClient( pos );

		return qtrue;
	}

	return qfalse;
}

int CG_Debriefing_GetNextWeaponStat( int pos )
{
	int i;

	for( i = pos+1; i < WS_MAX; i++ ) {
		if( cgs.dbWeaponStats[i].numShots ) {
			return i;
		}
	}

	return -1;
}

void CG_DebriefingPlayerWeaponStats_Draw( panel_button_t* button )
{
	int i;
	float y = button->rect.y + 12;
	int pos = 0;

	if( !cgs.dbWeaponStatsRecieved ) {
		return;
	}

	pos = CG_Debriefing_GetNextWeaponStat( -1 );
	for( i = cgs.dbWeaponListOffset; i > 0 && pos != -1; i-- ) {
		pos = CG_Debriefing_GetNextWeaponStat( pos );
	}

	for( i = 0; i < 6 && pos != -1; i++, pos = CG_Debriefing_GetNextWeaponStat( pos ) ) {
		CG_Text_Paint_Ext( 18, y, button->font->scalex, button->font->scaley, button->font->colour, WeaponStatsName(pos), 0, 0, 0, button->font->font );

		CG_Text_Paint_Ext( 78, y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", cgs.dbWeaponStats[pos].numShots ), 0, 0, 0, button->font->font );

		CG_Text_Paint_Ext( 118, y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", cgs.dbWeaponStats[pos].numHits ), 0, 0, 0, button->font->font );

		CG_Text_Paint_Ext( 148, y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", cgs.dbWeaponStats[pos].numKills ), 0, 0, 0, button->font->font );

		y += 12;
	}
}

const char* CG_Debriefing_TimeToString( float msec )
{
	int mins, seconds, tens;

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	return va( "%i:%i%i", mins, tens, seconds );
}

void CG_DebriefingTitle_Draw( panel_button_t* button )
{
	const char *s, *buf;
	float x, w;

	if( cg_gameType.integer == GT_WOLF_STOPWATCH ) {
		int defender, winner;

		s = CG_ConfigString( CS_MULTI_INFO );
		defender = atoi( Info_ValueForKey( s, "d" ) );	//defender

		s = CG_ConfigString( CS_MULTI_MAPWINNER );
		winner = atoi( Info_ValueForKey( s, "winner" ) );

		if( cgs.currentRound ) {
			// first round
			s = va( CG_TranslateString( "Clock is now set to %s!" ), CG_Debriefing_TimeToString( cgs.nextTimeLimit * 60.f * 1000.f ) );
		}
		else {
			// second round
			if( !defender ) {
				if( winner != defender ) {
					s = "ALLIES SUCCESSFULLY BEAT THE CLOCK!";
				}
				else {
					s = "ALLIES COULDN'T BEAT THE CLOCK!";
				}
			}
			else {
				if ( winner != defender ) {
					s = "AXIS SUCCESSFULLY BEAT THE CLOCK!";
				}
				else {
					s = "AXIS COULDN'T BEAT THE CLOCK!";
				}
			}
		}
	}
	else {

		s = CG_ConfigString( CS_MULTI_MAPWINNER );
		buf = Info_ValueForKey( s, "winner" );

		if ( atoi( buf ) == -1 ) {
			// neutral
			s = "It's a TIE!";
		}
		else if ( atoi( buf ) ) {
			// allies
			s = "Allies Win!";
		}
		else {
			// axis
			s = "Axis Win!";
		}
	}

	w = CG_Text_Width_Ext( s, button->font->scalex, 0, button->font->font );
	x = button->rect.x + 4; // + ((button->rect.w - w) * 0.5f);

	CG_Text_Paint_Ext( x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, s, 0, 0, 0, button->font->font );

	s = va( "%i seconds to next map", max( 60 - (cg.time - cgs.intermissionStartTime) / 1000, 0 ) );
	w = CG_Text_Width_Ext( s, button->font->scalex, 0, button->font->font );
	x = button->rect.x + button->rect.w - w - 4;

	CG_Text_Paint_Ext( x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, s, 0, 0, 0, button->font->font );
}

void CG_DebriefingXPHeader_Draw( panel_button_t* button )
{
	if( cgs.gametype == GT_WOLF_LMS ) {
		BG_PanelButtonsRender_TextExt( button, "Score" );
	}
	else {
		BG_PanelButtonsRender_TextExt( button, "XP" );
	}
}

void CG_DebriefingPlayerList_Draw( panel_button_t* button )
{
	int i, j;
	float y = button->rect.y + 12;
	score_t* score = NULL;
	clientInfo_t *ci;

	for( i = 0; i + cgs.dbPlayerListOffset < cgs.maxclients && i < 24; i++ ) {
		ci = &cgs.clientinfo[cgs.dbSortedClients[i + cgs.dbPlayerListOffset]];
		if( !ci->infoValid ) {
			break;
		}

		for( j = 0; j < cgs.maxclients; j++ ) {
			if( cg.scores[j].client == cgs.dbSortedClients[i + cgs.dbPlayerListOffset] ) {
				score = &cg.scores[j];
				break;
			}
		}
		if( j == cgs.maxclients ) {
			continue;
		}

		if( cgs.dbSelectedClient == cgs.dbSortedClients[i + cgs.dbPlayerListOffset] ) {
			vec4_t clr = { 1.f, 1.f, 1.f, 0.3f };
			CG_FillRect( button->rect.x, y - 10, 640 - 10 - 8 - 2 - button->rect.x, 12, clr );
		}

		CG_Text_Paint_Ext( DB_RANK_X, y, button->font->scalex, button->font->scaley, button->font->colour, CG_Debriefing_RankNameForClientInfo( ci ), 0, 0, 0, button->font->font );

		CG_Text_Paint_Ext( DB_NAME_X, y, button->font->scalex, button->font->scaley, button->font->colour, ci->name, 0, 28, 0, button->font->font );

		CG_Text_Paint_Ext( DB_TIME_X, y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", score->time ), 0, 0, 0, button->font->font );

		CG_Text_Paint_Ext( DB_XP_X, y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", ci->score ), 0, 0, 0, button->font->font );

		if( cgs.dbPlayerKillsDeathsRecieved ) {
			CG_Text_Paint_Ext( DB_KILLS_X, y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", ci->kills ), 0, 0, 0, button->font->font );
			CG_Text_Paint_Ext( DB_DEATHS_X, y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", ci->deaths ), 0, 0, 0, button->font->font );
		}
		else {
			CG_Text_Paint_Ext( DB_KILLS_X, y, button->font->scalex, button->font->scaley, button->font->colour, "-", 0, 0, 0, button->font->font );
			CG_Text_Paint_Ext( DB_DEATHS_X, y, button->font->scalex, button->font->scaley, button->font->colour, "-", 0, 0, 0, button->font->font );
		}

		y += 12;
	}
}


int QDECL CG_SortPlayersByXP( const void *a, const void *b )
{
	int ca = *(int*)a;
	int cb = *(int*)b;

	if( !cgs.clientinfo[cb].infoValid ) {
		return -1;
	}
	if( !cgs.clientinfo[ca].infoValid ) {
		return 1;
	}

	if( cgs.clientinfo[cb].score > cgs.clientinfo[ca].score ) {
		return 1;
	}
	if( cgs.clientinfo[ca].score > cgs.clientinfo[cb].score ) {
		return -1;
	}

	return 0;
}

const char* CG_Debriefing_FullRankNameForClientInfo( clientInfo_t* ci )
{
	if( ci->team != TEAM_AXIS && ci->team != TEAM_ALLIES ) {
		return "Spectator";
	}

	return ci->team == TEAM_AXIS ? rankNames_Axis[ci->rank] : rankNames_Allies[ci->rank];
}

const char* CG_Debriefing_RankNameForClientInfo( clientInfo_t* ci )
{
	if( ci->team != TEAM_AXIS && ci->team != TEAM_ALLIES ) {
		return "Spc";
	}

	return ci->team == TEAM_AXIS ? miniRankNames_Axis[ci->rank] : miniRankNames_Allies[ci->rank];
}

void CG_Debriefing_ParseWeaponAccuracies( void )
{
	int i;
	for( i = 0; i < cgs.maxclients; i++ ) {
		cgs.clientinfo[i].totalWeapAcc = atoi( CG_Argv( i + 1 ) );
	}
	cgs.dbAccuraciesRecieved = qtrue;
}

void CG_Debriefing_ParsePlayerKillsDeaths( void )
{
	int i;
	for( i = 0; i < cgs.maxclients; i++ ) {
		cgs.clientinfo[i].kills =	atoi( CG_Argv( i*2 + 1 ) );
		cgs.clientinfo[i].deaths =	atoi( CG_Argv( i*2 + 2 ) );
	}
	cgs.dbPlayerKillsDeathsRecieved = qtrue;
}

void CG_Debriefing_ParseWeaponStats( void )
{
	int i;

	cgs.dbHitRegions[HR_HEAD] = atoi( CG_Argv(1) );
	cgs.dbHitRegions[HR_ARMS] = atoi( CG_Argv(2) );
	cgs.dbHitRegions[HR_BODY] = atoi( CG_Argv(3) );
	cgs.dbHitRegions[HR_LEGS] = atoi( CG_Argv(4) );

	for( i = 0; i < WS_MAX; i++ ) {
		cgs.dbWeaponStats[i].numShots = atoi( CG_Argv( ( i * 3 ) + 5 ) );
		cgs.dbWeaponStats[i].numHits =	atoi( CG_Argv( ( i * 3 ) + 6 ) );
		cgs.dbWeaponStats[i].numKills =	atoi( CG_Argv( ( i * 3 ) + 7 ) );
	}

	cgs.dbWeaponStatsRecieved = qtrue;
}

qboolean CG_Debriefing_ServerCommand( const char* cmd )
{
	if( !Q_stricmp( cmd, "imwa" ) ) {
		CG_Debriefing_ParseWeaponAccuracies();
		return qtrue;
	}

	if( !Q_stricmp( cmd, "imws" ) ) {
		CG_Debriefing_ParseWeaponStats();
		return qtrue;
	}

	if( !Q_stricmp( cmd, "impkd" ) ) {
		CG_Debriefing_ParsePlayerKillsDeaths();
		return qtrue;
	}

	// MAPVOTE
	if(!Q_stricmp(cmd, "immaplist")) {
		CG_parseMapVoteListInfo();
		return qtrue;
	}

	if(!Q_stricmp(cmd, "imvotetally")) {
		CG_parseMapVoteTally();
		return qtrue;
	}
	// MAPVOTE END

	return qfalse;
}

int CG_Debriefing_ScrollGetMax( panel_button_t* button )
{
	// CG_Printf( "^3***CG_Debriefing_ScrollGetMax: %i\n", button->data[0]);
	switch( button->data[0] ) {
		case 0: // player list TODO:check case 0 - 2
			return 24;
		case 1: // weaponstats
			return 7;
		case 2: // campaign
			return 7;
		case 3:	// awards
			return NUMSHOW_ENDGAME_AWARDS;
		case 4: // MAPVOTES
			return 17;
	}
	return 0;
}

void CG_Debreifing2_Awards_Parse( void );

int CG_Debriefing_ScrollGetCount( panel_button_t* button )
{
	int i, cnt = 0;
	// CG_Printf( "^3***CG_Debriefing_ScrollGetCount: %i\n", button->data[0]);
	switch( button->data[0] ) {
		case 0: // player list TODO:check case 0 - 2
			for( i = 0; i < cgs.maxclients; i++ ) {
				if( !cgs.clientinfo[cgs.dbSortedClients[i]].infoValid ) {
					return i;
				}
			}
			return cgs.maxclients;
		case 1: // weaponstats
			if( !cgs.dbWeaponStatsRecieved ) {
				return 0;
			}
			for( i = 0; i < WS_MAX; i++ ) {
				if( cgs.dbWeaponStats[i].numShots ) {
					cnt++;
				}
			}
			return cnt;
		case 2: // campaign
			if( cgs.campaignInfoLoaded ) {
				return cgs.campaignData.mapCount;
			}
			return 0;
		case 3: // awards
			if( !cgs.dbAwardsParsed ) {
				CG_Debreifing2_Awards_Parse();
			}
			for( i = 0; i < NUM_ENDGAME_AWARDS; i++ ) {
				if ( cgs.dbAwardTeams[i] != -1 ) {
					cnt++;
				}
			}
			return cnt;
		case 4: // MAPVOTES
			return cgs.dbNumMaps;
	}
	return 0;
}

int CG_Debriefing_ScrollGetOffset( panel_button_t* button )
{
	// CG_Printf( "^3***CG_Debriefing_ScrollGetOffset: %i\n", button->data[0]);
	switch( button->data[0] ) {
		case 0:  // player list
			return cgs.dbPlayerListOffset;
		case 1: // weaponstats
			return cgs.dbWeaponListOffset;
		case 2: // campaign
			return cgs.tdbMapListOffset;
		case 3:	// awards
			return cgs.dbAwardsListOffset;
		case 4:	// MAPVOTES
			return cgs.dbMapVoteListOffset;
	}
	return 0;
}

void CG_Debriefing_ScrollSetOffset( panel_button_t* button, int ofs )
{
	// CG_Printf( "^3***CG_Debriefing_ScrollSetOffset: %i\n", button->data[0]);
	switch( button->data[0] ) {
		case 0:	// playerlist
			cgs.dbPlayerListOffset = ofs;
			return;
		case 1:	// weaponlist
			cgs.dbWeaponListOffset = ofs;
			return;
		case 2:	// campaign
			cgs.tdbMapListOffset = ofs;
			return;
		case 3:	// awards
			cgs.dbAwardsListOffset = ofs;
			return;
		case 4:
			cgs.dbMapVoteListOffset = ofs;
			return;
	}
}

void CG_Debriefing_ScrollGetBarRect( panel_button_t* button, rectDef_t* r )
{
	int max =		CG_Debriefing_ScrollGetMax		( button );
	int cnt =		CG_Debriefing_ScrollGetCount	( button );
	int offset =	CG_Debriefing_ScrollGetOffset	( button );

	if(cnt > max) {
		float h = button->rect.h;
		r->h = h * (max / (float)cnt);
		r->y = button->rect.y + (offset / (float)(cnt - max)) * (h-r->h);
	}
	else {
		r->h = button->rect.h;
		r->y = button->rect.y;
	}

	r->x = button->rect.x;
	r->w = button->rect.w;
}

void CG_Debriefing_ScrollCheckOffset( panel_button_t* button )
{
	int max =		CG_Debriefing_ScrollGetMax		( button );
	int cnt =		CG_Debriefing_ScrollGetCount	( button );
	int offset =	CG_Debriefing_ScrollGetOffset	( button );

	int maxofs = max( 0, cnt - max );

	if(offset > maxofs) {
		CG_Debriefing_ScrollSetOffset( button, maxofs );
	}
	else if(offset < 0) {
		CG_Debriefing_ScrollSetOffset( button, 0 );
	}
}

void CG_Debriefing_MouseEvent( int x, int y )
{
	panel_button_t* button;
	// CG_Printf( "^3***CG_Debriefing_MouseEvent: %i\n", cgs.dbMode);
	switch( cgs.dbMode ) {
		case 0: // mapvote
		case 2: // awards
		case 3: // campaign
			button = BG_PanelButtons_GetFocusButton();
			if( button && button->onDraw == CG_Debriefing_Scrollbar_Draw ) {
				rectDef_t r;
				int count, cnt;

				cnt = CG_Debriefing_ScrollGetCount( button );
				CG_Debriefing_ScrollGetBarRect( button, &r );

				button->data[1] += y;

				count = (cnt * button->data[1] * 0.5f) / (float)(r.h);
				if( count ) {
					int ofs = CG_Debriefing_ScrollGetOffset( button );
					CG_Debriefing_ScrollSetOffset( button, ofs + count );
					CG_Debriefing_ScrollCheckOffset( button );
					ofs = CG_Debriefing_ScrollGetOffset( button ) - ofs;

					if(ofs == count) {
						button->data[1] -= ofs * (r.h / (float)cnt);
					}
				}

				CG_Debriefing_ScrollGetBarRect( button, &r );
				cgs.cursorY = r.y + button->data[2];

				return;
			}
			break;
		default:
			break;
	}


	cgs.cursorX += x;
	if( cgs.cursorX < 0 ) {
		cgs.cursorX = 0;
	}
	else if( cgs.cursorX > 640 ) {
		cgs.cursorX = 640;
	}

	cgs.cursorY += y;
	if( cgs.cursorY < 0 ) {
		cgs.cursorY = 0;
	}
	else if( cgs.cursorY > 480 ) {
		cgs.cursorY = 480;
	}
}

void CG_Debriefing_Scrollbar_Draw( panel_button_t* button )
{
	rectDef_t r;
	vec4_t clr1 = { 41/255.f,	51/255.f,	43/255.f,	204/255.f };
	vec4_t clr2 = { 0.f,		0.f,		0.f,		153/255.f };

	CG_Debriefing_ScrollCheckOffset( button );

	CG_FillRect(				button->rect.x,	button->rect.y,	button->rect.w,	button->rect.h,	clr2);
	CG_DrawRect_FixedBorder(	button->rect.x,	button->rect.y,	button->rect.w,	button->rect.h,	1, colorMdGrey);

	CG_Debriefing_ScrollGetBarRect( button, &r );

	CG_FillRect(				r.x, r.y, r.w, r.h, clr1);
	CG_DrawRect_FixedBorder(	r.x, r.y, r.w, r.h, 1, colorMdGrey);
}

qboolean CG_Debriefing_Scrollbar_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		rectDef_t r;
		CG_Debriefing_ScrollGetBarRect( button, &r );
		if( BG_CursorInRect( &r ) ) {
			BG_PanelButtons_SetFocusButton( button );
			button->data[1] = 0;
			button->data[2] = cgs.cursorY - r.y;
		}
	}
	return qfalse;
}

qboolean CG_Debriefing_Scrollbar_KeyUp( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		if( BG_PanelButtons_GetFocusButton() == button ) {
			BG_PanelButtons_SetFocusButton( NULL );
		}
	}
	return qfalse;
}

void CG_Debriefing_KeyEvent( int key, qboolean down )
{
	switch( cgs.dbMode ) {
		case 0: // mapvote
			if( BG_PanelButtonsKeyEvent( key, down, mapVoteButtons ) ) {
				return;
			}
			break;
		case 1: // players
			break;
		case 2: // awards
			if( BG_PanelButtonsKeyEvent( key, down, teamDebriefPanelButtons ) ) {
				return;
			}
			break;
		case 3: // campaign
			if( BG_PanelButtonsKeyEvent( key, down, debriefPanelButtons ) ) {
				return;
			}
			break;
	}

	if( BG_PanelButtonsKeyEvent( key, down, chatPanelButtons ) ) {
		return;
	}

	if( !BG_PanelButtons_GetFocusButton() && down && key != K_MOUSE1 ) {
		BG_PanelButtons_SetFocusButton( &charPanelEdit );
		BG_PanelButton_EditClick( &charPanelEdit, key );
		BG_PanelButtons_SetFocusButton( NULL );
	}
}

void CG_Debriefing_PlayerSkills_Draw( panel_button_t* button )
{
	clientInfo_t* ci = CG_Debriefing_GetSelectedClientInfo();
	int i;
	float x;

	CG_Text_Paint_Ext( button->rect.x, button->rect.y - 2, button->font->scalex, button->font->scaley, button->font->colour, skillNames[button->data[0]], 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	x = button->rect.x;
	CG_DrawPic( x, button->rect.y, button->rect.w, button->rect.h, cgs.media.skillPics[ button->data[0] ] );
	// Meyer - move skill stars closer together
	x += button->rect.w - 4;
	for( i = ci->skill[button->data[0]]; i > 0; i-- ) {

		CG_DrawPicST( x + 4, button->rect.y + 1, button->rect.w * .8 , button->rect.h * .8, 0, 0, 1.f, 0.5f, cgs.media.limboStar_roll );

		x += button->rect.w - 4;
	}

	{
		vec4_t clr = { 1.f, 1.f, 1.f, 0.2f };
		trap_R_SetColor( clr );
		// Meyer - add more blank stars
		for( i = ci->skill[button->data[0]]; i < 9; i++ ) {

			CG_DrawPicST( x + 4, button->rect.y + 1, button->rect.w * .8, button->rect.h * .8, 0, 0, 1.f, 0.5f, cgs.media.limboStar_roll );

			x += button->rect.w - 4;
		}
		trap_R_SetColor( NULL );
	}
}

void CG_Debriefing_PlayerHitRegions_Draw( panel_button_t* button )
{
	int totalHits = cgs.dbHitRegions[HR_HEAD] + cgs.dbHitRegions[HR_ARMS] + cgs.dbHitRegions[HR_BODY] + cgs.dbHitRegions[HR_LEGS];
	float alphaH = (totalHits && cgs.dbHitRegions[HR_HEAD])? ((float)cgs.dbHitRegions[HR_HEAD]/(float)totalHits * 0.8f) + 0.2f : 0.0f;
	float alphaA = (totalHits && cgs.dbHitRegions[HR_ARMS])? ((float)cgs.dbHitRegions[HR_ARMS]/(float)totalHits * 0.8f) + 0.2f : 0.0f;
	float alphaB = (totalHits && cgs.dbHitRegions[HR_BODY])? ((float)cgs.dbHitRegions[HR_BODY]/(float)totalHits * 0.8f) + 0.2f : 0.0f;
	float alphaL = (totalHits && cgs.dbHitRegions[HR_LEGS])? ((float)cgs.dbHitRegions[HR_LEGS]/(float)totalHits * 0.8f) + 0.2f : 0.0f;
	vec4_t colorH, colorA, colorB, colorL;

	qhandle_t img = trap_R_RegisterShaderNoMip( "gfx/misc/hitregions.tga" );
	qhandle_t imgH = trap_R_RegisterShaderNoMip( "gfx/misc/hitregion_head.tga" );
	qhandle_t imgA = trap_R_RegisterShaderNoMip( "gfx/misc/hitregion_arms.tga" );
	qhandle_t imgB = trap_R_RegisterShaderNoMip( "gfx/misc/hitregion_body.tga" );
	qhandle_t imgL = trap_R_RegisterShaderNoMip( "gfx/misc/hitregion_legs.tga" );

	float w = CG_Text_Width_Ext( "Head: ", button->font->scalex, 0, button->font->font );
	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, "Head:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", cgs.dbHitRegions[HR_HEAD] ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	w = CG_Text_Width_Ext( "Arms: ", button->font->scalex, 0, button->font->font );
	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y+12, button->font->scalex, button->font->scaley, button->font->colour, "Arms:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
	CG_Text_Paint_Ext( button->rect.x, button->rect.y+12, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", cgs.dbHitRegions[HR_ARMS] ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	w = CG_Text_Width_Ext( "Body: ", button->font->scalex, 0, button->font->font );
	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y+2*12, button->font->scalex, button->font->scaley, button->font->colour, "Body:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
	CG_Text_Paint_Ext( button->rect.x, button->rect.y+2*12, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", cgs.dbHitRegions[HR_BODY] ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	w = CG_Text_Width_Ext( "Legs: ", button->font->scalex, 0, button->font->font );
	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y+3*12, button->font->scalex, button->font->scaley, button->font->colour, "Legs:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
	CG_Text_Paint_Ext( button->rect.x, button->rect.y+3*12, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", cgs.dbHitRegions[HR_LEGS] ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	// draw the image of a puppet, indicating red++ colors for higher hit-ratios per region
	CG_DrawPic( button->rect.x, button->rect.y-12, 54, 54, img );

	if ( alphaH ) {
		Vector4Set( colorH, 1.0f, 0.f, 0.f, alphaH );
		trap_R_SetColor( colorH );
		CG_DrawPic( button->rect.x, button->rect.y-12, 54, 54, imgH );
		trap_R_SetColor( NULL );
	}

	if ( alphaA ) {
		Vector4Set( colorA, 1.0f, 0.f, 0.f, alphaA );
		trap_R_SetColor( colorA );
		CG_DrawPic( button->rect.x, button->rect.y-12, 54, 54, imgA );
		trap_R_SetColor( NULL );
	}

	if ( alphaB ) {
		Vector4Set( colorB, 1.0f, 0.f, 0.f, alphaB );
		trap_R_SetColor( colorB );
		CG_DrawPic( button->rect.x, button->rect.y-12, 54, 54, imgB );
		trap_R_SetColor( NULL );
	}

	if ( alphaL ) {
		Vector4Set( colorL, 1.0f, 0.f, 0.f, alphaL );
		trap_R_SetColor( colorL );
		CG_DrawPic( button->rect.x, button->rect.y-12, 54, 54, imgL );
		trap_R_SetColor( NULL );
	}
}

void CG_Debriefing_PlayerACC_Draw( panel_button_t* button )
{
	clientInfo_t* ci = CG_Debriefing_GetSelectedClientInfo();
	float w = CG_Text_Width_Ext( "ACC: ", button->font->scalex, 0, button->font->font );

	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, "ACC:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i%%", ci->totalWeapAcc ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
}

void CG_Debriefing_PlayerXP_Draw( panel_button_t* button )
{
	clientInfo_t* ci = CG_Debriefing_GetSelectedClientInfo();
	float w = CG_Text_Width_Ext( "XP: ", button->font->scalex, 0, button->font->font );

	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, "XP:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", ci->score ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
}

void CG_Debriefing_PlayerTime_Draw( panel_button_t* button )
{
	score_t* score = NULL;
	int i;
	float w;

	for( i = 0; i < cgs.maxclients; i++ ) {
		if( cg.scores[i].client == cgs.dbSelectedClient ) {
			score = &cg.scores[i];
			break;
		}
	}
	if( !score ) {
		return;
	}

	w = CG_Text_Width_Ext( "Time: ", button->font->scalex, 0, button->font->font );
	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, "Time:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", score->time ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
}

void CG_Debriefing_PlayerMedals_Draw( panel_button_t* button )
{
	clientInfo_t* ci = CG_Debriefing_GetSelectedClientInfo();
	float x = button->rect.x;
	float w = CG_Text_Width_Ext( "Medals: ", button->font->scalex, 0, button->font->font );
	int i;

	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, "Medals:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	x = button->rect.x;
	for( i = 0; i < SK_NUM_SKILLS; i++ ) {
		if( ci->medals[i] ) {
			CG_DrawPic( x, button->rect.y - 10, 16, 16, cgs.media.medals[i] );

			x += 16 + 2;
		}
	}
}

void CG_Debriefing_PlayerRank_Draw( panel_button_t* button )
{
	clientInfo_t* ci = CG_Debriefing_GetSelectedClientInfo();
	float w = CG_Text_Width_Ext( "Rank: ", button->font->scalex, 0, button->font->font );

	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, "Rank:", 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, CG_Debriefing_FullRankNameForClientInfo( ci ), 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
}

void CG_Debriefing_PlayerName_Draw( panel_button_t* button ) {
	clientInfo_t* ci = CG_Debriefing_GetSelectedClientInfo();

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, ci->name, 0, 0, ITEM_TEXTSTYLE_SHADOWED, button->font->font );
}

clientInfo_t* CG_Debriefing_GetSelectedClientInfo( void )
{
	clientInfo_t* ci;

	if( cgs.dbSelectedClient < 0 || cgs.dbSelectedClient > cgs.maxclients ) {
		CG_Debrieing_SetSelectedClient( cg.clientNum );
	}

	ci = &cgs.clientinfo[cgs.dbSelectedClient];
	if( !ci->infoValid ) {
		CG_Debrieing_SetSelectedClient( cg.clientNum );
		ci = &cgs.clientinfo[cgs.dbSelectedClient];
	}

	return ci;
}

void CG_Debrieing_SetSelectedClient( int clientNum )
{
	if( clientNum < 0 || clientNum >= cgs.maxclients ) {
		return;
	}

	if( clientNum != cgs.dbSelectedClient ) {
		cgs.dbSelectedClient = clientNum;
		cgs.dbWeaponStatsRecieved = qfalse;
	}
}

void CG_Debriefing_HTMLButton_Draw( panel_button_t* button )
{
	if( cgs.dbMode != 1 ) {
		return;
	}

	CG_PanelButtonsRender_Button( button );
}

qboolean CG_Debriefing_ChatButton_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		cgs.dbChatMode = (cgs.dbChatMode+1)%3;

		if( cgs.dbChatMode > 0 ) {
			if( cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR ) {
				cgs.dbChatMode = 0;
			}

			if( cgs.dbChatMode > 1 ) {
				if( !CG_IsOnFireteam( cg.clientNum ) ) {
					cgs.dbChatMode = 0;
				}
			}
		}

		return qtrue;
	}

	return qfalse;
}

void CG_Debriefing_ReadyButton_Draw( panel_button_t* button )
{
	if( !cg.snap ) {
		return;
	}

	if( cg.snap->ps.eFlags & EF_READY ) {
		return;
	}

	// disable the ready button so players don't interrupt intermission
	// (just easier and we don't touch the vote vars for other gametypes)
	if (cgs.gametype == GT_WOLF_MAPVOTE) {
		return;
	}

	CG_PanelButtonsRender_Button( button );
}

void CG_Debriefing_ChatButton_Draw( panel_button_t* button )
{
	const char* str;

	switch( cgs.dbChatMode ) {
		case 1:
			str = "^5TO TEAM";
			break;
		case 2:
			str = "^3TO FIRETEAM";
			break;
		default:
			str = "^2TO GLOBAL";
			break;
	}

	CG_PanelButtonsRender_Button_Ext( &button->rect, str );
}

void CG_QuickMessage_f( void );

qboolean CG_Debriefing_ReadyButton_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		if( !cg.snap ) {
			return qfalse;
		}

		if( cg.snap->ps.eFlags & EF_READY ) {
			return qfalse;
		}

		trap_SendClientCommand( "imready" );

		return qtrue;
	}

	return qfalse;
}

qboolean CG_Debriefing_QCButton_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		CG_QuickMessage_f();
		return qtrue;
	}
	return qfalse;
}

qboolean CG_Debriefing_NextButton_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		cgs.dbMode = (cgs.dbMode + 1)%4;
		if ( cgs.gametype != GT_WOLF_MAPVOTE && cgs.dbMode == 0 ) {
			cgs.dbMode = 1;
		}
		return qtrue;
	}

	return qfalse;
}

void CG_Debriefing_NextButton_Draw( panel_button_t* button )
{
	CG_PanelButtonsRender_Button( button );
}

void CG_Debriefing_ChatEditFinish( panel_button_t* button )
{
	char buffer[256];
	trap_Cvar_VariableStringBuffer( button->text, buffer, 256 );

	switch( cgs.dbChatMode ) {
		case 0:
			trap_SendClientCommand( va( "say %s\n", buffer ) );
			break;
		case 1:
			trap_SendClientCommand( va( "say_team %s\n", buffer ) );
			break;
		case 2:
			trap_SendClientCommand( va( "say_buddy %s\n", buffer ) );
			break;
	}

	trap_Cvar_Set( button->text, "" );
}

float CG_Debriefing_CalcCampaignProgress( void )
{
	int i;

	if( !cgs.campaignInfoLoaded ) {
		return 0;
	}

	for( i = 0; i < cgs.campaignData.mapCount; i++ ) {
		if( !Q_stricmp( cgs.campaignData.mapnames[i], cgs.rawmapname ) ) {
			return (i+1) / (float)cgs.campaignData.mapCount;
		}
	}

	return 0;
}

qboolean CG_TeamDebriefingMapList_KeyDown( panel_button_t* button, int key )
{
	// core: only check buttons for campaign gametype..
	if ( cgs.gametype != GT_WOLF_CAMPAIGN ) {
		return qfalse;
	}

	if( key == K_MOUSE1 ) {
		int pos	= (( cgs.cursorY - button->rect.y ) / 12) + cgs.tdbMapListOffset;

		if( pos < 0 || pos > cgs.campaignData.mapCount ) {
			return qfalse;
		}

		cgs.tdbSelectedMap = pos;

		return qtrue;
	}

	return qfalse;
}

void CG_TeamDebriefingOutcome_Draw( panel_button_t* button )
{
	const char* cs;
	char *s, *p;
	char buffer[1024];
	float y;

	// core: only draw this for campaign gametype..
	if ( cgs.gametype != GT_WOLF_CAMPAIGN ) {
		return;
	}

//	DC->fillRect( button->rect.x, button->rect.y, button->rect.w, button->rect.h, colorRed );

	if( cgs.tdbSelectedMap == 0 ) {
		return;
	}
	else {
		if( cg.teamWonRounds[1] & (1 << (cgs.tdbSelectedMap-1)) ) {
			cs = cgs.campaignData.arenas[ cgs.tdbMapListOffset - 1].axiswintext;
		}
		else if( cg.teamWonRounds[0]  & (1 << (cgs.tdbSelectedMap-1)) ) {
			cs = cgs.campaignData.arenas[ cgs.tdbMapListOffset - 1].alliedwintext;
		}
		else {
			return;
		}
	}

	Q_strncpyz( buffer, cs, sizeof(buffer) );
	while ((s = strchr(buffer, '*'))) {
		*s = '\n';
	}

	BG_FitTextToWidth_Ext( buffer, button->font->scalex, button->rect.w - 16, sizeof(buffer), button->font->font );

	y = button->rect.y + 12;

	s = p = buffer;
	while(*p) {
		if(*p == '\n') {
			*p++ = '\0';
			CG_Text_Paint_Ext( button->rect.x + 4, y, button->font->scalex, button->font->scaley, button->font->colour, s, 0, 0, 0, button->font->font );
			y += 8;
			s = p;
		}
		else {
			p++;
		}
	}
}

void CG_TeamDebriefingMapList_Draw( panel_button_t* button )
{
	float y = button->rect.y + 12;
	vec4_t clr = { 1.f, 1.f, 1.f, 0.3f };

	// core: only draw this for campaign gametype..
	if ( cgs.gametype == GT_WOLF_CAMPAIGN ) {
		int i;

		// draw the campaign map..
		for( i = 0; i + cgs.tdbMapListOffset <= MAX_MAPS_PER_CAMPAIGN && i < 4; i++ ) {
			if( cgs.tdbSelectedMap == i + cgs.tdbMapListOffset ) {

				CG_FillRect( button->rect.x, y -10, button->rect.w, 12, clr );
			}

			if( i + cgs.tdbMapListOffset == 0 ) {
				CG_Text_Paint_Ext( button->rect.x, y, button->font->scalex, button->font->scaley, button->font->colour, "Campaign Overview", 0, 0, 0, button->font->font );
			}
			else {
				CG_Text_Paint_Ext( button->rect.x, y, button->font->scalex, button->font->scaley, button->font->colour, cgs.campaignData.arenas[ i + cgs.tdbMapListOffset - 1].longname, 0, 0, 0, button->font->font );
			}

			y += 12;
		}
	}
}

int CG_TeamDebriefing_CalcXP( team_t team, int mapindex, int skillindex )
{
	int j, cnt = 0;

	if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
		int i;

		for( i = 0; i < cgs.campaignData.mapCount; i++ ) {
			if( mapindex != -1 && i != mapindex ) {
				continue;
			}

			for( j = 0; j < SK_NUM_SKILLS; j++ ) {
				if( skillindex != -1 && j != skillindex ) {
					continue;
				}

				cnt += team == TEAM_AXIS ? cgs.tdbAxisMapsXP[ j ][ i ] : cgs.tdbAlliedMapsXP[ j ][ i ];
			}
		}
	}
	else if( cg_gameType.integer == GT_WOLF ||
			cg_gameType.integer == GT_WOLF_STOPWATCH ||
			cg_gameType.integer == GT_WOLF_MAPVOTE ) {
		for( j = 0; j < SK_NUM_SKILLS; ++j ) {
			if( skillindex != -1 && j != skillindex ) {
				continue;
			}

			cnt += team == TEAM_AXIS ? cgs.tdbAxisMapsXP[ j ][ 0 ] : cgs.tdbAlliedMapsXP[ j ][ 0 ];
		}
	}

	return cnt;
}

void CG_TeamDebriefingTeamXP_Draw( panel_button_t* button )
{
	team_t team = button->data[0] == 0 ? TEAM_AXIS : TEAM_ALLIES;

	int xp = CG_TeamDebriefing_CalcXP( team, cgs.tdbSelectedMap - 1, -1 );

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, va( "%s XP: %i", team == TEAM_AXIS ? "Axis" : "Allies", xp ), 0, 0, 0, button->font->font );
}

void CG_TeamDebriefingTeamSkillXP_Draw( panel_button_t* button )
{
	team_t team = button->data[0] == 0 ? TEAM_AXIS : TEAM_ALLIES;

	int xp;

	if( button->data[1] == SK_NUM_SKILLS ) {
		xp = CG_TeamDebriefing_CalcXP( team, cgs.tdbSelectedMap - 1, -1 );
	}
	else {
		xp = CG_TeamDebriefing_CalcXP( team, cgs.tdbSelectedMap - 1, button->data[1] );
	}

//	const char* text = va( "%s: ", skillNames[ button->data[1] ] );

//	float w = CG_Text_Width_Ext( text, button->font->scalex, 0, button->font->font );

//	CG_Text_Paint_Ext( button->rect.x - w, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, text, 0, 0, 0, button->font->font );

	CG_Text_Paint_Ext( button->rect.x, button->rect.y, button->font->scalex, button->font->scaley, button->font->colour, va( "%i", xp ), 0, 0, 0, button->font->font );
}

void CG_TeamDebriefingMapShot_Draw( panel_button_t* button )
{
	if( cgs.tdbSelectedMap == 0 ) {
		CG_DrawPicST( button->rect.x, button->rect.y, button->rect.w, button->rect.h, 0, 0, 0.6875, 1, trap_R_RegisterShaderNoMip( "gfx/loading/map_back" ) );
	}
	else {
		CG_DrawPic( button->rect.x, button->rect.y, button->rect.w, button->rect.h, trap_R_RegisterShaderNoMip( va( "levelshots/%s_cc.tga", cgs.campaignData.mapnames[cgs.tdbSelectedMap-1] ) ) );
	}
}

void CG_TeamDebriefingMapWinner_Draw( panel_button_t* button )
{
}

void CG_PanelButtonsRender_Button_Ext( rectDef_t* r, const char* text )
{
	vec4_t clrBdr =	{ 0.1f,		0.1f,	0.1f,	0.5f	};
	vec4_t clrBck =	{ 0.3f,		0.3f,	0.3f,	0.4f	};
	vec4_t clrTxt =	{ 0.6f,		0.6f,	0.6f,	1.0f	};

	vec4_t clrBck_hi = { 0.5f,	0.5f,	0.5f,	0.4f	};
	vec4_t clrTxt_hi = { 0.9f,	0.9f,	0.9f,	1.f		};

	qboolean hilight = BG_CursorInRect( r );

	CG_FillRect( r->x, r->y, r->w, r->h, hilight ? clrBck_hi : clrBck );
	CG_DrawRect_FixedBorder( r->x, r->y, r->w, r->h, 1, clrBdr );

	if( text ) {
		float w = CG_Text_Width_Ext( text, 0.2f, 0, &cgs.media.limboFont2 );
		CG_Text_Paint_Ext( r->x + ((r->w+2) - w) * 0.5f, r->y + 11, 0.19f, 0.19f, hilight ? clrTxt_hi : clrTxt, text, 0, 0, 0, &cgs.media.limboFont2 );
	}
}

void CG_PanelButtonsRender_Button( panel_button_t* button )
{
	CG_PanelButtonsRender_Button_Ext( &button->rect, button->text );
}

void CG_PanelButtonsRender_Window_Ext( rectDef_t* r, const char* text, int align, int innerheight, float fontscale, int yofs )
{
	vec4_t clrBck =			{ 0.0f,		0.0f,	0.0f,	0.8f	};
	vec4_t clrTxtBck =		{ 0.6f,		0.6f,	0.6f,	1.0f	};

	CG_FillRect( r->x, r->y, r->w, r->h, clrBck );
	CG_DrawRect_FixedBorder( r->x, r->y, r->w, r->h, 1, HUD_Border );

	CG_FillRect( r->x + 2, r->y + 2, r->w - 4, innerheight, HUD_Background );

	if( text ) {
		float x;

		if( align == ITEM_ALIGN_CENTER ) {
			float w = CG_Text_Width_Ext( text, fontscale, 0, &cgs.media.limboFont1 );
			x = r->x + (r->w - w) * 0.5f;
		}
		else if( align == ITEM_ALIGN_RIGHT ) {
			float w = CG_Text_Width_Ext( text, fontscale, 0, &cgs.media.limboFont1 );
			x = r->x + r->w - w;
		}
		else {
			x = r->x + 5;
		}

		CG_Text_Paint_Ext( x, r->y + yofs, fontscale, fontscale, clrTxtBck, text, 0, 0, 0, &cgs.media.limboFont1 );
	}
}


void CG_PanelButtonsRender_Window( panel_button_t* button )
{
	CG_PanelButtonsRender_Window_Ext( &button->rect, button->text, button->data[0], 12, 0.19f, 11 );
}

const char* CG_Debreifing2_WinStringForTeam( team_t team )
{
	switch( team ) {
		case TEAM_ALLIES:
			return "ALLIES WIN!";
		case TEAM_AXIS:
			return "AXIS WIN!";
		default:
			return "IT'S A TIE!";
	}
}

void CG_Debreifing2_MissionTitle_Draw( panel_button_t* button )
{
	const char* s;
	float x, w;
	vec4_t clrTxtBck =		{ 0.6f,		0.6f,	0.6f,	1.0f	};

	if( cg_gameType.integer == GT_WOLF_STOPWATCH ) {
		int defender, winner;

		s = CG_ConfigString( CS_MULTI_INFO );
		defender = atoi( Info_ValueForKey( s, "d" ) );	//defender

		s = CG_ConfigString( CS_MULTI_MAPWINNER );
		winner = atoi( Info_ValueForKey( s, "winner" ) );

		if ( cgs.currentRound ) {
			// first round
			s = va( CG_TranslateString( "Clock is now set to %s!" ), CG_Debriefing_TimeToString( cgs.nextTimeLimit * 60.f * 1000.f ) );
		}
		else {
			// second round
			if( !defender ) {
				if( winner != defender ) {
					s = "ALLIES SUCCESSFULLY BEAT THE CLOCK!";
				}
				else {
					s = "ALLIES COULDN'T BEAT THE CLOCK!";
				}
			}
			else {
				if ( winner != defender ) {
					s = "AXIS SUCCESSFULLY BEAT THE CLOCK!";
				}
				else {
					s = "AXIS COULDN'T BEAT THE CLOCK!";
				}
			}
		}
		CG_PanelButtonsRender_Window_Ext( &button->rect, s, 0, 18, 0.25f, 16 );
	}
	else if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
		CG_PanelButtonsRender_Window_Ext( &button->rect, CG_Debreifing2_WinStringForTeam(CG_Debriefing_FindWinningTeamForMap()), 0, 18, 0.25f, 16 );

		s = va( "CAMPAIGN STATUS: %s", CG_Debreifing2_WinStringForTeam(CG_Debriefing_FindOveralWinningTeam()) );
		w = CG_Text_Width_Ext( s, 0.25f, 0, &cgs.media.limboFont1 );
		CG_Text_Paint_Ext( button->rect.x + (button->rect.w-w)*0.5f, button->rect.y + 16, 0.25f, 0.25f, clrTxtBck, s, 0, 0, 0, &cgs.media.limboFont1 );
	}
	else {
		CG_PanelButtonsRender_Window_Ext( &button->rect, CG_Debreifing2_WinStringForTeam(CG_Debriefing_FindOveralWinningTeam()), 0, 18, 0.25f, 16 );
	}

	s = va( "%i SECS TO NEXT MAP", max( 60 - (cg.time - cgs.intermissionStartTime) / 1000, 0 ) );

	w = CG_Text_Width_Ext( s, 0.25f, 0, &cgs.media.limboFont1 );
	x = button->rect.x + button->rect.w - w - 4;

	CG_Text_Paint_Ext( x, button->rect.y + 16, 0.25f, 0.25f, clrTxtBck, s, 0, 0, 0, &cgs.media.limboFont1 );
}

// jet Pilot - new Award names,  core: added more..
const char* awardNames[ NUM_ENDGAME_AWARDS ] = {
	"Highest Fragger",				// most kills
	"Most Headshots",				// most headshots
	"Highest Overall Accuracy",		// best total accuracy
	"Best Killing Spree",			// most kills in a killing spree (minimum spree of 5)
	"Map XP Collector",				// most XP gain on this map
	"Jack the Ripper Award",		// most knife kills
	"Dr. Overdose",					// most poison kills
	"The Executioner",				// killed from behind
	"Bounty Hunter",				// most bounty collected
	"Kick Ass Award",				// most kick kills
	"Super Mario Award",			// minimum of 1 goomba
	"War Hero Award",				// best mission/objective play
	"Purple Heart Award",			// most deaths (not including darwin/newton/panzernoob)
	"Best Grenadier",				// most (throwing) grenade kills
	"Riflenade Master",				// most kills with riflenade
	"Panzernoob",					// most self/teamkills with panzer !! Darwin does no longer include self-panzer-kills
	"Best Engineer",				// best engineer
	"Best Medic",					// best medic
	"Life Saver Award",				// most rescusetations
	"Darwin Award",					// Most deaths from non-players (falling, crushed) and accidental self-kills
	"Isaac Newton Award",			// most falling deaths
	"I Ain't Got No Friends Award",	// most team kills (minimum of 5 tks)
	"Artillery Spammer",
	"Highest Experience Points",	// most xp
	"Highest Ranking Officer",		// highest rank
};

void CG_Debreifing2_Awards_Parse( void )
{
	int			i = 0;
	char*		cs = (char*)CG_ConfigString( CS_ENDGAME_STATS );
	const char* token;
	char*		s;
	int			size, len;
	char		buffer[sizeof( cgs.dbAwardNamesBuffer )];
	float		vf;
	int			clientNum;

	Q_strncpyz( buffer, cs, sizeof( cgs.dbAwardNamesBuffer ) );
	cs = buffer;

	s		= cgs.dbAwardNamesBuffer;
	size	= sizeof( cgs.dbAwardNamesBuffer );
	for( i = 0; i < NUM_ENDGAME_AWARDS; i++ ) {
		// clientNum
		token = COM_Parse( &cs );
		clientNum = atoi(token);
		if ( clientNum >= 0 && clientNum < MAX_CLIENTS ) {
			Q_strncpyz( s, va("%s", cgs.clientinfo[clientNum].name), size );
		}
		else {
			Q_strncpyz( s, "", size );
		}

		// score
		token = COM_Parse( &cs );
		vf = atof(token);
		if ( vf > 0 ) {
			strcat( s, (vf == (int)(vf))? va("^7 (%i)",(int)(vf)) : va("^7 (%.1f)",vf) );
		}

		cgs.dbAwardNames[ i ] = s;
		len	 = strlen( s );
		size -= len;
		s += len + 1;

		// teamNum
		token = COM_Parse( &cs );
		cgs.dbAwardTeams[ i ] = atoi(token);
	}

	cgs.dbAwardsParsed = qtrue;
}

void CG_Debreifing2_Awards_Draw( panel_button_t* button )
{
	int		i,j;
	float	y = button->rect.y + 1;
	vec4_t	clrTxtBck = { 0.6f, 0.6f, 0.6f, 1.0f };

	if( !cgs.dbAwardsParsed ) {
		CG_Debreifing2_Awards_Parse();
	}

	for( i = j = 0; i < NUM_ENDGAME_AWARDS && j < NUMSHOW_ENDGAME_AWARDS; ++i ) {
		if ( i + cgs.dbAwardsListOffset >= NUM_ENDGAME_AWARDS ) break;
		if( cgs.dbAwardTeams[ i + cgs.dbAwardsListOffset ] == -1 ) continue;
		++j;

		CG_DrawPic( button->rect.x + 6, y + 2, 18, 12, cgs.dbAwardTeams[i + cgs.dbAwardsListOffset] == TEAM_AXIS ? cgs.media.axisFlag : cgs.media.alliedFlag );

		CG_Text_Paint_Ext( button->rect.x + 28, y + 11, 0.19f, 0.19f, clrTxtBck, awardNames[i + cgs.dbAwardsListOffset], 0, 0, 0, &cgs.media.limboFont2 );

		CG_Text_Paint_Ext( button->rect.x + 28 + 180, y + 11, 0.19f, 0.19f, clrTxtBck, cgs.dbAwardNames[i + cgs.dbAwardsListOffset], 0, 0, 0, &cgs.media.limboFont2 );
		y += 16;
	}
}

void CG_Debreifing2_Maps_Draw( panel_button_t* button )
{
	vec4_t clrTxtBck =		{ 0.6f,		0.6f,	0.6f,	1.0f	};
	vec4_t clrBck =			{ 0.3f,		0.3f,	0.3f,	0.4f	};

	if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
		float y, w;
		int i;

		if( !cgs.campaignInfoLoaded ) {
			return;
		}

		if( cgs.tdbSelectedMap == 0 ) {
			CG_FillRect( button->rect.x + 2, button->rect.y + 2, button->rect.w - 4, 12, clrBck );
		}
		CG_Text_Paint_Ext( button->rect.x + 4, button->rect.y + 11, 0.19f, 0.19f, clrTxtBck, va( "Campaign: %s", cgs.campaignData.campaignName ), 0, 0, 0, &cgs.media.limboFont2 );

		y = button->rect.y + 14;
		for( i = 0; i < cgs.campaignData.mapCount; i++ ) {
			const char* str;

			if( cgs.tdbSelectedMap == i+1 ) {
				CG_FillRect( button->rect.x + 2, y + 2, button->rect.w - 4, 12, clrBck );
			}

			CG_Text_Paint_Ext( button->rect.x + 8, y + 11, 0.19f, 0.19f, clrTxtBck, va( "%i. %s", i+1, cgs.campaignData.arenas[i].longname ), 0, 0, 0, &cgs.media.limboFont2 );

			if( i <= cgs.currentCampaignMap ) {
				str = CG_Debreifing2_WinStringForTeam( CG_Debriefing_FindWinningTeamForPos( i + 1 ) );

				w = CG_Text_Width_Ext( str, 0.2f, 0, &cgs.media.limboFont2 );
				CG_Text_Paint_Ext( button->rect.x + button->rect.w - w - 8, y + 11, 0.19f, 0.19f, clrTxtBck, str, 0, 0, 0, &cgs.media.limboFont2 );
			}

			y += 14;
		}
	}
	//else if( cg_gameType.integer == GT_WOLF_STOPWATCH ) {
	//}
}

void CG_Debreifing2_Mission_Draw( panel_button_t* button )
{
	if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
		if( !cgs.campaignInfoLoaded ) {
			return;
		}

		if( cgs.campaignData.mapTC[0][0] && cgs.campaignData.mapTC[1][0] ) {
			int i;
			vec4_t colourFadedBlack = { 0.f, 0.f, 0.f, 0.4f };
			float x, y, w;

			CG_DrawPicST( button->rect.x, button->rect.y, button->rect.w, button->rect.h, cgs.campaignData.mapTC[0][0] / 1024.f, cgs.campaignData.mapTC[0][1] / 1024.f, cgs.campaignData.mapTC[1][0] / 1024.f, cgs.campaignData.mapTC[1][1] / 1024.f, trap_R_RegisterShaderNoMip( "gfx/loading/camp_map" ) );

			for( i = cgs.campaignData.mapCount-1; i >= 0; i-- ) {

				x = button->rect.x + ( ( cgs.campaignData.arenas[i].mappos[0] - cgs.campaignData.mapTC[0][0] ) / 650.f * button->rect.w );
				y = button->rect.y + ( ( cgs.campaignData.arenas[i].mappos[1] - cgs.campaignData.mapTC[0][1] ) / 650.f * button->rect.h );

				w = CG_Text_Width_Ext( cgs.campaignData.arenas[i].longname, 0.2f, 0, &cgs.media.limboFont2 );

				// CHRUKER: b049 - Correct placement of the map name
				// Pin half width is 12
				// Pin left margin is 3
				// Pin right margin is 0
				// Text margin is 2
				if( x + 14 + w > button->rect.x + button->rect.w ) {
					// x - pinhwidth (12) - pin left margin (3) - w - text margin (2) => x - w - 17
					CG_FillRect( x - w - 17 + 1, y - 6 + 1, 17 + w, 12, colourFadedBlack );
					CG_FillRect( x - w - 17, y - 6, 17 + w, 12, colorBlack );
				}
				else {
					// Width = pinhwidth (12) + pin right margin (0) + w + text margin (2) = 14 + w
					CG_FillRect( x + 1, y - 6 + 1, 14 + w, 12, colourFadedBlack );
					CG_FillRect( x, y - 6, 14 + w, 12, colorBlack );
				} // b049

				switch( CG_Debriefing_FindWinningTeamForPos( i + 1 ) ) {
					case TEAM_AXIS:
						CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_axis" ) );
						break;
					case TEAM_ALLIES:
						CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_allied" ) );
						break;
					default:
						CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_neutral" ) );
						break;
				}
				// CHRUKER: b049 - Correct placement of the map name
				if( x + 14 + w > button->rect.x + button->rect.w ) {
					// x - pinhwidth (12) - pin left margin (3) - w => x - w - 15
					CG_Text_Paint_Ext( x - w - 15, y + 3, 0.2f, 0.2f, colorWhite, cgs.campaignData.arenas[i].longname, 0, 0, 0, &cgs.media.limboFont2 );
				}
				else {
					// x + pinhwidth (12) + pin right margin (0) => x + 12
					CG_Text_Paint_Ext( x + 12, y + 3, 0.2f, 0.2f, colorWhite, cgs.campaignData.arenas[i].longname, 0, 0, 0, &cgs.media.limboFont2 );
				} // b049
			}

			if( cgs.tdbSelectedMap ) {
				float x = button->rect.x + ( ( cgs.campaignData.arenas[cgs.tdbSelectedMap-1].mappos[0] - cgs.campaignData.mapTC[0][0] ) / 650.f * button->rect.w );
				float y = button->rect.y + ( ( cgs.campaignData.arenas[cgs.tdbSelectedMap-1].mappos[1] - cgs.campaignData.mapTC[0][1] ) / 650.f * button->rect.h );

				switch( CG_Debriefing_FindWinningTeamForPos( cgs.tdbSelectedMap ) ) {
					case TEAM_AXIS:
						CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_axis" ) );
						break;
					case TEAM_ALLIES:
						CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_allied" ) );
						break;
					default:
						break;
				}
			}

		}
		else {
			CG_DrawPic( button->rect.x, button->rect.y, button->rect.w, button->rect.h, trap_R_RegisterShaderNoMip("menu/art/unknownmap"));
		}
		return;
	}

	if( !cgs.arenaInfoLoaded ) {
		return;
	}

	if ( cgs.gametype == GT_WOLF_CAMPAIGN ) {
		if( cgs.arenaData.mappos[0] && cgs.arenaData.mappos[1] ) {
			float x, y, w;
			vec2_t tl, br;
			vec4_t colourFadedBlack = { 0.f, 0.f, 0.f, 0.4f };

			tl[0] = cgs.arenaData.mappos[0] - .5*650.f;
			if( tl[0] < 0 )
				tl[0] = 0;
			br[0] = tl[0] + 650.f;
			if( br[0] > 1024.f ) {
				br[0] = 1024.f;
				tl[0] = br[0] - 650.f;
			}

			tl[1] = cgs.arenaData.mappos[1] - .5*650.f;
			if( tl[1] < 0 )
				tl[1] = 0;
			br[1] = tl[1] + 650.f;
			if( br[1] > 1024.f ) {
				br[1] = 1024.f;
				tl[1] = br[1] - 650.f;
			}

			CG_DrawPicST( button->rect.x, button->rect.y, button->rect.w, button->rect.h, tl[0] / 1024.f, tl[1] / 1024.f, br[0] / 1024.f, br[1] / 1024.f, trap_R_RegisterShaderNoMip( "gfx/loading/camp_map" ) );

			x = button->rect.x + ( ( cgs.arenaData.mappos[0] - tl[0] ) / 650.f * button->rect.w );
			y = button->rect.y + ( ( cgs.arenaData.mappos[1] - tl[1] ) / 650.f * button->rect.h );

			w = CG_Text_Width_Ext( cgs.arenaData.longname, 0.2f, 0, &cgs.media.limboFont2 );

			// CHRUKER: b049 - Correct placement of the map name
			// Pin half width is 12
			// Pin left margin is 3
			// Pin right margin is 0
			// Text margin is 2
			if( x + 14 + w > button->rect.x + button->rect.w ) {
				// x - pinhwidth (12) - pin left margin (3) - w - text margin (2) => x - w - 17
				CG_FillRect( x - w - 17 + 1, y - 6 + 1, 17 + w, 12, colourFadedBlack );
				CG_FillRect( x - w - 17, y - 6, 17 + w, 12, colorBlack );
			}
			else {
				// Width = pinhwidth (12) + pin right margin (0) + w + text margin (2) = 14 + w
				CG_FillRect( x + 1, y - 6 + 1, 14 + w, 12, colourFadedBlack );
				CG_FillRect( x, y - 6, 14 + w, 12, colorBlack );
			} // b049

			switch( CG_Debriefing_FindWinningTeam() ) {
				case TEAM_AXIS:
					CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_axis" ) );
					break;
				case TEAM_ALLIES:
					CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_allied" ) );
					break;
				default:
					CG_DrawPic( x - 12, y - 12, 24, 24, trap_R_RegisterShaderNoMip( "gfx/loading/pin_neutral" ) );
					break;
			}

			// CHRUKER: b049 - Correct placement of the map name
			if( x + 14 + w > button->rect.x + button->rect.w ) {
				// x - pinhwidth (12) - pin left margin (3) - w => x - w - 15
				CG_Text_Paint_Ext( x - w - 15, y + 3, 0.2f, 0.2f, colorWhite, cgs.arenaData.longname, 0, 0, 0, &cgs.media.limboFont2 );
			}
			else {
				// x + pinhwidth (12) + pin right margin (0) => x + 12
				CG_Text_Paint_Ext( x + 12, y + 3, 0.2f, 0.2f, colorWhite, cgs.arenaData.longname, 0, 0, 0, &cgs.media.limboFont2 );
			} // b049
		}
		else {
			CG_DrawPic( button->rect.x, button->rect.y, button->rect.w, button->rect.h, trap_R_RegisterShaderNoMip( "menu/art/unknownmap" ) );
		}
	} else {
		// draw the "photograph" image of the map..
		// ..and the long mapname.
		// ..and the BSP name.
		vec4_t colourFadedDkGrey = {0.25, 0.25, 0.25, 0.35};
		float h = (177.0f /233.0f * button->rect.w);
		CG_DrawPic( button->rect.x, button->rect.y, button->rect.w, h, trap_R_RegisterShaderNoMip( va( "levelshots/%s.tga", cgs.rawmapname ) ) );
		CG_FillRect( button->rect.x + 2, button->rect.y + h + 6, button->rect.w - 4, 14, colourFadedDkGrey );
		CG_Text_Paint_Centred_Ext( button->rect.x + (button->rect.w / 2), button->rect.y + h + 16, 0.2f, 0.2f, colorWhite, cgs.arenaData.longname, 0, 0, 0, &cgs.media.limboFont2 );
		CG_Text_Paint_Ext( button->rect.x, button->rect.y + h + 20 + 20, 0.2f, 0.2f, colorMdGrey, cgs.mapname, 0, 0, 0, &cgs.media.limboFont2 );
	}
}

team_t CG_Debriefing_FindWinningTeamForMap( void )
{
	const char* s = CG_ConfigString( CS_MULTI_MAPWINNER );
	const char* buf = Info_ValueForKey( s, "winner" );

	if( atoi( buf ) == -1 ) {
	}
	else if( atoi( buf ) ) {
		return TEAM_ALLIES;
	}
	else {
		return TEAM_AXIS;
	}

	return TEAM_FREE;
}

team_t CG_Debriefing_FindWinningTeamForPos( int pos )
{
	if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
		if( pos == 0 ) {
			int i;
			int axiswins = 0, alliedwins = 0;

			for( i = 0; i < cgs.campaignData.mapCount; i++ ) {
				if( cg.teamWonRounds[1] & (1 << i) ) {
					axiswins++;
				}
				else if( cg.teamWonRounds[0]  & (1 << i) ) {
					alliedwins++;
				}
			}

			if( axiswins > alliedwins ) {
				return TEAM_AXIS;
			}
			else if( alliedwins > axiswins ) {
				return TEAM_ALLIES;
			}
		}
		else {
			if( cg.teamWonRounds[1] & (1 << (pos-1)) ) {
				return TEAM_AXIS;
			}
			else if( cg.teamWonRounds[0]  & (1 << (pos-1)) ) {
				return TEAM_ALLIES;
			}
		}
	}
	else if(cg_gameType.integer == GT_WOLF ||
			cg_gameType.integer == GT_WOLF_LMS ||
			cg_gameType.integer == GT_WOLF_MAPVOTE) {
		const char* s = CG_ConfigString( CS_MULTI_MAPWINNER );
		const char* buf = Info_ValueForKey( s, "winner" );

		if( atoi( buf ) == -1 ) {
		}
		else if( atoi( buf ) ) {
			return TEAM_ALLIES;
		}
		else {
			return TEAM_AXIS;
		}
	}
	else if( cg_gameType.integer == GT_WOLF_STOPWATCH ) {
		int winner;
		const char* s = CG_ConfigString( CS_MULTI_INFO );
		int defender = atoi( Info_ValueForKey( s, "d" ) );	//defender

		s = CG_ConfigString( CS_MULTI_MAPWINNER );
		winner = atoi( Info_ValueForKey( s, "winner" ) );

		if( !cgs.currentRound ) {
			// second round
			if( !defender ) {
				if( winner != defender ) {
					return TEAM_ALLIES;
				} else {
					return TEAM_AXIS;
				}
			}
			else {
				if ( winner != defender ) {
					return TEAM_AXIS;
				}
				else {
					return TEAM_ALLIES;
				}
			}
		}
	}

	return TEAM_FREE;
}

team_t CG_Debriefing_FindOveralWinningTeam( void )
{
	return CG_Debriefing_FindWinningTeamForPos( 0 );
}

team_t CG_Debriefing_FindWinningTeam( void )
{
	if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
		return CG_Debriefing_FindWinningTeamForPos( cgs.tdbSelectedMap );
	}
	return CG_Debriefing_FindOveralWinningTeam();
}

qboolean CG_Debriefing2_Maps_KeyDown( panel_button_t* button, int key )
{
	if( key == K_MOUSE1 ) {
		if( cg_gameType.integer == GT_WOLF_CAMPAIGN ) {
			int pos	= (( cgs.cursorY - button->rect.y ) / 14) + cgs.tdbMapListOffset;

			if( pos < 0 || pos > cgs.currentCampaignMap + 1 ) {
				return qfalse;
			}

			cgs.tdbSelectedMap = pos;
		}

		return qtrue;
	}

	return qfalse;
}

int skillPositions[ SK_NUM_SKILLS + 1 ] = {
	0,
	70,
	140,
	210,
	280,
	350,
	420,
	490,
};

void CG_Debriefing2TeamSkillHeaders_Draw( panel_button_t* button )
{
	if( cg_gameType.integer == GT_WOLF_LMS ) {
		return;
	}

	{
		float w;
		int i, j;
		vec4_t clrTxtBck =		{ 0.6f,		0.6f,	0.6f,	1.0f	};

		for( j = 0; j < 2; j++ ) {
			for( i = 0; i <= SK_NUM_SKILLS; i++ ) {
				const char* str;

				if( j == 0 ) {
					if( i == SK_NUM_SKILLS ) {
						str = "Total";
					}
					else {
						str = skillNamesLine1[ i ];
					}
				}
				else {
					if( i == SK_NUM_SKILLS ) {
						str = "";
					}
					else {
						str = skillNamesLine2[ i ];
					}
				}

				if( *str ) {
					w = CG_Text_Width_Ext( str, 0.2f, 0, &cgs.media.limboFont2 );

					CG_Text_Paint_Ext( button->rect.x + 100 + skillPositions[ i ] - (w*0.5f), button->rect.y + (j*11), 0.2f, 0.2f, clrTxtBck, str, 0, 0, 0, &cgs.media.limboFont2 );
				}
			}
		}
	}
}

void CG_Debriefing2TeamSkillXP_Draw( panel_button_t* button )
{
	team_t winner = CG_Debriefing_FindOveralWinningTeam();
	team_t team;
	float scale;
	int xp, i;
	vec4_t clrTxtBck =		{ 0.6f,		0.6f,	0.6f,	1.0f	};

	if( cg_gameType.integer == GT_WOLF_LMS ) {
		return;
	}

	if( button->data[0] ) {
		team = winner == TEAM_AXIS ? TEAM_ALLIES : TEAM_AXIS;
	}
	else {
		team = winner == TEAM_AXIS ? TEAM_AXIS : TEAM_ALLIES;
	}

	if( team == winner ) {
		scale = 0.3f;
	}
	else {
		scale = 0.2f;
	}

	switch( team ) {
		case TEAM_AXIS:
			CG_Text_Paint_Ext( button->rect.x, button->rect.y + 11, scale, scale, clrTxtBck, "Axis", 0, 0, 0, &cgs.media.limboFont2 );
			break;
		default:
			CG_Text_Paint_Ext( button->rect.x, button->rect.y + 11, scale, scale, clrTxtBck, "Allies", 0, 0, 0, &cgs.media.limboFont2 );
			break;
	}

	{
		float w;
		const char* str;

		for( i = 0; i <= SK_NUM_SKILLS; i++ ) {

			if( i == SK_NUM_SKILLS ) {
				xp = CG_TeamDebriefing_CalcXP( team, cgs.tdbSelectedMap - 1, -1 );
			}
			else {
				xp = CG_TeamDebriefing_CalcXP( team, cgs.tdbSelectedMap - 1, i );
			}

			str = va( "%i", xp );

			w = CG_Text_Width_Ext( str, scale, 0, &cgs.media.limboFont2 );

			CG_Text_Paint_Ext( button->rect.x + 100 + skillPositions[ i ] - (w*0.5f), button->rect.y + 11, scale, scale, clrTxtBck, str, 0, 0, 0, &cgs.media.limboFont2 );
		}
	}
}


// MAPVOTE
void CG_parseMapVoteListInfo()
{
	int i;

	cgs.dbNumMaps = (trap_Argc() - 2) / 4;

	if ( atoi(CG_Argv(1)) ) {
		cgs.dbMapMultiVote = qtrue;
	}

	for ( i = 0; i < cgs.dbNumMaps; i++ ) {
		Q_strncpyz(cgs.dbMaps[i], CG_Argv((i*4)+2), sizeof(cgs.dbMaps[0]));
		cgs.dbMapVotes[i] = 0;
		cgs.dbMapID[i] = atoi(CG_Argv((i*4)+3));
		cgs.dbMapLastPlayed[i] = atoi(CG_Argv((i*4)+4));
		cgs.dbMapTotalVotes[i] = atoi(CG_Argv((i*4)+5));
		if ( CG_FindArenaInfo(va("scripts/%s.arena", cgs.dbMaps[i]), cgs.dbMaps[i], &cgs.arenaData) ) {
			Q_strncpyz( cgs.dbMapDispName[i], cgs.arenaData.longname, sizeof(cgs.dbMaps[0]) );
		} else {
			Q_strncpyz( cgs.dbMapDispName[i], cgs.dbMaps[i], sizeof(cgs.dbMaps[0]) );
		}
	}

	CG_LocateArena();

	cgs.dbMapListReceived = qtrue;

	return;
}

void CG_parseMapVoteTally()
{
	int i, numMaps;

	numMaps = (trap_Argc() - 1);
	for ( i = 0; i < numMaps; i++ ) {
		cgs.dbMapVotes[i] = atoi(CG_Argv(i+1));
	}
	return;
}
// MAPVOTE END
