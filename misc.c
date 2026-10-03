#include "wt.h"
#include "rocks.h"



void togglerearview(WTui *ui, void *data)
{
	if (rear_window == NULL)
	{
		rear_window = WTwindow_new(300,100,100,100,WTWINDOW_DEFAULT);
		WTwindow_setfgactions(rear_window, overlay_2Dfunction);
		WTwindow_setdrawfn   (rear_window, overlay_3Dfunction);
		WTwindow_setyonvalue(rear_window, 5000.0f);
		WTwindow_sethithervalue(rear_window, 0.01f);
		WTwindow_setbgrgb(rear_window, 0,0,0);
		if (cockpitView)
			WTwindow_setviewpoint(rear_window, outview);
		else
			WTwindow_setviewpoint(rear_window, inview);

	}
	else
	{
		WTwindow_delete(rear_window);
		rear_window = NULL;
	}
}



void togglecheck(WTui *ui, void *data)
{
	if (detect_rock_col)
	{
		simple_check = !simple_check;
		if (simple_check)
			MYmessage("Simple colision detection");
		else
			MYmessage("Complex colision detection");
	}
	else
		MYmessage("No change");

}


void toggleview(WTui *ui, void *data)
{
	WTwindow *win;

	cockpitView  = !cockpitView;

	win = WTuniverse_getwindows();

	while (win != NULL)
	{
		if (cockpitView)
		{
			WTwindow_setviewpoint(win, inview);
			WTwindow_setviewpoint(rear_window, outview);
		}
		else
		{
			WTwindow_setviewpoint(win, outview);
			WTwindow_setviewpoint(rear_window, inview);
		}

		win = WTwindow_next(win);
	}

}

float myrand(float scale)
{

	return (1.0f - 2*(float)rand()/(float)RAND_MAX) * scale;
}

float deltaT(void)
{
	float dt;
	dt = 1.0f / WTuniverse_avgframerate(5);
	return dt;
}

void MYmessage(char *message)
{
	strcpy(screen_message, message);
	screen_message_time = 5.0f;
}

void fire_missile0(void)
{
	if ((myShip != NULL) && (!paused))
	{
		/* only alllow a few missiles to be fired at a time */
		int w, xx, count;
		w = WTnode_numchildren(MissileRoot);
		count = 0;
		for (xx=0; xx<w; xx++)
		{
			RockData *missiledata;
			missile = WTnode_getchild(MissileRoot, xx);
			missiledata = WTnode_getdata(missile);
			if (missiledata->iff == IFF_OWNSHIP)
				count++;
		}

		if ( count > 4)
		{
			MYmessage("Recharging...");
		}
		else
		{
			WTp3 pos;
			WTq ori;
			RockData *missiledata;

			WTnode_gettranslation(myShip, pos);
			WTnode_getorientation(myShip, ori);
			missile = fire_missile(pos, ori, IFF_OWNSHIP);
			missiledata = WTnode_getdata(missile);

			missiledata->nodepath = WTnodepath_new(missile, root, 0);

//			ShockForce = ShockForceMax;
			WTsound_play(FireSound);
		}
	}
}

void print_performance(WTui *ui, void *data)
{
	int x, y, z;
	WTmessage("Performance Information --------\n");
	WTmessage("--------------------------------\n");
	WTmessage("Objects\n");
	WTmessage("    in scenegraph : %d\n",0);
	WTmessage("    in scene      : %d\n",0);
	WTmessage("Polygons\n"); 
	WTmessage("    in scenegraph : %d\n",WTnode_numpolys(root));
	WTmessage("    in window     : %d\n",WTwindow_numpolys(WTuniverse_getwindows()));
	WTmessage("Framerate\n");
	WTmessage("    instantaneous : %f\n", WTuniverse_avgframerate(1));
	WTmessage("    average       : %f\n", WTuniverse_framerate());
	WTmessage("Window\n");
	WTwindow_getposition(WTuniverse_getwindows(), &z, &z, &x, &y);
	WTmessage("    Size          : %d x %d\n", x, y);
	WTmessage("Memory usage\n");
	WTmessage("    Texture       : %d\n", WTtexture_getmemory());
	WTmessage("Rendering Options\n");
	WTmessage("    Gouraud       : ");
	if (WTuniverse_getrendering() & WTRENDER_GOURAUD)
		WTmessage("TRUE\n");
	else
		WTmessage("FALSE\n");
	WTmessage("    Textured      : ");
	if (WTuniverse_getrendering() & WTRENDER_TEXTURED)
		WTmessage("TRUE\n");
	else
		WTmessage("FALSE\n");
	WTmessage("    Perspective   : ");
	if (WTuniverse_getrendering() & WTRENDER_PERSPECTIVE)
		WTmessage("TRUE\n");
	else
		WTmessage("FALSE\n");
	WTmessage("    Anti-aliasing : ");
	if (WTuniverse_getrendering() & WTRENDER_ANTIALIAS)
		WTmessage("TRUE\n");
	else
		WTmessage("FALSE\n");
	WTmessage("    BEST          : ");
	if (WTuniverse_getrendering() & WTRENDER_BEST)
		WTmessage("TRUE\n");
	else
		WTmessage("FALSE\n");

	WTmessage("\n");
}


