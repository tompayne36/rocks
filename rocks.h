/****************** DEFINES *******************/

#define BOOM_TEXTURE	"screen2.tga"
#define SHIELD_TEXTURE	"screen.tga"
#define ROCK_TEXTURE	"gravel.tga"
#define BACK_TEXTURE1	"my_stars.tga"
#define BACK_TEXTURE2	"hourgls.tga"
#define BACK_TEXTURE3	"m100.tga"
#define BACK_TEXTURE4	"m31.tga"
#define BACK_TEXTURE5	"diskil.tga"

#define NUM_ASTEROIDS 5
#define NUM_LIVES 5
#define UNIVERSE_SIZE 100.0f
#define NUM_STARS 100

#define MAXSOUNDS 6

#ifndef M_PI
#define M_PI        3.14159265358979323846f
#endif

#define VERTEX_NORMALS 0x02
#define VERTEX_COLORS  0x01

#define TEXTURE_POINT	 0x00
#define TEXTURE_BILINEAR 0x01
#define TEXTURE_MIPMAP	 0x02

#define ROCKTYPE_BOX    0
#define ROCKTYPE_SIMPLE 1
#define ROCKTYPE_SPHERE 2
#define ROCKTYPE_DIST   3

#define IFF_OWNSHIP 0
#define IFF_ALIEN   1

#define SHIELDOPACITY 0.25f
#define BOOMOPACITY 1.0f

#define printf WTmessage

#define NETHELLO    99
#define NETREPLY    66
#define MISSILEINFO 10
#define ROCKINFO	11
#define SHIPINFO    12
#define NEWMISSILE  20
#define NEWROCK     21
#define NEWSHIP     22
#define NEWALIEN    23
#define DELMISSILE  30
#define DELROCK     31
#define DELSHIP     32
#define DELALIEN    33


/**************** TYPES *****************/

/* sensor types */
enum sensor_type {
	ST_UNKNOWN,
	ST_BIRD,
	ST_BOOM,
	ST_CRYSTALVR,
	ST_CYBERMAXX2,
	ST_FASTRAK,
	ST_FORMULA,
	ST_GEOBALL,
	ST_GLOVE5DT,
	ST_IGLASSES,
	ST_INSIDETRAK,
	ST_ISOTRAK2,
	ST_JOYSERIAL,
	ST_LOGITECH,
	ST_MOUSE,
	ST_POLHEMUS,
	ST_PRECNAVI,
	ST_REDBARON,
	ST_SPACEBALL,
	ST_SBALLSC,
	ST_SCONTROL,
	SENSOR_TYPES
};

typedef struct{
	FLAG prebuild;
	FLAG wireframe;
	FLAG mode_is_quads;
	FLAG render_modifiers;
	FLAG mode_is_overwrites;
	char texture_mode;
	FLAG measure_running;
	FLAG terrain;
	FLAG ripple,ripple_running;

	int frame_count;
	void (*old_action)(void);

	char vertex_state;
	int current_dim;
	int overwrite_depth;
	long total_polys;

	WTui *render_menus[5];
	WTui *test_menu[4];
	WTui *vertex_menu[2];
	WTui *texture_menu[3];
	WTui *terrain_menu;
	WTui *mode;
	WTui *debug[4];

	WTpath *terrain_path;

	int ripple_count;
}m_flags;

typedef struct {
	WTp3 dir;
	WTp3 ang;
	WTnode *colide;
	WTnodepath *nodepath;
	float mass;
	float timer;
	WTsound *sound;
	int iff;
	int id;
} RockData;

typedef struct {
	WTgeometry *geom;
	float size;
	FLAG makeship;
	WTmtable *mtable;
	int mtableid;
	float index;
} BoomData;


/**************** GLOBALS ***************/

extern WTui *toplevel;

extern WTsensor *sensor;
extern int sensor_type; 
extern int baud;
extern int unit;
extern int port;

extern WTwindow *rear_window;

extern char filename[80];
extern char *shipfilename;
extern char *missilefilename;


extern m_flags menu_flags;

extern FLAG simple_check;

extern FLAG debug_mode;

