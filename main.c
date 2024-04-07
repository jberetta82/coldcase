/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/***************************************************************************************************\
** Special Note : The 3D part on Jo Engine is still in development. So, some glitch may occur ;)
**                Btw, texture mapping for triangle base mesh (not quads) is experimental.
\***************************************************************************************************/

#include <jo/jo.h>
#include "pcmsys.h"
#include "collision.h"
#include "objects.h"
#include "skybox.h"
#include "hamster.h"
#include<stdio.h>
#include<string.h>

#define WORK_RAM_LOW 0x00200000
static XPDATA *xpdata_[1];
static CDATA *cdata_[1];

jo_sidescroller_physics_params  physics;

unsigned short mesh_total = 0;

extern Sint8 SynchConst;
Sint32 framerate;

player_params 		player;
game_params 		game;
map_params 			map_plan;
level_section		map_section[20];
enemy 				enemies[10];
powerup				powerups[10];
door				doors[5];
unsigned int				door_total = 0;
jo_pos3Df                   pos;
jo_rot3Df                   rot;
jo_palette                  image_pal;
jo_palette                  map_plan_pal;

GOURAUDTBL		gourRealMax[GOUR_REAL_MAX];
Uint8			vwork[GOUR_REAL_MAX];
Uint8 			enableRTG = 1;

//from XL2
#define KEY_PRESS(id, key)  ((Smpc_Peripheral[id].data & key) == 0)
#define KEY_DOWN(id, key)   ((Smpc_Peripheral[id].push & key) == 0)

static jo_camera    cam;
int					cam_pos_x = CAM_DEFAULT_X;
int					cam_pos_y = CAM_DEFAULT_Y;
int					cam_pos_z = CAM_DEFAULT_Z;
unsigned int 		default_cam = 0;
int					cam_1_x = 0;
int					cam_1_y = 0;
int					cam_1_z = 0;
int					cam_2_x = 0;
int					cam_2_y = 0;
int					cam_2_z = 0;
int					cam_3_x = 0;
int					cam_3_y = 0;
int					cam_3_z = 0;
int					next_cam_pos_x = 0;
int					next_cam_pos_y = 0;
int					next_cam_pos_z = 0;
int					cam_angle_x = 0;
int					cam_angle_y = 0;
int					cam_angle_z = 0;
int					cam_target_x = 0;
int					cam_target_y = 0;
int					cam_target_z = 0;
//static int			cam_zoom = 0;
static int			cam_angle_adj = 0;
int					delta_x = 0;
float 				delta_y = 0.0f;
int 				delta_z = 0;
int					cam_adj_x;
int					cam_adj_z;
int					next_cam_x;
int					next_cam_z;
int					cam_number = 0;
static Sint16			section_dist;
bool					toggle_cam = 0;

static bool			show_debug = false;	
//int					collpoint_total;
int					section_total;



static bool     	is_cd_playing = false;

//static float        rx, ry, rz;
//static int			model = 0;
static bool			use_light = true;
static bool 		flash = false;
static int			flash_counter = 0;
static int			spark_counter = 0;
static int			spark = false;
static int			spark_type = 0;

static int skybox_tex;//65
static int pspark_tex;//66
static int pblood_tex;//67
//static int pist_bullet_tex;
static int health_bar_tex;

static int			lerp_count;
 short     step_sound;
 short     die_sound;
 short     ouch_sound;
 short     jump_sound;
 short     pup_sound;
 short     flap_sound;
 short     door_sound;
 short		pistol_sound;
 short		reload_sound;
 short		empty_sound;
 short		shotgun_sound;
 short		ladder_sound;
 short		alert_sound;
 short		alert2_sound;
 short		map_sound;
unsigned int			cd_track = 0;
static int map_sprite_id;
static int			highlight_item;

static  int			enemy_total;
static  int			powerup_total;
static int			cam_height;
static int			cam_dist;
static int			cam_horiz_dist;
static int			cam_vert_dist;
static int			cam_pitch;
static int			cam_pitch_adj;
static bool			first_person_cam = false;
 bool			drop_camera = false;
 //static bool	action_button = true;
 bool			draw_sky = true;
static unsigned int		inventory_item_number = 0;
Uint8					object_viewer_mode = 0;
Uint8					model_num = 0;
Uint16 					object_number = 0;
float					object_scale = 1;
Uint16					object_pol_num = 0;
Uint16					object_last_texture = 0;
Uint16					object_last_pol_num = 0;
bool					object_show_poly = 0;
Uint8					map_builder_enemy_type = 1;
Uint8					anim_num = 0;
bool 					anim_loop = 1;
Uint8 					total_frames;
int						map_plan_tex;
Uint8					has_map_plan;

jo_palette          *my_tga_palette_handling(void)
{
    // We create a new palette for each image. It's not optimal but OK for a demo.
    jo_create_palette(&image_pal);
    return (&image_pal);
}

jo_palette          *my_tga_palette_handling2(void)
{
    // We create a new palette for each image. It's not optimal but OK for a demo.
    jo_create_palette(&map_plan_pal);
    return (&map_plan_pal);
}

/* 36 tiles */
static const jo_tile    PLAYER_Tileset[] =
{
	{0, 0, 32, 32},
	{0, 32, 32, 32},
	{0, 64, 32, 32},
	{32, 0, 32, 32},
	{32, 32, 32, 32},//forehead
	{32, 64, 32, 32},
	{64, 0, 32, 32},
	{64, 32, 32, 32},
	{64, 64, 32, 32},
	{96, 0, 32, 32},
	{96, 32, 32, 32},
	{96, 64, 32, 32},
	{128, 0, 32, 32},
	{128, 32, 32, 32},
	{128, 64, 32, 32},
	{32, 38, 16, 16},//nose pink
	{0, 96, 32, 32},
	{32, 96, 32, 32},
	{64, 96, 32, 32},
	{81, 118, 8, 1},
	{81, 104, 8, 1},
	{96, 96, 8, 24},
	{104, 96, 8, 30},
	{128, 96, 32, 32},
	{0, 128, 32, 32},
	{32, 128, 32, 32},
	{120, 96, 8, 1},
	{64, 128, 32, 32},
	{140, 74, 8, 8},
	{96, 128, 32, 32},
	{128, 128, 32, 32},
	{0, 160, 32, 32},
	{32, 160, 32, 32},
	{64, 160, 32, 32},
	{96, 160, 32, 32},
	{128, 160, 32, 32},
	{96, 120, 8, 8}
};
//3
static const jo_tile    ENEMY_Tileset[] =
{
	{0, 0, 32, 32},
	{32, 0, 32, 32},
	{64, 0, 32, 32},
	{0, 32, 32, 32},
	{32, 32, 32, 32},
	{64, 32, 16, 32},
	{0, 64, 24, 32},
	{24, 64, 40, 32},
	{64, 64, 16, 16},
	{80, 64, 16, 16},
	{64, 80, 16, 16},
	{80, 80, 16, 16}
	
};


static const jo_tile    MAP_Tileset[] =
{
	{0, 0, 32, 32},
	{0, 32, 32, 32},
	{0, 64, 32, 32},
	{0, 96, 32, 32},
	{0, 128, 32, 32},
	{0, 160, 32, 32},
	{0, 192, 32, 32},
	{32, 0, 32, 32},
	{32, 32, 32, 32},
	{32, 64, 32, 32},
	{32, 96, 32, 32},
	{32, 128, 32, 32},
	{32, 160, 32, 32},
	{32, 192, 32, 32},//13
	{64, 0, 32, 32},
	{64, 32, 32, 32},
	{64, 64, 32, 32},
	{64, 96, 32, 16},
	{64, 112, 32, 16},
	{64, 128, 32, 32},
	{64, 160, 32, 32},
	{64, 192, 32, 32},
	{96, 0, 32, 32},//22
	{96, 32, 32, 32},
	{96, 64, 32, 32},
	{96, 96, 32, 32},
	{96, 128, 32, 32},//26
	{96, 160, 32, 32},
	{96, 192, 64, 18},
	{128, 0, 32, 32},//29
	{128, 32, 32, 32},//30
	{128, 64, 32, 32},//31
	{128, 96, 32, 32},//32
	{128, 128, 32, 32},//33
	{128, 160, 32, 32},
	{230, 168, 16, 16},//35 nose pink
	{160, 64, 32, 32},
	{160, 96, 32, 32},
	{160, 128, 32, 32},
	{160, 160, 32, 32},
	{160, 192, 32, 32},//40
	{192, 64, 32, 32},//41
	{192, 96, 32, 32},//42
	{192, 128, 32, 32},//43
	{192, 160, 32, 32},//44
	{192, 192, 32, 32},//45
	{224, 0, 32, 32},//46 coat
	{224, 32, 32, 32},//47 jeans
	{224, 64, 32, 32},//48 tshirt
	{224, 96, 32, 32},
	{224, 128, 32, 32},//50 ear
	{224, 160, 32, 32},//51 fore head
	{224, 192, 32, 32},//52 face
	{256, 0, 32, 32},//53 side forehead
	{256, 32, 32, 32},//54 side face
	{256, 64, 32, 32},//55 hair
	{256, 96, 32, 32},
	{256, 128, 32, 32},
	{256, 160, 32, 32},//arrow
	{192, 0, 32, 32},//leaf2
	{160, 32, 32, 32},//leaf3
	{192, 32, 32, 32},
	{160, 0, 32, 32},//62 orange leaf1
	{256, 192, 32, 32}
	//{230, 168, 16, 16}//nose
	
};



int nDigits(int x)
{
	int minus = 0;
	if(x<0) minus = 1;
	if (JO_ABS(x) >= 1000000000) return 10 + minus;
    if (JO_ABS(x) >= 100000000)  return 9 + minus;
    if (JO_ABS(x) >= 10000000)   return 8 + minus;
    if (JO_ABS(x) >= 1000000)    return 7 + minus;
    if (JO_ABS(x) >= 100000)     return 6 + minus;
    if (JO_ABS(x) >= 10000)      return 5 + minus;
    if (JO_ABS(x) >= 1000)       return 4 + minus;
    if (JO_ABS(x) >= 100)        return 3 + minus;
    if (JO_ABS(x) >= 10)         return 2 + minus;
    return 1 + minus;

}

//(enemy_number, type, x, y, z, xdist, zdist, speed, hits)

void 		stop_sounds()
{
	
pcm_cease(step_sound);
pcm_cease(die_sound);
pcm_cease(ouch_sound);
pcm_cease(jump_sound);
pcm_cease(pup_sound);
pcm_cease(flap_sound);
pcm_cease(pistol_sound);
pcm_cease(shotgun_sound);
pcm_cease(door_sound);
pcm_cease(alert_sound);
pcm_cease(alert2_sound);
pcm_cease(map_sound);

	
}

unsigned int search_for_item(Uint16 id)
{
unsigned int current_item =0;
unsigned int total_items =0;
total_items = player.a_itemdata->nbIt;

for (unsigned int i = 0; i<total_items; i++)
	{
	if(player.a_itemdata->itemtbl[i].id == id)
	{current_item = i;
	}

	}		

	
return current_item;	
}

void create_enemy(enemy* new_enemy, Uint16 id, Uint8 type, Sint16 x,Sint16 y, Sint16 z, Sint16 xdist, Sint16 zdist, Sint16 max_speed, Sint16 health)
{
	new_enemy->id = id;
	new_enemy->type = type;
	//new_enemy->start_health = health;
	//new_enemy->health = health;
	new_enemy->can_be_hurt = true;
	new_enemy->start_x = x;
	new_enemy->start_y = y;
	new_enemy->start_z = z;
    new_enemy->x = x;
	new_enemy->y = y;
	new_enemy->z = z;
	new_enemy->xdist = xdist;
	new_enemy->zdist = zdist;
	new_enemy->max_speed = max_speed;
	new_enemy->explosion_size = 0;
	new_enemy->px = x;
	new_enemy->py = y;
	new_enemy->pz = z;
	new_enemy->shoot_wait = 0;
	new_enemy->anim_speed = ANIM_SPEED;
	new_enemy->alert = false;

	if(type == 1)//spider
	{new_enemy->xsize = 19;
	new_enemy->ysize = 21;
	new_enemy->zsize= 19;
	new_enemy->health = 40;
	}
	
	if(type == 2)//big bat
	{new_enemy->xsize = 19;
	new_enemy->ysize = 21;
	new_enemy->zsize= 19;
	new_enemy->health = 80;
	}
	if(type == 3)//small bat
	{new_enemy->xsize = 19;
	new_enemy->ysize = 21;
	new_enemy->zsize= 19;
	new_enemy->health = 20;
	}
	
	new_enemy->start_health = new_enemy->health;
	
	if(search_for_item(new_enemy->id) !=0)
	{
	new_enemy->alive = false;
	}else
	{
	new_enemy->alive = true;
	}
	
}

void				player_reload(weapon* weapon_to_reload)
{
	
Uint8 remaining_bullets;

remaining_bullets = weapon_to_reload->total_bullets - weapon_to_reload->bullets_in_clip;

			if(remaining_bullets > 0)
			{
			pcm_play(reload_sound, PCM_VOLATILE, 6);
				if(weapon_to_reload->bullets_in_clip + remaining_bullets >=weapon_to_reload->clip_size)
				{
				weapon_to_reload->bullets_in_clip = weapon_to_reload->clip_size;
				
				}else
				{
				weapon_to_reload->bullets_in_clip += remaining_bullets;
				
				}
			
			
			
			}else
			{
			// gun empty sound
			pcm_play(empty_sound, PCM_VOLATILE,6);
			}
		
	
}




unsigned int previous_item(Uint16 id)
{
unsigned int current_item =0;
unsigned int total_items =0;
total_items = player.a_itemdata->nbIt;
current_item = search_for_item(id);


jo_clear_screen();
			
if(current_item == 0)
			{
			current_item = total_items-1;	
			}else
			{
			current_item --;	
			}

while (player.a_itemdata->itemtbl[current_item].used)
	{
	
		if(current_item == 0)
			{
			current_item = total_items-1;	
			}else
			{
			current_item --;	
			}
	

	}		

	
return current_item;	
}

unsigned int next_item(Uint16 id)
{
unsigned int current_item =0;
unsigned int total_items =0;
total_items = player.a_itemdata->nbIt;
current_item = search_for_item(id);


jo_clear_screen();
			
if(current_item == total_items-1)
			{
			current_item = 0;	
			}else
			{
			current_item ++;	
			}

while (player.a_itemdata->itemtbl[current_item].used)
	{
	
		if(current_item == total_items-1)
			{
			current_item = 0;	
			}else
			{
			current_item ++;	
			}
	

	}		

	
return current_item;	
}





void add_item(Uint16 id, Uint8 type)
{

//add new item to players inventory_
unsigned int total_items =0;

total_items = player.a_itemdata->nbIt;

player.a_itemdata->itemtbl = (ITEM*)(player.a_itemdata + sizeof(unsigned int));//(ITEM*)(player.a_itemdata + sizeof(ITEM)*total_items);


player.a_itemdata->itemtbl[total_items].id = id;
player.a_itemdata->itemtbl[total_items].type = type;
player.a_itemdata->itemtbl[total_items].used = false;

switch(type)
{
		
case 0: strcpy(player.a_itemdata->itemtbl[total_items].name,"PISTOL");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"PISTOL DESCRIPTION");
		break;
		
case 1: strcpy(player.a_itemdata->itemtbl[total_items].name,"LARGE MEDKIT");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"restores full health");
		break;
		
case 2: strcpy(player.a_itemdata->itemtbl[total_items].name,"Pistol Ammo");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"Contains 8 bullets");
		break;
		
case 3: strcpy(player.a_itemdata->itemtbl[total_items].name,"SHOTGUN");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"SHOTGUN DESCRIPTION");
		break;

case 4: strcpy(player.a_itemdata->itemtbl[total_items].name,"Shotgun Ammo");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"Contains 6 Shells");
		break;
		
case 5: strcpy(player.a_itemdata->itemtbl[total_items].name,"Tower Key");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"Key to unlock door to Church Tower");
		break;
		
case 6: strcpy(player.a_itemdata->itemtbl[total_items].name,"Health Drink");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"Restores 10 health points");
		break;
		
case 7: strcpy(player.a_itemdata->itemtbl[total_items].name,"Evidence");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"A photo of a church");
		break;
	
case 8: strcpy(player.a_itemdata->itemtbl[total_items].name,"DEAD ENEMY");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"DEAD ENEMY");
		player.a_itemdata->itemtbl[total_items].used = true;
		break;
		
case 9: strcpy(player.a_itemdata->itemtbl[total_items].name,"EVIDENCE");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"A photo of a shop");
		break;

case 10: strcpy(player.a_itemdata->itemtbl[total_items].name,"Office Key");
		strcpy(player.a_itemdata->itemtbl[total_items].description,"Key to unlock Church Office");
		break;

	
	
}


total_items = total_items+1;

player.a_itemdata->nbIt = total_items;
	
}

void clear_items(void)
{
	player.a_itemdata->nbIt = 0;
	
		
}

void load_items(void)
{

	unsigned int total_saved_items =0;
	unsigned int total_items =0;
	
	ITEMDATA		*saved_itemdata;
	saved_itemdata = &ID_ItemsSaved;
	total_saved_items = saved_itemdata->nbIt;
	
	if(total_saved_items == 0)
	{
	return;
	}
	
	clear_items();
	
	for (unsigned int i = 0; i<total_saved_items; i++)
	{
	add_item(saved_itemdata->itemtbl[i].id,saved_itemdata->itemtbl[i].type);
	player.a_itemdata->itemtbl[i].used = saved_itemdata->itemtbl[i].used;
	
	total_items++;
	}
	
	player.a_itemdata->nbIt = total_items;
	
	///load current health and ammo
	pistol.bullets_in_clip = saved_pistol.bullets_in_clip;
	pistol.total_bullets = saved_pistol.total_bullets;
	shotgun.bullets_in_clip = saved_shotgun.bullets_in_clip;
	shotgun.total_bullets = saved_shotgun.total_bullets;
	player.health = player.saved_health;
	
}

void clear_saved_items(void)
{
	ITEMDATA		*saved_itemdata;
	saved_itemdata = &ID_ItemsSaved;
	saved_itemdata->nbIt = 0;	
	
}

void save_items(void)
{
	unsigned int total_saved_items =0;
	unsigned int total_items =0;
	
	total_items = player.a_itemdata->nbIt;
	
	if(total_items == 0)
	{
	return;
	}
	
	clear_saved_items();
	
	ITEMDATA		*saved_itemdata;
	saved_itemdata = &ID_ItemsSaved;
	
	
	
	for (unsigned int i = 0; i<total_items; i++)
	{
	saved_itemdata->itemtbl = (ITEM*)(saved_itemdata + sizeof(unsigned int));
	saved_itemdata->itemtbl[i].id = player.a_itemdata->itemtbl[i].id;
	saved_itemdata->itemtbl[i].type = player.a_itemdata->itemtbl[i].type;
	saved_itemdata->itemtbl[i].used = player.a_itemdata->itemtbl[i].used;

	total_saved_items++;
	}
	
	saved_itemdata->nbIt = total_saved_items;
	
	///save current health and ammo
	saved_pistol.bullets_in_clip = pistol.bullets_in_clip;
	saved_pistol.total_bullets = pistol.total_bullets;
	saved_shotgun.bullets_in_clip = shotgun.bullets_in_clip;
	saved_shotgun.total_bullets = shotgun.total_bullets;
	player.saved_health = player.health;
	
}

void create_powerup(powerup* new_powerup, Uint8 id, Uint8 type, Sint16 x,Sint16 y, Sint16 z)
{
	new_powerup->id = id;
	new_powerup->type = type;
	new_powerup->x = x;
	new_powerup->y = y;
	new_powerup->z = z;
	
	//  Set Model
	new_powerup->pup_model=(XPDATA *)pup_data[type];
	
	//need to check here if item is in players inventory:
	
	if(search_for_item(new_powerup->id) !=0)
	{
	new_powerup->used = true;
	}else
	{
	new_powerup->used = false;
	}
	
}

void create_door(door* new_door, Uint8 id, Uint8 type, Uint16 locked, Sint16 new_x, Sint16 new_y, Sint16 new_z, Sint16 new_ry)
{
	new_door->id = id;
	new_door->type = type;
	new_door->new_x = new_x;
	new_door->new_y = new_y;
	new_door->new_z = new_z;
	new_door->new_ry = new_ry;
	if(locked == 0)
	{
	new_door->locked = 0;
	}else
	{
	
	//need to check here if item is in players inventory:
		
	if(search_for_item(locked) !=0 && player.a_itemdata->itemtbl[search_for_item(locked)].used)
	{
	new_door->locked = 0;
	}else
	{
	new_door->locked = locked;
	}
	
	}
		
	
}

