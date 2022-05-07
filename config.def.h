/* See LICENSE file for copyright and license details. */

/* Allow use of special keys */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int padbar             = 2;        /* padding around bar text */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_black[]       = "#000000";
static const char col_red[]         = "#ff0000";
static const char col_yellow[]      = "#ffff00";
static const char col_white[]       = "#ffffff";

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm]   = { col_gray3, col_gray1,  col_gray2 },
	[SchemeSel]    = { col_gray4, col_cyan,   col_cyan },
	[SchemeWarn]   = { col_black, col_yellow, col_red },
	[SchemeUrgent] = { col_white, col_red,    col_red },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ NULL,       "spterm",   NULL,	      SPTAG(0),	    1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.60; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static int fakefullscreen    = 1;

/* HACK: doing this properly would make it hard to merge upstream commits */
#include "gaps.c"
#include "layouts.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "---",      horizgrid },
	{ "M[]",      deck },
	{ "|M|",      centeredmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },    /* terminator for {next,prev}layout() */
};

/* key definitions */
#define MODKEY Mod1Mask
#define ALTKEY Mod1Mask
#define SUPKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

/* scratchpads */
typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"st", "-n", "spcalc", "-g", "50x20", "-z", "16", "-e", "bc", "-lq", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
	{"spcalc",      spcmd2},
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, togglescratch,  {.ui = 0 } },

	{ MODKEY|ShiftMask,             XK_b,          togglebar,    {0} },
	{ MODKEY,                       XK_j,          focusstack,   {.i = +1 } },
	{ MODKEY,                       XK_k,          focusstack,   {.i = -1 } },
	{ MODKEY,                       XK_i,          incnmaster,   {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,          incnmaster,   {.i = -1 } },
	{ MODKEY,                       XK_h,          setmfact,     {.f = -0.01} },
	{ MODKEY,                       XK_l,          setmfact,     {.f = +0.01} },
	{ MODKEY,                       XK_w,          zoom,         {0} },
	{ MODKEY,                       XK_Tab,        view,         {0} },
	{ MODKEY|ShiftMask,             XK_c,          killclient,   {0} },

	/* Layouts */
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} }, /* tiled */
	{ MODKEY,                       XK_y,      setlayout,      {.v = &layouts[1]} }, /* monocole */
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[2]} }, /* bstack */
	{ MODKEY|ShiftMask,             XK_u,      setlayout,      {.v = &layouts[3]} }, /* bstack h */
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[4]} }, /* horizgrid */
	{ MODKEY|ShiftMask,             XK_r,      setlayout,      {.v = &layouts[5]} }, /* deck */
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[6]} }, /* centermaster */
	{ MODKEY|ShiftMask,             XK_y,      setlayout,      {.v = &layouts[7]} }, /* float */
	{ MODKEY,                       XK_space,  nextlayout,     {0} },
	{ MODKEY|ControlMask,           XK_space,  prevlayout,     {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefakefull, {0} },

	/* gaps */
	{ MODKEY,                       XK_z,      incrgaps,       {.i = +2 } },
	{ MODKEY|ShiftMask,             XK_z,      incrgaps,       {.i = -2 } },
	{ MODKEY,                       XK_s,      incrigaps,      {.i = +2 } },
	{ MODKEY|ShiftMask,             XK_s,      incrigaps,      {.i = -2 } },
	{ MODKEY,                       XK_x,      incrogaps,      {.i = +2 } },
	{ MODKEY|ShiftMask,             XK_x,      incrogaps,      {.i = -2 } },
	{ SUPKEY,                       XK_0,      togglegaps,     {0} },
	{ SUPKEY|ShiftMask,             XK_0,      defaultgaps,    {0} },

	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
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

