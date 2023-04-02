/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

#define SESSION_FILE "~/.local/src/dwm/.tmp/dwm-session"

/* commands */
static const char *cmd_toggle_mute[] = { "pactl", "set-sink-mute",   "0", "toggle", NULL };
static const char *cmd_vol_raise[]   = { "pactl", "set-sink-volume", "0", "+5%",    NULL };
static const char *cmd_vol_lower[]   = { "pactl", "set-sink-volume", "0", "-5%",    NULL };
static const char *cmd_vol_raise_fine[]   = { "pactl", "set-sink-volume", "0", "+1%",    NULL };
static const char *cmd_vol_lower_fine[]   = { "pactl", "set-sink-volume", "0", "-1%",    NULL };

static const char *cmd_mon_brightness_raise[] = { "sudo", "light", "-A", "5.00", NULL };
static const char *cmd_mon_brightness_lower[] = { "sudo", "light", "-U", "5.00", NULL };

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 5;        /* gaps between windows */
static const unsigned int snap      = 16;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int focusonwheel       = 1;
static const char *fonts[]          = { "SF Mono:style=Bold:size=12", "fontawesome:size=12" };
static const char dmenufont[]       = "SF Mono:style=Bold:size=12";
static const char col_fg_nor[]       = "#D8DEE9"; /* Unselected foreground */
static const char col_bg_nor[]       = "#2E3440"; /* Unselected background */
static const char col_br_nor[]       = "#3B4252"; /* Unselected border */
static const char col_fg_sel[]       = "#ECEFF4"; /* Selected foreground */
static const char col_bg_sel[]       = "#4C566A"; /* Selected background */
static const char col_br_sel[]       = "#88C0D0"; /* Selected border */
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_fg_nor, col_bg_nor, col_br_nor },
	[SchemeSel]  = { col_fg_sel, col_bg_sel, col_br_sel },
};

/* tagging */
static const char *tags[] = { 
	"",  /* Home */ 
	"",  /* Updates */
	"",  /* Terminal */ 
	"",  /* Programming */ 
	"",  /* Godot */ 
	"",  /* Writing */
	"",  /* Audio */ 
	"",  /* Video */ 
	"",  /* Web */
	"",  /* Games */ 
	"",  /* Comms */
};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int rmaster     = 0;    /* rotation of master client */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_bg_nor, "-nf", col_fg_nor, "-sb", col_br_sel, "-sf", col_fg_sel, NULL };
static const char *termcmd[]  = { "kitty", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_j,      rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      rotatestack,    {.i = -1 } },
	{ MODKEY,                       XK_r,      togglermaster,  {0} },

	/* Volume Controls */
	{ 0,                            XF86XK_AudioMute,        spawn, {.v = cmd_toggle_mute } },
	{ 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = cmd_vol_raise   } },
	{ 0,                            XF86XK_AudioLowerVolume, spawn, {.v = cmd_vol_lower   } },
	{ 0|ShiftMask,                  XF86XK_AudioRaiseVolume, spawn, {.v = cmd_vol_raise_fine } },
	{ 0|ShiftMask,                  XF86XK_AudioLowerVolume, spawn, {.v = cmd_vol_lower_fine } },

	/* Backlight Controls */
	{ 0,             		XF86XK_MonBrightnessUp,   spawn, {.v = cmd_mon_brightness_raise } },
	{ 0,             		XF86XK_MonBrightnessDown, spawn, {.v = cmd_mon_brightness_lower } },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

