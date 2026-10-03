/**************** GLOBALS ***************/

WTui *toplevel;

WTsensor *sensor;

FLAG debug_mode = FALSE;

char filename[80];
char *shipfilename;
char *missilefilename = NULL;

m_flags menu_flags;

int display_mode = WTDISPLAY_DEFAULT;
int window_mode = WTWINDOW_DEFAULT;
int default_sound_device = -1;

FLAG use_gui = TRUE;
char *object_dir = NULL;
char *sensor_name[SENSOR_TYPES] = {
	"Unknown",
	"Bird",
	"Boom",
	"CrystalEyesVR",
	"Cybermaxx2",
	"Fastrak",
	"Formula",
	"Geoball",
	"Glove5DT",
	"I-glasses",
	"Insidetrak",
	"IsotrakII",
	"Joyserial",
	"Logitech",
	"Mouse",
	"Polhemus",
	"Precnavi",
	"Red Baron",
	"Spaceball",
	"SpaceballSC",
	"SpaceControl"
};


float alien_insertion_timer;

/* sensors specified thru command line arguments: -s,-g */
FLAG use_spaceball=FALSE;
FLAG use_joystick=FALSE;
FLAG use_geoball=FALSE;
short spaceball_on, joystick_on, geoball_on;
WTserialname com[2] = {SERIAL1, SERIAL2};

FLAG movemouse = TRUE;		/* view point movement by mouse		*/
FLAG hand_open = FALSE;
FLAG grabbed = FALSE;

WTsensor *spaceball=NULL, *joystick=NULL, *geoball=NULL;
WTsensor *mouse=NULL;
WTsensor *sensor=NULL;   /* current active sensor */
WTviewpoint *outview = NULL;
WTviewpoint *inview = NULL;

/* movable nodes hierarchically assembled to make up the arm */
WTnode *myShip;
int GameLevel = 0;
int GameLives = NUM_LIVES;
int GameScore = 0;

WTnode *root;
WTnode *stars;
WTmotionlink *mouselink, *sensorlink;

WTpq initial_pq;

WTnode *RockRoot = NULL;
WTnode *ShipRoot = NULL;
WTnode *MissileRoot = NULL;
WTnode *AlienRoot = NULL;
WTnodepath *myShipPath = NULL;
WTnode *overnode = NULL;
WTnode *cached_alien = NULL;

WTp3 alien_pos;
FLAG show_alien = TRUE;
FLAG draw_alien_line = FALSE;

WTwindow *rear_window = NULL;

int RockType = ROCKTYPE_SPHERE;

WTnode *starfield;
int net_master = -1;
FLAG use_network = FALSE;

FLAG paused = FALSE;

FLAG simple_check = TRUE;
FLAG texture_flag = TRUE;

WTmtable *shieldTable;
int shieldID;
int missilematID;
int missile2matID;

WTnode *myShipShield;
WTnodepath *myShipShieldPath;
WTgeometry *myShipShieldGeom;

float shieldColors[3] = { 0.0f, 1.0f, 0.0f};
float shieldOpacity[1] = { SHIELDOPACITY };
float ShieldRadius;
float InitShieldRadius;
#define INITSHIELDRADIUS 15.0f

float missileColors[3] = { 1.0f, 1.0f, 0.0f};
float missileOpacity[1] = { SHIELDOPACITY };
float missile2Colors[3] = { 1.0f, 0.0f, 0.0f};
float missile2Opacity[1] = { SHIELDOPACITY };


float boomColors[3] = { 1.0f, 0.5f, 0.0f};
float boomOpacity[1] = { BOOMOPACITY };

float flicker_time = 0.0f;

float pitch_delta = 5.0f;
float yaw_delta   = 5.0f;
float shipVelocity = 0.0f;
float shipVelocityDelta = 1.0f;

int   MakeShock = 0;

float sens = 2.5f;

WTp3 zeroPos = {0.0f, 0.0f, 0.0f};

WTnode *missile;
WTnodepath *missilePath;

char screen_message[256];
float screen_message_time;

WTfont3d *my3DFont;

FLAG MoveRocks = TRUE;
FLAG ThreeDVersion = TRUE;
FLAG logo = TRUE;
FLAG cockpitView = FALSE;
FLAG displayText = TRUE;
FLAG detect_rock_col = TRUE;
FLAG draw_stars = FALSE;
FLAG draw_stats = FALSE;
FLAG stars_first_time = TRUE;

WTsounddevice *mySoundDevice = NULL;
WTsound *BoingSound = NULL;
WTsound *HummSound = NULL;
WTsound *ShieldSound = NULL;
WTsound *GameoverSound = NULL;
WTsound *FireSound = NULL;
WTsound *myStartSound = NULL;
WTsound *myShutDownSound = NULL;
WTsound *myAlienSound = NULL;

WTgeometry *holderGeom1, *holderGeom2;

