/* 
 * rocks.c - Space Rocks/VR
 * Copyright (c) 1996-1997 Common Sense Engineering 
 *
 * by Tom Payne
 */

#include "wt.h"
#include "string.h"
#include "rocks.h"
#include "globals.h"
#include "math.h"

char *name_buf = "SpaceRocks/VR v0.903";

FLAG WSmaster = TRUE;
int rock_sn = 0;

#ifdef WSERVER

#include "wsclient.h"
#include "client.h"
#include "client.p"
void sharecb(void *object, WTtype objtype, int property, int code);



void sharecb(void *object, WTtype objtype, int property, int code)
{
	switch(code)
	{
	case WTCLIENT_PROPSHARED:
		WTmessage ("Object property successfully shared!\n");
		break;
	case WTCLIENT_PROPSHAREDEXISTING:
		WTmessage ("Object property successfully shared (again)!\n");
		WSmaster = FALSE;
		break;
	case WTCLIENT_PROPSHAREFAILED:
		WTmessage ("Error sharing object property!\n");
		break;
	default:
		WTmessage ("Unknown status code returned during sharing\n");
	}
}

#endif

/************************************************************************
*
*	MAIN
*
************************************************************************/

int main(int argc, char *argv[])
{
	WTui *shell;
	int width, height;
	WTwindow *win;
    
	WTmessage("SpaceRocks/VR\n");
	WTmessage("Copyright 1996-1997 Common Sense Engineering\n\n");

	/* read command line arguments */
	scan_args( argc, argv);

	/* initialize universe */
	WTmessage ("Creating new universe\n");
 
#ifdef WSERVER
	/* open connection on my machine */
	{
		char pszHost[128];

		WTmessage("Initializing WorldServer Client subsystem\n");
		gethostname(pszHost, 128);
		lstrcat(pszHost, ":8888");
		if(!WTclient_initialize(pszHost)) {
			WTmessage("ERROR: Failed to initialize client.\n");
			return 0;
		} else WTmessage("...successful\n");
	}
#endif

	if (use_gui)
	{
		toplevel = WTui_init(&argc,argv);
		WTuniverse_new( WTDISPLAY_NOWINDOW, WTWINDOW_DEFAULT);

		/* set up a window with a GUI */ 
		if (texture_flag)
		{
			width  = 300;
			height = 300;
		}
		else
		{
			width  = 300;
			height = 300;
		}


		shell =  WTui_newform(toplevel, name_buf,
                          WTUIATT_LEFT, 0, WTUIATT_TOP, 0,
                          WTUIATT_WIDTH, width, WTUIATT_HEIGHT, height, NULL);

		build_menubar(shell);

		WTui_newwtkwindow(shell,WTWINDOW_NOBORDER);
	}
	else
	{
		WTuniverse_new( display_mode, window_mode);
	}


	win = WTuniverse_getwindows();

//	WTwindow_loadimage(win, "about.jpg", 1.0f, TRUE, TRUE);

	if (texture_flag == TRUE)
		texture_select(NULL,NULL);
	else
		shade_select(NULL,NULL);

	/* get the root node for later reference */
	root = WTuniverse_getrootnodes();

	/* Load lights from "lights" */
	WTlightnode_load( root, "lights"); 

	create_materials();

	shipfilename = (char *)malloc (80);
	sprintf(shipfilename, "ship2.nff");

	{
		/* cache the alien spaceship for later use */
		char *alienfilename = "station.nff";
		cached_alien = WTmovnode_load(NULL, alienfilename, 1.0f);
	}

	sprintf(filename, "gravel");

	/* create scene graph */
	create_scenegraph();

    initial_pq.p[X] = 0.0f;
	initial_pq.p[Y] = 0.0f;
	initial_pq.p[Z] = -UNIVERSE_SIZE * 1.5f;
	WTq_init(initial_pq.q);

	/* get the universe's viewpoint */
	outview = WTuniverse_getviewpoints();

	/* store initial viewpoint */
	WTviewpoint_setposition( outview, initial_pq.p);
	WTviewpoint_setorientation( outview, initial_pq.q);

	inview = WTviewpoint_copy(outview);

	WTviewpoint_setparallax(inview, 0.05f * UNIVERSE_SIZE);
	WTviewpoint_setparallax(outview, 0.05f * UNIVERSE_SIZE);

	/* start inside the ship */
	toggleview(NULL, NULL);

	/* display special key functions */
	display_keys();

	/* prepare to read keyboard */
	WTkeyboard_open();

	/* set up any sensors which may be attached to the ship */
	setup_sensors(&sensor);

	/* set universe action function */
	WTuniverse_setactions(actions);

	/* set up 2D/3D overlay draw routines */

	while (win != NULL)
	{
		WTwindow_setfgactions(win, overlay_2Dfunction);
		WTwindow_setdrawfn   (win, overlay_3Dfunction);

		WTwindow_setyonvalue(win, 5000.0f);
		WTwindow_sethithervalue(win, 0.01f);

		WTwindow_setbgrgb(win, 0,0,0);

		win = WTwindow_next(win);
	}

	/* Setup sounds */

	setup_sounds(default_sound_device);	
	WTsound_play(myStartSound);

	/* Set Perspective correction ON */
	WTuniverse_setrendering((FLAG)(WTuniverse_getrendering() ^ WTRENDER_PERSPECTIVE));

	/* Prepare alien timer */

	alien_insertion_timer = 100.0f + (float) fabs(myrand(200.0f));

	/* prepare 3D Font */

	my3DFont = WTfont3d_load("gameover.nff");

	/* set default y-blank for V-Split stereo */

	WTscreen_setyblank(42);

	/* ready! */

	WTmessage("Universe ready\n");
	WTuniverse_ready();

	/* enter main loop */
	if (use_gui)
	{
		WTui_manage(shell);
		WTui_go(toplevel,1);
	}
	else
	{
		WTuniverse_go();
	}

	return 0;
}