void gameovertask(WTnode *thisnode)
{

	WTp3 pos;
	WTp3 delta = {0.0f, 0.0f, -100.0f};

	WTnode_gettranslation(thisnode, pos);
	WTp3_mults(delta, deltaT() * sens);
	if (pos[Z] > -1000.0) WTnode_translate(thisnode, delta, WTFRAME_PARENT);
	WTnode_rotate(thisnode, 20.0f * deltaT() * sens, 0.0f, 0.0f, WTFRAME_LOCAL);

}

void toggle_network(WTui *ui, void *data)
{

	use_network = ! use_network;
	if (use_network)
	{
		WTmessage("Attempting to join network\n");
	}
	else
	{
		WTmessage("Exiting network\n");
		net_master = -1;
	}
}

void gameover(WTui *ui, void *data)
{

	WTgeometry *overtext[10];
	WTnode *tempnode;
	int x;
	WTp3 pos = { 0.0f, 0.0f, 1000.0f};

	WTmessage("Game Over\n");
	WTsound_play(GameoverSound);

	paused = TRUE;

	cockpitView = TRUE;
	toggleview(NULL,NULL);

	overtext[0] = WTgeometry_newtext3d(my3DFont, "G");
	overtext[1] = WTgeometry_newtext3d(my3DFont, "a");
	overtext[2] = WTgeometry_newtext3d(my3DFont, "m");
	overtext[3] = WTgeometry_newtext3d(my3DFont, "e");

	overtext[6] = WTgeometry_newtext3d(my3DFont, "O");
	overtext[7] = WTgeometry_newtext3d(my3DFont, "v");
	overtext[8] = WTgeometry_newtext3d(my3DFont, "e");
	overtext[9] = WTgeometry_newtext3d(my3DFont, "r");

	overnode = WTmovsepnode_new(root);

    for (x = 0; x < 10; x++)
	{
		WTp3 offpos;
		WTp3_copy (zeroPos, offpos);
		if ((x != 4) & (x !=5) )
		{
			offpos[X] = (float) x * WTfont3d_getspacing(my3DFont);
			WTgeometry_setrgb( overtext[x], 255, 0, 0);	
			tempnode = WTmovgeometrynode_new(overnode, overtext[x]);
			WTnode_settranslation(tempnode, offpos);
			WTtask_new(tempnode, gameovertask, 1.0f);
		}
	}

	pos[X] = - (WTfont3d_getspacing(my3DFont) * 9.0f) / 2.0f;
	WTnode_settranslation(overnode, pos);

}

/*
 * Handle a keypress.
 */
