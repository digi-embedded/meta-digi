#!/bin/sh

/etc/flutter-demo-init start

# Keep one foreground process alive; the demo wrapper backgrounds flutter-pi.
exec sleep infinity