void rock_task(WTnode *thisnode)
{
	RockData *thisdata;
	WTp3 pos, velocity;
	WTp3 angvel;
	float speed;
	FLAG update;

	if (!paused)
	{
		thisdata = (RockData *) WTnode_getdata(thisnode);
		WTnode_gettranslation(thisnode, pos);
		update = FALSE;
		if (pos[X] >  UNIVERSE_SIZE) 
		{
			thisdata->dir[X] = -thisdata->dir[X];
			update = TRUE;
		}
		if (pos[X] < -UNIVERSE_SIZE) 
		{
			thisdata->dir[X] = -thisdata->dir[X];
			update = TRUE;
		}
		if (pos[Y] >  UNIVERSE_SIZE) 
		{
			thisdata->dir[Y] = -thisdata->dir[Y];
			update = TRUE;
		}
		if (pos[Y] < -UNIVERSE_SIZE) 
		{
			thisdata->dir[Y] = -thisdata->dir[Y];
			update = TRUE;
		}
		if (pos[Z] >  UNIVERSE_SIZE) 
		{
			thisdata->dir[Z] = -thisdata->dir[Z];
			update = TRUE;
		}
		if (pos[Z] < -UNIVERSE_SIZE) 
		{
			thisdata->dir[Z] = -thisdata->dir[Z];
			update = TRUE;
		}

		/* even though the slave apps should be able to 
		   detect collision with the edge of the known universe,
		   use this opportunity to reset/synch up the rocks */

		if ((net_master == 1) && (update == TRUE))
			MyNetSetInfo(ROCKINFO, thisnode);

		
		if ((thisdata->colide != NULL) &&
			(WTnode_numpolys(thisdata->colide) != 0)) /* test if valid */	
		{
			WTp3 pos2, out;
			WTnode_gettranslation(thisdata->colide, pos2);
			WTp3_subtract(pos, pos2, out);
			WTp3_norm(out);
			WTp3_copy(out, thisdata->dir);
		}

		WTp3_copy(thisdata->dir, velocity);

		speed = 4.0f / thisdata->mass;

		WTp3_mults(velocity, speed * deltaT() * sens);

		if (MoveRocks && WSmaster) 
		{
			WTnode_translate(thisnode, velocity, WTFRAME_PARENT);
			if (!ThreeDVersion) 
			{
				WTnode_gettranslation(thisnode, pos);
				pos[Z] = 0.0f;
				WTnode_settranslation(thisnode, pos);
			}
			WTp3_copy(thisdata->ang, angvel);
			WTp3_mults(angvel, deltaT() * sens);
			WTnode_rotate(thisnode, angvel[X],angvel[Y],angvel[Z], WTFRAME_LOCAL);

		}
	}

}

WTnode *newrock(WTnode *parent, WTp3 pos, WTp3 dir, WTp3 ang, float size)
{
	RockData *myData;
	WTnode *baseRock;
	WTnode *typeRock;
	WTgeometry *geom;
	WTpoly *poly;
	int x;
	static int id = 0;

	baseRock = WTmovswitchnode_new(parent);

	{
		char buf[20];
		sprintf(buf,"rock%d",id);
		WTnode_setname(baseRock, buf);
	}


	for (x=0;x<4;x++)
	{
		switch (x) 
		{	
			case ROCKTYPE_BOX:
				geom = WTgeometry_newblock( size, size, size, TRUE);
				break;
			case ROCKTYPE_SIMPLE:
				geom = WTgeometry_newsphere( size/2.0f, 2, 4, FALSE, TRUE);
				break;
			case ROCKTYPE_SPHERE:
				geom = WTgeometry_newsphere( size/2.0f, 4, 8, FALSE, TRUE);
				break;
			case ROCKTYPE_DIST:
				geom = WTgeometry_newsphere( size/2.0f, 4, 8, FALSE, TRUE);
				/* triangulate the sphere */
				
				poly = WTgeometry_getpolys(geom);
				while (poly != NULL)
				{
					if (WTpoly_numvertices(poly) == 4 )
					{
						WTpoly *newpoly, *oldpoly;
						WTpoly *nextpoly;

						nextpoly = WTpoly_next(poly);

						newpoly = WTgeometry_beginpoly(geom);
						WTpoly_addvertexptr(newpoly, WTpoly_getvertex(poly, 0));
						WTpoly_addvertexptr(newpoly, WTpoly_getvertex(poly, 1));
						WTpoly_addvertexptr(newpoly, WTpoly_getvertex(poly, 2));
						WTpoly_close(newpoly);

						newpoly = WTgeometry_beginpoly(geom);
						WTpoly_addvertexptr(newpoly, WTpoly_getvertex(poly, 0));
						WTpoly_addvertexptr(newpoly, WTpoly_getvertex(poly, 2));
						WTpoly_addvertexptr(newpoly, WTpoly_getvertex(poly, 3));
						WTpoly_close(newpoly);

						oldpoly = poly;
						WTpoly_delete(oldpoly);
						poly = nextpoly;
					}
					else 
					{
						poly = WTpoly_next(poly);
					}
				}

				
				/* distort the vertices */

				{
					WTvertex *vert;

					vert = WTgeometry_getvertices(geom);
					WTgeometry_beginedit(geom);
					while (vert != NULL)
					{
						WTp3 pos;
						float dist;

						WTgeometry_getvertexposition(geom, vert, pos);
						dist = 1.0f + myrand( 0.2f);
						WTp3_mults(pos, dist);
						WTgeometry_setvertexposition(geom, vert, pos);
						vert = WTvertex_next(vert);
					}
					WTgeometry_endedit(geom);
					WTgeometry_recomputestats(geom, TRUE);
				}


				break;
		
		}
		WTgeometry_setrgb( geom, 128, 128, 128);
		WTgeometry_settexture(geom, filename, TRUE, FALSE);
		typeRock = WTmovgeometrynode_new( baseRock, geom);
		{
			/* Give the rock type a name (for debugging) */
			char buf[20];
			sprintf(buf,"type%d",x);
			WTnode_setname(typeRock, buf);
		}
	}

	WTnode_settranslation(baseRock,pos);
	WTswitchnode_setwhichchild(baseRock, RockType);

    myData = malloc(sizeof(RockData));
	myData->dir[X] = dir[X];
	myData->dir[Y] = dir[Y];
	myData->dir[Z] = dir[Z];

	myData->ang[X] = ang[X];
	myData->ang[Y] = ang[Y];
	myData->ang[Z] = ang[Z];

	WTp3_norm(myData->dir);

	myData->mass = size;
	WTnode_setdata(baseRock, (void *) myData);
	WTtask_new(baseRock, rock_task, 1.0f);

	myData->colide = NULL;
	myData->nodepath = WTnodepath_new(baseRock, root, 0);
	myData->id = id++; /* need some unique id ...using serial number */

#ifdef WSERVER
	{
		/* Give the rock a name (for WServer) */
		char buf[20];
		sprintf(buf,"rock%d",rock_sn++);
		WTnode_setname(baseRock, buf);
	}

	WTclient_shareproperty(baseRock, WTNODE_TRANSFORM, sharecb);
#endif

	return baseRock;
}

