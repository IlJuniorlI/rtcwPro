/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (RTCW MP Source Code).

RTCW MP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW MP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW MP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW MP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW MP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/



// cg_servercmds.c -- reliably sequenced text commands sent by the server
// these are processed at snapshot transition time, so there will definately
// be a valid snapshot this frame

#include "cg_local.h"
#include "../ui/ui_shared.h" // bk001205 - for Q3_ui as well

void CG_StartShakeCamera( float param );                                // NERVE - SMF

/*
=================
CG_ParseScores

=================
*/
static void CG_ParseScores( void ) {
	int i, powerups;

	cg.numScores = atoi( CG_Argv( 1 ) );
	if ( cg.numScores > MAX_CLIENTS ) {
		cg.numScores = MAX_CLIENTS;
	}

	cg.teamScores[0] = atoi( CG_Argv( 2 ) );
	cg.teamScores[1] = atoi( CG_Argv( 3 ) );

	memset( cg.scores, 0, sizeof( cg.scores ) );
	for ( i = 0 ; i < cg.numScores ; i++ ) {
		//
		cg.scores[i].client = atoi( CG_Argv( i * 8 + 4 ) );
		cg.scores[i].score = atoi( CG_Argv( i * 8 + 5 ) );
		cg.scores[i].ping = atoi( CG_Argv( i * 8 + 6 ) );
		cg.scores[i].time = atoi( CG_Argv( i * 8 + 7 ) );
		cg.scores[i].scoreFlags = atoi( CG_Argv( i * 8 + 8 ) );
		powerups = atoi( CG_Argv( i * 8 + 9 ) );
		cg.scores[i].playerClass = atoi( CG_Argv( i * 8 + 10 ) );       // NERVE - SMF
		cg.scores[i].respawnsLeft = atoi( CG_Argv( i * 8 + 11 ) );      // NERVE - SMF
		/*cg.scores[i].isReady = atoi(CG_Argv(i * 9 + 12));*/

		if ( cg.scores[i].client < 0 || cg.scores[i].client >= MAX_CLIENTS ) {
			cg.scores[i].client = 0;
		}
		cgs.clientinfo[cg.scores[i].client].score = cg.scores[i].score;
		cgs.clientinfo[cg.scores[i].client].powerups = powerups;
		/*player_ready_status[cg.scores[i].client].isReady = cg.scores[i].isReady;*/

		cg.scores[i].team = cgs.clientinfo[cg.scores[i].client].team;
	}
#ifdef MISSIONPACK
	CG_SetScoreSelection( NULL );
#endif

}

#define TEAMINFOARGS 12 // number of arguments for CG_ParseTeamInfo

/*
=================
CG_ParseTeamInfo

=================
*/
static void CG_ParseTeamInfo( void ) {
	int i;
	int client;

	if (!cg.demoPlayback) {
		//Current rtcwpro tinfo

		int teamInfoPlayers = Q_atoi(CG_Argv(1));

		numSortedTeamPlayers = teamInfoPlayers;

		if (teamInfoPlayers < 0 || teamInfoPlayers >= MAX_CLIENTS)
		{
			CG_Printf("CG_ParseTeamInfo: teamInfoPlayers out of range (%i)\n", teamInfoPlayers);
			return;
		}

		for (i = 0; i < teamInfoPlayers; i++)
		{
			client = Q_atoi(CG_Argv(i * TEAMINFOARGS + 2));

			if (client < 0 || client >= MAX_CLIENTS)
			{
				CG_Printf("CG_ParseTeamInfo: bad client number: %i\n", client);
				return;
			}

			sortedTeamPlayers[i] = client;

			cgs.clientinfo[client].location = Q_atoi(CG_Argv(i * TEAMINFOARGS + 3));
			cgs.clientinfo[client].health = Q_atoi(CG_Argv(i * TEAMINFOARGS + 4));
			cgs.clientinfo[client].powerups = Q_atoi(CG_Argv(i * TEAMINFOARGS + 5));

			cg_entities[client].currentState.teamNum = Q_atoi(CG_Argv(i * TEAMINFOARGS + 6));

			cgs.clientinfo[client].playerAmmo = Q_atoi(CG_Argv(i * TEAMINFOARGS + 7));
			cgs.clientinfo[client].playerAmmoClip = Q_atoi(CG_Argv(i * TEAMINFOARGS + 8));
			cgs.clientinfo[client].playerNades = Q_atoi(CG_Argv(i * TEAMINFOARGS + 9));
			cgs.clientinfo[client].playerWeapon = Q_atoi(CG_Argv(i * TEAMINFOARGS + 10));
			cgs.clientinfo[client].playerLimbo = Q_atoi(CG_Argv(i * TEAMINFOARGS + 11));
			player_ready_status[client].isReady = Q_atoi(CG_Argv(i * TEAMINFOARGS + 12));
			cgs.clientinfo[client].latchedClass = Q_atoi(CG_Argv(i * TEAMINFOARGS + 13));

		}

		
	}
	else {
		//demo playback tinfos
		
		//OSP, bani
		if (!isRtcwPro && gameVersionFound) {

			// NERVE - SMF
			cg.identifyClientNum = atoi(CG_Argv(1));
			cg.identifyClientHealth = atoi(CG_Argv(2));
			// -NERVE - SMF

			numSortedTeamPlayers = atoi(CG_Argv(3));

			for (i = 0; i < numSortedTeamPlayers; i++) {
				client = atoi(CG_Argv(i * 5 + 4));

				sortedTeamPlayers[i] = client;

				cgs.clientinfo[client].location = atoi(CG_Argv(i * 5 + 5));
				cgs.clientinfo[client].health = atoi(CG_Argv(i * 5 + 6));
				cgs.clientinfo[client].powerups = atoi(CG_Argv(i * 5 + 7));

				cg_entities[client].currentState.teamNum = atoi(CG_Argv(i * 5 + 8));
			}
			return;
		}
		else {
			//1.1.2 to 1.2.8
			if (isRtcwProV128) {

				// NERVE - SMF
				cg.identifyClientNum = atoi(CG_Argv(1));
				cg.identifyClientHealth = atoi(CG_Argv(2));
				// -NERVE - SMF

				numSortedTeamPlayers = atoi(CG_Argv(3));

				for (i = 0; i < numSortedTeamPlayers; i++) {
					client = atoi(CG_Argv(i * 11 + 4));

					sortedTeamPlayers[i] = client;

					cgs.clientinfo[client].location = atoi(CG_Argv(i * 11 + 5));
					cgs.clientinfo[client].health = atoi(CG_Argv(i * 11 + 6));
					cgs.clientinfo[client].powerups = atoi(CG_Argv(i * 11 + 7));

					cg_entities[client].currentState.teamNum = atoi(CG_Argv(i * 11 + 8));

					cgs.clientinfo[client].playerAmmo = atoi(CG_Argv(i * 11 + 9));
					cgs.clientinfo[client].playerAmmoClip = atoi(CG_Argv(i * 11 + 10));
					cgs.clientinfo[client].playerNades = atoi(CG_Argv(i * 11 + 11));
					cgs.clientinfo[client].playerWeapon = atoi(CG_Argv(i * 11 + 12));
					cgs.clientinfo[client].playerLimbo = atoi(CG_Argv(i * 11 + 13));
					player_ready_status[client].isReady = atoi(CG_Argv(i * 11 + 14));
				}
				return;
			}
			if (isRtcwProV129) {

				// NERVE - SMF
				cg.identifyClientNum = atoi(CG_Argv(1));
				cg.identifyClientHealth = atoi(CG_Argv(2));
				// -NERVE - SMF

				numSortedTeamPlayers = atoi(CG_Argv(3));

				for (i = 0; i < numSortedTeamPlayers; i++) {
					client = atoi(CG_Argv(i * TEAMINFOARGS + 4));

					sortedTeamPlayers[i] = client;

					cgs.clientinfo[client].location = atoi(CG_Argv(i * TEAMINFOARGS + 5));
					cgs.clientinfo[client].health = atoi(CG_Argv(i * TEAMINFOARGS + 6));
					cgs.clientinfo[client].powerups = atoi(CG_Argv(i * TEAMINFOARGS + 7));

					cg_entities[client].currentState.teamNum = atoi(CG_Argv(i * TEAMINFOARGS + 8));

					cgs.clientinfo[client].playerAmmo = atoi(CG_Argv(i * TEAMINFOARGS + 9));
					cgs.clientinfo[client].playerAmmoClip = atoi(CG_Argv(i * TEAMINFOARGS + 10));
					cgs.clientinfo[client].playerNades = atoi(CG_Argv(i * TEAMINFOARGS + 11));
					cgs.clientinfo[client].playerWeapon = atoi(CG_Argv(i * TEAMINFOARGS + 12));
					cgs.clientinfo[client].playerLimbo = atoi(CG_Argv(i * TEAMINFOARGS + 13));
					player_ready_status[client].isReady = atoi(CG_Argv(i * TEAMINFOARGS + 14));
					cgs.clientinfo[client].latchedClass = atoi(CG_Argv(i * TEAMINFOARGS + 15));
				}
				return;
			}
			if (isRtcwProV130 || isRtcwProV140) {
				int teamInfoPlayers = Q_atoi(CG_Argv(1));

				numSortedTeamPlayers = teamInfoPlayers;

				if (teamInfoPlayers < 0 || teamInfoPlayers >= MAX_CLIENTS)
				{
					CG_Printf("CG_ParseTeamInfo: teamInfoPlayers out of range (%i)\n", teamInfoPlayers);
					return;
				}

				for (i = 0; i < teamInfoPlayers; i++)
				{
					client = Q_atoi(CG_Argv(i * TEAMINFOARGS + 2));

					if (client < 0 || client >= MAX_CLIENTS)
					{
						CG_Printf("CG_ParseTeamInfo: bad client number: %i\n", client);
						return;
					}

					sortedTeamPlayers[i] = client;

					cgs.clientinfo[client].location = Q_atoi(CG_Argv(i * TEAMINFOARGS + 3));
					cgs.clientinfo[client].health = Q_atoi(CG_Argv(i * TEAMINFOARGS + 4));
					cgs.clientinfo[client].powerups = Q_atoi(CG_Argv(i * TEAMINFOARGS + 5));

					cg_entities[client].currentState.teamNum = Q_atoi(CG_Argv(i * TEAMINFOARGS + 6));

					cgs.clientinfo[client].playerAmmo = Q_atoi(CG_Argv(i * TEAMINFOARGS + 7));
					cgs.clientinfo[client].playerAmmoClip = Q_atoi(CG_Argv(i * TEAMINFOARGS + 8));
					cgs.clientinfo[client].playerNades = Q_atoi(CG_Argv(i * TEAMINFOARGS + 9));
					cgs.clientinfo[client].playerWeapon = Q_atoi(CG_Argv(i * TEAMINFOARGS + 10));
					cgs.clientinfo[client].playerLimbo = Q_atoi(CG_Argv(i * TEAMINFOARGS + 11));
					player_ready_status[client].isReady = Q_atoi(CG_Argv(i * TEAMINFOARGS + 12));
					cgs.clientinfo[client].latchedClass = Q_atoi(CG_Argv(i * TEAMINFOARGS + 13));

				}
				return;
			}
		}
	}
}