extern int net_master;
extern FLAG use_network;

extern int display_mode;
extern FLAG texture_flag;
extern int window_mode ;
extern int default_sound_device;
extern FLAG use_gui;
extern char *object_dir;
extern char *sensor_name[SENSOR_TYPES] ;

extern void open_task(WTnode *node);
extern void close_task(WTnode *node);
extern void makeship(WTp3);
extern void toggleview(WTui *, void *);
extern void printhelp(WTui *, void *);
extern void print_performance(WTui *, void *);

/* sensors specified thru command line arguments: -s,-g */
extern FLAG use_spaceball;
extern FLAG use_joystick;
extern FLAG use_geoball;
extern short spaceball_on, joystick_on, geoball_on;
extern WTserialname com[2];

extern FLAG movemouse;		/* view point movement by mouse		*/
extern FLAG hand_open;
extern FLAG grabbed;

extern WTsensor *spaceball, *joystick, *geoball;
extern WTsensor *mouse;
extern WTsensor *sensor;   /* current active sensor */
extern WTviewpoint *outview;
extern WTviewpoint *inview;

/* movable nodes hierarchically assembled to make up the arm */
extern WTnode *myShip;
extern int GameLevel;
extern int GameLives;
extern int GameScore;
extern FLAG paused;

extern WTnode *root;
extern WTnode *stars;
extern WTnode *overnode;

extern WTmotionlink *mouselink, *sensorlink;

extern WTpq initial_pq;

extern WTnode *RockRoot;
extern WTnode *ShipRoot;
extern WTnode *MissileRoot;
extern WTnode *AlienRoot;

extern WTnodepath *myShipPath;

extern int RockType;

extern 	WTnode *starfield;

extern WTmtable *shieldTable;
extern int shieldID;
extern int missilematID;
extern int missile2matID;

extern WTnode *myShipShield;
extern WTnodepath *myShipShieldPath;
extern WTgeometry *myShipShieldGeom;
extern float shieldColors[3];
extern float shieldOpacity[1];

extern float boomColors[3];
extern float boomOpacity[1];

extern int   MakeShock;

extern float alien_insertion_timer;

extern float pitch_delta;
extern float yaw_delta;
extern float shipVelocity;
extern float shipVelocityDelta;

extern float sens;

extern WTp3 zeroPos;
extern WTnode *missile;
extern WTnodepath *missilePath;

extern char screen_message[256];
extern float screen_message_time;

extern WTfont3d *my3DFont;

extern FLAG MoveRocks;
extern FLAG ThreeDVersion;
extern FLAG logo;
extern FLAG cockpitView;
extern FLAG displayText;
extern FLAG detect_rock_col;
extern FLAG draw_stars;
extern FLAG draw_stats;
extern FLAG stars_first_time;

extern WTsounddevice *mySoundDevice;
extern WTsound *BoingSound;
extern WTsound *HummSound;
extern WTsound *ShieldSound;
extern WTsound *GameoverSound;
extern WTsound *FireSound;
extern WTsound *myStartSound;
extern WTsound *myShutDownSound;
extern WTsound *myAlienSound;

extern WTgeometry *holderGeom1, *holderGeom2;


/****************** FUNCTION PROTOTYPES ********/
FLAG MyNetOpen(void);
void MyNetSetInfo(int type, WTnode *node);
void MyNetActions(void);

void load_animated_bits(void);
void make_animated_explosion(WTp3, FLAG);
void new_ship(void);
void scalenode(WTnode *,float );


void gameover(WTui *ui, void *data);
void create_materials( void);
void create_scenegraph( void);
void scan_args(int argc, char *argv[]);	/* scans command line for '-' args */
void actions(void);						/* universe actions fn */
void handle_key(int key);				/* interpret a keypress */
void display_keys(void);
void display_usage(void);
void overlay_2Dfunction(WTwindow *, FLAG);
void overlay_3Dfunction(WTwindow *, FLAG);
void MYmessage(char *);
FLAG setup_sounds(int mySoundDeviceIndex);

WTnode *fire_missile( WTp3 , WTq , int);
void fire_missile0(void);

