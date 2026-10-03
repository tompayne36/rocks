                            
						SpaceRocks
						----------

INTRODUCTION

SpaceRocks is an example appliation using Sense8's popular WorldToolKit 
development library. It not only demonstrates a wide variety of WTK's
features, but is actually fun to play!

WTK-NG BUILDS

The revived native macOS version can be built with:

	make                 build the native rocks-ng executable
	make run             run the native executable

WorldToolKit features that are highlighted in this application include:

	Real-time 3D graphics
	Utilizes hardware acceleration
	Billboards
	Animated textures
	Hierarchical Scene Graph
	Transparency
	Dynamic material definition
	Switch Nodes
	Collision detection
	Selectable texture filtering
	Multiple rendering modes
	Spatialized 3D sound
	Cross Platform (WinNT/Win95/SGI/Sun/DEC)
	Performance independant motion
	Cascading Menus
	Dialog Boxes
	Performance monitor
	Object Oriented design
	Per Object Data
	2D/3D text
	3D points
	2D lines
	3D lines
	User (re)defined senor model
	Stereo viewing options
	Support for multiple sound devices 
	Support for popular VR peripherals
	Models modifiable at runtime
	Sensors modifiable at runtime
	Sound modifiable at runtime
	Special Effects
	Network enabled
	Multiple simultaneous viewpoints
	Dynamic window creation
	Polygon-level manipulation

DEVELOPMENT

This game was developed over the course of multiple weekends/late evenings.
In total, about 40 hours have gone into it's development. It contains about
2500 lines of C code. Source code will be made available as soon as it has 
been "cleaned-up".

Models were built mostly with Sense8's WorldUp modeller, a little of 
MultiGen's ModelGen, and an ASCII text editor.

OBJECT OF THE GAME

The object of the game is simple: Use youe missiles to clear out all 
of the space rocks (otherwise known as asteroids) in your section of
the universe. Also, avoid the "annoying space alien" who appears from 
time to time spaying random, but deadly, missile fire.

In network play (coming soon), help your fellow rock hunters clear the 
area. (Missiles are configured with IFF - Identify Friend/Foe. Don't 
worry about shooting your buddy...but watch out for the alien!)

POINTS

100 - Space Rocks
500 - Annoying space alien

OTHER STUFF

SpaceRocks has a 2D mode as well as a 3D. While the objects are still
rendered in 3D, in this mode all motion is confined to the XY plane. This 
makes play more enjoyable from the "outside" viewpoint and play is more
like the classic "Asteroids" game.

KEYBOARD COMMANDS

 'f'   forward (thrust)
 'v'   viewpoint toggle (inside/outside)
 'R'   toggle auxiliary viewpoint
 '+/-' adjust sensor sensativity
 '3'   toggle 2D/3D motion 
 'b'   space brakes 
 'p'   pause (toggle)
 'w'   switch to wireframe 
 's'   switch to shaded 
 't'   switch to textured 
 'u'   display usage 
 'E'   increase rotational velocities  
 'e'   decrease rotational velocities  
 'd'   toggle display (score) 
 '</>' adjust parallax (for 3D viewing modes) 
 '?'   this display 
 'q'   Quit.
  
  ARROW keys - pitch/roll 
  SPACE - fire missile
 
 ----MOUSE---- 
 'left'   fire missile 
 'middle' space brake 
 'right'  accelerate 
 
 ----DEBUG---- 
 'z' zoom all 
 'r' reset ship position/viewpoint 
 'n' zoom view closer to ship 
 'A' make an alien ship 
 'C' toggle colision detection method
 'T' stop motion of Space Rocks 
 'S' redefine 3D star positions/turn stars on 
 'P' print scene graph 
 

COMMAND-LINE OPTIONS
  
