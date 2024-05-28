#!/usr/bin/python3
#
# Author: Maxence Guilhin <maxence.guilhin@st.com> for STMicroelectronics.
# Co-Author: Youssef Khemakhem <youssef.khemakhem@st.com> for STMicroelectronics.
# Copyright (c) 2020 STMicroelectronics. All rights reserved.
#
# This software component is licensed by ST under BSD 3-Clause license,
# the "License"; You may not use this file except in compliance with the
# License. You may obtain a copy of the License at:
#
# http://www.opensource.org/licenses/BSD-3-Clause

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('Gst', '1.0')
from gi.repository import Gtk
from gi.repository import Gdk
from gi.repository import GLib
from gi.repository import GdkPixbuf
from gi.repository import Gst

import numpy as np
import argparse
import signal
import os
import random
import json
import subprocess
import re
import os.path
from os import path
import cv2
from PIL import Image
import onnxruntime as ort
from timeit import default_timer as timer

#init gstreamer
Gst.init(None)
Gst.init_check(None)
#init gtk
Gtk.init(None)
Gtk.init_check(None)


RESOURCES_DIRECTORY = os.path.abspath(os.path.dirname(__file__)) + "/../../resources/"

class NeuralNetwork:
    """
    Class that handles Neural Network inference
    """

    def __init__(self, model_file, label_file, input_mean, input_std,maximum_detection):
        """
        :param model_path: .onnx model to be executed
        :param label_file:  name of file containing labels
        :param input_mean: input_mean
        :param input_std: input standard deviation
        """

        if args.num_threads == None :
            if os.cpu_count() <= 1:
                self.number_threads = 1
            else :
                self.number_threads = os.cpu_count()
        else :
           self.number_threads = int(args.num_threads)

        self._selected_delegate = None

        def load_labels(filename):
            my_labels = []
            input_file = open(filename, 'r')
            for l in input_file:
                my_labels.append(l.strip())
            return my_labels

        def create_session(model) -> ort.InferenceSession:
            providers = ['CPUExecutionProvider']
            options = ort.SessionOptions()
            options.intra_op_num_threads = self.number_threads
            options.execution_mode = ort.ExecutionMode.ORT_SEQUENTIAL
            return ort.InferenceSession(model, sess_options=options, providers=providers)

        self._model_file = model_file
        self._label_file = label_file
        self._input_mean = input_mean
        self._input_std = input_std
        self._floating_model = False

        self.maximum_detection = maximum_detection
        self.nn_result_locations = np.reshape(np.zeros((args.maximum_detection, 4)), (1, 10, 4))
        self.nn_result_classes = np.reshape(np.zeros(args.maximum_detection), (1, 10))
        self.nn_result_scores = np.reshape(np.zeros(args.maximum_detection), (1, 10))
        print("NN model used : ",self._model_file)
        print("Number threads used : ",self.number_threads)

        self._labels = load_labels(self._label_file)

        self.session = create_session(self._model_file)


    def get_labels(self):
        return self._labels


    def get_img_size(self):
        """
        :return: size of NN input image size
        """
        input_shape = self.session.get_inputs()[0].shape
        print("input shape", input_shape)
        if input_shape[1] == 3:
            self.channel_first = True
            C = input_shape[1]
            H = input_shape[2]
            W = input_shape[3]
        else:
            self.channel_first = False
            H = input_shape[1]
            W = input_shape[2]
            C = input_shape[3]
        print("H,W,C", H, W, C)
        return (int(H),
                int(W),
                int(C))


    def launch_inference(self, img):
        """
        This method launches inference using the invoke call
        :param img: the image to be inferred
        """
        # check the type of the input tensor
        image = np.expand_dims(img, axis=0)
        element_type = self.session.get_inputs()[0].type
        if element_type == "tensor(float)":
            self._floating_model = True
            print("Floating point Onnx Model")
        if self._floating_model:
            input_data = (np.float32(image) - self._input_mean) / self._input_std
        else :
            input_data = image
        ort_input = {self.session.get_inputs()[0].name: input_data}
        self.results = self.session.run(None, ort_input)

    def get_results(self):
        # display output results

        preds = self.results
        locations = preds[0]
        classes = preds[1]
        scores = preds[2]
        return (locations, classes, scores)

    def get_object_location_y0(self, idx):
        if idx < self.maximum_detection:
            return round(float(self.nn_result_locations[0][idx][0]), 9)
        return 0

    def get_object_location_x0(self, idx):
        if idx < self.maximum_detection:
            return round(float(self.nn_result_locations[0][idx][1]), 9)
        return 0

    def get_object_location_y1(self, idx):
        if idx < self.maximum_detection:
            return round(float(self.nn_result_locations[0][idx][2]), 9)
        return 0

    def get_object_location_x1(self, idx):
        if idx < self.maximum_detection:
            return round(float(self.nn_result_locations[0][idx][3]), 9)
        return 0

    def get_label(self, idx):
        labels = self.get_labels()
        if idx < self.maximum_detection:
            return labels[int(self.nn_result_classes[0][idx])]
        return 0

