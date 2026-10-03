#include "wt.h"
#include "rocks.h"

void ok_callback(WTui *,void *);
void help_callback(WTui *,void *);
void cancel_callback(WTui *,void *);
void test_callback(WTui *,void *);
void asens_callback(WTui *,void *);
void sens_callback(WTui *,void *);
void type_callback(WTui *,void *);
void unit_callback(WTui *,void *);
void port_callback(WTui *,void *);
void baud_callback(WTui *,void *);
void sound_ok_callback(WTui *ui,void *data);
void sound_help_callback(WTui *,void *);
void sound_cancel_callback(WTui *,void *);
void sound_test_callback(WTui *,void *);
void sounddevice_callback(WTui *ui,void *data);

char statline[160];

char *dummy[1];

WTsensor *dummy_sensor;

char bauds[6][6] = {{"1200"}, {"2400"}, {"4800"}, {"9600"},{"19200"}, {"38400"}};
int numbauds = 6;
int old_baud;

char *units[2] = {"1", "2"};
int numunits = 2;
int old_unit;

char *ports[4] = {"COM1:", "COM2:", "COM3:", "COM4:"};
int numports = 4;
int old_port;

int old_sensor_type;

float x = 3.0f;
float y = 3.0f;

float sensor_sensitivity;
float angular_rate;

int newSoundDeviceIndex;

WTui *shell, *statUI;

void build_statline(void)
{

	if ((unit == 99) & (baud != 99))
	{
		sprintf(statline, "SENSOR: %s on %s @ %s baud",
		sensor_name[sensor_type],
		ports[port],
		bauds[baud]);
	}

	if ((unit != 99) & (baud == 99))
	{
		sprintf(statline, "SENSOR: %s (unit %s)",
		sensor_name[sensor_type],
		units[unit]);
	}


	if ((baud == 99) & (unit == 99))
	{
		sprintf(statline, "SENSOR: %s ",
		sensor_name[sensor_type]);
	}

	if ((baud != 99) & (unit != 99) )
	{
		sprintf(statline, "SENSOR: %s (unit %s) on %s @ %s baud",
		sensor_name[sensor_type],
		units[unit],
		ports[port],
		bauds[baud]);
	}
}

void draw_statline(void)
{
	statUI   = WTui_newlabel(shell, statline, 0,
                  WTUIATT_LEFT,		(int) (40 * x), 
				  WTUIATT_TOP,		(int) (63 * y),
                  WTUIATT_WIDTH,	(int) (120 * x), 
				  WTUIATT_HEIGHT,	(int) (10 * y), 
				  NULL) ;
}

void new_statline(void)
{
	WTui_delete(statUI);
	draw_statline();
}

void about_ok_callback(WTui *ui,void *data)
{

	WTui_delete((WTui *)data);

}