// modified from XL2 map loading demo
void			load_binary(char * filename, void * startAddress)
{
char * stream;
 void * currentAddress;
unsigned int total_point =0;
unsigned int total_polygon=0;
unsigned int total_attribute=0;
unsigned int total_normal=0;
unsigned int total_collcubes=0;
int nxt = 0;
int line=2;
int length =0;
unsigned int att_tex = 0;
unsigned int att_plane = 0;
unsigned int att_flip = 0;
unsigned int g_count = 0;
unsigned int enemyid =0;
unsigned int enemytype =0;
int enemyx =0;
int enemyy =0;
int enemyz =0;
int enemyxdist =0;
int enemyzdist =0;
unsigned int enemyspeed =0;
unsigned int enemyhits =0;
unsigned int pupid =0;
unsigned int puptype =0;
int pupx =0;
int pupy =0;
int pupz =0;
unsigned int doorid =0;
unsigned int doorlocked =0;
unsigned int doortype = 0;
unsigned int	new_x;
unsigned int new_y;
unsigned int new_z;
unsigned int new_ry;
unsigned int shading_type = 0;


stream = jo_fs_read_file(filename, &length);
currentAddress = startAddress;

xpdata_[0]= currentAddress;

///shading type
shading_type = jo_swap_endian_uint(*((unsigned int *)(stream)));
jo_printf(0, line, "shading_type:         %d     ", shading_type);
line++;
nxt +=4;

///points
total_point = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
jo_printf(0, line, "LOADING....:                    ");
nxt +=4;

xpdata_[0]->pntbl = (POINT*)(xpdata_[0] + sizeof(unsigned int));

for (unsigned int i = 0; i<total_point; i++)
   {
	
    xpdata_[0]->pntbl[i][0] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt +=4;
	xpdata_[0]->pntbl[i][1] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt +=4;
	xpdata_[0]->pntbl[i][2] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt +=4;
	
	//jo_printf(0, line, "point: {%d,%d,%d} %d",point1,point2,point3, i);
	//line++;
	
       

    }
	xpdata_[0]->nbPoint = total_point;
	jo_printf(0, line, "total_point:          %d     ", total_point);
	line++;

///polygon

total_polygon = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
xpdata_[0]->pltbl = (POLYGON*)(xpdata_[0]->pntbl + sizeof(POINT)*total_point);
jo_printf(0, line, "LOADING....:                    ");
nxt +=4;

    for (unsigned int j = 0; j<total_polygon; j++)
   {
    xpdata_[0]->pltbl[j].norm[0] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->pltbl[j].norm[1] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->pltbl[j].norm[2] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->pltbl[j].Vertices[0] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->pltbl[j].Vertices[1] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->pltbl[j].Vertices[2] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->pltbl[j].Vertices[3] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	
	
    }
	xpdata_[0]->nbPolygon = total_polygon;
	jo_printf(0, line, "total_polygon:       %d     ", total_polygon);
	line++;


///attribute

total_attribute = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
xpdata_[0]->attbl = (ATTR*)(xpdata_[0]->pltbl + sizeof(POINT)*total_polygon);
jo_printf(0, line, "LOADING....:                    ");
nxt +=4;

    for (unsigned int k = 0; k<total_attribute; k++)
   {
     
     att_tex = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	att_plane = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	att_flip = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;


	if(shading_type == 0)
		{
			if(att_plane == 1 && att_flip == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;
			}else if(att_plane == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else if(att_flip == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}
		}else
		{
		if(att_plane == 1 && att_flip == 1)
		{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;
			}else if(att_plane == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else if(att_flip == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}
        }
		g_count++;
      
	}
	switch(shading_type)
	{
	case 1: slSetDepthTbl(DepthDataBlack,0xf000,32);
			break;
	case 2: slSetDepthTbl(DepthDataWhite,0xf000,32);
			break;
		
		
	}
	
	jo_printf(0, line, "total_attribute:     %d     ", total_polygon);
	line++;
	
///normal
total_normal = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
xpdata_[0]->vntbl = (VECTOR*)(xpdata_[0]->attbl + sizeof(POINT)*total_attribute);
jo_printf(0, line, "LOADING....:                    ");
nxt +=4;

    for (unsigned int l = 0; l<total_normal; l++)
   {
     
	xpdata_[0]->vntbl[l][0] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->vntbl[l][1] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	xpdata_[0]->vntbl[l][2] = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
    

	

    }
	
	jo_printf(0, line, "total_normal:        %d     ", total_polygon);
	line++;

///collpoints
total_collcubes = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));

currentAddress = (void*) (xpdata_[0]->vntbl + sizeof(VECTOR)*total_normal);
cdata_[0]= currentAddress;
cdata_[0]->cotbl = (COLLISON*)(cdata_[0] + sizeof(unsigned int));

jo_printf(0, line, "LOADING....:                    ");
nxt +=4;

    for (unsigned int m = 0; m<total_collcubes; m++)
   {
     
	cdata_[0]->cotbl[m].cen_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	cdata_[0]->cotbl[m].cen_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	cdata_[0]->cotbl[m].cen_z = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	cdata_[0]->cotbl[m].x_size = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	cdata_[0]->cotbl[m].y_size = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	cdata_[0]->cotbl[m].z_size = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	cdata_[0]->cotbl[m].att = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
	nxt+=4;
	
    }
	cdata_[0]->nbCo = total_collcubes;
	jo_printf(0, line, "total_collcubes      %d     ", total_collcubes);
	line++;
	
	
///ADD ENEMIES
	

jo_printf(0, line, "LOADING....:                    ");

enemy_total = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
nxt +=4;

	for (int e = 0; e<enemy_total; e++)
		{
		
		enemyid = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemytype = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemyx = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemyy = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemyz = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemyxdist = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemyzdist = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemyspeed = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		enemyhits = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;

		create_enemy(&enemies[e],enemyid, enemytype, enemyx,enemyy, enemyz, enemyxdist,enemyzdist,enemyspeed,enemyhits);
		
		
		}
		
jo_printf(0, line, "enemy_total:         %d     ", enemy_total);
line++;

jo_printf(0, line, "LOADING....:                    ");

powerup_total = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
nxt +=4;

		for (int p = 0; p<powerup_total; p++)
		{
				
		pupid = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		puptype = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		pupx = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		pupy = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		pupz = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		
		create_powerup(&powerups[p],pupid,puptype,pupx, pupy, pupz);
		
		}
	jo_printf(0, line, "powerup_total:         %d     ", powerup_total);
line++;

//DOORS
jo_printf(0, line, "LOADING....:                    ");

door_total = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
nxt +=4;

		for (Uint8 d = 0; d<door_total; d++)
		{
				
		doorid = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		doortype = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		doorlocked = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		new_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		new_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		new_z = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		new_ry = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		
		create_door(&doors[d],doorid,doortype,doorlocked,new_x,new_y,new_z,new_ry);
		
		}
	jo_printf(0, line, "door_total:         %d     ", door_total);
line++;

//CAMERA
default_cam = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
nxt +=4;
	
		cam_1_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_1_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_1_z = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_2_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_2_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_2_z = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_3_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_3_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		cam_3_z = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		
		jo_printf(0, line, "default_cam:        %d     ", default_cam);
line++;
		
//CD TRACK
cd_track = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
nxt +=4;
		jo_printf(0, line, "cd_track:           %d     ", cd_track);
line++;

//map plan
has_map_plan = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
nxt +=4;

if(has_map_plan)
{
		map_plan.min_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;	
		map_plan.max_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		map_plan.map_min_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;	
		map_plan.map_max_x = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;	
		map_plan.min_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;	
		map_plan.max_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
		map_plan.map_min_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;	
		map_plan.map_max_y = jo_swap_endian_uint(*((unsigned int *)(stream+nxt)));
		nxt+=4;
}

	
	jo_free(stream);

}
//old way using txt file - now using bin
void			load_file(char * filename, void * startAddress)
{
char * stream;
 void * currentAddress;
unsigned int total_point =0;
unsigned int total_polygon=0;
unsigned int total_attribute=0;
unsigned int total_normal=0;
unsigned int total_collcubes=0;
int nxt = 0;

int line=2;
int length =0;
int point1 =0;
int point2 =0;
int point3 =0;
unsigned int p_poly1 =0;
unsigned int p_poly2 =0;
unsigned int p_poly3 =0;
unsigned int p_poly4 =0;
unsigned int p_norm1 =0;
unsigned int p_norm2 =0;
unsigned int p_norm3 =0;
unsigned int att_tex = 0;
unsigned int att_plane = 0;
unsigned int att_flip = 0;
unsigned int norm1 = 0;
unsigned int norm2 = 0;
unsigned int norm3 = 0;
unsigned int collpoint1 = 0;
unsigned int collpoint2 = 0;
unsigned int collpoint3 = 0;
unsigned int collpoint4 = 0;
unsigned int collpoint5 = 0;
unsigned int collpoint6 = 0;
unsigned int collpoint7 = 0;
unsigned int g_count = 0;
unsigned int enemyid =0;
unsigned int enemytype =0;
int enemyx =0;
int enemyy =0;
int enemyz =0;
int enemyxdist =0;
int enemyzdist =0;
unsigned int enemyspeed =0;
unsigned int enemyhits =0;
unsigned int pupid =0;
unsigned int puptype =0;
int pupx =0;
int pupy =0;
int pupz =0;
unsigned int doorid =0;
unsigned int doorlocked =0;
unsigned int doortype = 0;
unsigned int	new_x;
unsigned int new_y;
unsigned int new_z;
unsigned int new_ry;
unsigned int shading_type = 0;



stream = jo_fs_read_file(filename, &length);
currentAddress = startAddress;

xpdata_[0]= currentAddress;

///shading type
sscanf(stream+nxt,"%d",&shading_type);
nxt+=nDigits(shading_type);

sscanf(stream+nxt,"%d",&total_point);
nxt+=nDigits(total_point);
nxt+=2;

xpdata_[0]->pntbl = (POINT*)(xpdata_[0] + sizeof(unsigned int));

jo_printf(0, line, "LOADING....:                    ");


for (unsigned int i = 0; i<total_point; i++)
{		
		
		sscanf(stream+nxt,"%d %d %d", &point1, &point2, &point3);
		nxt+= (nDigits(point1)+nDigits(point2)+nDigits(point3)+4);
					
		xpdata_[0]->pntbl[i][0] = point1;
		xpdata_[0]->pntbl[i][1] = point2;
		xpdata_[0]->pntbl[i][2] = point3;
		
		
}
jo_printf(0, line, "total_point:             %d     ", total_point);
line++;

//POLYGON
sscanf(stream+nxt,"%d",&total_polygon);
nxt+=nDigits(total_polygon);
nxt+=2;

xpdata_[0]->pltbl = (POLYGON*)(xpdata_[0]->pntbl + sizeof(POINT)*total_point);

jo_printf(0, line, "LOADING....:                    ");


for (unsigned int j = 0; j<total_polygon; j++)
{	
				
		sscanf(stream+nxt,"%d %d %d %d %d %d %d", &p_norm1, &p_norm2, &p_norm3, &p_poly1, &p_poly2, &p_poly3, &p_poly4);
		nxt+= (nDigits(p_norm1)+nDigits(p_norm2)+nDigits(p_norm3)+nDigits(p_poly1)+nDigits(p_poly2)+nDigits(p_poly3)+nDigits(p_poly4)+8);
			
		/// issue here?
		xpdata_[0]->pltbl[j].norm[0] = p_norm1;
		xpdata_[0]->pltbl[j].norm[1] = p_norm2;
		xpdata_[0]->pltbl[j].norm[2] = p_norm3;
		xpdata_[0]->pltbl[j].Vertices[0] = p_poly1;
		xpdata_[0]->pltbl[j].Vertices[1] = p_poly2;
		xpdata_[0]->pltbl[j].Vertices[2] = p_poly3;
		xpdata_[0]->pltbl[j].Vertices[3] = p_poly4;
}

jo_printf(0, line, "total_poly:              %d     ", total_polygon);
line++;

//ATTRIBUTES (texture number)
sscanf(stream+nxt,"%d",&total_attribute);
nxt+=nDigits(total_attribute);
nxt+=2;

xpdata_[0]->attbl = (ATTR*)(xpdata_[0]->pltbl + sizeof(POINT)*total_polygon);
jo_printf(0, line, "LOADING....:                    ");


	for (unsigned int k = 0; k<total_attribute; k++)
	{
		
		sscanf(stream+nxt,"%d %d %d", &att_tex, &att_plane, &att_flip);
		nxt+= (nDigits(att_tex)+nDigits(att_plane)+nDigits(att_flip)+4);
				
	//	jo_printf(0, line, "%d", att_tex,nxt);
		//line++;

		if(shading_type == 0)
		{
			if(att_plane == 1 && att_flip == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;
			}else if(att_plane == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else if(att_flip == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseGouraud|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}
		}else
		{
		if(att_plane == 1 && att_flip == 1)
		{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;
			}else if(att_plane == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Single_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else if(att_flip == 1)
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprVflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}else
			{
			ATTR bufAttr = ATTRIBUTE(Dual_Plane, SORT_MAX, MAP_TILESET+att_tex, LUTidx(MAP_TILESET+att_tex), GRreal(g_count),Window_In|MESHoff|HSSon|ECdis|CL32KRGB|CL_Gouraud, sprNoflip, UseDepth|UseClip);
			xpdata_[0]->attbl[k] = bufAttr;		
			}
        }
		g_count++;
      
	}
	switch(shading_type)
	{
	case 1: slSetDepthTbl(DepthDataBlack,0xf000,32);
			break;
	case 2: slSetDepthTbl(DepthDataWhite,0xf000,32);
			break;
		
		
	}
	

jo_printf(0, line, "total_att:               %d     ", total_attribute);
line++;
		
//NORMALS
sscanf(stream+nxt,"%d",&total_normal);
nxt+=nDigits(total_normal);
nxt+=2;

xpdata_[0]->vntbl = (VECTOR*)(xpdata_[0]->attbl + sizeof(POINT)*total_attribute);

jo_printf(0, line, "LOADING....:                    ");


	for (unsigned int l = 0; l<total_normal; l++)
	{
		
		sscanf(stream+nxt,"%d %d %d", &norm1, &norm2, &norm3);
		nxt+= (nDigits(norm1)+nDigits(norm2)+nDigits(norm3)+4);
			
		xpdata_[0]->vntbl[l][0] = norm1;
		xpdata_[0]->vntbl[l][1] = norm2;
		xpdata_[0]->vntbl[l][2] = norm3;
	}

jo_printf(0, line, "total_norm:              %d     ", total_normal);
line++;
	
//COLLISIONS
sscanf(stream+nxt,"%d",&total_collcubes);
nxt+=nDigits(total_collcubes);
nxt+=2;

currentAddress = (void*) (xpdata_[0]->vntbl + sizeof(VECTOR)*total_normal);
cdata_[0]= currentAddress;

cdata_[0]->cotbl = (COLLISON*)(cdata_[0] + sizeof(POINT)*total_attribute);
jo_printf(0, line, "LOADING....:                    ");



	for (unsigned int m = 0; m<total_collcubes; m++)
	{
		
		sscanf(stream+nxt,"%d %d %d %d %d %d %d", &collpoint1, &collpoint2, &collpoint3, &collpoint4, &collpoint5, &collpoint6, &collpoint7);
		nxt+= (nDigits(collpoint1)+nDigits(collpoint2)+nDigits(collpoint3)+nDigits(collpoint4)+nDigits(collpoint5)+nDigits(collpoint6)+nDigits(collpoint7)+8);
				
		cdata_[0]->cotbl[m].cen_x = collpoint1;
		cdata_[0]->cotbl[m].cen_y = collpoint2;
		cdata_[0]->cotbl[m].cen_z = collpoint3;
		cdata_[0]->cotbl[m].x_size = collpoint4;
		cdata_[0]->cotbl[m].y_size = collpoint5;
		cdata_[0]->cotbl[m].z_size = collpoint6;
		cdata_[0]->cotbl[m].att = collpoint7;
		
	}
jo_printf(0, line, "total_collcubes:         %d     ", total_collcubes);
line++;
	
	xpdata_[0]->nbPoint = total_point;
    xpdata_[0]->nbPolygon = total_polygon;
	cdata_[0]->nbCo = total_collcubes;
	
	
	///ADD ENEMIES
	

jo_printf(0, line, "LOADING....:                    ");


sscanf(stream+nxt,"%d",&enemy_total);
nxt+=nDigits(enemy_total);
nxt+=2;

	for (int e = 0; e<enemy_total; e++)
		{
		
		sscanf(stream+nxt,"%d %d %d %d %d %d %d %d %d", &enemyid, &enemytype, &enemyx, &enemyy, &enemyz, &enemyxdist, &enemyzdist, &enemyspeed, &enemyhits);
		nxt+= (nDigits(enemyid)+nDigits(enemytype)+nDigits(enemyx)+nDigits(enemyy)+nDigits(enemyz)+nDigits(enemyxdist)+nDigits(enemyzdist)+nDigits(enemyspeed)+nDigits(enemyhits)+10);

		create_enemy(&enemies[e],enemyid, enemytype, enemyx,enemyy, enemyz, enemyxdist,enemyzdist,enemyspeed,enemyhits);
		
		
		}
		
jo_printf(0, line, "enemy_total:         %d     ", enemy_total);
line++;
	
	
	///ADD POWERUPS
	
	jo_printf(0, line, "LOADING....:                    ");


sscanf(stream+nxt,"%d",&powerup_total);
nxt+=nDigits(powerup_total);
nxt+=2;
		for (int p = 0; p<powerup_total; p++)
		{
		sscanf(stream+nxt,"%d %d %d %d %d",&pupid, &puptype, &pupx, &pupy, &pupz);
		nxt+= (nDigits(pupid)+nDigits(puptype)+nDigits(pupx)+nDigits(pupy)+nDigits(pupz)+6);
		create_powerup(&powerups[p],pupid,puptype,pupx, pupy, pupz);
		
		}
	jo_printf(0, line, "powerup_total:         %d     ", powerup_total);
line++;

///ADD DOORS
	
	jo_printf(0, line, "LOADING....:                    ");


sscanf(stream+nxt,"%d",&door_total);
nxt+=nDigits(door_total);
nxt+=2;
		for (Uint8 d = 0; d<door_total; d++)
		{
		sscanf(stream+nxt,"%d %d %d %d %d %d %d", &doorid, &doortype, &doorlocked, &new_x, &new_y, &new_z, &new_ry);
		nxt+= (nDigits(doorid)+nDigits(doortype)+nDigits(doorlocked)+nDigits(new_x)+nDigits(new_y)+nDigits(new_z)+nDigits(new_ry)+8);
		create_door(&doors[d],doorid,doortype,doorlocked,new_x,new_y,new_z,new_ry);
		
		}
	jo_printf(0, line, "door_total:         %d     ", door_total);
line++;

/// Camera locations

sscanf(stream+nxt,"%d",&default_cam);
nxt+=nDigits(default_cam);
nxt+=2;

		sscanf(stream+nxt,"%d %d %d %d %d %d %d %d %d", &cam_1_x, &cam_1_y, &cam_1_z, &cam_2_x, &cam_2_y, &cam_2_z, &cam_3_x, &cam_3_y, &cam_3_z);
		nxt+= (nDigits(cam_1_x)+nDigits(cam_1_y)+nDigits(cam_1_z)+nDigits(cam_2_x)+nDigits(cam_2_y)+nDigits(cam_2_z)+nDigits(cam_3_x)+nDigits(cam_3_y)+nDigits(cam_3_z)+10);
	
///cd track to play

sscanf(stream+nxt,"%d",&cd_track);
nxt+=nDigits(cd_track);
nxt+=2;

	jo_free(stream);

}

void			load_map_textures(char * filename, int total_tiles)
{

	map_sprite_id = jo_sprite_add_tga_tileset("TEX", filename,JO_COLOR_Red,MAP_Tileset,total_tiles);
				
	
}

void            load_map_plan_tex(char * filename)
{
	if(has_map_plan)
	{
	jo_set_tga_palette_handling(my_tga_palette_handling2);
	map_plan_tex = jo_sprite_add_tga("BG", filename, JO_COLOR_Transparent);
	}
	
   
}

void create_player(void)
{
	
   
	//  Set alive
    player.alive = true;
	player.health = 100;
	
	// set size (hit box)
	player.xsize = 14;
	player.ysize = 16;
	player.zsize = 14;
	player.anim_speed = PLAYER_SPEED;
	player.ry = 164;
		
	player.start_x = WORLD_DEFAULT_X;
	player.start_y = WORLD_DEFAULT_Y;
	player.start_z = WORLD_DEFAULT_Z;
	
	
	
	player.jump_height = -4.0f;
	
	player.projectile_speed = 10 * framerate;
	
	//	Set Inventory Data
	player.a_itemdata = &ID_ItemsPlayer;
	player.a_item	=ID_ItemsPlayer.itemtbl;
	
	player.current_weapon = &pistol;
	pistol.bullets_in_clip = pistol.clip_size;
	pistol.total_bullets = pistol.clip_size;
	
	shotgun.bullets_in_clip = shotgun.clip_size;
	shotgun.total_bullets = shotgun.clip_size;
			
	//set gamepad
	player.gamepad = 0;
	add_item(0,0);
	
	
}

void reset_demo(void)
{
jo_clear_screen();
cam_pos_x = CAM_DEFAULT_X;
cam_pos_y = CAM_DEFAULT_Y;
cam_height = CAM_DEFAULT_Y;
cam_pos_z = CAM_DEFAULT_Z;
cam_dist = CAM_DIST_DEFAULT;
cam_angle_x = 0;
cam_angle_y = 0;
cam_angle_z = 0;
cam_target_x = 0;
cam_target_y = 0;
cam_target_z = 0;
cam_number = default_cam;

player.can_be_hurt = true;
player.shadow_y = player.y;
player.shadow_size = 1.0f;
player.key = 0;
player.speed = PLAYER_SPEED;
player.anim_frame = 0;

player.speed_y = 0;
player.health = 100;
drop_camera = false;
flash = false;
spark = false;
spark_counter = 0;
flash_counter = 0;
first_person_cam = 0;
highlight_item = 0;

clear_items();
create_player();

player.projectile_alive = false;	

}

void restart_level(void)
{

jo_clear_screen();
cam_pos_x = CAM_DEFAULT_X;
cam_pos_y = CAM_DEFAULT_Y;
cam_height = CAM_DEFAULT_Y;
cam_pos_z = CAM_DEFAULT_Z;
cam_dist = CAM_DIST_DEFAULT;
cam_angle_x = 0;
cam_angle_y = 0;
cam_angle_z = 0;
cam_target_x = 0;
cam_target_y = 0;
cam_target_z = 0;
cam_number = default_cam;

player.start_x = WORLD_DEFAULT_X;
player.start_y = WORLD_DEFAULT_Y;
player.start_z = WORLD_DEFAULT_Z;
player.can_be_hurt = true;
player.shadow_y = player.y;
player.shadow_size = 1.0f;
player.key = 0;
player.speed = PLAYER_SPEED;
player.anim_frame = 0;
player.blood = false;
player.blood_counter = 0;

player.speed_y = 0;
player.health = 100;
drop_camera = false;
flash = false;
spark = false;
spark_counter = 0;
flash_counter = 0;
first_person_cam = 0;
highlight_item = 0;

player.projectile_alive = false;	

load_items();

game.map_section = 1;
game.game_state = GAMESTATE_LOAD_LEVEL;

	
}


float Lerp(float a, float b,float t)
{
	lerp_count++;
	return a + t * (b - a);
	
}

void apply_player_gravity(void)
{
	if (player.speed_y < game.max_gravity)
	player.speed_y += game.gravity;
	
}

void apply_enemy_gravity(enemy * current_enemy)
{
	if (current_enemy->speed_y < game.max_gravity)
	current_enemy->speed_y += game.gravity;
	
}


void				player_jump(void)
{
			pcm_play(jump_sound, PCM_VOLATILE, 6);
			player.speed_y = player.jump_height;
		
}

void				player_flap(void)
{
			player.flapping = true;
			pcm_play(flap_sound, PCM_VOLATILE, 6);
			player.speed_y = -6.0f;
		
}

void				player_bounce(void)
{
			pcm_play(die_sound, PCM_VOLATILE, 6);
			player.speed_y = -8.0f;
		
}

void				player_bounceback(void)
{
			pcm_play(ouch_sound, PCM_VOLATILE, 6);
			player.speed_y = -8.0f;
		
}

void player_hurt(void)
{
	player.hurt_timer++;
			if(player.blood_counter > 0)
			{
			player.blood_counter--;	
			player.blood_y+=4;
			player.blood_size +=0.2f;
			}
			
			if(player.hurt_timer >= PLAYER_HURT_TIMER)
			{
				player.can_be_hurt = true;
				player.hurt_timer = 0;
				
			}
			if(player.health <= 0)
			{
			//player.alive = false;
			//reset_demo();
			game.game_state = GAMESTATE_DEAD;
			}

}

//	check X or Z collision
int has_horizontal_collision(Uint8 collpoints_type, Sint16 collpoints_x, Sint16 collpoints_y, Sint16 collpoints_z, Uint16 collpoints_xsize, Uint16 collpoints_ysize, Uint16 collpoints_zsize, Sint16 object_x, Sint16 object_y, Sint16 object_z, Uint16 object_xsize, Uint16 object_ysize, Uint16 object_zsize)
{
	bool x_collide = false;
	bool y_collide = false;
	bool z_collide = false;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Uint16 step;
	
			//	Reset values
			x_collide = false;
			y_collide = false;
			z_collide = false;
					 
			x_dist = JO_ABS(object_x - collpoints_x);
			y_dist = JO_ABS((object_y + object_ysize)- collpoints_y);
			z_dist = JO_ABS(object_z - collpoints_z);
			
			step = JO_ABS((object_y + object_ysize + object_ysize) - (collpoints_y - collpoints_ysize));
						
			//	Check x
			if(x_dist - (object_xsize + collpoints_xsize) <=0)
			{
				x_collide = true;
			}
			
			//	Check y
			if(y_dist - (object_ysize + collpoints_ysize) <=0)
			{
				y_collide = true;
			}
			
			//	Check z
			if(z_dist - (object_zsize + collpoints_zsize) <=0)
			{
				z_collide = true;
			}
			/*if(show_debug)
			{
			jo_nbg2_printf(0, 3, "ZCOL:\t%3d\t%3d\t%3d ",(int) x_collide, (int) y_collide, (int) z_collide);
			jo_nbg2_printf(0, 4, "ZDIS:\t%3d\t%3d\t%3d ",(int) x_dist, (int) y_dist, (int) z_dist);
			}*/
			//jo_nbg2_printf(0, 5, "Collisions:\t%d\t%d\t%d ",(int) x_collide, (int) y_collide, (int) z_collide);
			//jo_nbg2_printf(0, 6, "distance:\t%d\t%d\t%d ",(int) x_dist, (int) y_dist, (int) z_dist);
			
			//	Return if colliding
			
			
			if(x_collide && y_collide && z_collide && collpoints_type == 0)
			{
				if(show_debug)
				{
				jo_printf(0, 2, "ydist: %3d", step);
				}
				return step;
			}
		
	return 0;
}

//	check trigger collision, eg door
int has_trigger_collision(Uint8 collpoints_type, Sint16 collpoints_x, Sint16 collpoints_y, Sint16 collpoints_z, Uint16 collpoints_xsize, Uint16 collpoints_ysize, Uint16 collpoints_zsize, Sint16 object_x, Sint16 object_y, Sint16 object_z, Uint16 object_xsize, Uint16 object_ysize, Uint16 object_zsize)
{

	bool x_collide = false;
	bool y_collide = false;
	bool z_collide = false;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	
	//	Reset values
	x_collide = false;
	y_collide = false;
	z_collide = false;
					 
	if(collpoints_type > 7)
		{				
			x_dist = JO_ABS(object_x - collpoints_x);
			y_dist = JO_ABS(object_y - collpoints_y);
			z_dist = JO_ABS(object_z - collpoints_z);
				
			//	Check x
			if(x_dist - (object_xsize + collpoints_xsize) <=0)
			{
				x_collide = true;
			}
				
			//	Check y
			if(y_dist - (object_ysize + collpoints_ysize) <=0)
			{
				y_collide = true;
			}
				
			//	Check z
			if(z_dist - (object_zsize + collpoints_zsize) <=0)
			{
				z_collide = true;
			}		
				
				if(x_collide && y_collide && z_collide)
				{
					return collpoints_type;
				}
		}
	
	return NO_RAMP_COLLISION;
}

float has_vertical_collision(Uint8 collpoints_type, Sint16 collpoints_x, Sint16 collpoints_y, Sint16 collpoints_z, Uint16 collpoints_xsize, Uint16 collpoints_ysize, Uint16 collpoints_zsize, float object_int_height, Sint16 object_x, Sint16 object_y, Sint16 object_z, Uint16 object_xsize, Uint16 object_ysize, Uint16 object_zsize)
{
	bool x_collide = false;
	bool y_collide = false;
	bool z_collide = false;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Sint16 ramp_xdist;
	Sint16 ramp_zdist;
				
			//	Reset values
			x_collide = false;
			y_collide = false;
			z_collide = false;
				
			x_dist = JO_ABS(object_x - collpoints_x);
			y_dist = JO_ABS((object_y + object_ysize) - collpoints_y);
			z_dist = JO_ABS(object_z - collpoints_z);
			ramp_xdist = (collpoints_x - object_x);
			ramp_zdist = (collpoints_z - object_z);
						
			//	Check x
			if(x_dist - (object_xsize + collpoints_xsize) <=0)
			{
				x_collide = true;
			}
			
			//	Check y
			if(y_dist - (object_ysize + collpoints_ysize ) <=0)
			{				
				y_collide = true;
			}
			
			//	Check z
			if(z_dist - (object_zsize + collpoints_zsize) <=0)
			{
				z_collide = true;
			}
			/*if(show_debug)
			{
			jo_nbg2_printf(0, 5, "VCOL:\t%3d\t%3d\t%3d ",(int) x_collide, (int) y_collide, (int) z_collide);
			jo_nbg2_printf(0, 6, "VDIS:\t%3d\t%3d\t%3d ",(int) x_dist, (int) y_dist, (int) z_dist);
			}*/
			//	Return if colliding
			if(x_collide && y_collide && z_collide)//&& collpoints_type == 0
			{
				
					//check for ramp
					if(collpoints_type == 0)
					{
						object_int_height = collpoints_y - (collpoints_ysize + JO_MULT_BY_2(object_ysize));
						/*if(show_debug)
						{
						jo_nbg2_printf(0, 8, "collpoints_y:       %3d ",collpoints_y);
						jo_nbg2_printf(0, 9, "collpoints_ysize:   %3d ",collpoints_ysize);
						jo_nbg2_printf(0, 10, "object_int_height: %3d ",object_int_height);
						
						}*/
					}
					else
					{
						//check for water
						if(collpoints_type == 9)
						{
						return NO_RAMP_COLLISION;	
						}
						
					float bottom_height = 0;
					float top_height = (int) collpoints_ysize + object_ysize;
					float location = 0;
						
					if(collpoints_type == 2)
					{
					location = (1-((float)(ramp_xdist- object_xsize)/(float)collpoints_xsize))/2;
					}
					if(collpoints_type == 4)
					{
					location = (1+((float)(ramp_xdist + object_xsize)/(float)collpoints_xsize))/2;
					}
					if(collpoints_type == 1)
					{
					location = (1-((float)(ramp_zdist - object_zsize)/(float)collpoints_zsize))/2;
					}
					if(collpoints_type == 3)
					{
					location = (1+((float)(ramp_zdist + object_zsize)/(float)collpoints_zsize))/2;
					}
					
					
					object_int_height = Lerp(bottom_height,top_height,location);
					
					if(y_dist >(int) object_int_height)
					{
						/*if(show_debug)
						{
						jo_nbg2_printf(20, 1, "V object_int_height:%3d",(int) object_int_height );
						//jo_nbg2_printf(20, 7, "lerp:\t%3d\t%3d\t%3d ",(int) bottom_height,(int) top_height, (int)location * 100  );
						}*/
					return NO_RAMP_COLLISION;
					}
					
					}
				return object_int_height == NO_RAMP_COLLISION ? 1 : object_int_height;
			}
			
	return NO_RAMP_COLLISION;
	
}

float has_ramp_collision(float object_R_int_height, Uint8 collpoints_type, Sint16 collpoints_x, Sint16 collpoints_y, Sint16 collpoints_z, Uint16 collpoints_xsize, Uint16 collpoints_ysize, Uint16 collpoints_zsize,Sint16 object_x, Sint16 object_y, Sint16 object_z, Uint16 object_xsize, Uint16 object_ysize, Uint16 object_zsize)
{
	bool x_collide = false;
	bool y_collide = false;
	bool z_collide = false;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Sint16 ramp_xdist;
	Sint16 ramp_zdist;
	
	
	//set distance
	x_dist = JO_ABS(object_x - collpoints_x);
	y_dist = JO_ABS(object_y - collpoints_y);
	z_dist = JO_ABS(object_z - collpoints_z);
	
	
	if((x_dist + y_dist + z_dist) < 512)
	{

	
		//	Reset values
			x_collide = false;
			y_collide = false;
			z_collide = false;
	
			
		
			if(collpoints_type < 1 || collpoints_type >4)
			{
				return NO_RAMP_COLLISION;
			}				
		x_dist = JO_ABS(object_x - collpoints_x);
		y_dist = JO_ABS(object_y - collpoints_y);
		z_dist = JO_ABS(object_z - collpoints_z);
		ramp_xdist = (collpoints_x - object_x);
		ramp_zdist = (collpoints_z - object_z);
		
		
		
		
		//	Check x
		if(x_dist - (object_xsize + collpoints_xsize) <=0)//collpoints_x - collpoints_xsize == object_x + object_xsize || collpoints_x + collpoints_xsize == object_x - object_xsize
		{
			x_collide = true;
		}
		
		//	Check y
		if(y_dist - (object_ysize + collpoints_ysize) <=0)//y_dist - (object_ysize + collpoints_ysize) <=0)
		{
			y_collide = true;
		}
		
		//	Check z
		if(z_dist - (object_zsize + collpoints_zsize) <=0)
		{
			z_collide = true;
		}
		/*if(show_debug)
		{jo_printf(0, 2, "RCOL:\t%3d\t%3d\t%3d ",(int) x_collide, (int) y_collide, (int) z_collide);
		jo_printf(0, 3, "RDIS:\t%3d\t%3d\t%3d ",(int) x_dist, (int) y_dist, (int) z_dist);
		}*/
		//	Return if colliding
		if(x_collide && y_collide && z_collide && collpoints_type != 0)
		{
			float bottom_height = 0;
			float top_height = (int) (collpoints_ysize *2)+ object_ysize;//+ object_ysize
			float location =0;
			
			if(collpoints_type == 2)
			{
			location = (1-((float)(ramp_xdist - object_xsize) /(float)collpoints_xsize))/2;
			}
			
			if(collpoints_type == 4)
			{
			location = (1+((float)(ramp_xdist + object_xsize) /(float)collpoints_xsize))/2;
			}
			
			if(collpoints_type == 1)
			{
			location = (1-((float)(ramp_zdist - object_zsize) /(float)collpoints_zsize))/2;
			}
			
			if(collpoints_type == 3)
			{
			location = (1+((float)(ramp_zdist + object_zsize) /(float)collpoints_zsize))/2;
			}
			
			object_R_int_height = Lerp(bottom_height,top_height,location);
			
			//jo_printf(20, 7, "lerp:\t%3d\t%3d\t%3d ",(int) bottom_height,(int) top_height, (int)location * 100  );
			if(y_dist > (int) object_R_int_height)
			{
			/*if(show_debug)
			{
			jo_printf(20, 0, "R_int_height:%3d",(int) object_R_int_height );
			jo_printf(20, 4, "Rlerp:\t%3d\t%3d\t%3d ",(int) bottom_height,(int) top_height, (int)location*100 );
			}*/
			
			return NO_RAMP_COLLISION;
			}
			return object_R_int_height;
		}
		
	}
	
	return NO_RAMP_COLLISION;
	
}

float has_shadow_collision(level_section* map, Sint16 object_x, Sint16 object_y, Sint16 object_z, Uint16 object_xsize, Uint16 object_ysize, Uint16 object_zsize)
{
	bool x_collide = false;
	bool y_collide = false;
	bool z_collide = false;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Sint16 collpoints_x;
	Sint16 collpoints_y;
	Sint16 collpoints_z;
	Uint16 collpoints_xsize;
	Uint16 collpoints_ysize;
	Uint16 collpoints_zsize;
	Uint16 collpoints_total;
		
	collpoints_total = map->a_cdata->nbCo;

	for(Uint16 c = 0; c < collpoints_total; c++)
	{
		
		//	Reset values
			x_collide = false;
			y_collide = false;
			z_collide = false;
						
			collpoints_x = map->a_collison[c].cen_x + map->x + map->tx;
			collpoints_y = map->a_collison[c].cen_y + map->y + map->ty;
			collpoints_z = map->a_collison[c].cen_z + map->z + map->tz;
			
			collpoints_xsize = map->a_collison[c].x_size;
			collpoints_ysize = map->a_collison[c].y_size;
			collpoints_zsize = map->a_collison[c].z_size;
			
			
		
		x_dist = JO_ABS(object_x - collpoints_x);
		y_dist = JO_ABS(object_y - collpoints_y);
		z_dist = JO_ABS(object_z - collpoints_z);
				
		
		//	Check x
		if(x_dist - (object_xsize + collpoints_xsize) <=0)//collpoints_x - collpoints_xsize == object_x + object_xsize || collpoints_x + collpoints_xsize == object_x - object_xsize
		{
			x_collide = true;
		}
		
		//	Check y
		if(y_dist - (object_ysize + object_ysize + collpoints_ysize + collpoints_ysize) <=0)//collpoints_y - (collpoints_ysize * 2) == object_y + (object_ysize * 2)
		{				
			y_collide = true;
		}
		
		//	Check z
		if(z_dist - (object_zsize + collpoints_zsize) <=0)
		{
			z_collide = true;
		}
		/*if(show_debug)
		{
		jo_printf(0, 5, "VCOL:\t%3d\t%3d\t%3d ",(int) x_collide, (int) y_collide, (int) z_collide);
		jo_printf(0, 6, "VDIS:\t%3d\t%3d\t%3d ",(int) x_dist, (int) y_dist, (int) z_dist);
		}*/
		//	Return if colliding
		if(x_collide && y_collide && z_collide)//&& collpoints_type == 0
		{
			
			return y_dist - collpoints_ysize;
		}
		
	}
	return NO_RAMP_COLLISION;
	
}

float has_ceiling_collision(Sint16 collpoints_x, Sint16 collpoints_y, Sint16 collpoints_z, Uint16 collpoints_xsize, Uint16 collpoints_ysize, Uint16 collpoints_zsize, Sint16 object_x, Sint16 object_y, Sint16 object_z, Uint16 object_xsize, Uint16 object_ysize, Uint16 object_zsize, float object_speed_y)
{
	
	bool x_collide = false;
	bool y_collide = false;
	bool z_collide = false;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	
			//	Reset values
			x_collide = false;
			y_collide = false;
			z_collide = false;
			
			x_dist = JO_ABS(object_x - collpoints_x);
			y_dist = JO_ABS((object_y - object_ysize) - collpoints_y);
			z_dist = JO_ABS(object_z - collpoints_z);
						
			//	Check x
			if(x_dist - (object_xsize + collpoints_xsize) <=0)
			{
				x_collide = true;
			}
			
			//	Check y
			if(y_dist - (object_ysize + collpoints_ysize) <=0)
			{				
				y_collide = true;
			}
			
			//	Check z
			if(z_dist - (object_zsize + collpoints_zsize) <=0)
			{
				z_collide = true;
			}
			/*if(show_debug)
			{
			jo_nbg2_printf(0, 5, "VCOL:\t%3d\t%3d\t%3d ",(int) x_collide, (int) y_collide, (int) z_collide);
			jo_nbg2_printf(0, 6, "VDIS:\t%3d\t%3d\t%3d ",(int) x_dist, (int) y_dist, (int) z_dist);
			}*/
			//	Return if colliding
			if(x_collide && y_collide && z_collide && object_speed_y <=0)
			{
			return true;
			}
			
	return false;
	
}

bool has_object_collision(Sint16 object1_x, Sint16 object1_y, Sint16 object1_z, Uint16 object1_xsize, Uint16 object1_ysize, Uint16 object1_zsize, Sint16 object1_speed_x,Sint16 object1_speed_z,
							Sint16 object2_x, Sint16 object2_y, Sint16 object2_z, Uint16 object2_xsize, Uint16 object2_ysize, Uint16 object2_zsize)
{
	bool x_collide = false;
	bool y_collide = false;
	bool z_collide = false;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Sint16 next_pixel_x;
	Sint16 next_pixel_z;
	
	//set distance
	x_dist = JO_ABS(object1_x - object2_x);
	y_dist = JO_ABS(object1_y - object2_y);
	z_dist = JO_ABS(object1_z - object2_z);
	
	if((x_dist + y_dist + z_dist) < 250)
	{	
		//	Reset values
		x_collide = false;
		y_collide = false;
		z_collide = false;
		
				
		next_pixel_x = object1_speed_x > 0 ? object1_x + 4 :
                 object1_speed_x < 0 ? object1_x - 4 :
                 object1_x;
		next_pixel_z = object1_speed_z > 0 ? object1_z + 4 :
                 object1_speed_z < 0 ? object1_z - 4 :
                 object1_z;
		
				
		x_dist = JO_ABS(object2_x - next_pixel_x);
		y_dist = JO_ABS(object2_y - object1_y);
		z_dist = JO_ABS(object2_z - next_pixel_z);
		
		
		//	Check x
		if(x_dist - (object1_xsize + object2_xsize) <=0)
		{
			x_collide = true;
		}
		
		//	Check y
		if(y_dist - (object1_ysize + object2_ysize) <=0)
		{
			y_collide = true;
		}
		
		//	Check z
		if(z_dist - (object1_zsize + object2_zsize) <=0)
		{
			z_collide = true;
		}
		
		/*if(show_debug)
		{
		jo_printf(0, 1, "OBJECT COL:\t%3d\t%3d\t%3d ",(int) x_collide, (int) y_collide, (int) z_collide);
		
		}*/
		
		//	Return if colliding
		if(x_collide && y_collide && z_collide)
		{
			return true;
		}
		
	
	return false;
	}
	else
		return false;
	
}

void enemy_target(void)
{
	
	if (game.game_state != GAMESTATE_GAMEPLAY)
       return;
   
	int x_dist;
	int y_dist;
	int z_dist;
	//int dist_player_1;
	//int dist_player_2;
	//int rotation_difference;
	//int rotation_difference_x;
	
	for(int e = 0; e < enemy_total; e++)
	{
	
	
	//set active distance
	x_dist = JO_ABS(player.x - enemies[e].x);
	y_dist = JO_ABS(player.y - enemies[e].y);
	z_dist = JO_ABS(player.z - enemies[e].z);
	
		if((x_dist + y_dist + z_dist) < ENEMY_ACTIVE_DISTANCE)
		
		{
			
			if(!enemies[e].alert)
				{	
				
				if(enemies[e].type == 3)
				{
				pcm_play(alert2_sound, PCM_VOLATILE, 6);
				}else
				{
				pcm_play(alert_sound, PCM_VOLATILE, 6);
				}
				enemies[e].alert=true;
				}
		
	
	if(player.can_be_hurt)
	{		
	enemies[e].tx = player.x;
	enemies[e].ty = player.y;
	enemies[e].tz = player.z;
	}else
	{
	enemies[e].tx = enemies[e].start_x;
	enemies[e].ty = enemies[e].start_y;
	enemies[e].tz = enemies[e].start_z;
		
	}
	
	
	enemies[e].ry = jo_atan2f(-(enemies[e].tx - enemies[e].x),-(enemies[e].tz - enemies[e].z)) ;
	if(enemies[e].type == 2 || enemies[e].type == 3)
		
		{
	enemies[e].rx  = jo_atan2f((enemies[e].y - enemies[e].ty),(JO_ABS(enemies[e].x - enemies[e].tx) + JO_ABS(enemies[e].z - enemies[e].tz)));
		}
	
	/*
	
	enemies[e].tr = jo_atan2f(-(enemies[e].tx - enemies[e].x),-(enemies[e].tz - enemies[e].z)) ;//+ jo_sin(enemies[e].tz - enemies[e].z)
		
	rotation_difference = enemies[e].ry - enemies[e].tr;
		
	if(JO_ABS(rotation_difference) > 180)
	{
	rotation_difference += rotation_difference > 0 ? -360 : 360;	
	}
	
	if(rotation_difference <0)
	{
	   enemies[e].ry += enemies[e].max_speed;
	}

	if(rotation_difference >0)
	{
	   enemies[e].ry -= enemies[e].max_speed;
	}
	
	*/
	
	//stop and fire projectile if in range
/*	if(!enemies[e].projectile_alive && enemies[e].can_shoot)
	{
	enemies[e].pr = enemies[e].tr;	
	enemies[e].px = enemies[e].x;
	enemies[e].py = enemies[e].y;
	enemies[e].pz = enemies[e].z;
	enemies[e].ptx = enemies[e].tx;
	enemies[e].pty = enemies[e].ty;
	enemies[e].ptz = enemies[e].tz;
	enemies[e].shoot_counter = 0;
	enemies[e].projectile_alive = true;
				
	}*/
			
	}else
	{
		enemies[e].alert=false;
	}
	
	}
	

}

void enemy_collision_handling(void)
{
	if (game.game_state != GAMESTATE_GAMEPLAY)
       return;
	
	float collide;
	bool xcollide = 0;
	bool zcollide = 0;
	bool ycollide = 0;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Uint16 collpoints_total;
	Sint16 collpoints_x;
	Sint16 collpoints_y;
	Sint16 collpoints_z;
	Uint16 collpoints_xsize;
	Uint16 collpoints_ysize;
	Uint16 collpoints_zsize;
	Uint8 collpoints_type;
	
	for(int e = 0; e < enemy_total; e++)
	{
	
	enemies[e].nextx = enemies[e].x;
		enemies[e].nextz = enemies[e].z;
		enemies[e].nexty = enemies[e].y;
	
	//set draw distance
	x_dist = JO_ABS(player.x - enemies[e].x);
	y_dist = JO_ABS(player.y - enemies[e].y);
	z_dist = JO_ABS(player.z - enemies[e].z);
	
		if((x_dist + y_dist + z_dist) < ENEMY_ACTIVE_DISTANCE && enemies[e].alive)
		
		{
		enemies[e].speed_x = -((jo_cos(enemies[e].ry) + enemies[e].max_speed*jo_sin(enemies[e].ry))/32768);
		enemies[e].speed_z = -((enemies[e].max_speed*jo_cos(enemies[e].ry) - jo_sin(enemies[e].ry))/32768);
	
		enemies[e].nextx += enemies[e].speed_x;
		enemies[e].nextz += enemies[e].speed_z;
		
		if((enemies[e].type == 2 || enemies[e].type == 3) && !enemies[e].on_ladder_x &&  !enemies[e].on_ladder_z)
		{
		enemies[e].speed_y = -((jo_cos(enemies[e].rx) + enemies[e].max_speed*jo_sin(enemies[e].rx)  - jo_cos(enemies[e].rz) - enemies[e].max_speed*jo_sin(enemies[e].rz))/32768);
		}
		enemies[e].nexty += enemies[e].speed_y;	
		
	
	for(Uint16 i = 0; i < section_total; i++)
	{
		
	//set map section distance	
	x_dist = JO_ABS(player.x - map_section[i].x);
	y_dist = JO_ABS(player.y - map_section[i].y);
	z_dist = JO_ABS(player.z - map_section[i].z);
		
	section_dist = x_dist + y_dist + z_dist;
	
	collpoints_total = map_section[i].a_cdata->nbCo;
	
	for(Uint16 c = 0; c < collpoints_total; c++)
		{
			collpoints_x = map_section[i].a_collison[c].cen_x + map_section[i].x + map_section[i].tx;
			collpoints_y = map_section[i].a_collison[c].cen_y + map_section[i].y + map_section[i].ty;
			collpoints_z = map_section[i].a_collison[c].cen_z + map_section[i].z + map_section[i].tz;
			
			collpoints_xsize = map_section[i].a_collison[c].x_size;
			collpoints_ysize = map_section[i].a_collison[c].y_size;
			collpoints_zsize = map_section[i].a_collison[c].z_size;
			collpoints_type = map_section[i].a_collison[c].att;
	
	
	///VERTICAL COLLISION
			if(!ycollide)
			{
				collide = has_vertical_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,enemies[e].int_height, enemies[e].x,enemies[e].nexty,enemies[e].z,enemies[e].xsize,enemies[e].ysize,enemies[e].zsize);
				if(collide != NO_RAMP_COLLISION)
				{ 
					enemies[e].int_height = collide;
					
					if(enemies[e].speed_y >= 0.0f)
					{
					enemies[e].speed_y = 0.0f;
					enemies[e].nexty = enemies[e].int_height;
					
					}
					
					
					
				ycollide = true;
				
				
				}
				else 
				{
				//	if (i == enemies[e].current_map_section && c == enemies[e].current_collision)
				//	{
						if (!enemies[e].on_ladder_x && !enemies[e].on_ladder_z )						
						{
						//enemies[e].can_jump = false;
						if(enemies[e].type != 2 && enemies[e].type != 3)
						{
							
							apply_enemy_gravity(&enemies[e]);
							
						}
						enemies[e].rx = 0;	
						enemies[e].rz = 0;	
						}
					//}
					
				}
			}
	
	///check for X axis collision
			if(!xcollide)
			{
				collide = has_horizontal_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,enemies[e].nextx,enemies[e].y,enemies[e].z,enemies[e].xsize,enemies[e].ysize,enemies[e].zsize);
				if(collide != NO_RAMP_COLLISION)
				{ 
					enemies[e].nextx = enemies[e].x;
					xcollide = true;
					
				}else
				{
					enemies[e].on_ladder_x = false;
				}
			}else
			{
			//if(enemies[e].type == 1 )
			//		{
					enemies[e].on_ladder_x = true;
						if(enemies[e].speed_x >0)
						{
						//player.rx = -45;
						//player.rz = -90;
						enemies[e].speed_y = -enemies[e].speed_x;
						}else if (enemies[e].speed_z <0)
						{
						//player.rz = 90;
						enemies[e].speed_y = enemies[e].speed_x;
						}
				//	}	
			}
			
	///check for Z axis collision
			if(!zcollide)
			{
				collide = has_horizontal_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,enemies[e].x,enemies[e].y,enemies[e].nextz,enemies[e].xsize,enemies[e].ysize,enemies[e].zsize);
				if(collide != NO_RAMP_COLLISION)
				{ 
					enemies[e].nextz = enemies[e].z;
					zcollide = true;
					
				}else
				{
					enemies[e].on_ladder_z = false;
				}	
			}else
			{
			//if(enemies[e].type == 1 )
			//		{
					enemies[e].on_ladder_z = true;
						if(enemies[e].speed_z >0)
						{
						//player.rx = -45;
						//player.rz = -90;
						enemies[e].speed_y = -enemies[e].speed_z;
						}else if (enemies[e].speed_z <0)
						{
						//player.rz = 90;
						enemies[e].speed_y = enemies[e].speed_z;
						}
				//	}	
			}		
			
		
		
			
			
			
			
			
			
		}
	}
	
	//enemy collide with player
	
		if(has_object_collision(enemies[e].x, enemies[e].y, enemies[e].z, enemies[e].xsize, enemies[e].ysize, enemies[e].zsize, enemies[e].speed_x,enemies[e].speed_z,
								player.x, player.y, player.z, player.xsize, player.ysize, player.zsize))
		{
			
			if(player.can_be_hurt)
			{
			player.blood_counter = 10;
			player.blood_x = (player.x + enemies[e].x) /2;
			player.blood_y = enemies[e].y;
			player.blood_z = (player.z + enemies[e].z)/2;
			player.blood_size = 1;
			enemies[e].attack = true;
			player.health -=10;
			pcm_play(ouch_sound, PCM_VOLATILE, 6);
			enemies[e].nextx = enemies[e].x;
			enemies[e].nextz = enemies[e].z;
			enemies[e].can_shoot = false;	
			player.can_be_hurt = false;
			}
					
								
		}
		
		
		enemies[e].x = enemies[e].nextx;
		enemies[e].z = enemies[e].nextz;
		enemies[e].y = enemies[e].nexty;
		}
	
	}
	
	
	
		
}

