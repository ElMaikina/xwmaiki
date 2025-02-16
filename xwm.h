/* See LICENSE file for copyright and license details. */

/* typedefs */

/*
 * Struct that manages key presses and commands
 * */
typedef struct {
    unsigned int mod;			/* ModKey */
    xcb_keysym_t keysym;		/* KeySymbol */
    void (*func)(char **com);	/* Function to invoke */
    char **com;					/* Command parameters */
} Key;

/*
 * Event and input handler
 * */
typedef struct {
    uint32_t request;						/* Request Thread */
    void (*func)(xcb_generic_event_t *ev);	/* XCB Event */
} handler_func_t;

/* convert keycode to keysym and back */
static xcb_keycode_t *xcb_get_keycodes(xcb_keysym_t keysym);
static xcb_keysym_t    xcb_get_keysym(xcb_keycode_t keycode);

/* user defined command actions */
static void killclient(char **com);
static void spawn(char **com);
static void closewm(char **com);
static void moveandresizeclient(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
static void fullclient(char **com);
static void maximizeclient(char **com);
static void tileclientleft(char **com);
static void tileclientright(char **com);
static void tileclienttop(char **com);
static void tileclientbottom(char **com);

/* window behavior */
static void setFocus(xcb_drawable_t window);
static void setFocusColor(xcb_drawable_t window, int focus);

/* event hander actions */
static int eventHandler(void);
static void handleMotionNotify(xcb_generic_event_t *ev);
static void handleEnterNotify(xcb_generic_event_t *ev);
static void handleDestroyNotify(xcb_generic_event_t *ev);
static void handleButtonPress(xcb_generic_event_t *ev);
static void handleButtonRelease(xcb_generic_event_t *ev);
static void handleKeyPress(xcb_generic_event_t *ev);
static void handleMapRequest(xcb_generic_event_t *ev);
static void handleFocusIn(xcb_generic_event_t *ev);
static void handleFocusOut(xcb_generic_event_t *ev);
static handler_func_t handler_funs[] = {
    { XCB_MOTION_NOTIFY,  handleMotionNotify },
    { XCB_ENTER_NOTIFY,   handleEnterNotify },
    { XCB_DESTROY_NOTIFY, handleDestroyNotify },
    { XCB_BUTTON_PRESS,   handleButtonPress },
    { XCB_BUTTON_RELEASE, handleButtonRelease },
    { XCB_KEY_PRESS,      handleKeyPress },
    { XCB_MAP_REQUEST,    handleMapRequest },
    { XCB_FOCUS_IN,       handleFocusIn },
    { XCB_FOCUS_OUT,      handleFocusOut },
    { XCB_NONE,           NULL }
};

/* intialize */
static void setup(void);

/* error handling & misc. */
static int die(char *errstr);
static int strcmp_c(char *str1, char *str2);
