# Copyright (C) 2017, 2018 Digi International Inc.

# 'ares' and 'threaded-resolver' are mutually exclusive
PACKAGECONFIG:remove:class-target = "threaded-resolver"
PACKAGECONFIG:append:class-target = " ares"