void scalenode(WTnode *node,float scale)
{
    WTgeometry *geom;
    WTnode     *child;
    int         i,num;

    if(node==NULL) return;
    geom = WTnode_getgeometry(node);
    if(geom) {
        WTgeometry_scale(geom,scale,zeroPos);
    }
    num = WTnode_numchildren(node);
    for(i=0;i<num;i++) {
        child = WTnode_getchild(node,i);
        scalenode(child,scale);
    }
    num = WTmovnode_numattachments(node);
    for(i=0;i<num;i++) {
        child = WTmovnode_getattachment(node,i);
        scalenode(child,scale);
    }
}


WTnode *makenewship(WTnode *parent, WTp3 pos)
{
	WTnode *newShip; 
	
	/* Create a spacecraft */

	newShip = WTmovnode_load(parent, shipfilename, 0.1f);


	/* scale ship to "game friendly" size */
	{
		WTp3 msp;
		float shiprad;
		WTnode_getmidpoint(newShip, msp);
		shiprad = WTnode_getradius(newShip);
		scalenode(newShip, 8.0f/shiprad);
		shiprad = WTnode_getradius(newShip);
	}

	WTnode_settranslation(newShip, pos);

	return newShip;
}


void makeship(WTp3 pos)
{
	WTm3 myRot;
	RockData *data;

	myShip = makenewship(root, pos);
	myShipPath = WTnodepath_new(myShip, root, 0);
	WTeuler_2m3(3.1415f/2.0f, 0.0f, 0.0f, myRot);
	WTnode_setrotation(myShip, myRot);

    /* Add shields */

	myShipShieldGeom = WTgeometry_newsphere( INITSHIELDRADIUS, 4, 8, FALSE, TRUE);

	/* actually, the GeoRadius (bounding box) differs from the sphere radius */

	ShieldRadius = WTgeometry_getradius(myShipShieldGeom);
	InitShieldRadius = WTgeometry_getradius(myShipShieldGeom);

    WTgeometry_setmtable(myShipShieldGeom, shieldTable);
	WTgeometry_setmatid (myShipShieldGeom, shieldID);

	myShipShield = WTmovgeometrynode_new(NULL, myShipShieldGeom);
	WTmovnode_attach(myShip, myShipShield, 0);
	myShipShieldPath = WTnodepath_new(myShipShield, root, 0);

	shipVelocity = 0.0f;

	data = (RockData *) malloc(sizeof(RockData));
	data->id = -99;
	WTnode_setdata(myShip, (void *)data);


	if (net_master != -1) MyNetSetInfo(NEWSHIP, myShip);
}

void new_ship(void)
{
	WTp3 pos;
	pos[X] = (float) myrand(UNIVERSE_SIZE / 2.0f);
	pos[Y] = (float) myrand(UNIVERSE_SIZE / 2.0f);
	if (ThreeDVersion) 
		pos[Z] = (float) myrand(UNIVERSE_SIZE / 2.0f);
	else
		pos[Z] = 0.0f;
	makeship(pos);

	/* Countdown GameLives */
	GameLives -= 1;

	if (GameLives == 0) /* the game is OVER! */
	{
		gameover(NULL,NULL);
	}
}

void boom_task(WTnode *thisnode)
{
	BoomData *myData;
	float Opacity[1];

	if (!paused)
	{
		myData = (BoomData *)WTnode_getdata(thisnode);
		WTgeometry_scale(myData->geom, 1.0f + (0.5f * deltaT() * sens), zeroPos);
		myData->size = WTnode_getradius(thisnode);
		Opacity[0] = 1.0f - (myData->size / 20.0f);
		WTmtable_setvalue (myData->mtable, myData->mtableid, Opacity, WTMAT_OPACITY);
		if (myData->size > 20.0f)
		{
			if (myData->makeship)
			{
				new_ship();
				if (!cockpitView) MYmessage("Press 'v' to restore viewpoint");
			}
			WTnode_delete(thisnode);
			thisnode = NULL;
		}
	}
}

void makeexplosion(WTp3 pos, FLAG shipflag)
{
	BoomData *myData;
    WTnode *boomNode;

	MakeShock = 1;
	
	if ((WTuniverse_getrendering() & WTRENDER_TEXTURED))
	{
		make_animated_explosion(pos, shipflag);
	}
	else
	{

		myData = malloc(sizeof(BoomData));

		myData->makeship = shipflag;
		myData->size = 1.0f;
		myData->geom = WTgeometry_newsphere( myData->size/2.0f, 4, 8, FALSE, TRUE);
		WTgeometry_settexture(myData->geom, "screen2.tga", TRUE, TRUE);

		myData->mtable      = WTmtable_new(WTMAT_AMBIENTDIFFUSE | WTMAT_OPACITY, 0, NULL);
		myData->mtableid    = WTmtable_newentry(myData->mtable);

		WTmtable_setvalue (myData->mtable, myData->mtableid, boomOpacity, WTMAT_OPACITY);
		WTmtable_setvalue (myData->mtable, myData->mtableid, boomColors, WTMAT_AMBIENTDIFFUSE);
    
		WTgeometry_setmtable(myData->geom, myData->mtable);
		WTgeometry_setmatid (myData->geom, myData->mtableid);
		boomNode = WTmovgeometrynode_new( root, myData->geom);
		WTnode_settranslation(boomNode,pos);

		WTnode_setdata(boomNode, (void *) myData);
		WTtask_new(boomNode, boom_task, 1.0f);
	}

	/* make boom sound */
	WTsound_setposition(BoingSound, pos);
	WTsound_stop(BoingSound);
	WTsound_play(BoingSound);

}

