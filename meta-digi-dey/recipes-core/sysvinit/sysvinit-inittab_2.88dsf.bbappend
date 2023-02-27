# Copyright (C) 2016 Digi International.

python __anonymous () {
    if (d.getVar("TRUSTFENCE_CONSOLE_DISABLE") == "1"):
        d.setVar("SERIAL_CONSOLES", "")
}