void handle_key(int key)
{
	/* interpret keypress */
	switch ( key ) {
		case 'q':
			test_exit(NULL, NULL);	
			break;

		case 'Q':
			exit(0);	
			break;

		case WTKEY_UPARROW:
            if (ThreeDVersion) WTmovnode_rotateaxis(myShip, X, pitch_delta); 
			break;

		case WTKEY_DOWNARROW:
            if (ThreeDVersion) WTmovnode_rotateaxis(myShip, X, -pitch_delta); 
			break;

		case WTKEY_LEFTARROW:
			WTmovnode_rotateaxis(myShip, Y,-yaw_delta); 
			break;

		case WTKEY_RIGHTARROW:
            WTmovnode_rotateaxis(myShip, Y, yaw_delta); 
			break;

		case '+':
			WTsensor_setsensitivity(sensor,	WTsensor_getsensitivity(sensor) * 1.1f);
			break;

		case '-':
			WTsensor_setsensitivity(sensor,	WTsensor_getsensitivity(sensor) * 0.9f);
			break;

		case '<':
			{
				float parallax;
				parallax = WTviewpoint_getparallax(inview) * 1.1f;
				WTviewpoint_setparallax(inview, parallax);
				WTviewpoint_setparallax(outview, parallax);
				WTmessage("Parallax: %f\n", parallax);
			}
			break;
	
		case '>':
			{
				float parallax;
				parallax = WTviewpoint_getparallax(inview) * 0.9f;
				WTviewpoint_setparallax(inview, parallax);
				WTviewpoint_setparallax(outview, parallax);
				WTmessage("Parallax: %f\n", parallax);
			}
			break;

		case 'T':
			toggle_rock_motion(NULL, NULL);
			break;

		case '3':
			togglemode(NULL, NULL);
			break;

		case 'p':
			togglepause(NULL, NULL);
			break;

		case 'P':
			print_performance(NULL, NULL);
			break;

		case 'N':
			toggle_network(NULL, NULL);
			break;

		case 'G':
			WTnode_print(root);
			break;

		case '0':
			gameover(NULL, NULL);
			break;

		case 'w':
			wireframe_select(NULL,NULL);
			break;

		case 's':
			shade_select(NULL,NULL);
			break;

		case 'X':
			{
				WTp3 pos;
				WTp3_init(pos);
				makeexplosion(pos, FALSE);
			}
			break;

		case 't':
			texture_select(NULL,NULL);
			break;

	    case 'r':
			reset0(NULL,NULL);
			break;

	    case 'S':
			draw_stars = TRUE;
			define_stars(NULL,NULL);
			break;

		case 'u':
			display_usage();
			break;

		case 'e':
			pitch_delta *= 2.0f;
			yaw_delta   *= 2.0f;
			WTmessage ("Pitch delta = %f\n", pitch_delta);
			WTmessage ("  Yaw delta = %f\n", yaw_delta);
			break;

		case 'E':
			pitch_delta /= 2.0f;
			yaw_delta   /= 2.0f;
			WTmessage ("Pitch delta = %f\n", pitch_delta);
			WTmessage ("  Yaw delta = %f\n", yaw_delta);
			break;
				
		case ' ':
			fire_missile0();
			break;

		case 'v':
			toggleview(NULL, NULL);
			break;

		case 'R':
			togglerearview(NULL, NULL);
			break;

		case 'z':
			zoomall(NULL, NULL);
			break;

		case 'n':
			zoomship(NULL, NULL);
			break;

		case 'C':
			togglecheck(NULL, NULL);
			break;

		case '?':
			break;

		case 'f':
			shipVelocity += shipVelocityDelta;
			break;

		case 'b':
			shipVelocity = 0.0f;
			break;

		case 'd':
			displayText = !displayText;
			break;

		case 'A':
			make_alien();
			break;

		case '1':
			MakeShock = 1;
			break;

		default:
			display_keys();
			WTmessage("\nEnter command..\n");
	}
}