void player_target(void)
{
	
	if (game.game_state != GAMESTATE_GAMEPLAY)
       return;
   
	int x_dist;
	int y_dist;
	int z_dist;
	int current_enemy_dist;
	int closest_enemy_dist;
	int closest_enemy;
	
	//int rotation_difference;
	
	closest_enemy_dist = DRAW_DISTANCE;
	closest_enemy = -1;
	for(int e = 0; e < enemy_total; e++)
	{
	
	
	//set draw distance
	x_dist = JO_ABS(player.x - enemies[e].x);
	y_dist = JO_ABS(player.y - enemies[e].y);
	z_dist = JO_ABS(player.z - enemies[e].z);
	
		if((x_dist + y_dist + z_dist) < DRAW_DISTANCE && enemies[e].alive)
		
		{

		current_enemy_dist = (x_dist + y_dist + z_dist);
		
			if(current_enemy_dist < closest_enemy_dist)
			{
			closest_enemy = e;
			closest_enemy_dist = current_enemy_dist;
			}
		}
	}
	player.target = closest_enemy;
	
	if(player.target > -1)
	{
	player.tx = enemies[player.target].x;
	player.ty = enemies[player.target].y;
	player.tz = enemies[player.target].z;

	player.ry = jo_atan2f((player.tx - player.x),(player.tz - player.z));
	player.aim_rx  = jo_atan2f((player.y-48 - player.ty),(JO_ABS(player.x - player.tx) + JO_ABS(player.z - player.tz))) + RARM_ADJ + player.recoil;
	
	}else
	{
	player.aim_rx  = RARM_ADJ + player.recoil;							
	
	}

	

}


