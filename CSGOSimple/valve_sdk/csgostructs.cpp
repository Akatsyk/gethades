#include "csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"

bool C_BaseEntity::is_player()
{
	return CallVFunction< bool(__thiscall*) (C_BaseEntity*) >(this, vfindex::ccsplayer::is_player) (this);
}

Vector C_BaseEntity::get_bone_pos(const int i)
{
	matrix3x4_t bone_matrix[128];

	if (this->setup_bones(bone_matrix, 128, bone_used_by_hitbox, 0.f))
		return Vector(bone_matrix[i][0][3], bone_matrix[i][1][3], bone_matrix[i][2][3]);

	return Vector(0, 0, 0);
}

Vector C_BaseEntity::get_head_pos()
{
	return this->get_bone_pos(6);
}

bool C_BaseEntity::is_loot()
{
	return (get_client_class()->class_id == CPhysPropAmmoBox ||
		get_client_class()->class_id == CPhysPropLootCrate ||
		get_client_class()->class_id == CPhysPropRadarJammer ||
		get_client_class()->class_id == CPhysPropWeaponUpgrade ||
		get_client_class()->class_id == CDrone ||
		get_client_class()->class_id == CDronegun ||
		get_client_class()->class_id == CItem_Healthshot ||
		get_client_class()->class_id == CItemCash);
}

bool C_BaseEntity::is_weapon()
{
	return CallVFunction< bool(__thiscall*) (C_BaseEntity*) >(this, vfindex::ccscombat_weapon::is_weapon) (this);
}

bool C_BaseEntity::is_planted_c4()
{
	return get_client_class()->class_id == CPlantedC4;
}

bool C_BaseEntity::is_defuse_kit()
{
	return get_client_class()->class_id == CBaseAnimating;
}

void C_BaseEntity::standard_blending_rules(studiohdr_t* hdr, Vector* pos, quaternion_t* q, float curtime)
{
	using fn = void(__fastcall*) (studiohdr_t*, Vector*, quaternion_t*, float);
	reinterpret_cast<fn> (reinterpret_cast<uintptr_t> (this + 205) ) (hdr, pos, q, curtime);
}

CCSWeaponInfo* C_BaseCombatWeapon::get_cs_weapon_data()
{
	return CallVFunction< CCSWeaponInfo* (__thiscall*) (void*) >(this, vfindex::ccscombat_weapon::get_cs_weapon_data) (this);
}

bool C_BaseCombatWeapon::has_bullets()
{
	return !is_reloading() && clip1() > 0;
}

bool C_BaseCombatWeapon::can_fire()
{
	static decltype (this) stored_weapon = nullptr;
	static auto stored_tick = 0;
	static bool switched_weapons = false;
	static float switch_idle_time = 0.f;

	if (stored_weapon != this || stored_tick >= g_local->tick_base())
	{
		stored_weapon = this;
		stored_tick = g_local->tick_base();
		switched_weapons = true;
		switch_idle_time = this->time_weapon_idle();

		return false;
	}

	if (switched_weapons && this->time_weapon_idle() == switch_idle_time)
		return false;
	else
		switched_weapons = false;

	if (is_reloading() || clip1() <= 0 || !g_local)
		return false;

	return next_primary_attack() <= interfaces::global_vars->curtime;
}

bool C_BaseCombatWeapon::is_grenade()
{
	return get_cs_weapon_data()->weapon_type == weapontype_grenade;
}

bool C_BaseCombatWeapon::is_gun()
{
	switch (get_cs_weapon_data()->weapon_type)
	{
	case weapontype_c4:
		return false;

	case weapontype_grenade:
		return false;

	case weapontype_knife:
		return false;

	case weapontype_unknown:
		return false;

	default:
		return true;
	}
}

bool C_BaseCombatWeapon::is_zeus()
{
	return this->item().item_definition_index() == WEAPON_ZEUS;
}

