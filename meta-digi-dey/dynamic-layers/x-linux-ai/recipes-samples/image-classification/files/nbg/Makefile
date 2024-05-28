OPENCV_PKGCONFIG?="opencv4"
SYSROOT?=""
TARGET_BIN = nbg_image_classification

CXXFLAGS += -Wall $(shell pkg-config --cflags gtk+-3.0 $(OPENCV_PKGCONFIG) gstreamer-plugins-base-1.0 gstreamer-wayland-1.0)
CXXFLAGS += -std=c++17

LDFLAGS  = $(shell pkg-config --libs gtk+-3.0 gstreamer-plugins-base-1.0 gstreamer-wayland-1.0)
LDFLAGS += -lpthread -ldl -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
LDFLAGS += -L$(SYSROOT)/usr/lib -ljpeg -lovxlib -lOpenVX -lOpenVXU
LDFLAGS += -Wl,--no-as-needed

ifeq ($(NEW_GST_WAYLAND_API), 1)
	CXXFLAGS += -DNEW_GST_WAYLAND_API
endif

SRCS = nbg_image_classification.cc vnn_utils.cc
OBJS = $(SRCS:.cc=.o)

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CXX)  -o $@ $^ $(LDFLAGS)

$(OBJS): $(SRCS)
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -rf $(OBJS) $(TARGET_BIN)