void player_collision_handling(void)
{
	int pcollide = 0;
	int display_message = 1;
	int trigger = 0;
	int map = 0; //current map
	unsigned int item_num = 0;
	
	float collide;
	bool ycollide = 0;
	bool xcollide = 0;
	bool zcollide = 0;
	bool ccollide = 0;
	bool rcollide = 0;
	bool tcollide = 0;
	bool cam_collide = 0;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Uint16 collpoints_total;
	Sint16 collpoints_x;
	Sint16 collpoints_y;
	Sint16 collpoints_z;
	Uint16 collpoints_xsize;
	Uint16 collpoints_ysize;
	Uint16 collpoints_zsize;
	Uint8 collpoints_type;
	unsigned int d = 0;
	
	
	
	if(!player.can_be_hurt)
		{
			player_hurt();
			
		}
		
	player.nextx = player.x;
player.nexty = player.y;
player.nextz = player.z;


	player.delta_x += player.speed_x;
	player.delta_y += player.speed_y;
	player.delta_z += player.speed_z;

	player.nextx += (player.delta_x*jo_cos(player.ry) + player.delta_z*jo_sin(player.ry))/32768;
	player.nexty += player.delta_y;
	player.nextz += (player.delta_z*jo_cos(player.ry) - player.delta_x*jo_sin(player.ry))/32768;
	
	
	for(Uint16 i = 0; i < section_total; i++)
	{
		
	//set map section distance	
	x_dist = JO_ABS(player.x - map_section[i].x);
	y_dist = JO_ABS(player.y - map_section[i].y);
	z_dist = JO_ABS(player.z - map_section[i].z);
		
	section_dist = x_dist + y_dist + z_dist;
	
	collpoints_total = map_section[i].a_cdata->nbCo;
	
	for(Uint16 c = 0; c < collpoints_total; c++)
		{
			collpoints_x = map_section[i].a_collison[c].cen_x + map_section[i].x + map_section[i].tx;
			collpoints_y = map_section[i].a_collison[c].cen_y + map_section[i].y + map_section[i].ty;
			collpoints_z = map_section[i].a_collison[c].cen_z + map_section[i].z + map_section[i].tz;
			
			collpoints_xsize = map_section[i].a_collison[c].x_size;
			collpoints_ysize = map_section[i].a_collison[c].y_size;
			collpoints_zsize = map_section[i].a_collison[c].z_size;
			collpoints_type = map_section[i].a_collison[c].att;

	
	///camera trigger collision
		if(!cam_collide)
		{
			collide = has_trigger_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,player.x,player.y,player.z,player.xsize,player.ysize,player.zsize);
			if(collide >=50 && collide < 54)
				{
				cam_number = collide - 50;
				cam_collide = true;
				}
		}
	
	///has trigger collision
		if(!tcollide)
		{
			trigger = has_trigger_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,player.x,player.y,player.z,player.xsize,player.ysize,player.zsize);
			if(trigger != NO_RAMP_COLLISION)
			{ 
			if(trigger >=10 && trigger <50)
			{//door
				if (player.action)
			 {
				 //find door ID
				 for (d = 0; d<door_total; d++)
				 {
					 if(doors[d].id == trigger - 10)
						break;
				 }
				 
				 
					if(doors[d].locked == 0)
					{
						
					//change colour to green
					if(has_map_plan)
					{
					map_plan_pal.data[255-(trigger - 10)] = JO_COLOR_RGB(126, 255, 0);	
					}	
					player.start_x = doors[d].new_x;
					player.start_y = doors[d].new_y;
					player.start_z = doors[d].new_z;
					player.ry = doors[d].new_ry;
						if(doors[d].type == 0)
						{
							game.map_section = trigger - 10;
							game.game_state = GAMESTATE_LOAD_AREA;
						}else
						{
							game.level = trigger - 10;
							game.map_section = 1;
							game.game_state = GAMESTATE_LOAD_LEVEL;
						}
						
					
					}
					
					else
					{
						
						if(search_for_item(doors[d].locked) !=0)
						{
						player.a_itemdata->itemtbl[search_for_item(doors[d].locked)].used = true;
						doors[d].locked = 0;
						game.message_timer = MESSAGE_TIMER;
						jo_printf(16,20, "Used Key");
						}else if(doors[d].locked == 99)
						{
						
						//change colour to red
						if(has_map_plan)
						{
						map_plan_pal.data[255-(trigger-10)] = JO_COLOR_RGB(204, 0, 0);	
						}
						game.message_timer = MESSAGE_TIMER;
						jo_printf(16,20, "Lock Broken");	
						}else
						{
						//change colour to orange
						if(has_map_plan)
						{
						map_plan_pal.data[255-(trigger - 10)] = JO_COLOR_RGB(255, 180, 0);	
						}
						game.message_timer = MESSAGE_TIMER;
						jo_printf(16,20, "Locked");	
							
						}
						
					}
			 }
			}
			
			if(trigger ==9)
			{//player death zone
			player.health = 0;
			player.can_be_hurt = false;
			
			}
			
			if(trigger ==8)
			{//ladder	
			player.type = 1;
			
			}else
			{
			player.type = 0;
			player.on_ladder_x = false;
			player.on_ladder_z = false;
			}	
				/*if(show_debug)
				{
				jo_printf(0, 5, "trigger: %2d",trigger );
				
				}*/
				
			if(trigger >= 60 && trigger<70 && display_message == 1)
			{
			//investigate message	
			game.message_timer = MESSAGE_TIMER;
			switch(trigger)
			{
			case 60: 	jo_printf(16,20, "Investigate");
						break;
			case 61:    jo_printf(0,20, "This is the end... for now!");
						break;
						
			case 62:    jo_printf(16,20, "Thanks for playing!");
						break;
				
			}
				
			display_message = 0;
			
			}else
			{
			tcollide = true;
			}
			
			}else
			{	/*if(show_debug)
				{
				jo_printf(0, 5, "trigger:  0 ");
				}*/
				player.type = 0;
			player.on_ladder_x = false;
			player.on_ladder_z = false;
			//jo_printf(16,20, "      ");
			}
		
		}
			
		
		
		if(show_debug)
				{
				jo_printf(0, 5, "trigger: %2d %d",trigger, d);
				
				}
				
				
				///check for ramp collision
		/*if(!rcollide)
		{
			collide = has_ramp_collision(player.R_int_height,collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,player.nextx,player.nexty,player.nextz,player.xsize,player.ysize,player.zsize);
			if(collide != NO_RAMP_COLLISION)
			{
				player.R_int_height = collide;
				rcollide = true;
				if((int)player.R_int_height > player.ramp_height_adj)
				{
				player.y -= ((int)player.R_int_height - player.ramp_height_adj);
				delta_y -= ((int)player.R_int_height - player.ramp_height_adj);
				player.nexty -= ((int)player.R_int_height - player.ramp_height_adj); 
				player.ramp_height_adj = (int)player.R_int_height;
				}
			}
			else
			{
			player.ramp_height_adj = player.R_int_height;	
			}
		}*/
		
		//VERTICAL COLLISION
			if(!ycollide)
			{
				collide = has_vertical_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,player.int_height, player.x,player.nexty,player.z,player.xsize,player.ysize,player.zsize);
				if(collide != NO_RAMP_COLLISION)
				{ 
					player.int_height = collide;
					
					if(player.speed_y <= 0.0f)
					{player.can_jump = true;
					}
					else
					{
					player.speed_y = 0.0f;
					delta_y = 0.0f;
					player.nexty = player.int_height;
					/*if(!rcollide)
					{
					player.nexty = player.int_height;
					}*/
							
					}
					
					player.current_map_section = i;
					player.current_collision = c;
				ycollide = true;//break;
				
				
				}
				else 
				{
					if (i == player.current_map_section && c == player.current_collision)
					{
						if (!player.on_ladder_x && !player.on_ladder_z )//&&!player.on_ceiling )
						{
						player.can_jump = false;
						apply_player_gravity();
						
						player.rx = 0;	
						player.rz = 0;	
						}
					}
					
				}
			}
	
		///check for X axis collision
			if(!xcollide)
			{
				collide = has_horizontal_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,player.nextx,player.y,player.z,player.xsize,player.ysize,player.zsize);
				if(collide != NO_RAMP_COLLISION)
				{ 
					
					if(collide < 21)
					{
					player.nexty -=collide;
					}else
					{
					player.nextx = player.x;
					}
					xcollide = true;
					
					
				}else
				{
					player.on_ladder_x = false;
					
				}
			}else
			{
			//ladder
					if(player.type == 1 )
					{
					player.on_ladder_x = true;
						if(player.speed_z >0)
						{
						
						player.speed_y = -player.speed_z;
						}else if (player.speed_z <0)
						{
						
						player.speed_y = player.speed_z;
						}
					}	
			}
			
		
		
				///check for Z axis collision
			if(!zcollide)
			{
				collide = has_horizontal_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,player.x,player.y,player.nextz,player.xsize,player.ysize,player.zsize);
				if(collide != NO_RAMP_COLLISION)
				{ 
					
				if(collide < 21)
					{
					player.nexty -=collide;
					}else
					{
				player.nextz = player.z;
					}
			
				zcollide = true;
					
				}else
				{
				player.on_ladder_z = false;	
				}
			
			}else
			{
			//ladder
					if(player.type == 1 )
					{
					player.on_ladder_z = true;
						if(player.speed_z >0)
						{
						player.speed_y = -player.speed_z;
						}else if (player.speed_z <0)
						{
						player.speed_y = player.speed_z;
						}
						
						
						
					}	
			}
		
		/*if(!ccollide && collpoints_type == 0)
			{
				//CEILING COLLISION
				//collide = has_ceiling_collision(&map_section[i],player.x,player.y,player.z,player.xsize,player.ysize,player.zsize,player.speed_y);
				collide = has_ceiling_collision(collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,player.x,player.nexty,player.z,player.xsize,player.ysize,player.zsize,player.speed_y);
				if(collide != NO_RAMP_COLLISION)
				{
					player.speed_y = 0.0f;
						delta_y = 0.0f;
						apply_player_gravity();
						player.nexty = player.y;
					
					player.current_map_section = i;
					ccollide = true;
				}else
				{	
					
					//player.on_ceiling = false;//remove for section fix
					
				}
			}
			*/
		
			
		}
		
		
	}
		
	///has enemy collision
	for(int e = 0; e < enemy_total; e++)
	{
		
		if(enemies[e].alive)
		{
		
			
			
			//player enemy x collision
			if(has_object_collision(player.nextx, player.y, player.z, player.xsize, player.ysize, player.zsize, player.speed_x,player.speed_z,
								enemies[e].x, enemies[e].y, enemies[e].z, enemies[e].xsize, enemies[e].ysize, enemies[e].zsize))
			{
				
				player.nextx = player.x;
				
									
			}
			
			//player enemy z collision
			if(has_object_collision(player.x, player.y, player.nextz, player.xsize, player.ysize, player.zsize, player.speed_x,player.speed_z,
								enemies[e].x, enemies[e].y, enemies[e].z, enemies[e].xsize, enemies[e].ysize, enemies[e].zsize))
			{
				
				player.nextz = player.z;
				
									
			}
		
		}
	}
	
	///has powerup collision
	for(int p = 0; p < powerup_total; p++)
	{
		if(!powerups[p].used)
		{
			if(has_object_collision(player.x, player.y, player.z, player.xsize, player.ysize, player.zsize, player.speed_x,player.speed_z,
								powerups[p].x, powerups[p].y, powerups[p].z, 32, 32, 32))
			{
			pcollide = true;
			highlight_item = powerups[p].id;
			if (player.action)
			{
			pcm_play(pup_sound, PCM_VOLATILE, 6);
			powerups[p].used = true;
			add_item(powerups[p].id,powerups[p].type);
			item_num = search_for_item(powerups[p].id);
			game.message_timer = MESSAGE_TIMER;
			 jo_printf(10, 20, "You got %s", player.a_itemdata->itemtbl[item_num].name);
			}
			}
			
			
		}
	}
	
	if(!pcollide)
	{
	highlight_item = 0;	
	
	}
	
	//message box
	if(game.message_timer <=0)
	{
	jo_printf(0,20, "                                        ");
	}else
	{
	slCurWindow(winNear);
	
	jo_sprite_enable_half_transparency();
		jo_sprite_change_sprite_scale_xy(20,1);
		jo_sprite_draw3D(28,0, 43, 100);
		jo_sprite_restore_sprite_scale();
		
		jo_sprite_disable_half_transparency();	
	game.message_timer--;

slCurWindow(winFar);	
	}
	
	collpoints_total = map_section[map].a_cdata->nbCo;
	///projectile collide with map
	if(player.projectile_alive)
	{
		for(int c = 0; c < collpoints_total; c++)
		{
			
		
	
		collpoints_x = map_section[map].a_collison[c].cen_x + map_section[map].x + map_section[map].tx;
		collpoints_y = map_section[map].a_collison[c].cen_y + map_section[map].y + map_section[map].ty;
		collpoints_z = map_section[map].a_collison[c].cen_z + map_section[map].z + map_section[map].tz;
			
		collpoints_xsize = map_section[map].a_collison[c].x_size;
		collpoints_ysize = map_section[map].a_collison[c].y_size;
		collpoints_zsize = map_section[map].a_collison[c].z_size;
		collpoints_type = map_section[map].a_collison[c].att;
		
		if(collpoints_type ==0)
		{
		if(has_object_collision(player.px, player.py, player.pz, 10, 10, 10, player.speed_px,player.speed_px,
									collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize))
				{
				spark = true;
				spark_type = 2;
				spark_counter = 0;
				player.projectile_alive = false;
				}

		}
		}
			
	}
	
	
	
	/// projectile collide with enemy
	for(int e = 0; e < enemy_total; e++)
	{
		if(!enemies[e].can_be_hurt)
		{
		enemies[e].hurt = true;
			
		}
		
		if(enemies[e].alive && enemies[e].can_be_hurt && player.projectile_alive)
		{
			if(has_object_collision(player.px, player.py, player.pz, 10, 10, 10, player.speed_px,player.speed_pz,
										enemies[e].x, enemies[e].y, enemies[e].z, enemies[e].xsize, enemies[e].ysize, enemies[e].zsize))
			{
			spark = true;
			spark_type = 1;
			spark_counter = 0;
				
			
			player.projectile_alive = false;
			enemies[e].health -= player.current_weapon->damage;
			enemies[e].can_be_hurt = false;
													
			}
		}
	}
		
	///fire projectile
	if (player.shoot)
	{
		flash = true;
		flash_counter = 0;
		
		
			switch(player.current_weapon->type)
			{
			case 0: pcm_play(pistol_sound, PCM_VOLATILE, 6);
					break;
			case 1: pcm_play(shotgun_sound, PCM_VOLATILE, 6);
					break;
			
				
			}
		
			
	player.projectile_alive = true;	
		
	player.px = player.x;
	player.py = player.y-48;
	player.pz = player.z;
	
	
	player.speed_px = (jo_cos(player.ry) + player.projectile_speed*jo_sin(player.ry))/32768;
	player.speed_py = (jo_cos(player.aim_rx - RARM_ADJ) + player.projectile_speed*jo_sin(player.aim_rx + RARM_ADJ) - jo_cos(player.rz) - player.projectile_speed*jo_sin(player.rz))/32768;
	player.speed_pz = (player.projectile_speed*jo_cos(player.ry) - jo_sin(player.ry))/32768;
	
	
	
	player.current_weapon->bullets_in_clip --;
	player.current_weapon->total_bullets --;
	
	}
	
	if(!cam_collide)
	{
	cam_number = default_cam;	
	}
	
	
	
	player.x = player.nextx;
	player.y = player.nexty;
	player.z = player.nextz;
	
	
	if(show_debug)
					{
					
					jo_printf(0, 6, "LADDER  %d %d" ,player.on_ladder_x, xcollide);
					jo_printf(0, 7, "CEILING %d" ,ccollide);
					
					}
	
		
	
	
	
}




void destroy_last_map_section(void)
{
section_total--;
	
}

void create_map_section(level_section* section, Uint8 type, Sint16 x, Sint16 y, Sint16 z)
{
	
	// set type
	section->type = type;
	
	//  Set Location
    section->x = x;
    section->y = y;
    section->z = z;
		
	//  Set Model
	section->map_model=xpdata_[0];
	//	Set Collision Data
	section->a_cdata=cdata_[0];
	section->a_collison	=cdata_[0]->cotbl;
	
	
}

/**Taken from RB demo**/
static FIXED light[XYZ];
static ANGLE light_ang[XYZ];
void computeLight()
{
    FIXED light_init[XYZ] = { toFIXED(0.57735026), toFIXED(0.57735026), toFIXED(0.57735026) };

    slPushUnitMatrix();
    {

        slRotX(light_ang[X]);
        slRotY(light_ang[Y]);
        slRotZ(light_ang[Z]);
        slCalcVector(light_init, light);
    }
    slPopMatrix();
	
	
	light[X] = toFIXED(0.0);
        light[Y] = toFIXED(0.851);
        light[Z] = toFIXED(0.451);
	
}

void				player_animation_handling(void)
{



//animation
	ANIMDATA		*a_animdata;
	ANIM_FRAME *a_frame;
	

if((player.on_ladder_x || player.on_ladder_z) && player.speed_y != 0)
{
a_animdata = &Anim_player_ladder;
total_frames = a_animdata->nbAn;
a_frame = a_animdata->animtbl;
anim_loop = 1;

if(player.anim_frame == 1)
			{
			pcm_cease(ladder_sound);
			pcm_play(ladder_sound, PCM_VOLATILE, 6);
			}
	if(player.anim_frame == 12)
			{
			pcm_cease(ladder_sound);
			pcm_play(ladder_sound, PCM_VOLATILE, 6);
			}
	
}else if(player.speed_y != 0)
{
a_animdata = &Anim_player_jump;
total_frames = a_animdata->nbAn;
a_frame = a_animdata->animtbl;	
anim_loop = 0;
	
}else if ((player.speed_x != 0 || player.speed_z != 0)&& (player.speed_y == 0))//walk
{
	
if(player.run)
{
a_animdata = &Anim_player_run;
total_frames = a_animdata->nbAn;
a_frame = a_animdata->animtbl;	
anim_loop = 1;
}else
{
a_animdata = &Anim_player_walk;
total_frames = a_animdata->nbAn;
a_frame = a_animdata->animtbl;
anim_loop = 1;
}


	if(player.anim_frame == 1)
			{
			//foot on floor - make walking sound
			//pcm_cease(step_sound);
			pcm_play(step_sound, PCM_VOLATILE, 6);
			
			}
	if(player.anim_frame == 12)
			{
			//pcm_cease(step_sound);
			pcm_play(step_sound, PCM_VOLATILE, 6);
			}

}else
{//idle
a_animdata = &Anim_player_idle;
total_frames = a_animdata->nbAn;
a_frame = a_animdata->animtbl;
}

if(player.anim_frame > total_frames)
		{
		player.anim_frame = 1;
		}
	
	player.y_adj = a_frame[player.anim_frame].y_adj;
	player.bodyt_rx = a_frame[player.anim_frame].bodyt_rx;
	player.bodyt_ry = a_frame[player.anim_frame].bodyt_ry;
	player.bodyt_rz = a_frame[player.anim_frame].bodyt_rz;
	player.head_rx = a_frame[player.anim_frame].head_rx;
	player.head_ry = a_frame[player.anim_frame].head_ry + player.head_cam_adj;
	player.head_rz = a_frame[player.anim_frame].head_rz;
	player.rarm_rx = a_frame[player.anim_frame].rarm_rx;
	player.rarm_ry = a_frame[player.anim_frame].rarm_ry;
	player.rarm_rz = a_frame[player.anim_frame].rarm_rz;
	player.rarm2_rx = a_frame[player.anim_frame].rarm2_rx;
	player.rarm2_ry = a_frame[player.anim_frame].rarm2_ry;
	player.rarm2_rz = a_frame[player.anim_frame].rarm2_rz;
	player.larm_rx = a_frame[player.anim_frame].larm_rx;
	player.larm_ry = a_frame[player.anim_frame].larm_ry;
	player.larm_rz = a_frame[player.anim_frame].larm_rz;
	player.larm2_rx = a_frame[player.anim_frame].larm2_rx;
	player.larm2_ry = a_frame[player.anim_frame].larm2_ry;
	player.larm2_rz = a_frame[player.anim_frame].larm2_rz;
	player.bodyb_rx = a_frame[player.anim_frame].bodyb_rx;
	player.bodyb_ry = a_frame[player.anim_frame].bodyb_ry;
	player.bodyb_rz = a_frame[player.anim_frame].bodyb_rz;	
	player.rleg_rx = a_frame[player.anim_frame].rleg_rx;
	player.rleg_ry = a_frame[player.anim_frame].rleg_ry;
	player.rleg_rz = a_frame[player.anim_frame].rleg_rz;
	player.rleg2_rx = a_frame[player.anim_frame].rleg2_rx;
	player.rleg2_ry = a_frame[player.anim_frame].rleg2_ry;
	player.rleg2_rz = a_frame[player.anim_frame].rleg2_rz;
	player.rleg3_rx = a_frame[player.anim_frame].rleg3_rx;
	player.rleg3_ry = a_frame[player.anim_frame].rleg3_ry;
	player.rleg3_rz = a_frame[player.anim_frame].rleg3_rz;
	player.lleg_rx = a_frame[player.anim_frame].lleg_rx;
	player.lleg_ry = a_frame[player.anim_frame].lleg_ry;
	player.lleg_rz = a_frame[player.anim_frame].lleg_rz;
	player.lleg2_rx = a_frame[player.anim_frame].lleg2_rx;
	player.lleg2_ry = a_frame[player.anim_frame].lleg2_ry;
	player.lleg2_rz = a_frame[player.anim_frame].lleg2_rz;
	player.lleg3_rx = a_frame[player.anim_frame].lleg3_rx;
	player.lleg3_ry = a_frame[player.anim_frame].lleg3_ry;
	player.lleg3_rz = a_frame[player.anim_frame].lleg3_rz;
	
	if(player.aim)
	{
	switch(player.current_weapon->type)
		{
		case 0: player.rarm_rx = player.aim_rx;
				break;
				
		case 1: player.rarm2_rx = player.aim_rx;
				break;
			
		}
		
		if(player.shoot)
		{
		player.recoil = 10;	
		}else
		{
		player.recoil = 0;
		player.head_rx = player.aim_rx - RARM_ADJ;
		}
	}
	
	
	
	
		
	
	//loop
	if(anim_loop)
	{
		if(player.anim_frame >= total_frames-1)
		{
		player.anim_frame = 1;
		}else //if(game.frame_counter == a_speed)
		{
		player.anim_frame ++;		
		}
	}else if(player.anim_frame < total_frames-1)
	{
	player.anim_frame ++;		
	}
	

}



void				animate_enemies(enemy* current_enemy)
{
switch(current_enemy->type)

{
	case 1:
	//jump
		if(current_enemy->speed_y != 0)
		{
		current_enemy->body_rx = -16;	
		}
		else
		{
		current_enemy->body_rx = 0;	
		}
		

		if((current_enemy->speed_x != 0 || current_enemy->speed_z != 0) && current_enemy->speed_y == 0)//walk x 
			{
				
				if(current_enemy->body_ry == -16)
				{current_enemy->anim_speed = ANIM_SPEED;
				}
				else
				if(current_enemy->body_ry == 16)
				{current_enemy->anim_speed = -ANIM_SPEED;
				}
			
				current_enemy->body_ry += current_enemy->anim_speed;
				current_enemy->larm_rx += current_enemy->anim_speed;
				current_enemy->rarm_rx -= current_enemy->anim_speed;
				current_enemy->lleg_rx -= current_enemy->anim_speed;
				current_enemy->rleg_rx += current_enemy->anim_speed;
				
				if(current_enemy->rleg_rx == 0 || current_enemy->lleg_rx == 0)
				{
				//foot on floor - make walking sound and add dust clouds
				//jo_printf(0, 0, "STEP");
				//jo_audio_play_sound_on_channel(&step_sound, 2);
				
				}/*else
				{
				jo_printf(0, 0, "    ");	
				}*/
			}else
			{	current_enemy->body_ry = 0;
				current_enemy->body_rx = 0;
				current_enemy->larm_rx = 0;
				current_enemy->rarm_rx = 0;
				current_enemy->lleg_rx = 0;
				current_enemy->rleg_rx = 0;
				
			}
		break;
		
	case 2:

			if(current_enemy->speed_x != 0 || current_enemy->speed_z != 0)//fly
			{
				
				if(current_enemy->rleg_rx <= -44)
				{current_enemy->anim_speed = 4;
				}
				else
				if(current_enemy->rleg_rx >= 44)
				{current_enemy->anim_speed = -4;
				}
			
				current_enemy->rleg_rx += current_enemy->anim_speed;
				current_enemy->larm_rz -= current_enemy->anim_speed;
				current_enemy->rarm_rz += current_enemy->anim_speed;
				
				
				if(current_enemy->rarm_rz == 44 )
				{
				//flap - make flapping sound
				pcm_play(flap_sound, PCM_VOLATILE, 6);
				
				}/*else
				{
				jo_printf(0, 0, "    ");	
				}*/
			}
			else
			{	current_enemy->rleg_rx = 0;
				current_enemy->larm_rz = 0;
				current_enemy->rarm_rz = 0;
			
				
			}
			break;
			
	case 3:

			if(current_enemy->speed_x != 0 || current_enemy->speed_z != 0)//fly
			{
				
				if(current_enemy->rleg_rx <= -44)
				{current_enemy->anim_speed = 16;
				}
				else
				if(current_enemy->rleg_rx >= 44)
				{current_enemy->anim_speed = -16;
				}
			
				current_enemy->rleg_rx += current_enemy->anim_speed;
				current_enemy->larm_rz -= current_enemy->anim_speed;
				current_enemy->rarm_rz += current_enemy->anim_speed;
				
				
				if(current_enemy->rarm_rz == 44 )
				{
				//flap - make flapping sound
				pcm_play(flap_sound, PCM_VOLATILE, 6);			
				
				}/*else
				{
				jo_printf(0, 0, "    ");	
				}*/
			}
			else
			{	current_enemy->rleg_rx = 0;
				current_enemy->larm_rz = 0;
				current_enemy->rarm_rz = 0;
			
				
			}
			break;
	
}
	
	if(current_enemy->attack)
	{
		current_enemy->attack_timer ++;
		
		if(current_enemy->attack_timer >= 20)
		{current_enemy->attack_timer = 0;
			current_enemy->attack = false;
		}
		current_enemy->head_rz = -16;	
		current_enemy->body_rz = 16;
		
	}else if(current_enemy->hurt)
		
		{
			current_enemy->hurt_timer++;
			current_enemy->head_rz = 16;	
			current_enemy->body_rz = -16;
			
			if(current_enemy->hurt_timer >= ENEMY_HURT_TIMER)
			{
				current_enemy->can_be_hurt = true;
				current_enemy->hurt = false;
				current_enemy->hurt_timer = 0;
				
			}
			if(current_enemy->health <= 0)
			{
			current_enemy->alive = false;
			pcm_play(die_sound, PCM_VOLATILE, 6);
			add_item(current_enemy->id,8);
			}
			
		}else
	{
		current_enemy->head_rz = 0;	
		current_enemy->body_rz = 0;
	}
	
}

