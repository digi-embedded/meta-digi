OPENCV_PKGCONFIG?="opencv4"
SYSROOT?=""
EDGETPU?=""
ARCHITECTURE?=""
TARGET_BIN = tflite_object_detection

CXXFLAGS += -Wall $(shell pkg-config --cflags gtk+-3.0 $(OPENCV_PKGCONFIG) gstreamer-plugins-base-1.0 gstreamer-wayland-1.0)
CXXFLAGS += -std=c++17
CXXFLAGS += -I$(SYSROOT)/usr/include/tensorflow/lite/flatbuffers/include
CXXFLAGS += -I$(SYSROOT)/usr/include/tensorflow/lite/abseil-cpp
CXXFLAGS += -I$(SYSROOT)/usr/include/rapidjson

LDFLAGS  = $(shell pkg-config --libs gtk+-3.0 gstreamer-plugins-base-1.0 gstreamer-wayland-1.0)
LDFLAGS += -lpthread -ldl -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -ltensorflow-lite
ifneq (,$(findstring stm32mp2_npu,$(ARCHITECTURE)))
	CXXFLAGS += -DVSI_OP
	LDFLAGS += -lvx_custom_op
endif

ifneq (,$(findstring TRUE,$(EDGETPU)))
	CXXFLAGS += -DEDGETPU
	LDFLAGS += -ledgetpu
endif

SRCS = tflite_object_detection.cc
OBJS = $(SRCS:.cc=.o)

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS): $(SRCS)
	$(CXX) $(CXXFLAGS) -c $^

clean:
	rm -rf $(OBJS) $(TARGET_BIN)