void display_keys(void)
{

	WTmessage("\n");
	WTmessage("'r'   reset viewpoint\n");
	WTmessage("'p'   pause (toggle)\n");
	WTmessage("'w'   switch to wireframe\n");
	WTmessage("'s'   switch to shaded\n");
	WTmessage("'t'   switch to textured\n");
	WTmessage("'3'   toggle 2D/3D motion\n");
	WTmessage("'f'   forward (thrust)\n");
	WTmessage("'u'   display usage\n");
	WTmessage("'E'  increase rotational velocities \n");
	WTmessage("'e'   decrease rotational velocities \n");
	WTmessage("'v'   viewpoint toggle\n");
	WTmessage("'+/-' adjust sensor sensitivity\n");
	if (!use_gui) WTmessage("'</>' adjust parallax (stereo viewing)\n");
	WTmessage("'b'   space brakes\n");
	WTmessage("'d'   toggle display (score)\n");
	WTmessage("'R'   toggle aux viewpoint\n");
	WTmessage("'?' this display\n");
	WTmessage("'q' Quit.\n");
	WTmessage(" ARROW key - pitch/roll\n");
	WTmessage(" SPACE - fire missile\n");
	WTmessage("----MOUSE----\n");
	WTmessage("'left'   fire missile\n");
	WTmessage("'middle' space brake\n");
	WTmessage("'right'  accelerate\n");
	WTmessage("----DEBUG----\n");
	WTmessage("'z' zoom all\n");
	WTmessage("'n' zoom to ship\n");
	WTmessage("'A' make an alien ship\n");
	WTmessage("'N' join network\n");
	WTmessage("'C' toggle simple/complex rock colision detection\n");
	WTmessage("'T' stop motion of Space Rocks\n");
	WTmessage("'S' redefine 3D star positions/turn stars on\n");
	WTmessage("'G' print scene graph\n");
	WTmessage("'X' make an explosion\n");
	WTmessage("\n");
}

void printhelp(WTui *ui, void *data)
{
	char buf[2048];

	WTui *shell, *pb1;
	int width = 300;
	int height = 300;

	shell =  WTui_newform(toplevel, "SpaceRocks Help",
                          WTUIATT_LEFT, 0, WTUIATT_TOP, 0,
                          WTUIATT_WIDTH, width, WTUIATT_HEIGHT, height, NULL);

	strcpy(buf, "Instructions:\r\n");

	strcat(buf, "\r\n");
	strcat(buf, "'r' reset viewpoint\r\n");
	strcat(buf, "'w' switch to wireframe\r\n");
	strcat(buf, "'s' switch to shaded\r\n");
	strcat(buf, "'t' switch to textured\r\n");
	strcat(buf, "'T' stop motion of Space Rocks\r\n");
	strcat(buf, "'f' forward (thrust)\r\n");
	strcat(buf, "'3' toggle 2D/3D mode\r\n");
	strcat(buf, "'u' display usage\r\n");
	strcat(buf, "'G' increase rotational velocities\r\n");
	strcat(buf, "'g' decrease rotational velocities\r\n");
	strcat(buf, "'v' viewpoint toggle\r\n");
	strcat(buf, "'z' zoom all\r\n");
	strcat(buf, "'b' space brakes\r\n");
	strcat(buf, "'?' this display\r\n");
	strcat(buf, "'q' Quit.\r\n");
	strcat(buf, "'Q' Quit. (not graceful)\r\n");
	strcat(buf, " ARROW key - pitch/roll\r\n");
	strcat(buf, " SPACE - fire missile\r\n");
	strcat(buf, "\n");

	WTui_newscrolledtext(shell, buf, FALSE,
		WTUIATT_LEFT, 6, 
		WTUIATT_TOP, 5,
		WTUIATT_WIDTH, width - 23,
		WTUIATT_HEIGHT, height - 100, NULL);

	pb1 = WTui_newpushbutton(shell, "OK",
		WTUIATT_LEFT, (width/2) - 40, 
		WTUIATT_TOP, height - 100 + 20,
		WTUIATT_WIDTH, 80,
		WTUIATT_HEIGHT, 50,
		NULL);

	WTui_setcallback(pb1,   WTUIEVENT_ACTIVATE, closeui, (void *)shell);
//	WTui_setcallback(shell, WTUIEVENT_DONE, closeui, (void *)shell);
//	WTui_setcallback(shell, WTUIEVENT_OK, closeui, (void *)shell);
//	WTui_setcallback(shell, WTUIEVENT_CLOSE, closeui, (void *)shell);
	
	WTui_manage(shell);
}



