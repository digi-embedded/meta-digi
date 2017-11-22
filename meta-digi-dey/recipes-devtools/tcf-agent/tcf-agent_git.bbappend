# Copyright (C) 2017 Digi International Inc.

# tcf-agent falls back to '/bin/sh' if 'bash' is not available, so don't
# depend on bash at runtime.
RDEPENDS_${PN}_remove = "bash"