void makerocks(int numrocks)
{
	int x;
	WTmessage("Creating %d Space Rocks\n",numrocks);
	for (x=0; x<numrocks;x++)
	{
		WTp3 pos, dir, ang;

		pos[X] = (float) myrand(UNIVERSE_SIZE);
		pos[Y] = (float) myrand(UNIVERSE_SIZE);
		pos[Z] = (float) myrand(UNIVERSE_SIZE);
		
		while ( sqrt( pos[X]*pos[X] +
			      pos[Y]*pos[Y] +
		                              pos[Z]*pos[Z] ) < 10.0f)
		{
			pos[X] = (float) myrand(UNIVERSE_SIZE);
			pos[Y] = (float) myrand(UNIVERSE_SIZE);
			pos[Z] = (float) myrand(UNIVERSE_SIZE);
		}
		dir[X] = (float) myrand(5.0f);
		dir[Y] = (float) myrand(5.0f);
		dir[Z] = (float) myrand(5.0f);
		WTp3_norm(dir);

		ang[X] = (float) myrand(5.0f);
		ang[Y] = (float) myrand(5.0f);
		ang[Z] = (float) myrand(5.0f);

		newrock(RockRoot, pos, dir, ang, 15.0f);
	}

}

void alien_task(WTnode *thisnode)
{
	RockData *thisdata;
	WTp3 velocity;
	WTp3 angvel;

	if (!paused)
	{
		thisdata = (RockData *) WTnode_getdata(thisnode);
		WTnode_gettranslation(thisnode, alien_pos);

		WTp3_copy(thisdata->dir, velocity);
		WTp3_mults(velocity, deltaT() * sens * 4.0f);

		WTnode_translate(thisnode, velocity, WTFRAME_PARENT);
		if (!ThreeDVersion) 
		{
			WTnode_gettranslation(thisnode, alien_pos);
			alien_pos[Z] = 0.0f;
			WTnode_settranslation(thisnode, alien_pos);
		}
		WTp3_copy(thisdata->ang, angvel);
		WTp3_mults(angvel, deltaT() * sens);
		WTnode_rotate(thisnode, angvel[X],angvel[Y],angvel[Z], WTFRAME_LOCAL);

		/*  randomly fire missiles */

		if ((thisdata->timer < 0.0f) && (net_master != 0))
		{
			/* fire a missile */
			WTq m_ori;
			WTeuler_2q(myrand(M_PI),myrand(M_PI),myrand(M_PI),m_ori);
			fire_missile(alien_pos, m_ori, IFF_ALIEN);
			/* reset timer */
			thisdata->timer = (float) fabs(myrand(10.0f));
		}
		else
		{
			/* decrement timer */
			thisdata->timer -= (deltaT() * sens);
		}

		WTsound_setposition(myAlienSound, alien_pos);

		if ( WTp3_mag(alien_pos) > UNIVERSE_SIZE * 1.1f )
		{
			WTsound_stop(myAlienSound);
			WTsound_setnodepath(myAlienSound, NULL);
//			WTsound_stop(thisdata->sound);
//			WTsound_delete(thisdata->sound);
			WTnode_delete(thisnode);
			thisnode = NULL;
			draw_alien_line = FALSE;
		}
	}
}

void make_alien(void)
{
	WTp3 dir, ang;
	WTnode *alien;
	float ang1, ang2, r;
	RockData *myData;

//	alien = WTmovnode_load(AlienRoot, alienfilename, 1.0f);
	alien = WTmovnode_instance(AlienRoot, cached_alien);

	/* position alien at "edge of known universe" */

	r = UNIVERSE_SIZE;
	ang1 = myrand (180.0f);
	ang2 = myrand (180.0f);

	alien_pos[X] = r * (float)sin(ang1) * (float)cos(ang2);
	alien_pos[Y] = r * (float)sin(ang1) * (float)sin(ang2);
	alien_pos[Z] = r * (float)cos(ang1);

	WTnode_settranslation(alien, alien_pos);

	/* select an "out" position (store in dir[], temporarily) */

	r = UNIVERSE_SIZE / 2.0f;
	ang1 = myrand (180.0f);
	ang2 = myrand (180.0f);

	dir[X] = r * (float)sin(ang1) * (float)cos(ang2);
	dir[Y] = r * (float)sin(ang1) * (float)sin(ang2);
	dir[Z] = r * (float)cos(ang1);

	/* subtract "in" from "out" to get a direction */

	WTp3_subtract(dir, alien_pos, dir);
	
	WTp3_norm(dir);

    /* establish a random orientation */

	ang[X] = (float) myrand(5.0f);
	ang[Y] = (float) myrand(5.0f);
	ang[Z] = (float) myrand(5.0f);

	/* scale alien ship to "game friendly" size */
	{
		WTp3 msp;
		float shiprad;
		WTnode_getmidpoint(alien, msp);
		shiprad = WTnode_getradius(alien);
		scalenode(alien, 8.0f/shiprad);
	}


	/* assign action routine to alien ship */

	WTtask_new(alien, alien_task, 1.0f);

	/* associate data with alien ship */

    myData = malloc(sizeof(RockData));
	WTnode_setdata(alien, (void *) myData);

	myData->dir[X] = dir[X];
	myData->dir[Y] = dir[Y];
	myData->dir[Z] = dir[Z];

	myData->timer = (float) fabs(myrand(10.0f));

	myData->ang[X] = 0.0f;
	myData->ang[Y] = 0.0f;
	myData->ang[Z] = 0.0f;

	myData->nodepath = WTnodepath_new(alien, root, 0);

//	myData->sound =	WTsound_load(mySoundDevice, "alien2.wav");	
//	myData->sound =	myAlienSound;

//	WTsound_setparam(myData->sound,  WTSOUND_LOOPS, -1.0f);
//	WTsound_setparam(myData->sound,  WTSOUND_SPATIALIZE, 1.0f);

//	WTsound_setnodepath(myData->sound, myData->nodepath);
//	WTsound_play(myData->sound);

	WTsound_setparam(myAlienSound,  WTSOUND_LOOPS, -1.0f);
	WTsound_setparam(myAlienSound,  WTSOUND_SPATIALIZE, 1.0f);

//	WTsound_setnodepath(myAlienSound, myData->nodepath);
	WTsound_play(myAlienSound);

	draw_alien_line = FALSE;
	if (show_alien == TRUE) draw_alien_line = TRUE;

	if (net_master == 1) MyNetSetInfo(NEWALIEN, alien);
}

