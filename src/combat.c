/*
 *  QWProgs-DM
 *  Copyright (C) 2004  [sd] angel
 *
 *  This code is based on QuakeWorld DM mod code by Id Software, Inc.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  $Id$
 */

#include "g_local.h"

void ClientObituary(gedict_t *e1, gedict_t *e2);
void BotPlayerKilledEvent(gedict_t *targ, gedict_t *attacker, gedict_t *inflictor);
void bloodfest_killed_hook(gedict_t *killed, gedict_t *attacker);

#define DEATHTYPE( _dt_, _dt_str_ ) #_dt_str_,
char *deathtype_strings[] =
{
#include "deathtype.h"
};
#undef DEATHTYPE

const int deathtype_strings_cnt = sizeof(deathtype_strings) / sizeof(deathtype_strings[0]);

char* death_type(deathType_t dt)
{
	return deathtype_strings[(int)bound(0, dt, deathtype_strings_cnt - 1)];
}

//============================================================================

qbool ISLIVE(gedict_t *e)
{
	if (!e)
	{
		return false;
	}

	if (e->ct == ctPlayer)
	{
		return ((e->s.v.health > 0) && e->ca_alive);
	}

	return (e->s.v.health > 0);
}

qbool ISDEAD(gedict_t *e)
{
	return !ISLIVE(e);
}

//============================================================================

/*
 ============
 CanDamage

 Returns true if the inflictor can directly damage the target.  Used for
 explosions and melee attacks.
 ============
 */