Usage: rocks [options]  

 Options: 
 -a(1,2)[1..n]		for Ascension Bird.  For a flock, specify which unit. 
 -b(1,2)			for Fake Space Boom on port 1,2 
 -c					use CrystalEyes Stereoview 
 -d					use CrystalEyes Stereoview (force V-split)
 -S					use two window stereo 
 -W					use stereo in window (SGI only) 
 -R					use Red/Blue stereo 
 -M					Sound: use WinMM 
 -K					Sound: use DiamondWare 
 -G					Sound: use SGI 
 -V					Sound: use VSI 
 -C					sound: use CRE 
 -D					sound: use DirectSound 
 -e(1,2)			for Virtual i-O i-glasses! head tracker on port 1,2 
 -f(1,2)[1..4]		for a Polhemus Fastrak. Specify unit number after port. 
 -g(1,2)			for Geoball on port 1,2 
 -h(1,2)			for SpaceWare Spaceball SPACECONTROLLER on port 1,2 				
 -i(1,2)			for Insidetrak (unit 1 or 2) 
 -j(1,2)			for joyserial 1,2 (uses gameports, not serial ports) 
 -k(1,2)			for Logitech Space Control device on port 1,2: 
						(on SGI, use spaceball driver (-s) for this device)  
 -l(1,2)			for Logitech tracker on port 1,2 
 -m					for mouse (default) 
 -n(1,2)			for Precision Navigation tracker on port 1,2 				
 -p(1,2)			for Polhemus Isotrak on port 1,2 
 -q(1,2)			for 5DT Glove on port 1,2 
 -r(1,2)			for Logitech Red Baron on port 1,2 
 -s(1,2)			for Spaceball on port 1,2 
 -t(1,2)[1..2]		for Polhemus IsotrakII on port 1,2 
 -u(1,2) 			for Thrustmaster Formula T2 driving console (unit 1 or 2)\n");
 -v(1,2)			for CrystalEyes VR sensor on port 1,2 
 -x(1,2) 			for Cybermaxx2 on port 1,2 		



ACKNOWLEDGEMENTS

Thanks to the folks at NASA for building the Hubble Space Telescope 
which collected the images used in the backgound texture. These images
were obtained from the NASA web site.

Thanks to the people at Atari who came up with the original Asteroids.
which provided the inspiration for this game...and endless hours of
entertainment.

Special thanks to the WorldToolkit team at Sense8:
	Sumant Ravulakollu
	Colin Sharp
	JD Cole
	Makund Bhakta
	Rajeev Sikka

****************************************************************
*                                                              *
* Revision History                                             *
*                                                              *
****************************************************************

11-20-96

- Fixed 3D points (starfield)
- Fixed 2D texture overlay code (still need better logo)

12-07-96

- Improved 3D starfield algorithm
- Sound Configuration dialog added
-- enables user to change sound device at run time
- Rock type selectable
-- utilized switch node(s) to select 4 types of rocks
- added "About" box

12-08-96

- improved flight model (no more "gimble lock")
- added annoying alien
- improved logo texture
- added support for various stereo views

12-09-96

- added command line selectable sound device
- ship resets to 0,0,0 after explosion 

12-10-96

- added V-Split force (for CrystalEyes on Intergraph)
--> still doesn't work...WTK bug
- added "otherview" window
- adjusted UI for unix 
- added "About..." graphic
- improved colision detection between rocks
- new ships start at zero velocity

12-11-96

- -X option for non-texture startup 

12-15-96

- added pause option
- finally incorporated "end of game" & reset

12-17-96 

- added new sounds
- added animated explosion (textured)
- set initial viewpoint inside ship
- smaller rocks faster

12-20-96

- added "find home" line (3D)
- universe moves with ship
- changed fire/thrust keys

1-15-97

- changed texture_replace to changetexture in animation
- cached textures for explosion animation sequence
- added texture memory statistic

****************************************************************
*                                                              *
* Still to do                                                  *
*                                                              *
****************************************************************

- "Explode" ship polygons upon shield failure
- Read/Write configuration files
- Make sensor code (config dialog, etc.) reusable
- Design/implement more generalized sensor model
- Add LOD's
- Network interface(s)
-- WTK v2.1 style
-- DIS (MaK VR-Link)
-- WorldServer
- Port to WTK Object System
- HUD targeting elements (lock on / aim point)
- game over condition(s)/game reset
- random onscreen keyboard hints
- add prebuild option
- develop "benchmark"/hardware profiler
- user selectable missile models
- port to MAC
- hyperspace
- smart bomb/expanding "blast field"
- help gui
- help file

****************************************************************
*                                                              *
* Information                                                  *
*                                                              *
****************************************************************

For information on WorldToolKit, contact:

	Sense8 Corporation
	100 Shoreline Hwy, #282
	Mill Valley, CA 94941
	415-331-6318
	http://www.sense8.com
	info@sense8.com

For comments on SpaceRocks, contact:

	Tom Payne
	tomp@sense8.com
