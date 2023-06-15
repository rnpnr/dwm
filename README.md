dwm - dynamic window manager
============================
dwm is an extremely fast, small, and dynamic window manager for X.

This is my personal fork of [dwm](https://dwm.suckless.org) from suckless.

I put it in a repo so that it is easier for me to transfer around to
other systems. This has the extra side effect of allowing others to see
and take ideas from it. I will not merge changes to suit other peoples
needs. Follow the suckless ideology and modify your own build.

NOTE: I regularly force push this branch to rebase it on master so don't
expect it to be stable.

Changes
-------

The following features have been added/changed:
* unity/single translation unit build
* Fake fullscreen: "fullscreen" a window but keep it constrained to its area
* Fullscreen any window (monocle mode still has borders and the top bar)
* Control bar padding via config.h
* From [hiltjo's branch](https://git.codemadness.org/dwm/log.html)
	- functions for cycling layouts
* Patches taken from the wiki:
	- bottomstack layout
	- deck layout
	- grid layout
	- scratchpad
	- statuscolors (slightly modified)
	- vanity gaps

Configuration
-------------

`config.h` is included in this branch and represents my normal settings.

`config.def.h` should contain all keybind changes and corresponds with
the manpage.

Why use suckless?
-----------------

Suckless programs are by designed to be simple and easy to
understand. This means that you can easily change and modify core
features to suit your needs. Using a packaged version of these programs
is pointless, to truly understand why they are so great you need to use
them to solve your problems.
