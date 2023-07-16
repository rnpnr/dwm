/* See LICENSE file for copyright and license details. */

/* Allow use of special keys */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int padbar             = 8;        /* adjusts the bars padding */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int gappih    = 0;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 0;       /* vert inner gap between windows */
static const unsigned int gappoh    = 0;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 0;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
//static const char *fonts[]          = { "みかちゃん:style=Regular:size=16",
static const char *fonts[]          = { "あんずもじ:style=Regular:size=20",
					"Go Mono:style=bold:size=16",
					"Source Han Sans JP:style=Medium:size=16",
					"Source Han Sans CN:style=Medium:size=16",
					"Source Han Sans KR:style=Medium:size=16" };
static const char dmenufont[]       = "Go Mono:style=bold:size=16";
static const char col_gray1[]       = "#000000";
static const char col_gray2[]       = "#1b1b1b";
static const char col_gray3[]       = "#666666";
static const char col_gray4[]       = "#999999";
static const char col_cyan[]        = "#444444";
static const char col_red[]         = "#e6404e";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm]   = { col_gray3, col_gray1, col_gray1 },
	[SchemeSel]    = { col_gray4, col_gray1, col_cyan },
	[SchemeWarn]   = { col_gray3, col_red,   col_gray1 },
	[SchemeUrgent] = { col_red,   col_gray1, col_gray1 },
};

/* tagging */
static const char *tags[] = { "壹", "貳", "參", "肆", "伍", "陸", "漆", "捌", "玖" };
#define TAG_WEB (1 << 0)

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class        instance                  title  tags mask     isfloating   monitor */
	{ "Chromium",   0,                        0,     TAG_WEB,      0,           -1 },
	{ "firefox",    0,                        0,     TAG_WEB,      0,           -1 },
	{ 0,            0,                        "gf2", 1 << 2,       0,           -1 },
	{ "mgba-sdl",   0,                        0,     0,            1,           -1 },
	{ "Matplotlib", 0,                        0,     0,            1,           -1 },
	{ 0,            "spterm",                 0,     SPTAG(0),     1,           -1 },
	{ 0,            "spcal",                  0,     SPTAG(1),     1,           -1 },
	{ 0,            "spkeys",                 0,     SPTAG(2),     1,           -1 },
	{ 0,            "sun-awt-X11-XFramePeer", 0,     0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.60; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */
static int fakefullscreen    = 1;

#include "gaps.c"
#include "layouts.c"
#include "custom.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "M[]",      deck },
	{ "---",      horizgrid },
	{ "===",      bstackhoriz },
	{ "TTT",      bstack },
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

#define BOOKMARKS "xdotool type $(grep -v -e '#' -e '^$' ~/doc/urls | dmenu -i -l 20 | cut -f 1 -d ' ')"
#define EMOJI "xdotool type $(cat ~/.local/share/emoji | dmenu -i -l 20 | cut -d ' ' -f 1)"
#define RSS "sfeed_dmenu blogs soft videos | plumb; pkill -HUP status"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]  = { "dmenu_run", "-i", "-m", dmenumon, "-fn", dmenufont, NULL };
static const char *termcmd[]   = { "vtgl", NULL };
static const char *sterm[]     = { "vtgl", "-z", "36", NULL };
static const char *browser[]   = { "browser", NULL };
static const char *dpcon[]     = { "displaycon", NULL };
static const char *mpdnext[]   = { "mpdnext", NULL };
static const char *mpdprev[]   = { "mpdprev", NULL };
static const char *mpdtog[]    = { "mpdtoggle", NULL };
static const char *music[]     = { "st", "-z", "32", "ncmpcpp", NULL };
static const char *pass[]      = { "passmenu", NULL };
static const char *scrotum[]   = { "screenshot", NULL };
static const char *search[]    = { "surfraw_menu", NULL };
static const char *sysact[]    = { "sysact", NULL };
static const char *tpadtg[]    = { "trackpad", "toggle", NULL };
static const char *voldn[]     = { "volume", "-2%", NULL };
static const char *voltg[]     = { "volume", "toggle", NULL };
static const char *volup[]     = { "volume", "+2%", NULL };
static const char *boomer[]    = {"boomer", 0};

/* scratchpads */
const char *spcmd0[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd1[] = {"st", "-n", "spcal", "-z", "24", "calcurse", NULL };
const char *spcmd2[] = {"st", "-n", "spkeys", "-g", "80x50", "/bin/sh", "-c", "man ~/.config/keybinds.1", NULL };
struct {
	const char *name;
	const void *cmd;
} scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd0},
	{"spcal",       spcmd1},
	{"spkeys",      spcmd2},
};

