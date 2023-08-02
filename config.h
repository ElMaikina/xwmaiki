/* DEFAULT KEYS
 * The following are the possible mask definitions.  Note
 * that these definitions may vary between X implementations
 * and keyboard models.
 *     XCB_MOD_MASK_1       -> Alt_L Alt_R Meta_L
 *     XCB_MOD_MASK_2       -> Num_Lock
 *     XCB_MOD_MASK_3       -> ISO_Level3_Shift
 *     XCB_MOD_MASK_4       -> Super_L Super_R SuperL Hyper_L
 *     XCB_MOD_MASK_5       -> ISO_Level5_Shifta
 *     XCB_MOD_MASK_SHIFT
 *     XCB_MOD_MASK_CONTROL
 *     XCB_MOD_MASK_LOCK
 *     XCB_MOD_MASK_ANY
 */

#define MOD1                   XCB_MOD_MASK_4
#define MOD2                   XCB_MOD_MASK_SHIFT

/* DEFAULT WINDOW PROPERTIES
 * The following parameters can be used to change existing and new
 * window behavior.
 */

/* Main default values */
#define WINDOW_WIDTH          	600      /* pixels */
#define WINDOW_HEIGHT          	400      /* pixels */
#define WINDOW_MIN_WIDTH       	24       /* pixels */
#define WINDOW_MIN_HEIGHT      	24       /* pixels */
#define BORDER_WIDTH           	1        /* 0 = no border effect */
#define PANEL_HEIGHT           	24       /* pixels */
#define TILE_BORDER           	270      /* pixels */

#define BORDER_COLOR_FOCUSED           	0x005577       /* RGB */
#define BORDER_COLOR_UNFOCUSED          0x757575       /* RGB */

/* Panel and launcher values */
static char main_font[]			= "Century Gothic:size=8";
static char highlight_color[]	= "#ffffff";
static char panel_color[]		= "#000000";
static char accent_color[]		= "#005577";

/* ALIASED COMMANDS
 * Each space delimited argument should be passed as an additional
 * value to the character pointer array. For example, to run
 * "foo -a bar", the array would be "{ "foo", "-a", "bar", NULL }".
 * Since execvp() is a variadic functions, each argument pointer must
 * be terminated by a NULL pointer.
 */

/* The command for launching the terminal, in this case st */
static char *termcmd[] = { "st", NULL };

/* The command for launching the kitty */
static char *kittycmd[] = { "kitty", NULL };

/* The command for launching the file manger, in this case thunar */
static char *filescmd[] = { "thunar", NULL };

/* The command for launching the app launcher, in this case dmenu */
static char *menucmd[] = { 
	"dmenu_run", 
	"-fn", main_font, 
	"-nb", panel_color, 
	"-nf", highlight_color, 
	"-sb", accent_color, 
	"-sf", highlight_color, 
	"-b", 
	NULL 
};

/* The command for launchin the web browser, in this case firefox */
static char *browcmd[] = { "firefox", NULL };

/* KEY ALIASES
 * In general, one shortcut key should exist per alias. For more key
 * definitions, refer to the keysymdef.h and XF86keysym.h headers.
 */

static Key keys[] = {
	{ MOD1,     	0x0062,		spawn,						browcmd },	/* 0x0062 = XK_b */
	{ MOD1,     	0xff0d, 	spawn,      				termcmd },	/* 0xff0d = XK_Enter */
	{ MOD1|MOD2,	0xff0d, 	spawn,      				kittycmd },	/* 0xff0d = XK_Enter */
	{ MOD1,			0x6e, 		spawn,      				filescmd },	/* 0xff0d = XK_Enter */
	{ MOD1,			0x0020, 	spawn,      				menucmd },	/* 0x0020 = XK_space */
	{ MOD1|MOD2,	0xff51, 	tileclientleft,				NULL },		/* 0x0066 = XK_Left */
	{ MOD1|MOD2,	0xff53, 	tileclientright,			NULL },		/* 0x0066 = XK_Right */
	{ MOD1|MOD2,	0xff52, 	tileclienttop,				NULL },		/* 0x0066 = XK_Up */
	{ MOD1|MOD2,	0xff54, 	tileclientbottom,			NULL },		/* 0x0066 = XK_Up */
	{ MOD1,			0xff52, 	maximizeclient,				NULL },		/* 0x0066 = XK_Up */
	{ MOD1,			0x0066, 	fullclient,					NULL },		/* 0x0066 = XK_f */
	{ MOD1,			0x0071, 	killclient,					NULL },		/* 0x0071 = XK_q */
	{ MOD1|MOD2,	0x0071, 	closewm,					NULL }		/* 0x0071 = XK_q */
};
