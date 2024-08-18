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

#ifndef __HAMSTER_H__
# define __HAMSTER_H__

# define WORLD_DEFAULT_X                (0)
# define WORLD_DEFAULT_Y                (-64)
# define WORLD_DEFAULT_Z                (0)


# define CAM_DEFAULT_X                (0)
# define CAM_DEFAULT_Y                (-53) //-64
# define CAM_DEFAULT_Z                (-150)//-150
# define CAM_DIST_DEFAULT				(40)//150
# define CAM_SPEED                		(5)//5
# define CAM_MAX_DIST              		(400)
# define DRAW_DISTANCE					(600)
# define DRAW_DISTANCE_2					(1536)
# define DRAW_DISTANCE_3					(2304)
# define DRAW_DISTANCE_MAX					(2304)//768
# define ENEMY_ACTIVE_DISTANCE			(300)
//# define FOV						(70)

//define game states
#define GAMESTATE_UNINITIALIZED         (0)
#define GAMESTATE_TITLE_SCREEN          (1)
#define GAMESTATE_LOAD_LEVEL          	(2)
#define GAMESTATE_GAMEPLAY              (3)
#define GAMESTATE_PAUSED                (4)
#define GAMESTATE_GAME_OVER             (5)
#define GAMESTATE_VICTORY               (6)
#define GAMESTATE_MAP_BUILDER           (7)
#define GAMESTATE_LOAD_AREA				(8)
#define GAMESTATE_INVENTORY				(9)
#define GAMESTATE_OBJECT_VIEWER			(10)
#define GAMESTATE_DEAD					(11)
#define GAMESTATE_MAP					(12)
#define GAMESTATE_PUZZLE				(13)
#define GAMESTATE_TRANSITION_TO_MIRROR	(14)

//define menu options
#define PAUSE_MENU_MAX               (3)
#define LEVEL_MENU_MAX               (3)
#define TITLE_SCREEN_MENU_MAX               (1)
# define TITLE_SCREEN_TIMER              (150)

#define ANIM_SPEED               (2)//1
#define MAX_PLAYERS         (2)
#define PLAYER_SPEED         (2)//1
#define PLAYER_RUN_SPEED         (4)//2
#define PLAYER_HURT_TIMER        (60)
#define ENEMY_HURT_TIMER        (20)
#define MESSAGE_TIMER        (90)
#define PLAYER_DEAD_TIMER        (120)
#define NO_RAMP_COLLISION		(0)
#define CEILING_COLLISION		(999999)
#define RARM_ADJ				(90)



typedef struct {
	
   	Uint16		id;
	Uint8		type;
	char		name[20];
	char		description[50];
	bool		used;

} ITEM;

typedef struct {
	ITEM		*itemtbl;
	Uint32		 nbIt;
}ITEMDATA;


typedef struct {
	
   	Sint16		y_adj;
	Sint16		bodyt_rx;
	Sint16		bodyt_ry;
	Sint16		bodyt_rz;
	Sint16		head_rx;
	Sint16		head_ry;
	Sint16		head_rz;
	Sint16		rarm_rx;
	Sint16		rarm_ry;
	Sint16		rarm_rz;
	Sint16		rarm2_rx;
	Sint16		rarm2_ry;
	Sint16		rarm2_rz;
	Sint16		larm_rx;
	Sint16		larm_ry;
	Sint16		larm_rz;
	Sint16		larm2_rx;
	Sint16		larm2_ry;
	Sint16		larm2_rz;	
	Sint16		bodyb_rx;
	Sint16		bodyb_ry;
	Sint16		bodyb_rz;
	Sint16		rleg_rx;
	Sint16		rleg_ry;
	Sint16		rleg_rz;
	Sint16		rleg2_rx;
	Sint16		rleg2_ry;
	Sint16		rleg2_rz;
	Sint16		rleg3_rx;
	Sint16		rleg3_ry;
	Sint16		rleg3_rz;
	Sint16		lleg_rx;
	Sint16		lleg_ry;
	Sint16		lleg_rz;
	Sint16		lleg2_rx;
	Sint16		lleg2_ry;
	Sint16		lleg2_rz;
	Sint16		lleg3_rx;
	Sint16		lleg3_ry;
	Sint16		lleg3_rz;
	

} ANIM_FRAME;

typedef struct {
	ANIM_FRAME		*animtbl;
	Uint32		 nbAn;
}ANIMDATA;

ANIM_FRAME Frames_player_idle[]={
	
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	
};
ANIMDATA	Anim_player_idle={
	Frames_player_idle,2
};

