st4b is a simplistic statusbar generator for use with [lemonbar](https://github.com/LemonBoy/bar) on OpenBSD.  It was created specifically for my own use,  and it is published simply for archival purposes.  Please use as you see fit.

Installation from source
========================

```
make
PREFIX=$HOME make install
```

Then add to `.xsession` or your window manager config script the following line:

```
$HOME/bin/st4b | /usr/local/bin/lemonbar
```
