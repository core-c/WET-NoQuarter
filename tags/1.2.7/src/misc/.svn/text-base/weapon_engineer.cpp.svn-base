void Weapon_Engineer( gentity_t *ent ) 
{
	trace_t		tr;
	gentity_t	*traceEnt, *hit;
	vec3_t		mins, maxs;									// JPW NERVE
	int			i, num, touch[MAX_GENTITIES], scored = 0;	// JPW NERVE
	int			dynamiteDropTeam;
	int			engineerLevel;
	vec3_t		end;
	vec3_t		origin;

	// jP - Shortcut
	engineerLevel = ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION];

	// DHM - Nerve :: Can't heal an MG42 if you're using one!
	if( ent->client->ps.persistant[PERS_HWEAPON_USE] ) 
	{
		return;
	}

	// if touching a constructable objective, try to build it
	if( ent->client->touchingTOI )
	{
		if( TryConstructing( ent ) ) 
		{
			return;
		}
	}

	AngleVectors (ent->client->ps.viewangles, forward, right, up);
	VectorCopy( ent->client->ps.origin, muzzleTrace );
	muzzleTrace[2] += ent->client->ps.viewheight;
	
	VectorMA (muzzleTrace, 64, forward, end);			// CH_BREAKABLE_DIST
	trap_EngineerTrace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT | CONTENTS_TRIGGER );
	
	if( tr.surfaceFlags & SURF_NOIMPACT )
	{
		return;
	}

	// jet Pilot - TODO : Add helping mg soldiers here

	// no contact
	if( tr.fraction == 1.0f ) 
	{
		return;
	}

	// dont mess with the world, cause it's unmessable-with
	if( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD ) 
	{
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	if( G_EmplacedGunIsRepairable( traceEnt, ent ) )
	{		

		if ( ent->client->ps.classWeaponTime + level.engineerChargeTime[ent->client->sess.sessionTeam-1] < level.time ) 
		{
			ent->client->ps.classWeaponTime = level.time - level.engineerChargeTime[ent->client->sess.sessionTeam-1];
		}

		// jet Pilot - use the array of costs now, instead of the if/else block	
		ent->client->ps.classWeaponTime += chargeCost_Repair[engineerLevel] * 150;

		if ( ent->client->ps.classWeaponTime > level.time )
		{
			ent->client->ps.classWeaponTime = level.time;
			return;		// Out of "ammo"
		}
		
		if( traceEnt->health >= 255 )			// finished repairing it
		{
			traceEnt->s.frame = 0;

			if ( traceEnt->mg42BaseEnt > 0 ) 
			{
				g_entities[ traceEnt->mg42BaseEnt ].health		= MG42_MULTIPLAYER_HEALTH;
				g_entities[ traceEnt->mg42BaseEnt ].takedamage  = qtrue;
				traceEnt->health = 0;
			} 
			else
			{
				traceEnt->health = MG42_MULTIPLAYER_HEALTH;
			}

			G_LogPrintf("Repair: %d\n", ent - g_entities);	// OSP

			if( traceEnt->sound3to2 != ent->client->sess.sessionTeam )
			{
				AddScore( ent, WOLF_REPAIR_BONUS ); // JPW NERVE props to the E for the fixin'
				G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 3.f );
				G_DebugAddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 3.f, "repairing a MG42" );
			}

			traceEnt->takedamage = qtrue;
			traceEnt->s.eFlags  &= ~EF_SMOKING;

			trap_SendServerCommand(ent-g_entities, "cp \"You have repaired the MG!\n\"");
			G_AddEvent( ent, EV_MG42_FIXED, 0 );
		} 
		else // not finished, so continue repairing it
		{
			traceEnt->health += 3;
		}
	}
	else	// not an MG needin' fixin'
	{
		trap_EngineerTrace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );

		if ( tr.surfaceFlags & SURF_NOIMPACT )
		{
			return;
		}
		
		if(tr.fraction == 1.0f)
		{
			return;
		}
		
		if ( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD )
		{
			return;
		}

		traceEnt = &g_entities[ tr.entityNum ];

		if ( traceEnt->methodOfDeath == MOD_LANDMINE ) 
		{
			trace_t tr2;
			vec3_t base;
			vec3_t tr_down = {0, 0, 16};

			VectorSubtract(traceEnt->s.pos.trBase, tr_down, base);

			trap_EngineerTrace( &tr2, traceEnt->s.pos.trBase, NULL, NULL, base, traceEnt->s.number, MASK_SHOT );
			
			// ydnar: added "surfaceparm landmine" (SURF_LANDMINE) support
			//%	if(!(tr2.surfaceFlags & (SURF_GRASS | SURF_SNOW | SURF_GRAVEL)) || 
			if( !(tr2.surfaceFlags & SURF_LANDMINE) || (tr2.entityNum != ENTITYNUM_WORLD && (!g_entities[tr2.entityNum].inuse || g_entities[tr2.entityNum].s.eType != ET_CONSTRUCTIBLE))) 
			{
				trap_SendServerCommand(ent-g_entities, "cp \"Landmine cannot be armed here...\" 1");

				G_FreeEntity( traceEnt );

				Add_Ammo(ent, WP_LANDMINE, 1, qfalse);				

				// jet Pilot - use charge cost array (Landmine)
				jP_UnDoChargeCost(ent, SK_ENGINEER, chargeCost_Landmine);		

				ent->client->sess.aWeaponStats[WS_LANDMINE].atts--;
				return;
//bani
// rain - #384 - check landmine team so that enemy mines can be disarmed
// even if you're using all of yours :x
			} 
			else if ( G_CountTeamLandmines(ent->client->sess.sessionTeam) >= g_maxTeamLandmines.integer && G_LandmineTeam(traceEnt) == ent->client->sess.sessionTeam) 
			{
				if(G_LandmineUnarmed(traceEnt)) 
				{
// rain - should be impossible now
//					if ( G_LandmineTeam( traceEnt ) != ent->client->sess.sessionTeam )
//						return;

					trap_SendServerCommand(ent-g_entities, "cp \"Your team has too many landmines placed...\" 1");

					G_FreeEntity( traceEnt );

					Add_Ammo(ent, WP_LANDMINE, 1, qfalse);
					// rain - #202 - give back the correct charge amount
/*					if (ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 3) {
						ent->client->ps.classWeaponTime -= .33f * 
							level.engineerChargeTime[ent->client->sess.sessionTeam-1];
					}
					else {
						ent->client->ps.classWeaponTime -= .5f * 
							level.engineerChargeTime[ent->client->sess.sessionTeam-1];
					}
*/

					// jet Pilot - use charge cost array (Landmine)
					jP_UnDoChargeCost(ent, SK_ENGINEER, chargeCost_Landmine);
			//		ent->client->ps.classWeaponTime -= chargeCost_Landmine[engineerLevel] * 
			//			level.engineerChargeTime[ent->client->sess.sessionTeam-1];

					ent->client->sess.aWeaponStats[WS_LANDMINE].atts--;
					return;
				}
//bani - #471
				else 
				{
					goto evilbanigoto;
				}
			}
			else // dont have too many mine placed already, and it is a suitable spot
			{
				if(G_LandmineUnarmed(traceEnt)) {
					// Opposing team cannot accidentally arm it
					if( G_LandmineTeam(traceEnt) != ent->client->sess.sessionTeam )
						return;

					G_PrintClientSpammyCenterPrint(ent-g_entities, "Arming landmine...");

					// Give health until it is full, don't continue

					

					// jet Pilot - Use Arming Speed Array (Landmine)
					traceEnt->health += armSpeed_LandMine[engineerLevel]; 
					
					/*	
					if( ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 2 ) {
						traceEnt->health += 24;
					} else {
						traceEnt->health += 12;
					}
					*/

					if ( traceEnt->health >= 250 ) {
						//traceEnt->health = 255;
						trap_SendServerCommand(ent-g_entities, "cp \"Landmine armed...\" 1");
					} else {
						return;
					}

					traceEnt->r.contents = 0;	// (player can walk through)
					trap_LinkEntity( traceEnt );

					// jet Pilot - Forty mod crosshair mines
					traceEnt->s.otherEntityNum = ent->s.number; 

					// Don't allow disarming for sec (so guy that WAS arming doesn't start disarming it!
					traceEnt->timestamp 	= level.time + 1000;
					traceEnt->health		= 0;
					traceEnt->s.teamNum		= ent->client->sess.sessionTeam;
					traceEnt->s.modelindex2 = 0;
					traceEnt->nextthink		= level.time + 2000;
					traceEnt->think			= G_LandminePrime;
				} 
				else // landmine is armed
				{
//bani - #471
evilbanigoto:
					if (traceEnt->timestamp > level.time)
						return;
					if (traceEnt->health >= 250) // have to do this so we don't score multiple times
						return;


					// jet Pilot - Use Arming Speed Array (Landmine) (defuse at 1/4 speed)
					traceEnt->health += disarmSpeed_LandMine[engineerLevel]; 
					/*
					if( ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 2 ) {
						traceEnt->health += 6;
					} else {
						traceEnt->health += 3;
					}
					*/

					G_PrintClientSpammyCenterPrint(ent-g_entities, "Defusing landmine");

					if ( traceEnt->health >= 250 ) {
/*						traceEnt->health	= 255;
						traceEnt->think		= G_FreeEntity;
						traceEnt->nextthink = level.time + FRAMETIME;*/

						trap_SendServerCommand(ent-g_entities, "cp \"Landmine defused...\" 1");

						Add_Ammo(ent, WP_LANDMINE, 1, qfalse);

						if( G_LandmineTeam( traceEnt ) != ent->client->sess.sessionTeam ) {
							G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 4.f );
							G_DebugAddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 4.f, "defusing an enemy landmine" );
						}

						// update our map
						/*{
							// if it's an enemy mine, update both teamlists
							int teamNum;
							mapEntityData_t	*mEnt;
							mapEntityData_Team_t *teamList;

							teamNum = traceEnt->s.teamNum % 4;

							teamList = ent->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[0] : &mapEntityData[1];
							if((mEnt = G_FindMapEntityData(teamList, traceEnt-g_entities)) != NULL) {
								G_FreeMapEntityData( teamList, mEnt );
							}

							teamList = ent->client->sess.sessionTeam == TEAM_AXIS ? &mapEntityData[1] : &mapEntityData[0];	// inverted
							if((mEnt = G_FindMapEntityData(teamList, traceEnt-g_entities)) != NULL) {
								if( teamNum != ent->client->sess.sessionTeam ) {
									G_FreeMapEntityData( teamList, mEnt );
								} else {
								//	mEnt->type = ME_LANDMINE;	// set it back to this as it might have been set to 'about to explode'.
									mEnt->entNum = -1;
								}
							}
						}*/
						{
							mapEntityData_t	*mEnt;

							if((mEnt = G_FindMapEntityData(&mapEntityData[0], traceEnt-g_entities)) != NULL) {
								G_FreeMapEntityData( &mapEntityData[0], mEnt );
							}

							if((mEnt = G_FindMapEntityData(&mapEntityData[1], traceEnt-g_entities)) != NULL) {
								G_FreeMapEntityData( &mapEntityData[1], mEnt );
							}

							G_FreeEntity( traceEnt );
						}
					} else {
						return;
					}
				}	
			}
		} 
		else if ( traceEnt->methodOfDeath == MOD_SATCHEL ) 
		{
			if( traceEnt->health >= 250 ) // have to do this so we don't score multiple times
				return;
			
			// jet Pilot - Use disarm speed array (Satchel)
			traceEnt->health += disarmSpeed_Satchel[engineerLevel]; // 3;

			G_PrintClientSpammyCenterPrint(ent-g_entities, "Disarming satchel charge...");

			if ( traceEnt->health >= 250 ) {

				traceEnt->health = 255;
				traceEnt->think = G_FreeEntity;
				traceEnt->nextthink = level.time + FRAMETIME;

				//bani - consistency with dynamite defusing
				G_PrintClientSpammyCenterPrint(ent-g_entities, "Satchel charge disarmed...");

				G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
				G_DebugAddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "disarming satchel charge" );
			} else {
				return;
			}