/*
================
CG_ParseServerinfo

This is called explicitly when the gamestate is first received,
and whenever the server updates any serverinfo flagged cvars
================
*/
void CG_ParseServerinfo( void ) {
	const char  *info;
	char    *mapname;

	info = CG_ConfigString( CS_SERVERINFO );
	cgs.gametype = atoi( Info_ValueForKey( info, "g_gametype" ) );
	cgs.antilag = atoi( Info_ValueForKey( info, "g_antilag" ) );
	if ( !cgs.localServer ) {
		trap_Cvar_Set( "g_gametype", va( "%i", cgs.gametype ) );
		trap_Cvar_Set( "g_antilag", va( "%i", cgs.antilag ) );
	}
	cgs.dmflags = 0; //atoi( Info_ValueForKey( info, "dmflags" ) );				// NERVE - SMF - no longer serverinfo
	cgs.teamflags = 0; //atoi( Info_ValueForKey( info, "teamflags" ) );
	cgs.fraglimit = 0; //atoi( Info_ValueForKey( info, "fraglimit" ) );			// NERVE - SMF - no longer serverinfo
	cgs.capturelimit = 0; //atoi( Info_ValueForKey( info, "capturelimit" ) );	// NERVE - SMF - no longer serverinfo
	cgs.timelimit = atof( Info_ValueForKey( info, "timelimit" ) );
	cgs.maxclients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	mapname = Info_ValueForKey( info, "mapname" );
	Q_strncpyz(cgs.rawmapname, mapname, sizeof(cgs.rawmapname));
	Com_sprintf( cgs.mapname, sizeof( cgs.mapname ), "maps/%s.bsp", mapname );

// JPW NERVE
// prolly should parse all CS_SERVERINFO keys automagically, but I don't want to break anything that might be improperly set for wolf SP, so I'm just parsing MP relevant stuff here
	trap_Cvar_Set( "g_medicChargeTime",Info_ValueForKey( info,"g_medicChargeTime" ) );
	trap_Cvar_Set( "g_engineerChargeTime",Info_ValueForKey( info,"g_engineerChargeTime" ) );
	trap_Cvar_Set( "g_soldierChargeTime",Info_ValueForKey( info,"g_soldierChargeTime" ) );
	trap_Cvar_Set( "g_LTChargeTime",Info_ValueForKey( info,"g_LTChargeTime" ) );
	trap_Cvar_Set( "g_redlimbotime",Info_ValueForKey( info,"g_redlimbotime" ) );
	// DHM - TEMP FIX
	cg_redlimbotime.integer = atoi( Info_ValueForKey( info,"g_redlimbotime" ) );
	//trap_Cvar_Set("g_bluelimbotime",Info_ValueForKey(info,"g_bluelimbotime"));
	cg_bluelimbotime.integer = atoi( Info_ValueForKey( info,"g_bluelimbotime" ) );
// jpw

	cgs.minclients = atoi( Info_ValueForKey( info, "g_minGameClients" ) );       // NERVE - SMF

	// TTimo - make this available for ingame_callvote
	trap_Cvar_Set( "cg_ui_voteFlags", Info_ValueForKey( info, "g_voteFlags" ) );

	//grab the CVAR_SERVERINFO things out of the config string
	trap_Cvar_Set( "sv_fps", Info_ValueForKey( info, "sv_fps" ) );
	trap_Cvar_Set( "g_maxLagCompensation", Info_ValueForKey( info, "g_maxLagCompensation" ) );
	trap_Cvar_Set( "sv_minRestartDelay", Info_ValueForKey( info, "sv_minRestartDelay" ) );
	trap_Cvar_Set( "sv_minRestartPlayers", Info_ValueForKey( info, "sv_minRestartPlayers" ) );
	trap_Cvar_Set( "g_delagHitscan", Info_ValueForKey( info, "g_delagHitscan" ) );
	trap_Cvar_Set( "g_fixedphysicsfps", Info_ValueForKey( info, "g_fixedphysicsfps" ) );
	trap_Cvar_Set( "g_allowEnemySpawnTimer", Info_ValueForKey( info, "g_allowEnemySpawnTimer" ) );
	trap_Cvar_Set( "stats_matchid", Info_ValueForKey( info, "stats_matchid" ) );
	trap_Cvar_Set( "version", Info_ValueForKey( info, "version" ) );
}

/*
==================
CG_ParseWolfinfo

NERVE - SMF
==================
*/
void CG_ParseWolfinfo( void ) {
	const char  *info;
    int        old_gs = cgs.gamestate;
	info = CG_ConfigString( CS_WOLFINFO );

	cgs.currentRound = atoi( Info_ValueForKey( info, "g_currentRound" ) );
	cgs.nextTimeLimit = atof( Info_ValueForKey( info, "g_nextTimeLimit" ) );
	cgs.gamestate = atoi( Info_ValueForKey( info, "gamestate" ) );

	// Announce game in progress if we are really playing
	if (old_gs != GS_PLAYING && cgs.gamestate == GS_PLAYING)
	{
		trap_S_StartLocalSound(cgs.media.announceFight, CHAN_ANNOUNCER);

		CG_Printf("[skipnotify]^1FIGHT!\n");
		CPriP(CG_TranslateString("^1FIGHT!\n"));
	}

	if ( !cgs.localServer ) {
		trap_Cvar_Set( "gamestate", va( "%i", cgs.gamestate ) );
	}
}

/*
==================
CG_ParseWarmup
==================
*/
static void CG_ParseWarmup( void ) {
	const char  *info;
	int warmup;

	info = CG_ConfigString( CS_WARMUP );

	warmup = atoi( info );
	cg.warmupCount = -1;

	if ( warmup == 0 && cg.warmup ) {

	} else if ( warmup > 0 && cg.warmup <= 0 ) {
		trap_S_StartLocalSound( cgs.media.countPrepareSound, CHAN_ANNOUNCER );
		// OSPx - Auto Actions
		if (!cg.demoPlayback && cg_autoAction.integer & AA_DEMORECORD) {
			CG_autoRecord_f();
		}
	}

	cg.warmup = warmup;
}

/*
=====================
CG_ParseScreenFade
=====================
*/
static void CG_ParseScreenFade( void ) {
	const char  *info;
	char *token;

	info = CG_ConfigString( CS_SCREENFADE );

	token = COM_Parse( (char **)&info );
	cgs.fadeAlpha = atof( token );

	token = COM_Parse( (char **)&info );
	cgs.fadeStartTime = atoi( token );
	token = COM_Parse( (char **)&info );
	cgs.fadeDuration = atoi( token );

	if ( cgs.fadeStartTime + cgs.fadeDuration < cg.time ) {
		cgs.fadeAlphaCurrent = cgs.fadeAlpha;
	}
}


/*
==============
CG_ParseFog
	float near dist
	float far dist
	float density
	float[3] r,g,b
	int		time
==============
*/
static void CG_ParseFog( void ) {
	const char  *info;
	char *token;
	float ne, fa, r, g, b, density;
	int time;

	info = CG_ConfigString( CS_FOGVARS );

	token = COM_Parse( (char **)&info );    ne = atof( token );
	token = COM_Parse( (char **)&info );    fa = atof( token );
	token = COM_Parse( (char **)&info );    density = atof( token );
	token = COM_Parse( (char **)&info );    r = atof( token );
	token = COM_Parse( (char **)&info );    g = atof( token );
	token = COM_Parse( (char **)&info );    b = atof( token );
	token = COM_Parse( (char **)&info );    time = atoi( token );

	if ( fa ) {    // far of '0' from a target_fog means "return to map fog"
		trap_R_SetFog( FOG_SERVER, (int)ne, (int)fa, r, g, b, density + .1 );
		trap_R_SetFog( FOG_CMD_SWITCHFOG, FOG_SERVER, time, 0, 0, 0, 0 );
	} else {
		trap_R_SetFog( FOG_CMD_SWITCHFOG, FOG_MAP, time, 0, 0, 0, 0 );
	}
}

/*
================
L0 - Reinforcements offset

Parse reinforcements offset
================
*/
void CG_ParseReinforcementTimes( const char *pszReinfSeedString ) {
	const char *tmp = pszReinfSeedString, *tmp2;
	unsigned int i, j, dwDummy, dwOffset[TEAM_NUM_TEAMS];

#define GETVAL( x,y ) if ( ( tmp = strchr( tmp, ' ' ) ) == NULL ) {return;} x = atoi( ++tmp ) / y;

	dwOffset[TEAM_BLUE] = atoi( pszReinfSeedString ) >> REINF_BLUEDELT;
	GETVAL( dwOffset[TEAM_RED], ( 1 << REINF_REDDELT ) );
	tmp2 = tmp;

	for ( i = TEAM_RED; i <= TEAM_BLUE; i++ ) {
		tmp = tmp2;
		for ( j = 0; j < MAX_REINFSEEDS; j++ ) {
			if ( j == dwOffset[i] ) {
				GETVAL( cgs.aReinfOffset[i], aReinfSeeds[j] );
				cgs.aReinfOffset[i] *= 1000;
				break;
			}
			GETVAL( dwDummy, 1 );
		}
	}
}
/*
================
L0 - Pause

Parse Pause state
================
*/
void CG_ParsePause( const char *pTime ) {

	if (atoi(pTime) == 10000) {
		cgs.match_paused = PAUSE_RESUMING;
		cgs.match_resumes = 0;
		cgs.match_expired = 0;
	}
	else if (atoi(pTime) > 0) {
		cgs.match_paused = PAUSE_ON;
		cgs.match_resumes = atoi(pTime);
		cgs.match_expired = 0;
	}
	else {
		cgs.match_paused = PAUSE_NONE;
		cgs.match_resumes = 0;
		cgs.match_expired = 0;
	}
}

/*
================
L0 - Ready

Parse Ready state
================
*/
void CG_ParseReady(const char* pState) {
	cgs.readyState = atoi(pState);
}

/*
================
CG_SetConfigValues

Called on load to set the initial values from configure strings
================
*/
void CG_SetConfigValues( void ) {
	int pState = atoi(CG_ConfigString(CS_PAUSED));
	cgs.match_resumes = (pState > 0 ? pState : 0);

	if (pState && pState == 10000)
		cgs.match_paused = PAUSE_RESUMING;
	else if (pState && (pState > 0 && pState < 10000))
		cgs.match_paused = PAUSE_ON;
	else if (!pState || pState == 0)
		cgs.match_paused = PAUSE_NONE;

	cgs.scores1 = atoi( CG_ConfigString( CS_SCORES1 ) );
	cgs.scores2 = atoi( CG_ConfigString( CS_SCORES2 ) );
	cgs.levelStartTime = atoi( CG_ConfigString( CS_LEVEL_START_TIME ) );
	cg.warmup = atoi( CG_ConfigString( CS_WARMUP ) );

	// voting - Source ETL
	// set all of this crap in cgs - it won't be set if it doesn't
	// change, otherwise.  consider:
	// vote was called 5 minutes ago for 'Match Reset'.  you connect.
	// you're sent that value for CS_VOTE_STRING, but ignore it, so
	// you have nothing to use if another 'Match Reset' vote is called
	// (no update will be sent because the string will be the same.)
	cgs.voteTime = atoi(CG_ConfigString(CS_VOTE_TIME));
	cgs.voteYes = atoi(CG_ConfigString(CS_VOTE_YES));
	cgs.voteNo = atoi(CG_ConfigString(CS_VOTE_NO));
	Q_strncpyz(cgs.voteString, CG_ConfigString(CS_VOTE_STRING), sizeof(cgs.voteString));

	CG_ParseReinforcementTimes( CG_ConfigString( CS_REINFSEEDS ) );
	CG_ParseReady(CG_ConfigString(CS_READY) );
}

/*
=====================
CG_ShaderStateChanged
=====================
*/
void CG_ShaderStateChanged( void ) {
	char originalShader[MAX_QPATH];
	char newShader[MAX_QPATH];
	char timeOffset[16];
	const char *o;
	char *n,*t;

	o = CG_ConfigString( CS_SHADERSTATE );
	while ( o && *o ) {
		n = strstr( o, "=" );
		if ( n && *n ) {
			strncpy( originalShader, o, n - o );
			originalShader[n - o] = 0;
			n++;
			t = strstr( n, ":" );
			if ( t && *t ) {
				strncpy( newShader, n, t - n );
				newShader[t - n] = 0;
			} else {
				break;
			}
			t++;
			o = strstr( t, "@" );
			if ( o ) {
				strncpy( timeOffset, t, o - t );
				timeOffset[o - t] = 0;
				o++;
				trap_R_RemapShader( originalShader, newShader, timeOffset );
			}
		} else {
			break;
		}
	}
}