ANIM_FRAME Frames_player_walk[]={

{0,0,4,0,-3,0,0,1,0,-8,-2,0,3,0,0,8,20,-3,-7,0,4,0,9,-4,-2,-24,1,5,10,-1,1,11,0,0,-52,-3,-12,0,-9,10},
{0,0,2,0,-4,2,0,4,1,-9,4,1,2,-2,0,7,16,-3,-6,0,2,0,3,-2,-2,-24,0,7,9,0,0,16,0,0,-45,-2,-9,2,-7,8},
{1,0,1,0,-4,4,-1,7,1,-9,9,1,0,-4,0,7,13,-3,-6,0,1,0,-1,-1,-2,-24,-1,9,9,1,-1,19,0,-1,-39,-2,-7,3,-6,6},
{1,-1,0,0,-5,6,-1,10,2,-10,14,2,-1,-6,1,6,9,-3,-5,-1,0,0,-6,1,-3,-23,-2,10,8,2,-2,23,1,-1,-33,-1,-5,4,-4,4},
{2,-1,-2,0,-6,8,-1,13,3,-10,19,3,-2,-8,1,5,6,-3,-5,-1,-2,0,-11,2,-3,-23,-3,12,8,3,-3,27,1,-1,-27,-1,-3,6,-3,2},
{2,-1,-4,0,-7,11,-2,18,4,-11,28,4,-4,-11,1,4,0,-3,-4,-1,-4,0,-19,5,-3,-23,-4,15,7,5,-5,34,1,-2,-16,0,1,8,0,-1},
{2,-1,-3,0,-7,7,-2,15,4,-10,26,5,-3,-10,16,4,5,-3,-4,-1,-3,0,-13,4,-2,-27,-2,13,7,5,-5,26,-1,-2,-15,0,-1,7,1,0},
{1,-1,-1,-1,-7,3,-2,11,4,-8,24,6,-1,-10,31,5,11,-3,-4,-1,-1,-1,-7,2,-2,-31,1,12,8,5,-5,18,-2,-2,-13,0,-2,5,1,0},
{1,-1,0,-1,-7,-1,-2,8,4,-7,22,7,0,-9,46,5,16,-4,-4,-1,0,-1,-1,1,-1,-36,4,10,8,5,-6,10,-4,-3,-12,-1,-4,4,2,1},
{1,0,1,-1,-7,-4,-1,4,3,-5,20,7,1,-8,60,5,21,-4,-3,0,1,-1,5,-1,0,-40,6,8,8,4,-6,2,-6,-3,-10,-1,-5,3,3,2},
{0,0,3,-2,-7,-8,-1,1,3,-4,18,8,3,-8,75,6,27,-4,-3,0,3,-2,11,-3,0,-44,9,7,9,4,-6,-6,-7,-3,-9,-1,-7,1,3,2},
{0,0,4,-2,-7,-12,-1,-3,3,-2,16,9,4,-7,90,6,32,-4,-3,0,4,-2,17,-4,1,-48,11,5,9,4,-6,-14,-9,-3,-7,-1,-8,0,4,3},
{1,0,3,-2,-7,-12,-1,-6,3,-3,13,9,3,-5,75,8,34,-6,-6,0,3,-2,20,-3,1,-42,11,4,13,3,-4,-14,-9,-2,-11,0,-11,-6,6,6},
{1,0,3,-1,-7,-12,-1,-9,3,-4,10,9,3,-3,60,10,36,-8,-9,0,3,-1,23,-2,1,-36,11,4,17,2,-1,-14,-9,-1,-15,0,-14,-12,7,10},
{2,-1,2,-1,-7,-12,-1,-13,3,-5,8,9,2,-1,45,13,38,-10,-12,-1,2,-1,26,-2,1,-30,11,3,21,1,1,-15,-9,1,-20,1,-17,-19,9,13},
{2,-1,1,0,-7,-12,-1,-16,2,-6,5,9,1,1,29,15,39,-12,-15,-1,1,0,28,-1,0,-24,11,2,25,0,3,-15,-9,2,-24,2,-20,-25,11,16},
{3,-1,1,1,-7,-12,-1,-19,2,-7,2,9,1,3,14,17,41,-14,-18,-1,1,1,31,0,0,-18,11,2,29,-1,6,-15,-9,3,-28,2,-23,-31,12,20},
{3,-1,0,1,-7,-12,-1,-22,2,-8,-1,9,0,5,-1,19,43,-16,-21,-1,0,1,34,1,0,-12,11,1,33,-2,8,-15,-9,4,-32,3,-26,-37,14,23},
{3,-1,1,1,-6,-10,-1,-18,2,-8,-1,8,1,4,-1,17,39,-14,-19,-1,1,1,30,0,0,-14,9,2,29,-2,7,-11,-8,3,-35,2,-24,-31,10,21},
{2,-1,1,1,-6,-8,-1,-14,1,-8,-1,6,1,3,-1,15,35,-12,-16,-1,1,1,26,-1,-1,-16,8,2,25,-2,6,-6,-6,3,-39,1,-21,-25,6,19},
{2,-1,2,1,-5,-6,-1,-11,1,-8,-2,5,2,3,-1,14,32,-10,-14,-1,2,1,22,-2,-1,-18,6,3,22,-2,5,-2,-5,2,-42,0,-19,-19,3,17},
{1,0,3,0,-4,-4,0,-7,1,-8,-2,3,2,2,0,12,28,-7,-12,0,3,0,17,-2,-1,-20,4,4,18,-1,3,2,-3,1,-45,-1,-17,-12,-1,14},
{1,0,3,0,-4,-2,0,-3,0,-8,-2,2,3,1,0,10,24,-5,-9,0,3,0,13,-3,-2,-22,3,4,14,-1,2,7,-2,1,-49,-2,-14,-6,-5,12},
	
};
ANIMDATA	Anim_player_walk={
	Frames_player_walk,23
};

