#include <stdio.h>
#include <wt.h>
#include "rocks.h"

FILE *stream, *stream2;


/*
void set_defaults( void )
{

	sensor_type = ST_MOUSE;
	baud = 99;
	port = 99;
	unit = 99;
	buttons = ST_MOUSE;
	sprintf(shipfilename,			"ship2.nff");
	sprintf(shieldtexturefilename,	"screen.tga");
	sprintf(explosionfilename,		"screen2.nff");
	sprintf(alienfilename,			"station.nff");

	texture_mode = TRUE;
	antialias_mode = TRUE;
	wireframe_mode = FALSE;
	perspectivetexture_mode = TRUE;
	lighting_mode = TRUE;
	shading_mode = TRUE;
	yblank = 0;
	backgroundr = 100;
	backgroundg = 100;
	backgroundb = 100;
    display_mode = WTDISPLAY_DEFAULT;
	window_mode = WTWINDOW_NOBORDER;

}

*/

FLAG read_settings(char *filename)
{
	char token[128], equals[10], value[256];

	/* Open for read (will fail if file "planets.ini" does not exist) */
	if( (stream  = fopen( filename, "r" )) == NULL )
	{
		printf( "The file '%s' was not found...using defaults\n", filename );
		return FALSE;
	}
	else
		printf( "Using '%s' data file...\n", filename );

	fscanf(stream, "%s %s %s", &token, &equals, &value);


//	switch(get_token(token))
//	{
//		case
//
//		default
//	}

	/* Close stream */
	fclose( stream );
	return TRUE;

}


/*


myShip.Model = ship2.nff
myShip.BackgroundSound = humm.wav
myShip.ShieldSound = richochet.wav

Alien.Model = station.wrl
Alien.Sound = alien2.wav

Explosion.AnimationBase = bang
Explosion.NumFrames = 57
Explosion.Sound = expl.wav

Missile.Model = <default>
Missile.Sound = fire.wav

Universe.Model = uni.nff
Universe.TexturedStars = FALSE
Universe.PointStars = TRUE
Universe.RenderMode = TEXTURED
#Universe.RenderMode = SHADED
#Universe.RenderMode = WIREFRAME
Universe.FilterMode = TRILINEAR
#Universe.FilterMode = BILINEAR
#Universe.FilterMode = POINT
Universe.SoundDevice = DIAMONDWARE
#Universe.SoundDevice = WINMM
#Universe.SoundDevice = DIRECTSOUND
#Universe.SoundDevice = CRE
#Universe.SoundDevice = VSI
#Universe.SoundDevice = SGI
Universe.Sensor = MOUSE
Universe.DisplayMode = WINDOW
#Universe.DisplayMode = NOBORDER
Universe.UseGUI = TRUE
Universe.DebugMode = FALSE;
Universe.StartupSound = startup.wav
Universe.ShutdownSound = shutdown.wav
Universe.GameOverSound = gameover.wav


*/