/*
================
CG_ConfigStringModified

================
*/
static void CG_ConfigStringModified( void ) {
	const char  *str;
	int num;

	num = atoi( CG_Argv( 1 ) );

	// get the gamestate from the client system, which will have the
	// new configstring already integrated
	trap_GetGameState( &cgs.gameState );

	// look up the individual string that was modified
	str = CG_ConfigString( num );

	// do something with it if necessary
	if ( num == CS_MUSIC ) {
		CG_StartMusic();
	} else if ( num == CS_SERVERINFO ) {
		CG_ParseServerinfo();
	} else if ( num == CS_WARMUP ) {
		CG_ParseWarmup();
	} else if ( num == CS_WOLFINFO ) {      // NERVE - SMF
		CG_ParseWolfinfo();
	} else if ( num == CS_SCORES1 ) {
		cgs.scores1 = atoi( str );
	} else if ( num == CS_SCORES2 ) {
		cgs.scores2 = atoi( str );
	} else if ( num == CS_LEVEL_START_TIME ) {
		cgs.levelStartTime = atoi( str );
	} else if ( num == CS_VOTE_TIME ) {
		cgs.voteTime = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_YES ) {
		cgs.voteYes = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_NO ) {
		cgs.voteNo = atoi( str );
		cgs.voteModified = qtrue;
	} else if ( num == CS_VOTE_STRING ) {
		Q_strncpyz( cgs.voteString, str, sizeof( cgs.voteString ) );
#if 0
		trap_S_StartLocalSound( cgs.media.voteNow, CHAN_ANNOUNCER );
	} else if ( num >= CS_TEAMVOTE_TIME && num <= CS_TEAMVOTE_TIME + 1 ) {
		cgs.teamVoteTime[num - CS_TEAMVOTE_TIME] = atoi( str );
		cgs.teamVoteModified[num - CS_TEAMVOTE_TIME] = qtrue;
	} else if ( num >= CS_TEAMVOTE_YES && num <= CS_TEAMVOTE_YES + 1 ) {
		cgs.teamVoteYes[num - CS_TEAMVOTE_YES] = atoi( str );
		cgs.teamVoteModified[num - CS_TEAMVOTE_YES] = qtrue;
	} else if ( num >= CS_TEAMVOTE_NO && num <= CS_TEAMVOTE_NO + 1 ) {
		cgs.teamVoteNo[num - CS_TEAMVOTE_NO] = atoi( str );
		cgs.teamVoteModified[num - CS_TEAMVOTE_NO] = qtrue;
	} else if ( num >= CS_TEAMVOTE_STRING && num <= CS_TEAMVOTE_STRING + 1 ) {
		Q_strncpyz( cgs.teamVoteString[num - CS_TEAMVOTE_STRING], str, sizeof( cgs.teamVoteString ) );
		trap_S_StartLocalSound( cgs.media.voteNow, CHAN_ANNOUNCER );
#endif
	} else if ( num == CS_INTERMISSION ) {
		cg.intermissionStarted = atoi( str );
	} else if ( num == CS_SCREENFADE ) {
		CG_ParseScreenFade();
	} else if ( num == CS_FOGVARS ) {
		CG_ParseFog();
	} else if ( num >= CS_MODELS && num < CS_MODELS + MAX_MODELS ) {
		cgs.gameModels[ num - CS_MODELS ] = trap_R_RegisterModel( str );
	} else if ( num >= CS_SOUNDS && num < CS_SOUNDS + MAX_MODELS ) {
		if ( str[0] != '*' ) {   // player specific sounds don't register here

			// Ridah, register sound scripts seperately
			if ( !strstr( str, ".wav" ) ) {
				CG_SoundScriptPrecache( str );
			} else {
				cgs.gameSounds[ num - CS_SOUNDS] = trap_S_RegisterSound( str );
			}

		}
	} else if ( num >= CS_PLAYERS && num < CS_PLAYERS + MAX_CLIENTS ) {
		CG_NewClientInfo( num - CS_PLAYERS );
	}
	// Rafael particle configstring
	else if ( num >= CS_PARTICLES && num < CS_PARTICLES + MAX_PARTICLES_AREAS ) {
		CG_NewParticleArea( num );
	}
//----(SA)	have not reached this code yet so I don't know if I really need this here
	else if ( num >= CS_DLIGHTS && num < CS_DLIGHTS + MAX_DLIGHTS ) {
//		CG_Printf(">>>>>>>>>>>got configstring for dlight: %d\ntell Sherman!!!!!!!!!!", num-CS_DLIGHTS);
//----(SA)
	} else if ( num == CS_SHADERSTATE )   {
		CG_ShaderStateChanged();
	}
	 // Reinforcmements offset
	else if ( num == CS_REINFSEEDS ) {
		CG_ParseReinforcementTimes( str );
	}  // Pause
	else if ( num == CS_PAUSED ) {
		CG_ParsePause( str );
	} // Ready
	else if ( num == CS_READY ) {
		CG_ParseReady( str );
	}
}