ANIM_FRAME Frames_player_run[]={
	
{1,-21,0,0,2,0,1,40,5,-14,65,-23,6,-5,-4,15,73,23,4,0,4,0,18,0,1,-58,5,1,38,-1,2,19,0,1,-116,-2,-4,-6,-1,3},
{-1,-19,-6,0,0,3,2,44,10,-16,64,-25,14,-13,0,21,71,21,-5,0,5,0,11,0,1,-58,3,0,30,-1,2,22,0,1,-94,-2,-3,-4,-1,1},
{-2,-18,-11,1,-1,5,4,48,14,-18,64,-27,21,-19,3,26,69,19,-12,0,5,0,5,0,0,-58,2,-2,24,-1,2,24,0,1,-76,-2,-3,-3,-1,-1},
{-2,-16,-16,1,-2,8,5,51,18,-21,63,-29,28,-25,6,32,67,17,-20,0,6,0,-2,0,-1,-58,1,-3,18,0,1,26,0,1,-57,-1,-2,-2,0,-2},
{-3,-15,-21,1,-2,10,6,55,22,-23,63,-31,35,-31,9,37,65,15,-27,0,6,0,-8,0,-1,-59,-1,-4,11,0,1,29,0,1,-39,-1,-2,0,0,-4},
{-3,-12,-30,2,-4,15,8,61,30,-27,62,-34,48,-43,15,46,62,11,-41,0,7,0,-19,0,-2,-59,-3,-6,0,0,0,33,1,1,-6,0,-1,2,0,-7},
{-3,-14,-23,1,-3,9,6,55,27,-25,61,-32,46,-31,13,38,61,11,-29,0,4,0,-11,0,-3,-67,-1,-3,3,2,0,31,1,0,-16,0,0,8,2,-5},
{-2,-16,-16,0,-1,4,3,49,24,-23,61,-30,43,-20,12,29,61,11,-17,0,2,0,-3,0,-3,-74,1,-1,6,4,0,29,1,0,-26,0,2,14,3,-4},
{-2,-19,-9,-1,0,-2,1,43,21,-21,60,-28,41,-9,11,21,60,10,-4,0,-1,0,5,0,-3,-82,3,2,10,5,0,27,1,-1,-35,1,3,21,4,-3},
{-2,-21,-1,-1,2,-8,-2,37,18,-19,59,-26,38,2,10,13,59,10,8,0,-4,0,12,1,-4,-90,5,4,13,7,0,25,1,-2,-45,1,4,27,6,-2},
{-1,-23,6,-2,3,-13,-4,31,14,-17,59,-24,36,14,9,5,59,10,20,0,-6,0,20,1,-4,-98,7,7,16,9,0,23,1,-2,-55,1,5,33,7,-1},
{-1,-25,13,-3,5,-19,-7,25,11,-15,58,-22,33,25,8,-3,58,9,32,0,-9,0,28,1,-4,-105,10,9,19,11,1,21,1,-3,-65,2,6,39,9,1},
{-1,-24,17,-3,3,-20,-8,27,18,-20,64,-33,36,26,7,2,63,10,28,0,-8,0,31,1,-5,-91,8,10,17,9,1,13,1,-2,-62,1,5,33,6,0},
{-2,-22,20,-2,2,-21,-9,28,26,-26,70,-44,39,26,5,7,68,11,24,0,-8,0,33,2,-5,-78,7,10,16,7,1,6,1,-1,-59,0,4,26,3,0},
{-2,-21,24,-2,1,-22,-10,30,33,-31,77,-55,42,27,4,12,74,12,20,0,-7,0,36,2,-5,-64,6,10,14,5,2,-2,1,-1,-56,-1,3,20,0,0},
{-2,-20,28,-1,-1,-24,-12,31,40,-36,83,-66,45,27,3,18,79,13,16,0,-6,0,39,2,-5,-50,5,11,12,3,2,-10,1,0,-53,-1,2,13,-2,0},
{-3,-18,31,-1,-2,-25,-13,33,47,-42,89,-77,48,28,2,23,84,13,12,0,-6,0,41,3,-6,-36,4,11,11,1,3,-18,0,1,-50,-2,1,7,-5,0},
{-3,-17,35,0,-3,-26,-14,34,54,-47,95,-88,51,28,1,28,89,14,9,0,-5,0,44,3,-6,-22,3,11,9,-1,3,-25,0,2,-48,-3,0,0,-8,0},
{-2,-18,29,0,-2,-22,-12,35,46,-42,90,-77,43,22,0,26,86,16,8,0,-4,0,40,2,-5,-28,3,10,14,-1,3,-18,0,2,-59,-3,-1,-1,-7,0},
{-2,-18,23,0,-2,-18,-9,36,38,-36,85,-67,36,17,-1,24,84,17,7,0,-2,0,35,2,-4,-34,4,8,19,-1,3,-10,0,2,-70,-3,-1,-2,-6,1},
{-1,-19,18,0,-1,-13,-7,37,30,-30,80,-56,28,11,-2,22,81,19,6,0,-1,0,31,1,-2,-40,4,6,24,-1,3,-3,0,2,-82,-3,-2,-3,-4,1},
{0,-20,12,0,0,-9,-4,38,21,-25,75,-45,21,6,-3,19,78,20,5,0,1,0,27,1,-1,-46,4,5,28,-1,3,4,0,2,-93,-3,-3,-4,-3,2},
{0,-20,6,0,1,-5,-2,39,13,-19,70,-34,14,0,-3,17,76,22,5,0,3,0,22,0,0,-52,5,3,33,-1,3,12,0,1,-104,-2,-3,-5,-2,2},

};
ANIMDATA	Anim_player_run={
	Frames_player_run,23
};