class GstWidget(Gtk.Box):
    """
    Class that handles Gstreamer pipeline using gtkwaylandsink and appsink
    """
    def __init__(self, app, nn):
         super().__init__()
         # connect the gtkwidget with the realize callback
         self.connect('realize', self._on_realize)
         self.instant_fps = 0
         self.app = app
         self.nn = nn
         self.cpt_frame = 0

    def _on_realize(self, widget):
            """
            creation of the gstreamer pipeline when gstwidget is created
            """
            # gstreamer pipeline creation
            self.pipeline = Gst.Pipeline()

            # creation of the source v4l2src
            self.v4lsrc1 = Gst.ElementFactory.make("v4l2src", "source")
            video_device = "/dev/" + str(self.app.video_device)
            self.v4lsrc1.set_property("device", video_device)
            self.v4lsrc1.set_property("io-mode", 0)

            #creation of the v4l2src caps
            caps = str(self.app.camera_caps) + ", framerate=" + str(args.framerate)+ "/1"
            print("Camera pipeline configuration : ",caps)
            camera1caps = Gst.Caps.from_string(caps)
            self.camerafilter1 = Gst.ElementFactory.make("capsfilter", "filter1")
            self.camerafilter1.set_property("caps", camera1caps)

            # creation of the videoconvert elements
            self.videoformatconverter1 = Gst.ElementFactory.make("videoconvert", "video_convert1")
            self.videoformatconverter2 = Gst.ElementFactory.make("videoconvert", "video_convert2")

            self.tee = Gst.ElementFactory.make("tee", "tee")

            # creation and configuration of the queue elements
            self.queue1 = Gst.ElementFactory.make("queue", "queue-1")
            self.queue2 = Gst.ElementFactory.make("queue", "queue-2")
            self.queue1.set_property("max-size-buffers", 1)
            self.queue1.set_property("leaky", 2)
            self.queue2.set_property("max-size-buffers", 1)
            self.queue2.set_property("leaky", 2)

            # creation and configuration of the appsink element
            self.appsink = Gst.ElementFactory.make("appsink", "appsink")
            nn_caps = "video/x-raw, format = RGB, width=" + str(self.app.nn_input_width) + ",height=" + str(self.app.nn_input_height)
            nncaps = Gst.Caps.from_string(nn_caps)
            self.appsink.set_property("caps", nncaps)
            self.appsink.set_property("emit-signals", True)
            self.appsink.set_property("sync", False)
            self.appsink.set_property("max-buffers", 1)
            self.appsink.set_property("drop", True)
            self.appsink.connect("new-sample", self.new_sample)

            # creation of the gtkwaylandsink element to handle the gstreamer video stream
            properties_names=["drm-device"]
            properties_values=[" "]
            self.gtkwaylandsink = Gst.ElementFactory.make_with_properties("gtkwaylandsink",properties_names,properties_values)
            self.pack_start(self.gtkwaylandsink.props.widget, True, True, 0)
            self.gtkwaylandsink.props.widget.show()

            # creation and configuration of the fpsdisplaysink element to measure display fps
            self.fps_disp_sink = Gst.ElementFactory.make("fpsdisplaysink", "fpsmeasure1")
            self.fps_disp_sink.set_property("signal-fps-measurements", True)
            self.fps_disp_sink.set_property("fps-update-interval", 2000)
            self.fps_disp_sink.set_property("text-overlay", False)
            self.fps_disp_sink.set_property("video-sink", self.gtkwaylandsink)
            self.fps_disp_sink.connect("fps-measurements",self.get_fps_display)

            # creation of the video rate and video scale elements
            self.video_rate = Gst.ElementFactory.make("videorate", "video-rate")
            self.video_scale = Gst.ElementFactory.make("videoscale", "video-scale")

            # Add all elements to the pipeline
            self.pipeline.add(self.v4lsrc1)
            self.pipeline.add(self.camerafilter1)
            self.pipeline.add(self.videoformatconverter1)
            self.pipeline.add(self.videoformatconverter2)
            self.pipeline.add(self.tee)
            self.pipeline.add(self.queue1)
            self.pipeline.add(self.queue2)
            self.pipeline.add(self.appsink)
            self.pipeline.add(self.fps_disp_sink)
            self.pipeline.add(self.video_rate)
            self.pipeline.add(self.video_scale)

            # linking elements together
            #                              -> queue 1 -> videoconvert -> fpsdisplaysink
            # v4l2src -> video rate -> tee
            #                              -> queue 2 -> videoconvert -> video scale -> appsink
            self.v4lsrc1.link(self.video_rate)
            self.video_rate.link(self.camerafilter1)
            self.camerafilter1.link(self.tee)
            self.queue1.link(self.videoformatconverter1)
            self.videoformatconverter1.link(self.fps_disp_sink)
            self.queue2.link(self.videoformatconverter2)
            self.videoformatconverter2.link(self.video_scale)
            self.video_scale.link(self.appsink)
            self.tee.link(self.queue1)
            self.tee.link(self.queue2)

            # set pipeline playing mode
            self.pipeline.set_state(Gst.State.PLAYING)
            # getting pipeline bus
            self.bus = self.pipeline.get_bus()
            self.bus.add_signal_watch()
            self.bus.connect('message::error', self.msg_error_cb)
            self.bus.connect('message::eos', self.msg_eos_cb)
            self.bus.connect('message::info', self.msg_info_cb)
            self.bus.connect('message::application', self.msg_application_cb)
            self.bus.connect('message::state-changed', self.msg_state_changed_cb)


    def msg_eos_cb(self, bus, message):
        print('eos message -> {}'.format(message))

    def msg_info_cb(self, bus, message):
        print('info message -> {}'.format(message))

    def msg_error_cb(self, bus, message):
        print('error message -> {}'.format(message.parse_error()))

    def msg_state_changed_cb(self, bus, message):
        oldstate,newstate,pending = message.parse_state_changed()
        if (oldstate == Gst.State.NULL) and (newstate == Gst.State.READY):
            Gst.debug_bin_to_dot_file(self.pipeline, Gst.DebugGraphDetails.ALL,"pipeline_py_NULL_READY")

    def msg_application_cb(self, bus, message):
        if message.get_structure().get_name() == 'inference-done':
            self.app.update_ui()

    def update_isp_config(self):
        if self.cpt_frame == 0:
            isp_file = "/usr/local/demo/application/camera/bin/isp"
            isp_config = "/usr/local/demo/application/camera/bin/isp -w > /dev/null"
            if os.path.exists(isp_file) and self.app.dcmipp_sensor=="imx335" :
                subprocess.run(isp_config,shell=True)
        return True

    def gst_to_opencv(self,sample):
        """
        conversion of the gstreamer frame buffer into numpy array
        """
        buf = sample.get_buffer()
        caps = sample.get_caps()
        #get gstreamer buffer size
        buffer_size = buf.get_size()
        #determine the shape of the numpy array
        number_of_column = caps.get_structure(0).get_value('width')
        number_of_lines = caps.get_structure(0).get_value('height')
        channels = 3
        #buffer size without padding
        expected_buff_size = number_of_column * number_of_lines * channels
        #byte added by the padding
        extra_bytes = buffer_size - expected_buff_size
        extra_offset_per_line = int(extra_bytes/number_of_lines)
        # number of bytes to pass from a line to another
        line_stride = number_of_column * channels
        # pixel_stride : number of bytes to pass from a pixel to another
        # pixel_stride = number of bits per pixel / number of bits in one byte
        # in RBG888 case (24/8)
        pixel_stride = 3
        # number of bytes to pass from a channel to another
        channel_stride = 1
        #stride for each channels line / pixels / channel
        strides = (line_stride+extra_offset_per_line,pixel_stride,channel_stride)
        arr = np.ndarray(
            (number_of_lines,
             number_of_column,
             channels),
            strides=strides,
            buffer=buf.extract_dup(0, buf.get_size()),
            dtype=np.uint8)
        return arr

    def new_sample(self,*data):
        """
        recover video frame from appsink
        and run inference
        """
        global image_arr
        sample = self.appsink.emit("pull-sample")
        arr = self.gst_to_opencv(sample)
        if arr is not None :
            self.update_isp_config()
            self.cpt_frame += 1
            if self.cpt_frame == 30:
                self.cpt_frame = 0
            start_time = timer()
            self.nn.launch_inference(arr)
            stop_time = timer()
            self.app.nn_inference_time = stop_time - start_time
            self.app.nn_inference_fps = (1000/(self.app.nn_inference_time*1000))
            self.app.nn.nn_result_locations[:, :, :], self.app.nn.nn_result_classes[:, :], self.app.nn.nn_result_scores[:, :] = self.nn.get_results()
            struc = Gst.Structure.new_empty("inference-done")
            msg = Gst.Message.new_application(None, struc)
            self.bus.post(msg)
        return Gst.FlowReturn.OK

    def get_fps_display(self,fpsdisplaysink,fps,droprate,avgfps):
        """
        measure and recover display fps
        """
        self.instant_fps = fps
        return self.instant_fps