qbool CanDamage(gedict_t *targ, gedict_t *inflictor)
{
	vec3_t dif;

// bmodels need special checking because their origin is 0,0,0
	if (targ->s.v.movetype == MOVETYPE_PUSH)
	{
		traceline(PASSVEC3(inflictor->s.v.origin),
					0.5 * (targ->s.v.absmin[0] + targ->s.v.absmax[0]),
					0.5 * (targ->s.v.absmin[1] + targ->s.v.absmax[1]),
					0.5 * (targ->s.v.absmin[2] + targ->s.v.absmax[2]), true, self);

		if (g_globalvars.trace_fraction == 1)
		{
			return true;
		}

		if (PROG_TO_EDICT(g_globalvars.trace_ent) == targ)
		{
			return true;
		}

		return false;
	}

	traceline(PASSVEC3(inflictor->s.v.origin), PASSVEC3(targ->s.v.origin), true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

// 1998-09-16 CanDamage fix by Maddes/Kryten start

// testing middle of half-size bounding box
	dif[2] = 0;

	// ...front right
	dif[1] = targ->s.v.maxs[1] * 0.5;
	dif[0] = targ->s.v.maxs[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...front left
	dif[0] = targ->s.v.mins[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...back left
	dif[1] = targ->s.v.mins[1] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...back right
	dif[0] = targ->s.v.maxs[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// testing top of half-sized bounding box
	dif[2] = targ->s.v.maxs[2] * 0.5;

	// ...front right
	dif[1] = targ->s.v.maxs[1] * 0.5;
	dif[0] = targ->s.v.maxs[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...front left
	dif[0] = targ->s.v.mins[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...back left
	dif[1] = targ->s.v.mins[1] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...back right
	dif[0] = targ->s.v.maxs[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// testing bottom of half-sized bounding box
	dif[2] = targ->s.v.mins[2] * 0.5;

	// ...front right
	dif[1] = targ->s.v.maxs[1] * 0.5;
	dif[0] = targ->s.v.maxs[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...front left
	dif[0] = targ->s.v.mins[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...back left
	dif[1] = targ->s.v.mins[1] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

	// ...back right
	dif[0] = targ->s.v.maxs[0] * 0.5;
	traceline(PASSVEC3(inflictor->s.v.origin), targ->s.v.origin[0] + dif[0],
				targ->s.v.origin[1] + dif[1], targ->s.v.origin[2] + dif[2], true, self);
	if (g_globalvars.trace_fraction == 1)
	{
		return true;
	}

// 1998-09-16 CanDamage fix by Maddes/Kryten end

	return false;
}


/*
 ============
 Killed
 ============
 */
void Killed(gedict_t *targ, gedict_t *attacker, gedict_t *inflictor)
{
	gedict_t *oself;

#ifdef BOT_SUPPORT
	BotPlayerKilledEvent(targ, attacker, inflictor);
#endif

	if (lgc_enabled())
	{
		lgc_register_kill(attacker);
	}

	oself = self;
	self = targ;

	if (self->antilag_data != NULL)
	{
		if (time_corrected >= self->teleport_time)
		{
			antilag_unmove_specific(self);
		}
	}

	if (self->s.v.health < -99)
	{
		self->s.v.health = -99;	// don't let sbar look bad if a player
	}

	self->dead_time = g_globalvars.time;

	if (self->ct == ctPlayer)
	{
		; // empty
	}
	else if ((self->s.v.movetype == MOVETYPE_PUSH) || (self->s.v.movetype == MOVETYPE_NONE))
	{			// doors, triggers, etc
		if (self->th_die)
		{
			self->th_die();
		}

		self = oself;

		return;
	}

	self->s.v.enemy = EDICT_TO_PROG(attacker);

	// bump the monster counter
	if (((int)(self->s.v.flags)) & FL_MONSTER)
	{
		float resp_time = bound(0, cvar("k_monster_spawn_time"), 999999);

		// for nightmare mode
		self->monster_desired_spawn_time = (
				resp_time ? g_globalvars.time + resp_time + resp_time * g_random() * 0.5 : 0);

		g_globalvars.killed_monsters++;
		WriteByte( MSG_ALL, SVC_KILLEDMONSTER);

		// in coop, killing a monster gives you a frag
		if (coop)
		{
			if (attacker->ct == ctPlayer)
			{
				attacker->s.v.frags++;
			}
		}
	}

	ClientObituary(self, attacker);

	self->s.v.takedamage = DAMAGE_NO;
	self->touch = (func_t) SUB_Null;
	self->s.v.effects = 0;
	if (self->haspack && g_globalvars.time - self->pack_pickup_time > cvar("k_packwarranty"))
	{
		self->haspack = false;
		G_cp2all("%s lost the pack!", self->netname);
	}

	monster_death_use();

	if (self->th_die)
	{
		self->th_die();
	}

	self = oself;

	// KTEAMS: check if sudden death is the case
	if (!isHoonyModeAny())
	{
		Check_SD(targ);

		// check fraglimit
		if (fraglimit
				&& (((targ->s.v.frags >= fraglimit) && (targ->ct == ctPlayer))
						|| ((attacker->s.v.frags >= fraglimit) && (attacker->ct == ctPlayer))))
		{
			EndMatch(0);
		}

		if (fraglimit && isTeam())
		{
			// in team modes fraglimit can be a sum of frags of all players in the team
			if ((get_scores1() >= fraglimit) || (get_scores2() >= fraglimit)
					|| (get_scores3() >= fraglimit))
			{
				EndMatch(0);
			}
		}
	}

	if (k_bloodfest)
	{
		bloodfest_killed_hook(targ, attacker);
	}
}

#ifndef Q3_VM

// qvm have some bugs/round problem as i get from SD-Angel, so this trick

static float newceil(float f)
{
	return ceil(((int)(f * 1000.0)) / 1000.0);
}

#else

// native use lib ceil function

#define newceil ceil

#endif

// this was part of T_Damage(), but I split it, so less mess
void MidairDamageBonus(gedict_t *attacker, gedict_t *targ, float midheight)
{
	char *rank, *icon;
	rank = icon = "";

	attacker->ps.mid_total++;

	if (midheight > 1024)
	{
		attacker->ps.mid_platinum++;
		attacker->s.v.frags += 8;
		rank = "platinum";
		icon = "\211";
	}
	else if (midheight > 512)
	{
		attacker->ps.mid_gold++;
		attacker->s.v.frags += 4;
		rank = "gold";
		icon = "\210";
	}
	else if (midheight > 256)
	{
		attacker->ps.mid_silver++;
		attacker->s.v.frags += 2;
		rank = "silver";
		icon = "\204";
	}
	else
	{
		attacker->ps.mid_bronze++;
		attacker->s.v.frags += 1;
		rank = "bronze";
		icon = "\213";
	}

	G_bprint(2, "%s %-15s \133%s\135 by %s on %s\n", icon, dig3s("%s midair", rank),
				dig3s("%7.1f", midheight), attacker->netname, targ->netname);

	if (attacker->ps.mid_total > 1)
	{
		attacker->ps.mid_totalheight += midheight;
		attacker->ps.mid_avgheight = attacker->ps.mid_totalheight / attacker->ps.mid_total;
	}
	else
	{
		attacker->ps.mid_totalheight = attacker->ps.mid_avgheight = midheight;
	}

	if (attacker->ps.mid_maxheight < midheight)
	{
		attacker->ps.mid_maxheight = midheight;
	}
}

/*
 ============
 T_Damage

 The damage is coming from inflictor, but get mad at attacker
 This should be the only function that ever reduces health.
 ============
 */

gedict_t *damage_attacker, *damage_inflictor;

static int dmg_is_splash = 0;

void T_Damage(gedict_t *targ, gedict_t *inflictor, gedict_t *attacker, float damage)
{
	vec3_t dir;
	gedict_t *oldself;
	float save;
	float take;
	int i, c1 = 8, c2 = 4, hdp;
	float dmg_dealt = 0, virtual_take = 0, unbound_dmg_dealt = 0;
	float non_hdp_damage; // save damage before handicap apply for kickback calculation
	float native_damage = damage; // save damage before apply any modificator
	float smashKBmult = 1;
	char *attackerteam, *targteam, *attackername, *victimname;
	qbool tp4teamdmg = false;

	//midair and instagib
	float playerheight = 0, midheight = 0;
	qbool midair = false, inwater = false, do_dmg = false, rl_dmg = false, stomp_dmg = false;

	// used by buttons and triggers to set activator for target firing
	damage_attacker = attacker;
	damage_inflictor = inflictor;

	attackerteam = getteam(attacker);
	targteam = getteam(targ);

	// can't apply damage to dead
	if (!targ->s.v.takedamage || ISDEAD(targ))
	{
		if (isRA() && !targ->s.v.takedamage && (dtTELE1 == targ->deathtype || dtTELE2 == targ->deathtype || dtTELE3 == targ->deathtype || dtSUICIDE == targ->deathtype))
		{
			targ->s.v.takedamage = true;
		}
		else
		{
			return;
		}
	}

	// don't bounce around players in prewar who wish to be left alone
	if (match_in_progress != 2 && targ->leavemealone)
	{
		if (attacker != targ && ((targ->ct == ctPlayer) && (attacker->ct == ctPlayer)))
		{
			return;
		}
		else if (dtTELE1 == targ->deathtype	// always do tele damage
				|| dtTELE2 == targ->deathtype	// always do tele damage
				|| dtTELE3 == targ->deathtype)	// always do tele damage
		{
			// telefrags still work, to avoid getting stuck
		}
	}

	// can't damage other players in race
	if (isRACE() && (attacker != targ))
	{
		if ((targ->ct == ctPlayer) && (attacker->ct == ctPlayer))
		{
			return;
		}
	}

	if (cvar("k_smashmode"))
	{
		if (dtWATER_DMG == targ->deathtype || dtFALL == targ->deathtype) // No drowning or fall damage
		{
			return;
		}
		
	}
	
	if (isCA())
	{
		if (dtWATER_DMG == targ->deathtype		// No drowning in CA
				|| dtFALL == targ->deathtype)	// No fall damage in CA
		{
			return;
		}
		// ignore almost all damage in CA while coutdown
		else if (match_in_progress && (ra_match_fight != 2))
		{
			if (!(dtTELE1 == targ->deathtype	// always do tele damage
				|| dtTELE2 == targ->deathtype	// always do tele damage
				|| dtTELE3 == targ->deathtype	// always do tele damage
				|| dtSUICIDE == targ->deathtype // do suicide damage anyway
				|| (dtTRIGGER_HURT == targ->deathtype && cvar("k_smashmode")))) // and if we're in smash mode, do trigger_hurt damage too
			{
				return;
			}
		}
		
		// don't accept any damage in CA modes if no_pain is true 
		if ((targ->no_pain || (attacker->no_pain && attacker->in_play)) && (match_in_progress == 2))
		{	
			if (attacker == targ)
			{
				tp4teamdmg = true; // don't take damage but still get stopped/bounced by weapon fire
			}
			else 
			{
				if (targ->in_play && (targ->invincible_sound < g_globalvars.time) && strneq(targteam, attackerteam))
				{
					sound(targ, CHAN_AUTO, "items/protect3.wav", 0.75, ATTN_NORM);
					targ->invincible_sound = g_globalvars.time + 2;
				}
	
				return;
			}
		}
	}

	if ((int)cvar("k_midair"))
	{
		midair = true;
	}

	// in bloodfest boss damage factor.
	if (k_bloodfest && attacker->bloodfest_boss)
	{
		damage *= 4;
	}

	// check for quad damage powerup on the attacker
	// midair quad makes rockets fast, but no change to damage
	if ((attacker->super_damage_finished > g_globalvars.time) && strneq(inflictor->classname, "door")
			&& (dtSTOMP != targ->deathtype) && !midair)
	{
		// in dmm4 quad is octa actually, unless tot_mode_enabled(),
		// then it's possible to set custom multiplier
		damage *= (deathmatch != 4 ? 4 : tot_mode_enabled() ? FrogbotQuadMultiplier() : 8);
	}

	// ctf strength rune
	if (attacker->ctf_flag & CTF_RUNE_STR)
	{
		damage *= (cvar("k_ctf_rune_power_str") / 2) + 1;
	}

	// ctf resistance rune
	if (targ->ctf_flag & CTF_RUNE_RES)
	{
		damage /= (cvar("k_ctf_rune_power_res") / 2) + 1;
		ResistanceSound(targ);
	}

	// did we hurt enemy flag carrier?
	if ((targ->ctf_flag & CTF_FLAG) && (!streq(targteam, attackerteam)))
	{
		attacker->carrier_hurt_time = g_globalvars.time;
	}

	// in teamplay 4 we do no armor or health damage to teammates (unless telefrag), but do apply velocity changes
	if (tp_num()
			== 4&& streq(targteam, attackerteam) && ( isCA() || cvar("k_smashmode") || targ != attacker ) && !TELEDEATH(targ))
	{
		tp4teamdmg = true;
	}

	if (midair || cvar("k_instagib"))
	{
		traceline(PASSVEC3(targ->s.v.origin), targ->s.v.origin[0], targ->s.v.origin[1],
					targ->s.v.origin[2] - 2048, true, targ);

		playerheight = targ->s.v.absmin[2] - g_globalvars.trace_endpos[2]
				+ (cvar("k_instagib") ? 1 : 0);
	}

	// get some data before apply damage in mid air mode
	if (midair)
	{
		inwater = (((int)targ->s.v.flags & FL_INWATER) && targ->s.v.waterlevel > 1);

		if (streq(inflictor->classname, "rocket"))
		{
			midheight = targ->s.v.origin[2] - inflictor->s.v.oldorigin[2];
		}

		rl_dmg = (targ->ct == ctPlayer && dtRL == targ->deathtype);
		stomp_dmg = (targ->ct == ctPlayer && dtSTOMP == targ->deathtype);

		if (!rl_dmg)
		{
			// damage types which ignore "lowheight"
			do_dmg = targ->ct != ctPlayer	// always do damage to non player, secret doors etc...
			|| dtWATER_DMG == targ->deathtype	// always do water damage
			|| dtLAVA_DMG == targ->deathtype	// always do lava damage
			|| dtSLIME_DMG == targ->deathtype	// always do slime damage
			|| dtSTOMP == targ->deathtype	// always do stomp damage
			|| dtTELE1 == targ->deathtype	// always do tele damage
			|| dtTELE2 == targ->deathtype	// always do tele damage
			|| dtTELE3 == targ->deathtype	// always do tele damage
			|| dtSUICIDE == targ->deathtype; // do suicide damage anyway
		}
	}

	non_hdp_damage = damage; // save damage before handicap apply for kickback calculation

	// #handicap#
	if (attacker != targ) // attack no self
	{
		if ((attacker->ct == ctPlayer) && (targ->ct == ctPlayer)) // player vs player
		{
			if ((hdp = GetHandicap(attacker)) != 100) // skip checks if hdp == 100
			{
				if ((dtAXE == targ->deathtype) || (dtSG == targ->deathtype) || (dtSSG == targ->deathtype)
						|| (dtNG == targ->deathtype) || (dtSNG == targ->deathtype)
						|| (dtGL == targ->deathtype) || (dtRL == targ->deathtype)
						|| (dtLG_BEAM == targ->deathtype) || (dtLG_DIS == targ->deathtype)
						|| (dtLG_DIS_SELF == targ->deathtype)) // even that impossible
				{
					damage *= 0.01f * hdp;
				}
			}
		}
	}

	// save damage based on the target's armor level

	save = newceil(targ->s.v.armortype * damage);

	if (tp4teamdmg || cvar("k_smashmode"))
	{
		save = 0; // we do not touch armor
	}

	if (save >= targ->s.v.armorvalue)
	{
		save = targ->s.v.armorvalue;
		targ->s.v.armortype = 0;	// lost all armor
		targ->s.v.items -= ((int)targ->s.v.items & ( IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3));
	}

	dmg_dealt += save;

	if (match_in_progress == 2)
	{
		targ->s.v.armorvalue = targ->s.v.armorvalue - save;
	}

	take = newceil(damage - save);

	// mid air damage modificators
	if (midair)
	{
		int k_midair_minheight, midair_minheight;

		k_midair_minheight = (int)cvar("k_midair_minheight");

		if (k_midair_minheight == 1)
		{
			midair_minheight = 128;
		}
		else if (k_midair_minheight == 2)
		{
			midair_minheight = 256;
		}
		else if (k_midair_minheight == 3)
		{
			midair_minheight = 512;
		}
		else if (k_midair_minheight == 4)
		{
			midair_minheight = 1024;
		}
		else
		{
			midair_minheight = 64;
		}

		if (rl_dmg || stomp_dmg)
		{
			take = 9999;
		}

		if ((playerheight < midair_minheight) && rl_dmg)
		{
			take = 0; // no dmg done if target is not high enough
		}

		if ((playerheight < 45) && !inwater && rl_dmg)
		{
			take = 0; // no rl dmg in such case
		}

		if (!rl_dmg && !do_dmg)
		{
			take = 0; // unknown damage for midair, so do not damage
		}

		if (rl_dmg && (targ == attacker))
		{
			take = 0; // no self rl damage
		}
	}

	// instagib damage modificators
	if (cvar("k_instagib"))
	{
		if (inflictor->ct == ctPlayer)
		{
			take = 5000;
		}

		if (attacker == targ)
		{
			take = 0;
		}
	}

	// helps kill player in prewar at "wrong" places
	if ((match_in_progress != 2) && (native_damage > 450))
	{
		take = 99999;
	}

	// team play damage avoidance and godmode or invincibility check

	virtual_take = max(0, take); // virtual_take used for calculating dmg_dealt only in case of k_dmgfrags

	// ignore this checks for suicide damage
	if (dtSUICIDE != targ->deathtype)
	{
		if ((int)targ->s.v.flags & FL_GODMODE)
		{
			take = 0; // what if god was one of us
		}
		else if (targ->invincible_finished >= g_globalvars.time)
		{
			if (targ->invincible_sound < g_globalvars.time)
			{
				sound(targ, CHAN_AUTO, "items/protect3.wav", 1, ATTN_NORM);
				targ->invincible_sound = g_globalvars.time + 2;
			}

			take = 0;
		}
		else if (((tp_num() == 1) || ((tp_num() == 3) && (targ != attacker)))
				&& !strnull(attackerteam) && streq(targteam, attackerteam)
				&& (attacker->ct == ctPlayer)
				&& strneq( inflictor->classname, "door")
				&& !TELEDEATH(targ)) // do telefrag damage in tp
		{
			// teamplay == 1 don't damage self and mates (armor affected anyway)
			// teamplay == 3 don't damage mates, do damage to self (armor affected anyway)

			take = 0;
		}
		else if (tp4teamdmg)
		{
			take = 0; // we do not touch health	   	
		}
	}

	take = max(0, take); // avoid negative take, if any

	unbound_dmg_dealt = dmg_dealt;
	if (cvar("k_dmgfrags"))
	{
		if (TELEDEATH(targ))
		{
			// tele doesn't count for any dmgfrags damage
			dmg_dealt = 0;
		}
		else if (targ->invincible_finished >= g_globalvars.time)
		{
			// damage dealt _not_ capped by victim's health if victim has pent
			unbound_dmg_dealt += virtual_take;
			dmg_dealt += virtual_take;
		}
		else
		{
			// damage dealt capped by victim's health
			unbound_dmg_dealt += virtual_take;
			dmg_dealt += bound(0, virtual_take, targ->s.v.health);
		}
	}
	else if (cvar("k_smashmode"))
	{
 		if (!(dtTRIGGER_HURT == targ->deathtype || dtTELE1 == targ->deathtype || dtTELE2 == targ->deathtype || dtTELE3 == targ->deathtype))
		{
			take = 0; // smashmode blocks health damage unless it's a telefrag or hurt trigger.
			dmg_dealt += native_damage;
		}
	}
	else
	{
		// damage dealt capped by victim's health
		unbound_dmg_dealt += virtual_take;
		dmg_dealt += bound(0, take, targ->s.v.health);
	}

	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// FIXME: remove after combining shotgun blasts?
	if (targ->ct == ctPlayer)
	{
		targ->s.v.dmg_take += take;
		targ->s.v.dmg_save += save;
		targ->s.v.dmg_inflictor = EDICT_TO_PROG(inflictor);
	}

	unbound_dmg_dealt = bound(0, unbound_dmg_dealt, 9999);
	if ((sv_extensions & SV_EXTENSIONS_MVDHIDDEN) && ((int)unbound_dmg_dealt > 0)
			&& ((attacker->ct == ctPlayer) || (targ->ct == ctPlayer)))
	{
		int damage_flags = (dmg_is_splash ? MVDHIDDEN_DMGDONE_SPLASHDAMAGE : 0);
		// MVD damage: always send
		WriteShort(MSG_MULTICAST, mvdhidden_dmgdone);
		WriteShort(MSG_MULTICAST, damage_flags | targ->deathtype);
		WriteShort(MSG_MULTICAST, NUM_FOR_EDICT(attacker));
		WriteShort(MSG_MULTICAST, NUM_FOR_EDICT(targ));
		WriteShort(MSG_MULTICAST, (short)unbound_dmg_dealt);
		trap_multicast(PASSVEC3(targ->s.v.origin), MULTICAST_MVD_HIDDEN);
	}

	// Only send to clients during standby
	if (!match_in_progress && ((int)unbound_dmg_dealt > 0) && (attacker->ct == ctPlayer)
			&& (targ->ct == ctPlayer))
	{
		int di = atoi(ezinfokey(attacker, "di"));

		if (di && ((di == 2) || (attacker != targ)))
		{
			qbool has_eyes = ((int)targ->s.v.items) & IT_INVISIBILITY;

			stuffcmd_flags(attacker, STUFFCMD_IGNOREINDEMO, "//ktx di %d %d %d %d %d %d %d\n",
							PASSINTVEC3(targ->s.v.origin), targ->deathtype,
							(short)unbound_dmg_dealt, dmg_is_splash ? 1 : 0,
							SameTeam(attacker, targ) && !has_eyes ? 1 : 0);
		}
	}

	if (save)
	{
		if ((streq(inflictor->classname, "worldspawn") || strnull(attacker->classname))
				|| (targ->deathtype == dtWATER_DMG) || (targ->deathtype == dtEXPLO_BOX)
				|| (targ->deathtype == dtFALL) || (targ->deathtype == dtSQUISH)
				|| (targ->deathtype == dtCHANGELEVEL) || (targ->deathtype == dtFIREBALL)
				|| (targ->deathtype == dtSLIME_DMG) || (targ->deathtype == dtLAVA_DMG)
				|| (targ->deathtype == dtTRIGGER_HURT))
		{
			attackername = "world";
		}
		else
		{
			attackername = attacker->netname;
		}
		victimname = targ->netname;

		log_printf("\t\t<event>\n"
					"\t\t\t<damage>\n"
					"\t\t\t\t<time>%f</time>\n"
					"\t\t\t\t<attacker>%s</attacker>\n"
					"\t\t\t\t<target>%s</target>\n"
					"\t\t\t\t<type>%s</type>\n"
					"\t\t\t\t<quad>%d</quad>\n"
					"\t\t\t\t<splash>%d</splash>\n"
					"\t\t\t\t<value>%d</value>\n"
					"\t\t\t\t<armor>1</armor>\n"
					"\t\t\t</damage>\n"
					"\t\t</event>\n",
					g_globalvars.time - match_start_time, cleantext(attackername),
					cleantext(victimname), death_type(targ->deathtype),
					(int)(attacker->super_damage_finished > g_globalvars.time ? 1 : 0),
					dmg_is_splash, (int)save);
	}

	// Core Smashmode mechanics
	if (cvar("k_smashmode"))
	{
		if (targ->invincible_finished <= g_globalvars.time && (!streq(attacker->classname, "trigger_hurt")))
		{
			if (attacker == targ && targ->haspack) // self damage to packman
				targ->s.v.armorvalue += native_damage / cvar("k_packselfdmgratio"); // only do the armor add
			
			if (attacker != targ)
			{
				if (!streq(targteam, attackerteam) || tp_num() == 0)
				{ // opposite teams, add % and give credit for kills
					hdp = GetHandicap(targ);
					targ->s.v.armorvalue += native_damage / (cvar("k_smashdmgratio") * (float)hdp / 100); //add to armor per original damage done
					if (attacker->s.v.health > 0) // if the attacker is still alive
						attacker->s.v.health = max(targ->s.v.armorvalue, 1); // set attacker's health to the target's armorvalue
					targ->last_deathtype = targ->deathtype;
					targ->last_attacker = attacker;
				}
				else // team attack.  don't apply damage, but do give credit if it kills.
					targ->last_attacker = attacker;
			}
		}
		
		//If target was grabbing the ledge, knock him off and block grab timer for 1s
		if (targ->is_grabbing)
		{
			targ->is_grabbing = false;
			targ->grab_time = g_globalvars.time + 1;
		}
	}
	
	// figure momentum add
	if ((inflictor != world)
			&& (targ->teleport_time < time_corrected || targ == attacker)
			&& ((targ->s.v.movetype == MOVETYPE_WALK)
					|| (k_bloodfest && ((int)targ->s.v.flags & FL_MONSTER))))
	{
		float nailkick;

		for (i = 0; i < 3; i++)
		{
			dir[i] = targ->s.v.origin[i]
					- (inflictor->s.v.absmin[i] + inflictor->s.v.absmax[i]) * 0.5;
		}

		VectorNormalize(dir);

		dir[2] =
				((dtLG_DIS_SELF == targ->deathtype || dtLG_DIS == targ->deathtype) && dir[2] < 0) ?
						-dir[2] : dir[2];

		if (midair && (non_hdp_damage < 60) && (attacker != targ))
		{
			c1 = 11;
			c2 = 6;
		}

		// Yawnmode: nails increases kickback
		// - Molgrum
		if (k_yawnmode && streq(inflictor->classname, "spike"))
		{
			nailkick = 1.2;
		}
		else
		{
			nailkick = 1.0;
		}

		if (cvar("k_smashmode") && match_in_progress == 2 && targ != attacker)
		{ // smashKBmult is initialized at 1, so assume it's still 1 if these conditions are not met.
			if (targ->invincible_finished >= g_globalvars.time)
				smashKBmult = 0;
			else if (dtLG_BEAM == targ->deathtype && ((int)targ->s.v.flags & FL_ONGROUND)) // target on ground vs LG
				smashKBmult = min(1.5, 0.5 + 0.0025 * targ->s.v.armorvalue); // .5 knockback at 0 armor, 1 knockback at 200 armor, caps at 1.5 knockback for 400 armor.
			else if (dtLG_BEAM == targ->deathtype)
				smashKBmult = min(0.85, 0.5 + 0.0025 * targ->s.v.armorvalue); // same linear scaling, but caps at 0.85 knockback (140 armor).
			else if (targ->s.v.armorvalue > 100)
				smashKBmult = targ->s.v.armorvalue*0.01;
		}
 
		for (i = 0; i < 3; i++)
		{
			targ->s.v.velocity[i] += dir[i] * non_hdp_damage * c1 * nailkick * smashKBmult
					* (midair && playerheight >= 45 ? (1 + (playerheight - 45) / 64) : 1);
		}

		if (midair && (playerheight < 45))
		{
			targ->s.v.velocity[2] += dir[2] * non_hdp_damage * c2 * nailkick; // only for z component
		}

		if (k_bloodfest && ((int)targ->s.v.flags & FL_MONSTER))
		{
			targ->s.v.flags = (int)targ->s.v.flags & ~FL_ONGROUND;
		}

#ifdef BOT_SUPPORT
		targ->fb.path_state |= AIR_ACCELERATION;
#endif
	}

#ifdef BOT_SUPPORT
	BotDamageInflictedEvent(attacker, targ);
#endif

	if ((match_in_progress == 2) && ((int)cvar("k_dmgfrags") || lgc_enabled()))
	{
		if ((attacker->ct == ctPlayer) && (targ->ct == ctPlayer) && (attacker != targ))
		{
			if (isDuel() || isFFA() || strneq(attackerteam, targteam))
			{
				int dmg_frags;

				attacker->ps.dmg_frags += dmg_dealt; // add dealt
				dmg_frags = attacker->ps.dmg_frags / 100; // 1 frag = 100 damage
				attacker->s.v.frags = (int)(attacker->s.v.frags + dmg_frags);
				attacker->ps.dmg_frags -= dmg_frags * 100;
			}
		}
	}

	// do the damage

	if ((match_in_progress == 2) || (dtSUICIDE == targ->deathtype) // do suicide damage anyway
			|| TELEDEATH(targ) || (k_practice && targ->ct != ctPlayer) // #practice mode#
			|| (take >= 99999)) // do such huge damage even in prewar, prewar because indirectly here match_in_progress != 2
	{
		targ->s.v.health -= take;

//		G_bprint( 2, "%s %f\n", targ->classname, targ->s.v.health );

		if (take)
		{
			if ((streq(inflictor->classname, "worldspawn") || strnull(attacker->classname))
					|| (targ->deathtype == dtWATER_DMG) || (targ->deathtype == dtEXPLO_BOX)
					|| (targ->deathtype == dtFALL) || (targ->deathtype == dtSQUISH)
					|| (targ->deathtype == dtCHANGELEVEL) || (targ->deathtype == dtFIREBALL)
					|| (targ->deathtype == dtSLIME_DMG) || (targ->deathtype == dtLAVA_DMG)
					|| (targ->deathtype == dtTRIGGER_HURT))
			{
				attackername = "world";
			}
			else
			{
				attackername = attacker->netname;
			}
			victimname = targ->netname;

			log_printf("\t\t<event>\n"
						"\t\t\t<damage>\n"
						"\t\t\t\t<time>%f</time>\n"
						"\t\t\t\t<attacker>%s</attacker>\n"
						"\t\t\t\t<target>%s</target>\n"
						"\t\t\t\t<type>%s</type>\n"
						"\t\t\t\t<quad>%d</quad>\n"
						"\t\t\t\t<splash>%d</splash>\n"
						"\t\t\t\t<value>%d</value>\n"
						"\t\t\t\t<armor>0</armor>\n"
						"\t\t\t</damage>\n"
						"\t\t</event>\n",
						g_globalvars.time - match_start_time, cleantext(attackername),
						cleantext(victimname), death_type(targ->deathtype),
						(int)(attacker->super_damage_finished > g_globalvars.time ? 1 : 0),
						dmg_is_splash, (int)take);
		}

		if (!targ->s.v.health || (dtSUICIDE == targ->deathtype))
		{
			targ->s.v.health = -1; // qqshka, no zero health, heh, imo less bugs after this
		}
	}

	// show damage in sbar
	if ((match_in_progress != 2) && ISLIVE(targ) && !k_matchLess)
	{
		if (!midair || ((int)targ->s.v.flags & FL_ONGROUND))
		{
			if (targ->ct == ctPlayer)
			{
				targ->s.v.currentammo = 1000 + Q_rint(damage);
			}

			if ((attacker != targ) && (attacker->ct == ctPlayer))
			{
				attacker->s.v.health = 1000 + Q_rint(damage);
			}
		}
	}

	// update damage stats like: give/taked/team damage
	if ((attacker->ct == ctPlayer) && (targ->ct == ctPlayer))
	{
		int weapon = wpNONE;

		switch (targ->deathtype)
		{
			case dtAXE:
				weapon = wpAXE;
				break;

			case dtSG:
				weapon = wpSG;
				break;

			case dtSSG:
				weapon = wpSSG;
				break;

			case dtNG:
				weapon = wpNG;
				break;

			case dtSNG:
				weapon = wpSNG;
				break;

			case dtGL:
				weapon = wpGL;
				break;

			case dtRL:
				weapon = wpRL;
				break;
			case dtLG_BEAM:
			case dtLG_DIS:
				weapon = wpLG;
				break;
		}

		if (attacker == targ)
		{
			// self damage
			attacker->ps.dmg_self += dmg_dealt;
		}
		else
		{
			int items = targ->s.v.items;

			if (tp_num() && streq(attackerteam, targteam))
			{
				attacker->ps.dmg_team += dmg_dealt;
				attacker->ps.wpn[weapon].tdamage += dmg_dealt;

				// damage to enemy weapon
				if (items & (IT_ROCKET_LAUNCHER | IT_LIGHTNING))
				{
					attacker->ps.dmg_tweapon += dmg_dealt;
				}
			}
			else
			{
				attacker->ps.dmg_g += dmg_dealt;
				targ->ps.dmg_t += dmg_dealt;
				attacker->ps.wpn[weapon].edamage += dmg_dealt;

				// damage to enemy weapon
				if (items & (IT_ROCKET_LAUNCHER | IT_LIGHTNING))
				{
					attacker->ps.dmg_eweapon += dmg_dealt;
				}
			}

			// real hits

			if (take || save)
			{
				if (dtRL == targ->deathtype)
				{
					attacker->ps.wpn[wpRL].rhits++;
				}

				if (dtGL == targ->deathtype)
				{
					attacker->ps.wpn[wpGL].rhits++;
				}
			}

			// virtual hits

			if (virtual_take || save)
			{
				if (dtRL == targ->deathtype)
				{
					attacker->ps.wpn[wpRL].vhits++;
					// virtual given rl damage
					attacker->ps.dmg_g_rl += (virtual_take + save);
				}

				if (dtGL == targ->deathtype)
				{
					attacker->ps.wpn[wpGL].vhits++;
				}
			}
		}
	}

	// mid air bonuses
	if (midair && (match_in_progress == 2) && (attacker != targ) && take && rl_dmg)
	{
		MidairDamageBonus(attacker, targ, midheight);
	}

	if (midair && (match_in_progress == 2) && stomp_dmg)
	{
		attacker->ps.mid_stomps++;
		targ->s.v.frags -= 3;
	}

#ifdef BOT_SUPPORT
	FrogbotSetHealthArmour(targ);
#endif

	// if targed killed, do appropriate action and return
	if (ISDEAD(targ))
	{
		Killed(targ, attacker, inflictor);

		return;
	}

	// react to the damage - call pain function
	oldself = self;
	self = targ;

	if ((int)self->s.v.flags & FL_MONSTER)
	{
		GetMadAtAttacker(attacker);
	}

	if (self->th_pain)
	{
		self->th_pain(attacker, take);
	}

	self = oldself;
}

void T_RadiusDamageApply(gedict_t *inflictor, gedict_t *attacker, gedict_t *head, float damage,
							deathType_t dtype)
{
	float points;
	vec3_t org;

	if (head->s.v.takedamage)
	{
		org[0] = inflictor->s.v.origin[0]
				- (head->s.v.origin[0] + (head->s.v.mins[0] + head->s.v.maxs[0]) * 0.5);
		org[1] = inflictor->s.v.origin[1]
				- (head->s.v.origin[1] + (head->s.v.mins[1] + head->s.v.maxs[1]) * 0.5);
		org[2] = inflictor->s.v.origin[2]
				- (head->s.v.origin[2] + (head->s.v.mins[2] + head->s.v.maxs[2]) * 0.5);
		points = 0.5 * vlen(org);

		if (points < 0)
		{
			points = 0;
		}

		points = damage - points;

		if (head == attacker)
		{
			points = points * 0.5;
		}
		// no out of water discharge damage if k_dis 2
		else if ((cvar("k_dis") == 2) && (dtLG_DIS == dtype) && !head->s.v.waterlevel)
		{
			points = 0;
		}

		if (points > 0)
		{
			if (CanDamage(head, inflictor))
			{
				head->deathtype = dtype;

				dmg_is_splash = 1; // mark damage as splash

				if (cvar("k_instagib") || isRACE()) // in instagib splash applied to inflictor only, for coil jump
				{
					if (head == attacker)
					{
						T_Damage(head, inflictor, attacker, points);
					}
				}
				else
				{
					// shamblers only take half damage from rocket/grenade explosions
					if (streq(head->classname, "monster_shambler") && !cvar("k_bloodfest"))
					{
						points = points / 2;
					}

					T_Damage(head, inflictor, attacker, points);
				}

				dmg_is_splash = 0; // unmark splash
			}
		}
	}
}

/*
 ============
 T_RadiusDamage
 ============
 */
void T_RadiusDamage(gedict_t *inflictor, gedict_t *attacker, float damage, gedict_t *ignore,
					deathType_t dtype)
{
	gedict_t *head;

	if (isRACE())
	{
		attacker->s.v.solid = SOLID_BBOX;
		T_RadiusDamageApply(inflictor, attacker, attacker, damage, dtype);
		attacker->s.v.solid = SOLID_NOT;

		return;
	}

	head = trap_findradius(world, inflictor->s.v.origin, damage + 40);

	while (head)
	{
		if (head != ignore)
		{
			T_RadiusDamageApply(inflictor, attacker, head, damage, dtype);
		}

		head = trap_findradius(head, inflictor->s.v.origin, damage + 40);
	}
}

void T_RadiusDamage_Ignore2(gedict_t *inflictor, gedict_t *attacker, float damage, gedict_t *ignore, gedict_t *ignore2,
	deathType_t dtype)
{

	gedict_t *head;
	head = trap_findradius(world, inflictor->s.v.origin, damage + 40);

	while (head)
	{
		if (head != ignore && head != ignore2)
		{
			T_RadiusDamageApply(inflictor, attacker, head, damage, dtype);
		}

		head = trap_findradius(head, inflictor->s.v.origin, damage + 40);
	}
}

/*
 ============
 T_BeamDamage
 ============
 */
void T_BeamDamage(gedict_t *attacker, float damage)
{
	vec3_t tmpv;
	float points;
	gedict_t *head;

	head = trap_findradius(world, attacker->s.v.origin, damage + 40);

	while (head)
	{
		if (head->s.v.takedamage)
		{
			VectorSubtract(attacker->s.v.origin, head->s.v.origin, tmpv)
			points = 0.5 * vlen(tmpv);
			if (points < 0)
			{
				points = 0;
			}

			points = damage - points;
			if (head == attacker)
			{
				points = points * 0.5;
			}

			if (points > 0)
			{
				if (CanDamage(head, attacker))
				{
					T_Damage(head, attacker, attacker, points);
				}
			}
		}

		head = trap_findradius(head, attacker->s.v.origin, damage + 40);
	}
}