ANIM_FRAME Frames_player_jump[]={
	
{0,0,0,-6,0,0,0,-81,-6,5,26,3,1,25,6,4,66,6,4,0,0,0,81,0,0,-106,0,0,-18,0,0,-25,0,0,-29,0,0,0,0,0},
{0,-3,0,-3,-4,0,0,-82,-11,9,24,4,0,20,4,9,63,7,1,0,0,0,87,0,0,-105,0,0,-14,0,0,-25,0,0,-39,0,0,0,0,0},
{0,-6,0,-1,-8,0,0,-84,-16,12,23,5,0,16,3,14,61,7,-2,0,0,0,92,0,0,-104,0,0,-10,0,0,-25,0,0,-47,0,0,0,0,0},
{0,-8,0,1,-11,0,0,-85,-20,15,22,6,-1,12,1,18,59,8,-5,0,0,0,98,0,0,-103,0,0,-7,0,0,-25,0,0,-55,0,0,0,0,0},
{0,-11,0,3,-15,0,0,-86,-25,18,20,7,-2,8,0,22,57,8,-8,0,0,0,103,0,0,-102,0,0,-3,0,0,-25,0,0,-63,0,0,0,0,0},
{0,-15,0,7,-21,0,0,-88,-33,23,18,9,-3,1,-3,30,53,9,-13,0,0,0,112,0,0,-100,0,0,3,0,0,-25,0,0,-77,0,0,0,0,0},
{0,-17,1,6,-20,0,0,-84,-33,18,19,9,-3,-5,8,23,57,10,-13,0,0,0,103,0,0,-91,0,0,3,0,0,-12,0,0,-80,0,0,0,0,0},
{0,-19,2,4,-18,0,0,-80,-32,13,19,8,-2,-12,19,15,61,12,-13,0,0,0,93,0,0,-82,0,0,3,0,0,0,0,0,-84,0,0,0,0,0},
{0,-22,3,3,-17,0,0,-76,-32,9,20,8,-2,-18,30,8,65,13,-13,0,0,0,84,0,0,-73,0,0,3,0,0,13,0,0,-87,0,0,0,0,0},
{0,-24,3,1,-15,0,0,-72,-31,4,20,8,-2,-24,40,0,69,14,-13,0,0,0,75,0,0,-63,0,0,3,0,0,26,0,0,-90,0,0,0,0,0},
{0,-26,4,-1,-14,0,0,-68,-31,-1,21,7,-1,-31,51,-8,73,16,-13,0,0,0,65,0,0,-54,0,0,3,0,0,38,0,0,-94,0,0,0,0,0},
{0,-28,5,-2,-12,0,0,-64,-30,-6,21,7,-1,-37,62,-15,77,17,-13,0,0,0,56,0,0,-45,0,0,3,0,0,51,0,0,-97,0,0,0,0,0},
{0,-27,3,-2,-12,0,0,-56,-22,-11,18,6,-1,-46,56,-11,66,14,-11,3,0,0,46,0,0,-41,0,0,1,0,0,51,0,0,-88,0,0,1,0,0},
{0,-25,1,-1,-12,0,0,-47,-15,-16,14,5,-1,-55,50,-7,56,11,-9,6,0,0,36,0,0,-37,0,0,-2,0,0,51,0,0,-79,0,0,2,0,0},
{0,-24,-1,-1,-13,0,0,-39,-7,-21,11,4,-1,-65,45,-3,45,9,-7,9,0,0,26,0,0,-33,0,0,-4,0,0,51,0,0,-71,0,0,3,0,0},
{0,-23,-3,-1,-13,0,0,-31,1,-26,8,2,0,-74,39,1,34,6,-4,12,0,0,16,0,0,-29,0,0,-6,0,0,51,0,0,-62,0,0,3,0,0},
{0,-21,-5,0,-13,0,0,-22,8,-31,4,1,0,-83,33,5,24,3,-2,15,0,0,6,0,0,-25,0,0,-9,0,0,51,0,0,-53,0,0,4,0,0},
{0,-20,-7,0,-13,0,0,-14,16,-36,1,0,0,-92,27,9,13,0,0,18,0,0,-4,0,0,-21,0,0,-11,0,0,51,0,0,-44,0,0,5,0,0},
{0,-17,-6,-1,-11,0,0,-25,12,-29,5,1,0,-73,24,8,22,1,1,15,0,0,10,0,0,-35,0,0,-12,0,0,38,0,0,-42,0,0,4,0,0},
{0,-13,-5,-2,-9,0,0,-36,9,-22,9,1,0,-53,20,7,31,2,1,12,0,0,24,0,0,-49,0,0,-13,0,0,26,0,0,-39,0,0,3,0,0},
{0,-10,-4,-3,-7,0,0,-48,5,-16,14,2,1,-34,17,7,40,3,2,9,0,0,39,0,0,-64,0,0,-15,0,0,13,0,0,-37,0,0,3,0,0},
{0,-7,-2,-4,-4,0,0,-59,1,-9,18,2,1,-14,13,6,48,4,3,6,0,0,53,0,0,-78,0,0,-16,0,0,0,0,0,-34,0,0,2,0,0},
{0,-3,-1,-5,-2,0,0,-70,-2,-2,22,3,1,6,10,5,57,5,3,3,0,0,67,0,0,-92,0,0,-17,0,0,-12,0,0,-32,0,0,1,0,0},

};
ANIMDATA	Anim_player_jump={
	Frames_player_jump,23
};


