CFLAGS  += -O2 -Icommon/inc -DLINUX
VPATH    = common/src
BINARIES = es20_example es11_example

ifeq ($(EGL_FLAVOUR),fb)
$(info BUILDING FOR FB)
CFLAGS += -DEGL_API_FB
else ifeq ($(EGL_FLAVOUR),x11)
$(info BUILDING FOR X11)
CFLAGS += -DEGL_USE_X11
X11LIBS = -lX11
else
$(error Platform not recognized)
endif

all: $(BINARIES)

es11_example: es11_example.o fsl_egl.o fslutil.o glu3.o
	$(CC) $(LDFLAGS) $^ -lm -lEGL -lGLESv1_CM $(X11LIBS) -o $@

es20_example: es20_example.o fsl_egl.o fslutil.o
	$(CC) $(LDFLAGS) $^ -lm -lEGL -lGLESv2 $(X11LIBS) -o $@

install: $(BINARIES)
	-mkdir -p $(DEST_DIR)/opt/dey-examples-opengles
	install -m 0755 $^ $(DEST_DIR)/opt/dey-examples-opengles
	install -m 0644 texture.bmp $(DEST_DIR)/opt/dey-examples-opengles

clean:
	-rm -f *.o $(BINARIES)