class MainWindow(Gtk.Window):
    """
    This class handles all the functions necessary
    to display video stream in GTK GUI or still
    pictures using OpenCVS
    """

    def __init__(self,args,app):
        """
        Setup instances of class and shared variables
        useful for the application
        """
        Gtk.Window.__init__(self)
        self.app = app
        self.main_ui_creation(args)

    def set_ui_param(self):
        """
        Setup all the UI parameter depending
        on the screen size
        """
        if self.app.window_height > self.app.window_width :
            window_constraint = self.app.window_width
        else :
            window_constraint = self.app.window_height

        self.ui_cairo_font_size = 23
        self.ui_cairo_font_size_label = 37
        self.ui_icon_exit_width = '50'
        self.ui_icon_exit_height = '50'
        self.ui_icon_st_width = '130'
        self.ui_icon_st_height = '160'
        if window_constraint <= 272:
               # Display 480x272
               self.ui_cairo_font_size = 11
               self.ui_cairo_font_size_label = 18
               self.ui_icon_exit_width = '25'
               self.ui_icon_exit_height = '25'
               self.ui_icon_st_width = '42'
               self.ui_icon_st_height = '52'
        elif window_constraint <= 600:
               #Display 800x480
               #Display 1024x600
               self.ui_cairo_font_size = 16
               self.ui_cairo_font_size_label = 29
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '65'
               self.ui_icon_st_height = '80'
        elif window_constraint <= 720:
               #Display 1280x720
               self.ui_cairo_font_size = 23
               self.ui_cairo_font_size_label = 38
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'
        elif window_constraint <= 1080:
               #Display 1920x1080
               self.ui_cairo_font_size = 33
               self.ui_cairo_font_size_label = 48
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'

    def main_ui_creation(self,args):
        """
        Setup the Gtk UI of the main window
        """
        # remove the title bar
        self.set_decorated(False)

        self.first_drawing_call = True
        GdkDisplay = Gdk.Display.get_default()
        monitor = Gdk.Display.get_monitor(GdkDisplay, 0)
        workarea = Gdk.Monitor.get_workarea(monitor)

        GdkScreen = Gdk.Screen.get_default()
        provider = Gtk.CssProvider()
        css_path = RESOURCES_DIRECTORY + "Default.css"
        self.set_name("main_window")
        provider.load_from_path(css_path)
        Gtk.StyleContext.add_provider_for_screen(GdkScreen, provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
        self.maximize()
        self.screen_width = workarea.width
        self.screen_height = workarea.height

        self.set_position(Gtk.WindowPosition.CENTER)
        self.connect('destroy', Gtk.main_quit)
        self.set_ui_param()
        # setup info_box containing inference results and ST_logo which is a
        # "next inference" button in still picture mode
        if self.app.enable_camera_preview == True:
            # camera preview mode
            self.info_box = Gtk.VBox()
            self.info_box.set_name("gui_main_stbox")
            self.st_icon_path = RESOURCES_DIRECTORY + 'onnx_st_icon_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
            self.st_icon = Gtk.Image.new_from_file(self.st_icon_path)
            self.st_icon_event = Gtk.EventBox()
            self.st_icon_event.add(self.st_icon)
            self.info_box.pack_start(self.st_icon_event,False,False,2)
            self.inf_time = Gtk.Label()
            self.inf_time.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.inf_time,False,False,2)
            info_sstr = "  disp.fps :     " + "\n" + "  inf.fps :     " + "\n" + "  inf.time :     " + "\n"
            self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.ui_cairo_font_size,info_sstr))
        else :
            # still picture mode
            self.info_box = Gtk.VBox()
            self.info_box.set_name("gui_main_stbox")
            self.st_icon_path = RESOURCES_DIRECTORY + 'onnx_st_icon_next_inference_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
            self.st_icon = Gtk.Image.new_from_file(self.st_icon_path)
            self.st_icon_event = Gtk.EventBox()
            self.st_icon_event.add(self.st_icon)
            self.info_box.pack_start(self.st_icon_event,False,False,20)
            self.inf_time = Gtk.Label()
            self.inf_time.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.inf_time,False,False,2)
            info_sstr = "  inf.fps :     " + "\n" + "  inf.time :     " + "\n"
            self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.ui_cairo_font_size,info_sstr))

        # setup video box containing gst stream in camera preview mode
        # and a openCV picture in still picture mode
        # An overlay is used to keep a gtk drawing area on top of the video stream
        self.video_box = Gtk.HBox()
        self.video_box.set_name("gui_main_video")
        if self.app.enable_camera_preview == True:
            # camera preview => gst stream
            self.video_widget = self.app.gst_widget
            self.video_widget.set_app_paintable(True)
            self.video_box.pack_start(self.video_widget, True, True, 0)
        else :
            # still picture => openCV picture
            self.image = Gtk.Image()
            self.video_box.pack_start(self.image, True, True, 0)
        # setup the exit box which contains the exit button
        self.exit_box = Gtk.VBox()
        self.exit_box.set_name("gui_main_exit")
        self.exit_icon_path = RESOURCES_DIRECTORY + 'exit_' + self.ui_icon_exit_width + 'x' + self.ui_icon_exit_height + '.png'
        self.exit_icon = Gtk.Image.new_from_file(self.exit_icon_path)
        self.exit_icon_event = Gtk.EventBox()
        self.exit_icon_event.add(self.exit_icon)
        self.exit_box.pack_start(self.exit_icon_event,False,False,2)

        # setup main box which group the three previous boxes
        self.main_box =  Gtk.HBox()
        self.exit_box.set_name("gui_main")
        self.main_box.pack_start(self.info_box,False,False,0)
        self.main_box.pack_start(self.video_box,True,True,0)
        self.main_box.pack_start(self.exit_box,False,False,0)
        self.add(self.main_box)
        return True

    def update_frame(self, frame):
        """
        update frame in still picture mode
        """
        img = Image.fromarray(frame)
        data = img.tobytes()
        data = GLib.Bytes.new(data)
        pixbuf = GdkPixbuf.Pixbuf.new_from_bytes(data,
                                                 GdkPixbuf.Colorspace.RGB,
                                                 False,
                                                 8,
                                                 frame.shape[1],
                                                 frame.shape[0],
                                                 frame.shape[2] * frame.shape[1])
        self.image.set_from_pixbuf(pixbuf.copy())