WTnode *add_background(WTnode *rootnode)
{
	WTnode *node;

	node = WTmovnode_load(rootnode, "universe.nff", 1.0f);
	scalenode(node, 100.0f);
	return node;

}

void create_scenegraph( void)
{

	makeship(zeroPos);

	starfield = add_background(root);

	/* Countdown GameLives */
	GameLives -= 1;

	/* Create Asteroid Node */

	RockRoot    = WTgroupnode_new(root);	
	ShipRoot    = WTgroupnode_new(root);	
	MissileRoot = WTgroupnode_new(root);	
	AlienRoot   = WTgroupnode_new(root);	

	load_animated_bits();
}

void blowship (WTnode *ship)
{
	WTp3 pos;
	WTnode_gettranslation(ship, pos);
	if (cockpitView) toggleview(NULL, NULL);
	if (net_master == 1)
	{
		MyNetSetInfo(DELSHIP, ship);
	}
	WTnode_delete(ship);
	ship = NULL;
	myShip = NULL;
	myShipPath = NULL;
	myShipShieldPath = NULL;
	makeexplosion(pos, TRUE);
	WTmessage("BOOM\n");
}


void overlay_2Dfunction(WTwindow *win, FLAG eye)
{
	WTp2 pos[4], posuv[4];

	

	if (logo && (win != rear_window))
	{
		pos  [0][X] = 0.0f;
		pos  [0][Y] = 0.0f;
		posuv[0][X] = 0.0f;
		posuv[0][Y] = 0.0f;

		pos  [1][X] = 0.0f;
		pos  [1][Y] = 0.1f;
		posuv[1][X] = 0.0f;
		posuv[1][Y] = 1.0f;

		pos  [2][X] = 0.2f;
		pos  [2][Y] = 0.1f;
		posuv[2][X] = 1.0f;
		posuv[2][Y] = 1.0f;

		pos  [3][X] = 0.2f;
		pos  [3][Y] = 0.0f;
		posuv[3][X] = 1.0f;
		posuv[3][Y] = 0.0f;

		WTwindow_draw2Dtexture(win, "logo3r", TRUE, pos, posuv);
	}

	if (((cockpitView) && (win != rear_window)) || 
		((!cockpitView) && (win == rear_window)))
	{
		float green;
		float xx;
		green = (255.0f * shieldOpacity[0]);
		WTwindow_set2Dcolor(win, 0, (char)green, 0);
		WTwindow_set2Dlinewidth(win, 3.0f);

		xx = 0.2f - (0.2f * ShieldRadius / InitShieldRadius);

		WTwindow_draw2Dline(win, 0.0f, 0.2f, 0.4f - xx, 0.4f - xx);
		WTwindow_draw2Dline(win, 0.2f, 0.0f, 0.4f - xx, 0.4f - xx);

		WTwindow_draw2Dline(win, 0.0f, 0.8f, 0.4f - xx, 0.6f + xx);
		WTwindow_draw2Dline(win, 0.2f, 1.0f, 0.4f - xx, 0.6f + xx);

		WTwindow_draw2Dline(win, 0.8f, 1.0f, 0.6f + xx, 0.6f + xx);
		WTwindow_draw2Dline(win, 1.0f, 0.8f, 0.6f + xx, 0.6f + xx);

		WTwindow_draw2Dline(win, 1.0f, 0.2f, 0.6f + xx, 0.4f - xx);
		WTwindow_draw2Dline(win, 0.8f, 0.0f, 0.6f + xx, 0.4f - xx);

		WTwindow_set2Dlinestyle(win, 0x3333);
		WTwindow_set2Dlinewidth(win, 1.0f);
		WTwindow_draw2Dline(win, 0.3f, 0.5f, 0.7f, 0.5f);
		WTwindow_draw2Dline(win, 0.5f, 0.7f, 0.5f, 0.3f);
	}

	if ((screen_message_time > 0.0f) && (win != rear_window))
	{
		float w, h;
		screen_message_time -= (sens * deltaT() ) ;
		WTwindow_get2Dtextextents(win, screen_message, &w, &h);
		WTwindow_set2Dcolor(win, 0, 255, 0);
		WTwindow_draw2Dtext(win, (.5f - w/2.0f), .4f, screen_message);
	}

	if ((displayText) && (win != rear_window))
	{
		static char buf[40];
		sprintf(buf, "Score: %d", GameScore);
		WTwindow_set2Dcolor(win, 0, 255, 0);
		WTwindow_draw2Dtext(win, 0.6f,0.9f,buf);

		sprintf(buf, "Lives: %d", GameLives);
		WTwindow_draw2Dtext(win, 0.1f,0.9f,buf);

	}

	if ((draw_stats) && (win != rear_window))
	{
		static char buf[40];
		sprintf(buf, "%3.1f fps", WTuniverse_framerate());
		WTwindow_set2Dcolor(win, 0, 255, 0);
		WTwindow_draw2Dtext(win, 0.1f,0.1f,buf);
		
	}


}