ANIM_FRAME Frames_player_ladder[]={

{0,-14,0,0,36,0,0,60,-2,0,67,-20,20,117,-20,-5,-1,-15,-47,39,0,0,-2,-13,-15,-76,42,9,0,0,0,2,11,15,-37,-27,-19,0,0,0},
{0,-13,0,0,34,0,0,63,0,-4,72,-25,27,108,-16,-4,8,-10,-43,41,0,0,-3,-15,-16,-71,43,12,0,0,0,0,10,14,-36,-23,-16,-4,0,0},
{0,-12,0,0,33,0,0,66,2,-7,76,-28,33,101,-14,-3,15,-6,-39,42,0,0,-5,-17,-17,-67,44,13,0,0,0,-1,9,12,-35,-20,-14,-7,0,1},
{0,-11,0,0,32,0,0,69,3,-10,80,-32,39,94,-11,-2,23,-2,-35,43,0,0,-6,-19,-18,-62,45,15,0,0,0,-3,8,11,-34,-18,-12,-11,0,1},
{0,-11,0,0,30,0,0,71,5,-13,85,-36,45,86,-9,-1,30,2,-31,45,0,0,-7,-20,-19,-58,46,17,0,0,0,-4,7,10,-33,-15,-10,-14,0,1},
{0,-9,0,0,28,0,0,76,8,-18,92,-43,55,73,-4,1,43,9,-24,47,0,0,-10,-24,-20,-50,47,20,0,0,0,-7,5,8,-32,-9,-6,-20,0,2},
{0,-10,0,0,29,0,0,84,9,-15,73,-31,52,71,-2,1,44,9,-21,47,0,0,-10,-23,-19,-49,46,19,0,0,0,-8,8,10,-40,-18,-9,-16,0,2},
{0,-10,0,0,30,0,0,91,9,-11,54,-19,49,69,0,1,46,8,-18,48,0,0,-10,-23,-18,-49,45,18,0,0,0,-9,12,13,-48,-26,-11,-11,0,2},
{0,-10,0,0,31,0,0,99,9,-8,35,-8,46,68,2,1,47,7,-16,48,0,0,-9,-23,-17,-49,43,17,0,0,0,-10,15,16,-56,-34,-13,-7,0,2},
{0,-11,0,0,32,0,0,107,9,-5,16,4,43,66,4,2,48,7,-13,48,0,0,-9,-23,-16,-48,42,16,0,0,0,-10,19,19,-64,-42,-15,-2,0,2},
{0,-11,0,0,33,0,0,114,10,-1,-3,15,40,64,6,2,50,6,-10,49,0,0,-9,-23,-15,-48,41,15,0,0,0,-11,23,22,-72,-50,-18,2,0,2},
{0,-11,0,0,34,0,0,122,10,2,-22,27,36,62,9,2,51,5,-7,49,0,0,-9,-23,-14,-48,39,14,0,0,0,-12,26,25,-80,-59,-20,7,0,2},
{-1,-11,0,0,34,0,0,113,9,2,-8,19,36,68,5,2,48,6,-14,48,0,0,-9,-22,-12,-47,37,12,0,-4,0,-12,26,24,-76,-59,-23,7,-1,2},
{-1,-11,0,0,33,0,-1,104,8,3,7,12,36,74,1,2,45,7,-21,47,0,0,-9,-21,-10,-46,34,9,0,-8,-1,-11,25,24,-72,-59,-26,8,-1,2},
{-1,-11,0,0,33,0,-1,95,7,3,21,4,35,81,-3,3,42,8,-28,47,0,0,-9,-21,-8,-45,31,6,0,-12,-1,-11,24,23,-68,-59,-29,8,-1,2},
{-1,-11,0,0,33,0,-2,85,6,4,35,-4,35,87,-7,3,39,9,-35,46,0,0,-9,-20,-6,-44,28,3,0,-17,-1,-10,24,22,-64,-59,-32,8,-2,2},
{-2,-11,0,0,32,0,-2,76,4,4,50,-11,35,93,-10,4,36,10,-42,45,0,0,-8,-19,-4,-43,26,0,1,-21,-2,-10,23,22,-60,-59,-35,8,-2,2},
{-2,-11,0,0,32,0,-3,67,3,5,64,-19,34,99,-14,4,33,11,-50,44,0,0,-8,-18,-2,-42,23,-3,1,-25,-2,-9,23,21,-56,-59,-37,9,-3,2},
{-2,-11,0,0,33,0,-2,66,2,4,65,-19,32,102,-15,2,27,7,-49,43,0,0,-7,-17,-4,-47,26,-1,1,-21,-2,-7,21,20,-53,-54,-34,7,-2,2},
{-1,-12,0,0,33,0,-2,65,2,3,65,-19,29,105,-16,1,22,2,-49,42,0,0,-6,-17,-6,-53,29,1,0,-17,-1,-5,19,19,-50,-48,-31,6,-2,1},
{-1,-12,0,0,34,0,-1,64,1,2,66,-20,27,108,-17,0,16,-2,-48,42,0,0,-5,-16,-9,-59,32,3,0,-12,-1,-3,17,18,-46,-43,-28,4,-1,1},
{-1,-13,0,0,35,0,-1,62,0,1,66,-20,25,111,-18,-2,11,-6,-48,41,0,0,-4,-15,-11,-65,35,5,0,-8,-1,-2,15,17,-43,-38,-25,3,-1,1},
{0,-13,0,0,35,0,0,61,-1,0,67,-20,22,114,-19,-3,5,-11,-48,40,0,0,-3,-14,-13,-71,38,7,0,-4,0,0,13,16,-40,-32,-22,1,0,0},

	
};
ANIMDATA	Anim_player_ladder={
	Frames_player_ladder,23
};