/*
=======================
CG_AddToTeamChat

=======================
*/
static void CG_AddToTeamChat( const char *str ) {
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight;

	if ( cg_teamChatHeight.integer < TEAMCHAT_HEIGHT ) {
		chatHeight = cg_teamChatHeight.integer;
	} else {
		chatHeight = TEAMCHAT_HEIGHT;
	}

	if ( chatHeight <= 0 || cg_teamChatTime.integer <= 0 ) {
		// team chat disabled, dump into normal chat
		cgs.teamChatPos = cgs.teamLastChatPos = 0;
		return;
	}

	len = 0;

	p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while ( *str ) {
		if ( len > TEAMCHAT_WIDTH - 1 ) {
			if ( ls ) {
				str -= ( p - ls );
				str++;
				p -= ( p - ls );
			}
			*p = 0;

			cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;

			cgs.teamChatPos++;
			p = cgs.teamChatMsgs[cgs.teamChatPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if ( *str == ' ' ) {
			ls = p;
		}
		*p++ = *str++;
		len++;
	}
	*p = 0;

	cgs.teamChatMsgTimes[cgs.teamChatPos % chatHeight] = cg.time;
	cgs.teamChatPos++;

	if ( cgs.teamChatPos - cgs.teamLastChatPos > chatHeight ) {
		cgs.teamLastChatPos = cgs.teamChatPos - chatHeight;
	}
}

/*
=======================
CG_AddToNotify

=======================
*/
void CG_AddToNotify( const char *str ) {
	int len;
	char *p, *ls;
	int lastcolor;
	int chatHeight;
	float notifytime;
	char var[MAX_TOKEN_CHARS];

	trap_Cvar_VariableStringBuffer( "con_notifytime", var, sizeof( var ) );
	notifytime = atof( var ) * 1000;

	trap_Cvar_VariableStringBuffer("cg_notifyTextLines", var, sizeof(var));
	chatHeight = atoi( var );
	if (chatHeight > MAX_NOTIFY_HEIGHT) {
		chatHeight = MAX_NOTIFY_HEIGHT;
	}

	if (cg.demoPlayback) {
		trap_Cvar_VariableStringBuffer("cg_notifyPlayerOnly", var, sizeof(var));
		int notifyPlayerOnly = atoi(var);
		if (notifyPlayerOnly) {
			char* playerName = Info_ValueForKey(CG_ConfigString(CS_PLAYERS + cg.snap->ps.clientNum), "n");
			if (strlen(playerName)) {
				if (strstr(str, playerName) == NULL) {
					return;
				}
			}
		}
	}


	if ( chatHeight <= 0 || notifytime <= 0 ) {
		// team chat disabled, dump into normal chat
		cgs.notifyPos = cgs.notifyLastPos = 0;
		return;
	}

	len = 0;

	p = cgs.notifyMsgs[cgs.notifyPos % chatHeight];
	*p = 0;

	lastcolor = '7';

	ls = NULL;
	while ( *str ) {
		if ( len > NOTIFY_WIDTH - 1 || ( *str == '\n' && ( *( str + 1 ) != 0 ) ) ) {
			if ( ls ) {
				str -= ( p - ls );
				str++;
				p -= ( p - ls );
			}
			*p = 0;

			cgs.notifyMsgTimes[cgs.notifyPos % chatHeight] = cg.time;

			cgs.notifyPos++;
			p = cgs.notifyMsgs[cgs.notifyPos % chatHeight];
			*p = 0;
			*p++ = Q_COLOR_ESCAPE;
			*p++ = lastcolor;
			len = 0;
			ls = NULL;
		}

		if ( Q_IsColorString( str ) ) {
			*p++ = *str++;
			lastcolor = *str;
			*p++ = *str++;
			continue;
		}
		if ( *str == ' ' ) {
			ls = p;
		}
		while ( *str == '\n' ) {
			// TTimo gcc warning: value computed is not used
			// was *str++;
			str++;
		}

		if ( *str ) {
			*p++ = *str++;
			len++;
		}
	}
	*p = 0;

	cgs.notifyMsgTimes[cgs.notifyPos % chatHeight] = cg.time;
	cgs.notifyPos++;

	if ( cgs.notifyPos - cgs.notifyLastPos > chatHeight ) {
		cgs.notifyLastPos = cgs.notifyPos - chatHeight;
	}
}

/*
===============
CG_SendMoveSpeed
===============
*/
void CG_SendMoveSpeed( animation_t *animList, int numAnims, char *modelName ) {
	animation_t *anim;
	int i;
	char text[10000];

	if ( !cgs.localServer ) {
		return;
	}

	text[0] = 0;
	Q_strcat( text, sizeof( text ), modelName );

	for ( i = 0, anim = animList; i < numAnims; i++, anim++ ) {
		if ( anim->moveSpeed <= 0 ) {
			continue;
		}

		// add this to the list
		Q_strcat( text, sizeof( text ), va( " %s %i", anim->name, anim->moveSpeed ) );
	}

	// send the movespeeds to the server
	trap_SendMoveSpeedsToGame( 0, text );
}

/*
===============
CG_SendMoveSpeeds

  send moveSpeeds for all unique models
===============
*/
void CG_SendMoveSpeeds( void ) {
	int i;
	animModelInfo_t *modelInfo;

	for ( i = 0, modelInfo = cgs.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, modelInfo++ ) {
		if ( !modelInfo->modelname[0] ) {
			continue;
		}

		// send this model
		CG_SendMoveSpeed( modelInfo->animations, modelInfo->numAnimations, modelInfo->modelname );
	}

}


/*
===============
CG_MapRestart

The server has issued a map_restart, so the next snapshot
is completely new and should not be interpolated to.

A tournement restart will clear everything, but doesn't
require a reload of all the media
===============
*/
static void CG_MapRestart( void ) {
	int i;
	if ( cg_showmiss.integer ) {
		CG_Printf( "CG_MapRestart\n" );
	}

	memset( &cg.lastWeapSelInBank[0], 0, MAX_WEAP_BANKS * sizeof( int ) );  // clear weapon bank selections

	cg.centerPrintTime = 0; // reset centerprint counter so previous messages don't re-appear
	cg.itemPickupTime = 0;  // reset item pickup counter so previous messages don't re-appear
	cg.cursorHintFade = 0;  // reset cursor hint timer
	cg.popinPrintTime = 0;	// OSPx - reset pop in prints..

	// DHM - Nerve :: Reset complaint system
	cgs.complaintClient = -1;
	cgs.complaintEndTime = 0;

	// (SA) clear zoom (so no warpies)
	cg.zoomedBinoc = qfalse;
	cg.zoomedBinoc = cg.zoomedScope = qfalse;
	cg.zoomTime = 0;
	cg.zoomval = 0;
	// OSPx - Reset ZoomedFOV
	cg.zoomedFOV = qfalse;
	cg.zoomedTime = 0;

	// reset fog to world fog (if present)
	trap_R_SetFog( FOG_CMD_SWITCHFOG, FOG_MAP,20,0,0,0,0 );

	CG_InitLocalEntities();
	CG_InitMarkPolys();

	//Rafael particles
	CG_ClearParticles();
	// done.

	for ( i = 1; i < MAX_PARTICLES_AREAS; i++ )
	{
		{
			int rval;

			rval = CG_NewParticleArea( CS_PARTICLES + i );
			if ( !rval ) {
				break;
			}
		}
	}


	// Ridah, trails
//	CG_ClearTrails ();
	// done.

	// Ridah
	CG_ClearFlameChunks();
	CG_SoundInit();
	// done.

	// make sure the "3 frags left" warnings play again
	cg.fraglimitWarnings = 0;

	cg.timelimitWarnings = 0;

	cg.intermissionStarted = qfalse;

	cgs.voteTime = 0;

	cg.lightstylesInited    = qfalse;

	cg.mapRestart = qtrue;
	cgs.dumpStatsTime = 0;	// L0 - OSP Stats

	CG_StartMusic();

	trap_S_ClearLoopingSounds( qtrue );

	cg.latchAutoActions = qfalse;			// OSPx - Auto Actions

// JPW NERVE -- reset render flags
	cg_fxflags = 0;
// jpw


	// we really should clear more parts of cg here and stop sounds
	cg.v_dmg_time = 0;
	cg.v_noFireTime = 0;
	cg.v_fireTime = 0;
	// L0 - Pause
	// If resetted while paused clear the screen.
	cgs.fadeAlpha = 0;

	// inform LOCAL server of animationSpeeds for AI use (ONLY)
	//if (cgs.localServer) {
	//CG_SendMoveSpeeds();
	//}

	// play the "fight" sound if this is a restart without warmup
	if ( cg.warmup == 0 && cgs.gametype == GT_TOURNAMENT ) {
		trap_S_StartLocalSound( cgs.media.countFightSound, CHAN_ANNOUNCER );
		CG_CenterPrint( "FIGHT!", 120, GIANTCHAR_WIDTH * 2 );
	}
#ifdef MISSIONPACK
	if ( cg_singlePlayerActive.integer ) {
		trap_Cvar_Set( "ui_matchStartTime", va( "%i", cg.time ) );
		if ( cg_recordSPDemo.integer && cg_recordSPDemoName.string && *cg_recordSPDemoName.string ) {
			trap_SendConsoleCommand( va( "set g_synchronousclients 1 ; record %s \n", cg_recordSPDemoName.string ) );
		}
	}
#endif

	trap_Cvar_Set( "cg_thirdPerson", "0" );
	// RTCWPro
	trap_Cvar_Set("cg_spawnTimer_set", "-1");
	trap_Cvar_Set("cg_spawnTimer_period", "0");
}

/*
=================
CG_RequestMoveSpeed
=================
*/
void CG_RequestMoveSpeed( const char *modelname ) {
	animModelInfo_t *modelInfo;

	modelInfo = BG_ModelInfoForModelname( (char *)modelname );

	if ( !modelInfo ) {
		// ignore it
		return;
	}

	// send it
	CG_SendMoveSpeed( modelInfo->animations, modelInfo->numAnimations, (char *)modelname );
}

// NERVE - SMF
#define MAX_VOICEFILESIZE   16384
#define MAX_VOICEFILES      8
#define MAX_VOICECHATS      64
#define MAX_VOICESOUNDS     64
#define MAX_CHATSIZE        64
#define MAX_HEADMODELS      64

typedef struct voiceChat_s
{
	char id[64];
	int numSounds;
	sfxHandle_t sounds[MAX_VOICESOUNDS];
	char chats[MAX_VOICESOUNDS][MAX_CHATSIZE];
	qhandle_t sprite[MAX_VOICESOUNDS];          // DHM - Nerve
} voiceChat_t;

typedef struct voiceChatList_s
{
	char name[64];
	int gender;
	int numVoiceChats;
	voiceChat_t voiceChats[MAX_VOICECHATS];
} voiceChatList_t;

typedef struct headModelVoiceChat_s
{
	char headmodel[64];
	int voiceChatNum;
} headModelVoiceChat_t;

voiceChatList_t voiceChatLists[MAX_VOICEFILES];
headModelVoiceChat_t headModelVoiceChat[MAX_HEADMODELS];

/*
=================
CG_ParseVoiceChats
=================
*/
int CG_ParseVoiceChats( const char *filename, voiceChatList_t *voiceChatList, int maxVoiceChats ) {
	int len, i;
	int current = 0;
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;
	voiceChat_t *voiceChats;
	qboolean compress;

	compress = qtrue;
	if ( cg_buildScript.integer ) {
		compress = qfalse;
	}

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "voice chat file not found: %s\n", filename ) );
		return qfalse;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return qfalse;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	Com_sprintf( voiceChatList->name, sizeof( voiceChatList->name ), "%s", filename );
	voiceChats = voiceChatList->voiceChats;
	for ( i = 0; i < maxVoiceChats; i++ ) {
		voiceChats[i].id[0] = 0;
	}
	token = COM_ParseExt( p, qtrue );
	if ( !token || token[0] == 0 ) {
		return qtrue;
	}
	if ( !Q_stricmp( token, "female" ) ) {
		voiceChatList->gender = GENDER_FEMALE;
	} else if ( !Q_stricmp( token, "male" ) )        {
		voiceChatList->gender = GENDER_MALE;
	} else if ( !Q_stricmp( token, "neuter" ) )        {
		voiceChatList->gender = GENDER_NEUTER;
	} else {
		trap_Print( va( S_COLOR_RED "expected gender not found in voice chat file: %s\n", filename ) );
		return qfalse;
	}

	voiceChatList->numVoiceChats = 0;
	while ( 1 ) {
		token = COM_ParseExt( p, qtrue );
		if ( !token || token[0] == 0 ) {
			return qtrue;
		}
		Com_sprintf( voiceChats[voiceChatList->numVoiceChats].id, sizeof( voiceChats[voiceChatList->numVoiceChats].id ), "%s", token );
		token = COM_ParseExt( p, qtrue );
		if ( Q_stricmp( token, "{" ) ) {
			trap_Print( va( S_COLOR_RED "expected { found %s in voice chat file: %s\n", token, filename ) );
			return qfalse;
		}
		voiceChats[voiceChatList->numVoiceChats].numSounds = 0;
		current = voiceChats[voiceChatList->numVoiceChats].numSounds;

		while ( 1 ) {
			token = COM_ParseExt( p, qtrue );
			if ( !token || token[0] == 0 ) {
				return qtrue;
			}
			if ( !Q_stricmp( token, "}" ) ) {
				break;
			}
			voiceChats[voiceChatList->numVoiceChats].sounds[current] = trap_S_RegisterSound( token /*, compress */ );
			token = COM_ParseExt( p, qtrue );
			if ( !token || token[0] == 0 ) {
				return qtrue;
			}
			Com_sprintf( voiceChats[voiceChatList->numVoiceChats].chats[current], MAX_CHATSIZE, "%s", token );

			// DHM - Nerve :: Specify sprite shader to show above player's head
			token = COM_ParseExt( p, qfalse );
			if ( !Q_stricmp( token, "}" ) || !token || token[0] == 0 ) {
				voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( "sprites/voiceChat" );
				COM_RestoreParseSession( p );
			} else {
				voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( token );
				if ( voiceChats[voiceChatList->numVoiceChats].sprite[current] == 0 ) {
					voiceChats[voiceChatList->numVoiceChats].sprite[current] = trap_R_RegisterShader( "sprites/voiceChat" );
				}
			}
			// dhm - end

			voiceChats[voiceChatList->numVoiceChats].numSounds++;
			current = voiceChats[voiceChatList->numVoiceChats].numSounds;

			if ( voiceChats[voiceChatList->numVoiceChats].numSounds >= MAX_VOICESOUNDS ) {
				break;
			}
		}

		voiceChatList->numVoiceChats++;
		if ( voiceChatList->numVoiceChats >= maxVoiceChats ) {
			return qtrue;
		}
	}
	return qtrue;
}

/*
=================
CG_LoadVoiceChats
=================
*/
void CG_LoadVoiceChats( void ) {
	int size;

	size = trap_MemoryRemaining();
	CG_ParseVoiceChats( "scripts/wm_axis_chat.voice", &voiceChatLists[0], MAX_VOICECHATS );
	CG_ParseVoiceChats( "scripts/wm_allies_chat.voice", &voiceChatLists[1], MAX_VOICECHATS );
//	CG_ParseVoiceChats( "scripts/female2.voice", &voiceChatLists[1], MAX_VOICECHATS );
//	CG_ParseVoiceChats( "scripts/female3.voice", &voiceChatLists[2], MAX_VOICECHATS );
//	CG_ParseVoiceChats( "scripts/male1.voice", &voiceChatLists[3], MAX_VOICECHATS );
//	CG_ParseVoiceChats( "scripts/male2.voice", &voiceChatLists[4], MAX_VOICECHATS );
//	CG_ParseVoiceChats( "scripts/male3.voice", &voiceChatLists[5], MAX_VOICECHATS );
//	CG_ParseVoiceChats( "scripts/male4.voice", &voiceChatLists[6], MAX_VOICECHATS );
//	CG_ParseVoiceChats( "scripts/male5.voice", &voiceChatLists[7], MAX_VOICECHATS );
	CG_Printf( "voice chat memory size = %d\n", size - trap_MemoryRemaining() );
}

/*
=================
CG_HeadModelVoiceChats
=================
*/
int CG_HeadModelVoiceChats( char *filename ) {
	int len, i;
	fileHandle_t f;
	char buf[MAX_VOICEFILESIZE];
	char **p, *ptr;
	char *token;

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( "voice chat file not found: %s\n", filename ) );
		return -1;
	}
	if ( len >= MAX_VOICEFILESIZE ) {
		trap_Print( va( S_COLOR_RED "voice chat file too large: %s is %i, max allowed is %i", filename, len, MAX_VOICEFILESIZE ) );
		trap_FS_FCloseFile( f );
		return -1;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	ptr = buf;
	p = &ptr;

	token = COM_ParseExt( p, qtrue );
	if ( !token || token[0] == 0 ) {
		return -1;
	}

	for ( i = 0; i < MAX_VOICEFILES; i++ ) {
		if ( !Q_stricmp( token, voiceChatLists[i].name ) ) {
			return i;
		}
	}

	//FIXME: maybe try to load the .voice file which name is stored in token?

	return -1;
}


/*
=================
CG_GetVoiceChat
=================
*/
int CG_GetVoiceChat( voiceChatList_t *voiceChatList, const char *id, sfxHandle_t *snd, qhandle_t *sprite, char **chat ) {
	int i, rnd;

	for ( i = 0; i < voiceChatList->numVoiceChats; i++ ) {
		if ( !Q_stricmp( id, voiceChatList->voiceChats[i].id ) ) {
			rnd = random() * voiceChatList->voiceChats[i].numSounds;
			*snd = voiceChatList->voiceChats[i].sounds[rnd];
			*sprite = voiceChatList->voiceChats[i].sprite[rnd];
			*chat = voiceChatList->voiceChats[i].chats[rnd];
			return qtrue;
		}
	}
	return qfalse;
}

