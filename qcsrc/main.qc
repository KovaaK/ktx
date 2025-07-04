


#define NENT_PROJECTILE		10
#define NENT_WEAPONPRED		20


string 	autocvar_r_rockettrail;
string 	autocvar_r_grenadetrail;
float	autocvar_r_rocketlight;
vector	autocvar_r_rocketlight_colour;
float	autocvar_cl_sproj_xerp;

int Projectile_FindTrail(int is_grenade)
{
	string cvar_to_use;
	int select;
	int trail_index = 0;
	
	if (is_grenade)
		cvar_to_use = autocvar_r_grenadetrail;
	else
		cvar_to_use = autocvar_r_rockettrail;
	select = stoi(cvar_to_use);
	
	switch (select)
	{
	case 0: // check for string, otherwise no trail
		if (cvar_to_use == "0")
		{
			trail_index = 0;
			break;
		}
		else
		{
			int effect = particleeffectnum(cvar_to_use);

			if (effect)
			{
				trail_index = effect;
				break;
			}
		}
		// fall through to default (so semicheat will work properly)
	case 1: // default model effect
	default:
		if (is_grenade)
			trail_index = particleeffectnum("TR_GRENADE");
		else
			trail_index = particleeffectnum("TR_ROCKET");
		break;
	case 2: // opposite effect
		if (is_grenade)
			trail_index = particleeffectnum("TR_ROCKET");
		else
			trail_index = particleeffectnum("TR_GRENADE");
		break;
	case 3: // alt rocket effect
		trail_index = particleeffectnum("TR_ALTROCKET");
		break;
	case 4: // gib
		trail_index = particleeffectnum("TR_BLOOD");
		break;
	case 5: // zombie gib
		trail_index = particleeffectnum("TR_SLIGHTBLOOD");
		break;
	case 6: // Scrag tracer
		trail_index = particleeffectnum("TR_WIZSPIKE");
		break;
	case 7: // Knight tracer
		trail_index = particleeffectnum("TR_KNIGHTSPIKE");
		break;
	case 8: // Vore tracer
		trail_index = particleeffectnum("TR_VORESPIKE");
		break;
	case 9: // rail trail
		trail_index = particleeffectnum("TE_RAILTRAIL");
		break;
	}
	
	return trail_index;
}


float Predraw_Projectile()
{
	float delta = time - self.s_time;
	if (autocvar_cl_sproj_xerp && self.ownnum != player_localentnum && self.ownnum != 0)
		delta += ping_time;
	
	self.origin = self.s_origin;
	self.origin += self.velocity * delta;
	setorigin(self, self.origin);
	
	// we need to space out the particles incase we're running at very high fps
	if (time > self.p_time)
	{
		if (self.p_time) // we want to draw at least 1 frame worth of trail, lest cl_sproj_xerp stop any trails from existing
		{
			traceline(self.oldorigin, self.origin, MOVE_NOMONSTERS, self);
			if (trace_fraction < 1)
			{
				self.oldorigin = trace_endpos;
				return PREDRAW_NEXT;
			}
		}
		
		if (self.traileffectnum)
			trailparticles(self.traileffectnum, self, self.oldorigin, self.origin);
		
		self.p_time = time + 0.013;
		self.oldorigin = self.origin;
	}
	
	if (self.modelflags & MF_ROCKET)
	{
		dynamiclight_add(self.origin, 200, autocvar_r_rocketlight_colour, 0);
	}
	
	return PREDRAW_AUTOADD;
}


void EntUpdate_Projectile(float is_new)
{
	int sendflags = ReadByte();
	
	if (sendflags & 1)
	{
		self.s_origin = [ReadCoord(), ReadCoord(), ReadCoord()];
		self.velocity = [ReadCoord(), ReadCoord(), ReadCoord()];
		self.s_time = ReadFloat();
	}
	
	
	if (sendflags & 2)
	{
		// Reki: note- we need to do this monstrosity of a setmodel because setmodelindex doesn't return modelflags
		setmodel(self, modelnameforindex(ReadShort()));	
		self.effects = ReadShort();
	}
	
	
	if (sendflags & 4)
	{
		self.angles = [ReadAngle(), ReadAngle(), ReadAngle()];
	}
	
	
	float antilag_time = 0;
	if (sendflags & 8)
	{
		self.ownnum = ReadEntityNum();
		antilag_time = (float)ReadByte() / 255;
	}
	
	
	if (is_new)
	{
		self.drawmask = MASK_ENGINE;
		self.predraw = Predraw_Projectile;
		
		self.effects |= EF_NOSHADOW;
		if (self.modelflags & MF_ROCKET)
			self.traileffectnum = Projectile_FindTrail(FALSE);
		else if (self.modelflags & MF_GRENADE)
			self.traileffectnum = Projectile_FindTrail(TRUE);
		
		self.oldorigin = self.s_origin;
		
		if !(PredProjectile_MatchProjectile())
			is_new = 2;
		
		if (is_new == 2 && antilag_time)
			self.oldorigin -= self.velocity * antilag_time;
	}
}






void(float apilevel, string enginename, float engineversion) CSQC_Init
{
	WeaponPred_Init();
	
	precache_model("progs/missile.mdl");
	precache_model("progs/spike.mdl");
	precache_model("progs/s_spike.mdl");
	
	registercvar("cl_predict_weaponsound", "1");
	registercvar("cl_sproj_xerp", "0");
}


float(float entnum, float channel, string soundname, float vol, float attenuation, vector pos, float pitchmod, float flags) CSQC_Event_Sound
{
	if (entnum == player_localentnum)
	{
		if (channel == CHAN_WEAPON)
		{
			if (autocvar_cl_predict_weaponsound == 1)
				return TRUE;
			else if (autocvar_cl_predict_weaponsound > 1)
				return PM_FilterWeaponSound(soundname);
		}
	}
	
	
	return FALSE;
}


void(float is_new) CSQC_Ent_Update
{
	int ent_type = ReadByte();
	
	switch (ent_type)
	{
		case NENT_PROJECTILE: EntUpdate_Projectile(is_new); break;
		case NENT_WEAPONPRED: EntUpdate_WeaponPred(is_new); break;
		
		
	}
}


void(float vwidth, float vheight, float notmenu) CSQC_UpdateView
{
	ping_time = getplayerkeyfloat(player_localnum, INFOKEY_P_PING) / 1000;
	chat_info = stof(getlocaluserinfo(0, "chat"));
	
	clearscene();
	setproperty(VF_DRAWENGINESBAR, TRUE);
	setproperty(VF_DRAWCROSSHAIR, TRUE);
	addentities(MASK_ENGINE | (intermission?0:MASK_PRED_VIEWMODEL));
	renderscene();
}