ANIM_FRAME Frames_player_ladder2[]={

{0,-14,0,0,36,0,0,60,-2,0,67,-20,20,117,-20,-5,-1,-15,-47,39,0,0,-2,-13,-15,-76,42,9,0,0,0,2,11,15,-37,-27,-19,0,0,0},
{0,-13,0,0,34,0,0,63,0,-4,72,-25,27,108,-16,-4,8,-10,-43,41,0,0,-3,-15,-16,-71,43,12,0,0,0,0,10,14,-36,-23,-16,-4,0,0},
{0,-12,0,0,33,0,0,66,2,-7,76,-28,33,101,-14,-3,15,-6,-39,42,0,0,-5,-17,-17,-67,44,13,0,0,0,-1,9,12,-35,-20,-14,-7,0,1},
{0,-11,0,0,32,0,0,69,3,-10,80,-32,39,94,-11,-2,23,-2,-35,43,0,0,-6,-19,-18,-62,45,15,0,0,0,-3,8,11,-34,-18,-12,-11,0,1},
{0,-11,0,0,30,0,0,71,5,-13,85,-36,45,86,-9,-1,30,2,-31,45,0,0,-7,-20,-19,-58,46,17,0,0,0,-4,7,10,-33,-15,-10,-14,0,1},
{0,-9,0,0,28,0,0,76,8,-18,92,-43,55,73,-4,1,43,9,-24,47,0,0,-10,-24,-20,-50,47,20,0,0,0,-7,5,8,-32,-9,-6,-20,0,2},
{0,-10,0,0,29,0,0,84,9,-15,73,-31,52,71,-2,1,44,9,-21,47,0,0,-10,-23,-19,-49,46,19,0,0,0,-8,8,10,-40,-18,-9,-16,0,2},
{0,-10,0,0,30,0,0,91,9,-11,54,-19,49,69,0,1,46,8,-18,48,0,0,-10,-23,-18,-49,45,18,0,0,0,-9,12,13,-48,-26,-11,-11,0,2},
{0,-10,0,0,31,0,0,99,9,-8,35,-8,46,68,2,1,47,7,-16,48,0,0,-9,-23,-17,-49,43,17,0,0,0,-10,15,16,-56,-34,-13,-7,0,2},
{0,-11,0,0,32,0,0,107,9,-5,16,4,43,66,4,2,48,7,-13,48,0,0,-9,-23,-16,-48,42,16,0,0,0,-10,19,19,-64,-42,-15,-2,0,2},
{0,-11,0,0,33,0,0,114,10,-1,-3,15,40,64,6,2,50,6,-10,49,0,0,-9,-23,-15,-48,41,15,0,0,0,-11,23,22,-72,-50,-18,2,0,2},
{0,-11,0,0,34,0,0,122,10,2,-22,27,36,62,9,2,51,5,-7,49,0,0,-9,-23,-14,-48,39,14,0,0,0,-12,26,25,-80,-59,-20,7,0,2},
{-1,-11,0,0,34,0,0,113,9,2,-8,19,36,68,5,2,48,6,-14,48,0,0,-9,-22,-12,-47,37,12,0,-4,0,-12,26,24,-76,-59,-23,7,-1,2},
{-1,-11,0,0,33,0,-1,104,8,3,7,12,36,74,1,2,45,7,-21,47,0,0,-9,-21,-10,-46,34,9,0,-8,-1,-11,25,24,-72,-59,-26,8,-1,2},
{-1,-11,0,0,33,0,-1,95,7,3,21,4,35,81,-3,3,42,8,-28,47,0,0,-9,-21,-8,-45,31,6,0,-12,-1,-11,24,23,-68,-59,-29,8,-1,2},
{-1,-11,0,0,33,0,-2,85,6,4,35,-4,35,87,-7,3,39,9,-35,46,0,0,-9,-20,-6,-44,28,3,0,-17,-1,-10,24,22,-64,-59,-32,8,-2,2},
{-2,-11,0,0,32,0,-2,76,4,4,50,-11,35,93,-10,4,36,10,-42,45,0,0,-8,-19,-4,-43,26,0,1,-21,-2,-10,23,22,-60,-59,-35,8,-2,2},
{-2,-11,0,0,32,0,-3,67,3,5,64,-19,34,99,-14,4,33,11,-50,44,0,0,-8,-18,-2,-42,23,-3,1,-25,-2,-9,23,21,-56,-59,-37,9,-3,2},
{-2,-11,0,0,33,0,-2,66,2,4,65,-19,32,102,-15,2,27,7,-49,43,0,0,-7,-17,-4,-47,26,-1,1,-21,-2,-7,21,20,-53,-54,-34,7,-2,2},
{-1,-12,0,0,33,0,-2,65,2,3,65,-19,29,105,-16,1,22,2,-49,42,0,0,-6,-17,-6,-53,29,1,0,-17,-1,-5,19,19,-50,-48,-31,6,-2,1},
{-1,-12,0,0,34,0,-1,64,1,2,66,-20,27,108,-17,0,16,-2,-48,42,0,0,-5,-16,-9,-59,32,3,0,-12,-1,-3,17,18,-46,-43,-28,4,-1,1},
{-1,-13,0,0,35,0,-1,62,0,1,66,-20,25,111,-18,-2,11,-6,-48,41,0,0,-4,-15,-11,-65,35,5,0,-8,-1,-2,15,17,-43,-38,-25,3,-1,1},
{0,-13,0,0,35,0,0,61,-1,0,67,-20,22,114,-19,-3,5,-11,-48,40,0,0,-3,-14,-13,-71,38,7,0,-4,0,0,13,16,-40,-32,-22,1,0,0},

	
};
ANIMDATA	Anim_player_ladder2={
	Frames_player_ladder2,23
};