void makeexplosion(WTp3, FLAG);
WTnode *newrock(WTnode *parent, WTp3 pos, WTp3 dir, WTp3 ang, float size);
WTnode *makenewship(WTnode *, WTp3);

float myrand(float scale);
float deltaT(void);
void make_alien(void);
void zoomall(WTui *ui,void *data);
void zoomship(WTui *ui,void *data);
void togglepause(WTui *ui,void *data);
void closeui(WTui *ui,void *data);
void togglemode(WTui *ui,void *data);
void reset0(WTui *ui,void *data);
void really_exit(WTsound *sound);
void test_exit(WTui *ui,void *data);
void toggle_logo(WTui *ui,void *data);
void togglescore(WTui *ui,void *data);
void toggle_detect_col(WTui *ui,void *data);
void toggle_rock_motion(WTui *ui,void *data);
void render_mode(WTui *ui,void *style);
void texture_mode(WTui *ui,void *menuitem);
void print_readme(WTui *ui,void *menuitem);
void starfield_texture_toggle(WTui *ui,void *data);
void draw_stars_toggle(WTui *ui,void *data);
void performance_toggle(WTui *ui, void *data);
void wireframe_select(WTui *ui, void *data);
void texture_select(WTui *ui, void *data);
void shade_select(WTui *ui, void *data);
void define_stars(WTui *ui, void *data);
FLAG read_settings(char *);

/******* gui functions *********/

static void onfilesopenshipok(WTui *ui, void *data);
static void onfilesopensoundok(WTui *ui, void *data);
static void fileopenship(WTui *pStruct, void *pData);
static void fileopensound(WTui *pStruct, void *pData);
static void fileopentexture(WTui *pStruct, void *pData);
static void sorry(WTui *pStruct, void *pData);
void build_menubar(WTui *parent);

/**** sensor functions ************/

void setup_sensors(WTsensor **);
void sensor_config(WTui *ui,void *data);
void sound_config(WTui *ui,void *data);

void mouse_myupdate(WTsensor *sensor);

/*********** From WTK.C ***********/

#define MAXANIMS 10
#define PAR_SCALE 0.002	/* parallax scaling value */

/* cursor modes */
#define CM_NONE			1
#define CM_ROTATE		2
#define CM_TRANSLATE	3
#define CM_POLYSCALE	4
#define CM_LIGHTS		5
#define CM_SENSITIVITY	6
#define CM_ANGULARRATE	7
#define CM_BACKGROUND	8
#define CM_PARALLAX		9
#define CM_CONVERGENCE	10
#define CM_VIEWANGLE	11
#define CM_HITHERCLIP	12
#define CM_ADJUST_COLOR	13
#define CM_YBLANK		14


/* mouse button modes */
#define BM_NONE			0
#define BM_AUTO_COLOR	1
#define BM_AUTO_TEXTURE	2

/* key menu modes */
#define ME_GLOBAL		1
#define ME_LIGHT		2
#define ME_OBJECT		3
#define ME_OBJECTGLOBAL	4
#define ME_OBJECTNEW	5
#define ME_PATH			6
#define ME_POLY			7
#define ME_POLYTEXTURE	8
#define ME_POLYCOLOR	9
#define ME_SENSOR		10
#define ME_SCENEGRAPH	11
#define ME_RENDER		12
#define ME_VIEWPOINT	13
#define ME_OBJECTTEXTURE 14
#define ME_WINDOW		15


/* color picking variables */
#define R 0
#define G 1
#define B 2


/* sensor usages */
#define SU_UNKNOWN		0	/* not for any specific use */
#define SU_VIEWPOINT	1	/* for controlling the viewpoint */
#define SU_MANIPULATE	2	/* for manipulating objects */
#define SU_TOGGLES		3
/* (If VIEWPOINT and MANIPULATE are both set, the sensor toggles) */

/* application-maintained association of selected objects */
typedef struct selectedobj
{
	WTnode *node;
	struct selectedobj *next;
} selectedobj;



extern void scan_args(int argc, char * argv[]);
extern void scan_arg(char * arg);
extern void parse_sensor(char * arg);