bool C_BaseCombatWeapon::is_knife()
{
	if (this->item().item_definition_index() == WEAPON_ZEUS)
		return false;

	return get_cs_weapon_data()->weapon_type == weapontype_knife;
}

bool C_BaseCombatWeapon::is_rifle()
{
	switch (get_cs_weapon_data()->weapon_type)
	{
	case weapontype_rifle:
		return true;

	case weapontype_submachinegun:
		return true;

	case weapontype_shotgun:
		return true;

	case weapontype_machinegun:
		return true;

	default:
		return false;
	}
}

bool C_BaseCombatWeapon::is_pistol()
{
	switch (get_cs_weapon_data()->weapon_type)
	{
	case weapontype_pistol:
		return true;

	default:
		return false;
	}
}

bool C_BaseCombatWeapon::is_sniper()
{
	switch (get_cs_weapon_data()->weapon_type)
	{
	case weapontype_sniper_rifle:
		return true;

	default:
		return false;
	}
}

bool C_BaseCombatWeapon::is_auto()
{
	switch (this->item().item_definition_index())
	{
	case WEAPON_SCAR20:
	case WEAPON_G3SG1:
		return true;

	default:
		return false;
	}
}

bool C_BaseCombatWeapon::is_scout()
{
	return this->item().item_definition_index() == WEAPON_SSG08;
}

bool C_BaseCombatWeapon::is_r8()
{
	return this->item().item_definition_index() == WEAPON_REVOLVER;
}

bool C_BaseCombatWeapon::is_deagle()
{
	return this->item().item_definition_index() == WEAPON_DEAGLE;
}

bool C_BaseCombatWeapon::is_lmg()
{
	return this->get_cs_weapon_data()->weapon_type == weapontype_machinegun;
}

bool C_BaseCombatWeapon::is_smg()
{
	return this->get_cs_weapon_data()->weapon_type == weapontype_submachinegun;
}

bool C_BaseCombatWeapon::is_reloading()
{
	return *reinterpret_cast<bool*> (reinterpret_cast<uintptr_t> (this) + 0x32A5);
}

float C_BaseCombatWeapon::get_inaccuracy()
{
	return CallVFunction< float(__thiscall*) (void*) >(this, vfindex::ccscombat_weapon::get_inaccuracy) (this);
}

float C_BaseCombatWeapon::get_spread()
{
	return CallVFunction< float(__thiscall*) (void*) >(this, vfindex::ccscombat_weapon::get_spread) (this);
}

void C_BaseCombatWeapon::update_accuracy_penalty()
{
	CallVFunction< void(__thiscall*) (void*) >(this, vfindex::ccscombat_weapon::update_accuracy_penalty) (this);
}

CUtlVector< IRefCounted* >& C_BaseCombatWeapon::custom_materials()
{
	static auto custom_materials = *reinterpret_cast<uint32_t*> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("83 BE ? ? ? ? ? 7F 67")) + 2) - 12;

	return *reinterpret_cast<CUtlVector< IRefCounted* >*> (reinterpret_cast<uintptr_t> (this) + custom_materials);
}

bool* C_BaseCombatWeapon::custom_material_initialized()
{
	static auto custom_material_initialized = *reinterpret_cast<uint32_t*> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("C6 86 ? ? ? ? ? FF 50 04")) + 2);

	return reinterpret_cast<bool*> (reinterpret_cast<uintptr_t> (this) + custom_material_initialized);
}

usercmd_t*& C_BasePlayer::current_command()
{
	static auto current_command = *reinterpret_cast<uint32_t*> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("89 BE ? ? ? ? E8 ? ? ? ? 85 FF")) + 2);

	return *reinterpret_cast<usercmd_t**> (reinterpret_cast<uintptr_t> (this) + current_command);
}

int C_BasePlayer::get_num_anim_overlays()
{
	return *reinterpret_cast<int*> (reinterpret_cast<DWORD> (this) + 0x298C);
}

animation_layer_t* C_BasePlayer::get_anim_overlays()
{
	return *reinterpret_cast<animation_layer_t**> (reinterpret_cast<DWORD> (this) + 0x2980);
}