void display_usage(void)
{
	WTmessage( "Usage: rocks [options] \n" );
	WTmessage( "Options:\n" );
	WTmessage( "-c				use CrystalEyes Stereoview\n" );
	WTmessage( "-d				use CrystalEyes Stereoview (force VSplit)\n" );
	WTmessage( "-S				use two window stereo\n" );
	WTmessage( "-W				use stereo in window (SGI only)\n" );
	WTmessage( "-R				use Red/Blue stereo\n" );
	WTmessage( "-B				borderless windows\n" );
	WTmessage( "-M				Sound: use WinMM\n" );
	WTmessage( "-K				Sound: use DiamondWare\n" );
	WTmessage( "-G				Sound: use SGI\n" );
	WTmessage( "-V				Sound: use VSI\n" );
	WTmessage( "-C				Sound: use CRE\n" );
	WTmessage( "-D				Sound: use DirectSound\n" );
	WTmessage( "-X				Disable texture on startup\n" );
	WTmessage( "-Z				Debug mode\n" );
	WTmessage( "-a(1,2)[1..n]	for Ascension Bird.  For a flock, specify which unit.\n" );
	WTmessage( "-b(1,2)			for Fake Space Boom on port 1,2\n" );
	WTmessage( "-e(1,2)			for Virtual i-O i-glasses! head tracker on port 1,2\n" );
	WTmessage( "-f(1,2)[1..4]	for a Polhemus Fastrak. Specify unit number after port.\n" );
	WTmessage( "-g(1,2)			for Geoball on port 1,2\n" );
	WTmessage( "-h(1,2)			for SpaceWare Spaceball SPACECONTROLLER on port 1,2\n" );				
	WTmessage( "-i(1,2)			for Insidetrak (unit 1 or 2)\n" );
	WTmessage( "-j(1,2)			for joyserial 1,2 (uses gameports, not serial ports)\n" );
	WTmessage( "-k(1,2)			for Logitech Space Control device on port 1,2:\n" );
	WTmessage( "                (on SGI, use spaceball driver (-s) for this device) \n" );
	WTmessage( "-l(1,2)			for Logitech tracker on port 1,2\n" );
	WTmessage( "-m				for mouse (default)\n" );
	WTmessage( "-n(1,2)			for Precision Navigation tracker on port 1,2\n" );				
	WTmessage( "-p(1,2)			for Polhemus Isotrak on port 1,2\n" );
	WTmessage( "-q(1,2)			for 5DT Glove on port 1,2\n" );
	WTmessage( "-r(1,2)			for Logitech Red Baron on port 1,2\n" );
	WTmessage( "-s(1,2)			for Spaceball on port 1,2\n" );
	WTmessage( "-t(1,2)[1..2]	for Polhemus IsotrakII on port 1,2\n" );
	WTmessage( "-u(1,2) 		for Thrustmaster Formula T2 driving console (unit 1 or 2)\n");
	WTmessage( "-v(1,2)			for CrystalEyes VR sensor on port 1,2\n" );
	WTmessage( "-x(1,2) 		for Cybermaxx2 on port 1,2\n" );		

	WTmessage( "-----\n" );
	WTmessage( "An exciting space game which demonstrates many of the features\n" );
	WTmessage( "Sense8's WorldToolKit real-time 3D graphics API\n" );
	WTmessage( "-----\n" );
}

/************************************************************************
*
*	MISC FUNCTIONS
*
************************************************************************/

/*
 * process command line arguments (sets global flags)
 */