void overlay_3Dfunction(WTwindow *win, FLAG eye)
{
	FLAG find_home = FALSE;
	WTp3 pos[2];
	
	if (myShip != NULL) 
	{
		WTnode_gettranslation(myShip, pos[1]);
		if ((pos[1][X] >  UNIVERSE_SIZE) |
			(pos[1][X] < -UNIVERSE_SIZE) |
			(pos[1][Y] >  UNIVERSE_SIZE) |
			(pos[1][Y] < -UNIVERSE_SIZE) |
			(pos[1][Z] >  UNIVERSE_SIZE) |
			(pos[1][Z] < -UNIVERSE_SIZE) ) find_home = TRUE;
	}

	if (find_home &&
	    ((( cockpitView) && (win != rear_window)) ||
		 ((!cockpitView) && (win == rear_window))))
	{
		WTq ori;
		WTp3 dir;

		/* draw line from origin to ship's nose */

		WTp3_init(pos[0]);
		WTnode_gettranslation(myShip, pos[1]);
		WTnode_getorientation(myShip, ori);
		WTq_2dir(ori,dir);
		WTp3_norm(dir);
		WTp3_mults(dir, 4.0f);
		WTp3_add(pos[1], dir, pos[1]);
		WTwindow_set3Dcolor(win, 255, 0, 0);
		WTwindow_draw3Dlines(win, pos, 2, WTLINE_SEGMENTS);
	}

	if (draw_alien_line &&
	    ((( cockpitView) && (win != rear_window)) ||
		 ((!cockpitView) && (win == rear_window))))
	{
		WTq ori;
		WTp3 dir;

		/* draw line from origin to ship's nose */

		WTp3_init(pos[0]);
		WTp3_copy(alien_pos, pos[0]);

		WTnode_gettranslation(myShip, pos[1]);
		WTnode_getorientation(myShip, ori);
		WTq_2dir(ori,dir);
		WTp3_norm(dir);
		WTp3_mults(dir, 4.0f);
		WTp3_add(pos[1], dir, pos[1]);
		WTwindow_set3Dcolor(win, 0, 0, 255);
		WTwindow_draw3Dlines(win, pos, 2, WTLINE_SEGMENTS);
	}


	if (draw_stars)
	{
		static WTp3 pts[NUM_STARS];
		if (stars_first_time)
		{
			int x;

			for (x=0; x<NUM_STARS; x++)
			{
				float r,ang1,ang2;

				r = myrand(UNIVERSE_SIZE);
				if (r < 0) 
					r -= UNIVERSE_SIZE;
				else
					r += UNIVERSE_SIZE;

				ang1 = myrand (180.0f);
				ang2 = myrand (180.0f);

				pts[x][X] = r * (float)sin(ang1) * (float)cos(ang2);
				pts[x][Y] = r * (float)sin(ang1) * (float)sin(ang2);
				pts[x][Z] = r * (float)cos(ang1);

			}

			stars_first_time=FALSE;
		}

		WTwindow_set3Dcolor(win, 255, 255, 255);
		WTwindow_set3Dpointsize(win,1.0f);
		WTwindow_draw3Dpoints(win, pts, NUM_STARS);
		
	}

}

/*
 **********************
 *	UNIVERSE ACTIONS  *
 **********************
 */
