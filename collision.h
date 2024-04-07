#ifndef __Collision_H__
# define __Collision_H__

///COLLISION V4///
#define MAX_LEVEL		(4)
/*
***ATTRIBUTES***
0 = solid wall/floor
1 = ramp
2 = ramp
3 = ramp
4 = ramp
5 = 
6 = diag wall
7 = diag wall
8 = ladder
9 = kill player
10 - 19 = door
*/
typedef struct {
	
    Sint16	 cen_x;	//x pos of coll cube		
	Sint16  cen_y;			
	Sint16  cen_z;	
	Sint16	 x_size; //x size of coll cube
	Sint16	 y_size;
	Sint16	 z_size;	
	Uint8  att;	//attribute		
	//int	 gru;	//group (model)	
} COLLISON;

typedef struct {
	COLLISON	*cotbl;		/* ポリゴン定義テーブル  collision definition table*/
	Uint32		 nbCo;		/* ポリゴンの数 number of coll cubes*/
}CDATA;


typedef struct section_data{
   
	Uint8 type;
		
    // Position
    Sint16 x;
    Sint16 y;
    Sint16 z;
	
	//position adj (moving platforms etc)
	Sint16 tx;
	Sint16 ty;
	Sint16 tz;

    // Rotation
    //int rx;
    //int ry;
    //int rz;
	
	//model
	XPDATA	*map_model;
	
	//collisions
	CDATA		*a_cdata;
	COLLISON	*a_collison;
	
	//align with map
	Sint8 rotation;
	Sint16 min_x;
	Sint16 min_y;
	Sint16 max_x;
	Sint16 max_y;
	Sint16 map_min_x;
	Sint16 map_min_y;
	Sint16 map_max_x;
	Sint16 map_max_y;
	   
   
}level_section;

level_section		map_section[];

///collision cubes: {x, y, z, sizex, sizey, sizez, type}
/// types: 0:normal wall/floor 1:Zramp, 2:Xramp, 3:Zramp, 4: Xramp, 5 ladder,  10+: door (10 door 0, 11 door 1 etc - corresponds to map section number)

char *level5[]={
	
"STREET.TGA",
"STREET.BIN"

};

char *level4[]={
	
"M_CHUR.TGA",
"CH_MOFF.BIN",
"CH_MIN.BIN",
"CH_MTOW.BIN"

};

char *level3[]={
	
"M_APT.TGA",
"APT_MB.BIN",
"APT_MA.BIN"

};

char *level2[]={
	
"CHURCH.TGA",
"CH_FRONT.BIN",
"CH_TEST2.BIN",
"CH_TOWER.BIN",
"CH_ROOF.BIN",
"CH_YARD.BIN",
"CH_OFF.BIN"

};

char *level1[]={
	
"APT.TGA",
"APT_A.BIN",
"APT_B.BIN",

};


char *level0[]={
	
"STREET.TGA",
"TITLE.BIN",

};



#endif /* !__Collision_H__ */