//bani - no tripmine...
#if 0
		} else if ( traceEnt->methodOfDeath == MOD_TRIPMINE ) {
			// Give health until it is full, don't continue
			traceEnt->health += 3;

			G_PrintClientSpammyCenterPrint(ent-g_entities, "Disarming tripmine...");

			if ( traceEnt->health >= 250 ) {
				traceEnt->health = 255;
				traceEnt->think = G_FreeEntity;
				traceEnt->nextthink = level.time + FRAMETIME;

				Add_Ammo(ent, WP_TRIPMINE, 1, qfalse);
			} else {
				return;
			}
#endif
		} else
		if ( traceEnt->methodOfDeath == MOD_DYNAMITE ) {

			// Not armed
			if ( traceEnt->s.teamNum >= 4 ) {
				//bani
				qboolean friendlyObj	= qfalse;
				qboolean enemyObj		= qfalse;

				// Opposing team cannot accidentally arm it
				if ( (traceEnt->s.teamNum - 4) != ent->client->sess.sessionTeam )
					return;

				G_PrintClientSpammyCenterPrint(ent-g_entities, "Arming dynamite...");

				// Give health until it is full, don't continue

				// jet Pilot - use arming speed array (Dynamite)
				traceEnt->health += armSpeed_Dynamite[engineerLevel];

				/*
				if( ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 2 )
					traceEnt->health += 14;
				else
					traceEnt->health += 7;
				*/

				{
					int		entityList[MAX_GENTITIES];
					int		numListedEntities;
					int		e;
					vec3_t  org;

					VectorCopy( traceEnt->r.currentOrigin, org );
					org[2] += 4;	// move out of ground

					G_TempTraceIgnorePlayersAndBodies();
					numListedEntities = EntsThatRadiusCanDamage( org, traceEnt->splashRadius, entityList );
					G_ResetTempTraceIgnoreEnts();

					for( e = 0; e < numListedEntities; e++ ) {
						hit = &g_entities[entityList[ e ]];

						if( hit->s.eType != ET_CONSTRUCTIBLE ) {
							continue;
						}

						// invulnerable
						if( hit->spawnflags & CONSTRUCTIBLE_INVULNERABLE || (hit->parent && hit->parent->spawnflags & 8) ) {
							continue;
						}

						if( !G_ConstructionIsPartlyBuilt( hit ) ) {
							continue;
						}

						// is it a friendly constructible
						if( hit->s.teamNum == traceEnt->s.teamNum - 4 ) {
//bani
//							G_FreeEntity( traceEnt );
//							trap_SendServerCommand( ent-g_entities, "cp \"You cannot arm dynamite near a friendly construction!\" 1");
//							return;
							friendlyObj = qtrue;
						}
					}
				}

				VectorCopy( traceEnt->r.currentOrigin, origin );
				SnapVector( origin );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );
				num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );

				for ( i=0 ; i<num ; i++ ) {
					hit = &g_entities[touch[i]];

					if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
						continue;
					}

					if ((hit->s.eType == ET_OID_TRIGGER)) {
						if ( !(hit->spawnflags & (AXIS_OBJECTIVE|ALLIED_OBJECTIVE)) )
							continue;

						// Arnout - only if it targets a func_explosive
						if( hit->target_ent && Q_stricmp( hit->target_ent->classname, "func_explosive" ) )
							continue;

						if ( ((hit->spawnflags & AXIS_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_AXIS)) || 
							 ((hit->spawnflags & ALLIED_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_ALLIES)) ) {
//bani
//							G_FreeEntity( traceEnt );
//							trap_SendServerCommand( ent-g_entities, "cp \"You cannot arm dynamite near a friendly objective!\" 1");
//							return;
							friendlyObj = qtrue;
						}

						//bani
						if ( ((hit->spawnflags & AXIS_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_ALLIES)) ||
							 ((hit->spawnflags & ALLIED_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_AXIS)) ) {
							enemyObj = qtrue;
						}
					}
				}

				//bani
				if( friendlyObj && !enemyObj ) {
					G_FreeEntity( traceEnt );
					trap_SendServerCommand( ent-g_entities, "cp \"You cannot arm dynamite near a friendly objective!\" 1");
					return;
				}

				if ( traceEnt->health >= 250 ) {
					traceEnt->health = 255;
				} else {
					return;
				}

				// Don't allow disarming for sec (so guy that WAS arming doesn't start disarming it!
				traceEnt->timestamp = level.time + 1000;
				traceEnt->health = 5;

				// set teamnum so we can check it for drop/defuse exploit
				traceEnt->s.teamNum = ent->client->sess.sessionTeam;
				// For dynamic light pulsing
				traceEnt->s.effect1Time = level.time;

				// ARM IT!
				traceEnt->nextthink = level.time + 30000;
				traceEnt->think = G_ExplodeMissile;

				// Gordon: moved down here to prevent two prints when dynamite IS near objective

				trap_SendServerCommand( ent-g_entities, "cp \"Dynamite is now armed with a 30 second timer!\" 1");

				// check if player is in trigger objective field
				// NERVE - SMF - made this the actual bounding box of dynamite instead of range, also must snap origin to line up properly
				VectorCopy( traceEnt->r.currentOrigin, origin );
				SnapVector( origin );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );
				num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

				for ( i=0 ; i<num ; i++ ) {
					hit = &g_entities[touch[i]];

					if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
						continue;
					}
					if ((hit->s.eType == ET_OID_TRIGGER)) {

						if ( !(hit->spawnflags & (AXIS_OBJECTIVE|ALLIED_OBJECTIVE)) )
							continue;

						// Arnout - only if it targets a func_explosive
						if( hit->target_ent && Q_stricmp( hit->target_ent->classname, "func_explosive" ) )
							continue;

						if ( hit->spawnflags & AXIS_OBJECTIVE ) {
							if (ent->client->sess.sessionTeam == TEAM_ALLIES) { // transfer score info if this is a bomb scoring objective
								traceEnt->accuracy = hit->accuracy;
							}
						} else if (hit->spawnflags & ALLIED_OBJECTIVE) {
							if (ent->client->sess.sessionTeam == TEAM_AXIS) { // ditto other team
								traceEnt->accuracy = hit->accuracy;
							}
						}

						// rain - spawnflags 128 = disabled (#309)
						if (!(hit->spawnflags & 128) && (((hit->spawnflags & AXIS_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_ALLIES)) ||
							 ((hit->spawnflags & ALLIED_OBJECTIVE) && (ent->client->sess.sessionTeam == TEAM_AXIS))) ) {

							gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
							pm->s.effect2Time = 0;
							pm->s.effect3Time = hit->s.teamNum;
							pm->s.teamNum = ent->client->sess.sessionTeam;

							G_Script_ScriptEvent( hit, "dynamited", "" );

							if ( !(hit->spawnflags & OBJECTIVE_DESTROYED) ) {
								AddScore(traceEnt->parent, WOLF_DYNAMITE_PLANT); // give drop score to guy who dropped it
								if(traceEnt->parent && traceEnt->parent->client) {
									G_LogPrintf("Dynamite_Plant: %d\n", traceEnt->parent - g_entities);	// OSP
								}
								traceEnt->parent = ent; // give explode score to guy who armed it
							}
							//bani - fix #238
							traceEnt->etpro_misc_1 |= 1;
						}
//bani
//						i = num;
						return;	//bani - bail out here because primary obj's take precendence over constructibles
					}		
				}