animation_layer_t* C_BasePlayer::get_anim_overlay(int i)
{
	if (i < 15)
		return &get_anim_overlays()[i];

	return nullptr;
}

int C_BasePlayer::get_sequence_activity(int sequence)
{
	const auto hdr = interfaces::mdl_info->get_studiomodel(this->get_model());

	if (!hdr)
		return -1;

	static auto get_sequence_activity_call = reinterpret_cast<int(__fastcall*) (void*, studiohdr_t*, int)> (g_utils.pattern_scan(
		xor_str("client.dll"), xor_str("55 8B EC 53 8B 5D 08 56 8B F1 83")));

	return get_sequence_activity_call(this, hdr, sequence);
}

c_base_player_anim_state* C_BasePlayer::get_base_player_anim_state()
{
	return *reinterpret_cast<c_base_player_anim_state**> (reinterpret_cast<DWORD> (this) + 0x3914);
}

CCSPlayerAnimState* C_BasePlayer::get_player_anim_state()
{
	return *reinterpret_cast<CCSPlayerAnimState**> (reinterpret_cast<DWORD> (this) + 0x3914);
}

void C_BasePlayer::update_animation_state(CCSGOPlayerAnimState* state, QAngle angle)
{
	static auto UpdateAnimState = g_utils.pattern_scan("client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	if (!UpdateAnimState)
		return;

	__asm {
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::reset_animation_state(CCSGOPlayerAnimState* state)
{
	using reset_anim_state_t = void(__thiscall*) (CCSGOPlayerAnimState*);

	static auto reset_anim_state = reinterpret_cast<reset_anim_state_t> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("56 6A 01 68 ? ? ? ? 8B F1")));

	if (!reset_anim_state)
		return;

	reset_anim_state(state);
}

void C_BasePlayer::create_animation_state(CCSGOPlayerAnimState* state)
{
	using create_anim_state_t = void(__thiscall*) (CCSGOPlayerAnimState*, C_BasePlayer*);

	static auto create_anim_state = reinterpret_cast<create_anim_state_t> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("55 8B EC 56 8B F1 B9 ? ? ? ? C7 46")));

	if (!create_anim_state)
		return;

	create_anim_state(state, this);
}

void C_BasePlayer::modify_eye_pos(c_base_player_anim_state* animstate, vector_t* pos)
{
	if (interfaces::engine_client->is_hltv() || interfaces::engine_client->is_playing_demo())
		return;

	static auto lookup_bone_fn = reinterpret_cast<int(__thiscall*) (void*, const char*)> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("55 8B EC 53 56 8B F1 57 83 BE ?? ?? ?? ?? ?? 75 14")));

	if (*reinterpret_cast<bool*> (uintptr_t(animstate) + 0x109) && animstate->m_fDuckAmount != 0.f && animstate->m_pBaseEntity
		&& reinterpret_cast<C_BasePlayer*> (animstate->m_pBaseEntity)->ground_entity()
		&& reinterpret_cast<C_BasePlayer*> (animstate->m_pBaseEntity)->ground_entity().get())
	{
		auto base_entity = animstate->m_pBaseEntity;

		auto bone_pos = get_bone_pos(lookup_bone_fn(base_entity, xor_str("head_0")));

		bone_pos.z += 1.7f;

		if ((*pos).z > bone_pos.z)
		{
			float some_factor = 0.f;

			float delta = (*pos).z - bone_pos.z;

			float some_offset = (delta - 4.f) / 6.f;

			if (some_offset >= 0.f)
				some_factor = std::fminf(some_offset, 1.f);

			(*pos).z += ((bone_pos.z - (*pos).z) * (((some_factor * some_factor) * 3.f) - (((some_factor * some_factor) * 2.f) * some_factor
				)));
		}
	}
}