void scan_arg(char *arg)
{


	if( strcmp( arg, "-usage" ) == 0 ) 
	{
		display_usage();
		exit( 0 );
	}

	if( strcmp( arg, "-?" ) == 0 ) 
	{
		display_usage();
		exit( 0 );
	}

	if( strcmp( arg, "?" ) == 0 ) 
	{
		display_usage();
		exit( 0 );
	}

	switch (arg[1]) {
		case 'a':	/* ascension bird */
		case 'b':	/* boom */
		case 'e':	/* i-glasses */
		case 'f':	/* fastrak */
		case 'g':	/* geoball */
		case 'h':	/* spaceball SPACECONTROLLER */
		case 'i':	/* insidetrak */
		case 'j':	/* joyserial */
		case 'k':	/* spacecontrol */
		case 'l':	/* logitech */
		case 'm':	/* mouse */
		case 'n':	/* precnavi */
		case 'p':	/* polhemus */
		case 'q':	/* glove5DT */
		case 'r':	/* red baron */
		case 's':	/* spaceball */
		case 't':	/* isotrak2 */
		case 'u':	/* formula */
		case 'v':	/* crystaleyes VR */
		case 'x':	/* cybermaxx2 */
		break;
	}
	switch (arg[1]) {
		case 'a': sensor_type = ST_BIRD; break;	/* Ascension Bird */
		case 'b': sensor_type = ST_BOOM; break;
		case 'c':
			display_mode = WTDISPLAY_CRYSTALEYES;
			window_mode  = WTWINDOW_NOBORDER;
			use_gui = FALSE;
			break;
		case 'd':
			display_mode = WTDISPLAY_CRYSTALEYES;
/*			window_mode  = WTWINDOW_STEREOVSPLIT;
*/
			use_gui = FALSE;
			break;
		case 'S':
			display_mode = WTDISPLAY_STEREO;
			window_mode  = WTWINDOW_NOBORDER;
			use_gui = FALSE;
			break;
		case 'W':
			display_mode = WTDISPLAY_STEREOWINDOW;
			window_mode  = WTWINDOW_NOBORDER;
			use_gui = FALSE;
			break;
		case 'R':
			display_mode = WTDISPLAY_RBSTEREO;
			use_gui = FALSE;
			break;
		case 'B':
			window_mode = WTWINDOW_NOBORDER;
			use_gui = FALSE;
			break;
		case 'Z':
			debug_mode = TRUE;
			break;
		case 'M':
			default_sound_device = WTSOUNDDEVICE_WINMM;
			break;
		case 'K':
			default_sound_device = WTSOUNDDEVICE_DWSTK;
			break;
		case 'V':
			default_sound_device = WTSOUNDDEVICE_VSI;
			break;
		case 'G':
			default_sound_device = WTSOUNDDEVICE_SGI;
			break;
		case 'C':
			default_sound_device = WTSOUNDDEVICE_CRE;
			break;
		case 'D':
			default_sound_device = WTSOUNDDEVICE_DIRECTSOUND;
			break;
		case 'X':
			texture_flag = FALSE;
			break;
		case 'e': sensor_type = ST_IGLASSES; break;
		case 'f': sensor_type = ST_FASTRAK; break;
		case 'g': sensor_type = ST_GEOBALL;	break;
		case 'h': sensor_type = ST_SBALLSC; break;
		case 'i': sensor_type = ST_INSIDETRAK; break;
		case 'j': sensor_type = ST_JOYSERIAL; break;
		case 'k': sensor_type = ST_SCONTROL; break;	
		case 'l': sensor_type = ST_LOGITECH; break;
		case 'm': sensor_type = ST_MOUSE; break;
		case 'n': sensor_type = ST_PRECNAVI; break;
		case 'p': sensor_type = ST_POLHEMUS; break;
		case 'q': sensor_type = ST_GLOVE5DT; break; 
		case 'r': sensor_type = ST_REDBARON; break;
		case 's': sensor_type = ST_SPACEBALL; break;
		case 't': sensor_type = ST_ISOTRAK2; break;
		case 'u': sensor_type = ST_FORMULA; break;
		case 'v': sensor_type = ST_CRYSTALVR; break;
		case 'w': 
			window_mode  = WTWINDOW_NOBORDER;
			use_gui = FALSE;
			break;	
		case 'x': sensor_type = ST_CYBERMAXX2; break;

				
		default:
			WTerror("Unrecognized argument -%c\n", arg[1]);
	}
	if (sensor_type != 999) {
		parse_sensor(arg+2);
		if (port != 0 && port != 1) 
		{
			WTmessage("bad port number (should be 1 or 2) in argument \"%s\"\n", arg);
		}
	}
}

void new_scan_args(int argc, char *argv[])
{
	short i;
	FLAG settings_read = FALSE;

	/* read settings file */
	if (argc > 1)
	{
		if ( strcmp( argv[1], "F") == 0) 
		if (argv[2] == "")
			read_settings(argv[3]);
		else
			read_settings(argv[2]);
	}
	else
	{
		read_settings("planets.ini");
		settings_read = TRUE;
		for (i = 1; i < argc; i++) 
		{
			if (argv[i][0] == '-')
				scan_arg(argv[i]);
		}
	}
	if (!settings_read) read_settings("planets.ini");
}