void actions()
{
	int key, x, y, z;
	static WTp3 currVel = { 0.0f, 0.0f, 0.0f};
	WTp3 pos;
	WTq ori;

	key = WTkeyboard_getkey();
	if( key ) handle_key( key);

	if (!paused)
	{
		if (myShip != NULL)
		{
			currVel[Z] = shipVelocity * deltaT() * sens;
			WTnode_translate(myShip, currVel, WTFRAME_LOCAL);

       		if (sensor) /* if (sensor) */
    		{
				WTsensor_getrotation(sensor, ori);
				WTnode_rotateq(myShip, ori, WTFRAME_LOCAL);
			}

			WTnode_gettranslation(myShip, pos);
			WTnode_getorientation(myShip, ori);
			if (net_master != -1) MyNetSetInfo(SHIPINFO, myShip);

			/* center universe around ship */
			WTnode_settranslation(starfield, pos);

			/* simulate concussion wave effects */

			MakeShock = 0; /* disable ShockWave, for now */

			if (MakeShock > 0)
			{
				static float t;
				WTp3 ShockVector;
				float ShockForce    =  6.0f;
				float ShockDuration =  3.0f;
				float ShockDelay    =  0.5f;
				float w             = 20.0f;
				float p             =  1.0f;

				if (MakeShock == 1)
				{
					t = 0.0f;
					MakeShock = 2;
				}

				if ((t > ShockDelay) && (MakeShock == 2))
					MakeShock = 3;
				if (t > ShockDuration) 
					MakeShock = 0;

				if (MakeShock == 3)
				{
					float xx;
					WTp3 noise;
					
					noise[X] = 0.1f;
					noise[Y] = 0.1f;
					noise[Z] = 0.1f;

					xx = ShockForce * (float)exp ( -1* p * t) * (float)cos(w*t);
					WTq_2dir(ori, ShockVector);
					WTp3_add(ShockVector, noise, ShockVector);
					WTp3_mults(ShockVector, xx);
					WTp3_add(pos, ShockVector, pos);
				}

				t = t + deltaT();

			}

			WTviewpoint_setposition(inview, pos);
			WTviewpoint_setorientation(inview, ori);

		}

 		/* check collisions */

		z = WTnode_numchildren(RockRoot);

		if ((z == 0) && (net_master != 0))/* first time thru...no rocks yet */
		{
			char buf[256];
			/* create some new rocks */
			GameLevel++;
			sprintf(buf,"Welcome to Level %d", GameLevel);
			MYmessage(buf);
			z = NUM_ASTEROIDS * GameLevel;
			makerocks(z);
		}

		for (x=0; x<z; x++) /* loop thru all the rocks */
		{
			RockData *rockdata;
			WTnodepath *myRockPathX;
			int w, xx;

			rockdata = WTnode_getdata(WTnode_getchild(RockRoot, x));

			myRockPathX = rockdata->nodepath;

			w = WTnode_numchildren(MissileRoot);
			
			for (xx=0; xx<w; xx++) /* check for missile collisions */
			{
				RockData *missiledata;
				missile = WTnode_getchild(MissileRoot, xx);
				missiledata = WTnode_getdata(missile);
				missilePath = missiledata->nodepath;

				if (WTnodepath_intersectbbox(myRockPathX, missilePath))
				{
					missiledata->colide = WTnode_getchild(RockRoot, x);
				}
			}

			rockdata->colide = NULL;

			/* Bounce Rocks off of shields */

			if ((myShip!=NULL) && (WTnodepath_intersectbbox(myRockPathX, myShipShieldPath)))
			{
				ShieldRadius = WTgeometry_getradius(myShipShieldGeom);
				if ( ShieldRadius > rockdata->mass)
				{
					MYmessage ("Shields holding...");
					rockdata->colide = myShipShield;
					WTgeometry_scale(myShipShieldGeom, 0.9f, zeroPos);
					WTsound_play(ShieldSound);
					flicker_time = 0.2f;
				}
				else
				{
					MYmessage ("Shield FAILURE!");
					WTmessage ("Shield FAILURE!\n");
					blowship(myShip); 
				}
			}

			/* bounce rocks off each other */

			if (detect_rock_col)
			{
				for (y=0; y<z; y++)
				{	
					if (x != y)
					{
						if (simple_check)  /* use simple colision checking */
						{
							float dist;
							WTnode *rockX, *rockY;
							WTp3 posX, posY;
							RockData *rockdataY;

							rockX = WTnode_getchild(RockRoot, x);
							WTnode_gettranslation(rockX, posX);
							rockY = WTnode_getchild(RockRoot, y);
							WTnode_gettranslation(rockY, posY);
							dist  = WTp3_distance(posX, posY);
							rockdataY = WTnode_getdata(rockY);	

							if (dist < ((rockdata->mass / 2.0f) + (rockdataY->mass / 2.0f)))  
								rockdata->colide = WTnode_getchild(RockRoot, y);

						}
						else       /* use WTK colision detection */
						{
							WTnodepath *myRockPathY;
							RockData *rockdataY;
							rockdataY = WTnode_getdata(WTnode_getchild(RockRoot, y));	

							myRockPathY = rockdataY->nodepath;
							if (WTnodepath_intersectbbox(myRockPathX, myRockPathY))
							{
								rockdata->colide = WTnode_getchild(RockRoot, y);
							}


						}

					}
		
				}
			}

			if ((myShip != NULL) & (flicker_time > 0.0f))
			{
				shieldOpacity[0] = SHIELDOPACITY + flicker_time;
				WTmtable_setvalue (shieldTable, shieldID, shieldOpacity, WTMAT_OPACITY);
				flicker_time += (-0.01f * deltaT() * sens);
			}

		}


		if (sensor && (WTsensor_getmiscdata(sensor) & WTMOUSE_LEFTBUTTON)) fire_missile0();
		if (sensor && (WTsensor_getmiscdata(sensor) & WTMOUSE_RIGHTBUTTON)) shipVelocity += shipVelocityDelta;
		if (sensor && (WTsensor_getmiscdata(sensor) & WTMOUSE_MIDDLEBUTTON)) shipVelocity = 0.0f;

		if ((!ThreeDVersion) && myShip && (WTnode_numpolys(myShip) > 0))
		{
			WTm3 m;
			WTnode_getrotation(myShip, m);
			m[1][1] = 0.0f;
			WTnode_setrotation(myShip, m);
		}
   
		/*  randomly insert alien */

		if ((net_master < 1) && (use_network == TRUE))
		{
			/* do nothing */
		}
		else
		{
			if (alien_insertion_timer < 0.0f)
			{
				/* only make one alien at a time */
				if (WTnode_numchildren(AlienRoot) == 0) make_alien();
				/* reset timer */
				alien_insertion_timer = (float) fabs(myrand(200.0f));
			}
			else
			{
				/* decrement timer */
				alien_insertion_timer -= (deltaT() * sens);
			}

		}	

		/* update the sound system (for spatialization information) */
	}
	if (use_network) MyNetActions();
	WTsounddevice_update(mySoundDevice);
}

void explode_alien(WTnode *thisAlien)
{
	WTp3 pos;
	RockData *thisAlienData;

	thisAlienData = (RockData *)WTnode_getdata(thisAlien);
	WTnode_gettranslation(thisAlien,pos);
	makeexplosion(pos, FALSE);
	GameScore += 500;

	if (net_master != -1) MyNetSetInfo(DELALIEN, thisAlien);

	draw_alien_line = FALSE;
	WTsound_setnodepath(myAlienSound, NULL);
	WTsound_stop(myAlienSound);
	WTnode_delete(thisAlien);
    thisAlien=NULL;
}


