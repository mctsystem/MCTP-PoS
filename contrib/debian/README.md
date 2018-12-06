
Debian
====================
This directory contains files used to package mctd/mct-qt
for Debian-based Linux systems. If you compile mctd/mct-qt yourself, there are some useful files here.

## mct: URI support ##


mct-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install mct-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your mct-qt binary to `/usr/bin`
and the `../../share/pixmaps/mct128.png` to `/usr/share/pixmaps`

mct-qt.protocol (KDE)