void				draw_enemies(enemy* current_enemy)
{
	

	if(current_enemy->alive)
	{
		
		switch(current_enemy->type)
		{
			case 1:
		
			slPushMatrix();
			{
				slTranslate(toFIXED(current_enemy->x), toFIXED(current_enemy->y), toFIXED(current_enemy->z));
				slRotX(DEGtoANG(current_enemy->rx)); slRotY(DEGtoANG(current_enemy->ry)); slRotZ(DEGtoANG(current_enemy->rz));
				slRotX(DEGtoANG(current_enemy->body_rx)); slRotY(DEGtoANG(current_enemy->body_ry)); slRotZ(DEGtoANG(current_enemy->body_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_spider_body, light);
				
				slPushMatrix();
				{
				slTranslate(toFIXED(0), toFIXED(-5), toFIXED(0));
				slRotX(DEGtoANG(current_enemy->head_rx)); slRotY(DEGtoANG(current_enemy->head_ry)); slRotZ(DEGtoANG(current_enemy->head_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_spider_head, light);
				}
				slPopMatrix();
				
				slPushMatrix();
				{
				slTranslate(toFIXED(0), toFIXED(-5), toFIXED(0));
				slRotX(DEGtoANG(current_enemy->larm_rx)); slRotY(DEGtoANG(current_enemy->larm_ry)); slRotZ(DEGtoANG(current_enemy->larm_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_spider_LL1, light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slTranslate(toFIXED(0), toFIXED(-5), toFIXED(0));
				slRotX(DEGtoANG(current_enemy->rarm_rx)); slRotY(DEGtoANG(current_enemy->rarm_ry)); slRotZ(DEGtoANG(current_enemy->rarm_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_spider_RL1, light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slTranslate(toFIXED(0), toFIXED(5), toFIXED(0));
				slRotX(DEGtoANG(current_enemy->lleg_rx)); slRotY(DEGtoANG(current_enemy->lleg_ry)); slRotZ(DEGtoANG(current_enemy->lleg_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_spider_LL2, light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slTranslate(toFIXED(0), toFIXED(5), toFIXED(0));
				slRotX(DEGtoANG(current_enemy->rleg_rx)); slRotY(DEGtoANG(current_enemy->rleg_ry)); slRotZ(DEGtoANG(current_enemy->rleg_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_spider_RL2, light);
				}
				slPopMatrix();
			}
			slPopMatrix();
			
			
			
			break;
			
			case 2:
		
			
			
			slPushMatrix();
			{
				slTranslate(toFIXED(current_enemy->x), toFIXED(current_enemy->y), toFIXED(current_enemy->z));
				slRotX(DEGtoANG(current_enemy->rx)); slRotY(DEGtoANG(current_enemy->ry)); slRotZ(DEGtoANG(current_enemy->rz));
				slRotX(DEGtoANG(current_enemy->body_rx)); slRotY(DEGtoANG(current_enemy->body_ry)); slRotZ(DEGtoANG(current_enemy->body_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_bat_body, light);
							
				slPushMatrix();
				{
				slRotX(DEGtoANG(current_enemy->head_rx)); slRotY(DEGtoANG(current_enemy->head_ry)); slRotZ(DEGtoANG(current_enemy->head_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(-14.78));
				//jo_3d_set_scalef(0.5,0.5,0.5);				
				slPutPolygonX((XPDATA *)&xpdata_bat_head, light);
				}
				slPopMatrix();
				
				slPushMatrix();
				{
				slRotX(DEGtoANG(current_enemy->larm_rx)); slRotY(DEGtoANG(current_enemy->larm_ry)); slRotZ(DEGtoANG(current_enemy->larm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)&xpdata_bat_lwing, light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(current_enemy->rarm_rx)); slRotY(DEGtoANG(current_enemy->rarm_ry)); slRotZ(DEGtoANG(current_enemy->rarm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)&xpdata_bat_rwing, light);
				}
				slPopMatrix();
						
				slPushMatrix();
				{
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(12.5));
				slRotX(DEGtoANG(current_enemy->rleg_rx)); slRotY(DEGtoANG(current_enemy->rleg_ry)); slRotZ(DEGtoANG(current_enemy->rleg_rz));
				slPutPolygonX((XPDATA *)&xpdata_bat_body_bottom, light);
				}
				slPopMatrix();
				
			}
			slPopMatrix();
			
			
			
			
			
			break;
			
			case 3:
		
			
			
			slPushMatrix();
			{
				slTranslate(toFIXED(current_enemy->x), toFIXED(current_enemy->y), toFIXED(current_enemy->z));
				slRotX(DEGtoANG(current_enemy->rx)); slRotY(DEGtoANG(current_enemy->ry)); slRotZ(DEGtoANG(current_enemy->rz));
				slRotX(DEGtoANG(current_enemy->body_rx)); slRotY(DEGtoANG(current_enemy->body_ry)); slRotZ(DEGtoANG(current_enemy->body_rz));
				
				slPutPolygonX((XPDATA *)&xpdata_sm_bat_body, light);
							
				slPushMatrix();
				{
				slRotX(DEGtoANG(current_enemy->head_rx)); slRotY(DEGtoANG(current_enemy->head_ry)); slRotZ(DEGtoANG(current_enemy->head_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(-4));
				//jo_3d_set_scalef(0.5,0.5,0.5);				
				slPutPolygonX((XPDATA *)&xpdata_sm_bat_head, light);
				}
				slPopMatrix();
				
				slPushMatrix();
				{
				slRotX(DEGtoANG(current_enemy->larm_rx)); slRotY(DEGtoANG(current_enemy->larm_ry)); slRotZ(DEGtoANG(current_enemy->larm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)&xpdata_sm_bat_lwing, light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(current_enemy->rarm_rx)); slRotY(DEGtoANG(current_enemy->rarm_ry)); slRotZ(DEGtoANG(current_enemy->rarm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)&xpdata_sm_bat_rwing, light);
				}
				slPopMatrix();
						
				
				
			}
			slPopMatrix();
			
			
			
			
			
			break;
		}
	
	
	
	
	
	
	
	//move enemy around path
		
	/*if(current_enemy->x + current_enemy->z == current_enemy->start_x + current_enemy->start_z)
	{current_enemy->waypoint = 0;
	}
	
	if(current_enemy->x == current_enemy->start_x + current_enemy->xdist && current_enemy->z == current_enemy->start_z)
	{current_enemy->waypoint = 1;
	}
	
	if(current_enemy->x == current_enemy->start_x + current_enemy->xdist && current_enemy->z == current_enemy->start_z + current_enemy->zdist)
	{current_enemy->waypoint = 2;
	}
	
	if(current_enemy->x == current_enemy->start_x && current_enemy->z == current_enemy->start_z + current_enemy->zdist)
	{current_enemy->waypoint = 3;
	}
	
	if(current_enemy->waypoint == 0)
	{current_enemy->speed_x = 1;
	current_enemy->speed_z = 0;
	current_enemy->ry = -90;
	}else if(current_enemy->waypoint == 1)
	{current_enemy->speed_z = 1;
	current_enemy->speed_x = 0;
	current_enemy->ry = 180;
	}else if(current_enemy->waypoint == 2)
	{current_enemy->speed_x = -1;
	current_enemy->speed_z = 0;
	current_enemy->ry = 90;
	}else if(current_enemy->waypoint == 3)
	{current_enemy->speed_z = -1;
	current_enemy->speed_x = 0;
	current_enemy->ry = 0;
	}
	
	current_enemy->x += current_enemy->speed_x;
	current_enemy->z += current_enemy->speed_z;
	*/
	
	}
	
	
	
}

void				draw_powerups(powerup* current_powerup)
{
	
	if(current_powerup->id == highlight_item)
	{
	slSetGouraudColor(CD_Yellow);
	}
	else
	{
		slSetGouraudColor(CD_White);
	}

	if(!current_powerup->used)
	{
		
		slPushMatrix();
				{
				slTranslate(toFIXED(current_powerup->x), toFIXED(current_powerup->y), toFIXED(current_powerup->z));
				slRotX(DEGtoANG(0)); slRotY(DEGtoANG(current_powerup->ry)); slRotZ(DEGtoANG(0));
				//slRotX(DEGtoANG(player.head_rx)); slRotY(DEGtoANG(player.head_ry)); slRotZ(DEGtoANG(player.head_rz));
				
				slPutPolygonX(current_powerup->pup_model, light);
				}
				slPopMatrix();
		
		
		if(current_powerup->ry >=360)
	{
	current_powerup->ry = 0;
	}else
	{
	current_powerup->ry++;	
	}
		
	}
	
	slSetGouraudColor(CD_White);
	
}

void				draw_player(void)
{
	
	
	if(!player.can_be_hurt)
	{
	slSetGouraudColor(CD_Red);
	}
	else
	{
		slSetGouraudColor(CD_White);
	}
	
	
	
						
					
					
					if(!first_person_cam)
					{
					/*slPushMatrix();
						{
							slTranslate(toFIXED(player.x), toFIXED(player.y+24), toFIXED(player.z));
							slRotY(DEGtoANG(player.ry));//slRotX(DEGtoANG(player.rx));slRotZ(DEGtoANG(player.rz));
							slPutPolygonX((XPDATA *)&xpdata_player_head,light);
							slPutPolygonX((XPDATA *)&xpdata_player_body,light);
							slPutPolygonX((XPDATA *)&xpdata_player_rarm,light);
							slPutPolygonX((XPDATA *)&xpdata_player_larm,light);
							
							slPushMatrix();
						{
						
						slTranslate(toFIXED(12), toFIXED(-58), toFIXED(30));
						slPutPolygonX((XPDATA *)&xpdata_PISTOL, light);
							if(flash && flash_counter <10)
							{jo_3d_set_scalef(2.0f,2.0f,2.0f);	
							slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
							flash_counter ++;
							}
							
						}
						slPopMatrix();
							
						}
					slPopMatrix();	
					*/
					
				
				slPushMatrix();
				{
					slTranslate(toFIXED(player.x), toFIXED(player.y-11+player.y_adj), toFIXED(player.z));//-22.183
					slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
					slRotX(DEGtoANG(player.bodyt_rx)); slRotY(DEGtoANG(player.bodyt_ry)); slRotZ(DEGtoANG(player.bodyt_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_player_body_top, light);
							
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(-17.817), toFIXED(0));
					slRotX(DEGtoANG(player.head_rx)); slRotY(DEGtoANG(player.head_ry)); slRotZ(DEGtoANG(player.head_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_player_head, light);
					}
					slPopMatrix();
					
					//left arm
					
					slPushMatrix();
					{
					slTranslate(toFIXED(-8), toFIXED(-13.817), toFIXED(-3));
					slRotX(DEGtoANG(player.larm_rx)); slRotY(DEGtoANG(player.larm_ry)); slRotZ(DEGtoANG(player.larm_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_LARM_1, light);
					
					slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(13), toFIXED(0));
						slRotX(DEGtoANG(player.larm2_rx)); slRotY(DEGtoANG(player.larm2_ry)); slRotZ(DEGtoANG(player.larm2_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_LARM_2, light);
							
						
							slPushMatrix();
							{
							slTranslate(toFIXED(0), toFIXED(15), toFIXED(0));
							slRotX(DEGtoANG(10.8)); slRotY(DEGtoANG(0)); slRotZ(DEGtoANG(0));
							slPutPolygonX((XPDATA *)&xpdata_PLAYER_LARM_3, light);
							}
							slPopMatrix();
						}	
						slPopMatrix();
					}
					slPopMatrix();
					
					//right arm and weapon
				
					slPushMatrix();
					{
					slTranslate(toFIXED(8), toFIXED(-13.817), toFIXED(-3));
					slRotX(DEGtoANG(player.rarm_rx )); slRotY(DEGtoANG(player.rarm_ry)); slRotZ(DEGtoANG(player.rarm_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_RARM_1, light);
					
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(13), toFIXED(0));
						slRotX(DEGtoANG(player.rarm2_rx)); slRotY(DEGtoANG(player.rarm2_ry)); slRotZ(DEGtoANG(player.rarm2_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_RARM_2, light);
							
						
							slPushMatrix();
							{
							slTranslate(toFIXED(0), toFIXED(15), toFIXED(0));
							slRotX(DEGtoANG(10.8)); slRotY(DEGtoANG(0)); slRotZ(DEGtoANG(0));
							slPutPolygonX((XPDATA *)&xpdata_PLAYER_RARM_3, light);
							
					
						slPushMatrix();
						switch(player.current_weapon->type)
						{
						case 0:
								{
								
								slTranslate(toFIXED(2), toFIXED(6), toFIXED(5));
								slRotX(DEGtoANG(-80));
								slPutPolygonX((XPDATA *)&xpdata_PISTOL, light);
									if(flash && flash_counter <10)
									{
										jo_3d_set_scalef(2.0f,2.0f,2.0f);	
									slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
									flash_counter ++;
									}
									
								}
								break;
						
						case 1:
								{
								
								slTranslate(toFIXED(2), toFIXED(6), toFIXED(5));
								slRotX(DEGtoANG(-90));
								slPutPolygonX((XPDATA *)&xpdata_SHOTGUN, light);
									if(flash && flash_counter <10)
									{
										jo_3d_set_scalef(3.0f,3.0f,3.0f);	
									slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
									flash_counter ++;
									}
									
								}
								break;
						} 
						slPopMatrix();
					}
					slPopMatrix();
					}	
							slPopMatrix();
						}
						slPopMatrix();
				}
				slPopMatrix();
				
				slPushMatrix();
				{
					slTranslate(toFIXED(player.x), toFIXED(player.y-11+player.y_adj), toFIXED(player.z));//-22.183
					slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
					slRotX(DEGtoANG(player.bodyb_rx)); slRotY(DEGtoANG(player.bodyb_ry)); slRotZ(DEGtoANG(player.bodyb_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_player_body_bottom, light);
				
				//left leg
				slPushMatrix();
				{
					slTranslate(toFIXED(0), toFIXED(8.18304), toFIXED(0));
					slRotX(DEGtoANG(player.lleg_rx)); slRotY(DEGtoANG(player.lleg_ry)); slRotZ(DEGtoANG(player.lleg_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_LLEG1, light);
					
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(19.7), toFIXED(0));
					slRotX(DEGtoANG(player.lleg2_rx)); slRotY(DEGtoANG(player.lleg2_ry)); slRotZ(DEGtoANG(player.lleg2_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_LLEG2, light);
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(14.96), toFIXED(-2.9));
						slRotX(DEGtoANG(player.lleg3_rx)); slRotY(DEGtoANG(player.lleg3_ry)); slRotZ(DEGtoANG(player.lleg3_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_LLEG3, light);
						}
						slPopMatrix();
					}
					slPopMatrix();
				}
				slPopMatrix();
				
				
				//right leg
				slPushMatrix();
				{
					slTranslate(toFIXED(0), toFIXED(8.18304), toFIXED(0));
					slRotX(DEGtoANG(player.rleg_rx)); slRotY(DEGtoANG(player.rleg_ry)); slRotZ(DEGtoANG(player.rleg_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_RLEG1, light);
					
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(19.7), toFIXED(0));
					slRotX(DEGtoANG(player.rleg2_rx)); slRotY(DEGtoANG(player.rleg2_ry)); slRotZ(DEGtoANG(player.rleg2_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_RLEG2, light);
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(14.96), toFIXED(-2.9));
						slRotX(DEGtoANG(player.rleg3_rx)); slRotY(DEGtoANG(player.rleg3_ry)); slRotZ(DEGtoANG(player.rleg3_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_RLEG3, light);
						}
						slPopMatrix();
					}
					slPopMatrix();
				}
				slPopMatrix();
					
					
				}
				slPopMatrix();
				
				
				
				///reflection

			if((game.level == 1 && game.map_section == 2)|| (game.level == 2 && game.map_section == 6)|| (game.level == 3 && game.map_section == 1)|| (game.level == 4 && game.map_section == 1))
			{
				slPushMatrix();
				{
					slTranslate(toFIXED(player.x), toFIXED(player.y-11+player.y_adj), toFIXED(-player.z-112));
					slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(-player.ry-180)); slRotZ(DEGtoANG(player.rz));
					slRotX(DEGtoANG(player.bodyt_rx)); slRotY(DEGtoANG(-player.bodyt_ry)); slRotZ(DEGtoANG(player.bodyt_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_player_body_top, light);
							
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(-17.817), toFIXED(0));
					slRotX(DEGtoANG(player.head_rx)); slRotY(DEGtoANG(player.head_ry)); slRotZ(DEGtoANG(player.head_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_player_head, light);
					//slPutPolygonX((XPDATA *)&xpdata_spider_head, light);
					}
					slPopMatrix();
					
					//left arm
					
					slPushMatrix();
					{
					slTranslate(toFIXED(-8), toFIXED(-13.817), toFIXED(-3));
					slRotX(DEGtoANG(player.rarm_rx)); slRotY(DEGtoANG(player.rarm_ry)); slRotZ(DEGtoANG(player.rarm_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_LARM_1, light);
					
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(13), toFIXED(0));
						slRotX(DEGtoANG(player.rarm2_rx)); slRotY(DEGtoANG(player.rarm2_ry)); slRotZ(DEGtoANG(player.rarm2_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_LARM_2, light);
							
						
							slPushMatrix();
							{
							slTranslate(toFIXED(0), toFIXED(15), toFIXED(0));
							slRotX(DEGtoANG(10.8)); slRotY(DEGtoANG(0)); slRotZ(DEGtoANG(0));
							slPutPolygonX((XPDATA *)&xpdata_PLAYER_LARM_3, light);
							
					
						slPushMatrix();
						switch(player.current_weapon->type)
						{
						case 0:
								{
								
								slTranslate(toFIXED(-2), toFIXED(6), toFIXED(5));
								slRotX(DEGtoANG(-80));
								slPutPolygonX((XPDATA *)&xpdata_PISTOL, light);
									if(flash && flash_counter <10)
									{
										jo_3d_set_scalef(2.0f,2.0f,2.0f);	
									slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
									flash_counter ++;
									}
									
								}
								break;
						
						case 1:
								{
								
								slTranslate(toFIXED(-2), toFIXED(6), toFIXED(5));
								slRotX(DEGtoANG(-90));
								slPutPolygonX((XPDATA *)&xpdata_SHOTGUN, light);
									if(flash && flash_counter <10)
									{
										jo_3d_set_scalef(3.0f,3.0f,3.0f);	
									slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
									flash_counter ++;
									}
									
								}
								break;
						} 
						slPopMatrix();
					}
					slPopMatrix();
					}	
							slPopMatrix();
						}
						slPopMatrix();
				
					//right arm
					
					slPushMatrix();
					{
					slTranslate(toFIXED(8), toFIXED(-13.817), toFIXED(-3));
					slRotX(DEGtoANG(player.larm_rx)); slRotY(DEGtoANG(player.larm_ry)); slRotZ(DEGtoANG(player.larm_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_RARM_1, light);
					
					slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(13), toFIXED(0));
						slRotX(DEGtoANG(player.larm2_rx)); slRotY(DEGtoANG(player.larm2_ry)); slRotZ(DEGtoANG(player.larm2_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_RARM_2, light);
							
						
							slPushMatrix();
							{
							slTranslate(toFIXED(0), toFIXED(15), toFIXED(0));
							slRotX(DEGtoANG(10.8)); slRotY(DEGtoANG(0)); slRotZ(DEGtoANG(0));
							slPutPolygonX((XPDATA *)&xpdata_PLAYER_RARM_3, light);
							}
							slPopMatrix();
						}	
						slPopMatrix();
					}
					slPopMatrix();
					
					}
				slPopMatrix();
				
				slPushMatrix();
				{
					slTranslate(toFIXED(player.x), toFIXED(player.y-11+player.y_adj), toFIXED(-player.z-112));
					slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(-player.ry-180)); slRotZ(DEGtoANG(player.rz));
					slRotX(DEGtoANG(player.bodyb_rx)); slRotY(DEGtoANG(player.bodyb_ry)); slRotZ(DEGtoANG(player.bodyb_rz));
					
					slPutPolygonX((XPDATA *)&xpdata_player_body_bottom, light);
					
					slPushMatrix();
				{
					slTranslate(toFIXED(0), toFIXED(8.18304), toFIXED(0));
					slRotX(DEGtoANG(player.rleg_rx)); slRotY(DEGtoANG(player.rleg_ry)); slRotZ(DEGtoANG(player.rleg_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_LLEG1, light);
					
					
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(19.7), toFIXED(0));
					slRotX(DEGtoANG(player.rleg2_rx)); slRotY(DEGtoANG(player.rleg2_ry)); slRotZ(DEGtoANG(player.rleg2_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_LLEG2, light);
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(14.96), toFIXED(-2.9));
						slRotX(DEGtoANG(player.rleg3_rx)); slRotY(DEGtoANG(player.rleg3_ry)); slRotZ(DEGtoANG(player.rleg3_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_LLEG3, light);
						}
						slPopMatrix();
					}
					slPopMatrix();
					
				}
				slPopMatrix();
				
				slPushMatrix();
				{
					slTranslate(toFIXED(0), toFIXED(8.18304), toFIXED(0));
					slRotX(DEGtoANG(player.lleg_rx)); slRotY(DEGtoANG(player.lleg_ry)); slRotZ(DEGtoANG(player.lleg_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_RLEG1, light);
					
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(19.7), toFIXED(0));
					slRotX(DEGtoANG(player.lleg2_rx)); slRotY(DEGtoANG(player.lleg2_ry)); slRotZ(DEGtoANG(player.lleg2_rz));
					slPutPolygonX((XPDATA *)&xpdata_PLAYER_RLEG2, light);
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(14.96), toFIXED(-2.9));
						slRotX(DEGtoANG(player.lleg3_rx)); slRotY(DEGtoANG(player.lleg3_ry)); slRotZ(DEGtoANG(player.lleg3_rz));
						slPutPolygonX((XPDATA *)&xpdata_PLAYER_RLEG3, light);
						}
						slPopMatrix();
					}
					slPopMatrix();
				}
				slPopMatrix();
					
				}
				slPopMatrix();

			}
				///				
				
					
					}
					
					
					
					else
					{
						
						player.gun_x = ((player.x*32768 + 5 * jo_sin(player.ry))/32768);
						player.gun_y = (((player.y-48)*32768 + 5 * jo_sin(player.aim_rx))/32768);
						player.gun_z = ((player.z*32768 + 5 * jo_cos(player.ry))/32768);
						
						slPushMatrix();
						{
						slTranslate(toFIXED(player.gun_x), toFIXED(player.gun_y), toFIXED(player.gun_z));
						
						slRotY(DEGtoANG(player.ry)); slRotX(DEGtoANG((player.aim_rx*32768 + 5 * jo_sin(player.ry))/32768)); slRotZ(DEGtoANG(-(player.rz*32768 + 5 * jo_cos(player.ry))/32768));
						
						slTranslate(toFIXED(5), toFIXED(5), toFIXED(0));
						slPutPolygonX((XPDATA *)&xpdata_PISTOL, light);
							if(flash && flash_counter <10)
							{
								jo_3d_set_scalef(2.0f,2.0f,2.0f);	
							slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
							flash_counter ++;
							}
							
						}
						slPopMatrix();
						
					}
					///draw blood if projectile hits enemy
					if(spark && spark_counter <10)
					{
					
					
					switch(spark_type)
					{
						
						case 1:	
						jo_3d_draw_scaled_billboard(pblood_tex,player.px, player.py + spark_counter, player.pz,player.current_weapon->damage_size);
						break;
						
						case 2:	
						jo_3d_draw_scaled_billboard(pspark_tex,player.px, player.py + spark_counter, player.pz,player.current_weapon->damage_size);
						break;
						
					}	
					spark_counter ++;
				
					}
					
					//draw projectile for testing
					//jo_3d_draw_scaled_billboard(pspark_tex,player.px, player.py + spark_counter, player.pz,1.0f);
					///draw blood if player gets hurt
					if(player.blood_counter>0)
					{
					jo_3d_draw_scaled_billboard(pblood_tex,player.blood_x, player.blood_y, player.blood_z,player.blood_size);
					}
	
	slSetGouraudColor(CD_White);
	
	
	if(player.projectile_alive)
			{
			
			
			player.px += player.speed_px;
			player.py += player.speed_py;
			player.pz += player.speed_pz;
			
			
		
			}
	
	
}



void draw_fog(void)
{

		
			
			slPushMatrix();
			{
				//slRotX(DEGtoANG(180));
				slTranslate(toFIXED(cam_pos_x), toFIXED( - 300), toFIXED(cam_pos_z));
				jo_3d_set_scalef(2.0f,2.5f,2.0f);	
			//lRotY(DEGtoANG(ry));
			
				
				{
				
				slPutPolygon((PDATA *)&xpdata_skybox);
				}
			}
			slPopMatrix();
			

}

void				draw_map(level_section* section)
{
		
	slPushMatrix();
	{
	slTranslate(toFIXED(section->x), toFIXED(section->y), toFIXED(section->z));
	slPutPolygonX(section->map_model, light);
	}
	slPopMatrix();
	
			
}

void draw_hud(void)
{
	int	bullet_x = 148;	
	int hbar_x_adj = 0;	 
	int hbar_x = (hbar_x_adj + (8 * (player.health/10))/2)-140;
			
		jo_sprite_change_sprite_scale_xy(player.health/10,1);
		
		jo_sprite_draw3D(health_bar_tex,hbar_x, 104, 100);
		jo_sprite_restore_sprite_scale();
		
				
		if(player.current_weapon->bullets_in_clip > 0)
		{
			
			for(int i = 1; i < player.current_weapon->bullets_in_clip+1; i++)
			{
			switch(player.current_weapon->type)
			{
			case 0: jo_sprite_draw3D(21,bullet_x, 104, 100);
					break;
			case 1: jo_sprite_draw3D(22,bullet_x, 104, 100);
					break;
							
			}	
			bullet_x -= 10;
			}
		}
		
	
}

void			    my_draw(void)
{
	if (game.game_state != GAMESTATE_GAMEPLAY)
       return;
	
    jo_3d_camera_look_at(&cam);
	int x_dist;
	int y_dist;
	int z_dist;
	
		
	//play cd audio
	if(cd_track >0)
	{
	if (!is_cd_playing)
        {
             CDDAPlaySingle(cd_track, true);
            is_cd_playing = true;
        }
	}
	slCurWindow(winFar);
	
	 slPushMatrix();
    {
        if(use_light) computeLight();
    
		
    }
	slPopMatrix();
	
	/*if(game.map_section == 1 || game.map_section == 4)
	{
	draw_fog();	
	}*/
	
	for(int i = 0; i < section_total; i++)
	{
	draw_map(&map_section[i]);
	}
	
	//animate_player();
	player_collision_handling();
	player_animation_handling();
	draw_player();
	
	for(int e = 0; e < enemy_total; e++)
	{
	
	if(enemies[e].alive)
		{
	
		//set draw distance
		x_dist = JO_ABS(player.x - enemies[e].x);
		y_dist = JO_ABS(player.y - enemies[e].y);
		z_dist = JO_ABS(player.z - enemies[e].z);
	
		if((x_dist + y_dist + z_dist) < DRAW_DISTANCE)
		
			{
				draw_enemies(&enemies[e]);
				animate_enemies(&enemies[e]);
				
				enemy_target();
				enemy_collision_handling();
			}
			
		
		}
	
	}
	
	for(int p = 0; p < powerup_total; p++)
	{
	
	if(!powerups[p].used)
		{
		//set draw distance
		x_dist = JO_ABS(player.x - powerups[p].x);
		y_dist = JO_ABS(player.y - powerups[p].y);
		z_dist = JO_ABS(player.z - powerups[p].z);
	
		if((x_dist + y_dist + z_dist) < DRAW_DISTANCE)
			{
			draw_powerups(&powerups[p]);
			}
		
		}
	
	}
	
	slCurWindow(winNear);
	draw_hud();
	//jo_camera * cam;
   
   if(show_debug)
		{
		jo_printf(0, 0, "CPOS: :\t%3d\t%3d\t%3d ",(int) cam_pos_x, (int) cam_pos_y, (int) cam_pos_z);
		//jo_printf(0, 1, "CANG: :\t%3d\t%3d\t%3d ",&cam->z_angle, (int) &cam->target_pos.y, (int) &cam->target_pos.z);
		jo_printf(20, 0, "PPOS:\t%3d\t%3d\t%3d ",(int) player.x, (int) player.y, (int) player.z);
		jo_printf(20, 1, "PROT:\t%3d\t%3d\t%3d ",(int) player.rx, (int) player.ry, (int) player.rz);
		
		//jo_printf(0, 22,  "PRESSED_ST: %d ", game.pressed_start);
		//jo_printf(0, 22,  "TOGGLE_CAM: %d ", toggle_cam);
		jo_printf(0, 23,  "SPRITE MEMORY USAGE: %d%%  ", jo_sprite_usage_percent());
		jo_printf(0, 24, "polygon count %4d" , jo_3d_get_polygon_count());
		jo_printf(0, 25, "polygons displayed %4d" , jo_3d_get_displayed_polygon_count());
		
		//jo_printf(20, 3, "NPOS:\t%3d\t%3d\t%3d ",(int) doors[1].new_x, (int) doors[1].new_y, (int) doors[1].new_z);
		//jo_printf(0, 0, "EPOS: :\t%3d\t%3d\t%3d ",(int) enemies[0].x, (int) enemies[0].y, (int) enemies[0].z);
		//jo_printf(0, 1, "TARG: :\t%3d\t%3d\t%3d ",(int) player.tx, (int)player.ty, (int) player.tz);
		//jo_printf(0, 1, "ENEMYTOTAL: %d", enemy_total);
		//jo_printf(0, 23, "cam %2d" , cam_number);
		//jo_printf(20, 3, "RARM: %3d", player.rarm_rx);
		//jo_printf(18, 3, "TARGET: %3d %3d %3d", player.tx, player.ty, player.tz);
		//jo_printf(18, 4, "ENEMY : %3d %3d %3d", enemies[0].x, enemies[0].y, enemies[0].z);
		//jo_printf(20, 3, "cam_pitch_adj: %3d", cam_pitch_adj);
		//jo_printf(20, 4, "cam_dist: %3d", cam_dist);
		//jo_printf(20, 2, "Items: %3d %3d", (int)player.a_itemdata->nbIt,(int)ID_ItemsSaved.nbIt);
		//jo_printf(20, 2, "J L L: %d %d %d", player.can_jump,player.on_ladder_x,player.on_ladder_z);
		//jo_printf(20, 1, "ANIM_FRAME: %3d",player.anim_frame);
		}
	
   
  
}

void init_level(void)
{
	
jo_clear_screen();
cam_pos_x = CAM_DEFAULT_X;
cam_pos_y = CAM_DEFAULT_Y;
cam_height = CAM_DEFAULT_Y;
cam_pos_z = CAM_DEFAULT_Z;
cam_dist = CAM_DIST_DEFAULT;
cam_angle_x = 0;
cam_angle_y = 0;
cam_angle_z = 0;
cam_target_x = 0;
cam_target_y = 0;
cam_target_z = 0;
cam_number = default_cam;

player.x = player.start_x;
player.y = player.start_y;
player.z = player.start_z;
player.can_be_hurt = true;
player.shadow_y = player.y;
player.shadow_size = 1.0f;
player.key = 0;
player.speed = PLAYER_SPEED;
player.anim_frame = 0;
player.blood = false;
player.blood_counter = 0;

player.speed_y = 0;
drop_camera = false;
flash = false;
spark = false;
spark_counter = 0;
flash_counter = 0;
first_person_cam = 0;
highlight_item = 0;
player.can_jump = true;
player.on_ladder_x = false;
player.on_ladder_z = false;
apply_player_gravity();





player.projectile_alive = false;	
	
	rot.rx = JO_DEG_TO_RAD(90.0);
	rot.ry = JO_DEG_TO_RAD(0.0);
	rot.rz = JO_DEG_TO_RAD(0.0);
	slZdspLevel(5);
	game.gravity = 0.4f;
	game.max_gravity = 6.0f;
	
	int i = 0;

	create_map_section(&map_section[i], 0,0,0,0);i++;
	
	section_total = i;
	
	
	
}


void				transition_to_title_screen(void)
{
	
	game.game_state = GAMESTATE_UNINITIALIZED;
	stop_sounds();
	is_cd_playing = false;
	jo_sprite_free_from(map_sprite_id);
	game.level=1;
	load_map_textures("STREET.TGA",64);
	load_binary((char*)"TITLE.BIN", (void*)WORK_RAM_LOW);
	jo_clear_screen();
	game.start_timer = false;
	game.title_screen_timer = 0;
	player.z = 0;
	player.head_ry = 0;
	
	game.game_state = GAMESTATE_TITLE_SCREEN;
	game.pressed_start = false;
	
	
}

void				load_level(void)
{
	if (game.game_state != GAMESTATE_LOAD_LEVEL)
       return; 
	stop_sounds();
	is_cd_playing = false;
	
	jo_sprite_free_from(map_sprite_id);
	switch(game.level)
	{
				
	case 1: load_map_textures(level1[0],64);
			load_binary((char*)level1[game.map_section], (void*)WORK_RAM_LOW);
			load_map_plan_tex(level1[0]);
			slSetDepthLimit(0,8,5);
			break;
	
	case 2: load_map_textures(level2[0],64);
			load_binary((char*)level2[game.map_section], (void*)WORK_RAM_LOW);
			load_map_plan_tex(level2[0]);
			slSetDepthLimit(0,8,5);
			break;
			
	case 3: load_map_textures(level3[0],64);
			load_binary((char*)level3[game.map_section], (void*)WORK_RAM_LOW);
			load_map_plan_tex(level3[0]);
			slSetDepthLimit(0,8,5);
			break;
			
	case 4: load_map_textures(level4[0],64);
			load_binary((char*)level4[game.map_section], (void*)WORK_RAM_LOW);
			load_map_plan_tex(level4[0]);
			slSetDepthLimit(0,9,5);
			break;
			
	case 5: load_map_textures(level5[0],64);
			load_binary((char*)level5[game.map_section], (void*)WORK_RAM_LOW);
			load_map_plan_tex(level5[0]);
			slSetDepthLimit(0,10,5);
			break;
			
	
	}
	
	///create map section	
	section_total = 0;
	create_map_section(&map_section[0], 0,0,0,0);section_total++;
	
	game.map_section = 1;
	
	
	//set default camera
	cam_number = default_cam;
	
	save_items();
	init_level();
	game.game_state = GAMESTATE_GAMEPLAY;
	
	
	
}

void				load_area(void)
{
	if (game.game_state != GAMESTATE_LOAD_AREA)
       return; 
	stop_sounds();
	pcm_play(door_sound, PCM_VOLATILE, 6);
	is_cd_playing = false;
	
	switch(game.level)
	{
	case 1: load_binary((char*)level1[game.map_section], (void*)WORK_RAM_LOW);
			break;
	
	case 2: load_binary((char*)level2[game.map_section], (void*)WORK_RAM_LOW);
			break;
			
	case 3: load_binary((char*)level3[game.map_section], (void*)WORK_RAM_LOW);
			break;
			
	case 4: load_binary((char*)level4[game.map_section], (void*)WORK_RAM_LOW);
			break;
			
	
	
	}
	section_total = 0;
	create_map_section(&map_section[0], 0,0,0,0);
	section_total = 1;
	game.game_state = GAMESTATE_GAMEPLAY;
	init_level();
	
	//set default camera
	cam_number = default_cam;
	

	
	
}

void use_item(Uint16 id)
{
unsigned int current_item =0;	
unsigned int item_type = 0;
current_item = search_for_item(id);
item_type = player.a_itemdata->itemtbl[current_item].type;

switch(item_type)
{
case 0:	
	pcm_play(reload_sound, PCM_VOLATILE, 6);
	player.current_weapon = &pistol;
	
	break;
	
case 1:	
	if(player.health < 100)
	{
	pcm_play(pup_sound, PCM_VOLATILE, 6);
	player.a_itemdata->itemtbl[current_item].used = true;
	player.health = 100;
	}
	break;
	
case 2:	
	
	player.a_itemdata->itemtbl[current_item].used = true;
	pistol.total_bullets += 8;
	player_reload(&pistol);
	
	break;
	
case 3:	
	pcm_play(reload_sound, PCM_VOLATILE, 6);
	player.current_weapon = &shotgun;
	
	break;
	
case 4:	
	player.a_itemdata->itemtbl[current_item].used = true;
	shotgun.total_bullets += 6;
	player_reload(&shotgun);
	
	break;
	
case 6:	
	if(player.health < 100)
	{
	pcm_play(pup_sound, PCM_VOLATILE, 6);
	player.a_itemdata->itemtbl[current_item].used = true;
	player.health += 10;
	}
	break;
	
case 7:	

	if(game.level != 2)
	{
	game.level = 2;		
	game.map_section = 1;
	game.game_state = GAMESTATE_LOAD_LEVEL;
	}
	break;
	
case 9:	

	if(game.level != 5)
	{
	game.level = 5;		
	game.map_section = 1;
	game.game_state = GAMESTATE_LOAD_LEVEL;
	}
	break;
	
}
	
}

void			    show_map_plan(void)
{
	if (game.game_state != GAMESTATE_MAP)
       return;
	
	jo_printf(0, 1, "                *MAP*");
	
	jo_printf(8, 25, "PRESS B TO RETURN TO GAME");
	
	if(!has_map_plan)
	{
		jo_printf(8, 24, "NO MAP FOR THIS AREA");
		jo_sprite_change_sprite_scale_xy(40,30);
		jo_sprite_draw3D(28,0, 0, 300);
		jo_sprite_restore_sprite_scale();
	}else
		
	{
	
	float location;
	float map_pos_x;
	float map_pos_y;
	float f_min_x = (float)map_plan.min_x;
	float f_max_x = (float)map_plan.max_x;
	float f_map_min_x = (float)map_plan.map_min_x;
	float f_map_max_x = (float)map_plan.map_max_x;
	float f_min_y = (float)map_plan.min_y;
	float f_max_y = (float)map_plan.max_y;
	float f_map_min_y = (float)map_plan.map_min_y;
	float f_map_max_y = (float)map_plan.map_max_y;
	
	location = 1+((player.x - f_max_x)/(f_max_x - f_min_x));
	map_pos_x = Lerp(f_map_min_x,f_map_max_x,location);
	
	location = 1+((player.z - f_max_y)/(f_max_y - f_min_y));
	map_pos_y = Lerp(f_map_min_y,f_map_max_y,location);
	
	jo_sprite_set_palette(map_plan_pal.id);
	jo_sprite_draw3D(map_plan_tex,0, 0, 300);
	jo_sprite_draw3D_and_rotate(36,map_pos_x, map_pos_y, 200,player.ry-180);
	
	
	 jo_3d_camera_look_at(&cam);
	jo_3d_camera_set_viewpoint(&cam,0,0,0);
	jo_3d_camera_set_target(&cam,0,0,100);
	
	
	}
	
	
	
	if (KEY_DOWN(0,PER_DGT_TB))
		{
			
			jo_clear_screen();
			jo_sprite_set_palette(image_pal.id);
			game.game_state = GAMESTATE_GAMEPLAY;
			
		}
  
}

void			    inventory(void)
{
	if (game.game_state != GAMESTATE_INVENTORY)
       return;
   
   
		jo_sprite_change_sprite_scale_xy(40,30);
		jo_sprite_draw3D(28,0, 0, 300);
		jo_sprite_restore_sprite_scale();
		
	
   
   draw_sky = false;
    
	Uint8 health_col_r;
	Uint8 health_col_g;
	int line = 10;
	health_col_r = (Uint8)(((float)JO_ABS(player.health-100) / 100) * 255);
	health_col_g = (Uint8)(((float)player.health / 100) * 255);
		
   
    jo_printf(0, 1, "                *INVENTORY*");
	
	jo_printf(8, 24, "PRESS A TO USE ITEM");
	jo_printf(8, 25, "PRESS B TO RETURN TO GAME");
	
	
	//reset camera
    jo_3d_camera_look_at(&cam);
	jo_3d_camera_set_viewpoint(&cam,0,0,0);
	jo_3d_camera_set_target(&cam,0,0,100);

	draw_hud();
	
	jo_printf(20, line, "%s", player.a_itemdata->itemtbl[inventory_item_number].name);line++; 
	
	jo_printf(20, line, "%s", player.a_itemdata->itemtbl[inventory_item_number].description);line++;
	if(player.a_itemdata->itemtbl[inventory_item_number].type == 0)
	{
	jo_printf(20, line, "Bullets: %3d", pistol.total_bullets);
	}
	if(player.a_itemdata->itemtbl[inventory_item_number].type == 3)
	{
	jo_printf(20, line, "Shells: %3d", shotgun.total_bullets);
	}
	
	
  
  slSetGouraudColor(CD_White);
   //draw item
	slPushMatrix();
		{
			
			slTranslate(toFIXED(-40), toFIXED(0), toFIXED(100));
			slRotX(DEGtoANG(30)); slRotY(DEGtoANG(powerups[0].ry)); slRotZ(DEGtoANG(0));
			jo_3d_set_scalef(6,6,6);
				
			slPutPolygonX((XPDATA *)pup_data[player.a_itemdata->itemtbl[inventory_item_number].type], light);
				
		}
			slPopMatrix();
			
	//draw players head
	
	
	
	
	slSetGouraudColor(JO_COLOR_RGB(health_col_r,health_col_g,0));
	slPushMatrix();
		{
			slTranslate(toFIXED(60), toFIXED(-40), toFIXED(100));
			slRotX(DEGtoANG(0)); slRotY(DEGtoANG(powerups[0].ry)); slRotZ(DEGtoANG(0));
			jo_3d_set_scalef(1.5,1.5,1.5);
				
			slPutPolygonX((XPDATA *)&xpdata_player_head, light);
				
		}
			slPopMatrix();
   
   //controls
   if (KEY_DOWN(0,PER_DGT_TA))
		{
			use_item(player.a_itemdata->itemtbl[inventory_item_number].id);
			if(player.a_itemdata->itemtbl[inventory_item_number].used)
			{
			inventory_item_number = next_item(player.a_itemdata->itemtbl[inventory_item_number].id);
			}
		}
		
	if (KEY_DOWN(0,PER_DGT_TB))
		{
			
			draw_sky = true;
			jo_clear_screen();
			//init_3d_planes();
			game.game_state = GAMESTATE_GAMEPLAY;
			//jo_vdp2_enable_rbg0();
		}
		
	if (KEY_DOWN(0,PER_DGT_KL))
		{
			inventory_item_number = previous_item(player.a_itemdata->itemtbl[inventory_item_number].id);
			/*jo_clear_screen();
			if(inventory_item_number == 0)
			{
			inventory_item_number = total_items-1;	
			}else
			{
			inventory_item_number --;	
			}*/
		}
		
	if (KEY_DOWN(0,PER_DGT_KR))
		{
		inventory_item_number = next_item(player.a_itemdata->itemtbl[inventory_item_number].id);
		}	
   
	
	if(powerups[0].ry >=360)
	{
	powerups[0].ry = 0;
	}else
	{
	powerups[0].ry++;	
	}
		
}

void				map_builder_animate_enemy(void)
{



			switch(anim_num)
						{
							
						case 0:	player.head_rz = 0;	
								player.bodyt_rz = 0;
								player.bodyt_ry = 0;
								player.bodyt_rx = 0;
								player.larm_rx = 0;
								player.rarm_rx = 0;
								player.lleg_rx = 0;
								player.rleg_rx = 0;
						
								break;
							
						case 1:	
								
								{
									if(map_builder_enemy_type == 1)
									{
											if(player.bodyt_ry == -16)
											{player.anim_speed = ANIM_SPEED;
											}
											else
											if(player.bodyt_ry == 16)
											{player.anim_speed = -ANIM_SPEED;
											}
										
											player.bodyt_ry += player.anim_speed;
											player.larm_rx += player.anim_speed;
											player.rarm_rx -= player.anim_speed;
											player.lleg_rx -= player.anim_speed;
											player.rleg_rx += player.anim_speed;
											
									}else if(map_builder_enemy_type == 2)
									{
											if(player.rleg_rx <= -44)
											{player.anim_speed = ANIM_SPEED;
											}
											else
											if(player.rleg_rx >= 44)
											{player.anim_speed = -ANIM_SPEED;
											}
										
											player.rleg_rx += player.anim_speed;
											player.larm_rz -= player.anim_speed;
											player.rarm_rz += player.anim_speed;
											
											
											if(player.rarm_rz == 44 )
											{
											//flap - make flapping sound
											pcm_play(flap_sound, PCM_VOLATILE, 6);
											
											
											}
											
									}else if(map_builder_enemy_type == 3)
									{
											if(player.rleg_rx <= -44)
											{player.anim_speed = (ANIM_SPEED*4);
											}
											else
											if(player.rleg_rx >= 44)
											{player.anim_speed = -(ANIM_SPEED*4);
											}
										
											player.rleg_rx += player.anim_speed;
											player.larm_rz -= player.anim_speed;
											player.rarm_rz += player.anim_speed;
											
											
											if(player.rarm_rz == 44 )
											{
											//flap - make flapping sound
											pcm_play(flap_sound, PCM_VOLATILE, 6);
											
											}
											
									}
									
											
								
								}break;
									
						
						case 2:
								player.kick_timer ++;
								
								if(player.kick_timer >= 20)
								{player.kick_timer = 0;
									
								}
								player.head_rz = -16;	
								player.bodyt_rz = 16;
								break;
					
						case 3:																																
					
					
						if(player.bodyt_rx <= 0)
						{
						player.bodyt_rx = 20;
						player.larm_rz = 40;
						player.larm_rx = 20;
						player.rarm_rz = 40;
						player.rarm_rx = 20;
						}
						player.bodyt_rx --;
						player.larm_rz -=2;
						player.larm_rx --;
						player.rarm_rz -=2;
						player.rarm_rx --;
						break;
						
				}
				
			
			
		
	
			
		
	
		
				
			
	
}

void				map_builder_animate_player(void)
{
ANIMDATA		*a_animdata;
ANIM_FRAME *a_frame;
	
a_animdata = player_anim_data[anim_num];
total_frames = a_animdata->nbAn;
a_frame = a_animdata->animtbl;



player.y_adj = a_frame[player.anim_frame].y_adj;
	player.bodyt_rx = a_frame[player.anim_frame].bodyt_rx;
	player.bodyt_ry = a_frame[player.anim_frame].bodyt_ry;
	player.bodyt_rz = a_frame[player.anim_frame].bodyt_rz;
	player.head_rx = a_frame[player.anim_frame].head_rx;
	player.head_ry = a_frame[player.anim_frame].head_ry;
	player.head_rz = a_frame[player.anim_frame].head_rz;
	player.rarm_rx = a_frame[player.anim_frame].rarm_rx;
	player.rarm_ry = a_frame[player.anim_frame].rarm_ry;
	player.rarm_rz = a_frame[player.anim_frame].rarm_rz;
	player.rarm2_rx = a_frame[player.anim_frame].rarm2_rx;
	player.rarm2_ry = a_frame[player.anim_frame].rarm2_ry;
	player.rarm2_rz = a_frame[player.anim_frame].rarm2_rz;
	player.larm_rx = a_frame[player.anim_frame].larm_rx;
	player.larm_ry = a_frame[player.anim_frame].larm_ry;
	player.larm_rz = a_frame[player.anim_frame].larm_rz;
	player.larm2_rx = a_frame[player.anim_frame].larm2_rx;
	player.larm2_ry = a_frame[player.anim_frame].larm2_ry;
	player.larm2_rz = a_frame[player.anim_frame].larm2_rz;
	player.bodyb_rx = a_frame[player.anim_frame].bodyb_rx;
	player.bodyb_ry = a_frame[player.anim_frame].bodyb_ry;
	player.bodyb_rz = a_frame[player.anim_frame].bodyb_rz;	
	player.rleg_rx = a_frame[player.anim_frame].rleg_rx;
	player.rleg_ry = a_frame[player.anim_frame].rleg_ry;
	player.rleg_rz = a_frame[player.anim_frame].rleg_rz;
	player.rleg2_rx = a_frame[player.anim_frame].rleg2_rx;
	player.rleg2_ry = a_frame[player.anim_frame].rleg2_ry;
	player.rleg2_rz = a_frame[player.anim_frame].rleg2_rz;
	player.rleg3_rx = a_frame[player.anim_frame].rleg3_rx;
	player.rleg3_ry = a_frame[player.anim_frame].rleg3_ry;
	player.rleg3_rz = a_frame[player.anim_frame].rleg3_rz;
	player.lleg_rx = a_frame[player.anim_frame].lleg_rx;
	player.lleg_ry = a_frame[player.anim_frame].lleg_ry;
	player.lleg_rz = a_frame[player.anim_frame].lleg_rz;
	player.lleg2_rx = a_frame[player.anim_frame].lleg2_rx;
	player.lleg2_ry = a_frame[player.anim_frame].lleg2_ry;
	player.lleg2_rz = a_frame[player.anim_frame].lleg2_rz;
	player.lleg3_rx = a_frame[player.anim_frame].lleg3_rx;
	player.lleg3_ry = a_frame[player.anim_frame].lleg3_ry;
	player.lleg3_rz = a_frame[player.anim_frame].lleg3_rz;



	jo_printf(0, 5, "bodyt : %3d %3d %3d", player.bodyt_rx,player.bodyt_ry,player.bodyt_rz);
	jo_printf(0, 6, "head  : %3d %3d %3d", player.head_rx,player.head_ry,player.head_rz);
	jo_printf(0, 7, "rarm  : %3d %3d %3d", player.rarm_rx,player.rarm_ry,player.rarm_rz);
	jo_printf(0, 8, "rarm2 : %3d %3d %3d", player.rarm2_rx,player.rarm2_ry,player.rarm2_rz);
	jo_printf(0, 9, "larm  : %3d %3d %3d", player.larm_rx,player.larm_ry,player.larm_rz);
	jo_printf(0, 10, "larm2: %3d %3d %3d", player.larm2_rx,player.larm2_ry,player.larm2_rz);
	jo_printf(0, 11, "bodyb : %3d %3d %3d", player.bodyb_rx,player.bodyb_ry,player.bodyb_rz);
	jo_printf(0, 12, "rleg : %3d %3d %3d", player.rleg_rx,player.rleg_ry,player.rleg_rz);
	jo_printf(0, 13, "rleg2: %3d %3d %3d", player.rleg2_rx,player.rleg2_ry,player.rleg2_rz);
	jo_printf(0, 14, "rleg3: %3d %3d %3d", player.rleg3_rx,player.rleg3_ry,player.rleg3_rz);
	jo_printf(0, 15, "lleg : %3d %3d %3d", player.lleg_rx,player.lleg_ry,player.lleg_rz);
	jo_printf(0, 16, "lleg2: %3d %3d %3d", player.lleg2_rx,player.lleg2_ry,player.lleg2_rz);
	jo_printf(0, 17, "lleg3: %3d %3d %3d", player.lleg3_rx,player.lleg3_ry,player.lleg3_rz);

	if(anim_loop)
	{
		if(player.anim_frame >= total_frames-1)
		{
		player.anim_frame = 0;
		}else
		{
		player.anim_frame ++;		
		}	
	}
}

void				map_builder_draw_player(void)
{		
		
			/*slPushMatrix();
			{
				slTranslate(toFIXED(player.x), toFIXED(player.y), toFIXED(player.z));
				slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
				jo_3d_set_scalef(object_scale,object_scale,object_scale);
				slPutPolygonX((XPDATA *)player_data[model_num], light);
				
				
			}
			slPopMatrix();
			*/
			
			slPushMatrix();
				{
					slTranslate(toFIXED(player.x), toFIXED(player.y-22.183+player.y_adj), toFIXED(player.z));
					slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
					slRotX(DEGtoANG(player.bodyt_rx)); slRotY(DEGtoANG(player.bodyt_ry)); slRotZ(DEGtoANG(player.bodyt_rz));
					jo_3d_set_scalef(object_scale,object_scale,object_scale);
					
					slPutPolygonX((XPDATA *)player_data[1], light);
							
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(-17.817), toFIXED(0));
					slRotX(DEGtoANG(player.head_rx)); slRotY(DEGtoANG(player.head_ry)); slRotZ(DEGtoANG(player.head_rz));
					slPutPolygonX((XPDATA *)player_data[0], light);
					}
					slPopMatrix();
					
					//left arm
					
					slPushMatrix();
					{
					slTranslate(toFIXED(-8), toFIXED(-13.817), toFIXED(-3));
					slRotX(DEGtoANG(player.larm_rx)); slRotY(DEGtoANG(player.larm_ry)); slRotZ(DEGtoANG(player.larm_rz));					
					slPutPolygonX((XPDATA *)player_data[2], light);
					
					slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(13), toFIXED(0));
						slRotX(DEGtoANG(player.larm2_rx)); slRotY(DEGtoANG(player.larm2_ry)); slRotZ(DEGtoANG(player.larm2_rz));
						slPutPolygonX((XPDATA *)player_data[3], light);
							
						
							slPushMatrix();
							{
							slTranslate(toFIXED(0), toFIXED(15), toFIXED(0));
							slRotX(DEGtoANG(10.8)); slRotY(DEGtoANG(0)); slRotZ(DEGtoANG(0));
							slPutPolygonX((XPDATA *)player_data[4], light);
							}
							slPopMatrix();
						}	
						slPopMatrix();
					}
					slPopMatrix();
					
					//right arm and weapon
				
					slPushMatrix();
					{
					slTranslate(toFIXED(8), toFIXED(-13.817), toFIXED(-3));
					slRotX(DEGtoANG(player.rarm_rx)); slRotY(DEGtoANG(player.rarm_ry)); slRotZ(DEGtoANG(player.rarm_rz));					
					slPutPolygonX((XPDATA *)player_data[5], light);
					
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(13), toFIXED(0));
						slRotX(DEGtoANG(player.rarm2_rx)); slRotY(DEGtoANG(player.rarm2_ry)); slRotZ(DEGtoANG(player.rarm2_rz));
						slPutPolygonX((XPDATA *)player_data[6], light);
							
						
							slPushMatrix();
							{
							slTranslate(toFIXED(0), toFIXED(15), toFIXED(0));
							slRotX(DEGtoANG(10.8)); slRotY(DEGtoANG(0)); slRotZ(DEGtoANG(0));
							slPutPolygonX((XPDATA *)player_data[7], light);
							
					
						slPushMatrix();
						switch(player.current_weapon->type)
						{
						case 0:
								{
								
								slTranslate(toFIXED(2), toFIXED(6), toFIXED(5));
								slRotX(DEGtoANG(-80));
								slPutPolygonX((XPDATA *)&xpdata_PISTOL, light);
									if(flash && flash_counter <10)
									{
										jo_3d_set_scalef(2.0f,2.0f,2.0f);	
									slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
									flash_counter ++;
									}
									
								}
								break;
						
						case 1:
								{
								
								slTranslate(toFIXED(2), toFIXED(6), toFIXED(5));
								slRotX(DEGtoANG(-90));
								slPutPolygonX((XPDATA *)&xpdata_SHOTGUN, light);
									if(flash && flash_counter <10)
									{
										jo_3d_set_scalef(3.0f,3.0f,3.0f);	
									slPutPolygon((PDATA *)&xpdata_PISTOL_FLASH);
									flash_counter ++;
									}
									
								}
								break;
						} 
						slPopMatrix();
					}
					slPopMatrix();
					}	
							slPopMatrix();
						}
						slPopMatrix();
				}
				slPopMatrix();
				
				slPushMatrix();
				{
					slTranslate(toFIXED(player.x), toFIXED(player.y-22.183+player.y_adj), toFIXED(player.z));
					slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
					slRotX(DEGtoANG(player.bodyb_rx)); slRotY(DEGtoANG(player.bodyb_ry)); slRotZ(DEGtoANG(player.bodyb_rz));
					jo_3d_set_scalef(object_scale,object_scale,object_scale);
					slPutPolygonX((XPDATA *)player_data[8], light);
				
				//left leg
				slPushMatrix();
				{
					slTranslate(toFIXED(0), toFIXED(8.18304), toFIXED(0));
					slRotX(DEGtoANG(player.lleg_rx)); slRotY(DEGtoANG(player.lleg_ry)); slRotZ(DEGtoANG(player.lleg_rz));
					slPutPolygonX((XPDATA *)player_data[9], light);
					
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(19.7), toFIXED(0));
					slRotX(DEGtoANG(player.lleg2_rx)); slRotY(DEGtoANG(player.lleg2_ry)); slRotZ(DEGtoANG(player.lleg2_rz));
					slPutPolygonX((XPDATA *)player_data[10], light);
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(14.96), toFIXED(-2.9));
						slRotX(DEGtoANG(player.lleg3_rx)); slRotY(DEGtoANG(player.lleg3_ry)); slRotZ(DEGtoANG(player.lleg3_rz));
						slPutPolygonX((XPDATA *)player_data[11], light);
						}
						slPopMatrix();
					}
					slPopMatrix();
				}
				slPopMatrix();
				
				
				//right leg
				slPushMatrix();
				{
					slTranslate(toFIXED(0), toFIXED(8.18304), toFIXED(0));
					slRotX(DEGtoANG(player.rleg_rx)); slRotY(DEGtoANG(player.rleg_ry)); slRotZ(DEGtoANG(player.rleg_rz));
					slPutPolygonX((XPDATA *)player_data[12], light);
					
					slPushMatrix();
					{
					slTranslate(toFIXED(0), toFIXED(19.7), toFIXED(0));
					slRotX(DEGtoANG(player.rleg2_rx)); slRotY(DEGtoANG(player.rleg2_ry)); slRotZ(DEGtoANG(player.rleg2_rz));
					slPutPolygonX((XPDATA *)player_data[13], light);
					
						slPushMatrix();
						{
						slTranslate(toFIXED(0), toFIXED(14.96), toFIXED(-2.9));
						slRotX(DEGtoANG(player.rleg3_rx)); slRotY(DEGtoANG(player.rleg3_ry)); slRotZ(DEGtoANG(player.rleg3_rz));
						slPutPolygonX((XPDATA *)player_data[14], light);
						}
						slPopMatrix();
					}
					slPopMatrix();
				}
				slPopMatrix();
					
					
				}
				slPopMatrix();
			
		

}

