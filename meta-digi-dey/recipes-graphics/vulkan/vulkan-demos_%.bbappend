
DEPENDS_remove = "vulkan"
DEPENDS_append = " vulkan-headers vulkan-loader"

# The vulkan-validationlayers package is necessary for the demos to work
RDEPENDS_${PN} = "vulkan-validationlayers"
