# Copyright (C) 2013 Digi International.

# Override default bootscript settings from Poky
#
# On an embedded target using the serial terminal as console, the
# 'psplash' utility throws several errors on target's shutdown.
INITSCRIPT_PARAMS = "start 0 S ."