ANIMDATA *player_anim_data[]={
&Anim_player_idle,
&Anim_player_walk,
&Anim_player_run,
&Anim_player_jump,
&Anim_player_ladder,
&Anim_player_ladder2,
	
};


ITEM ItemsPlayer[]={
	
};
ITEMDATA	ID_ItemsPlayer={
	ItemsPlayer,0
};

ITEM ItemsSaved[]={
	
};
ITEMDATA	ID_ItemsSaved={
	ItemsSaved,0
};

typedef struct	_WEAPON
{
Uint8	type;
Uint8	clip_size;
Uint8	total_bullets;
Uint8	bullets_in_clip;
Uint8	damage;
Uint8	damage_size;
	
	
}weapon;

weapon	pistol={0,8,8,8,10,1};
weapon	shotgun={1,2,6,2,40,4};
weapon	pipe={2,0,0,0,10,2};
weapon	saved_pistol={10,8,0,0,10,1};
weapon	saved_shotgun={11,2,0,0,40,4};

typedef struct  _PLAYER
{
	Uint8 anim_frame;
	weapon *current_weapon;
    bool alive;
	Sint16 health;
	Sint16 saved_health;
	bool hurt;
	Uint8 type;
	Sint16 hud_sprite;
	bool can_be_hurt;
	Sint16 hurt_timer;
	int gamepad;
	bool pressed_start;
	bool pressed_up;
	bool pressed_down;
	Uint8 dpad;
	Uint16 gems;
	Uint16 key;
	bool on_ladder_x;	
	bool on_ladder_z;	
	bool flapping;
	float jump_height;
	bool run;
	
	//Inventory
	ITEMDATA		*a_itemdata;
	ITEM	*a_item;
	   
	// Start Position
    Sint16 start_x;
    Sint16 start_y;
    Sint16 start_z;
	// Position
    Sint16 x;
    Sint16 y;
    Sint16 z;
	Sint16 nextx;
	Sint16 nexty;
	Sint16 nextz;
	Sint16 delta_x;
	float delta_y;
	Sint16 delta_z;
	Sint16 gun_x;
    Sint16 gun_y;
    Sint16 gun_z;
	bool projectile_alive;
	

    // Rotation
    Sint16 rx;
    Sint16 ry;
    Sint16 rz;
	
	//limb rotation
	Sint16		head_cam_adj;
	Sint16		y_adj;
	Sint16		bodyt_rx;
	Sint16		bodyt_ry;
	Sint16		bodyt_rz;
	Sint16		head_rx;
	Sint16		head_ry;
	Sint16		head_rz;
	Sint16		rarm_rx;
	Sint16		rarm_ry;
	Sint16		rarm_rz;
	Sint16		rarm2_rx;
	Sint16		rarm2_ry;
	Sint16		rarm2_rz;
	Sint16		larm_rx;
	Sint16		larm_ry;
	Sint16		larm_rz;
	Sint16		larm2_rx;
	Sint16		larm2_ry;
	Sint16		larm2_rz;	
	Sint16		bodyb_rx;
	Sint16		bodyb_ry;
	Sint16		bodyb_rz;
	Sint16		rleg_rx;
	Sint16		rleg_ry;
	Sint16		rleg_rz;
	Sint16		rleg2_rx;
	Sint16		rleg2_ry;
	Sint16		rleg2_rz;
	Sint16		rleg3_rx;
	Sint16		rleg3_ry;
	Sint16		rleg3_rz;
	Sint16		lleg_rx;
	Sint16		lleg_ry;
	Sint16		lleg_rz;
	Sint16		lleg2_rx;
	Sint16		lleg2_ry;
	Sint16		lleg2_rz;
	Sint16		lleg3_rx;
	Sint16		lleg3_ry;
	Sint16		lleg3_rz;
	
	Sint16 aim_rx;
	
	//shadow
	float shadow_y;
	float shadow_speed;
	float shadow_size;
	
	//strike
	Sint16 strike_timer;
	bool strike; 
	
	//kick
	Sint16 kick_timer;
	bool kick; 
	
	//special attack
	Sint16 special_attack_timer;
	bool special_attack; 
	float special_attack_size;
	
	//projectile
	Sint16 px;
	Sint16 py;
	Sint16 pz;
	Sint16 pr;
	Sint16 projectile_speed;
	Sint16 speed_px;
	Sint16 speed_py;
	Sint16 speed_pz;
	
	//enemy target
	Sint16 target;
	Sint16 tx;
	Sint16 ty;
	Sint16 tz;
	
	bool aim;
	bool shoot;
	bool action;
	
	Uint8	bullets_in_clip;
	Uint8 total_bullets;
	Sint16 recoil;
	
	Sint16 anim_speed;
	
	// Size (Hitboxes)
    Uint16 xsize;
    Uint16 ysize;
    Uint16 zsize;
	
	Sint16 speed;
	Sint16 speed_x;
	float speed_y;
	Sint16 speed_z;
	
	Uint16 current_map_section;
	Uint16 current_collision;
	bool can_jump;
	
	//ramp
	float R_int_height;
	float int_height;
	Sint16 ramp_height_adj;
	float X_int_adj;
	float Z_int_adj;
	
	//blood cloud
	bool blood;
	Sint16 blood_counter;
	float blood_size;
	Sint16 blood_x;
	Sint16 blood_y;
	Sint16 blood_z;
	



}               player_params;

