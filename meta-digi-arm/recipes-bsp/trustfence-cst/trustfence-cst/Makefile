TARGET = linux64/cst
LIBS = -lfrontend -lcrypto
CFLAGS += -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = code/back_end/src/*.c
HEADERS = code/back_end/hdr
LIBS_PATH = linux64/lib

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -L $(LIBS_PATH) $(LIBS) -I $(HEADERS) -o $@

clean:
	-rm -f *.o $(TARGET)