void about_box(WTui *ui, void *data)
{
	WTui	*labelUI,
			*okUI;

	shell    = WTui_newform(toplevel, "About...",
                          WTUIATT_LEFT,		0, 
						  WTUIATT_TOP,		0,
                          WTUIATT_WIDTH,	(int) ( 90 * x), 
						  WTUIATT_HEIGHT,	(int) (110 * y), 
						  NULL);

	if (TRUE)
	{
		labelUI = WTui_newlabel(shell, "about.bmp", 1, 
			                  WTUIATT_LEFT,		(int) (  9 * x), 
							  WTUIATT_TOP,		(int) (  8 * y),
					          WTUIATT_WIDTH,	(int) ( 216 ), 
							  WTUIATT_HEIGHT,	(int) ( 212 ), 
							  NULL);
	}
	else
	{
		labelUI = WTui_newlabel(shell, "Space Rocks", 0, 
							  WTUIATT_LEFT,		(int) (35 * x), 
							  WTUIATT_TOP,		(int) (10 * y),
							  WTUIATT_WIDTH,	(int) (100 * x), 
							  WTUIATT_HEIGHT,	(int) (10  * y), 
							  NULL);

		labelUI = WTui_newlabel(shell, "by", 0, 
							  WTUIATT_LEFT,		(int) (46 * x), 
							  WTUIATT_TOP,		(int) (20 * y),
							  WTUIATT_WIDTH,	(int) (100 * x), 
							  WTUIATT_HEIGHT,	(int) (10  * y), 
							  NULL);

		labelUI = WTui_newlabel(shell, "Tom Payne", 0, 
							  WTUIATT_LEFT,		(int) (36 * x), 
							  WTUIATT_TOP,		(int) (30 * y),
							  WTUIATT_WIDTH,	(int) (100 * x), 
							  WTUIATT_HEIGHT,	(int) (10  * y), 
							  NULL);

		labelUI = WTui_newlabel(shell, "tomp@sense8.com", 0, 
							  WTUIATT_LEFT,		(int) (26 * x), 
							  WTUIATT_TOP,		(int) (40 * y),
							  WTUIATT_WIDTH,	(int) (100 * x), 
							  WTUIATT_HEIGHT,	(int) (10  * y), 
							  NULL);

		labelUI = WTui_newlabel(shell, "Sense8 Corporation (c)1996", 0, 
							  WTUIATT_LEFT,		(int) (19 * x), 
							  WTUIATT_TOP,		(int) (50 * y),
							  WTUIATT_WIDTH,	(int) (100 * x), 
							  WTUIATT_HEIGHT,	(int) (10  * y), 
							  NULL);
	}

	okUI     = WTui_newpushbutton(shell, "OK",
		                  WTUIATT_LEFT,		(int) (30 * x), 
						  WTUIATT_TOP,		(int) (83 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;
	
	WTui_setcallback(okUI, WTUIEVENT_ACTIVATE, about_ok_callback, shell);

	WTui_manage(shell);

}

void sound_config(WTui *ui, void *data)
{
	WTui	*typeUI,
			*okUI, *helpUI, *testUI, *cancelUI;

	shell    = WTui_newform(toplevel, "Sound Configuration",
                          WTUIATT_LEFT,		0, 
						  WTUIATT_TOP,		0,
                          WTUIATT_WIDTH,	(int) (195 * x), 
						  WTUIATT_HEIGHT,	(int) (95  * y), 
						  NULL);

	typeUI   = WTui_newscrolledlist(shell, "Device", dummy, 0,
		                  WTUIATT_LEFT,		(int) (10 * x), 
						  WTUIATT_TOP,		(int) (3  * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;

	WTui_insertitem(typeUI, 0, "Windows MM");
	WTui_insertitem(typeUI, 1, "DiamondWare");
	WTui_insertitem(typeUI, 2, "DirectSound");
	WTui_insertitem(typeUI, 3, "VSI");
	WTui_insertitem(typeUI, 4, "Crystal River");
	WTui_insertitem(typeUI, 5, "SGI Audio Libary");
	WTui_insertitem(typeUI, 6, "none");


	cancelUI = WTui_newpushbutton(shell, "Cancel",
		                  WTUIATT_LEFT,		(int) (20 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;	

	testUI   = WTui_newpushbutton(shell, "Test",
		                  WTUIATT_LEFT,		(int) (60 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;
	
	okUI     = WTui_newpushbutton(shell, "OK",
		                  WTUIATT_LEFT,		(int) (100 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;
	
	helpUI   = WTui_newpushbutton(shell, "Help",
		                  WTUIATT_LEFT,		(int) (140 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;

	WTui_setcallback(okUI,     WTUIEVENT_ACTIVATE, sound_ok_callback,     shell);
    WTui_setcallback(helpUI,   WTUIEVENT_ACTIVATE, sound_help_callback,   NULL);
    WTui_setcallback(cancelUI, WTUIEVENT_ACTIVATE, sound_cancel_callback, shell);
    WTui_setcallback(testUI,   WTUIEVENT_ACTIVATE, sound_test_callback,   NULL);

    WTui_setcallback(typeUI,   WTUIEVENT_ACTIVATE, sounddevice_callback, NULL);

	WTui_manage(shell);

}

void sound_help_callback(WTui *ui,void *data) {}
void sound_cancel_callback(WTui *ui,void *data) {}
void sound_test_callback(WTui *ui,void *data) {}

void sensor_config(WTui *ui,void *data)
{
	WTui	*asensUI, *sensUI, *typeUI, *portUI, *unitUI, *baudUI,
			*okUI, *helpUI, *testUI, *cancelUI, *tempUI;
	int i;

	dummy_sensor = NULL;
	old_baud = baud;
	old_port = port;
	old_unit = unit;
	old_sensor_type = sensor_type;
	sensor_sensitivity = WTsensor_getsensitivity (sensor);
	angular_rate       = WTsensor_getangularrate(sensor);


	shell    = WTui_newform(toplevel, "Sensor Configuration",
                          WTUIATT_LEFT,		0, 
						  WTUIATT_TOP,		0,
                          WTUIATT_WIDTH,	(int) (195 * x), 
						  WTUIATT_HEIGHT,	(int) (95  * y), 
						  NULL);

	tempUI   = WTui_newframe(shell, "",
		                  WTUIATT_LEFT,		(int) (10 * x), 
						  WTUIATT_TOP,		(int) (3  * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;


	typeUI   = WTui_newscrolledlist(tempUI, "Type", dummy, 0,
		                  WTUIATT_LEFT,		(int) (10 * x), 
						  WTUIATT_TOP,		(int) (3  * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;

	for (i = 0; i < SENSOR_TYPES; i++) WTui_insertitem(typeUI, i, sensor_name[i]);

	tempUI   = WTui_newframe(shell, "",
		                  WTUIATT_LEFT,		(int) (70 * x), 
						  WTUIATT_TOP,		(int) (3  * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;

	portUI   = WTui_newscrolledlist(tempUI, "Port", dummy, 0,
		                  WTUIATT_LEFT,		(int) (70 * x), 
						  WTUIATT_TOP,		(int) (3  * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;

	for (i = 0; i < numports; i++) WTui_insertitem(portUI, i, ports[i]);

	tempUI   = WTui_newframe(shell, "",
		                  WTUIATT_LEFT,		(int) (130 * x), 
						  WTUIATT_TOP,		(int) (3  * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (5  * y), 
						  NULL) ;

	unitUI   = WTui_newscrolledlist(tempUI, "Unit", dummy, 0,
		                  WTUIATT_LEFT,		(int) (130 * x), 
						  WTUIATT_TOP,		(int) (3  * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (5  * y), 
						  NULL) ;

	for (i = 0; i < numunits; i++) WTui_insertitem(unitUI, i, units[i]);

	tempUI   = WTui_newframe(shell, "",
		                  WTUIATT_LEFT,		(int) (10 * x), 
						  WTUIATT_TOP,		(int) (33 * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL);

	baudUI   = WTui_newscrolledlist(tempUI, "Baud Rate", dummy, 0,
		                  WTUIATT_LEFT,		(int) (10 * x), 
						  WTUIATT_TOP,		(int) (33 * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL);

  	for (i = 0; i < numbauds; i++) WTui_insertitem(baudUI, i, bauds[i]);
	
	asensUI   = WTui_newscale(shell, "Angular Rate", 0, 30, 2, (int)(angular_rate * 100.0f), 
		                  WTUIATT_LEFT,		(int) (70 * x), 
						  WTUIATT_TOP,		(int) (35 * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (5 * y), 
						  NULL) ;

	sensUI   = WTui_newscale(shell, "Sensitivity", 0, 100, 1, (int)(sensor_sensitivity * 10.0f), 
		                  WTUIATT_LEFT,		(int) (130 * x), 
						  WTUIATT_TOP,		(int) (35 * y),
                          WTUIATT_WIDTH,	(int) (50 * x), 
						  WTUIATT_HEIGHT,	(int) (5 * y), 
						  NULL) ;

	cancelUI = WTui_newpushbutton(shell, "Cancel",
		                  WTUIATT_LEFT,		(int) (20 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;	

	testUI   = WTui_newpushbutton(shell, "Test",
		                  WTUIATT_LEFT,		(int) (60 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;
	
	okUI     = WTui_newpushbutton(shell, "OK",
		                  WTUIATT_LEFT,		(int) (100 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;
	
	helpUI   = WTui_newpushbutton(shell, "Help",
		                  WTUIATT_LEFT,		(int) (140 * x), 
						  WTUIATT_TOP,		(int) (73 * y),
                          WTUIATT_WIDTH,	(int) (30 * x), 
						  WTUIATT_HEIGHT,	(int) (10 * y), 
						  NULL) ;

	build_statline();	  
	draw_statline();

	WTui_setcallback(okUI,     WTUIEVENT_ACTIVATE, ok_callback,     shell);
    WTui_setcallback(helpUI,   WTUIEVENT_ACTIVATE, help_callback,   NULL);
    WTui_setcallback(cancelUI, WTUIEVENT_ACTIVATE, cancel_callback, shell);
    WTui_setcallback(testUI,   WTUIEVENT_ACTIVATE, test_callback,   NULL);

    WTui_setcallback(sensUI,   WTUIEVENT_ACTIVATE, sens_callback,   NULL);
    WTui_setcallback(asensUI,  WTUIEVENT_ACTIVATE, asens_callback,  NULL);
    WTui_setcallback(typeUI,   WTUIEVENT_ACTIVATE, type_callback,   NULL);
    WTui_setcallback(unitUI,   WTUIEVENT_ACTIVATE, unit_callback,   NULL);
    WTui_setcallback(portUI,   WTUIEVENT_ACTIVATE, port_callback,   NULL);
    WTui_setcallback(baudUI,   WTUIEVENT_ACTIVATE, baud_callback,   NULL);

	WTui_manage(shell);
}

void ok_callback(WTui *ui,void *data)
{
	if (dummy_sensor == NULL)
	{
		if (sensor_type != old_sensor_type)	setup_sensors(&sensor);
	}
	else
		sensor = dummy_sensor;

	if (sensor != NULL)
	{
		if (sensor_type != old_sensor_type)	WTmessage("Sensor %s being used\n", sensor_name[sensor_type]);
		WTsensor_setsensitivity(sensor, sensor_sensitivity);
		WTsensor_setangularrate(sensor, angular_rate);
	}
	else
		WTmessage("Error attempting to use sensor (%s)\n", sensor_name[sensor_type]);


	WTui_delete((WTui *)data);
}

void help_callback(WTui *ui,void *data)
{
	sorry(NULL, NULL);
}

void cancel_callback(WTui *ui,void *data)
{
	baud = old_baud;
	port = old_port;
	unit = old_unit;
	sensor_type = old_sensor_type;

	WTui_delete((WTui *)data);
}

void test_callback(WTui *ui,void *data)
{

	setup_sensors(&dummy_sensor);
}

void sens_callback(WTui *ui,void *data)
{
	float *value;
	value = (float *)data;
	sensor_sensitivity = *value;
}

void asens_callback(WTui *ui,void *data)
{
	float *value;
	value = (float *)data;
	angular_rate = *value;
}

void sound_ok_callback(WTui *ui,void *data)
{

	if (setup_sounds(newSoundDeviceIndex) == TRUE)
			WTui_delete((WTui *)data);

}

void sounddevice_callback(WTui *ui, void *data)
{
	int *newtype;
	newtype = (int *)data;
	*newtype;

	switch (*newtype) {
			case 0:
				newSoundDeviceIndex = WTSOUNDDEVICE_WINMM;
				sprintf(statline,"Windows Multimedia");
				break;
			case 1:
				newSoundDeviceIndex = WTSOUNDDEVICE_DWSTK;
				sprintf(statline,"DiamondWare");
				break;
			case 2:
				newSoundDeviceIndex = WTSOUNDDEVICE_DIRECTSOUND;
				sprintf(statline,"Windows DirectSound");
				break;
			case 3:
				newSoundDeviceIndex = WTSOUNDDEVICE_VSI;
				sprintf(statline,"VSI");
				break;
			case 4:
				newSoundDeviceIndex = WTSOUNDDEVICE_SGI;
				sprintf(statline,"SGI Audio");
				break;
			case 5:
				newSoundDeviceIndex = WTSOUNDDEVICE_CRE;
				sprintf(statline,"Crystal River");
				break;
			case 6:
				newSoundDeviceIndex = 99;
				sprintf(statline,"- sound disabled -");
				break;
	}
	new_statline();
}


void type_callback(WTui *ui,void *data)
{
	int *newtype;
	newtype = (int *)data;
	sensor_type = *newtype;

	switch (sensor_type) {
			case ST_BIRD:
				baud = 3; if (unit == 99) unit = 0; break;
			case ST_BOOM:
				baud = 3; break;
			case ST_CRYSTALVR:
				baud = 0; break;
			case ST_FASTRAK:
				baud = 4; if (unit == 99) unit = 0; break;
			case ST_FORMULA:
				baud = 99; if (unit == 99) unit = 0; break;
			case ST_GEOBALL:
				baud = 3; break;
			case ST_GLOVE5DT:
				baud = 4;break;
			case ST_IGLASSES:
				baud = 4;break;
			case ST_INSIDETRAK:
				baud=99; if (unit == 99) unit = 0; break;
			case ST_ISOTRAK2:
				baud = 4; if (unit == 99) unit = 0; break;
			case ST_JOYSERIAL:
				baud = 4;break;
			case ST_LOGITECH:
				baud = 0; break;
			case ST_POLHEMUS:
				baud = 3;break;
			case ST_PRECNAVI:
				baud = 5; break;
			case ST_REDBARON:
				baud = 0;break;
			case ST_SCONTROL:
				baud = 3; break;
			case ST_SPACEBALL:
				baud = 3;break;
			case ST_MOUSE:
				baud = 99;
				break;
	}
	
	build_statline();
	new_statline();
}

void unit_callback(WTui *ui,void *data)
{
	int *newtype;
	newtype = (int *)data;
	unit = *newtype;
	build_statline();	  
	new_statline();
}

void baud_callback(WTui *ui,void *data)
{
	int *newtype;
	newtype = (int *)data;
	baud = *newtype;
	build_statline();	  
	new_statline();
}

void port_callback(WTui *ui,void *data)
{
	int *newtype;
	newtype = (int *)data;
	port = *newtype;
	build_statline();	  
	new_statline();
}

void zoomall(WTui *ui,void *data)
{
	WTwindow_zoomviewpoint(WTuniverse_getwindows());
}

void zoomship(WTui *ui,void *data)
{
	WTwindow_zoomviewtonode(WTuniverse_getwindows(),myShipShield, 0);
}

void closeui(WTui *ui,void *data)
{
	WTui_delete((WTui *)data);
}

void togglemode(WTui *ui,void *data)
{
	ThreeDVersion = !ThreeDVersion;
	if (ThreeDVersion)
	{
		MYmessage("3D Mode");
		WTui_setmenutext(menu_flags.mode,"2D Mode");
	}
	else
	{
		MYmessage("2D Mode");
		WTui_setmenutext(menu_flags.mode,"3D Mode");
	}
	{
		WTm3 myRot;
		WTeuler_2m3(3.1415f/2.0f, 0.0f, 0.0f, myRot);
		WTnode_setrotation(myShip, myRot);
	}
}

void reset0(WTui *ui,void *data)
{

	if (net_master != 0)
	{
		/* reset view back to initial view */
		MYmessage("Game Reset");
		WTviewpoint_moveto( outview, &initial_pq);

		/* return ship to initial position */
		WTnode_settranslation(myShip, zeroPos);

		paused = FALSE;
		GameScore = 0;
		GameLives = NUM_LIVES - 1; /* first ship already made */
		alien_insertion_timer = 100.0f + (float) fabs(myrand(200.0f));

		GameLevel = 0;

		WTnode_delete(RockRoot);
		RockRoot = WTgroupnode_new(root);	
		/* action routine will replace rocks */

		if (overnode != NULL)
			WTnode_delete(overnode);
		overnode = NULL;
	}
	else
	{
		WTmessage("Only net master can reset!\n");
	}
}

void really_exit(WTsound *sound)
{
	WTui_delete(toplevel);
	WTuniverse_delete();

	exit(0);
}


void test_exit(WTui *ui,void *data)
{
	static int count=0;

	if (count == 1) really_exit(NULL); /* sometimes, exiting doesn't work the first time */
									   /* due to donefn() bug */
	count++;

	/*Release all memory allocated by WTK*/
	if (myShutDownSound == NULL || mySoundDevice == NULL)
	{
		really_exit(NULL);
	}
	else
	{	
		WTsound_setdonefn(myShutDownSound, really_exit);
		WTsound_play(myShutDownSound);
	}

}

void toggle_logo(WTui *ui,void *data)
{
	logo = !logo;
}	


void togglescore(WTui *ui,void *data)
{
	displayText = !displayText;
}	

void togglepause(WTui *ui,void *data)
{
	paused = !paused;
	if (paused)
		MYmessage("Frozen");
	else
		MYmessage("Thawed");
}	

void toggle_detect_col(WTui *ui,void *data)
{
	detect_rock_col = !detect_rock_col;
	if (detect_rock_col)
	{
		MYmessage("Collision Detection Enabled (Rocks)");
		WTui_setmenutext(menu_flags.debug[1],"Disable Colisions");
	}
	else
	{
		MYmessage("Collision Detection Disabled (Rocks)");
		WTui_setmenutext(menu_flags.debug[1],"Enable Colisions");
	}

}

void toggle_rock_motion(WTui *ui,void *data)
{
	MoveRocks = !MoveRocks;
	if (MoveRocks)
	{
		MYmessage("Rock Motion Enabled");
		WTui_setmenutext(menu_flags.debug[0],"Disable Rock Motion");
	}
	else
	{
		MYmessage("Rock Motion Disabled");
		WTui_setmenutext(menu_flags.debug[0],"Enable Rock Motion");
	}

}

void render_mode(WTui *ui,void *style)
{
	FLAG new_style;
	int menu_index;
    int temp= (int)style;

	switch(temp){
		case WTRENDER_WIREFRAME:
			if(menu_flags.wireframe){
				new_style= menu_flags.render_modifiers;

				/*"un-dim" the other menu items	 */
				for(menu_index= 1;menu_index< 4;menu_index++)
					WTui_dimitem(menu_flags.render_menus[menu_index],FALSE);

				/*Change the label of the menu to "Wireframe"  */
				WTui_setmenutext(ui,"Enable Wireframe Mode");
			}
			else{
				new_style= WTRENDER_WIREFRAME;

				/*Dim the other menu items	*/
				for(menu_index= 1;menu_index< 4;menu_index++)
					WTui_dimitem(menu_flags.render_menus[menu_index],TRUE);

				/*Change the label of the menu to "Solid"*/
				WTui_setmenutext(ui,"Disable Wireframe");
			}

			/*Toggle the current wireframe state */
			menu_flags.wireframe^= TRUE;
			break;
		case WTRENDER_LIGHTING:
			new_style= (unsigned char)(menu_flags.render_modifiers ^ WTRENDER_LIGHTING);
			menu_flags.render_modifiers= new_style;

			/*Change the label of the menu */
			if(new_style & WTRENDER_LIGHTING)
				WTui_setmenutext(menu_flags.render_menus[1],"Turn Lighting Off");
			else
				WTui_setmenutext(menu_flags.render_menus[1],"Turn Lighting On");

			break;
		case WTRENDER_SMOOTH:
			new_style= (unsigned char)(menu_flags.render_modifiers ^ WTRENDER_SMOOTH);
			menu_flags.render_modifiers= new_style;

			/*Change the label of the menu */
			if(new_style & WTRENDER_SMOOTH)
				WTui_setmenutext(menu_flags.render_menus[2],"Turn Smooth Shading Off");
			else
				WTui_setmenutext(menu_flags.render_menus[2],"Turn Smooth Shading On");
			
			break;
		case WTRENDER_TEXTURED:
			new_style= (unsigned char)(menu_flags.render_modifiers ^ WTRENDER_TEXTURED);
			menu_flags.render_modifiers= new_style;

			/*Change the label of the menu	*/
			if(new_style & WTRENDER_TEXTURED)
				WTui_setmenutext(menu_flags.render_menus[3],"Turn Texturing Off");
			else
				WTui_setmenutext(menu_flags.render_menus[3],"Turn Texturing On");
			
			break;
		case WTRENDER_PERSPECTIVE:
			new_style= (unsigned char)(menu_flags.render_modifiers ^ WTRENDER_PERSPECTIVE);
			menu_flags.render_modifiers= new_style;

			/*Change the label of the menu	*/
			if(new_style & WTRENDER_PERSPECTIVE)
				WTui_setmenutext(menu_flags.render_menus[4],"Turn Perspective Texturing Off");
			else
				WTui_setmenutext(menu_flags.render_menus[4],"Turn Perspective Texturing On");
			
			break;
	}

	WTuniverse_setrendering(new_style);
}

void texture_mode(WTui *ui,void *menuitem)
{
	int choice= (int)menuitem;
		switch (choice) {
			case TEXTURE_POINT: /* Point sampling */
				
				WTtexture_setfilter(BOOM_TEXTURE,  WTFILTER_NEAREST,WTFILTER_NEAREST);
				WTtexture_setfilter(SHIELD_TEXTURE,WTFILTER_NEAREST,WTFILTER_NEAREST);
				WTtexture_setfilter(ROCK_TEXTURE,  WTFILTER_NEAREST,WTFILTER_NEAREST);
				WTtexture_setfilter(BACK_TEXTURE1, WTFILTER_NEAREST,WTFILTER_NEAREST);
				WTtexture_setfilter(BACK_TEXTURE2, WTFILTER_NEAREST,WTFILTER_NEAREST);
				WTtexture_setfilter(BACK_TEXTURE3, WTFILTER_NEAREST,WTFILTER_NEAREST);
				WTtexture_setfilter(BACK_TEXTURE4, WTFILTER_NEAREST,WTFILTER_NEAREST);
				WTtexture_setfilter(BACK_TEXTURE5, WTFILTER_NEAREST,WTFILTER_NEAREST);
				break;
			case TEXTURE_BILINEAR: /* Bilinear */
			
				WTtexture_setfilter(BOOM_TEXTURE,  WTFILTER_LINEAR,WTFILTER_LINEAR);
				WTtexture_setfilter(SHIELD_TEXTURE,WTFILTER_LINEAR,WTFILTER_LINEAR);
				WTtexture_setfilter(ROCK_TEXTURE,  WTFILTER_LINEAR,WTFILTER_LINEAR);
				WTtexture_setfilter(BACK_TEXTURE1, WTFILTER_LINEAR,WTFILTER_LINEAR);
				WTtexture_setfilter(BACK_TEXTURE2, WTFILTER_LINEAR,WTFILTER_LINEAR);
				WTtexture_setfilter(BACK_TEXTURE3, WTFILTER_LINEAR,WTFILTER_LINEAR);
				WTtexture_setfilter(BACK_TEXTURE4, WTFILTER_LINEAR,WTFILTER_LINEAR);
				WTtexture_setfilter(BACK_TEXTURE5, WTFILTER_LINEAR,WTFILTER_LINEAR);
				
				break;
			case TEXTURE_MIPMAP: /* Mipmapped */
							
				WTtexture_setfilter(BOOM_TEXTURE,  WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				WTtexture_setfilter(SHIELD_TEXTURE,WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				WTtexture_setfilter(ROCK_TEXTURE,  WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				WTtexture_setfilter(BACK_TEXTURE1, WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				WTtexture_setfilter(BACK_TEXTURE2, WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				WTtexture_setfilter(BACK_TEXTURE3, WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				WTtexture_setfilter(BACK_TEXTURE4, WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				WTtexture_setfilter(BACK_TEXTURE5, WTFILTER_LINEAR,WTFILTER_LINEARMIPMAPLINEAR);
				break;
		}

	/*Adjust the menus accordingly*/
	WTui_dimitem(menu_flags.texture_menu[0], (FLAG)(choice == TEXTURE_POINT));
	WTui_dimitem(menu_flags.texture_menu[1], (FLAG)(choice == TEXTURE_BILINEAR));
	WTui_dimitem(menu_flags.texture_menu[2], (FLAG)(choice == TEXTURE_MIPMAP));

	menu_flags.texture_mode= (char)choice;
}

void print_readme(WTui *ui,void *menuitem)
{
	/* do nothing */
}


void select_rock(int which)
{

	int x,y;

	RockType = which;

	y = WTnode_numchildren(RockRoot);
	
	for (x=0; x<y; x++)
	{
		WTswitchnode_setwhichchild(WTnode_getchild(RockRoot,x), which);
	}

}

void select_rock0(WTui *ui,void *data)
{
	select_rock(0);
}

void select_rock1(WTui *ui,void *data)
{
	select_rock(1);
}

void select_rock2(WTui *ui,void *data)
{
	select_rock(2);
}

void select_rock3(WTui *ui,void *data)
{
	select_rock(3);
}

WTui *URL_select;

static void onURLopenshipok(WTui *ui, void *data)
{
	WTp3 pos;

	shipfilename = WTui_gettext(URL_select);
	WTmessage ("Attempting to load ship model from: %s\n", shipfilename);
	WTnode_gettranslation(myShip, pos);
	WTnode_delete(myShip);
	myShip = NULL;
	makeship(pos);
	WTui_delete(ui);
}

static void onfilesopenshipok(WTui *ui, void *data)
{
	WTp3 pos;

	shipfilename = (char *)data;
	WTnode_gettranslation(myShip, pos);
	WTnode_delete(myShip);
	myShip = NULL;
	makeship(pos);
	WTui_delete(ui);
}

static void onfilesopensoundok(WTui *ui, void *data)
{

	BoingSound = WTsound_load(mySoundDevice, (char *)data);

	WTui_delete(ui);
}

static void fileopenship(WTui *pStruct, void *pData)
{
	WTui *file_select;
	
	file_select = WTui_newfileselection(toplevel, "Load Ship" ,NULL, "*.nff; *.flt; *.wrl; *.obj; *.3ds", NULL);
	WTui_setcallback(file_select, WTUIEVENT_ACTIVATE, onfilesopenshipok, NULL);
}

static void fileopenshipfromurl(WTui *pStruct, void *pData)
{
	
	URL_select = WTui_newtextinput(toplevel, "Enter URL of VRML 1.0 file (.wrl)" ,FALSE);
	WTui_setcallback(URL_select, WTUIEVENT_ACTIVATE, onURLopenshipok, NULL);
}

static void fileopensound(WTui *pStruct, void *pData)
{
	WTui *file_select;
	
	file_select = WTui_newfileselection(toplevel, "Load Sound" ,NULL, "*.wav", NULL);
	WTui_setcallback(file_select, WTUIEVENT_ACTIVATE, onfilesopensoundok, pData);
}

static void fileopentexture(WTui *pStruct, void *pData)
{
	WTui *file_select;
	
	file_select = WTui_newfileselection(toplevel, "Load Ship" ,NULL, "*.nff; *.flt; *.wrl; *.obj; *.3ds", NULL);
	WTui_setcallback(file_select, WTUIEVENT_ACTIVATE, onfilesopenshipok, NULL);
}

static void sorry(WTui *pStruct, void *pData)
{
	WTui_newmessagebox(toplevel, "Sorry, this feature has not yet been implimented", "Sorry!", NULL);
}


void build_menubar(WTui *parent)
{
    WTui *mmenu;
	WTui *cb1, *cb2, *pb0, *pb1, *pb2, *pb3, *pb4,*pb5, *pb6, *pb7, *pb8;
	WTui *xb1, *xb2, *xb3, *xb4;
	int temp=  WTRENDER_WIREFRAME;

    /*** Create the menubar ****/
    mmenu = WTui_newmenubar(parent,NULL);
    
	/*Create the Test menu*/
	cb1 = WTui_newmenupopup(mmenu, "File",NULL);
   
	/*Build the "Face Type" submenu*/
	pb1 = WTui_newmenuitem(cb1, "New Game", NULL);
	pb4 = WTui_newmenuitem(cb1, "Pause", NULL);
	cb2 = WTui_newmenupopup(cb1,"Load",NULL);
    pb2 = WTui_newmenuitem(cb1, "Print Performance",NULL);
    pb3 = WTui_newmenuitem(cb1, "Exit",NULL);
	
    WTui_setcallback(pb1,WTUIEVENT_ACTIVATE,reset0,NULL);
    WTui_setcallback(pb4,WTUIEVENT_ACTIVATE,togglepause,NULL);
    WTui_setcallback(pb2,WTUIEVENT_ACTIVATE,print_performance,NULL);
    WTui_setcallback(pb3,WTUIEVENT_ACTIVATE,test_exit,NULL);

    pb1 = WTui_newmenuitem (cb2, "Ship Model",NULL);
    pb4 = WTui_newmenuitem (cb2, "Ship Model from URL",NULL);
    pb2 = WTui_newmenuitem (cb2, "Rock Texture",NULL);
    pb3 = WTui_newmenupopup(cb2, "Sounds",NULL);

    WTui_setcallback(pb1,WTUIEVENT_ACTIVATE,fileopenship,NULL);
    WTui_setcallback(pb2,WTUIEVENT_ACTIVATE,fileopenship,NULL);
    WTui_setcallback(pb4,WTUIEVENT_ACTIVATE,fileopenshipfromurl,NULL);

    xb1 = WTui_newmenuitem(pb3, "Explosions",NULL);
    xb2 = WTui_newmenuitem(pb3, "Background",NULL);
    xb3 = WTui_newmenuitem(pb3, "Device Configure",NULL);

    WTui_setcallback(xb1,WTUIEVENT_ACTIVATE,fileopensound, "1");
    WTui_setcallback(xb2,WTUIEVENT_ACTIVATE,fileopensound, "2");
    WTui_setcallback(xb3,WTUIEVENT_ACTIVATE,sound_config, NULL);

	cb1 = WTui_newmenupopup(mmenu, "Edit", NULL);	

    pb1 = WTui_newmenupopup(cb1, "Render",NULL);
    pb2 = WTui_newmenupopup(cb1, "Texture",NULL);
    pb3 = WTui_newmenupopup(cb1, "Displays",NULL);

    pb4 = WTui_newmenuitem (cb1, "Toggle Viewpoint",NULL);
    WTui_setcallback(pb4,WTUIEVENT_ACTIVATE, toggleview, NULL);

    pb5 = WTui_newmenupopup(cb1, "Rocks",NULL);

	if (debug_mode)
	{
		pb6 = WTui_newmenuitem (cb1, "Sensor Config",NULL);
        WTui_setcallback(pb6,WTUIEVENT_ACTIVATE, sensor_config, NULL);
		pb7 = WTui_newmenuitem (cb1, "Sound Config",NULL);
	    WTui_setcallback(pb7,WTUIEVENT_ACTIVATE, sound_config, NULL);
	}

	pb8 = WTui_newmenupopup(cb1, "Starfield", NULL);


    /* set menu item text to option available to user */

    xb1 = WTui_newmenuitem(pb1, "Wireframe",NULL);
    xb2 = WTui_newmenuitem(pb1, "Shaded",NULL);
    xb3 = WTui_newmenuitem(pb1, "Textured",NULL);
    WTui_setcallback(xb1,WTUIEVENT_ACTIVATE,wireframe_select,NULL);
    WTui_setcallback(xb2,WTUIEVENT_ACTIVATE,shade_select,NULL);
    WTui_setcallback(xb3,WTUIEVENT_ACTIVATE,texture_select,NULL);
	menu_flags.render_menus[0]= xb1;
	menu_flags.render_menus[1]= xb2;
	menu_flags.render_menus[2]= xb3;
	
	/*Create the Texture submenu*/
    xb1 = WTui_newmenuitem(pb2, "Point",NULL);
    xb2 = WTui_newmenuitem(pb2, "Bilinear",NULL);
    xb3 = WTui_newmenuitem(pb2, "Mipmap",NULL);
    WTui_setcallback(xb1,WTUIEVENT_ACTIVATE,texture_mode,(void *) TEXTURE_POINT);
    WTui_setcallback(xb2,WTUIEVENT_ACTIVATE,texture_mode,(void *) TEXTURE_BILINEAR);
    WTui_setcallback(xb3,WTUIEVENT_ACTIVATE,texture_mode,(void *) TEXTURE_MIPMAP);

	/*Set the default modes and save the menu items*/
	menu_flags.texture_mode= TEXTURE_MIPMAP;
	WTui_dimitem(xb3,TRUE);
	menu_flags.texture_menu[0]= xb1;
	menu_flags.texture_menu[1]= xb2;
	menu_flags.texture_menu[2]= xb3;

	/*Create the Displays submenu*/
    xb1 = WTui_newmenuitem(pb3, "Score",NULL);
    WTui_setcallback(xb1,WTUIEVENT_ACTIVATE,togglescore, NULL);

	if (debug_mode)
	{
	    xb2 = WTui_newmenuitem(pb3, "HUD",NULL);
	    WTui_setcallback(xb2,WTUIEVENT_ACTIVATE,sorry, NULL);

		xb3 = WTui_newmenuitem(pb3, "Messages",NULL);
		WTui_setcallback(xb3,WTUIEVENT_ACTIVATE,sorry, NULL);
	}

    xb4 = WTui_newmenuitem(pb3, "Performance",NULL);
    WTui_setcallback(xb4,WTUIEVENT_ACTIVATE,performance_toggle, NULL);
	
	/*Create the Rocks submenu*/
    xb1 = WTui_newmenuitem(pb5, "Cubes",NULL);
    xb2 = WTui_newmenuitem(pb5, "2/4 Sphere",NULL);
    xb3 = WTui_newmenuitem(pb5, "4/8 Sphere",NULL);
    xb4 = WTui_newmenuitem(pb5, "Crumpled",NULL);
    WTui_setcallback(xb1,WTUIEVENT_ACTIVATE, select_rock0, NULL);
    WTui_setcallback(xb2,WTUIEVENT_ACTIVATE, select_rock1, NULL);
    WTui_setcallback(xb3,WTUIEVENT_ACTIVATE, select_rock2, NULL);
    WTui_setcallback(xb4,WTUIEVENT_ACTIVATE, select_rock3, NULL);

	/*Create the Starfield submenu*/
    xb1 = WTui_newmenuitem(pb8, "Texture toggle",NULL);
    xb2 = WTui_newmenuitem(pb8, "Points toggle",NULL);
    WTui_setcallback(xb1,WTUIEVENT_ACTIVATE, starfield_texture_toggle, NULL);
    WTui_setcallback(xb2,WTUIEVENT_ACTIVATE, draw_stars_toggle, NULL);

	/*Create the Game menu*/
    cb1 = WTui_newmenupopup(mmenu,"Game",NULL);
	pb0 = WTui_newmenuitem(cb1, "Reset",NULL);
	pb1 = WTui_newmenuitem(cb1, "2D Mode",NULL);
	WTui_setcallback(pb0,WTUIEVENT_ACTIVATE,reset0,NULL);
	WTui_setcallback(pb1,WTUIEVENT_ACTIVATE,togglemode,NULL);
	menu_flags.mode = pb1;


	if (debug_mode)
	/*Create the Debug menu*/
	{
	    cb1 = WTui_newmenupopup(mmenu,"Debug",NULL);
		pb0 = WTui_newmenuitem(cb1, "Disable Rock Motion",NULL);
		pb1 = WTui_newmenuitem(cb1, "Disable Colisions",NULL);
		pb2 = WTui_newmenuitem(cb1, "Zoom All",NULL);
		pb3 = WTui_newmenuitem(cb1, "Reset",NULL);
		pb4 = WTui_newmenuitem(cb1, "Toggle Logo",NULL);
		pb5 = WTui_newmenuitem(cb1, "Game Over sequence",NULL);
		pb6 = WTui_newmenuitem(cb1, "Redefine star positions",NULL);

		WTui_setcallback(pb0,WTUIEVENT_ACTIVATE,toggle_rock_motion,NULL);
		WTui_setcallback(pb1,WTUIEVENT_ACTIVATE,toggle_detect_col,NULL);
		WTui_setcallback(pb2,WTUIEVENT_ACTIVATE,zoomall,NULL);
		WTui_setcallback(pb3,WTUIEVENT_ACTIVATE,reset0,NULL);
		WTui_setcallback(pb4,WTUIEVENT_ACTIVATE,toggle_logo,NULL);
		WTui_setcallback(pb5,WTUIEVENT_ACTIVATE,gameover,NULL);
		WTui_setcallback(pb6,WTUIEVENT_ACTIVATE,define_stars,NULL);

		menu_flags.debug[0] = pb0;
		menu_flags.debug[1] = pb1;
		menu_flags.debug[2] = pb2;
		menu_flags.debug[3] = pb3;
	}

	/*Create the Help menu*/
    cb1 = WTui_newmenupopup(mmenu,"Help",NULL);
	pb0 = WTui_newmenuitem(cb1, "Readme",NULL);
	pb1 = WTui_newmenuitem(cb1, "About",NULL);
	WTui_setcallback(pb0,WTUIEVENT_ACTIVATE,printhelp,NULL);
	WTui_setcallback(pb1,WTUIEVENT_ACTIVATE,about_box,NULL);

}

void wireframe_select(WTui *ui, void *data)
{
	MYmessage("Wireframe on");
	WTuniverse_setrendering( WTRENDER_WIREFRAME);
	WTui_dimitem(menu_flags.render_menus[0],TRUE);
	WTui_dimitem(menu_flags.render_menus[1],FALSE);
	WTui_dimitem(menu_flags.render_menus[2],FALSE);
	WTwindow_setbgrgb(WTuniverse_getwindows(), 50,50,50);
	logo = FALSE;

}

void shade_select(WTui *ui, void *data)
{
	MYmessage("Shading on");
	WTuniverse_setrendering( WTRENDER_SHADED | WTRENDER_GOURAUD);
	WTui_dimitem(menu_flags.render_menus[0],FALSE);
	WTui_dimitem(menu_flags.render_menus[1],TRUE);
	WTui_dimitem(menu_flags.render_menus[2],FALSE);
	WTwindow_setbgrgb(WTuniverse_getwindows(), 0,0,0);
	logo = FALSE;
	draw_stars = TRUE;
	WTnode_enable(starfield, FALSE);

}

void texture_select(WTui *ui, void *data)
{
	MYmessage("Texture on");
	WTuniverse_setrendering( WTRENDER_SHADED | WTRENDER_GOURAUD | WTRENDER_TEXTURED);
	WTui_dimitem(menu_flags.render_menus[0],FALSE);
	WTui_dimitem(menu_flags.render_menus[1],FALSE);
	WTui_dimitem(menu_flags.render_menus[2],TRUE);
	WTwindow_setbgrgb(WTuniverse_getwindows(), 0,0,0);
	logo = TRUE;
	WTnode_enable(starfield, TRUE);
}

void starfield_texture_toggle(WTui *ui, void *data)
{

	if (WTnode_isenabled(starfield))
		WTnode_enable(starfield, FALSE);
	else
		WTnode_enable(starfield, TRUE);
}

void draw_stars_toggle(WTui *ui, void *data)
{

	draw_stars = !draw_stars;
}

void define_stars(WTui *ui, void *data)
{

	stars_first_time = TRUE;
}

void performance_toggle(WTui *ui, void *data)
{

	draw_stats = !draw_stats;
}