/*
=================
CG_VoiceChatListForClient
=================
*/
voiceChatList_t *CG_VoiceChatListForClient( int clientNum ) {
	clientInfo_t *ci;
	int voiceChatNum, i, j, k, gender;
	char filename[128], *headModelName;

	// NERVE - SMF
	if ( cgs.gametype >= GT_WOLF ) {
		if ( cgs.clientinfo[ clientNum ].team == TEAM_RED ) {
			return &voiceChatLists[0];
		} else {
			return &voiceChatLists[1];
		}
	}
	// -NERVE - SMF

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	headModelName = ci->headModelName;
	if ( headModelName[0] == '*' ) {
		headModelName++;
	}
	// find the voice file for the head model the client uses
	for ( i = 0; i < MAX_HEADMODELS; i++ ) {
		if ( !Q_stricmp( headModelVoiceChat[i].headmodel, headModelName ) ) {
			break;
		}
	}
	if ( i < MAX_HEADMODELS ) {
		return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
	}
	// find a <headmodelname>.vc file
	for ( i = 0; i < MAX_HEADMODELS; i++ ) {
		if ( !strlen( headModelVoiceChat[i].headmodel ) ) {
			Com_sprintf( filename, sizeof( filename ), "scripts/%s.vc", headModelName );
			voiceChatNum = CG_HeadModelVoiceChats( filename );
			if ( voiceChatNum == -1 ) {
				break;
			}
			Com_sprintf( headModelVoiceChat[i].headmodel, sizeof( headModelVoiceChat[i].headmodel ),
						 "%s", headModelName );
			headModelVoiceChat[i].voiceChatNum = voiceChatNum;
			return &voiceChatLists[headModelVoiceChat[i].voiceChatNum];
		}
	}
	gender = ci->gender;
	for ( k = 0; k < 2; k++ ) {
		// just pick the first with the right gender
		for ( i = 0; i < MAX_VOICEFILES; i++ ) {
			if ( strlen( voiceChatLists[i].name ) ) {
				if ( voiceChatLists[i].gender == gender ) {
					// store this head model with voice chat for future reference
					for ( j = 0; j < MAX_HEADMODELS; j++ ) {
						if ( !strlen( headModelVoiceChat[j].headmodel ) ) {
							Com_sprintf( headModelVoiceChat[j].headmodel, sizeof( headModelVoiceChat[j].headmodel ),
										 "%s", headModelName );
							headModelVoiceChat[j].voiceChatNum = i;
							break;
						}
					}
					return &voiceChatLists[i];
				}
			}
		}
		// fall back to male gender because we don't have neuter in the mission pack
		if ( gender == GENDER_MALE ) {
			break;
		}
		gender = GENDER_MALE;
	}
	// store this head model with voice chat for future reference
	for ( j = 0; j < MAX_HEADMODELS; j++ ) {
		if ( !strlen( headModelVoiceChat[j].headmodel ) ) {
			Com_sprintf( headModelVoiceChat[j].headmodel, sizeof( headModelVoiceChat[j].headmodel ),
						 "%s", headModelName );
			headModelVoiceChat[j].voiceChatNum = 0;
			break;
		}
	}
	// just return the first voice chat list
	return &voiceChatLists[0];
}

#define MAX_VOICECHATBUFFER     32

typedef struct bufferedVoiceChat_s
{
	int clientNum;
	sfxHandle_t snd;
	qhandle_t sprite;
	int voiceOnly;
	char cmd[MAX_SAY_TEXT];
	char message[MAX_SAY_TEXT];
	vec3_t origin;          // NERVE - SMF
} bufferedVoiceChat_t;

bufferedVoiceChat_t voiceChatBuffer[MAX_VOICECHATBUFFER];

/*
=================
CG_PlayVoiceChat
=================
*/
void CG_PlayVoiceChat( bufferedVoiceChat_t *vchat ) {
	// if we are going into the intermission, don't start any voices
/*	// NERVE - SMF - don't do this in wolfMP
	if ( cg.intermissionStarted ) {
		return;
	}
*/
	// OSPx - Not in demo if disabled
	if (cg.demoPlayback && cgs.noVoice) {
		return;
	}
 // nihi moved outside of novoicechats
	// DHM - Nerve :: Show icon above head
		if ( vchat->clientNum == cg.snap->ps.clientNum ) {
			cg.predictedPlayerEntity.voiceChatSprite = vchat->sprite;
			if ( vchat->sprite == cgs.media.voiceChatShader ) {
				cg.predictedPlayerEntity.voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
			} else {
				cg.predictedPlayerEntity.voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer * 2;
			}
		} else {
			cg_entities[ vchat->clientNum ].voiceChatSprite = vchat->sprite;
			VectorCopy( vchat->origin, cg_entities[ vchat->clientNum ].lerpOrigin );            // NERVE - SMF
			if ( vchat->sprite == cgs.media.voiceChatShader ) {
				cg_entities[ vchat->clientNum ].voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer;
			} else {
				cg_entities[ vchat->clientNum ].voiceChatSpriteTime = cg.time + cg_voiceSpriteTime.integer * 2;
			}
		}
		//end nihi
	if ( !cg_noVoiceChats.integer ) {
		trap_S_StartLocalSound( vchat->snd, CHAN_VOICE );

	  // NIHI MOVED THE ABOVE SECTION FROM HERE
		// dhm - end

#ifdef MISSIONPACK
		if ( vchat->clientNum != cg.snap->ps.clientNum ) {
			int orderTask = CG_ValidOrder( vchat->cmd );
			if ( orderTask > 0 ) {
				cgs.acceptOrderTime = cg.time + 5000;
				Q_strncpyz( cgs.acceptVoice, vchat->cmd, sizeof( cgs.acceptVoice ) );
				cgs.acceptTask = orderTask;
				cgs.acceptLeader = vchat->clientNum;
			}
			// see if this was an order
			CG_ShowResponseHead();
		}
#endif
	}
	if ( !vchat->voiceOnly && !cg_noVoiceText.integer || (cg.demoPlayback && !cgs.noVoice) ) {
		CG_AddToTeamChat( vchat->message );
		CG_Printf( va( "[skipnotify]: %s\n", vchat->message ) ); // JPW NERVE
	}
	voiceChatBuffer[cg.voiceChatBufferOut].snd = 0;
}

/*
=====================
CG_PlayBufferedVoieChats
=====================
*/
void CG_PlayBufferedVoiceChats( void ) {
	if ( cg.voiceChatTime < cg.time ) {
		if ( cg.voiceChatBufferOut != cg.voiceChatBufferIn && voiceChatBuffer[cg.voiceChatBufferOut].snd ) {
			//
			CG_PlayVoiceChat( &voiceChatBuffer[cg.voiceChatBufferOut] );
			//
			cg.voiceChatBufferOut = ( cg.voiceChatBufferOut + 1 ) % MAX_VOICECHATBUFFER;
			cg.voiceChatTime = cg.time + 1000;
		}
	}
}

/*
=====================
CG_AddBufferedVoiceChat
=====================
*/
void CG_AddBufferedVoiceChat( bufferedVoiceChat_t *vchat ) {
	// if we are going into the intermission, don't start any voices
/*	// NERVE - SMF - don't do this in wolfMP
	if ( cg.intermissionStarted ) {
		return;
	}
*/

// JPW NERVE new system doesn't buffer but overwrites vchats FIXME put this on a cvar to choose which to use
	memcpy( &voiceChatBuffer[0],vchat,sizeof( bufferedVoiceChat_t ) );
	cg.voiceChatBufferIn = 0;
	CG_PlayVoiceChat( &voiceChatBuffer[0] );

/* JPW NERVE pulled this
	memcpy(&voiceChatBuffer[cg.voiceChatBufferIn], vchat, sizeof(bufferedVoiceChat_t));
	cg.voiceChatBufferIn = (cg.voiceChatBufferIn + 1) % MAX_VOICECHATBUFFER;
	if (cg.voiceChatBufferIn == cg.voiceChatBufferOut) {
		CG_PlayVoiceChat( &voiceChatBuffer[cg.voiceChatBufferOut] );
		cg.voiceChatBufferOut++;
	}
*/
}