void				map_builder_draw_enemy(void)
{		
		
		switch(map_builder_enemy_type)
		{
			case 1:
			
			slPushMatrix();
			{
				slTranslate(toFIXED(player.x), toFIXED(player.y), toFIXED(player.z));
				slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
				slRotX(DEGtoANG(player.bodyt_rx)); slRotY(DEGtoANG(player.bodyt_ry)); slRotZ(DEGtoANG(player.bodyt_rz));
				jo_3d_set_scalef(object_scale,object_scale,object_scale);
				slPutPolygonX((XPDATA *)enemy_data_1[0], light);
			
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.head_rx)); slRotY(DEGtoANG(player.head_ry)); slRotZ(DEGtoANG(player.head_rz));
				slTranslate(toFIXED(0), toFIXED(-5), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_1[1], light);
				}
				slPopMatrix();
				
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.larm_rx)); slRotY(DEGtoANG(player.larm_ry)); slRotZ(DEGtoANG(player.larm_rz));
				slTranslate(toFIXED(0), toFIXED(-5), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_1[2], light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.lleg_rx)); slRotY(DEGtoANG(player.lleg_ry)); slRotZ(DEGtoANG(player.lleg_rz));
				slTranslate(toFIXED(0), toFIXED(5), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_1[3], light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.rarm_rx)); slRotY(DEGtoANG(player.rarm_ry)); slRotZ(DEGtoANG(player.rarm_rz));
				slTranslate(toFIXED(0), toFIXED(-5), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_1[4], light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.rleg_rx)); slRotY(DEGtoANG(player.rleg_ry)); slRotZ(DEGtoANG(player.rleg_rz));
				slTranslate(toFIXED(0), toFIXED(5), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_1[5], light);
				}
				slPopMatrix();
			}
			slPopMatrix();
			break;
			
		case 2:
		
			slPushMatrix();
			{
				slTranslate(toFIXED(player.x), toFIXED(player.y), toFIXED(player.z));
				slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
				slRotX(DEGtoANG(player.bodyt_rx)); slRotY(DEGtoANG(player.bodyt_ry)); slRotZ(DEGtoANG(player.bodyt_rz));
				jo_3d_set_scalef(object_scale,object_scale,object_scale);
				slPutPolygonX((XPDATA *)enemy_data_2[0], light);
							
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.head_rx)); slRotY(DEGtoANG(player.head_ry)); slRotZ(DEGtoANG(player.head_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(-14.78));
				//jo_3d_set_scalef(0.5,0.5,0.5);
				slPutPolygonX((XPDATA *)enemy_data_2[1], light);
				}
				slPopMatrix();
				
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.larm_rx)); slRotY(DEGtoANG(player.larm_ry)); slRotZ(DEGtoANG(player.larm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_2[2], light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.rarm_rx)); slRotY(DEGtoANG(player.rarm_ry)); slRotZ(DEGtoANG(player.rarm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_2[3], light);
				}
				slPopMatrix();
						
				slPushMatrix();
				{
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(12.5));
				slRotX(DEGtoANG(player.rleg_rx)); slRotY(DEGtoANG(player.rleg_ry)); slRotZ(DEGtoANG(player.rleg_rz));
				
				slPutPolygonX((XPDATA *)enemy_data_2[4], light);
				}
				slPopMatrix();
				
			}
			slPopMatrix();
			
			
			break;
			
			
			case 3:
		
			slPushMatrix();
			{
				slTranslate(toFIXED(player.x), toFIXED(player.y), toFIXED(player.z));
				slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
				slRotX(DEGtoANG(player.bodyt_rx)); slRotY(DEGtoANG(player.bodyt_ry)); slRotZ(DEGtoANG(player.bodyt_rz));
				jo_3d_set_scalef(object_scale,object_scale,object_scale);
				slPutPolygonX((XPDATA *)enemy_data_3[0], light);
							
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.head_rx)); slRotY(DEGtoANG(player.head_ry)); slRotZ(DEGtoANG(player.head_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(-4));
				//jo_3d_set_scalef(0.5,0.5,0.5);
				slPutPolygonX((XPDATA *)enemy_data_3[1], light);
				}
				slPopMatrix();
				
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.larm_rx)); slRotY(DEGtoANG(player.larm_ry)); slRotZ(DEGtoANG(player.larm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_3[2], light);
				}
				slPopMatrix();
			
				slPushMatrix();
				{
				slRotX(DEGtoANG(player.rarm_rx)); slRotY(DEGtoANG(player.rarm_ry)); slRotZ(DEGtoANG(player.rarm_rz));
				slTranslate(toFIXED(0), toFIXED(0), toFIXED(0));
				slPutPolygonX((XPDATA *)enemy_data_3[3], light);
				}
				slPopMatrix();
						
				
				
			}
			slPopMatrix();
			
			
			break;
			
			
		}
			
			
		

}