//bani - reordered this check so its AFTER the primary obj check
				// Arnout - first see if the dynamite is planted near a constructable object that can be destroyed
				{
					int		entityList[MAX_GENTITIES];
					int		numListedEntities;
					int		e;
					vec3_t  org;

					VectorCopy( traceEnt->r.currentOrigin, org );
					org[2] += 4;	// move out of ground

					G_TempTraceIgnorePlayersAndBodies();
					numListedEntities = EntsThatRadiusCanDamage( org, traceEnt->splashRadius, entityList );
					G_ResetTempTraceIgnoreEnts();

					for( e = 0; e < numListedEntities; e++ ) {
						hit = &g_entities[entityList[ e ]];

						if( hit->s.eType != ET_CONSTRUCTIBLE )
							continue;

						// invulnerable
						if( hit->spawnflags & CONSTRUCTIBLE_INVULNERABLE )
							continue;

						if( !G_ConstructionIsPartlyBuilt( hit ) ) {
							continue;
						}

						// is it a friendly constructible
						if( hit->s.teamNum == traceEnt->s.teamNum ) {
//bani - er, didnt we just pass this check earlier?
//							G_FreeEntity( traceEnt );
//							trap_SendServerCommand( ent-g_entities, "cp \"You cannot arm dynamite near a friendly construction!\" 1");
//							return;
							continue;
						}

						// not dynamite-able
						if( hit->constructibleStats.weaponclass < 1 ) {
							continue;
						}

						if( hit->parent ) {
							gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
							pm->s.effect2Time = 0; // 0 = planted
							pm->s.effect3Time = hit->parent->s.teamNum;
							pm->s.teamNum = ent->client->sess.sessionTeam;

							G_Script_ScriptEvent( hit, "dynamited", "" );
	
							if( (!(hit->parent->spawnflags & OBJECTIVE_DESTROYED)) && 
								hit->s.teamNum && (hit->s.teamNum == ent->client->sess.sessionTeam) ) {	// ==, as it's inverse
								AddScore(traceEnt->parent, WOLF_DYNAMITE_PLANT); // give drop score to guy who dropped it
								if( traceEnt->parent && traceEnt->parent->client ) {
									G_LogPrintf("Dynamite_Plant: %d\n", traceEnt->parent - g_entities);	// OSP
								}
								traceEnt->parent = ent; // give explode score to guy who armed it
							}
							//bani - fix #238
							traceEnt->etpro_misc_1 |= 1;
						}
						return;
					}
				}
			} 
			else 
			{
				if (traceEnt->timestamp > level.time)
					return;
				if (traceEnt->health >= 248) // have to do this so we don't score multiple times
					return;
				dynamiteDropTeam = traceEnt->s.teamNum; // set this here since we wack traceent later but want teamnum for scoring
				
				traceEnt->health += disarmSpeed_Dynamite[engineerLevel];
				/*
				if( ent->client->sess.skill[SK_EXPLOSIVES_AND_CONSTRUCTION] >= 2 )
					traceEnt->health += 6;
				else
					traceEnt->health += 3;
				*/

				G_PrintClientSpammyCenterPrint(ent-g_entities, "Defusing dynamite...");

				if ( traceEnt->health >= 248 ) {
//bani
					qboolean defusedObj = qfalse;

					traceEnt->health = 255;
					// Need some kind of event/announcement here

//					Add_Ammo( ent, WP_DYNAMITE, 1, qtrue );

					traceEnt->think = G_FreeEntity;
					traceEnt->nextthink = level.time + FRAMETIME;

					VectorCopy( traceEnt->r.currentOrigin, origin );
					SnapVector( origin );
					VectorAdd( origin, traceEnt->r.mins, mins );
					VectorAdd( origin, traceEnt->r.maxs, maxs );
					num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

					// don't report if not disarming *enemy* dynamite in field
/*					if (dynamiteDropTeam == ent->client->sess.sessionTeam)
						return;*/

					//bani - eh, why was this commented out? it makes sense, and prevents a sploit.
					if (dynamiteDropTeam == ent->client->sess.sessionTeam)
						return;

					for ( i=0 ; i<num ; i++ ) {
						hit = &g_entities[touch[i]];

						if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
							continue;
						}
						if ((hit->s.eType == ET_OID_TRIGGER)) {

							if ( !(hit->spawnflags & (AXIS_OBJECTIVE | ALLIED_OBJECTIVE)) )
								continue;

							// rain - spawnflags 128 = disabled (#309)
							if (hit->spawnflags & 128)
								continue;

							//bani - prevent plant/defuse exploit near a/h cabinets or non-destroyable locations (bank doors on goldrush)
							if( !hit->target_ent || hit->target_ent->s.eType != ET_EXPLOSIVE ) {
								continue;
							}

							if (ent->client->sess.sessionTeam == TEAM_AXIS) {
								if ((hit->spawnflags & AXIS_OBJECTIVE) && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DebugAddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++;
								}
								if(hit->target_ent) {
									G_Script_ScriptEvent( hit->target_ent, "defused", "" );
								}

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

//								trap_SendServerCommand(-1, "cp \"Axis engineer disarmed the Dynamite!\n\"");
								//bani
								defusedObj = qtrue;
							} else { // TEAM_ALLIES
								if ((hit->spawnflags & ALLIED_OBJECTIVE) && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DebugAddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++; 
									hit->spawnflags &= ~OBJECTIVE_DESTROYED; // "re-activate" objective since it wasn't destroyed
								}
								if(hit->target_ent) {
									G_Script_ScriptEvent( hit->target_ent, "defused", "" );
								}

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

//								trap_SendServerCommand(-1, "cp \"Allied engineer disarmed the Dynamite!\n\"");
								//bani
								defusedObj = qtrue;
							}
						}
					}
//bani - prevent multiple messages here
					if( defusedObj )
						return;

//bani - reordered this check so its AFTER the primary obj check
					// Gordon - first see if the dynamite was planted near a constructable object that would have been destroyed
					{
						int		entityList[MAX_GENTITIES];
						int		numListedEntities;
						int		e;
						vec3_t  org;

						VectorCopy( traceEnt->r.currentOrigin, org );
						org[2] += 4;	// move out of ground

						numListedEntities = EntsThatRadiusCanDamage( org, traceEnt->splashRadius, entityList );

						for( e = 0; e < numListedEntities; e++ ) {
							hit = &g_entities[entityList[ e ]];

							if( hit->s.eType != ET_CONSTRUCTIBLE )
								continue;

							// not completely build yet - NOTE: don't do this, in case someone places dynamite before construction is complete
							//if( hit->s.angles2[0] < 255 )
							//	continue;

							// invulnerable
							if( hit->spawnflags & CONSTRUCTIBLE_INVULNERABLE )
								continue;

							// not dynamite-able
							if( hit->constructibleStats.weaponclass < 1 ) {
								continue;
							}

							// we got somthing to destroy
							if (ent->client->sess.sessionTeam == TEAM_AXIS) {
								if ( hit->s.teamNum == TEAM_AXIS && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									if(ent && ent->client) G_LogPrintf("Dynamite_Diffuse: %d\n", ent - g_entities);	// OSP
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DebugAddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++;
								}
								G_Script_ScriptEvent( hit, "defused", "" );

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->parent->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

//								trap_SendServerCommand(-1, "cp \"Axis engineer disarmed the Dynamite!\" 2");
							} else { // TEAM_ALLIES
								if ( hit->s.teamNum == TEAM_ALLIES && (!scored)) {
									AddScore(ent,WOLF_DYNAMITE_DIFFUSE);
									if(ent && ent->client) G_LogPrintf("Dynamite_Diffuse: %d\n", ent - g_entities);	// OSP
									G_AddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f );
									G_DebugAddSkillPoints( ent, SK_EXPLOSIVES_AND_CONSTRUCTION, 6.f, "defusing enemy dynamite" );
									scored++; 
								}
								G_Script_ScriptEvent( hit, "defused", "" );

								{
									gentity_t* pm = G_PopupMessage( PM_DYNAMITE );
									pm->s.effect2Time = 1; // 1 = defused
									pm->s.effect3Time = hit->parent->s.teamNum;
									pm->s.teamNum = ent->client->sess.sessionTeam;
								}

//								trap_SendServerCommand(-1, "cp \"Allied engineer disarmed the Dynamite!\" 2");
							}

							return;
						}
					}
				}
	// jpw
			}
		}
	}
}

