#!/usr/bin/python3
#
# Author: Maxence Guilhin <maxence.guilhin@st.com> for STMicroelectronics.
#
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

import sys
import numpy as np
import argparse
import signal
import os
import random
import json
import subprocess
import re
import time
import os.path
import math
from os import path
import cv2
from PIL import Image
import PIL
import tflite_runtime.interpreter as tflr
from timeit import default_timer as timer

np.set_printoptions(threshold=np.inf)
#init gstreamer
Gst.init(None)
Gst.init_check(None)
#init gtk
Gtk.init(None)
Gtk.init_check(None)

#path definition
LIBVX_PATH = "/usr/lib/libvx_delegate.so.2"
RESOURCES_DIRECTORY = os.path.abspath(os.path.dirname(__file__)) + "/../../resources/"

class NeuralNetwork:
    """
    Class that handles Neural Network inference
    """

    def __init__(self, model_file, label_file, input_mean, input_std, ext_delegate, npu):
        """
        :param model_path: .tflite model to be executed")
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

        self._model_file = model_file
        self._label_file = label_file
        self._input_mean = input_mean
        self._input_std = input_std
        self._floating_model = False
        self.first_inference_done = False
        self.colors_map = np.array([(0,0,0,0),          #1 background
                                    (3,35,75,180),      #2 airplane
                                    (229,204,255,180),  #3 bicycle
                                    (60,180,230,180),   #4 bird
                                    (255,210,0,180),    #5 boat
                                    (0,100,0,180),      #6 bottle
                                    (100,56,0,180),     #7 bus
                                    (52,0,100,180),     #8 car
                                    (0,247,255,180),    #9 cat
                                    (255,190,133,180),  #10 chair
                                    (255,0,0,180),      #11 cow
                                    (178,255,102,180),  #12 dining table
                                    (0,0,255,180),      #13 dog
                                    (100, 0, 40,180),   #14 horse
                                    (210,105,30,180),   #15 motorbike
                                    (230,0,126,180),    #16 person
                                    (204,204,0,180),    #17 potted plant
                                    (87,74,44,180),     #18 sheep
                                    (255,128,0,180),    #19 sofa
                                    (255,255,255,180),  #20 train
                                    (0,255,34,180)])    #21 tv

        if npu is True:
            if path.exists(LIBVX_PATH) :
                self._selected_delegate = LIBVX_PATH
            else :
                print("No delegate ",LIBVX_PATH, " found fall back on CPU mode")

        elif ext_delegate is not None :
            if path.exists(ext_delegate):
                self._selected_delegate = ext_delegate
            else :
                print("No delegate ",ext_delegate, "found fall back on CPU mode")

        if self._selected_delegate is not None:
            vx_delegate = tflr.load_delegate( library=self._selected_delegate,
                                                options={"cache_file_path": "/usr/local/demo-ai/semantic-segmentation/models/deeplabv3/deeplabv3.nb", "allowed_cache_mode":"true"})
            print('Loading external delegate from {}'.format(self._selected_delegate))
            print("number of threads used in tflite interpreter : ",self.number_threads)
            self._interpreter = tflr.Interpreter(model_path=self._model_file,
                                                 num_threads = self.number_threads,
                                                 experimental_delegates=[vx_delegate])
        else :
            print("no delegate to use, CPU mode activated")
            print("number of threads used in tflite interpreter : ",self.number_threads)
            self._interpreter = tflr.Interpreter(model_path=self._model_file,
                                                 num_threads = self.number_threads)

        self._interpreter.allocate_tensors()
        self._input_details = self._interpreter.get_input_details()
        self._output_details = self._interpreter.get_output_details()

        # check the type of the input tensor
        if self._input_details[0]['dtype'] == np.float32:
            self._floating_model = True
            print("Floating point Tensorflow Lite Model")

        self._labels = load_labels(self._label_file)

    def __getstate__(self):
        return (self._model_file, self._label_file, self._input_mean,
                self._input_std, self._floating_model, self._selected_delegate, self.number_threads, \
                self._input_details, self._output_details, self._labels)

    def __setstate__(self, state):
        self._model_file, self._label_file, self._input_mean, \
                self._input_std, self._floating_model, self._selected_delegate, self.number_threads, \
                self._input_details, self._output_details, self._labels = state

        if self._selected_delegate is not None:
            self._interpreter = tflr.Interpreter(model_path=self._model_file,
                                                 num_threads = self.number_threads,
                                                 experimental_delegates=[tflr.load_delegate(self._selected_delegate)])
        else :
            self._interpreter = tflr.Interpreter(model_path=self._model_file,
                                                 num_threads = self.number_threads)
        self._interpreter.allocate_tensors()

    def get_labels(self):
        return self._labels

    def get_img_size(self):
        """
        :return: size of NN input image size
        """
        # NxHxWxC, H:1, W:2, C:3
        return (int(self._input_details[0]['shape'][1]),
                int(self._input_details[0]['shape'][2]),
                int(self._input_details[0]['shape'][3]))

    def launch_inference(self, img):
        """
        This method launches inference using the invoke call
        :param img: the image to be inferred
        """
        # add N dim
        input_data = np.expand_dims(img, axis=0)

        if self._floating_model:
            input_data = (np.float32(input_data) - self._input_mean) / self._input_std

        self._interpreter.set_tensor(self._input_details[0]['index'], input_data)
        self._interpreter.invoke()

    def get_results(self):
         """
         This method is used to recover NN results
         and do the minimal post-process required
         """
         output_data = self._interpreter.get_tensor(self._output_details[0]['index'])
         seg_map = np.squeeze(output_data)
         seg_map_np = np.asarray(seg_map)
         seg_map_argmax = np.argmax(seg_map_np, axis=2)
         seg_map_argmax = seg_map_argmax.astype(np.int8)
         seg_map_argmax = np.asarray(seg_map_argmax)
         seg_map_colored = self.colors_map[seg_map_argmax].astype(np.uint8)
         unique_label=np.unique(seg_map_argmax)
         return unique_label,seg_map_colored

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
            nn_caps = "video/x-raw, format=RGB, width=" + str(self.app.nn_input_width) + ",height=" + str(self.app.nn_input_height)
            nncaps = Gst.Caps.from_string(nn_caps)
            self.appsink.set_property("caps", nncaps)
            self.appsink.set_property("emit-signals", True)
            self.appsink.set_property("sync", False)
            self.appsink.set_property("max-buffers", 1)
            self.appsink.set_property("drop", True)
            self.appsink.connect("new-sample", self.new_sample)

            # creation of the gtkwaylandsink element to handle the gestreamer video stream
            self.gtkwaylandsink = Gst.ElementFactory.make("gtkwaylandsink")
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
            self.video_scale_nn = Gst.ElementFactory.make("videoscale", "video-scale_nn")

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
            self.pipeline.add(self.video_scale_nn)

            # linking elements together
            #                              -> queue 1 -> videoconvert -> video scale -> fpsdisplaysink
            # v4l2src -> video rate -> tee
            #                              -> queue 2 -> videoconvert -> video scale -> appsink
            self.v4lsrc1.link(self.video_rate)
            self.video_rate.link(self.camerafilter1)
            self.camerafilter1.link(self.tee)
            self.queue1.link(self.videoformatconverter1)
            self.videoformatconverter1.link(self.fps_disp_sink)
            self.queue2.link(self.videoformatconverter2)
            self.videoformatconverter2.link(self.video_scale_nn)
            self.video_scale_nn.link(self.appsink)
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
            self.app.draw_inference = True
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
        sample = self.appsink.emit("pull-sample")
        arr = self.gst_to_opencv(sample)
        self.last_picture = arr.copy()
        self.cpt_frame += 1
        if self.cpt_frame == 60:
            self.cpt_frame = 0
            self.update_isp_config()
        if (args.validation):
            if (arr is not None):
                start_time = timer()
                self.nn.launch_inference(arr)
                stop_time = timer()
                self.app.nn_inference_time = stop_time - start_time
                self.app.nn_inference_fps = (1000/(self.app.nn_inference_time*1000))
                self.app.unique_label, self.app.nn_seg_map = self.app.nn.get_results()
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
        usefull for the application
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
        self.ui_icon_label_width = '64'
        self.ui_icon_label_height = '64'
        if window_constraint <= 272:
               # Display 480x272
               self.ui_cairo_font_size = 11
               self.ui_cairo_font_size_label = 18
               self.ui_icon_exit_width = '25'
               self.ui_icon_exit_height = '25'
               self.ui_icon_st_width = '42'
               self.ui_icon_st_height = '52'
               self.ui_icon_label_width = '32'
               self.ui_icon_label_height = '32'
        elif window_constraint <= 600:
               #Display 800x480
               #Display 1024x600
               self.ui_cairo_font_size = 16
               self.ui_cairo_font_size_label = 29
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '65'
               self.ui_icon_st_height = '80'
               self.ui_icon_label_width = '64'
               self.ui_icon_label_height = '64'
        elif window_constraint <= 720:
               #Display 1280x720
               self.ui_cairo_font_size = 23
               self.ui_cairo_font_size_label = 38
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'
               self.ui_icon_label_width = '64'
               self.ui_icon_label_height = '64'
        elif window_constraint <= 1080:
               #Display 1920x1080
               self.ui_cairo_font_size = 33
               self.ui_cairo_font_size_label = 48
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'
               self.ui_icon_label_width = '64'
               self.ui_icon_label_height = '64'

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
            self.st_icon_path = RESOURCES_DIRECTORY + 'tfl_st_icon_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
            self.st_icon = Gtk.Image.new_from_file(self.st_icon_path)
            self.st_icon_event = Gtk.EventBox()
            self.st_icon_event.add(self.st_icon)
            self.info_box.pack_start(self.st_icon_event,False,False,2)
            self.inf_time = Gtk.Label()
            self.inf_time.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.inf_time,False,False,10)
            info_sstr = "    disp.fps :      " + "\n" + "  inf.fps :     " + "\n" + "  inf.time :    " + "\n"
            self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.ui_cairo_font_size,info_sstr))
            self.labels_to_display = Gtk.Label()
            self.labels_to_display.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.labels_to_display,False,False,2)
            self.rules = Gtk.Label()
            self.rules.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.rules,False,False,10)
            rules_sstr = " Click on " + "\n" + " camera preview " + "\n" + " to run " + "\n" + " segmentation " + "\n"
            self.rules.set_markup("<span font=\'%d\' color='#000000'><b>%s\n</b></span>" % (self.ui_cairo_font_size,rules_sstr))
            self.label_icon_path = RESOURCES_DIRECTORY + 'label_icon_' + self.ui_icon_label_width + 'x' + self.ui_icon_label_height + '.png'
            self.label_icon = Gtk.Image.new_from_file(self.label_icon_path)
            self.label_icon_event = Gtk.EventBox()
            self.label_icon_event.add(self.label_icon)
            self.info_box.pack_start(self.label_icon_event,False,False,10)
        else :
            # still picture mode
            self.info_box = Gtk.VBox()
            self.info_box.set_name("gui_main_stbox")
            self.st_icon_path = RESOURCES_DIRECTORY + 'tfl_st_icon_next_inference_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
            self.st_icon = Gtk.Image.new_from_file(self.st_icon_path)
            self.st_icon_event = Gtk.EventBox()
            self.st_icon_event.add(self.st_icon)
            self.info_box.pack_start(self.st_icon_event,False,False,20)
            self.inf_time = Gtk.Label()
            self.inf_time.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.inf_time,False,False,2)
            info_sstr = "  inf.fps :     " + "\n" + "  inf.time :     " + "\n"
            self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.ui_cairo_font_size,info_sstr))
            self.labels_to_display = Gtk.Label()
            self.labels_to_display.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.labels_to_display,False,False,2)
            self.label_icon_path = RESOURCES_DIRECTORY + 'label_icon_' + self.ui_icon_label_width + 'x' + self.ui_icon_label_height + '.png'
            self.label_icon = Gtk.Image.new_from_file(self.label_icon_path)
            self.label_icon_event = Gtk.EventBox()
            self.label_icon_event.add(self.label_icon)
            self.info_box.pack_start(self.label_icon_event,False,False,2)

        # setup video box containing gst stream in camera previex mode
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
        # # setup the exit box which contains the exit button
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
        self.previous_touch_event = 0
        self.display_help = False

    def exit_icon_cb(self,eventbox, event):
        """
        Exit callback to close application
        """
        self.destroy()
        Gtk.main_quit()

    def label_icon_event_cb(self,eventbox, event):
        """
        Exit callback to close application
        """
        if event.type == Gdk.EventType.BUTTON_PRESS:
            if (self.display_help):
                self.display_help = False
                self.app.update_ui()
            else :
                self.display_help = True
                self.app.update_ui()


    def touch_event_cb(self, widget, event):
        """
        Touch event callback to stop camera stream and run inference on last camera frame
        """
        if event.touch.type == Gdk.EventType.TOUCH_BEGIN:
            state = self.app.gst_widget.pipeline.get_state(Gst.CLOCK_TIME_NONE)
            if (state.state == Gst.State.PLAYING):
                self.app.gst_widget.pipeline.set_state(Gst.State.PAUSED)
                self.app.draw_inference=True
                if (self.app.gst_widget.last_picture is not None):
                    start_time = timer()
                    self.app.nn.launch_inference(self.app.gst_widget.last_picture)
                    stop_time = timer()
                    self.app.nn_inference_time = stop_time - start_time
                    self.app.nn_inference_fps = (1000/(self.app.nn_inference_time*1000))
                    self.app.unique_label, self.app.nn_seg_map = self.app.nn.get_results()
                self.app.update_ui()
            elif (state.state == Gst.State.PAUSED):
                self.app.gst_widget.pipeline.set_state(Gst.State.PLAYING)
                self.app.draw_inference = False
                self.app.update_ui()

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
        self.ui_icon_label_width = '64'
        self.ui_icon_label_height = '64'
        if window_constraint <= 272:
               # Display 480x272
               self.ui_cairo_font_size = 11
               self.ui_cairo_font_size_label = 18
               self.ui_icon_exit_width = '25'
               self.ui_icon_exit_height = '25'
               self.ui_icon_st_width = '42'
               self.ui_icon_st_height = '52'
               self.ui_icon_label_width = '32'
               self.ui_icon_label_height = '32'
        elif window_constraint <= 600:
               #Display 800x480
               #Display 1024x600
               self.ui_cairo_font_size = 16
               self.ui_cairo_font_size_label = 29
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '65'
               self.ui_icon_st_height = '80'
               self.ui_icon_label_width = '64'
               self.ui_icon_label_height = '64'
        elif window_constraint <= 720:
               #Display 1280x720
               self.ui_cairo_font_size = 23
               self.ui_cairo_font_size_label = 38
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'
               self.ui_icon_label_width = '64'
               self.ui_icon_label_height = '64'
        elif window_constraint <= 1080:
               #Display 1920x1080
               self.ui_cairo_font_size = 33
               self.ui_cairo_font_size_label = 48
               self.ui_icon_exit_width = '50'
               self.ui_icon_exit_height = '50'
               self.ui_icon_st_width = '130'
               self.ui_icon_st_height = '160'
               self.ui_icon_label_width = '64'
               self.ui_icon_label_height = '64'

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
            self.st_icon_path = RESOURCES_DIRECTORY + 'tfl_st_icon_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
            self.st_icon = Gtk.Image.new_from_file(self.st_icon_path)
            self.st_icon_event = Gtk.EventBox()
            self.st_icon_event.add(self.st_icon)
            self.info_box.pack_start(self.st_icon_event,False,False,2)
            self.inf_time = Gtk.Label()
            self.inf_time.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.inf_time,False,False,10)
            info_sstr = "    disp.fps :      " + "\n" + "  inf.fps :     " + "\n" + "  inf.time :    " + "\n"
            self.inf_time.set_markup("<span font=\'%d\' color='#FFFFFFFF'><b>%s\n</b></span>" % (self.ui_cairo_font_size,info_sstr))
            self.labels_to_display = Gtk.Label()
            self.labels_to_display.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.labels_to_display,False,False,2)
            self.rules = Gtk.Label()
            self.rules.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.rules,False,False,10)
            rules_sstr = " Click on " + "\n" + " camera preview " + "\n" + " to run " + "\n" + " segmentation " + "\n"
            self.rules.set_markup("<span font=\'%d\' color='#E6007E'><b>%s\n</b></span>" % (self.ui_cairo_font_size,rules_sstr))
            self.label_icon_path = RESOURCES_DIRECTORY + 'label_icon_' + self.ui_icon_label_width + 'x' + self.ui_icon_label_height + '.png'
            self.label_icon = Gtk.Image.new_from_file(self.label_icon_path)
            self.label_icon_event = Gtk.EventBox()
            self.label_icon_event.add(self.label_icon)
            self.label_icon_event.connect("button_press_event",self.label_icon_event_cb)
            self.info_box.pack_start(self.label_icon_event,False,False,10)
        else :
            # still picture mode
            self.info_box = Gtk.VBox()
            self.info_box.set_name("gui_overlay_stbox")
            self.st_icon_path = RESOURCES_DIRECTORY + 'tfl_st_icon_next_inference_' + self.ui_icon_st_width + 'x' + self.ui_icon_st_height + '.png'
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
            self.labels_to_display = Gtk.Label()
            self.labels_to_display.set_justify(Gtk.Justification.CENTER)
            self.info_box.pack_start(self.labels_to_display,False,False,2)
            self.label_icon_path = RESOURCES_DIRECTORY + 'label_icon_' + self.ui_icon_label_width + 'x' + self.ui_icon_label_height + '.png'
            self.label_icon = Gtk.Image.new_from_file(self.label_icon_path)
            self.label_icon_event = Gtk.EventBox()
            self.label_icon_event.add(self.label_icon)
            self.label_icon_event.connect("button_press_event",self.label_icon_event_cb)
            self.info_box.pack_start(self.label_icon_event,False,False,2)

        # setup video box containing a transparent drawing area
        # to draw over the video stream
        self.video_box = Gtk.HBox()
        self.video_box.set_name("gui_overlay_video")
        self.video_box.set_app_paintable(True)
        self.drawing_area = Gtk.DrawingArea()
        self.drawing_area.connect("draw", self.drawing)
        self.drawing_area.connect("touch-event", self.touch_event_cb)
        self.drawing_area.add_events(Gdk.EventMask.TOUCH_MASK)
        self.drawing_area.set_name("overlay_draw")
        self.drawing_area.set_app_paintable(True)
        self.video_box.pack_start(self.drawing_area, True, True, 0)

        # # setup the exit box which contains the exit button
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
            cr.set_font_size(self.ui_cairo_font_size)
            self.draw = True
            if self.app.enable_camera_preview == False :
                self.app.still_picture_next = True
                self.app.draw_inference = True
                if args.validation:
                    GLib.idle_add(self.app.process_picture)
                else:
                    self.app.process_picture()
            return False

        if (self.app.draw_inference):
            self.display_help = False
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
                    self.app.draw_inference = True

            #load the segmentation bitmap as a picture
            img = Image.fromarray(self.app.nn_seg_map,'RGBA')
            size = (int(preview_width),int(preview_height))
            img = img.resize(size)
            img_alpha = img.copy()
            img_alpha.save("/home/weston/bitmap.png","PNG")

            #load the bitmap to display it as overlay
            pixbuf = GdkPixbuf.Pixbuf.new_from_file('/home/weston/bitmap.png')
            img = Gdk.cairo_set_source_pixbuf(cr, pixbuf.copy(),int(offset), int(vertical_offset))
            cr.paint()
            if (self.app.enable_camera_preview == False):
                self.app.draw_inference = False
        else:
            if (self.display_help):
                cr.rectangle(0, 0, self.drawing_width, self.drawing_height)
                cr.set_source_rgba(3,35,75,0.25)
                cr.fill()
                #determine labels to display
                labels = self.app.nn.get_labels()
                label_map = np.arange(len(labels)).reshape(len(labels), 1)
                color_map = self.app.nn.colors_map[label_map]
                offset_x = self.drawing_width/6
                offset_y = self.drawing_height/8
                #display labels
                for i in range(len(labels)):
                    if (i < 10):
                        label = labels[i]
                        text = str(label)
                        cr.set_font_size(self.ui_cairo_font_size*2)
                        xbearing, ybearing, width, height, xadvance, yadvance = cr.text_extents(text)
                        cr.move_to(offset_x,offset_y+((self.ui_cairo_font_size*3)*i))
                        cr.text_path(text)
                        cr.set_source_rgba(color_map[i][0][0]/255, color_map[i][0][1]/255, color_map[i][0][2]/255,1)
                        cr.fill_preserve()
                        cr.set_source_rgb(1, 1, 1)
                        cr.set_line_width(0.1)
                        cr.stroke()
                    else :
                        label = labels[i]
                        text = str(label)
                        cr.set_font_size(self.ui_cairo_font_size*2)
                        xbearing, ybearing, width, height, xadvance, yadvance = cr.text_extents(text)
                        cr.move_to(3*offset_x,offset_y+((self.ui_cairo_font_size*3)*(i-10)))
                        cr.text_path(text)
                        cr.set_source_rgba(color_map[i][0][0]/255, color_map[i][0][1]/255, color_map[i][0][2]/255,1)
                        cr.fill_preserve()
                        cr.set_source_rgb(1, 1, 1)
                        cr.set_line_width(0.1)
                        cr.stroke()
            else :
                self.app.main_window.label_icon.show()
                self.label_icon.show()
                self.app.main_window.inf_time.hide()
                self.inf_time.hide()
                self.app.main_window.rules.show()
                self.rules.show()
                self.labels_to_display.hide()
        return True

    def still_picture(self,  widget, event):
        """
        ST icon cb which trigger a new inference
        """
        self.app.still_picture_next = True
        self.app.draw_inference = True
        return self.app.process_picture()

class Application:
    """
    Class that handles the whole application
    """
    def __init__(self, args):
        #init variables uses :
        self.exit_app = False
        self.first_call = True
        self.loading_nn = True
        self.draw_inference = False
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
        self.nn = NeuralNetwork(args.model_file, args.label_file, float(args.input_mean), float(args.input_std), args.ext_delegate, args.npu)
        self.shape = self.nn.get_img_size()
        self.nn_input_width = self.shape[1]
        self.nn_input_height = self.shape[0]
        self.nn_input_channel = self.shape[2]
        self.nn_inference_time = 0.0
        self.nn_inference_fps = 0.0
        self.unique_label = []
        self.nn_seg_map = np.zeros((self.nn_input_width,self.nn_input_height,3))

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

        # remove .csv file
        item_to_remove = []
        for item in self.files:
            if item.endswith(".csv"):
                item_to_remove.append(item)

        for item in item_to_remove:
            self.files.remove(item)

        index = random.randrange(0, len(self.files))
        file_path = self.files[index]
        self.files.pop(index)
        return file_path

    def load_valid_results_from_csv_file(self,csv_file):
        """
        Load csv files containing expected results for the validation mode
        """
        csv_file = csv_file + '.csv'
        print("csv file name ",csv_file)
        load_validation_data = np.loadtxt(csv_file,delimiter=',',dtype=np.dtype(int))
        return load_validation_data

    def update_label_still(self, inference_time):
        """
        update inference results in still picture mode
        """
        str_inference_time = str("{0:0.1f}".format(inference_time)) + " ms"
        inference_fps = 1000/inference_time
        str_inference_fps = str("{0:.1f}".format(inference_fps)) + " fps"
        #determine labels to display
        labels = self.nn.get_labels()
        label_map = np.arange(len(labels)).reshape(len(labels), 1)
        color_map = self.nn.colors_map[label_map]
        label_sstr = ""
        if (self.draw_inference):
            #display labels
            for i in range(len(self.unique_label)):
                if (i != 0):
                    label = labels[self.unique_label[i]]
                    text = str(label)
                    R = hex(color_map[self.unique_label[i]][0][0])
                    G = hex(color_map[self.unique_label[i]][0][1])
                    B = hex(color_map[self.unique_label[i]][0][2])
                    R = R.removeprefix('0x')
                    G = G.removeprefix('0x')
                    B = B.removeprefix('0x')
                    R = R.upper()
                    G = G.upper()
                    B = B.upper()
                    if (len(R)==1):
                        R = "0" + R
                    if (len(G)==1):
                        G = "0" + G
                    if (len(B)==1):
                        B = "0" + B
                    label_sstr += "<span font=\'" + str(self.overlay_window.ui_cairo_font_size) + "\' color='#" + str(R) + str(G) + str(B) + "'><b>" +  text + "\n</b></span>"
            label_sstr = "<span font=\'" + str(self.overlay_window.ui_cairo_font_size) + "\' color='#FFFFFF'><b>Labels : \n</b></span>" + label_sstr
            info_sstr = "  inf.fps :     " + "\n" + str_inference_fps + "\n" + "  inf.time :     " + "\n"  + str_inference_time + "\n"
            self.overlay_window.inf_time.set_markup("<span font=\'%d\' color='#FFFFFF'><b>%s\n</b></span>" % (self.overlay_window.ui_cairo_font_size,info_sstr))
            self.overlay_window.labels_to_display.set_markup(label_sstr)
            self.main_window.label_icon.hide()
            self.overlay_window.label_icon.hide()
            self.main_window.inf_time.show()
            self.overlay_window.inf_time.show()
            self.overlay_window.labels_to_display.show()


    # Updating the labels and the inference infos displayed on the GUI interface - camera input
    def update_label_preview(self):
        """
        Updating the labels and the inference infos displayed on the GUI interface - camera input
        """
        inference_time = self.nn_inference_time * 1000
        inference_fps = self.nn_inference_fps
        display_fps = self.gst_widget.instant_fps

        if (args.validation) and (inference_time != 0) and (self.valid_draw_count > 5):
            self.valid_preview_fps.append(round(self.gst_widget.instant_fps))
            self.valid_inference_time.append(round(self.nn_inference_time * 1000, 4))

        str_inference_time = str("{0:0.1f}".format(inference_time)) + " ms"
        str_display_fps = str("{0:.1f}".format(display_fps)) + " fps"
        str_inference_fps = str("{0:.1f}".format(inference_fps)) + " fps"

        #determine labels to display
        labels = self.nn.get_labels()
        label_map = np.arange(len(labels)).reshape(len(labels), 1)
        color_map = self.nn.colors_map[label_map]
        label_sstr = ""
        if (self.draw_inference):
            #display labels
            for i in range(len(self.unique_label)):
                if (i != 0):
                    label = labels[self.unique_label[i]]
                    text = str(label)
                    R = hex(color_map[self.unique_label[i]][0][0])
                    G = hex(color_map[self.unique_label[i]][0][1])
                    B = hex(color_map[self.unique_label[i]][0][2])
                    R = R.removeprefix('0x')
                    G = G.removeprefix('0x')
                    B = B.removeprefix('0x')
                    R = R.upper()
                    G = G.upper()
                    B = B.upper()
                    if (len(R)==1):
                        R = "0" + R
                    if (len(G)==1):
                        G = "0" + G
                    if (len(B)==1):
                        B = "0" + B
                    label_sstr += "<span font=\'" + str(self.overlay_window.ui_cairo_font_size) + "\' color='#" + str(R) + str(G) + str(B) + "'><b>" +  text + "\n</b></span>"
            label_sstr = "<span font=\'" + str(self.overlay_window.ui_cairo_font_size) + "\' color='#FFFFFF'><b>Labels : \n</b></span>" + label_sstr
            info_sstr = "  inf.fps :     " + "\n" + str_inference_fps + "\n" + "      inf.time :        " + "\n"  + str_inference_time + "\n"
            self.overlay_window.inf_time.set_markup("<span font=\'%d\' color='#FFFFFF'><b>%s\n</b></span>" % (self.overlay_window.ui_cairo_font_size,info_sstr))
            self.overlay_window.labels_to_display.set_markup(label_sstr)
            self.overlay_window.rules.hide()
            self.main_window.label_icon.hide()
            self.overlay_window.label_icon.hide()
            self.main_window.inf_time.show()
            self.overlay_window.inf_time.show()
            self.overlay_window.labels_to_display.show()

        if args.validation:
            # reload the timeout
            GLib.source_remove(self.valid_timeout_id)
            self.valid_timeout_id = GLib.timeout_add(50000,
                                                     self.valid_timeout_callback)

            self.valid_draw_count = self.valid_draw_count + 1
            # stop the application after a certain amount of draws
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

    def process_picture(self):
        """
        Still picture inference function
        Load the frame, launch inference and
        call functions to refresh UI
        """
        if self.exit_app:
            Gtk.main_quit()
            return False
        print("process picture enterred")
        if self.still_picture_next and self.overlay_window.draw :
            print("get a random file")
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

            # update the preview frame
            self.main_window.update_frame(prev_frame)
            self.draw = False

            #resize the frame to feed the NN model
            nn_frame = cv2.resize(np.array(img), (self.nn_input_width, self.nn_input_height))

            if (self.first_call):
            # execute a first inference to trigger model compilation
                self.nn.launch_inference(nn_frame)
                if(self.loading_nn):
                    self.loading_nn = False
                self.first_call = False

            start_time = timer()
            self.nn.launch_inference(nn_frame)
            stop_time = timer()
            self.still_picture_next = False
            self.nn_inference_time = stop_time - start_time
            self.nn_inference_fps = (1000/(self.nn_inference_time*1000))
            self.unique_label, self.nn_seg_map= self.nn.get_results()

            # write information onf the GTK UI
            inference_time = self.nn_inference_time * 1000

            if args.validation and inference_time != 0:
                # reload the timeout
                self.draw_inference = True
                GLib.source_remove(self.valid_timeout_id)
                self.valid_timeout_id = GLib.timeout_add(50000,
                                                        self.valid_timeout_callback)

                #prepare inference results to compare with validation results
                np_array_seg = self.nn_seg_map.copy()
                np_array_seg = np.transpose(np_array_seg,(2,0,1))
                np_array_seg = np_array_seg.reshape(4,-1)
                np_array_seg = np.asarray(np_array_seg)
                #  get file path without extension
                file_name_no_ext = os.path.splitext(rfile)[0]

                print("\nInput file: " + args.image + "/" + rfile)

                input_file = args.image + "/" + file_name_no_ext
                # retreive associated CSV file information
                seg_map_expected = self.load_valid_results_from_csv_file(input_file)
                if not(np.array_equal(seg_map_expected,np_array_seg)):
                    print("Inference result mismatch with validation results")
                    os._exit(5)
                self.valid_inference_time.append(round(self.nn_inference_time * 1000, 4))

                # process all the file
                if len(self.files) == 0:
                   avg_inf_time = sum(self.valid_inference_time) / len(self.valid_inference_time)
                   avg_inf_time = round(avg_inf_time,4)
                   print("avg inference time= " + str(avg_inf_time) + " ms")
                   self.exit_app = True
            self.update_label_still(inference_time)
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
        """
        main function
        """
        self.main_window.connect("delete-event", Gtk.main_quit)
        self.main_window.show_all()
        self.overlay_window.connect("delete-event", Gtk.main_quit)
        self.overlay_window.show_all()
        # start a timeout timer in validation process to close application if
        # timeout occurs
        if args.validation:
            self.valid_timeout_id = GLib.timeout_add(50000,
                                                     self.valid_timeout_callback)
        return True

if __name__ == '__main__':
    # add signal to catch CRTL+C
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    #Tensorflow Lite NN intitalisation
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--image", default="", help="image directory with image to be classified")
    parser.add_argument("-v", "--video_device", default="", help="video device ex: video0")
    parser.add_argument("--frame_width", default=640, help="width of the camera frame (default is 320)")
    parser.add_argument("--frame_height", default=480, help="height of the camera frame (default is 240)")
    parser.add_argument("--framerate", default=15, help="framerate of the camera (default is 15fps)")
    parser.add_argument("-m", "--model_file", default="", help=".tflite model to be executed")
    parser.add_argument("-l", "--label_file", default="", help="name of file containing labels")
    parser.add_argument("-e", "--ext_delegate",default = None, help="external_delegate_library path")
    parser.add_argument("--npu", action='store_true', help="enable NPU acceleration")
    parser.add_argument("--input_mean", default=127.5, help="input mean")
    parser.add_argument("--input_std", default=127.5, help="input standard deviation")
    parser.add_argument("--validation", action='store_true', help="enable the validation mode")
    parser.add_argument("--num_threads", default=None, help="Select the number of threads used by tflite interpreter to run inference")
    parser.add_argument("--val_run", default=50, help="set the number of draws in the validation mode")
    args = parser.parse_args()

    try:
        application = Application(args)

    except Exception as exc:
        print("Main Exception: ", exc )

    Gtk.main()
    #remove bitmap.png file before closing app
    file = 'bitmap.png'
    location = "/home/weston"
    path = os.path.join(location,file)
    os.remove(path)
    print("gtk main finished")
    print("application exited properly")
    os._exit(0)