class OverlayWindow(Gtk.Window):
    """
    This class handles all the functions necessary
    to display overlayed information on top of the
    video stream and in side information boxes of
    the GUI
    """
    def __init__(self,args,app):
        """
        Setup instances of class and shared variables
        usefull for the application
        """
        Gtk.Window.__init__(self)
        self.app = app
        self.overlay_ui_creation(args)

    def exit_icon_cb(self,eventbox, event):
        """
        Exit callback to close application
        """
        self.destroy()
        Gtk.main_quit()

    def update_label_still(self, label, inference_time):
        """
        update inference results in still picture mode
        """
        str_inference_time = str("{0:0.1f}".format(inference_time))

        self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%sms\n</b></span>" % (self.ui_cairo_font_size,str_inference_time))
        self.app.label_to_display = label

    def set_ui_param(self):
        """
        Setup all the UI parameter depending
        on the screen size
        """
        if self.app.window_height > self.app.window_width :
            window_constraint = self.app.window_width
        else :
            window_constraint = self.app.window_height

        self.ui_cairo_font_size = 23
        self.ui_cairo_font_size_label = 37
        self.ui_icon_exit_width = '50'
        self.ui_icon_exit_height = '50'
        self.ui_icon_st_width = '130'
        self.ui_icon_st_height = '160'
        if window_constraint <= 272:
               # Display 480x272
               self.ui_cairo_font_size = 11
               self.ui_cairo_font_size_label = 18
               self.ui_icon_exit_width = '25'
               self.ui_icon_exit_height = '25'
               self.ui_icon_st_width = '42'
               self.ui_icon_st_height = '52'
        elif window_constraint <= 600:
               #Display 800x480
               #Display 1024x600
               self.ui_cairo_font_size = 16
               self.ui_cairo_font_size_label = 29
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '65'
               self.ui_icon_st_height = '80'
        elif window_constraint <= 720:
               #Display 1280x720
               self.ui_cairo_font_size = 23
               self.ui_cairo_font_size_label = 38
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'
        elif window_constraint <= 1080:
               #Display 1920x1080
               self.ui_cairo_font_size = 33
               self.ui_cairo_font_size_label = 48
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'

    def overlay_ui_creation(self,args):
        """
        Setup the Gtk UI of the overlay window
        """
        # remove the title bar
        self.set_decorated(False)

        self.first_drawing_call = True
        GdkDisplay = Gdk.Display.get_default()
        monitor = Gdk.Display.get_monitor(GdkDisplay, 0)
        workarea = Gdk.Monitor.get_workarea(monitor)

        GdkScreen = Gdk.Screen.get_default()
        provider = Gtk.CssProvider()
        css_path = RESOURCES_DIRECTORY + "Default.css"
        self.set_name("overlay_window")
        provider.load_from_path(css_path)
        Gtk.StyleContext.add_provider_for_screen(GdkScreen, provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
        self.maximize()
        self.screen_width = workarea.width
        self.screen_height = workarea.height

        self.set_position(Gtk.WindowPosition.CENTER)
        self.connect('destroy', Gtk.main_quit)
        self.set_ui_param()

        # setup info_box containing inference results and ST_logo which is a
        # "next inference" button in still picture mode
        if self.app.enable_camera_preview == True:
            # camera preview mode
            self.info_box = Gtk.VBox()
            self.info_box.set_name("gui_overlay_stbox")
            self.st_icon_path = RESOURCES_DIRECTORY + 'onnx_st_icon_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
            self.st_icon = Gtk.Image.new_from_file(self.st_icon_path)
            self.st_icon_event = Gtk.EventBox()
            self.st_icon_event.add(self.st_icon)
            self.info_box.pack_start(self.st_icon_event,False,False,2)
            self.inf_time = Gtk.Label()
            self.inf_time.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.inf_time,False,False,2)
            info_sstr = "  disp.fps :     " + "\n" + "  inf.fps :     " + "\n" + "  inf.time :     " + "\n"
            self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.ui_cairo_font_size,info_sstr))
        else :
            # still picture mode
            self.info_box = Gtk.VBox()
            self.info_box.set_name("gui_overlay_stbox")
            self.st_icon_path = RESOURCES_DIRECTORY + 'onnx_st_icon_next_inference_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
            self.st_icon = Gtk.Image.new_from_file(self.st_icon_path)
            self.st_icon_event = Gtk.EventBox()
            self.st_icon_event.add(self.st_icon)
            self.st_icon_event.connect("button_press_event",self.still_picture)
            self.info_box.pack_start(self.st_icon_event,False,False,20)
            self.inf_time = Gtk.Label()
            self.inf_time.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.inf_time,False,False,2)
            info_sstr = "  inf.fps :     " + "\n" + "  inf.time :     " + "\n"
            self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.ui_cairo_font_size,info_sstr))

        # setup video box containing a transparent drawing area
        # to draw over the video stream
        self.video_box = Gtk.HBox()
        self.video_box.set_name("gui_overlay_video")
        self.video_box.set_app_paintable(True)
        self.drawing_area = Gtk.DrawingArea()
        self.drawing_area.connect("draw", self.drawing)
        self.drawing_area.set_name("overlay_draw")
        self.drawing_area.set_app_paintable(True)
        self.video_box.pack_start(self.drawing_area, True, True, 0)

        # setup the exit box which contains the exit button
        self.exit_box = Gtk.VBox()
        self.exit_box.set_name("gui_overlay_exit")
        self.exit_icon_path = RESOURCES_DIRECTORY + 'exit_' + self.ui_icon_exit_width + 'x' + self.ui_icon_exit_height + '.png'
        self.exit_icon = Gtk.Image.new_from_file(self.exit_icon_path)
        self.exit_icon_event = Gtk.EventBox()
        self.exit_icon_event.add(self.exit_icon)
        self.exit_icon_event.connect("button_press_event",self.exit_icon_cb)
        self.exit_box.pack_start(self.exit_icon_event,False,False,2)

        # setup main box which group the three previous boxes
        self.main_box =  Gtk.HBox()
        self.exit_box.set_name("gui_overlay")
        self.main_box.pack_start(self.info_box,False,False,0)
        self.main_box.pack_start(self.video_box,True,True,0)
        self.main_box.pack_start(self.exit_box,False,False,0)
        self.add(self.main_box)
        return True

    def drawing(self, widget, cr):
        """
        Drawing callback used to draw with cairo on
        the drawing area
        """
        if self.first_drawing_call :
            self.first_drawing_call = False
            self.drawing_width = widget.get_allocated_width()
            self.drawing_height = widget.get_allocated_height()
            cr.set_font_size(self.ui_cairo_font_size_label)
            self.boxes_printed = True
            if self.app.enable_camera_preview == False :
                self.app.still_picture_next = True
                if args.validation:
                    GLib.idle_add(self.app.process_picture)
                else:
                    self.app.process_picture()
            return False

        if (self.app.label_to_display == ""):
            # waiting screen
            text = "Loading NN model"
            cr.set_font_size(self.ui_cairo_font_size*3)
            xbearing, ybearing, width, height, xadvance, yadvance = cr.text_extents(text)
            cr.move_to((self.drawing_width/2-width/2),(self.drawing_height/2))
            cr.text_path(text)
            cr.set_source_rgb(0.012,0.137,0.294)
            cr.fill_preserve()
            cr.set_source_rgb(1, 1, 1)
            cr.set_line_width(0.2)
            cr.stroke()
            return True
        else :
            #recover the widget size depending of the information to display
            self.drawing_width = widget.get_allocated_width()
            self.drawing_height = widget.get_allocated_height()

            #adapt the drawing overlay depending on the image/camera stream displayed
            if self.app.enable_camera_preview == True:
                preview_ratio = float(args.frame_width)/float(args.frame_height)
                preview_height = self.drawing_height
                preview_width =  preview_ratio * preview_height
                if preview_width >= self.drawing_width:
                   offset = 0
                   preview_width = self.drawing_width
                   preview_height = preview_width / preview_ratio
                   vertical_offset = (self.drawing_height - preview_height)/2
                else :
                    offset = (self.drawing_width - preview_width)/2
                    vertical_offset = 0
            else :
                preview_width = self.app.frame_width
                preview_height = self.app.frame_height
                preview_ratio = preview_width / preview_height
                offset = int((self.drawing_width - preview_width)/2)
                vertical_offset = (self.drawing_height - preview_height)/2
                if args.validation:
                    self.app.still_picture_next = True

            # draw rectangle around the 5 first detected object with a score greater
            # than the value determined in the threshold argument
            MAX_PRINTED_BOXES = int((args.maximum_detection)/2)
            for i in range (MAX_PRINTED_BOXES):
                if i == 0:
                    cr.set_source_rgb(1, 0, 0)
                elif i == 1:
                    cr.set_source_rgb(0, 1, 0)
                elif i == 2:
                    cr.set_source_rgb(0, 0, 1)
                elif i == 3:
                    cr.set_source_rgb(0.5, 0.5, 0)
                elif i == 4:
                    cr.set_source_rgb(0.5, 0, 0.5)
                if self.app.nn.nn_result_scores[0][i] > args.threshold:
                    y0 = int(self.app.nn.get_object_location_y0(i) * preview_height)
                    x0 = int(self.app.nn.get_object_location_x0(i) * preview_width)
                    y1 = int(self.app.nn.get_object_location_y1(i) * preview_height)
                    x1 = int(self.app.nn.get_object_location_x1(i) * preview_width)
                    x = x0 + offset
                    y = y0 + vertical_offset
                    width = (x1 - x0)
                    height = (y1 - y0)
                    label = self.app.nn.get_label(i)
                    accuracy = self.app.nn.nn_result_scores[0][i] * 100
                    cr.rectangle(int(x),int(y),width,height)
                    cr.stroke()
                    cr.move_to(x , (y - (self.ui_cairo_font_size/2)))
                    text_to_display = label + " " + str(int(accuracy)) + "%"
                    cr.show_text(text_to_display)
        return True

    def still_picture(self,  widget, event):
        """
        ST icon cb which trigger a new inference
        """
        self.app.still_picture_next = True
        return self.app.process_picture()