void missile_task(WTnode *thisnode)
{
	RockData *thisdata;
	int delete_me = 0;
	WTp3 velocity, angvel;
	int y,z;

	thisdata = (RockData *) WTnode_getdata(thisnode);

	if (!paused)
	{

		WTp3_copy(thisdata->dir, velocity);
		WTp3_copy(thisdata->ang, angvel);

		WTp3_mults(velocity, deltaT() * sens);
		WTp3_mults(angvel,   deltaT() * sens);


		WTnode_translate(thisnode, velocity, WTFRAME_PARENT);
		WTnode_rotate(thisnode, angvel[X], angvel[Y], angvel[Z], WTFRAME_LOCAL);

		if (thisdata->colide != NULL)	/* we colided with something */
		{
			RockData *moredata;
			WTp3 missile_pos, rock_pos;
			float rockmass;
			int x;

			/* Add to GameScore */

			GameScore += 100;

			WTnode_gettranslation(thisnode, missile_pos);
			WTnode_gettranslation(thisdata->colide, rock_pos);

			makeexplosion(rock_pos, FALSE);

			/* check size - replace if big enough */
			moredata = (RockData *)WTnode_getdata(thisdata->colide);
			rockmass = moredata->mass;

			/* remove the object (rock) that the missile collided with */		
			WTnode_delete(thisdata->colide);

			thisdata->colide = NULL;

			/* prepare the missile to be removed, too */
			delete_me = 1;
			
			if (rockmass > 4.0f)
			{
				for (x=0;x<2;x++)
				{
					WTp3 pos, dir, ang;

					pos[X] = missile_pos[X] + (float) myrand(rockmass/2.0f);
					pos[Y] = missile_pos[Y] + (float) myrand(rockmass/2.0f);
					pos[Z] = missile_pos[Z] + (float) myrand(rockmass/2.0f);

					dir[X] = (float) myrand(5.0f);
					dir[Y] = (float) myrand(5.0f);
					dir[Z] = (float) myrand(5.0f);
					WTp3_norm(dir);

					ang[X] = (float) myrand(5.0f);
					ang[Y] = (float) myrand(5.0f);
					ang[Z] = (float) myrand(5.0f);

					newrock(RockRoot, pos, dir, ang, rockmass/2.0f);
		
				}
			}
		}
		
		/* did this missile intersect with an Alien Ship? */

		if (thisdata->iff != IFF_ALIEN)
		{
			z = WTnode_numchildren(AlienRoot);
			if (z != 0)
			{
				for (y=0; y<z; y++)
				{
					RockData *thisAlienData;
					WTnode *thisAlien;
					thisAlien = WTnode_getchild(AlienRoot, y);
					thisAlienData = (RockData *)WTnode_getdata(thisAlien);

					if (WTnodepath_intersectbbox(thisdata->nodepath, thisAlienData->nodepath))
					{
						/* collision */
						explode_alien(thisAlien);
	
					}
				}
			}
		}

		/* did this missile intersect with myShip? */

		if ((thisdata->iff != IFF_OWNSHIP) && myShip && myShipPath)
		{
			if (WTnodepath_intersectbbox(thisdata->nodepath, myShipPath))
			{
				/* collision */
				blowship(myShip);
				WTmessage( "Bad missile hit me!\n" ) ;
			}
		}

		/* use "mass" as missile time out */
		thisdata->mass -= 1.0f * deltaT() * sens;
		if (thisdata->mass < 0 ) delete_me = 1;

		if (delete_me == 1) WTnode_delete(thisnode);
		thisnode = NULL;
		
	}

}


WTnode *fire_missile( WTp3 pos, WTq ori, int iff)
{
	RockData *myData;
	WTnode *node;
	WTgeometry *geom;

	if (missilefilename == NULL)
	{
		//geom = WTgeometry_newblock( 2.0f, 2.0f, 2.0f, TRUE);
		geom = WTgeometry_newsphere( 1.0f, 2, 4, FALSE, TRUE);

		switch (iff)
		{
			case IFF_OWNSHIP:
				//WTgeometry_setrgb( geom, 0, 255, 255);
			    WTgeometry_setmtable(geom, shieldTable);
				WTgeometry_setmatid (geom, missilematID);

				break;
			case IFF_ALIEN:
				//WTgeometry_setrgb( geom, 255, 0, 0);
			    WTgeometry_setmtable(geom, shieldTable);
				WTgeometry_setmatid (geom, missile2matID);
				break;
		}
		node = WTmovgeometrynode_new( MissileRoot, geom);
	}
	else
	{
		float rad;
		node = WTmovnode_load(MissileRoot, missilefilename, 1.0f);
		rad = WTnode_getradius(node);
		scalenode(node, 2.0f/rad);
	}

	WTnode_settranslation(node, pos);

    myData = malloc(sizeof(RockData));

	WTq_2dir(ori, myData->dir);
	WTp3_mults(myData->dir, 10.0f);

	myData->ang[X] = 60.0f;
	myData->ang[Y] = 60.0f;
	myData->ang[Z] = 60.0f;


	myData->colide = 0;
	myData->mass = 20.0f; /* misile life span */

	myData->nodepath = WTnodepath_new(node, MissileRoot, 0);

	myData->iff = iff;

	WTnode_setdata(node, (void *) myData);
	WTtask_new(node, missile_task, 1.0f);

	if (net_master != -1) MyNetSetInfo(NEWMISSILE, node);

	return node;
}


void create_materials(void)
{
	shieldTable = WTmtable_new(WTMAT_AMBIENTDIFFUSE | WTMAT_OPACITY, 0, NULL);
	shieldID    = WTmtable_newentry(shieldTable);

	WTmtable_setvalue (shieldTable, shieldID, shieldOpacity, WTMAT_OPACITY);
	WTmtable_setvalue (shieldTable, shieldID, shieldColors, WTMAT_AMBIENTDIFFUSE);

	missilematID    = WTmtable_newentry(shieldTable);

	WTmtable_setvalue (shieldTable, missilematID, missileOpacity, WTMAT_OPACITY);
	WTmtable_setvalue (shieldTable, missilematID, missileColors, WTMAT_AMBIENTDIFFUSE);

	missile2matID    = WTmtable_newentry(shieldTable);

	WTmtable_setvalue (shieldTable, missile2matID, missile2Opacity, WTMAT_OPACITY);
	WTmtable_setvalue (shieldTable, missile2matID, missile2Colors, WTMAT_AMBIENTDIFFUSE);

	holderGeom1 = WTgeometry_newsphere( 1.0f, 2, 4, FALSE, TRUE);
    WTgeometry_setmtable(holderGeom1, shieldTable);
	WTgeometry_setmatid (holderGeom1, shieldID);
}
