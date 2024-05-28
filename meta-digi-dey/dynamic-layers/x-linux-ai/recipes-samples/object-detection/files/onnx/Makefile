OPENCV_PKGCONFIG?="opencv4"
SYSROOT?=""
TARGET_BIN = onnx_object_detection

CXXFLAGS += -Wall $(shell pkg-config --cflags gtk+-3.0 $(OPENCV_PKGCONFIG) gstreamer-plugins-base-1.0 gstreamer-wayland-1.0)
CXXFLAGS += -std=c++17
CXXFLAGS += -I$(SYSROOT)/usr/include/rapidjson
CXXFLAGS += -I$(SYSROOT)/usr/include/onnxruntime
CXXFLAGS += -I$(SYSROOT)/usr/include/onnxruntime/core/session

LDFLAGS  = $(shell pkg-config --libs gtk+-3.0 gstreamer-plugins-base-1.0 gstreamer-wayland-1.0)
LDFLAGS += -lpthread -ldl -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lonnxruntime

SRCS = onnx_object_detection.cc
OBJS = $(SRCS:.cc=.o)

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJS): $(SRCS)
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -rf $(OBJS) $(TARGET_BIN)