class Application:
    """
    Class that handles the whole application
    """
    def __init__(self, args):
        #init variables uses :
        self.exit_app = False
        self.dcmipp_camera = False
        self.first_call = True
        self.window_width = 0
        self.window_height = 0
        self.get_display_resolution()
        #if args.image is empty -> camera preview mode else still picture
        if args.image == "":
            print("camera preview mode activate")
            self.enable_camera_preview = True
            #Test if a camera is connected
            check_camera_cmd = RESOURCES_DIRECTORY + "check_camera_preview.sh"
            check_camera = subprocess.run(check_camera_cmd)
            if check_camera.returncode==1:
                print("no camera connected")
                exit(1)
            self.video_device,self.camera_caps,self.dcmipp_sensor=self.setup_camera()
        else:
            print("still picture mode activate")
            self.enable_camera_preview = False
            self.still_picture_next = False
        # initialize the list of the file to be processed (used with the
        # --image parameter)
        self.files = []
        # initialize the list of inference/display time to process the average
        # (used with the --validation parameter)
        self.valid_inference_time = []
        self.valid_inference_fps = []
        self.valid_preview_fps = []
        self.valid_draw_count = 0

        #instantiate the Neural Network class
        self.nn = NeuralNetwork(args.model_file, args.label_file, float(args.input_mean), float(args.input_std),args.maximum_detection)
        self.shape = self.nn.get_img_size()
        self.nn_input_width = self.shape[1]
        self.nn_input_height = self.shape[0]
        self.nn_input_channel = self.shape[2]
        self.nn_inference_time = 0.0
        self.nn_inference_fps = 0.0
        self.nn_result_label = 0
        self.label_to_display = ""

        #instantiate the Gstreamer pipeline
        self.gst_widget = GstWidget(self,self.nn)
        #instantiate the main window
        self.main_window = MainWindow(args,self)
        #instantiate the overlay window
        self.overlay_window = OverlayWindow(args,self)
        self.main()

    def get_display_resolution(self):
        cmd = "modetest -M stm -c > /tmp/display_resolution.txt"
        subprocess.run(cmd,shell=True)
        display_info_pattern = "#0"
        display_information = ""
        display_resolution = ""
        display_width = ""
        display_height = ""

        f = open("/tmp/display_resolution.txt", "r")
        for line in f :
            if display_info_pattern in line:
                display_information = line
        display_information_splited = display_information.split()
        for i in display_information_splited :
            if "x" in i :
                display_resolution = i
        display_resolution = display_resolution.replace('x',' ')
        display_resolution = display_resolution.split()
        display_width = display_resolution[0]
        display_height = display_resolution[1]

        print("display resolution is : ",display_width, " x ", display_height)
        self.window_width = int(display_width)
        self.window_height = int(display_height)
        return 0

    def setup_camera(self):
        """
        Used to configure the camera based on resolution passed as application arguments
        """
        width = str(args.frame_width)
        height = str(args.frame_height)
        framerate = str(args.framerate)
        device = str(args.video_device)
        config_camera = RESOURCES_DIRECTORY + "setup_camera.sh " + width + " " + height + " " + framerate + " " + device
        x = subprocess.check_output(config_camera,shell=True)
        x = x.decode("utf-8")
        x = x.split("\n")
        for i in x :
            if "V4L_DEVICE" in i:
                video_device = i.lstrip('V4L_DEVICE=')
            if "V4L2_CAPS" in i:
                camera_caps = i.lstrip('V4L2_CAPS=')
            if "DCMIPP_SENSOR" in i:
                dcmipp_sensor = i.lstrip('DCMIPP_SENSOR=')
        return video_device, camera_caps, dcmipp_sensor

    def valid_timeout_callback(self):
        """
        if timeout occurs that means that camera preview and the gtk is not
        behaving as expected */
        """
        print("Timeout: camera preview and/or gtk is not behaving has expected\n")
        Gtk.main_quit()
        os._exit(1)

    # get random file in a directory
    def getRandomFile(self, path):
        """
        Returns a random filename, chosen among the files of the given path.
        """
        if len(self.files) == 0:
            self.files = os.listdir(path)

        if len(self.files) == 0:
            return ''

        # remove .json file
        item_to_remove = []
        for item in self.files:
            if item.endswith(".json"):
                item_to_remove.append(item)

        for item in item_to_remove:
            self.files.remove(item)

        index = random.randrange(0, len(self.files))
        file_path = self.files[index]
        self.files.pop(index)
        return file_path

    def load_valid_results_from_json_file(self, json_file):
        """
        Load json files containing expected results for the validation mode
        """
        json_file = json_file + '.json'
        name = []
        x0 = []
        y0 = []
        x1 = []
        y1 = []
        with open(args.image + "/" + json_file) as json_file:
            data = json.load(json_file)
            for obj in data['objects_info']:
                name.append(obj['name'])
                x0.append(obj['x0'])
                y0.append(obj['y0'])
                x1.append(obj['x1'])
                y1.append(obj['y1'])

        return name, x0, y0, x1, y1
    # Updating the labels and the inference infos displayed on the GUI interface - camera input
    def update_label_preview(self):
        """
        Updating the labels and the inference infos displayed on the GUI interface - camera input
        """
        inference_time = self.nn_inference_time * 1000
        inference_fps = self.nn_inference_fps
        display_fps = self.gst_widget.instant_fps
        labels = self.nn.get_labels()
        label = labels[self.nn_result_label]

        if (args.validation) and (inference_time != 0) and (self.valid_draw_count > 5):
            self.valid_preview_fps.append(round(self.gst_widget.instant_fps))
            self.valid_inference_time.append(round(self.nn_inference_time * 1000, 4))

        str_inference_time = str("{0:0.1f}".format(inference_time)) + " ms"
        str_display_fps = str("{0:.1f}".format(display_fps)) + " fps"
        str_inference_fps = str("{0:.1f}".format(inference_fps)) + " fps"

        info_sstr = "  disp.fps :     " + "\n" + str_display_fps + "\n" + "  inf.fps :     " + "\n" + str_inference_fps + "\n" + "  inf.time :     " + "\n"  + str_inference_time + "\n"

        self.overlay_window.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.overlay_window.ui_cairo_font_size,info_sstr))

        self.label_to_display = label

        if args.validation:
            # reload the timeout
            GLib.source_remove(self.valid_timeout_id)
            self.valid_timeout_id = GLib.timeout_add(10000,
                                                     self.valid_timeout_callback)

            self.valid_draw_count = self.valid_draw_count + 1
            # stop the application after defined amount of draws
            if self.valid_draw_count > int(args.val_run):
                avg_prev_fps = sum(self.valid_preview_fps) / len(self.valid_preview_fps)
                avg_inf_time = sum(self.valid_inference_time) / len(self.valid_inference_time)
                avg_inf_fps = (1000/avg_inf_time)
                print("avg display fps= " + str(avg_prev_fps))
                print("avg inference fps= " + str(avg_inf_fps))
                print("avg inference time= " + str(avg_inf_time) + " ms")
                GLib.source_remove(self.valid_timeout_id)
                Gtk.main_quit()
                return True
        return True

    def update_label_still(self, label, inference_time):
        """
        update inference results in still picture mode
        """
        str_inference_time = str("{0:0.1f}".format(inference_time)) + " ms"
        inference_fps = 1000/inference_time
        str_inference_fps = str("{0:.1f}".format(inference_fps)) + " fps"
        info_sstr ="  inf.fps :     " + "\n" + str_inference_fps + "\n" + "  inf.time :     " + "\n"  + str_inference_time + "\n"
        self.overlay_window.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.main_window.ui_cairo_font_size,info_sstr))
        self.label_to_display = label

    def process_picture(self):
        """
        Still picture inference function
        Load the frame, launch inference and
        call functions to refresh UI
        """
        if self.exit_app:
            Gtk.main_quit()
            return False

        if self.still_picture_next and self.overlay_window.boxes_printed:
            # get randomly a picture in the directory
            rfile = self.getRandomFile(args.image)
            img = Image.open(args.image + "/" + rfile)

            # recover drawing box size and picture size
            screen_width = self.overlay_window.drawing_width
            screen_height = self.overlay_window.drawing_height
            picture_width, picture_height = img.size

            #adapt the frame to the screen with with the preservation of the aspect ratio
            width_ratio = float(screen_width/picture_width)
            height_ratio = float(screen_height/picture_height)

            if width_ratio >= height_ratio :
                self.frame_height = height_ratio * picture_height
                self.frame_width = height_ratio * picture_width
            else :
                self.frame_height = width_ratio * picture_height
                self.frame_width = width_ratio * picture_width

            self.frame_height = int(self.frame_height)
            self.frame_width = int(self.frame_width)
            prev_frame = cv2.resize(np.array(img), (self.frame_width, self.frame_height))
            #resize the frame to feed the NN model
            self.main_window.update_frame(prev_frame)
            self.boxes_printed = False
            # execute the inference
            nn_frame = cv2.resize(np.array(img), (self.nn_input_width, self.nn_input_height))
            if self.nn.channel_first :
                nn_frame = nn_frame.transpose(2,0,1)
            start_time = timer()
            self.nn.launch_inference(nn_frame)
            stop_time = timer()
            self.still_picture_next = False
            self.nn_inference_time = stop_time - start_time
            self.nn_inference_fps = (1000/(self.nn_inference_time*1000))
            self.nn.nn_result_locations[:, :, :], self.nn.nn_result_classes[:, :], self.nn.nn_result_scores[:, :] = self.nn.get_results()
            # write information on the GTK UI
            inference_time = self.nn_inference_time * 1000
            labels = self.nn.get_labels()
            label = labels[self.nn_result_label]
            if args.validation and inference_time != 0:
                # reload the timeout
                GLib.source_remove(self.valid_timeout_id)
                self.valid_timeout_id = GLib.timeout_add(10000,
                                                         self.valid_timeout_callback)

                #  get file path without extension
                file_name_no_ext = os.path.splitext(rfile)[0]

                print("\nInput file: " + args.image + "/" + rfile)

                # retreive associated JSON file information
                expected_label, expected_x0, expected_y0, expected_x1, expected_y1 = self.load_valid_results_from_json_file(file_name_no_ext)

                # count number of object above 60% and compare it with he expected
                # validation result
                count = 0
                for i in range(0, 5):
                    if self.nn.nn_result_scores[0][i] > args.threshold:
                        count = count + 1

                expected_count = len(expected_label)
                print("\texpect %s objects. Object detection inference found %s objects"
                      % (expected_count, count))
                if count != expected_count:
                    print("Inference result not aligned with the expected validation result\n")
                    os._exit(5)

                found = False
                valid_count = 0
                for i in range(0, count):
                    for j in range(0,expected_count):
                        label = self.nn.get_label(i)
                        if expected_label[j] == label:
                            found = True
                            if found :
                                valid_count += 1
                                found = False
                                break

                if valid_count != expected_count:
                        print("Inference result not aligned with the expected validation result\n")
                        os._exit(5)
                else :
                    valid_count = 0

                for i in range(0, count):
                    for j in range(0,expected_count):
                            label = self.nn.get_label(i)
                            x0 = self.nn.get_object_location_x0(i)
                            y0 = self.nn.get_object_location_y0(i)
                            x1 = self.nn.get_object_location_x1(i)
                            y1 = self.nn.get_object_location_y1(i)
                            error_epsilon = 0.02
                            if abs(x0 - float(expected_x0[j])) <= error_epsilon or \
                               abs(y0 - float(expected_y0[j])) <= error_epsilon or \
                               abs(x1 - float(expected_x1[j])) <= error_epsilon or \
                               abs(y1 - float(expected_y1[j])) <= error_epsilon:
                                   found = True
                                   if found :
                                       valid_count += 1
                                       found = False
                                       print("\t{0:12} (x0 y0 x1 y1) {1:12}{2:12}{3:12}{4:12}  expected result: {5:12} (x0 y0 x1 y1) {6:12}{7:12}{8:12}{9:12}".format(label, x0, y0, x1, y1, expected_label[j], expected_x0[j], expected_y0[j], expected_x1[j], expected_y1[j]))
                                       break
                if (valid_count != expected_count) :
                   print("Inference result not aligned with the expected validation result\n")
                   os._exit(5)
                valid_count = 0

                # store the inference time in a list so that we can compute the
                # average later on

                self.valid_inference_time.append(round(self.nn_inference_time * 1000, 4))

                # process all the file
                if len(self.files) == 0:
                    avg_inf_time = sum(self.valid_inference_time) / len(self.valid_inference_time)
                    print("\navg inference time= " + str(avg_inf_time) + " ms")
                    self.exit_app = True

            self.update_label_still(str(label), inference_time)
            self.main_window.queue_draw()
            self.overlay_window.queue_draw()
            return True
        else :
            return False

    def update_ui(self):
        """
        refresh overlay UI
        """
        self.update_label_preview()
        self.main_window.queue_draw()
        self.overlay_window.queue_draw()

    def main(self):

        self.main_window.connect("delete-event", Gtk.main_quit)
        self.main_window.show_all()
        self.overlay_window.connect("delete-event", Gtk.main_quit)
        self.overlay_window.show_all()
        # start a timeout timer in validation process to close application if
        # timeout occurs
        if args.validation:
            self.valid_timeout_id = GLib.timeout_add(35000,
                                                     self.valid_timeout_callback)
        return True

