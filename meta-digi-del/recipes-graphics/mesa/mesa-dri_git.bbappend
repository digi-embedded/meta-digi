PROVIDES = "${@base_contains('DISTRO_FEATURES', 'x11', 'virtual/libgl', '', d)}"