Vector C_BasePlayer::get_eye_pos()
{
	if (globals.in_fakeduck && this->ent_index() == interfaces::engine_client->get_localplayer())
	{
		auto origin = vec_origin();

		auto vDuckHullMin = interfaces::game_movement->get_player_mins(true);
		auto vStandHullMin = interfaces::game_movement->get_player_mins(false);

		float fMore = (vDuckHullMin.z - vStandHullMin.z);

		auto vecDuckViewOffset = interfaces::game_movement->get_player_view_offset(true);
		auto vecStandViewOffset = interfaces::game_movement->get_player_view_offset(false);
		float duckFraction = duck_amount();

		float tempz = ((vecDuckViewOffset.z - fMore) * duckFraction) +
			(vecStandViewOffset.z * (1 - duckFraction));

		origin.z += tempz;

		return origin;
	}

	return vec_origin() + view_offset();
}

vector_t C_BasePlayer::get_eye_pos(void* animstate)
{
	if (globals.in_fakeduck && this->ent_index() == interfaces::engine_client->get_localplayer())
	{
		auto origin = vec_origin();

		auto vDuckHullMin = interfaces::game_movement->get_player_mins(true);
		auto vStandHullMin = interfaces::game_movement->get_player_mins(false);

		float fMore = (vDuckHullMin.z - vStandHullMin.z);

		auto vecDuckViewOffset = interfaces::game_movement->get_player_view_offset(true);
		auto vecStandViewOffset = interfaces::game_movement->get_player_view_offset(false);
		float duckFraction = duck_amount();

		float tempz = ((vecDuckViewOffset.z - fMore) * duckFraction) +
			(vecStandViewOffset.z * (1 - duckFraction));

		origin.z += tempz;

		return origin;
	}

	auto pos = vector_t{ };
	pos = vec_origin() + view_offset();

	if (animstate)
		modify_eye_pos(reinterpret_cast<c_base_player_anim_state*> (animstate), &pos);

	return pos;
}

player_info_t C_BasePlayer::get_player_info() const
{
	player_info_t info;
	interfaces::engine_client->get_player_info(ent_index(), &info);

	return info;
}

bool C_BasePlayer::is_alive() const
{
	return e_state() == E_ALIVE || this->health() > 0;
}

bool C_BasePlayer::is_flashed()
{
	static auto flash_max_alpha = g_netvarsys.GetOffset(xor_str("DT_CSPlayer"), xor_str("m_flFlashMaxAlpha"));
	return *reinterpret_cast<float*> (reinterpret_cast<uintptr_t> (this) + flash_max_alpha - 0x8) > 200.0;
}

bool C_BasePlayer::has_c4()
{
	static auto fn_has_c4 = reinterpret_cast<bool(__thiscall*) (void*)> (g_utils.pattern_scan(xor_str("client.dll"), xor_str("56 8B F1 85 F6 74 31")));

	return fn_has_c4(this);
}