void				map_builder_draw_powerup(void)
{
		slPushMatrix();
			{
				slTranslate(toFIXED(player.x), toFIXED(player.y), toFIXED(player.z));
				slRotX(DEGtoANG(player.rx)); slRotY(DEGtoANG(player.ry)); slRotZ(DEGtoANG(player.rz));
				jo_3d_set_scalef(object_scale,object_scale,object_scale);
				{
					slPutPolygonX((XPDATA *)pup_data[model_num], light);
				}
			}
			slPopMatrix();
		
		
		
	
}

void			    object_viewer(void)
{
	if (game.game_state != GAMESTATE_OBJECT_VIEWER)
		
       return;
   
	
	XPDATA * current_object;
	Uint32 nbPt;
	
	jo_printf(0, 0, "           *OBJECT VIEWER*");
	jo_printf(0, 1, "MODEL TYPE: %2d", object_viewer_mode);
	jo_printf(0, 2, "POLYGON: %3d", object_pol_num);
	jo_printf(20, 2, "ANIM: %d  FRAME: %2d", anim_num, player.anim_frame);
	jo_printf(20, 1, "MODEL NUMBER: %2d", object_number);
	jo_printf(0, 28, "Y TO CHANGE TYPE, B TO CHANGE OBJ NUM,  ");
	jo_printf(0, 29, "C TO SHOW POLYGON, A TO CHANGE POLYGON  ");
	
   
   jo_3d_camera_look_at(&cam);
	
   switch(object_viewer_mode)
			{
			case 0:
					object_number = model_num;	
					current_object=(XPDATA *)player_data[model_num];
						
					break;
			
			case 1:
					object_number = model_num;	
					current_object=(XPDATA *)enemy_data_1[model_num];//need to add struct for enemy models					
					break;
					
			case 2:
					object_number = model_num;	
					current_object=(XPDATA *)enemy_data_2[model_num];				
					break;
					
			case 3:
					object_number = model_num;	
					current_object=(XPDATA *)enemy_data_3[model_num];					
					break;
					
			case 4:
					object_number = model_num;	
					current_object=(XPDATA *)pup_data[model_num];				
					break;
			}
   
	nbPt = current_object->nbPolygon;
   
   
   switch(object_viewer_mode)
		{
        case 0:
				map_builder_draw_player();
				map_builder_animate_player();
				break;
				
		case 1:
				map_builder_enemy_type = 1;
				map_builder_draw_enemy();
				map_builder_animate_enemy();
				break;
				
		case 2:
				map_builder_enemy_type = 2;
				map_builder_draw_enemy();
				map_builder_animate_enemy();
				break;

		case 3:
				map_builder_enemy_type = 3;
				map_builder_draw_enemy();
				map_builder_animate_enemy();
				break;
		
		case 4:
				map_builder_draw_powerup();
				break;
		
		}
   
   
   delta_x = 0;
	delta_y = 0.0f;
	delta_z = 0;
	
	if (!jo_is_pad1_available())
		return;
	
	if (KEY_PRESS(0,PER_DGT_KU))
		{
		//rotate x	
		player.rx--;
		}
			
	if (KEY_PRESS(0,PER_DGT_KD))
		{
		//rotate -x	
		player.rx++;		
		}
	
	if (KEY_PRESS(0,PER_DGT_KL))
		{
		//rotate -y	
		player.ry--;
		}
		
	if (KEY_PRESS(0,PER_DGT_KR))
		{//rotate y		
		player.ry++;
		}	
		
		
	if (KEY_PRESS(0,PER_DGT_TL))
		{//zoom out			
		if(object_scale >1.0f)
			object_scale-=0.1f;
		}
		
	if (KEY_PRESS(0,PER_DGT_TR))
		{
		//zoom in	
		if(object_scale <20.0f)
			object_scale+=0.1f;		
		}
		
	if (KEY_DOWN(0,PER_DGT_TB))
		{
			
		if(anim_num !=0)
		{
		if(player.anim_frame >= total_frames-1)
		{
		player.anim_frame = 0;
		}else
		{
		player.anim_frame ++;		
		}
		}else
		{
			
			if(object_show_poly)
					{
						current_object->attbl[object_pol_num].texno = object_last_texture;
						current_object->attbl[object_pol_num].colno = LUTidx(object_last_texture);
						object_show_poly = false;
					}
			object_pol_num = 0;
			
			switch(object_viewer_mode)
			{
			case 0:	++model_num;
					if (model_num >= 15)
					{
					model_num = 0;	
					}  
					break;
			
			case 1:	++model_num;
					if (model_num >= 6)
					{
					model_num = 0;			
					}  
					break;
					
			case 2:	++model_num;
					if (model_num >= 5)
					{
					model_num = 0;			
					}  
					break;
					
			case 3:	++model_num;
					if (model_num >= 4)
					{
					model_num = 0;			
					}  
					break;
					
			case 4:	++model_num;
					if (model_num >= 8)
					{
					model_num = 0;			
					}  
					break;
			}
			      
		}
		}
		
	if (KEY_DOWN(0,PER_DGT_TX))
		{
			if(anim_num >=4)
			{
			anim_num=0;
			}else
			{
			anim_num++;
			}
		}
		
	if (KEY_DOWN(0,PER_DGT_TZ))
		{
			anim_loop ^= true;
		}
	
	if (KEY_DOWN(0,PER_DGT_TY))
		{
			
			if(object_show_poly)
					{
						current_object->attbl[object_pol_num].texno = object_last_texture;
						current_object->attbl[object_pol_num].colno = LUTidx(object_last_texture);
						object_show_poly = false;
					}
			object_pol_num = 0;
			
			if(object_viewer_mode >=4)
			{
			object_viewer_mode=0;
			}else
			{
			object_viewer_mode++;
			}
		}
		
	if (KEY_DOWN(0,PER_DGT_TA) && object_show_poly)
		{
			object_last_pol_num = object_pol_num;
			
			if(object_pol_num == nbPt-1)
			{
			object_pol_num = 0;
			}else
			{
			object_pol_num++;
			}
			
			current_object->attbl[object_last_pol_num].texno = object_last_texture;
			current_object->attbl[object_last_pol_num].colno = LUTidx(object_last_texture);
			object_last_texture = (Uint16) current_object->attbl[object_pol_num].texno;
			current_object->attbl[object_pol_num].texno = MAP_TILESET+58;
			current_object->attbl[object_pol_num].colno = LUTidx(MAP_TILESET+58);
			
		}
		
	if (KEY_DOWN(0,PER_DGT_TC))
		{
			if(object_show_poly)
			{
				current_object->attbl[object_pol_num].texno = object_last_texture;
				current_object->attbl[object_pol_num].colno = LUTidx(object_last_texture);
				object_show_poly = false;
			}else
			{
				object_show_poly = true;
				object_pol_num = 0;
				object_last_texture = (Uint16) current_object->attbl[object_pol_num].texno;
				object_last_pol_num = object_pol_num;
				current_object->attbl[object_pol_num].texno = MAP_TILESET+58;
				current_object->attbl[object_pol_num].colno = LUTidx(MAP_TILESET+58);
			}
		}
	
	 // new menu needed here, option to return to title screen
    if (KEY_DOWN(0,PER_DGT_ST))
		{
			if(game.pressed_start == false)
			{
				player.rx = 0;player.ry=0;player.rz=0;
				jo_printf(0, 28, "                                       ");
				jo_printf(0, 29, "                ");
				game.game_state = GAMESTATE_PAUSED;
				game.pause_menu = 0;
				jo_printf(9, 12, "*");
			}
			game.pressed_start = true;
		}
    else
		{
			game.pressed_start = false;
		}
		
		jo_printf(0, 3, "POLYS: %3d", jo_3d_get_polygon_count());
    jo_printf(20, 3, "VERTS: %3d", jo_3d_get_vertices_count());
		
}

void				player_dead(void)
{
if (game.game_state != GAMESTATE_DEAD)
       return;
	
	int x_dist;
	int y_dist;
	int z_dist;
   
   jo_printf(10, 18, "YOU DIED!");
   cam_number = 4;
   //draw screen still
   game.player_dead_timer ++;
   jo_3d_camera_look_at(&cam);
   
 
	
	for(int i = 0; i < section_total; i++)
	{
	draw_map(&map_section[i]);
	}
	draw_player();
	
	for(int e = 0; e < enemy_total; e++)
	{
	
	if(enemies[e].alive)
		{
		
		//set draw distance
		x_dist = JO_ABS(player.x - enemies[e].x);
		y_dist = JO_ABS(player.y - enemies[e].y);
		z_dist = JO_ABS(player.z - enemies[e].z);
		
			if((x_dist + y_dist + z_dist) < DRAW_DISTANCE)
			
			{
			draw_enemies(&enemies[e]);
			}
			
		}
	
	}
	
	for(int p = 0; p < powerup_total; p++)
	{
	
	if(!powerups[p].used)
		{
		//set draw distance
		x_dist = JO_ABS(player.x - powerups[p].x);
		y_dist = JO_ABS(player.y - powerups[p].y);
		z_dist = JO_ABS(player.z - powerups[p].z);
		
			if((x_dist + y_dist + z_dist) < DRAW_DISTANCE)
			
			{
			draw_powerups(&powerups[p]);
				
			}
		}
	
	}
	
	if(game.player_dead_timer >= PLAYER_DEAD_TIMER)
	{
	game.player_dead_timer = 0;
	jo_printf(0, 18, "                              ");
	game.game_state = GAMESTATE_PAUSED;
		
		
	}
	
}

void			    pause_game(void)
{
	if (game.game_state != GAMESTATE_PAUSED)
       return;
   int x_dist;
	int y_dist;
	int z_dist;
   
   //draw screen still
   
   jo_3d_camera_look_at(&cam);
	
	slCurWindow(winFar);
	
	for(int i = 0; i < section_total; i++)
	{
	draw_map(&map_section[i]);
	}
	draw_player();
	
	for(int e = 0; e < enemy_total; e++)
	{
	
	if(enemies[e].alive)
		{
		
		//set draw distance
		x_dist = JO_ABS(player.x - enemies[e].x);
		y_dist = JO_ABS(player.y - enemies[e].y);
		z_dist = JO_ABS(player.z - enemies[e].z);
		
			if((x_dist + y_dist + z_dist) < DRAW_DISTANCE)
			
			{
			draw_enemies(&enemies[e]);
			}
			
		}
	
	}
	
	for(int p = 0; p < powerup_total; p++)
	{
	
	if(!powerups[p].used)
		{
		//set draw distance
		x_dist = JO_ABS(player.x - powerups[p].x);
		y_dist = JO_ABS(player.y - powerups[p].y);
		z_dist = JO_ABS(player.z - powerups[p].z);
		
			if((x_dist + y_dist + z_dist) < DRAW_DISTANCE)
			
			{
			draw_powerups(&powerups[p]);
				
			}
		}
	
	}
	
	slCurWindow(winNear);
	
	draw_hud();
	
	jo_sprite_enable_half_transparency();
		//jo_sprite_change_sprite_scale(22);
		jo_sprite_change_sprite_scale_xy(22,22);
		jo_sprite_draw3D(28,0, 0, 100);
		jo_sprite_restore_sprite_scale();
		
		jo_sprite_disable_half_transparency();	
	
	
   
   ///
   
    jo_printf(0, 1, "                   PAUSED");
	
  //  jo_3d_camera_look_at(&cam);
		
	
	
	jo_printf(10, 12, "CONTINUE");
    jo_printf(10, 13, "RESTART LEVEL");
    jo_printf(10, 14, "LOAD MAP %d",game.level);
	jo_printf(10, 15, "QUIT TO TITLE SCREEN");
	jo_printf(9, 12 + game.pause_menu, ">");
		
	if (KEY_DOWN(0,PER_DGT_KU))
		
    {
            jo_printf(9, 12, " ");
            jo_printf(9, 13, " ");
            jo_printf(9, 14, " ");
			jo_printf(9, 15, " ");
			
            if (game.pause_menu == 0)
                game.pause_menu = PAUSE_MENU_MAX;
            else
                game.pause_menu --;

    }
  

   if (KEY_DOWN(0,PER_DGT_KD))
    {
       
            jo_printf(9, 12, " ");
            jo_printf(9, 13, " ");
            jo_printf(9, 14, " ");
			jo_printf(9, 15, " ");

            if (game.pause_menu == PAUSE_MENU_MAX)
                game.pause_menu = 0;
            else
                game.pause_menu ++;
    
   }
   
   if (KEY_DOWN(0,PER_DGT_KR))
    {
	if(	game.pause_menu == 2)
	{
	
		if(game.level > 0 && game.level < MAX_LEVEL)
				{game.level ++;}
				else
				if(game.level == MAX_LEVEL)
				{game.level = 1;}	
	}
	}
   

    // did player one pause the game?
   if (KEY_DOWN(0,PER_DGT_ST))
    {
        if(player.pressed_start == false)
        {
            jo_clear_screen();
            if (game.pause_menu == 1)
            {
                game.game_state = GAMESTATE_GAMEPLAY;
				
                restart_level();
				//num_players = game.players;
            }

            else if (game.pause_menu == 2)
            {
				
					
				game.map_section = 1;
				game.game_state = GAMESTATE_LOAD_LEVEL;
				
			 }
			 else if (game.pause_menu == 3)
			 {
				 //quit to title
				 
				transition_to_title_screen();
				 
				 
			 } 
            else
			{game.game_state = GAMESTATE_GAMEPLAY;
			
			}


        }
       player.pressed_start = true;
   }
   else
   {
    player.pressed_start = false;
   }
	
		
}

void				move_camera(void)