void scan_args(int argc, char *argv[])
{
	short i;
	for (i = 1; i < argc; i++) 
	{
		if (argv[i][0] == '-')
			scan_arg(argv[i]);
	}
}


int read_settings0(char *filename)
{

	FILE *datastream;
	int x;

	datastream = fopen( filename, "r" );

	fscanf(datastream," Rendering     %f \n",&x);
	fscanf(datastream," Sound         %f \n",&x);
	fscanf(datastream," Sensor        %f \n",&x);
	fscanf(datastream," Sound Device  %f \n",&x);


	return TRUE; /* settings read successfully */
}

int save_settings(char *filename)
{


	return TRUE; /* settings read successfully */
}

WTsounddevice *try_sounddevices(void)
{
	WTsounddevice *temp;
	temp = WTsounddevice_open( WTSOUNDDEVICE_DWSTK, MAXSOUNDS, outview);
	if (temp == NULL)
	{
		temp = WTsounddevice_open( WTSOUNDDEVICE_DIRECTSOUND, MAXSOUNDS, outview);
		if (temp == NULL)
		{		
			temp = WTsounddevice_open( WTSOUNDDEVICE_SGI, MAXSOUNDS, outview);
			if (temp == NULL)
			{
				temp = WTsounddevice_open( WTSOUNDDEVICE_WINMM, MAXSOUNDS, outview);
				if (temp == NULL)
				{
					WTmessage("No default sound device found\n");
				}
				else WTmessage("Using Windows Multimedia sound\n");
			}
			else WTmessage("Using SGI sound\n");
		}
		else WTmessage("Using DirectSound\n");	
	}
	else WTmessage("Using DiamondWare sound\n");

	return temp;
}

FLAG setup_sounds(int mySoundDeviceIndex)
{
	if (mySoundDevice != NULL) 
	{
		WTsounddevice_close(mySoundDevice);
		WTsound_delete(BoingSound);
		WTsound_delete(HummSound);
		WTsound_delete(myStartSound);
		WTsound_delete(myShutDownSound);
		WTsound_delete(myAlienSound);

	}

	if (mySoundDeviceIndex != 99)
	{
		if (mySoundDeviceIndex == -1)
			mySoundDevice = try_sounddevices();
		else
			mySoundDevice = WTsounddevice_open( mySoundDeviceIndex, MAXSOUNDS, outview);
		if (!mySoundDevice) 
			
			WTmessage("Couldn't open sound device\n");

		else
		{
			WTsounddevice_setparam(mySoundDevice, WTSOUNDDEVICE_ROLLOFF, UNIVERSE_SIZE);

			BoingSound =	  WTsound_load(mySoundDevice, "explo.wav");
			HummSound =		  WTsound_load(mySoundDevice, "humm.wav");
			ShieldSound =	  WTsound_load(mySoundDevice, "clank.wav");
			GameoverSound =	  WTsound_load(mySoundDevice, "gameover.wav");
			FireSound =		  WTsound_load(mySoundDevice, "fire.wav");

			myStartSound =	  WTsound_load(mySoundDevice, "startup.wav");
			myShutDownSound = WTsound_load(mySoundDevice, "shutdown.wav");

			myAlienSound =	  WTsound_load(mySoundDevice, "alien2.wav");	

			WTsound_setparam(BoingSound, WTSOUND_VOLUME, 100.0f);
			WTsound_setparam(HummSound,  WTSOUND_LOOPS, -1.0f);

			if (!WTsound_play(HummSound)) WTmessage("Problem playing HUMM sound\n");
		}

	}
	if ( BoingSound && 
		 HummSound && 
		 myStartSound && 
		 myShutDownSound &&
		 mySoundDevice == NULL)

		return FALSE;
	else
		return TRUE;
}

#define NUM_TEXTURES 57

unsigned char *texture[NUM_TEXTURES];
WTnode *bigpoly = NULL;
int texture_width, texture_height;