Vector C_BasePlayer::get_hitbox_pos(const int hitbox_id)
{
	matrix3x4_t bone_matrix[maxstudiobones];

	if (setup_bones(bone_matrix, maxstudiobones, bone_used_by_hitbox, 0.0f))
	{
		auto studio_model = interfaces::mdl_info->get_studiomodel(get_model());

		if (studio_model)
		{
			const auto hitbox = studio_model->get_hitbox_set(0)->get_hitbox(hitbox_id);

			if (hitbox)
			{
				auto
					min = Vector{ },
					max = Vector{ };

				g_math.vector_transform(hitbox->bbmin, bone_matrix[hitbox->bone], min);
				g_math.vector_transform(hitbox->bbmax, bone_matrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}

	return Vector{ };
}

mstudiobbox_t* C_BasePlayer::get_hitbox(const int hitbox_id)
{
	matrix3x4_t bone_matrix[maxstudiobones];

	if (setup_bones(bone_matrix, maxstudiobones, bone_used_by_hitbox, 0.0f))
	{
		auto studio_model = interfaces::mdl_info->get_studiomodel(get_model());

		if (studio_model)
		{
			const auto hitbox = studio_model->get_hitbox_set(0)->get_hitbox(hitbox_id);

			if (hitbox)
			{
				return hitbox;
			}
		}
	}

	return nullptr;
}

bool C_BasePlayer::get_hitbox_pos(int hitbox, Vector& output)
{
	if (hitbox >= hitbox_max)
		return false;

	const auto model = this->get_model();

	if (!model)
		return false;

	auto studio_hdr = interfaces::mdl_info->get_studiomodel(model);

	if (!studio_hdr)
		return false;

	matrix3x4_t matrix[maxstudiobones];

	if (!this->setup_bones(matrix, maxstudiobones, 0x100, 0))
		return false;

	const auto studio_box = studio_hdr->get_hitbox_set(0)->get_hitbox(hitbox);

	if (!studio_box)
		return false;

	Vector min, max;

	g_math.vector_transform(studio_box->bbmin, matrix[studio_box->bone], min);
	g_math.vector_transform(studio_box->bbmax, matrix[studio_box->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::get_bone_pos(int bone)
{
	matrix3x4_t bone_matrix[maxstudiobones];

	if (setup_bones(bone_matrix, maxstudiobones, bone_used_by_anything, 0.0f))
		return bone_matrix[bone].at(3);

	return Vector{ };
}

bool C_BasePlayer::can_see_player(C_BasePlayer* player, int hitbox)
{
	if (!interfaces::engine_trace)
		return false;

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	const auto endpos = player->get_hitbox_pos(hitbox);

	ray.Init(get_eye_pos(), endpos);
	interfaces::engine_trace->trace_ray(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::can_see_player(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = this;

	ray.Init(get_eye_pos(), pos);
	interfaces::engine_trace->trace_ray(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

void C_BasePlayer::update_client_side_animation()
{
	return CallVFunction< void(__thiscall*) (void*) >(this, vfindex::ccsplayer::update_client_side_animations) (this);
}

void C_BasePlayer::set_angle2(const QAngle wantedang)
{
	typedef void(__thiscall * set_angle_fn) (void*, const QAngle&);

	static set_angle_fn set_angle2 = reinterpret_cast<set_angle_fn> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1")));
	
	set_angle2(this, wantedang);
}

float& C_BasePlayer::last_bone_setup_time()
{
	static auto invalidate_bone_cache = g_utils.pattern_scan(xor_str("client.dll"), 
		xor_str("80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81"));
	
	static auto last_bone_setup_time = *(uintptr_t*)(invalidate_bone_cache + 0x11);

	return *(float*)((uintptr_t)this + last_bone_setup_time);
}

uint32_t& C_BasePlayer::most_recent_model_bone_counter()
{
	static auto invalidate_bone_cache = g_utils.pattern_scan(xor_str("client.dll"), 
		xor_str("80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81"));

	static auto most_recent_model_bone_counter = *(uintptr_t*)(invalidate_bone_cache + 0x1B);

	return *(uint32_t*)((uintptr_t)this + most_recent_model_bone_counter);
}

void C_BasePlayer::invalidate_bone_cache()
{
	if (!this)
		return;

	last_bone_setup_time() = -FLT_MAX;
	most_recent_model_bone_counter() = UINT_MAX;
}

QAngle* C_BasePlayer::get_vangles()
{
	return reinterpret_cast<QAngle*> (reinterpret_cast<uintptr_t> (this) + 0x31D8);
}

float C_BasePlayer::GetSimulationTime()
{
	static auto m_flSimulationTime = g_netvarsys.GetOffset(xor_str("DT_BaseEntity"), xor_str("m_flSimulationTime"));
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + m_flSimulationTime);
}

float C_BasePlayer::GetOldSimulationTime()
{
	static auto m_flOldSimulationTime = g_netvarsys.GetOffset(xor_str("DT_BaseEntity"), xor_str("m_flSimulationTime")) + 4;
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + m_flOldSimulationTime);
}

void C_BaseAttributableItem::set_glove_model_index(const int model_index)
{
	return CallVFunction< void(__thiscall*) (void*, int) >(this, 75) (this, model_index);
}

float C_BasePlayer::get_flash_bang_time()
{
	return *reinterpret_cast<float*> (this + 0xA420);
}

void C_BaseViewModel::send_view_model_matching_sequence(int sequence)
{
	return CallVFunction< void(__thiscall*) (void*, int) >(this, 246) (this, sequence);
}

float c_base_player_anim_state::max_rotation()
{
	const auto animstate = reinterpret_cast<uintptr_t> (this);

	if (!animstate)
		return 58.f;

	const auto duckammount = *reinterpret_cast<float*> (animstate + 164);
	const auto speedfraction_clamped = std::max(0.f, std::min(*reinterpret_cast<float*> (animstate + 248), 1.f));

	const auto speedfactor = std::max(0.f, std::min(1.f, *reinterpret_cast<float*> (animstate + 252)));

	float unk1 = (((*reinterpret_cast<float*> (animstate + 284) * -0.30000001) - 0.19999999) * speedfraction_clamped) + 1.f;

	if (duckammount > 0)
		unk1 += ((duckammount * speedfactor) * (0.5f - unk1));

	return *reinterpret_cast<float*> (animstate + 820) * unk1;
}

CUtlVector< IRefCounted* >& C_EconItemView::custom_materials()
{
	return *reinterpret_cast<CUtlVector< IRefCounted* >*> (reinterpret_cast<uintptr_t> (this) + 0x14);
}

CUtlVector< IRefCounted* >& C_EconItemView::visuals_data_processors()
{
	static auto in_reload = *reinterpret_cast<uint32_t*> (g_utils.pattern_scan(xor_str("client.dll"),
		xor_str("81 C7 ? ? ? ? 8B 4F 0C 8B 57 04 89 4C 24 0C")) + 2);

	return *reinterpret_cast<CUtlVector< IRefCounted* >*> (reinterpret_cast<uintptr_t> (this) + in_reload);
}

float C_BasePlayer::get_max_desync_delta_inverted()
{
	const auto animstate = reinterpret_cast<uintptr_t> (this->get_base_player_anim_state());

	if (!animstate)
		return 0.f;

	const auto duckammount = *reinterpret_cast<float*> (animstate + 0xA4);
	const auto speedfraction_clamped = std::max(0.f, std::min(*reinterpret_cast<float*> (animstate + 0xF8), 1.f));

	const auto speedfactor = std::max(0.f, std::min(1.f, *reinterpret_cast<float*> (animstate + 0xFC)));

	auto unk1 = (((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction_clamped) + 1.f;

	if (duckammount > 0)
		unk1 += ((duckammount * speedfactor) * (0.5f - unk1));

	return *reinterpret_cast<float*> (animstate + 0x334) * unk1;
}

float C_BasePlayer::get_max_desync_delta()
{
	const auto animstate = reinterpret_cast<uintptr_t> (this->get_base_player_anim_state());

	if (!animstate)
		return 58.f;

	const auto duckammount = *reinterpret_cast<float*> (animstate + 164);
	const auto speedfraction_clamped = std::max(0.f, std::min(*reinterpret_cast<float*> (animstate + 248), 1.f));

	const auto speedfactor = std::max(0.f, std::min(1.f, *reinterpret_cast<float*> (animstate + 252)));

	float unk1 = (((*reinterpret_cast<float*> (animstate + 284) * -0.30000001) - 0.19999999) * speedfraction_clamped) + 1.f;

	if (duckammount > 0)
		unk1 += ((duckammount * speedfactor) * (0.5f - unk1));

	return *reinterpret_cast<float*> (animstate + 820) * unk1;
}

float C_BasePlayer::get_yaw_feet_delta()
{
	auto animstate = this->get_base_player_anim_state();

	if (!animstate)
		return -1.f;

	return g_math.angle_diff(*reinterpret_cast<float*> (reinterpret_cast<uintptr_t> (animstate) + 0x78), animstate->m_flGoalFeetYaw);
}