/*
=================
CG_VoiceChatLocal
=================
*/
void CG_VoiceChatLocal( int mode, qboolean voiceOnly, int clientNum, int color, const char *cmd, vec3_t origin ) {
	char *chat;
	voiceChatList_t *voiceChatList;
	clientInfo_t *ci;
	sfxHandle_t snd;
	qhandle_t sprite;
	bufferedVoiceChat_t vchat;
	const char *loc;            // NERVE - SMF

/*	// NERVE - SMF - don't do this in wolfMP
	// if we are going into the intermission, don't start any voices
	if ( cg.intermissionStarted ) {
		return;
	}
*/

	if ( clientNum < 0 || clientNum >= MAX_CLIENTS ) {
		clientNum = 0;
	}
	ci = &cgs.clientinfo[ clientNum ];

	cgs.currentVoiceClient = clientNum;

	voiceChatList = CG_VoiceChatListForClient( clientNum );

	if ( CG_GetVoiceChat( voiceChatList, cmd, &snd, &sprite, &chat ) ) {
		//
		if ( mode == SAY_TEAM || !cg_teamChatsOnly.integer ) {
			vchat.clientNum = clientNum;
			vchat.snd = snd;
			vchat.sprite = sprite;
			vchat.voiceOnly = voiceOnly;
			VectorCopy( origin, vchat.origin );     // NERVE - SMF
			Q_strncpyz( vchat.cmd, cmd, sizeof( vchat.cmd ) );

			// NERVE - SMF - get location
			loc = CG_ConfigString( CS_LOCATIONS + ci->location );
			if ( !loc || !*loc ) {
				loc = " ";
			}
			// -NERVE - SMF

			if ( mode == SAY_TELL ) {
				Com_sprintf( vchat.message, sizeof( vchat.message ), "[%s]%c%c[%s]: %c%c%s",
							 ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, CG_TranslateString( loc ), Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			} else if ( mode == SAY_TEAM )   {
				Com_sprintf( vchat.message, sizeof( vchat.message ), "(%s)%c%c(%s): %c%c%s",
							 ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, CG_TranslateString( loc ), Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			} else {
				Com_sprintf( vchat.message, sizeof( vchat.message ), "%s %c%c(%s): %c%c%s",
							 ci->name, Q_COLOR_ESCAPE, COLOR_YELLOW, CG_TranslateString( loc ), Q_COLOR_ESCAPE, color, CG_TranslateString( chat ) );
			}
			CG_AddBufferedVoiceChat( &vchat );
		}
	}
}

/*
=================
CG_VoiceChat
=================
*/
void CG_VoiceChat( int mode ) {
	const char *cmd;
	int clientNum, color;
	qboolean voiceOnly;
	vec3_t origin;          // NERVE - SMF

	voiceOnly = atoi( CG_Argv( 1 ) );
	clientNum = atoi( CG_Argv( 2 ) );
	color = atoi( CG_Argv( 3 ) );

	// NERVE - SMF - added origin
	origin[0] = atoi( CG_Argv( 5 ) );
	origin[1] = atoi( CG_Argv( 6 ) );
	origin[2] = atoi( CG_Argv( 7 ) );

	cmd = CG_Argv( 4 );

	if ( cg_noTaunt.integer != 0 ) {
		if ( !strcmp( cmd, VOICECHAT_KILLINSULT )  || !strcmp( cmd, VOICECHAT_TAUNT ) || \
			 !strcmp( cmd, VOICECHAT_DEATHINSULT ) || !strcmp( cmd, VOICECHAT_KILLGAUNTLET ) ||	\
			 !strcmp( cmd, VOICECHAT_PRAISE ) ) {
			return;
		}
	}

	CG_VoiceChatLocal( mode, voiceOnly, clientNum, color, cmd, origin );
}
// -NERVE - SMF

/*
=================
CG_RemoveChatEscapeChar
=================
*/
static void CG_RemoveChatEscapeChar( char *text ) {
	int i, l;

	l = 0;
	for ( i = 0; text[i]; i++ ) {
		if ( text[i] == '\x19' ) {
			continue;
		}
		text[l++] = text[i];
	}
	text[l] = '\0';
}

/*
=================
CG_LocalizeServerCommand

NERVE - SMF - localize string sent from server

- localization is ON by default.
- use [lof] in string to turn OFF
- use [lon] in string to turn back ON
=================
*/
const char* CG_LocalizeServerCommand( const char *buf ) {
	static char token[MAX_TOKEN_CHARS];
	char temp[MAX_TOKEN_CHARS];
	qboolean togloc = qtrue;
	const char *s;
	int i, prev;

	memset( token, 0, sizeof( token ) );
	s = buf;
	prev = 0;

	for ( i = 0; *s; i++, s++ ) {
		// TTimo:
		// line was: if ( *s == '[' && !Q_strncmp( s, "[lon]", 5 ) || !Q_strncmp( s, "[lof]", 5 ) ) {
		// || prevails on &&, gcc warning was 'suggest parentheses around && within ||'
		// modified to the correct behaviour
		if ( *s == '[' && ( !Q_strncmp( s, "[lon]", 5 ) || !Q_strncmp( s, "[lof]", 5 ) ) ) {

			if ( togloc ) {
				memset( temp, 0, sizeof( temp ) );
				strncpy( temp, buf + prev, i - prev );
				strcat( token, CG_TranslateString( temp ) );
			} else {
				strncat( token, buf + prev, i - prev );
			}

			if ( s[3] == 'n' ) {
				togloc = qtrue;
			} else {
				togloc = qfalse;
			}

			i += 5;
			s += 5;
			prev = i;
		}
	}

	if ( togloc ) {
		memset( temp, 0, sizeof( temp ) );
		strncpy( temp, buf + prev, i - prev );
		strcat( token, CG_TranslateString( temp ) );
	} else {
		strncat( token, buf + prev, i - prev );
	}

	return token;
}
// -NERVE - SMF


/*
=================
L0

Large OSP (ET port) Stats dump bellow
NOTE: It's modifed to suit wolfX..
NOTE: My changes aren't commented really.
=================
*/
///////////////////
// Window Prints //
///////////////////

// +wstats
void CG_parseWeaponStats_cmd( void( txt_dump ) ( char * ) ) {
	clientInfo_t *ci;

	qboolean fFull = ( txt_dump != CG_printWindow );
//	qboolean fFull = qfalse;
	qboolean fHasStats = qfalse;
	char strName[MAX_STRING_CHARS];
	int atts, deaths, dmg_given, dmg_rcvd, hits, kills, team_dmg, headshots, gibs;
	unsigned int i, iArg = 1;
	unsigned int nClient = atoi( CG_Argv( iArg++ ) );
	unsigned int nRounds = atoi( CG_Argv( iArg++ ) );
	unsigned int dwWeaponMask = atoi( CG_Argv( iArg++ ) );

	ci = &cgs.clientinfo[nClient];

	Q_strncpyz( strName, ci->name, sizeof( strName ) );
	txt_dump( va( "^7Overall stats for: ^z%s ^7(^2%d^7 Round%s)\n\n", strName, nRounds, ( ( nRounds != 1 ) ? "s" : "" ) ) );

	if ( fFull ) {
		txt_dump(     "Weapon     Acrcy Hits/Atts Kills Deaths Headshots\n" );
		txt_dump(     "-------------------------------------------------\n" );
	} else {
		txt_dump(     "Weapon     Acrcy Hits/Atts Kll Dth HS\n" );
		txt_dump(     "\n" );
	}

	if ( !dwWeaponMask ) {
		txt_dump( "^zNo weapon info available.\n" );
	} else {
		for ( i = WS_KNIFE; i < WS_MAX; i++ ) {
			if ( dwWeaponMask & ( 1 << i ) ) {
				hits = atoi( CG_Argv( iArg++ ) );
				atts = atoi( CG_Argv( iArg++ ) );
				kills = atoi( CG_Argv( iArg++ ) );
				deaths = atoi( CG_Argv( iArg++ ) );
				headshots = atoi( CG_Argv( iArg++ ) );

				Q_strncpyz( strName, va( "^z%-9s: ", aWeaponInfo[i].pszName ), sizeof( strName ) );
				if ( atts > 0 || hits > 0 ) {
					fHasStats = qtrue;
					Q_strcat( strName, sizeof( strName ), va( "^7%5.1f ^5%4d/%-4d ",
															  ( ( atts == 0 ) ? 0.0 : (float)( hits * 100.0 / (float)atts ) ),
															  hits, atts ) );
				} else {
					Q_strcat( strName, sizeof( strName ), va( "                " ) );
					if ( kills > 0 || deaths > 0 ) {
						fHasStats = qtrue;
					}
				}

				if ( fFull ) {
					txt_dump( va( "%s^2%5d ^1%6d%s\n", strName, kills, deaths, ( ( aWeaponInfo[i].fHasHeadShots ) ? va( " ^3%9d", headshots ) : "" ) ) );
				} else {
					txt_dump( va( "%s^2%3d ^1%3d%s\n", strName, kills, deaths, ( ( aWeaponInfo[i].fHasHeadShots ) ? va( " ^3%2d", headshots ) : "" ) ) );
				}
			}
		}

		if ( fHasStats ) {
			dmg_given = atoi( CG_Argv( iArg++ ) );
			dmg_rcvd  = atoi( CG_Argv( iArg++ ) );
			team_dmg  = atoi( CG_Argv( iArg++ ) );
			gibs	  = atoi( CG_Argv( iArg++ ) );

			if ( !fFull ) {
				txt_dump( "\n" );
			}

			txt_dump( va( "\n^zDamage Given: ^7%-6d  ^zTeam Damage : ^7%d\n", dmg_given, team_dmg ) );
			txt_dump( va(  "^zDamage Recvd: ^7%-6d  ^zBodies Gibbed: ^7%d \n", dmg_rcvd, gibs ) );
		}
	}
	txt_dump( "\n" );
}

// Shoutcast player follow stats
void CG_ParseGameStats(void) {
	clientInfo_t* ci;
	gameStats_t* gs = &cgs.gamestats;

	unsigned int iArg = 1;
	unsigned int nClient = atoi(CG_Argv(iArg++));

	ci = &cgs.clientinfo[nClient];

	gs->nClientID = nClient;
	gs->fHasStats = qtrue;
	gs->kills = atoi(CG_Argv(iArg++));
	gs->deaths = atoi(CG_Argv(iArg++));
	gs->suicides = atoi(CG_Argv(iArg++));
	gs->damage_giv = atoi(CG_Argv(iArg++));
	gs->damage_rec = atoi(CG_Argv(iArg++));
	gs->gibs = atoi(CG_Argv(iArg++));
	gs->revives = atoi(CG_Argv(iArg++));
	gs->health_given = atoi(CG_Argv(iArg++));
	gs->ammo_given = atoi(CG_Argv(iArg++));
}

// 1.0 like stats (+stats)
void CG_parseClientStats_cmd (void( txt_dump ) ( char * ) ) {
	clientInfo_t *ci;
	qboolean fFull = ( txt_dump != CG_printWindow );

//	qboolean fFull = qtrue;
	char strName[MAX_STRING_CHARS];
	int kills, headshots, deaths, team_kills, suicides, acc_shots, acc_hits, damage_giv, damage_rec;
	int bleed, ammo_giv, med_giv, revived, gibs, kill_peak;
	unsigned int iArg = 1;
	unsigned int nClient = atoi( CG_Argv( iArg++ ) );
	float acc;

	ci = &cgs.clientinfo[nClient];

	Q_strncpyz( strName, ci->name, sizeof( strName ) );
	txt_dump( va( "^7Current game stats for: ^7%s\n\n", strName ));

	if ( fFull ) {
		txt_dump(     "Kills Deaths HS TKs Suicides ^2DmgGiv ^1DmgRcv ^4TeamDmg\n" );
		txt_dump(     "----------------------------------------------------\n" );
	} else {
		txt_dump(     "^zKls Dth ^7HS TK Sui ^2DmGiv ^1DmRcv ^4TDmg\n" );
		//txt_dump(     "-------------------------------------\n");
		txt_dump(     "\n" );
	}


	kills = atoi( CG_Argv( iArg++ ) );
	headshots = atoi( CG_Argv( iArg++ ) );
	deaths = atoi( CG_Argv( iArg++ ) );
	team_kills = atoi( CG_Argv( iArg++ ) );
	suicides = atoi( CG_Argv( iArg++ ) );
	acc_shots = atoi( CG_Argv( iArg++ ) );
	acc_hits = atoi( CG_Argv( iArg++ ) );
	damage_giv = atoi( CG_Argv( iArg++ ) );
	damage_rec = atoi( CG_Argv( iArg++ ) );
	bleed = atoi( CG_Argv( iArg++ ) );
	gibs = atoi( CG_Argv( iArg++ ) );
	med_giv = atoi( CG_Argv( iArg++ ) );
	ammo_giv = atoi( CG_Argv( iArg++ ) );
	revived = atoi( CG_Argv( iArg++ ) );
	kill_peak = atoi( CG_Argv( iArg++ ) );

	acc = ( acc_shots > 0 ) ? (((float)acc_hits / (float)acc_shots ) * 100.00f) : 0.00;

	if ( fFull ) {
		txt_dump( va( "%-4d  %-3d   %-3d  %-2d  %-2d       ^2%-5d  ^1%-5d  ^4%-5d\n\n",
					kills, deaths, headshots,
					team_kills, suicides, damage_giv,
					damage_rec, bleed) );
	} else {
		txt_dump( va( "^z%-4d^7 %-3d%-3d %-2d %-2d  ^2%-5d ^1%-5d ^4%-4d\n\n\n",
					kills, deaths, headshots,
					team_kills, suicides, damage_giv,
					damage_rec, bleed) );
	}

	if (ammo_giv > 0 || med_giv > 0)
		txt_dump(va("^3Ammopacks: ^7%-3d    ^3Healthpacks: ^7%d\n", ammo_giv, med_giv));
	if (revived > 0)
		txt_dump(va("^3Revives  : ^7%-3d    \n", revived));
	if (kill_peak > 0 || gibs > 0)
		txt_dump(va("^3Kill Peak: ^7%-3d    ^3Gibbed     : ^7%d\n", kill_peak, gibs));
	if (acc_shots > 0 || acc_hits > 0)
		txt_dump(va("^3Accuracy: ^7%-3.2f  ^3Hits/Shots : ^7%d/^7%d\n", acc, acc_hits, acc_shots));

	if ( !fFull ) {
		txt_dump( "\n" );
	} else {
		txt_dump( "\n----------------------------------------------------\n" );
	}

	txt_dump( "\n" );
}

// +topshots / +bottomshots (console or window)
void CG_parseBestShotsStats_cmd( qboolean doTop, void( txt_dump ) ( char * ) ) {

	int iArg = 1;
	qboolean fFull = ( txt_dump != CG_printWindow );
//	qboolean fFull = qtrue;

	int iWeap = atoi( CG_Argv( iArg++ ) );
	if ( !iWeap ) {
		txt_dump(va("^3No qualifying %sshot info available.      \n\n", ((doTop) ? "top" : "bottom")));
		return;
	}

	if ( fFull ) {
		txt_dump( va( "^5%s Match Accuracies:\n", ( doTop ) ? "BEST" : "WORST" ) );
		txt_dump(  "\n^3WP   Acrcy Hits/Atts Kills Deaths\n" );
		txt_dump(    "-------------------------------------------------------------\n" );
	} else {
		// L0 - Sucks I know...alternative is to patch cg_info.c
		// (port shit load of stuff that we'll never use besides for this) and style it there..
		txt_dump( va( ( doTop ) ? "^dBEST Match Accuracies:                            \n\n" :
								  "^nWORST Match Accuracies:                           \n\n"));
		txt_dump(    "^zWP   Acrcy Hits/Atts Kll Dth\n" );
		txt_dump(    "\n" );
	}

	while ( iWeap ) {
		int cnum = atoi( CG_Argv( iArg++ ) );
		int hits = atoi( CG_Argv( iArg++ ) );
		int atts = atoi( CG_Argv( iArg++ ) );
		int kills = atoi( CG_Argv( iArg++ ) );
		int deaths = atoi( CG_Argv( iArg++ ) );
		float acc = ( atts > 0 ) ? (float)( hits * 100 ) / (float)atts : 0.0;
		char name[32];

		if ( fFull ) {
			BG_cleanName( cgs.clientinfo[cnum].name, name, 30, qfalse );
			txt_dump( va( "^3%s ^7%5.1f ^5%4d/%-4d ^2%5d ^1%6d ^7%s\n",
						  aWeaponInfo[iWeap - 1].pszCode, acc, hits, atts, kills, deaths, name ) );
		} else {
			BG_cleanName( cgs.clientinfo[cnum].name, name, 12, qfalse );
			txt_dump( va( "^z%s ^7%5.1f ^5%4d/%-4d ^2%3d ^1%3d ^7%s\n",
						  aWeaponInfo[iWeap - 1].pszCode, acc, hits, atts, kills, deaths, name ) );
		}

		iWeap = atoi( CG_Argv( iArg++ ) );
	}

	txt_dump( "\n" );
}

////////////////////
// Console Prints //
////////////////////

void CG_parseTopShotsStats_cmd( qboolean doTop, void( txt_dump ) ( char * ) ) {
	int i, iArg = 1;
	int cClients = atoi( CG_Argv( iArg++ ) );
	int iWeap = atoi( CG_Argv( iArg++ ) );
	int wBestAcc = atoi( CG_Argv( iArg++ ) );

	txt_dump( va( "Weapon accuracies for: ^3%s\n",
				  ( iWeap >= WS_KNIFE && iWeap < WS_MAX ) ? aWeaponInfo[iWeap].pszName : "UNKNOWN" ) );

	txt_dump( "\n^3  Acc Hits/Atts Kills Deaths\n" );
	txt_dump(    "----------------------------------------------------------\n" );

	if ( !cClients ) {
		txt_dump( "NO QUALIFYING WEAPON INFO AVAILABLE.\n" );
		return;
	}

	for ( i = 0; i < cClients; i++ ) {
		int cnum = atoi( CG_Argv( iArg++ ) );
		int hits = atoi( CG_Argv( iArg++ ) );
		int atts = atoi( CG_Argv( iArg++ ) );
		int kills = atoi( CG_Argv( iArg++ ) );
		int deaths = atoi( CG_Argv( iArg++ ) );
		float acc = ( atts > 0 ) ? (float)( hits * 100 ) / (float)atts : 0.0;
		const char* color = ( ( ( doTop ) ? acc : ( (float)wBestAcc ) + 0.999 ) >= ( ( doTop ) ? wBestAcc : acc ) ) ? "^3" : "^7";
		char name[32];

		BG_cleanName( cgs.clientinfo[cnum].name, name, 30, qfalse );
		txt_dump( va( "%s%5.1f ^5%4d/%-4d ^2%5d ^1%6d %s%s\n", color, acc, hits, atts, kills, deaths, color, name ) );
	}
}

/////////////////////
// Window handling //
/////////////////////

// +wstats window
void CG_wstatsParse_cmd( void ) {
    if( cg.showStats ) {
        if ( cg.statsWindow == NULL
            || cg.statsWindow->id != WID_STATS
            || cg.statsWindow->inuse == qfalse
            )  {
            CG_createStatsWindow();
        } else if ( cg.statsWindow->state == WSTATE_SHUTDOWN ) {
            cg.statsWindow->state = WSTATE_START;
            cg.statsWindow->time = trap_Milliseconds();
        }

        if ( cg.statsWindow == NULL ) {
                cg.showStats = qfalse;
        }
        else {
            cg.statsWindow->effects |= WFX_TEXTSIZING;
            cg.statsWindow->lineCount = 0;
            cg.windowCurrent = cg.statsWindow;
            CG_parseWeaponStats_cmd( CG_printWindow );
        }
    }
}
// +stats window
void CG_clientParse_cmd( void ) {
    if( cg.showCLgameStats ) {
        if ( cg.clientStatsWindow == NULL
             || cg.clientStatsWindow->id != WID_CLIENTSTATS
             || cg.clientStatsWindow->inuse == qfalse
             ) {
            CG_createClientStatsWindow();
        } else if ( cg.clientStatsWindow->state == WSTATE_SHUTDOWN ) {
            cg.clientStatsWindow->state = WSTATE_START;
            cg.clientStatsWindow->time = trap_Milliseconds();
        }

        if ( cg.clientStatsWindow == NULL ) {
            cg.showCLgameStats = qfalse;
        }
        else{
            cg.clientStatsWindow->effects |= WFX_TEXTSIZING;
            cg.clientStatsWindow->lineCount = 0;
            cg.windowCurrent = cg.clientStatsWindow;
            CG_parseClientStats_cmd( CG_printWindow );
        }

    }

}
// +topshots window
void CG_TopShotsParse_cmd( void ) {

	if ( cg.topshotsWindow == NULL
		 || cg.topshotsWindow->id != WID_TOPSHOTS
		 || cg.topshotsWindow->inuse == qfalse
		 ) {
		CG_createTopShotsWindow();
	} else if ( cg.topshotsWindow->state == WSTATE_SHUTDOWN ) {
		cg.topshotsWindow->state = WSTATE_START;
		cg.topshotsWindow->time = trap_Milliseconds();
	}

	if ( cg.topshotsWindow != NULL ) {
		cg.topshotsWindow->effects |= WFX_TEXTSIZING;
		cg.topshotsWindow->lineCount = 0;
		cg.windowCurrent = cg.topshotsWindow;
		CG_parseBestShotsStats_cmd(qtrue, CG_printWindow);
	}

}

//////////////////
// Stats - MISC //
//////////////////

// End table / Scores table
void CG_scores_cmd( void ) {
	const char *str = CG_Argv( 1 );

	//CG_Printf( "[skipnotify]%s", str );	// L0 - FIXME - this causes double prints..
	CG_Printf( "%s", str );	// L0 - FIXME - this causes double prints..
	if ( cgs.dumpStatsFile > 0 ) {
		char s[MAX_STRING_CHARS];

		BG_cleanName( str, s, sizeof( s ), qtrue );
		trap_FS_Write( s, strlen( s ), cgs.dumpStatsFile );
	}

	if ( trap_Argc() > 2 ) {
		if ( cgs.dumpStatsFile > 0 ) {
			qtime_t ct;

			trap_RealTime( &ct );
			str = va( "\nStats recorded: %02d:%02d:%02d (%02d %s %d)\n",
					  ct.tm_hour, ct.tm_min, ct.tm_sec,
					  ct.tm_mday, aMonths[ct.tm_mon], 1900 + ct.tm_year );

			trap_FS_Write( str, strlen( str ), cgs.dumpStatsFile );

			CG_Printf( "[cgnotify]\n^3>>> Stats recorded to: ^7%s\n", cgs.dumpStatsFileName );
			trap_FS_FCloseFile( cgs.dumpStatsFile );
			cgs.dumpStatsFile = 0;
		}
		cgs.dumpStatsTime = 0;
	}
}
// Print in file
void CG_printFile( char *str ) {
	CG_Printf( "%s", str );
	if ( cgs.dumpStatsFile > 0 ) {
		char s[MAX_STRING_CHARS];

		BG_cleanName( str, s, sizeof( s ), qtrue );
		trap_FS_Write( s, strlen( s ), cgs.dumpStatsFile );
	}
}
// Dump stats in file
void CG_dumpStats(qboolean endOfRound) {
	qtime_t ct;
	qboolean fDoScores = qfalse;
	const char *info = CG_ConfigString( CS_SERVERINFO );
	char *s = va( "\n^3>>> %s: ^3%s\n\n", CG_TranslateString( "Map" ), Info_ValueForKey( info, "mapname" ) );

	trap_RealTime( &ct );
	// /me holds breath (using circular va() buffer)
	if ( cgs.dumpStatsFile == 0 ) {
		fDoScores = qtrue;
		cgs.dumpStatsFileName = va( "stats/%d.%02d.%02d/%02d%02d%02d.%s.txt",
									1900 + ct.tm_year, ct.tm_mon + 1, ct.tm_mday,
									ct.tm_hour, ct.tm_min, ct.tm_sec, Info_ValueForKey( info, "mapname" ) ); // Map has to be last so they sort right..
	}

	if ( cgs.dumpStatsFile != 0 ) {
		trap_FS_FCloseFile( cgs.dumpStatsFile );
	}
	trap_FS_FOpenFile( cgs.dumpStatsFileName, &cgs.dumpStatsFile, FS_APPEND );

	CG_printFile( s );
	CG_parseWeaponStats_cmd( CG_printFile );
	if ( cgs.dumpStatsFile == 0 ) {
		CG_Printf( "[cgnotify]\n^3>>> %s: %s\n", CG_TranslateString( "Could not create logfile" ), cgs.dumpStatsFileName );
	}

	// Daisy-chain to scores info
	//	-- we play a game here for a statsall dump:
	//		1. we still have more ws entries in the queue to parse
	//		2. on the 1st ws entry, go ahead and send out the scores request
	//		3. we'll just continue to parse the remaining ws entries and dump them to the log
	//		   before the scores result would ever hit us.. thus, we still keep proper ordering :)
	if ( fDoScores ) {
		trap_SendClientCommand( "scoresdump" );
	}

	// if intermission play the end of round sounds
	if (endOfRound)
	{
		const char* buf;

		s = CG_ConfigString(CS_MULTI_MAPWINNER);
		buf = Info_ValueForKey(s, "winner");

		if (atoi(buf))
		{
			trap_S_StartLocalSound(cgs.media.alliesWin, CHAN_LOCAL_SOUND);
		}
		else
		{
			trap_S_StartLocalSound(cgs.media.axisWin, CHAN_LOCAL_SOUND);
		}
	}
}
/**************** L0 - OSP Stats dump ends here *****************/
void CG_ForceTapOut_f(void); // OSPx - Tapout
/*
=================
CG_ServerCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
static void CG_ServerCommand( void ) {
	const char  *cmd;
	char text[MAX_SAY_TEXT];

	cmd = CG_Argv( 0 );

	if ( !cmd[0] ) {
		// server claimed the command
		return;
	}

	if ( !strcmp( cmd, "tinfo" ) ) {
		CG_ParseTeamInfo();
		return;
	}

	if ( !strcmp( cmd, "scores" ) ) {
		CG_ParseScores();
		return;
	}

	if (!Q_stricmp(cmd, "cpm")) {
		//CG_AddPMItem(PM_MESSAGE, CG_LocalizeServerCommand(CG_Argv(1)), cgs.media.voiceChatShader); // ET had a popup message
		CG_CenterPrint(CG_LocalizeServerCommand(CG_Argv(1)), SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.25), SMALLCHAR_WIDTH);  //----(SA)	modified
		return;
	}
	if ( !strcmp( cmd, "cp" ) ) {
		// NERVE - SMF
		int args = trap_Argc();
		char *s;

		if ( args >= 3 ) {
			s = CG_TranslateString( CG_Argv( 1 ) );

			if ( args == 4 ) {
				s = va( "%s%s", CG_Argv( 3 ), s );
			}

			// OSPx - Client logging
			if (cg_printObjectiveInfo.integer > 0 && (args == 4 || atoi(CG_Argv(2)) > 1) && cgs.gamestate == GS_PLAYING && cgs.match_paused == PAUSE_NONE) {
				CG_Printf("[cgnotify]*** ^3INFO: ^5%s\n", Q_CleanStr((char *)CG_LocalizeServerCommand(CG_Argv(1))));
			}

			CG_PriorityCenterPrint( s, SCREEN_HEIGHT - ( SCREEN_HEIGHT * 0.25 ), SMALLCHAR_WIDTH, atoi( CG_Argv( 2 ) ) );
		} else {
			CG_CenterPrint( CG_LocalizeServerCommand( CG_Argv( 1 ) ), SCREEN_HEIGHT - ( SCREEN_HEIGHT * 0.25 ), SMALLCHAR_WIDTH );  //----(SA)	modified
		}
		return;
	}
	// L0 - Console print only..
	if ( !strcmp( cmd, "@print" ) ) {
		CG_Printf( "[skipnotify]%s\n", CG_Argv( 1 )  );  // Add to console so people can toggle it and see it again..
		return;
	}
	// End
	// Pop In
	if (!Q_stricmp(cmd, "popin")) {
		int args = trap_Argc();
		qboolean fade=qfalse;

		if (args >= 3) {
			fade = qtrue;
		}
		CG_PopinPrint(CG_LocalizeServerCommand(CG_Argv(1)), SMALLCHAR_HEIGHT, fade);
		return;
	}
	if (!Q_stricmp(cmd, "prioritypopin")) {
		if (cg_showPriorityText.integer)
		{
			int args = trap_Argc();
			qboolean fade = qfalse;

			if (args >= 3) {
				fade = qtrue;
			}
			CG_PopinPrint(CG_LocalizeServerCommand(CG_Argv(1)), SMALLCHAR_HEIGHT, fade);
		}
		return;
	}
// L0 - OSP's stats dump
	// Scores
	if ( !Q_stricmp( cmd, "sc" ) ) {
		CG_scores_cmd();
		return;
	}
	// Weapon stats (console dump)
	if ( !Q_stricmp( cmd, "ws" ) ) {
		if ( cgs.dumpStatsTime > cg.time ) {
			qboolean endOfRound = qfalse;
			if (cg.predictedPlayerState.pm_type == PM_INTERMISSION) endOfRound = qtrue;
			CG_dumpStats(endOfRound);
		} else {
			CG_parseWeaponStats_cmd( CG_printConsoleString );
			cgs.dumpStatsTime = 0;
		}
		return;
	}
	// +wstats
	if ( !Q_stricmp( cmd, "wws" ) ) {
		CG_wstatsParse_cmd();
		return;
	}
	// +stats
	if ( !Q_stricmp( cmd, "cgs" ) ) {
		CG_clientParse_cmd();
		return;
	}
	if (!Q_stricmp(cmd, "gamestats")) {
		CG_ParseGameStats();
		return;
	}
	// +topshots
	if ( !Q_stricmp( cmd, "wbstats" ) ) {
		CG_TopShotsParse_cmd();
		return;
	}
	// OSP - "topshots"-related commands (prints)
	if ( !Q_stricmp( cmd, "astats" ) ) {
		CG_parseTopShotsStats_cmd( qtrue, CG_printConsoleString );
		return;
	}
	if ( !Q_stricmp( cmd, "astatsb" ) ) {
		CG_parseTopShotsStats_cmd( qfalse, CG_printConsoleString );
		return;
	}
	if ( !Q_stricmp( cmd, "bstats" ) ) {
		CG_parseBestShotsStats_cmd( qtrue, CG_printConsoleString );
		return;
	}
	if ( !Q_stricmp( cmd, "bstatsb" ) ) {
		CG_parseBestShotsStats_cmd( qfalse, CG_printConsoleString );
		return;
	}
	// /stats (to console like in good old 1.0 days)
	if ( !Q_stricmp( cmd, "cgsp" ) ) {
		CG_parseClientStats_cmd( CG_printConsoleString );
		return;
	}
// L0 -End OSP Stats dump
	// Force instant tapout
	// NOTE: cg_forceTapout prevails if enabled..
	if (!Q_stricmp(cmd, "reqforcespawn")) {
		if (cg_instantTapout.integer && !cg_forceTapout.integer) {
			CG_ForceTapOut_f();
		}
		return;
	}
	// Force tapout on respawn (reuse instant tapout..)
	if (!Q_stricmp(cmd, "reqforcetapout")) {
		 if (cg_forceTapout.integer) {
			CG_ForceTapOut_f();
		}
		return;
	}
	if ( !strcmp( cmd, "cs" ) ) {
		CG_ConfigStringModified();
		return;
	}

	// NERVE - SMF
	if ( !strcmp( cmd, "shake" ) ) {
		CG_StartShakeCamera( atof( CG_Argv( 1 ) ) );
		return;
	}
	// -NERVE - SMF

	if ( !strcmp( cmd, "print" ) ) {
		CG_Printf( "[cgnotify]%s", CG_LocalizeServerCommand( CG_Argv( 1 ) ) );
#ifdef MISSIONPACK
		cmd = CG_Argv( 1 );           // yes, this is obviously a hack, but so is the way we hear about
									  // votes passing or failing
		if ( !Q_stricmpn( cmd, "vote failed", 11 ) || !Q_stricmpn( cmd, "team vote failed", 16 ) ) {
			trap_S_StartLocalSound( cgs.media.voteFailed, CHAN_ANNOUNCER );
		} else if ( !Q_stricmpn( cmd, "vote passed", 11 ) || !Q_stricmpn( cmd, "team vote passed", 16 ) ) {
			trap_S_StartLocalSound( cgs.media.votePassed, CHAN_ANNOUNCER );
		}
#endif
		return;
	}

	if ( !strcmp( cmd, "chat" ) ) {
		const char *s;

		if ( cg_teamChatsOnly.integer ) {
			return;
		}

		// OSPx - Not in demo if it's off..
		if (cg.demoPlayback && cgs.noChat) {
			return;
		}
		if ( atoi( CG_Argv( 2 ) ) ) {
			s = CG_LocalizeServerCommand( CG_Argv( 1 ) );
		} else {
			s = CG_Argv( 1 );
		}

		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		Q_strncpyz( text, s, MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );

		// OSPx - No chat if it's disabled..
		if (!cg_noChat.integer)
		{
			CG_AddToTeamChat(text); // JPW NERVE

			if (cg_chatBeep.integer == 1)
			{
				trap_S_StartLocalSound(cgs.media.normalChat, CHAN_LOCAL_SOUND);
			}

		}
		CG_Printf( "[skipnotify]%s\n", text ); // JPW NERVE

		// NERVE - SMF - we also want this to display in limbo chat
		if ( cgs.gametype >= GT_WOLF ) {
			trap_UI_LimboChat( text );
		}

		return;
	}

	if ( !strcmp( cmd, "tchat" ) ) {
		const char *s;

		if ( atoi( CG_Argv( 2 ) ) ) {
			s = CG_LocalizeServerCommand( CG_Argv( 1 ) );
		} else {
			s = CG_Argv( 1 );
		}

		// OSPx - Not in demo if it's off..
		if (cg.demoPlayback && cgs.noChat) {
			return;
		}

		// OSPx - No voice prints
		if  (cg_noVoice.integer < 2)
			trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		Q_strncpyz( text, s, MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );

		// OSPx - No chat if it's disabled..
		if (!cg_noChat.integer)
		{
			CG_AddToTeamChat(text);

			if (cg_chatBeep.integer == 1 || cg_chatBeep.integer == 2)
			{
				trap_S_StartLocalSound(cgs.media.teamChat, CHAN_LOCAL_SOUND);
			}
		}

		CG_Printf( "[skipnotify]%s\n", text ); // JPW NERVE

		// NERVE - SMF - we also want this to display in limbo chat
		if ( cgs.gametype >= GT_WOLF ) {
			trap_UI_LimboChat( text );
		}

		return;
	}

	if ( !strcmp( cmd, "vchat" ) ) {
		// OSPx - No voice prints
		if (cg_noVoice.integer == 1 || cg_noVoice.integer == 3)
			return;

		CG_VoiceChat( SAY_ALL );            // NERVE - SMF - enabled support
		return;
	}

	if ( !strcmp( cmd, "vtchat" ) ) {
		// OSPx - No voice prints
		if (cg_noVoice.integer == 2 || cg_noVoice.integer == 3)
			return;

		// OSPx - Not in demo if it's off..
		if (cg.demoPlayback && cgs.noVoice) {
			return;
		}
		CG_VoiceChat( SAY_TEAM );           // NERVE - SMF - enabled support
		return;
	}

	if ( !strcmp( cmd, "vtell" ) ) {
		// OSPx - No voice prints
		if (cg_noVoice.integer)
			return;
		// OSPx - Not in demo if it's off..
		if (cg.demoPlayback && cgs.noVoice) {
			return;
		}

		CG_VoiceChat( SAY_TELL );           // NERVE - SMF - enabled support
		return;
	}

	// NERVE - SMF - limbo chat
	if ( !strcmp( cmd, "lchat" ) ) {
		trap_S_StartLocalSound( cgs.media.talkSound, CHAN_LOCAL_SOUND );
		Q_strncpyz( text, CG_Argv( 1 ), MAX_SAY_TEXT );
		CG_RemoveChatEscapeChar( text );
		trap_UI_LimboChat( text );
		CG_Printf( "[skipnotify]%s\n", text ); // JPW NERVE
		return;
	}
	// -NERVE - SMF

	// NERVE - SMF
	if ( !Q_stricmp( cmd, "oid" ) ) {
		CG_ObjectivePrint( CG_Argv( 1 ), SMALLCHAR_WIDTH );
		return;
	}
	// -NERVE - SMF

	// DHM - Nerve :: Allow client to lodge a complaing
	if ( !Q_stricmp( cmd, "complaint" ) ) {
		cgs.complaintEndTime = cg.time + 20000;
		cgs.complaintClient = atoi( CG_Argv( 1 ) );

		if ( cgs.complaintClient < 0 ) {
			cgs.complaintEndTime = cg.time + 10000;
		}

		return;
	}
	// dhm

	if ( !strcmp( cmd, "map_restart" ) ) {
		CG_MapRestart();
		return;
	}

//	if ( !strcmp( cmd, "startCam" ) ) {
//		CG_StartCamera( CG_Argv(1), atoi(CG_Argv(2)) );
//		return;
//	}

	if ( !strcmp( cmd, "mvspd" ) ) {
		CG_RequestMoveSpeed( CG_Argv( 1 ) );
		return;
	}

	if ( Q_stricmp( cmd, "remapShader" ) == 0 ) {
		if ( trap_Argc() == 4 ) {
			trap_R_RemapShader( CG_Argv( 1 ), CG_Argv( 2 ), CG_Argv( 3 ) );
		}
	}

	// loaddeferred can be both a servercmd and a consolecmd
	if ( !strcmp( cmd, "loaddeferred" ) ) {  // spelling fixed (SA)
		CG_LoadDeferredPlayers();
		return;
	}

	// clientLevelShot is sent before taking a special screenshot for
	// the menu system during development
	if ( !strcmp( cmd, "clientLevelShot" ) ) {
		cg.levelShot = qtrue;
		return;
	}

	// ydnar: bug 267: server sends this command when it's about to kill the current server, before the client can reconnect
	//TODO: if demo is recording, stop recording and start a new demo after server respawn (eliminate multiple gamestates in a demo) 
	if (!Q_stricmp(cmd, "spawnserver")) {
		cg.serverRespawning = qtrue;
		return;
	}

	if (!Q_stricmp(cmd, "revalidate")) {
		trap_Rest_Validate();
		return;
	}

	if (!Q_stricmp(cmd, "rereload")) {
		trap_Rest_Build(CG_Argv(1));
		return;
	}

	// reqSS
	if (!strcmp(cmd, "reqss"))
	{
		//CG_Printf("^nServer requested screenshot.. sending.\n");
		char* address = va("%s", CG_Argv(1));
		char* hookid = va("%s", CG_Argv(2));
		char* hooktoken = va("%s", CG_Argv(3));
		char* waittime = va("%s", CG_Argv(4));
		char* datetime = va("%s", CG_Argv(5));

		trap_RequestSS(address, hookid, hooktoken, waittime, datetime);
		return;
	}

	// apiQuery
	if (!strcmp(cmd, "api"))
	{
		char* result = (char*)CG_Argv(1);
		PrintApiResponse(result); // CG_printConsoleString);
		return;
	}

	CG_Printf( "Unknown client game command: %s\n", cmd );
}

void PrintApiResponse(char* result) //void(txt_dump)(char*)
{
	result = Q_StrReplace(result, "[NL]", "\n");

	CG_Printf("%s", result);

	//char* result = va("%s", (char*)CG_Argv(1));
	//txt_dump(result);
	//txt_dump("\n");
}

/*
====================
CG_ExecuteNewServerCommands

Execute all of the server commands that were received along
with this this snapshot.
====================
*/
void CG_ExecuteNewServerCommands( int latestSequence ) {
	while ( cgs.serverCommandSequence < latestSequence ) {
		if ( trap_GetServerCommand( ++cgs.serverCommandSequence ) ) {
			CG_ServerCommand();
		}
	}
}
