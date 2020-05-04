SECO_FIRMWARE_NAME              ?= "UNSUPPORTED"
SECO_FIRMWARE_NAME_mx8qm         = "mx8qmb0-ahab-container.img"
SECO_FIRMWARE_NAME_mx8qxp        = "mx8qxb0-ahab-container.img"
SECO_FIRMWARE_NAME_mx8qxpc0      = "mx8qxc0-ahab-container.img"
SECO_FIRMWARE_NAME_mx8phantomdxl = "mx8qxb0-ahab-container.img"
SECO_FIRMWARE_NAME_mx8dxl        = "mx8dxla0-ahab-container.img"

# For the ccimx8x, we generate images for both the B0 and the C0 revisions of
# the i.MX8X silicon, so deploy both versions of the firmware.
SECO_FIRMWARE_NAME_ccimx8x       = "mx8qx*0-ahab-container.img"