{
	
	if(game.game_state != GAMESTATE_GAMEPLAY && game.game_state != GAMESTATE_DEAD)
	return;
	
	int collide;
	//int map = 0;
	Uint16 x_dist;
	Uint16 y_dist;
	Uint16 z_dist;
	Uint16 collpoints_total;
	Sint16 collpoints_x;
	Sint16 collpoints_y;
	Sint16 collpoints_z;
	Uint16 collpoints_xsize;
	Uint16 collpoints_ysize;
	Uint16 collpoints_zsize;
	Uint8 collpoints_type;
	
	
	if(first_person_cam == 0)
	{
	///3rd person camera
	//cam_pos_x = player.x + cam_adj_x;
	//cam_pos_y = player.y + cam_height;
	//cam_pos_z = player.z + cam_adj_z;
	
	if(player.aim)
	{
	player_target();
	}
		if(cam_number ==0 || toggle_cam)
		{
		
			
		
			/*if(player.aim)
			{
			player_target();
			///cam behind player (over shoulder)
				if(player.target >-1)
				{
				cam_pos_x =  -(-player.x*32768 + 100 * jo_sin(player.ry))/32768;
				cam_pos_y = player.y - 48;//(player.y*32768 + 100 * jo_sin(cam_angle_x))/32768;
				cam_pos_z = -(-player.z*32768 + 100 * jo_cos(player.ry))/32768;
					
				cam_target_x = player.tx;
				cam_target_y = player.ty;
				cam_target_z = player.tz;
				}else
				{
				cam_pos_x = -(-player.x*32768 + 100 * jo_sin(player.ry))/32768;
				cam_pos_y = player.y + cam_height;//(player.y*32768 + 100 * jo_sin(cam_angle_x))/32768;
				cam_pos_z = -(-player.z*32768 + 100 * jo_cos(player.ry))/32768;
					
				
				}
			}else
			{*/
				///cam behind player
				
				
				//next camera position
				
				
				cam_pitch = JO_DEG_TO_RAD(jo_atan2f(-(player.x - cam_pos_x),-(player.z - cam_pos_z)))+cam_pitch_adj;
				cam_angle_y = player.ry + cam_angle_adj;//need to change this, cam_angle_adj keeps increasing when stopped by wall. use cam speed and next_cam_angle_y to only adj cam_angle_y when not hitting wall?
				cam_horiz_dist = JO_DIV_BY_32768(cam_dist * jo_cos(cam_pitch));
				cam_vert_dist = JO_DIV_BY_32768(cam_dist * jo_sin(cam_pitch));
				
				next_cam_pos_x = -JO_DIV_BY_32768((JO_MULT_BY_32768(-player.x) + cam_horiz_dist * jo_sin(cam_angle_y)));
				next_cam_pos_z = -JO_DIV_BY_32768((JO_MULT_BY_32768(-player.z) + cam_horiz_dist * jo_cos(cam_angle_y)));
				
				next_cam_pos_y = player.y + cam_vert_dist + cam_height;
				
				/*if(show_debug)
				{
				jo_printf(0, 1, "next_cam: \t%3d\t%3d", next_cam_pos_x, next_cam_pos_z);
				jo_printf(20, 1, "cam_angle_adj: %4d", cam_angle_adj);
				}*/
				
				for(Uint16 i = 0; i < section_total; i++)
				{
				//set map section distance	
				x_dist = JO_ABS(player.x - map_section[i].x);
				y_dist = JO_ABS(player.y - map_section[i].y);
				z_dist = JO_ABS(player.z - map_section[i].z);
	
				section_dist = x_dist + y_dist + z_dist;
				collpoints_total = map_section[i].a_cdata->nbCo;
		
					for(Uint16 c = 0; c < collpoints_total; c++)
					{
					collpoints_x = map_section[i].a_collison[c].cen_x + map_section[i].x + map_section[i].tx;
					collpoints_y = map_section[i].a_collison[c].cen_y + map_section[i].y + map_section[i].ty;
					collpoints_z = map_section[i].a_collison[c].cen_z + map_section[i].z + map_section[i].tz;
				
					collpoints_xsize = map_section[i].a_collison[c].x_size;
					collpoints_ysize = map_section[i].a_collison[c].y_size;
					collpoints_zsize = map_section[i].a_collison[c].z_size;
					collpoints_type = map_section[i].a_collison[c].att;
					
					///check for X axis collision
					//collide = has_horizontal_collision(&map_section[map],next_cam_pos_x,cam_pos_y,cam_pos_z,16,16,16);
					collide = has_horizontal_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,next_cam_pos_x,cam_pos_y,cam_pos_z,32,32,32);
					if(collide)
					{ 
					next_cam_pos_x = cam_pos_x;
					
					}
					
					if(show_debug)
					{				
					jo_printf(0, 2, "x_collide: %d", collide);
					}
					///check for Z axis collision
					//collide = has_horizontal_collision(&map_section[map],cam_pos_x,cam_pos_y,next_cam_pos_z,16,16,16);
					collide = has_horizontal_collision(collpoints_type, collpoints_x, collpoints_y, collpoints_z, collpoints_xsize, collpoints_ysize, collpoints_zsize,cam_pos_x,cam_pos_y,next_cam_pos_z,32,32,32);
					if(collide)
					{ 
					next_cam_pos_z = cam_pos_z;
					}
					
					if(show_debug)
					{
					jo_printf(20, 2, "z_collide: %d", collide);
					}
					//CEILING COLLISION
				/*	collide = has_ceiling_collision(&map_section[map],cam_pos_x,next_cam_pos_y,cam_pos_z,16,16,16,-1);
					if(collide)
					{
					next_cam_pos_y = cam_pos_y;
					}*/
					}
					
				
				}
				 
				
				
				
				
				
				cam_pos_x = next_cam_pos_x;
				cam_pos_y = next_cam_pos_y;
				cam_pos_z = next_cam_pos_z;
				
				if(player.aim && player.target >-1)
				{
				
				
				cam_target_x = player.tx;
				cam_target_y = player.ty + cam_height;
				cam_target_z = player.tz;
				}else
				{
				
				cam_target_x = player.x;
				cam_target_y = player.y + cam_height;
				cam_target_z = player.z;
					
				}
				
				//cam_angle_y = jo_atan2f((cam_pos_x - cam_target_x),(cam_pos_z - cam_target_z));
				
				
			//}
				
		}else
		{
				
			if(player.aim && player.target >-1)
			{
			
			cam_target_x = player.tx;
			cam_target_y = player.ty;
			cam_target_z = player.tz;
			}else
			{
				cam_target_x = player.x;
					cam_target_y = player.y-32;
					cam_target_z = player.z-32;
			}
			switch(cam_number)
			{
			case 1: cam_pos_x = cam_1_x;
					cam_pos_y = -32 + cam_height + cam_1_y;
					cam_pos_z = cam_1_z;	
					
					break;
					
			case 2: cam_pos_x = cam_2_x;
					cam_pos_y = -32 + cam_height + cam_2_y;
					cam_pos_z = cam_2_z;	
					
					break;
					
			case 3: cam_pos_x = cam_3_x;
					cam_pos_y = -32 + cam_3_y;
					cam_pos_z = cam_3_z;	
					
					break;
					
			case 4: //player dead
					if(game.player_dead_timer < PLAYER_DEAD_TIMER)
					{
				cam_angle_y++;
				cam_pos_y --;
					}
					
				cam_pitch = JO_DEG_TO_RAD(jo_atan2f(-(player.x - cam_pos_x),-(player.z - cam_pos_z)))+cam_pitch_adj;
				cam_horiz_dist = JO_DIV_BY_32768(cam_dist * jo_cos(cam_pitch));
				cam_vert_dist = JO_DIV_BY_32768(cam_dist * jo_sin(cam_pitch));
				cam_pos_x = -JO_DIV_BY_32768((JO_MULT_BY_32768(-player.x) + cam_horiz_dist * jo_sin(cam_angle_y)));
				cam_pos_z = -JO_DIV_BY_32768((JO_MULT_BY_32768(-player.z) + cam_horiz_dist * jo_cos(cam_angle_y)));
				cam_target_x = player.x;
				cam_target_y = player.y +cam_height ;
				cam_target_z = player.z;
			}
			///static room cam
						

			
		
		}
	
	}
	else
	{
	///1st person camera
		if(!drop_camera)
		{
		
		cam_pos_x = player.nextx;
		cam_pos_y = player.nexty -48;
		cam_pos_z = player.nextz;
		}
		cam_angle_y = player.ry;
		cam_angle_x = player.head_rx;
		
	cam_target_x = (cam_pos_x*32768 + 100 * jo_sin(cam_angle_y))/32768;
	cam_target_y = (cam_pos_y*32768 + 100 * jo_sin(cam_angle_x))/32768;
	cam_target_z = (cam_pos_z*32768 + 100 * jo_cos(cam_angle_y))/32768;
	
	}
	
	
						
	jo_3d_camera_set_viewpoint(&cam,cam_pos_x,cam_pos_y,cam_pos_z);
	jo_3d_camera_set_target(&cam,cam_target_x,cam_target_y,cam_target_z);
	
	
}



void            title_screen(void)
{
	
	if (game.game_state != GAMESTATE_TITLE_SCREEN)
    return;
		
	//play cd audio
	
	if (!is_cd_playing)
        {
             
			 CDDAPlaySingle(2, true);
            is_cd_playing = true;
        }
	
	jo_3d_camera_look_at(&cam);
	
	jo_3d_camera_set_viewpoint(&cam,0,0,-500);
	jo_3d_camera_set_target(&cam,0,0,0);
	
	slPushMatrix();
    {
        if(use_light) computeLight();
    
		
    }
	slPopMatrix();
	
jo_printf(14, 24, "START GAME");
jo_printf(14, 25, "OBJECT_VIEWER");
jo_printf(13, 24 + game.title_screen_menu, ">");

slCurWindow(winFar);
				slPushMatrix();
					{
						jo_3d_set_scalef(game.title_size,game.title_size,game.title_size);
						slTranslate(toFIXED(0), toFIXED(100), toFIXED(100+player.z));
						slRotX(DEGtoANG(0));slRotY(DEGtoANG(180));slRotZ(DEGtoANG(player.head_ry));
						//slRotX(DEGtoANG(180));slRotY(DEGtoANG(20));slRotZ(DEGtoANG(player.head_ry));
						
						slPutPolygonX(map_section[0].map_model, light);
						//slPutPolygonX((XPDATA *)&xpdata_CC_LOGO, light);
						

					}
					slPopMatrix();
slCurWindow(winNear);			
					slPushMatrix();
					{
						jo_3d_set_scalef(game.title_size,game.title_size,game.title_size);
						slTranslate(toFIXED(0), toFIXED(100), toFIXED(player.z));
						slRotX(DEGtoANG(0));slRotY(DEGtoANG(200));slRotZ(DEGtoANG(player.head_ry));
						//slRotX(DEGtoANG(180));slRotY(DEGtoANG(20));slRotZ(DEGtoANG(player.head_ry));
						
						//slPutPolygonX(map_section[0].map_model, light);
						slPutPolygonX((XPDATA *)&xpdata_CC_LOGO, light);
						

					}
					slPopMatrix();

	if (KEY_DOWN(0,PER_DGT_KU))
		
    {
      //  if(player.pressed_up == false)
      //  {

            jo_printf(13, 24, " ");
            jo_printf(13, 25, " ");
           
			
            if (game.title_screen_menu == 0)
                game.title_screen_menu = TITLE_SCREEN_MENU_MAX;
            else
                game.title_screen_menu --;



            //jo_printf(13, 24 + game.title_screen_menu, ">");
			jo_clear_screen();

       // }
       // player.pressed_up = true;
   }
   // else
   // {
   //     player.pressed_up = false;
   // }

   if (KEY_DOWN(0,PER_DGT_KD))
    {
       // if(player.pressed_down == false)
      //  {
            jo_printf(13, 24, " ");
            jo_printf(13, 25, " ");
          

            if (game.title_screen_menu == TITLE_SCREEN_MENU_MAX)
                game.title_screen_menu = 0;
            else
                game.title_screen_menu ++;



            jo_clear_screen();
      //  }
      //  player.pressed_down = true;
    }
   // else
   // {
   //     player.pressed_down = false;
   // }
	
	
	if (KEY_DOWN(0,PER_DGT_ST))
	 {
		switch(game.title_screen_menu)
		{
		case 0:		//jo_audio_stop_cd();
					CDDAPlaySingle(3, false);
					//jo_audio_play_cd_track(3, 3, false);
					game.start_timer = true;
					break;
					
		case 1:		jo_clear_screen();
					//jo_audio_stop_cd();
					is_cd_playing = false;
					game.game_state = GAMESTATE_OBJECT_VIEWER;
					break;
		
		}
		
		//game.pressed_start = true;
		
		
		
	 }
	 
	 if(game.start_timer)
	{	if(player.z > -1000)
		{player.z-=4;
		player.head_ry ++;
		slSetGouraudColor(CD_Red);
		slSetDepthTbl(DepthDataRed,0xf000,32);
		}
		
	game.title_screen_timer ++;
	
	
	
	
	
	}else
	{
	game.title_size = 1.0f;	
	}
	
	if(game.title_screen_timer >= TITLE_SCREEN_TIMER)
	{
 	
		is_cd_playing = false;
		init_level();
		reset_demo();
		game.map_section = 1;
		game.level = 1;
		game.game_state = GAMESTATE_LOAD_LEVEL;
		
		
		
	}
	 
	
	
}

void my_vblank()
{

    if(enableRTG == 1)
        slGouraudTblCopy();
}

void init_display(void)
{
	
	jo_core_init(JO_COLOR_White);
    	
    jo_3d_camera_init(&cam);
	
	slCurWindow(winFar);
	jo_3d_window(0, 0, JO_TV_WIDTH-1, JO_TV_HEIGHT-1, DRAW_DISTANCE_MAX, JO_TV_WIDTH_2, JO_TV_HEIGHT_2);
	
	slSetDepthLimit(0,8,5);
	slSetDepthTbl(DepthDataBlack,0xf000,32);
	slInitGouraud(gourRealMax, GOUR_REAL_MAX, GRaddr, vwork);
	slIntFunction(my_vblank);
	
	/**Set your color here if you need one (depending on your light source)**/
    slSetGouraudColor(CD_White);
	
	slCurWindow(winNear);
	slWindow(0, 0, JO_TV_WIDTH-1, JO_TV_HEIGHT-1, DRAW_DISTANCE, JO_TV_WIDTH_2, JO_TV_HEIGHT_2); 
	
}




void			my_gamepad(void)
{
	if (game.game_state != GAMESTATE_GAMEPLAY)
       return;
	
	JO_ZERO(player.delta_x);
	player.delta_y = 0.0f;
	JO_ZERO(player.delta_z);
	
	
		
		
		if (KEY_PRESS(0,PER_DGT_KU))
					{	
						player.head_cam_adj = 0;
						if(cam_number == 0)
						{
							player.ry = cam_angle_y;
							cam_angle_adj  = 0;
						}	
						if(KEY_PRESS(0,PER_DGT_TL))
						{
							if(cam_pitch_adj < 55)
							{cam_pitch_adj +=CAM_SPEED;	}
						}else
						{
						player.speed_z = player.speed;
						}
												
						
					}//up
		else
		if (KEY_PRESS(0,PER_DGT_KD))
					{
						player.head_cam_adj = 0;
						if(KEY_PRESS(0,PER_DGT_TL))
							{
								if(cam_pitch_adj > -80)
								{cam_pitch_adj -=CAM_SPEED;	}
							}else
							{						
							player.speed_z = -player.speed;
							}
						}//down
		else
		{player.speed_z = 0;}
	
		
		if (KEY_PRESS(0,PER_DGT_KL))
					{
						if(cam_number == 0 && !player.aim)//KEY_PRESS(0,PER_DGT_TL))
						{
							if(KEY_PRESS(0,PER_DGT_TL))
							{
								if(cam_dist <60)
								{
									cam_dist +=CAM_SPEED;
								}
									
							}else
							{cam_angle_adj -=CAM_SPEED;	
							player.head_cam_adj = cam_angle_adj;	
							}
						}else
						{						
						player.ry -=CAM_SPEED;	
											
						}
					}//left
			
		if (KEY_PRESS(0,PER_DGT_KR))
					{
						if(cam_number == 0 && !player.aim)//KEY_PRESS(0,PER_DGT_TL))
						{
							if(KEY_PRESS(0,PER_DGT_TL))
							{
								if(cam_dist >10)
								{
									cam_dist -=CAM_SPEED;
								}
									
							}else
							{cam_angle_adj +=CAM_SPEED;	
							player.head_cam_adj = cam_angle_adj;		
							}
						}else
						{						
						player.ry +=CAM_SPEED;
										
						}
					}//right
		
	
		
		if (KEY_PRESS(0,PER_DGT_TZ)&& player.rarm_rx > -RARM_ADJ)
					{player.rarm_rx -=CAM_SPEED;}//z(look up)			
		
		//if (KEY_PRESS(0,PER_DGT_TC)&& player.rarm_rx < RARM_ADJ)
		//			{player.rarm_rx +=CAM_SPEED;}//c(look down)	
		
		
		///jump
		if (player.can_jump && (KEY_DOWN(0,PER_DGT_TC)))
		{player_jump();
		
		}
		///run
		if (KEY_PRESS(0,PER_DGT_TB))
		{player.speed = PLAYER_RUN_SPEED;
		player.run = true;
		
		}else
		{
		player.speed = PLAYER_SPEED;	
		player.run = false;
		}
		
		if (KEY_DOWN(0,PER_DGT_TA))
		{
			if(player.aim)
			{
				if(player.current_weapon->bullets_in_clip == 0)
				{
				player_reload(player.current_weapon);
				}else
				{
				player.shoot = true;
				}
			}else
			{
			player.action = true;
			}
		}else
		{
		player.shoot = false;
		player.action = false;
		}
		
		
		if (KEY_PRESS(0,PER_DGT_TR))
		 {
			 if(!player.aim)
			 {
			player.aim = true;
			//player.rarm_rx = RARM_ADJ;
			 }
		 }else
		 {
			 if(player.aim)
			 {
			player.aim = false;
			player.rarm_rx = 0;
			 }
		 }
		
		if (KEY_PRESS(0,PER_DGT_TL) && KEY_DOWN(0,PER_DGT_TR))
		 {
			 jo_clear_screen();
			 show_debug ^= true;
		 }
		
		
		
		 if (KEY_DOWN(0,PER_DGT_TX))
		 {
			  pcm_play(map_sound, PCM_VOLATILE, 6);
			 jo_clear_screen();
			 game.game_state = GAMESTATE_MAP;
		 }
		 
		 if(KEY_DOWN(0,PER_DGT_TY))
		{
			toggle_cam ^= true;
		}
		
		  if (KEY_DOWN(0,PER_DGT_TZ))
		{
			jo_clear_screen();
			game.game_state = GAMESTATE_INVENTORY;
		}
		 
		 
		 
		  
	
	 if (KEY_DOWN(0,PER_DGT_ST))
	 {
		if(player.pressed_start == false)
		{
			player.pressed_start = true;
				jo_printf(0, 28, "                                       ");
				jo_printf(0, 29, "                ");
				game.game_state = GAMESTATE_PAUSED;
				game.pause_menu = 0;
				jo_printf(9, 12, ">");
		}
			
		}
   else
	{
	player.pressed_start = false;
	}
   
   
 
   

	
	
		
	
	if (cam_angle_y > 180)
		cam_angle_y -=360;
	else if (cam_angle_y <= -180)
		cam_angle_y +=360;
	
	if (player.ry > 180)
		player.ry -=360;
	else if (player.ry <= -180)
		player.ry +=360;
	
	if (cam_angle_adj > 180)
		cam_angle_adj -=360;
	else if (cam_angle_adj <= -180)
		cam_angle_adj +=360;
	
	if(player.head_cam_adj < -75 || player.head_cam_adj > 75)
	{
		player.head_cam_adj = 0;
	}
   
	
}



void            load_background()
{
    jo_img      bg;

    bg.data = JO_NULL;
    jo_tga_loader(&bg, "BG", "SKY.TGA", JO_COLOR_Transparent);
    jo_set_background_sprite(&bg, 0, 0);
    jo_free_img(&bg);
}





void			load_textures(void)
{
	
		
	jo_printf(0, 0, "Loading textures......[1]");
	jo_sprite_add_tga_tileset("TEX", "PLAYER.TGA",JO_COLOR_Red,PLAYER_Tileset,37);
	jo_printf(0, 0, "Loading textures......[2]");
	skybox_tex = jo_sprite_add_tga("TEX", "SKY.TGA", JO_COLOR_Transparent);
	jo_printf(0, 0, "Loading textures......[3]");
	pspark_tex = jo_sprite_add_tga("TEX", "PSPARK.TGA", JO_COLOR_Blue);
	jo_printf(0, 0, "Loading textures......[4]");
	pblood_tex = jo_sprite_add_tga("TEX", "PBLOOD.TGA", JO_COLOR_Blue);
	jo_printf(0, 0, "Loading textures......[5]");
	health_bar_tex = jo_sprite_add_tga("TEX", "HBAR.TGA", JO_COLOR_Red);
	jo_printf(0, 0, "Loading textures......[6]");
	
	jo_sprite_add_tga_tileset("TEX", "ENEMY.TGA",JO_COLOR_Red,ENEMY_Tileset,12);
	
}


void			load_sound(void)
{
	//23040, 15360, 11520, or 7680Hz can be used.
	step_sound = load_16bit_pcm((Sint8 *)"STEP5.PCM", 7680);
	die_sound = load_16bit_pcm((Sint8 *)"DIE.PCM", 7680);
	ouch_sound = load_16bit_pcm((Sint8 *)"UMPH.PCM", 7680);
	jump_sound = load_16bit_pcm((Sint8 *)"HUH2.PCM", 23040);
	pup_sound = load_16bit_pcm((Sint8 *)"BUTTON.PCM", 7680);
	flap_sound = load_16bit_pcm((Sint8 *)"FLAP.PCM", 7680);
	door_sound = load_16bit_pcm((Sint8 *)"DOOR2.PCM", 7680);
	pistol_sound = load_16bit_pcm((Sint8 *)"PISTOL.PCM", 7680);
	reload_sound = load_16bit_pcm((Sint8 *)"RELOAD.PCM", 7680);
	empty_sound = load_16bit_pcm((Sint8 *)"EMPTY.PCM", 7680);
	shotgun_sound = load_16bit_pcm((Sint8 *)"SGUN.PCM", 7680);
	ladder_sound = load_16bit_pcm((Sint8 *)"LADR.PCM", 7680);
	alert_sound = load_16bit_pcm((Sint8 *)"ALERT.PCM", 7680);
	alert2_sound = load_16bit_pcm((Sint8 *)"SQK.PCM", 7680);
	map_sound = load_16bit_pcm((Sint8 *)"MAP.PCM", 7680);
	
}



void                            draw_3d_planes(void)
{
	
	if(!draw_sky)
	return;
	
   // SKY
    jo_3d_push_matrix();
	{
		jo_3d_rotate_matrix(-cam_pitch, -cam_angle_y, cam_angle_z);
		jo_3d_rotate_matrix_rad(rot.rx, rot.ry, rot.rz);
		
		jo_3d_translate_matrixf(pos.x, pos.y, pos.z);
        jo_background_3d_plane_a_draw(true);
	}
	jo_3d_pop_matrix();
	pos.y += 0.1f;
	pos.x += 0.1f;
	//FLOOR
	 jo_3d_push_matrix();
	{
		jo_3d_rotate_matrix(-cam_pitch, -cam_angle_y, cam_angle_z);
		jo_3d_rotate_matrix_rad(rot.rx, rot.ry, rot.rz);
		
		
		jo_3d_translate_matrixf(-player.x,-player.z,player.y - player.ysize);
        jo_background_3d_plane_b_draw(true);
	}
	jo_3d_pop_matrix();
	//pos.z ++;
    //SKY
	/*jo_3d_push_matrix();
	{
	    jo_3d_translate_matrix(rot.rz * 100, 110 - 112 * jo_cos_radf(rot.rx), 100);
        jo_3d_rotate_matrix_rad_z(-rot.ry);
        jo_background_3d_plane_b_draw(false);
	}
	jo_3d_pop_matrix();*/
}



void                init_3d_planes(void)
{
    jo_img_8bits    img;

    ///Added by XL2 : turns off the TV while we load data
    slTVOff();
    

    jo_enable_background_3d_plane(JO_COLOR_Black);

    // SKY
    img.data = JO_NULL;
    jo_tga_8bits_loader(&img, "BG", "SKY6.TGA", 0);
    jo_background_3d_plane_a_img(&img, image_pal.id, true, true);
    jo_free_img(&img);

    //FLOOR
    img.data = JO_NULL;
    jo_tga_8bits_loader(&img, "BG", "FLOOR.TGA", -1);
    jo_background_3d_plane_b_img(&img, image_pal.id, true, false);
    jo_free_img(&img);

   ///Added by XL2 : turns off the TV while we load data
	slTVOn();
    
}
///taken from REYE scary game
/*void LoadLineColorTable() {
    int line;

    // Sets palette
    Uint16 * Line_Color_Pal0 = (Uint16 *)COLOR_RAM_ADR;
    for(line = 0; line < JO_TV_HEIGHT; line++)
    {
        int color = (line - 128) * 2;
        color = JO_ABS(color);

    	Line_Color_Pal0[line+32] = JO_COLOR_RGB(color,color,color);
    }

    // Set indexes to palette
    Line_Color_Pal0	=(Uint16 *)LINE_COLOR_TABLE;
    for(line = 0; line < JO_TV_HEIGHT; line++)
    {
    	Line_Color_Pal0[line] = line + ((256*3)+32);
    }

    slLineColDisp(RBG0ON);
    slLineColTable((void *)LINE_COLOR_TABLE);
    slColorCalc(CC_RATE | CC_2ND | RBG0ON);
    slColorCalcOn(RBG0ON);
    slColRateLNCL(0x0a);
}*/


void gameLoop(void)
{
   	while (1)
    {
		sdrv_vblank_rq();
        slUnitMatrix(0);
		draw_3d_planes();
        my_gamepad();
		move_camera();
		object_viewer();
		title_screen();
        my_draw();
		pause_game();
		player_dead();
		inventory();
		show_map_plan();
		load_level();
		load_area();
        slSynch();
    }
}


void			jo_main(void)
{
	
	init_display();
	load_drv(ADX_MASTER_2304);
	
	 /**Added by XL2 **/
	slDynamicFrame(ON); //Dynamic framerate, when the VDP1 can't fully draw to the framebuffer in the allocated amount of time (1/60, 2/60, etc.) it will continue drawing when it's ON. Else it will try to complete drawing by skipping lines and details and finish in the allocated time. If the app runs well with the value at OFF, leave it at OFF!
    SynchConst=(Sint8)2;  //Framerate control. 1/60 = 60 FPS, 2/60 = 30 FPS, etc.
	framerate=2;
	
	jo_set_tga_palette_handling(my_tga_palette_handling);
	init_3d_planes();
	//LoadLineColorTable();
	game.game_state = GAMESTATE_TITLE_SCREEN;
	
	game.level=1;
	
	jo_printf(0, 0, "Loading textures........[ ]");

	load_textures();
	jo_printf(0, 0, "Loading textures........[X]");
	
	
	
	create_player();
	//load_background();
	
	jo_printf(0, 2, "Loading sound........[ ]");
	load_sound();
	jo_printf(0, 2, "Loading sound........[X]");
	
	jo_printf(0, 3, "Loading map tex......[ ]");
	load_map_textures("STREET.TGA",64);
	jo_printf(0, 3, "Loading map tex......[X]");
	jo_printf(0, 4, "Loading map..........[ ]");
	load_binary((char*)"TITLE.BIN", (void*)WORK_RAM_LOW);
	jo_printf(0, 4, "Loading map..........[X]");
	init_level();
	
	//reset_demo();
	
	//load_hud();
	
	 pos.x = 800.0;
	pos.y = 0.0;
	
	pos.z = 35.0;

		rot.rx = JO_DEG_TO_RAD(90.0);
	rot.ry = JO_DEG_TO_RAD(0.0);
	rot.rz = JO_DEG_TO_RAD(0.0);
	slZdspLevel(5);
	slSetGouraudColor(JO_COLOR_RGB(255,253,156));
	//transition_to_title_screen();
	

	gameLoop();
}

/*
** END OF FILE
*/