if __name__ == '__main__':
    # add signal to catch CRTL+C
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    # ONNX NN intitalisation
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--image", default="", help="image directory with image to be classified")
    parser.add_argument("-v", "--video_device", default="", help="video device ex: video0")
    parser.add_argument("--frame_width", default=640, help="width of the camera frame (default is 640)")
    parser.add_argument("--frame_height", default=480, help="height of the camera frame (default is 480)")
    parser.add_argument("--framerate", default=15, help="framerate of the camera (default is 15fps)")
    parser.add_argument("-m", "--model_file", default="", help=".onnx model to be executed")
    parser.add_argument("-l", "--label_file", default="", help="name of file containing labels")
    parser.add_argument("--input_mean", default=127.5, help="input mean")
    parser.add_argument("--input_std", default=127.5, help="input standard deviation")
    parser.add_argument("--validation", action='store_true', help="enable the validation mode")
    parser.add_argument("--val_run", default=50, help="set the number of draws in the validation mode")
    parser.add_argument("--num_threads", default=None,help="Select the number of threads used by ONNX interpreter to run inference")
    parser.add_argument("--maximum_detection", default=10, type=int,help="Adjust the maximum number of object detected in a frame accordingly to your NN model (default is 10)")
    parser.add_argument("--threshold", default=0.62, type=float,help="threshold of accuracy above which the boxes are displayed (default 0.62)")
    args = parser.parse_args()

    try:
        application = Application(args)

    except Exception as exc:
        print("Main Exception: ", exc )

    Gtk.main()
    print("gtk main finished")
    print("application exited properly")
    os._exit(0)
