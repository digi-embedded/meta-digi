CFLAGS  += -O2 -Ilib/include
VPATH    = lib/fslutil lib/glu3
BINARIES = es20_example es11_example

all: $(BINARIES)

es11_example: fslutil.o glu3.o
	$(CC) $(CFLAGS) $^ -lm -lEGL -lGLESv1_CM $@.c -o $@

es20_example: fslutil.o
	$(CC) $(CFLAGS) $^ -lm -lEGL -lGLESv2 $@.c -o $@

install: $(BINARIES)
	-mkdir -p $(DEST_DIR)/opt/dey-examples-opengles
	install -m 0755 $^ $(DEST_DIR)/opt/dey-examples-opengles
	install -m 0644 texture.bmp $(DEST_DIR)/opt/dey-examples-opengles