static struct backlight_arg backlight_up   = {.backlight = "dp_aux_backlight", .delta = +2 };
static struct backlight_arg backlight_down = {.backlight = "dp_aux_backlight", .delta = -2 };

static const Key keys[] = {
	/* modifier                     key                             function        argument */
	{ 0,                            XF86XK_MonBrightnessUp,         backlight,      {.v = &backlight_up} },
	{ 0,                            XF86XK_MonBrightnessDown,       backlight,      {.v = &backlight_down} },
	{ 0,                            XF86XK_AudioRaiseVolume,        spawn,          {.v = volup } },
	{ 0,                            XF86XK_AudioLowerVolume,        spawn,          {.v = voldn } },
	{ 0,                            XF86XK_AudioMute,               spawn,          {.v = voltg } },
	{ 0,				XF86XK_AudioPlay,		spawn,		{.v = mpdtog } },
	{ 0,				XF86XK_AudioNext,		spawn,		{.v = mpdnext } },
	{ 0,				XF86XK_AudioPrev,		spawn,		{.v = mpdprev } },
	{ MODKEY|ShiftMask,             XK_w,                           spawn,          {.v = browser } },
	{ MODKEY|ShiftMask,             XK_m,                           spawn,          {.v = music } },
	{ MODKEY|ShiftMask,             XK_s,                           spawn,          {.v = search } },
	{ MODKEY,                       XK_BackSpace,                   spawn,          {.v = sysact } },
	{ MODKEY,                       XK_F2,                          spawn,          {.v = dpcon } },
	{ MODKEY,                       XK_F12,                         spawn,          {.v = tpadtg } },
	{ MODKEY|ShiftMask,             XK_Print,                       spawn,          {.v = scrotum } },
	{ MODKEY,                       XK_Print,                       spawn,          {.v = scrotum } },
	{ MODKEY|ShiftMask,             XK_p,                           spawn,          {.v = scrotum } },
	{ MODKEY,                       XK_p,                           spawn,          {.v = pass } },
	{ MODKEY,                       XK_b,                           spawn,          SHCMD(BOOKMARKS) },
	{ MODKEY,                       XK_e,                           spawn,          SHCMD(EMOJI) },
	{ MODKEY,                       XK_n,                           spawn,          SHCMD(RSS) },
	{ MODKEY|ShiftMask,             XK_z,                           spawn,          {.v = boomer} },
	{ MODKEY,                       XK_d,      			spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, 			spawn,          {.v = termcmd } },
	{ SUPKEY,                       XK_Return, 			spawn,          {.v = sterm } },
	{ MODKEY|ShiftMask,             XK_Return,                      togglescratch,  {.ui = 0 } },
	{ MODKEY,                       XK_backslash,                   togglescratch,  {.ui = 1 } },
	{ MODKEY,                       XK_F1,                          togglescratch,  {.ui = 2 } },

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
	{ MODKEY,                       XK_space,  nextlayout,     {0} },
	{ MODKEY|ControlMask,           XK_space,  prevlayout,     {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefakefull, {0} },

	/* gaps */
	{ SUPKEY,                       XK_z,      incrgaps,       {.i = +2 } },
	{ SUPKEY|ShiftMask,             XK_z,      incrgaps,       {.i = -2 } },
	{ SUPKEY,                       XK_s,      incrigaps,      {.i = +2 } },
	{ SUPKEY|ShiftMask,             XK_s,      incrigaps,      {.i = -2 } },
	{ SUPKEY,                       XK_x,      incrogaps,      {.i = +2 } },
	{ SUPKEY|ShiftMask,             XK_x,      incrogaps,      {.i = -2 } },
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
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

