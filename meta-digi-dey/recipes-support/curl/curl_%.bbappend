# Copyright (C) 2017, 2018 Digi International Inc.

# 'ares' and 'threaded-resolver' are mutually exclusive
PACKAGECONFIG_remove_class-target = "threaded-resolver"
PACKAGECONFIG_append_class-target = " ares"
