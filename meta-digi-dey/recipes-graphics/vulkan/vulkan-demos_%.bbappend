
DEPENDS_remove = "vulkan"
DEPENDS_append = " vulkan-headers vulkan-loader"

# Digi: The vulkan-validationlayers package is necessary for the demos to work
RDEPENDS_${PN} = "vulkan-validationlayers"
