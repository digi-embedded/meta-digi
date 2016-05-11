# Copyright (C) 2016 Digi International.

python __anonymous () {
    if d.getVar("TRUSTFENCE_CONSOLE_DISABLE", True):
        d.setVar("SERIAL_CONSOLES", "")
}