extern player_params player;


typedef struct	_ENEMY
{
	Uint16 id;
	bool alive;
	bool alert;
	Sint16 health;
	Sint16 start_health;
	bool hurt;
	Uint8 type;
	int hud_sprite;
	bool can_be_hurt;
	Sint16 hurt_timer;
	Sint16 anim_speed;

    // Start Position
    Sint16 start_x;
    Sint16 start_y;
    Sint16 start_z;
	
	// Position
	Sint16 x;
    Sint16 y;
    Sint16 z;
	Sint16 nextx;
    Sint16 nexty;
    Sint16 nextz;
	Sint16 max_speed;
	Sint16 speed_x;
	float speed_y;
	Sint16 speed_z;
	Sint16 xdist;
	Sint16 zdist;
	bool flip_direction;
	Sint16 waypoint;
	bool on_ladder_x;	
	bool on_ladder_z;	
	// Size
    Uint16 xsize;
    Uint16 ysize;
    Uint16 zsize;
	
	

    // Rotation
    Sint16 rx;
    float ry;
    Sint16 rz;
	
	//head Rotation
	Sint16 head_rx;
	Sint16 head_ry;
	Sint16 head_rz;
	
	//body Rotation
	Sint16 body_rx;
	Sint16 body_ry;
	Sint16 body_rz;
	
	//arm Rotation
	Sint16 larm_rx;
	Sint16 larm_ry;
	Sint16 larm_rz;
	Sint16 rarm_rx;
	Sint16 rarm_ry;
	Sint16 rarm_rz;
	
	//leg Rotation
	Sint16 lleg_rx;
	Sint16 lleg_ry;
	Sint16 lleg_rz;
	Sint16 rleg_rx;
	Sint16 rleg_ry;
	Sint16 rleg_rz;
	
	//target
	Sint16 target;
	Sint16 tx;
	Sint16 ty;
	Sint16 tz;
	float tr;
	float trx;
	
	//melee attack
	Sint16 attack_timer;
	bool attack; 
	
	
	//projectile is active
	bool projectile_alive;
	Sint16 shoot_counter;
	Sint16 shoot_wait;
	bool can_shoot;
	
	//projectile position
	Sint16	px;
	Sint16 py;
	Sint16	pz;
	
	float speed_px;
	float speed_py;
	float speed_pz;
	
	//projectile target
	Sint16 ptx;
	Sint16 pty;
	Sint16 ptz;
	
	//projectile direction
	float pr;
	
	
	//ramp
	float R_int_height;
	float int_height;
	Sint16 ramp_height_adj;
	Uint16 current_map_section;
	bool can_jump;
	
	//death
	Sint16 explosion_size;

	
}				enemy;
extern enemy 	enemies[];

typedef struct	_POWERUP
{
	Uint16			id;
	Uint8			type;
	int				x;
	Sint16			y;
	Sint16			z;
	Sint16			ry;
	Uint8			state;
	bool			used;
	XPDATA	*pup_model;

	
}				powerup;
extern powerup	powerups[];



typedef struct  _GAME
{
    // game state variables
    Uint8         game_state;
    Uint8         pause_menu;
	 Uint8         title_screen_menu;
    bool        pressed_start;
	bool        pressed_left;
	bool        pressed_right;
    bool        pressed_up;
    bool        pressed_down;
	bool        pressed_X;
	Uint8		players;
	Sint16		map_section;
	Sint16		light_x;
	Sint16		light_y;
	Sint16		light_z;
	float		gravity;
	float		max_gravity;
	Uint8		shading_type;
	Uint8		level;
	Sint16		message_timer;
	bool		start_timer;
	float		title_size;
	Sint16		title_screen_timer;
	Sint16		frame_counter;
	Uint16		player_dead_timer;
	

}               game_params;
extern game_params game;

typedef struct	_PUZZLE
{
    Uint8               id;
    Uint8				type;
	Uint8                solved;
	
    
}                       puzzle;

extern puzzle puzzles[];

typedef struct	_DOOR
{
    Uint8               level;
    Uint16                key_id;
	Uint8				area;
	Sint16				new_x;
	Sint16				new_y;
	Sint16				new_z;
	Sint16				new_ry;
    
}                       door;

extern door doors[];

typedef struct	_MAP
{
    Sint16		min_x;
	Sint16		max_x;
	Sint16		map_min_x;
	Sint16		map_max_x;
	Sint16		min_y;
	Sint16		max_y;
	Sint16		map_min_y;
	Sint16		map_max_y;
    
}                       map_params;

extern map_params map_plan;

#endif /* !__HAMSTER_H__ */

/*
** END OF FILE
*/