void load_animated_bits(void)
{
	int i;
    int threshold = 120;
	for(i=0; i<NUM_TEXTURES; i++) 
	{
		char buf[80];
		unsigned char *image;
		int j;
		sprintf(buf, "bang%02d", i);
		texture[i] = WTtexture_load( buf, &texture_width, &texture_height);
		WTtexture_cache(buf, TRUE);

		/* set threshold for transparency */
		/* this makes sure that the dark parts are true black (transparent) */
        image = texture[i];
		for (j = 0; j < (texture_width*texture_height); j++) {
			if((image[0]< threshold) && (image[1]< threshold) && (image[2] < threshold)) {
				image[3] = 0;
			} else {
			    image[3] = 255;
			}
		    image += 4;
		}
		WTtexture_replace(buf, WTIMAGE_RGBA, texture_width, texture_height, texture[i]);
	}
}


void animated_explosion_task(WTnode *thisnode)
{
	BoomData *data;
	WTp3 vpos, pos;
	WTq ori;

	data = (BoomData *)WTnode_getdata(thisnode);

	/* point poly toward viewpoint */
	if (cockpitView)
		WTviewpoint_getposition(inview, vpos);
	else
		WTviewpoint_getposition(outview, vpos);
//	WTviewpoint_getposition(WTuniverse_getviewpoints(), vpos);
	WTnode_gettranslation(thisnode, pos);
	WTp3_subtract(vpos, pos, pos);
	WTp3_norm(pos);
	WTdir_2q(pos, ori);
	WTnode_setorientation(thisnode, ori);

	if(data->index <= NUM_TEXTURES) 
	{
		char buf[80];
		WTgeometry *thisgeom = WTnode_getgeometry(thisnode);

		sprintf(buf, "bang%02d", (int)data->index);

		if (!WTgeometry_changetexture(thisgeom, buf, FALSE, TRUE))
			WTmessage("Problem with animated texture\n");

		data->index += (3.0f * sens * deltaT());
	}
	if(data->index >= NUM_TEXTURES) 
	{
		if (data->makeship)
		{
			new_ship();
			if (!cockpitView) MYmessage("Press 'v' to restore viewpoint");
		}
		WTnode_delete(thisnode);
		thisnode = NULL;
	}
}



WTnode *make_bigpoly(WTnode *parent)
{
	WTnode *polynode = NULL;

#if 1

	WTpoly *poly = NULL;
	WTgeometry *geom = NULL;
	WTp3 p;

	geom = WTgeometry_begin();

	/* add vertices to the geometry */
	p[Z] = 0.0f;

	p[X] = 30.0f; p[Y] = 30.0f; WTgeometry_newvertex(geom, p);
	p[X] = 30.0f; p[Y] =-30.0f; WTgeometry_newvertex(geom, p);
	p[X] =-30.0f; p[Y] =-30.0f; WTgeometry_newvertex(geom, p);
	p[X] =-30.0f; p[Y] = 30.0f; WTgeometry_newvertex(geom, p);

	/* add polygon to the geometry */
	poly = WTgeometry_beginpoly(geom);
  	  WTpoly_addvertex(poly, 3);
	  WTpoly_addvertex(poly, 2);
	  WTpoly_addvertex(poly, 1);
	  WTpoly_addvertex(poly, 0);
	WTpoly_close(poly);

	/*finish the geometry definition */
	WTgeometry_close(geom);

	WTgeometry_setrgb(geom, 255, 255, 255);

	/* make a movnode and attach it to the paretn */

	polynode = WTmovgeometrynode_new(parent, geom);

#else
	polynode = WTmovnode_load(parent, "poly.nff", 3.0f);
#endif

	return polynode;

}


void make_animated_explosion(WTp3 pos, FLAG shipflag)

{
	WTpoly *demo_poly = NULL;
	WTnode *mnode;
	BoomData *data;

//	mnode = WTmovnode_instance(root, bigpoly);
	mnode = make_bigpoly(root);
	
	if( mnode ) {
		WTgeometry *g = WTnode_getgeometry(mnode);
		demo_poly = WTgeometry_getpolys(g);
		WTpoly_settexture(demo_poly,"bang00",FALSE,TRUE);
	}

	WTtask_new(mnode, animated_explosion_task, 1.0f);

	data = (BoomData *)malloc(sizeof(BoomData));

	WTnode_setdata(mnode, (void*) data);

	data->index = 0.0f;
	data->makeship = shipflag;


	WTnode_settranslation(mnode, pos);

}


