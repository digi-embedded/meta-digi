/*
 * label_nbg_gst_gtk.cc
 *
 * This application demonstrate a computer vision use case for image
 * classification where frames are grabbed from a camera input (/dev/videox) and
 * analyzed by a neural network model interpreted by OpenVX stack.
 *
 * Gstreamer pipeline is used to stream camera frames (using v4l2src), to
 * display a preview (using waylandsink) and to execute neural network inference
 * (using appsink).
 *
 * The result of the inference is displayed on the preview. The overlay is done
 * using GTK widget with cairo.
 *
 * This combination is quite simple and efficient in term of CPU consumption.
 *
 * Author: Alexis Brisson <alexis.brisson@st.com> for STMicroelectronics.
 *
 * Copyright (c) 2020 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *
 *     http://www.opensource.org/licenses/BSD-3-Clause
 */

#include <filesystem>
#include <stdio.h>
#include <getopt.h>
#include <glib.h>
#include <numeric>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <gst/video/videooverlay.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/gdkwayland.h>
#else
#error "Wayland is not supported in GTK+"
#endif

#include "wrapper_nbg.hpp"

/* Application parameters */
std::vector<std::string> dir_files;
std::string model_file_str;
std::string labels_file_str;
std::string image_dir_str;
std::string video_device_str  = "";
std::string camera_fps_str    = "15";
std::string camera_width_str  = "640";
std::string camera_height_str = "480";
std::string val_run_str = "50";

bool crop = false;
bool verbose = false;
bool validation = false;

float input_mean = 127.5f;
float input_std = 127.5f;
cv::Rect cropRect(0, 0, 0, 0);

/* NBG variables */
struct wrapper_nbg::NBG_Wrapper nbg_wrapper;
struct wrapper_nbg::Config config;
struct wrapper_nbg::Label_Results results;
std::vector<std::string> labels;

bool gtk_main_started = false;
bool exit_application = false;

/* Ressource directory on board */
#define RESOURCES_DIRECTORY "/usr/local/demo-ai/resources/"

/* Structure that contains frame size/position on the screen*/
typedef struct _FramePosition {
	int x;
	int y;
	int width;
	int height;
} FramePosition;

/* Structure that contains all camera configuration information */
typedef struct _Config_camera {
	std::string video_device;
	std::string camera_caps;
	std::string dcmipp_sensor;
} Config_camera;

/* Structure that contains all information to pass around */
typedef struct _CustomData {
	/* The gstreamer pipeline */
	GstElement *pipeline;

	/* The GTK window widget */
	GtkWidget *window_main;
	GtkWidget *window_overlay;

	/* The drawing area where the camera stream is shown */
	GtkWidget *video;
	GstVideoOverlay *video_overlay;
	GtkAllocation video_allocation;

	/*  Camera information  */
	Config_camera camera_info;

	/* Text widget to display info about
	 * - either the display framerate, and the inference time
	 * - in fps or ms and the accuracy of the model
	 */
	GtkWidget *info_inf_time;
	GtkWidget *overlay_draw;
	GtkWidget *info_inf_time_main;
	std::stringstream label_sstr;

	/* window resolution */
	int window_width;
	int window_height;

	/* UI parameters (values will depends on display size) */
	int ui_cairo_font_size_label;
	int ui_cairo_font_size;
	int ui_icon_exit_width;
	int	ui_icon_exit_height;
	int	ui_icon_st_width;
	int	ui_icon_st_height;
	double ui_box_line_width;
	int ui_weston_panel_thickness;

	/* ST icon widget */
	GtkWidget *st_icon_main;
	GtkWidget *st_icon_ov;
	std::stringstream st_icon_sstr;

	/* exit icon widget */
	GtkWidget *exit_icon_main;
	GtkWidget *exit_icon_ov;
	std::stringstream exit_icon_sstr;

	/* Preview camera enable (else still picture use case) */
	bool preview_enabled;

	/* NN input size */
	int nn_input_width;
	int nn_input_height;

	/* Frame resolution (still picture or camera) */
	int frame_width;
	int frame_height;

	/* Frame resolution and scaling factor */
	float x_scale;
	float y_scale;

	/* drawing area resolution */
	int widget_draw_width;
	int widget_draw_height;
	int offset;

	/* For validation purpose */
	int valid_timeout_id;
	int valid_draw_count;
	int val_run;
	std::vector<double> valid_inference_time;
	std::string file;

	/* aspect ratio */
	FramePosition frame_disp_pos;

	/*  Still picture variables */
	bool new_inference;
	cv::Mat img_to_display;

	/* ISP configuration */
	int cpt_frame = 0;

} CustomData;

/* Framerate statistics */
gdouble display_avg_fps = 0;

/**
 * This function is used to get the display resolution
 */
auto get_display_resolution(CustomData *data){
	std::stringstream cmd;
	cmd << "modetest -M stm -c > /tmp/display_resolution.txt";
	system(cmd.str().c_str());
	std::string display_info_pattern = "#0";
	std::ifstream file("/tmp/display_resolution.txt");
	std::string display_information;
	std::string display_resolution;
	std::string display_width;
	std::string display_height;
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			int found_resolution = line.find(display_info_pattern);
			if (found_resolution != -1) {
				display_information = line.substr((found_resolution + display_info_pattern.length()));
			}
	    }
	    file.close();
	}
	system("rm -rf /tmp/display_resolution.txt");
	std::stringstream display_resolution_ss(display_information);
	while(std::getline(display_resolution_ss, display_resolution, ' ')) {
		int found = display_resolution.find("x");
		if (found != -1) {
			display_width = display_resolution.substr(0,found);
			display_height = display_resolution.substr(found + 1);
		}
	}
	g_print("display resolution is : %s x %s \n",display_width.c_str(),display_height.c_str());
	data->window_width = std::stoi(display_width);
	data->window_height = std::stoi(display_height);
	return 0;
}

/**
 * This function is used to setup the camera depending on the
 * parameters defined by the user
 */
auto setup_camera() {
	std::stringstream config_camera;
	config_camera << RESOURCES_DIRECTORY << "setup_camera.sh " << camera_width_str << " " << camera_height_str << " " << camera_fps_str << " " << video_device_str << " > /tmp/camera_device.txt";
	system(config_camera.str().c_str());
	std::string video_device_pattern = "V4L_DEVICE=";
	std::string camera_caps_pattern = "V4L2_CAPS=";
	std::string dcmipp_sensor_pattern = "DCMIPP_SENSOR=";
	Config_camera camera_info;
	std::ifstream file("/tmp/camera_device.txt");
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			int found_device = line.find(video_device_pattern);
			if (found_device != -1) {
				camera_info.video_device = line.substr((found_device + video_device_pattern.length()));
			}
			int found_camera = line.find(camera_caps_pattern);
			if (found_camera != -1) {
				camera_info.camera_caps = line.substr((found_camera + camera_caps_pattern.length()));
			}
			int found_dcmipp = line.find(dcmipp_sensor_pattern);
			if (found_dcmipp != -1) {
				camera_info.dcmipp_sensor = line.substr((found_dcmipp + dcmipp_sensor_pattern.length()));
			}
	    }
	    file.close();
	}
	system("rm -rf /tmp/camera_device.txt");
	return camera_info;
}

/**
 * This function is called to for updating ISP configuration
 */

auto update_isp_config(CustomData *data)
{
	std::stringstream isp_file;
	std::stringstream isp_config;
	isp_file << "/usr/local/demo/application/camera/bin/isp";
	isp_config << isp_file.str().c_str() << " -w > /dev/null";
	std::ifstream file(isp_file.str().c_str());
	if (data->cpt_frame == 0 && file.good() && data->camera_info.dcmipp_sensor.compare("imx335") == 0 ){
		system(isp_config.str().c_str());
	}
    return TRUE;
}

/**
 * This function is called when the ST icon image is clicked
 */
static gboolean st_icon_cb(GtkWidget *event_box,
			   GdkEventButton *event,
			   CustomData *data)
{
	if (!data->preview_enabled){
		g_print("ST icon clicked: new inference\n");
		data->new_inference = true;
	}
	return TRUE;
}

/**
 * This function is called when the exit icon image is clicked
 */

static gboolean exit_icon_cb(GtkWidget *event_box,
			     GdkEventButton *event,
			     CustomData *data)
{
	g_print("Exit icon clicked: exit\n");
	gtk_main_quit();
	return TRUE;
}

/**
 * In validation mode this function is called when timeout occurs
 * Exit application if this function is called
 */
gint valid_timeout_callback(gpointer data)
{
	/* If timeout occurs that means that camera preview and the gtk is not
	 * behaving as expected */
	g_print("Timeout: application is not behaving has expected\n");
	exit(1);
}

/**
 * This function execute an NN inference
 */
static void nn_inference(uint8_t *img)
{
	nbg_wrapper.RunInference(img, &results);
}

/**
 * This function is an helper to get frame position on the display
 */
static void compute_frame_position(CustomData *data)
{
	int window_width = data->widget_draw_width;
	int window_height = data->widget_draw_height;
	int offset_x = 0;
	int offset_y = 0;

	float width_ratio   = (float)window_width / (float)data->frame_width;
	float height_ratio  = (float)window_height / (float)data->frame_height;

	if (width_ratio > height_ratio) {
		data->frame_disp_pos.width = (int)((float)data->frame_width * height_ratio);
		data->frame_disp_pos.height = (int)((float)data->frame_height * height_ratio);
		data->frame_disp_pos.x = (window_width - data->frame_disp_pos.width) / 2;
		data->frame_disp_pos.y = offset_y;
	} else {
		data->frame_disp_pos.width = (int)((float)data->frame_width * width_ratio);
		data->frame_disp_pos.height = (int)((float)data->frame_height * width_ratio);
		data->frame_disp_pos.x = offset_x;
		data->frame_disp_pos.y = (window_height - data->frame_disp_pos.height) / 2;
	}
}

/**
 * This function get a file randomly in a directory
 */
static std::string get_files_in_directory_randomly(std::string directory)
{
	std::stringstream file_path_sstr;
	/* Get the list of all the file ins the directory if the list of the
	 * file is empty */
	if (dir_files.size() == 0) {
		DIR* dirp;
		/* checking if the directory can be opened */
		if ((dirp = opendir(directory.c_str())) == NULL) {
			g_printerr("Cannot open %s\n",directory.c_str());
			exit (1);
		}
		struct dirent * dp;
		while ((dp = readdir(dirp)) != NULL) {
			if ((strcmp(dp->d_name, ".") !=0) &&
			    (strcmp(dp->d_name, "..") != 0))
				dir_files.push_back(dp->d_name);
		}
		closedir(dirp);
	}

	/* Check if directory is empty */
	if (dir_files.size() == 0)
		return "";

	int random = rand() % dir_files.size();
	file_path_sstr << directory << dir_files[random];
	dir_files.erase(dir_files.begin() + random);

	return file_path_sstr.str();
}

/**
 * This function is an idle function waiting for a new picture
 * to be infered
 */
bool first_call_validation = true;
static gboolean infer_new_picture(CustomData *data)
{
	if (exit_application) {
		gtk_main_quit();
		return FALSE;
	}

	if (data->new_inference){
		/* Select a picture in the directory */
		data->file = get_files_in_directory_randomly(image_dir_str);
		/* Check the read and right permission of the selected file */
		int ret = access(data->file.c_str(),R_OK);
		if (ret != 0){
			g_printerr("Read permission denied to the file %s\n",data->file.c_str());
			exit(1);
		}

		/* Read and format the picture */
		cv::Mat img_bgr, img_bgra, img_tdp, img_nn;

		img_bgr = cv::imread(data->file);
		cv::cvtColor(img_bgr, img_bgra, cv::COLOR_BGR2BGRA);
		data->frame_width =  img_bgra.size().width;
		data->frame_height = img_bgra.size().height;
		compute_frame_position(data);

		/* Get final frame position and dimension and resize it */
		cv::Size size(data->frame_disp_pos.width,data->frame_disp_pos.height);
		cv::resize(img_bgra, img_tdp , size);
		data->img_to_display = img_tdp.clone();
		/* prepare the inference */
		cv::Size size_nn(data->nn_input_width, data->nn_input_height);
		cv::resize(img_bgr, img_nn, size_nn);
		cv::cvtColor(img_nn, img_nn, cv::COLOR_BGR2RGB);

		nn_inference(img_nn.data);

		std::stringstream label_sstr;
		std::stringstream inference_fps_sstr;
		std::stringstream inference_time_sstr;
		std::stringstream accuracy_sstr;
		label_sstr << labels[results.index[0]];

		if (results.inference_time != 0)
		{
			inference_fps_sstr << std::right << std::setw(5) << std::fixed << std::setprecision(1) << (1000 / results.inference_time);
			inference_fps_sstr << std::right << std::setw(3) << " fps ";

			inference_time_sstr << std::right << std::setw(5) << std::fixed << std::setprecision(1) << results.inference_time;
			inference_time_sstr << std::right << std::setw(2) << " ms ";

			accuracy_sstr << std::right << std::setw(5) << std::fixed << std::setprecision(1) << results.accuracy[0] * 100;
			accuracy_sstr << std::right  << std::setw(1) << " % ";
		}

		/*  Update labels */
		std::stringstream info_sstr;
		info_sstr << "  inf.fps :     " << "\n" << inference_fps_sstr.str().c_str() << "\n" << "  inf.time :     " << "\n" << inference_time_sstr.str().c_str() << "\n" <<"  accuracy :     " << "\n" << accuracy_sstr.str().c_str();
		char *label_to_display = g_strdup_printf ("<span line_height=\"1\"  font=\"%d\" color=\"#FFFFFFFF\">""<b>%s\n</b>""</span>",data->ui_cairo_font_size,info_sstr.str().c_str());
		gtk_label_set_markup(GTK_LABEL(data->info_inf_time),label_to_display);
		g_free(label_to_display);

		if (validation) {
			/* Still picture use case */
			if (first_call_validation){
				/* Skip the first inference time in hardware
				 * accelerated mode which is the warmup time
				 * The warmup time is far more important than
				 * classic inference time and will make average
				 * inference time completly false
				 * */
				first_call_validation = false;
			} else {
				data->valid_inference_time.push_back(results.inference_time);
			}
			/* Reload the timeout */
			g_source_remove(data->valid_timeout_id);
			data->valid_timeout_id = g_timeout_add(10000,
							       valid_timeout_callback,
							       NULL);
			/* Get the name of the file without extension and underscore */
			std::string file_name = std::filesystem::path(data->file).stem();
			file_name = file_name.substr(0, file_name.find('_'));

			/* Verify that the inference output result matches the file
			 * name. Else exit the application: validation is failing */
			std::cout << "name extract from the picture file: "
				<< std::left  << std::setw(32) << file_name
				<<  "label: " << label_sstr.str() << std::endl;

			if (file_name.find(label_sstr.str()) == std::string::npos){
				std::cout << "Inference result mismatch the file name\n";
				exit(5);
			}

			/* Continue over all files */
			if (dir_files.size() == 0) {
				auto avg_inf_time = std::accumulate(data->valid_inference_time.begin(),
								    data->valid_inference_time.end(), 0.0) /
								    data->valid_inference_time.size();
				std::cout << "\navg inference time= " << avg_inf_time << " ms\n";
				g_source_remove(data->valid_timeout_id);
				gtk_widget_queue_draw(data->window_main);
				gtk_widget_queue_draw(data->window_overlay);
				exit_application = true;
			} else {
				data->new_inference = true;
			}
		} else {
			data->new_inference = false;
		}
		/*  Refresh main window and overlay */
		gtk_widget_queue_draw(data->window_overlay);
		gtk_widget_queue_draw(data->window_main);
	}
	return TRUE;
}

/**
 * This function display text is a black stroke and a white fill color
 */
void gui_display_outlined_text(cairo_t *cr,
			       const char* text)
{
	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_text_path(cr, text);
	cairo_fill_preserve(cr);
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_set_line_width(cr, 0.5);
	cairo_stroke(cr);
}

/**
 * This function is used to load the CSS file for UI configuration
 */
static void gui_gtk_style(CustomData *data)
{
	std::stringstream css_sstr;
	css_sstr << RESOURCES_DIRECTORY;
	css_sstr << "Default.css";

	/* checking the reading permission of the file */
	int ret = access(css_sstr.str().c_str(),R_OK);
	if (ret != 0){
		g_printerr("Read permission denied to the file %s\n",css_sstr.str().c_str());
		exit(1);
	}
	GtkCssProvider *cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path (cssProvider,
					 css_sstr.str().c_str(),
					 NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
						  GTK_STYLE_PROVIDER(cssProvider),
						  GTK_STYLE_PROVIDER_PRIORITY_USER);
}

/**
 * This function is an helper to set UI variable according to the display size
 */
static void gui_set_ui_parameters(CustomData *data)
{

	int window_constraint = 0;
	if (data->window_height > data->window_width)
		window_constraint = data->window_width;
	else
		window_constraint = data->window_height;

	if (window_constraint <= 272) {
		/* Display 480x272 */
		g_print("Display config <= 272p \n");
		data->ui_cairo_font_size_label = 15;
		data->ui_cairo_font_size = 10;
		data->ui_icon_exit_width = 25;
		data->ui_icon_exit_height = 25;
		data->ui_icon_st_width = 42;
		data->ui_icon_st_height = 52;
	} else if (window_constraint <= 600) {
		/* Display 800x480 */
		/* Display 1024x600 */
		g_print("Display config <= 600p \n");
		data->ui_cairo_font_size_label = 26;
		data->ui_cairo_font_size = 16;
		data->ui_icon_exit_width = 50;
		data->ui_icon_exit_height = 50;
		data->ui_icon_st_width = 65;
		data->ui_icon_st_height = 80;
	} else if (window_constraint <= 720) {
		/* Display 1280x720 */
		g_print("Display config <= 720p \n");
		data->ui_cairo_font_size = 20;
		data->ui_cairo_font_size_label = 35;
		data->ui_icon_exit_width = 50;
		data->ui_icon_exit_height = 50;
		data->ui_icon_st_width = 130;
		data->ui_icon_st_height = 160;
	} else if (window_constraint <= 1080) {
		/* Display 1920x1080 */
		g_print("Display config <= 1080p \n");
		data->ui_cairo_font_size_label = 45;
		data->ui_cairo_font_size = 30;
		data->ui_icon_exit_width = 50;
		data->ui_icon_exit_height = 50;
		data->ui_icon_st_width = 130;
		data->ui_icon_st_height = 160;
	} else {
		/* Default UI parameter */
		g_print("Display config fallback \n");
		data->ui_icon_exit_width = 50;
		data->ui_icon_exit_height = 50;
		data->ui_icon_st_width = 130;
		data->ui_icon_st_height = 160;
		data->ui_cairo_font_size = 20;
		data->ui_cairo_font_size_label = 35;
	}
}

/**
 * This function is an helper to set UI icons sizes according to the display size
 */
static void gui_set_icons_parameters(CustomData *data)
{
	/* set the icons */
	data->st_icon_sstr << RESOURCES_DIRECTORY << "nbg_st_icon_";

	if (!data->preview_enabled)
		data->st_icon_sstr << "next_inference_";
	data->st_icon_sstr << data->ui_icon_st_width << "x" << data->ui_icon_st_height << ".png";

	/* checking the reading permission of the file */
	int ret = access(data->st_icon_sstr.str().c_str(),R_OK);
	if (ret != 0){
		g_printerr("Read permission denied to the file %s\n",data->st_icon_sstr.str().c_str());
		exit(1);
	}

	data->exit_icon_sstr << RESOURCES_DIRECTORY << "exit_";
	data->exit_icon_sstr << data->ui_icon_exit_width << "x" << data->ui_icon_exit_height << ".png";

	/* checking the reading permission of the file */
	ret = access(data->exit_icon_sstr.str().c_str(),R_OK);
	if (ret != 0){
		g_printerr("Read permission denied to the file %s\n",data->exit_icon_sstr.str().c_str());
		exit(1);
	}
}

/**
 * This function is called each time the GTK UI is redrawn
 */
bool first_call_overlay = true;
static gboolean gui_draw_overlay_cb(GtkWidget *widget,
				    cairo_t *cr,
				    CustomData *data)
{
	/* Get drawing area informations */
	int draw_width = gtk_widget_get_allocated_width(widget);
	int draw_height= gtk_widget_get_allocated_height(widget);

	/* Updating the information with the new inference results */
	std::stringstream display_fps_sstr;
	std::stringstream inference_time_sstr;
	std::stringstream inference_fps_sstr;
	std::stringstream accuracy_sstr;
	float inf_time = 0;

	if (results.inference_time != 0)
	{
		/* Updating the information with the new inference results */
		display_fps_sstr   << std::right << std::setw(5) << std::fixed << std::setprecision(1) << display_avg_fps;
		display_fps_sstr   << std::right << std::setw(3) << " fps ";

		inference_fps_sstr << std::right << std::setw(5) << std::fixed << std::setprecision(1) << (1000 / results.inference_time);
		inference_fps_sstr << std::right << std::setw(3) << " fps ";

		inference_time_sstr << std::right << std::setw(5) << std::fixed << std::setprecision(1) << results.inference_time;
		inference_time_sstr << std::right << std::setw(2) << " ms ";

		accuracy_sstr << std::right << std::setw(5) << std::fixed << std::setprecision(1) << results.accuracy[0] * 100;
		accuracy_sstr << std::right  << std::setw(1) << " % ";
	}

	/* set the font and the size for the label*/
	cairo_select_font_face (cr, "monospace",
				CAIRO_FONT_SLANT_NORMAL,
				CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, data->ui_cairo_font_size_label);

	/*  Update the gtk labels with latest information */
	if (data->preview_enabled) {
		/* Camera preview use case */
		if (first_call_overlay){
			first_call_overlay = false;
			return FALSE;
		} else {
			/*  Update labels */
			std::stringstream info_sstr;
			info_sstr << "  disp.fps :     " << "\n" << display_fps_sstr.str().c_str() << "\n" << "  inf.fps :     " << "\n" << inference_fps_sstr.str().c_str() << "\n" << "  inf.time :     " << "\n" << inference_time_sstr.str().c_str() << "\n" <<"  accuracy :     " << "\n" << accuracy_sstr.str().c_str();
			char *label_to_display = g_strdup_printf ("<span line_height=\"1\"  font=\"%d\" color=\"#FFFFFFFF\">""<b>%s\n</b>""</span>",data->ui_cairo_font_size,info_sstr.str().c_str());
			gtk_label_set_markup(GTK_LABEL(data->info_inf_time),label_to_display);
			g_free(label_to_display);
		}
		if(exit_application)
			gtk_main_quit();
	} else {
		/* Still picture use case */
		if (first_call_overlay){
			first_call_overlay = false;
			/* add the inference function in idlle after the initialization of
			 * the GUI */
			g_idle_add((GSourceFunc)infer_new_picture,data);
			data->new_inference = true;
			return FALSE;
		} else {
			/*  Update labels */
			std::stringstream info_sstr;
			info_sstr << "  inf.fps :     " << "\n" << inference_fps_sstr.str().c_str() << "\n" << "  inf.time :     " << "\n" << inference_time_sstr.str().c_str() << "\n" <<"  accuracy :     " << "\n" << accuracy_sstr.str().c_str();
			char *label_to_display = g_strdup_printf ("<span line_height=\"1\"  font=\"%d\" color=\"#FFFFFFFF\">""<b>%s\n</b>""</span>",data->ui_cairo_font_size,info_sstr.str().c_str());
			gtk_label_set_markup(GTK_LABEL(data->info_inf_time),label_to_display);
			g_free(label_to_display);
		}
	}

	/* Display the label centered in the preview area */
	std::stringstream label_sstr;
	cairo_text_extents_t extents;
	double x, y;
	label_sstr << labels[results.index[0]];
	if (!data->preview_enabled) {
		draw_width = data->widget_draw_width;
		draw_height=  data->widget_draw_height;
	}

	cairo_text_extents(cr, label_sstr.str().c_str(), &extents);
	x = (draw_width/ 2) - ((extents.width / 2) + extents.x_bearing);
	y = draw_height + (extents.y_bearing / 2);
	cairo_move_to(cr, x, y);
	gui_display_outlined_text(cr, label_sstr.str().c_str());
	cairo_fill (cr);

	/*  Validation mode  */
	if(validation){
		if (data->preview_enabled) {
			if (data->valid_draw_count < 5){
				g_source_remove(data->valid_timeout_id);
				data->valid_timeout_id = g_timeout_add(100000,
													   valid_timeout_callback,
													   NULL);
				data->valid_draw_count++;
			} else {
				data->valid_inference_time.push_back(results.inference_time);
				/* Reload the timeout */
				g_source_remove(data->valid_timeout_id);
				data->valid_timeout_id = g_timeout_add(100000,
													   valid_timeout_callback,
													   NULL);
				data->valid_draw_count++;
				if (data->valid_draw_count > data->val_run) {
					auto avg_inf_time = std::accumulate(data->valid_inference_time.begin(),
														data->valid_inference_time.end(), 0.0) /
														data->valid_inference_time.size();
					/* Stop the timeout and properly exit the
					 * application */
					std::cout << "avg display fps= " << display_avg_fps << std::endl;
					std::cout << "avg inference fps= " << (1000 / avg_inf_time) << std::endl;
					std::cout << "avg inference time= " << avg_inf_time << std::endl;
					g_source_remove(data->valid_timeout_id);
					gtk_main_quit();
				}
			}
		}
	}
	return TRUE;
}
/**
 * This function is creating GTK widget to create the window to overlay UI
 * information
 */
static void gui_create_overlay(CustomData *data)
{
	/*Gtk Widget used for the window */
	GtkWidget *main_box;
	GtkWidget *drawing_box;
	GtkWidget *drawing_area;
	GtkWidget *still_pict_draw;
	GtkWidget *info_box;
	GtkWidget *exit_box;
	GtkWidget *st_icon_event_box;
	GtkWidget *exit_icon_event_box;

	/* Create the window */
	data->window_overlay = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(data->window_overlay, TRUE);
	g_signal_connect(G_OBJECT(data->window_overlay), "delete-event",
			 G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(data->window_overlay), "destroy",
			 G_CALLBACK(gtk_main_quit), NULL);

	/* Remove title bar */
	gtk_window_set_decorated(GTK_WINDOW(data->window_overlay), FALSE);

	/* Maximize the window size */
	gtk_window_maximize(GTK_WINDOW(data->window_overlay));

	/* Create the ST icon widget */
	data->st_icon_ov = gtk_image_new();
	st_icon_event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(st_icon_event_box), data->st_icon_ov);
	g_signal_connect(G_OBJECT(st_icon_event_box),"button_press_event",G_CALLBACK(st_icon_cb), data);

	/* Create the exit icon widget */
	data->exit_icon_ov = gtk_image_new();
	exit_icon_event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(exit_icon_event_box), data->exit_icon_ov);
	g_signal_connect(G_OBJECT(exit_icon_event_box),"button_press_event",G_CALLBACK(exit_icon_cb), data);

	gtk_image_set_from_file(GTK_IMAGE(data->st_icon_ov), data->st_icon_sstr.str().c_str());
	gtk_image_set_from_file(GTK_IMAGE(data->exit_icon_ov), data->exit_icon_sstr.str().c_str());

	if (data->preview_enabled) {
		/*  Camera preview use case */
		/* Create the drawing area to draw text on it using cairo */
		drawing_area = gtk_drawing_area_new();
		gtk_widget_set_app_paintable(drawing_area, TRUE);
		g_signal_connect(G_OBJECT(drawing_area), "draw",
				 G_CALLBACK(gui_draw_overlay_cb), data);

		data->info_inf_time = gtk_label_new(NULL);
		gtk_label_set_justify(GTK_LABEL(data->info_inf_time),GTK_JUSTIFY_CENTER);
		/*  initialize labels */
		std::stringstream info_sstr;
		info_sstr << "  disp.fps :     " << "\n" << "  inf.fps :     " << "\n" << "  inf.time :     " << "\n" <<"  accuracy :     " << "\n";
		char *label_to_display = g_strdup_printf ("<span line_height=\"1\"  font=\"%d\" color=\"#000000\">""<b>%s\n</b>""</span>",data->ui_cairo_font_size,info_sstr.str().c_str());
		gtk_label_set_markup(GTK_LABEL(data->info_inf_time),label_to_display);
		g_free(label_to_display);

	} else {
		/*  Still picture use case */
		/* Create the video widget */
		still_pict_draw = gtk_drawing_area_new();
		gtk_widget_set_app_paintable(still_pict_draw, TRUE);
		g_signal_connect(still_pict_draw, "draw",
				 G_CALLBACK(gui_draw_overlay_cb), data);

		data->info_inf_time = gtk_label_new(NULL);
		gtk_label_set_justify(GTK_LABEL(data->info_inf_time),GTK_JUSTIFY_CENTER);
		/*  initialize labels */
		std::stringstream info_sstr;
		info_sstr << "  inf.fps :     "  << "\n" << "  inf.time :     " << "\n" <<"  accuracy :     " << "\n";
		char *label_to_display = g_strdup_printf ("<span line_height=\"1\"  font=\"%d\" color=\"#000000\">""<b>%s\n</b>""</span>",data->ui_cairo_font_size,info_sstr.str().c_str());
		gtk_label_set_markup(GTK_LABEL(data->info_inf_time),label_to_display);
		g_free(label_to_display);
	}

	/* Set the boxes */
	info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(info_box , "gui_overlay_stbox");
	if (data->preview_enabled){
		/* Camera preview use case */
		gtk_box_pack_start(GTK_BOX(info_box), st_icon_event_box, FALSE, FALSE, 4);
		gtk_box_pack_start(GTK_BOX(info_box), data->info_inf_time, FALSE, FALSE, 4);

	} else {
		/*  Still picture use case */
		gtk_box_pack_start(GTK_BOX(info_box), st_icon_event_box, FALSE, FALSE, 4);
		gtk_box_pack_start(GTK_BOX(info_box), data->info_inf_time, FALSE, FALSE, 4);

	}

	drawing_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(drawing_box, "gui_overlay_draw");
	if (data->preview_enabled){
		/* Camera preview use case */
		gtk_box_pack_start(GTK_BOX(drawing_box), drawing_area, TRUE, TRUE, 0);
	} else {
		/*  Still picture use case */
		gtk_box_pack_start(GTK_BOX(drawing_box), still_pict_draw, TRUE, TRUE, 0);
	}

	exit_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(exit_box , "gui_overlay_exit");
	gtk_box_pack_start(GTK_BOX(exit_box), exit_icon_event_box, FALSE, FALSE, 2);

	main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(main_box, "gui_overlay");
	gtk_box_pack_start(GTK_BOX(main_box), info_box, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_box), drawing_box, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(main_box), exit_box , FALSE, FALSE, 2);

	/* Push the UI into the window_overlay */
	gtk_container_add(GTK_CONTAINER(data->window_overlay), main_box);
	gtk_widget_show_all(data->window_overlay);
}

static gboolean gui_draw_main_cb(GtkWidget *widget,
				 cairo_t *cr,
				 CustomData *data)
{
	int offset = 0;
	if (data->preview_enabled) {
		/* Camera preview use case */
		data->widget_draw_width = gtk_widget_get_allocated_width(widget);
		data->widget_draw_height= gtk_widget_get_allocated_height(widget);
	} else {
		/* Still picture use case */
		data->widget_draw_width = gtk_widget_get_allocated_width(widget);
		data->widget_draw_height = gtk_widget_get_allocated_height(widget);
		data->offset = (data->widget_draw_width - data->frame_disp_pos.width)/2;

		if(data->img_to_display.data){
			/*  Display the resized picture */
			/* Generate the cairo surface */
			int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24,
								   data->frame_disp_pos.width);

			cairo_surface_t *picture = cairo_image_surface_create_for_data(data->img_to_display.data,
									    CAIRO_FORMAT_RGB24,
									    data->frame_disp_pos.width,
									    data->frame_disp_pos.height,
									    stride);
			cairo_set_source_surface(cr,picture,data->offset,0);
			cairo_paint(cr);
			cairo_surface_destroy(picture);
		} else {
			/* Set the black background */
			cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
			cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
			cairo_paint(cr);
			data->new_inference = true;
		}
	}
	return FALSE;
}

/**
 * This function is creating GTK widget to create the main window use to render
 * the preview.
 * Title label are also used in the main window in order to create boxes with
 * sizes that match the label width.
 */
static void gui_create_main(CustomData *data)
{
	/*Gtk Widget used for the window */
	GtkWidget *main_box;
	GtkWidget *video_box;
	GtkWidget *still_pict_draw;
	GtkWidget *overlay;
	GtkWidget *info_box;
	GtkWidget *exit_box;
	GtkWidget *st_icon_event_box;
	GtkWidget *exit_icon_event_box;

	/* Create the window */
	data->window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	/* Remove title bar */
	gtk_window_set_decorated(GTK_WINDOW(data->window_main), FALSE);

	/* Maximize the window size */
	gtk_window_maximize(GTK_WINDOW(data->window_main));

	gtk_widget_set_app_paintable(data->window_main, TRUE);
	g_signal_connect(G_OBJECT(data->window_main), "delete-event",G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(data->window_main), "destroy",G_CALLBACK(gtk_main_quit), NULL);

	/* Create the ST icon widget */
	data->st_icon_main = gtk_image_new();
	st_icon_event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(st_icon_event_box), data->st_icon_main);
	g_signal_connect(G_OBJECT(st_icon_event_box),"button_press_event",G_CALLBACK(st_icon_cb), data);

	/* Create the exit icon widget */
	data->exit_icon_main = gtk_image_new();
	exit_icon_event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(exit_icon_event_box), data->exit_icon_main);
	g_signal_connect(G_OBJECT(exit_icon_event_box),"button_press_event",G_CALLBACK(exit_icon_cb), data);

	gtk_image_set_from_file(GTK_IMAGE(data->st_icon_main), data->st_icon_sstr.str().c_str());
	gtk_image_set_from_file(GTK_IMAGE(data->exit_icon_main), data->exit_icon_sstr.str().c_str());

	data->overlay_draw = gtk_drawing_area_new();
	gtk_widget_set_app_paintable(data->overlay_draw, TRUE);
	if (data->preview_enabled) {
		g_signal_connect(data->overlay_draw, "draw",G_CALLBACK(gui_draw_main_cb), data);
	}
	if (data->preview_enabled) {
		/* Camera preview use case */
		/* Create the video widget */
		GstElement *sink = gst_bin_get_by_name (GST_BIN (data->pipeline), "gtkwsink");
		if (!sink && !g_strcmp0 (G_OBJECT_TYPE_NAME (data->pipeline), "GstPlayBin")) {
			g_object_get (data->pipeline, "video-sink", &sink, NULL);
			if (sink && g_strcmp0 (G_OBJECT_TYPE_NAME (sink), "GstGtkWaylandSink") != 0
			    && GST_IS_BIN (sink)) {
				GstBin *sinkbin = GST_BIN (sink);
				sink = gst_bin_get_by_name (sinkbin, "gtkwsink");
				gst_object_unref (sinkbin);
			}
		}
		g_assert (sink);
		g_assert (!g_strcmp0 (G_OBJECT_TYPE_NAME (sink), "GstGtkWaylandSink"));
		g_object_get (sink, "widget", &data->video, NULL);
		gtk_widget_set_app_paintable(GTK_WIDGET(data->video), TRUE);

		data->info_inf_time_main = gtk_label_new(NULL);
		gtk_label_set_justify(GTK_LABEL(data->info_inf_time_main),GTK_JUSTIFY_CENTER);
		/*  initialize labels */
		std::stringstream info_sstr;
		info_sstr << "  disp.fps :     " << "\n" << "  inf.fps :     " << "\n" << "  inf.time :     " << "\n" << "  accuracy :     " << "\n";
		char *label_to_display = g_strdup_printf ("<span line_height=\"1\"  font=\"%d\" color=\"#000000\">""<b>%s\n</b>""</span>",data->ui_cairo_font_size,info_sstr.str().c_str());
		gtk_label_set_markup(GTK_LABEL(data->info_inf_time_main),label_to_display);
		g_free(label_to_display);

	} else {
		/*  Still picture preview use case */
		/* Create the drawing widget */
		still_pict_draw = gtk_drawing_area_new();
		gtk_widget_set_app_paintable(still_pict_draw, TRUE);
		g_signal_connect(still_pict_draw, "draw",
				 G_CALLBACK(gui_draw_main_cb), data);

		data->info_inf_time_main = gtk_label_new(NULL);
		gtk_label_set_justify(GTK_LABEL(data->info_inf_time_main),GTK_JUSTIFY_CENTER);
		/*  initialize labels */
		std::stringstream info_sstr;
		info_sstr << "  inf.fps :     "  << "\n" << "  inf.time :     " << "\n" << "  accuracy :     " << "\n";
		char *label_to_display = g_strdup_printf ("<span line_height=\"1\"  font=\"%d\" color=\"#000000\">""<b>%s\n</b>""</span>",data->ui_cairo_font_size,info_sstr.str().c_str());
		gtk_label_set_markup(GTK_LABEL(data->info_inf_time_main),label_to_display);
		g_free(label_to_display);
	}
	/* Set the boxes */
	info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(info_box, "gui_main_stbox");
	if (data->preview_enabled){
		/*  Camera preview use case */
		gtk_box_pack_start(GTK_BOX(info_box), st_icon_event_box, FALSE, FALSE, 4);
		gtk_box_pack_start(GTK_BOX(info_box), data->info_inf_time_main, TRUE, FALSE, 4);
	} else {
		/*  Still picture use case */
		gtk_box_pack_start(GTK_BOX(info_box), st_icon_event_box, FALSE, FALSE, 4);
		gtk_box_pack_start(GTK_BOX(info_box), data->info_inf_time_main, TRUE, FALSE, 4);
	}
	video_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_app_paintable(GTK_WIDGET(video_box), TRUE);
	gtk_widget_set_name(video_box, "gui_main_video");
	if (data->preview_enabled){
		/*  Camera preview use case  */
		gtk_box_pack_start(GTK_BOX(video_box), GTK_WIDGET(data->video), TRUE, TRUE, 0);
	} else {
		/*  Still picture use case */
		gtk_box_pack_start(GTK_BOX(video_box), still_pict_draw, TRUE, TRUE, 0);
	}
	exit_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(exit_box, "gui_main_exit");
	gtk_box_pack_start(GTK_BOX(exit_box), exit_icon_event_box, FALSE, FALSE, 2);

	main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_app_paintable(GTK_WIDGET(main_box), TRUE);
	gtk_widget_set_name(main_box, "gui_main");
	gtk_box_pack_start(GTK_BOX(main_box), info_box, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_box), video_box, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(main_box), exit_box, FALSE, FALSE, 2);

	overlay = gtk_overlay_new();
	gtk_container_add(GTK_CONTAINER(overlay),main_box);
	gtk_overlay_add_overlay(GTK_OVERLAY(overlay), data->overlay_draw);
	gtk_container_add(GTK_CONTAINER(data->window_main),overlay);
 	gtk_widget_show_all(data->window_main);
}

/**
 * This function is called when appsink Gstreamer element receives a buffer
 */
static GstFlowReturn gst_new_sample_cb(GstElement *sink, CustomData *data)
{
	GstSample *sample;
	GstBuffer *app_buffer, *buffer;
	GstMapInfo info;

	/* Retrieve the buffer */
	g_signal_emit_by_name (sink, "pull-sample", &sample);
	if (sample) {

		update_isp_config(data);
        data->cpt_frame += 1;
    	if (data->cpt_frame == 30)
            data->cpt_frame = 0;

		buffer = gst_sample_get_buffer (sample);

		/* Make a copy */
		app_buffer = gst_buffer_ref (buffer);

		gst_buffer_map(app_buffer, &info, GST_MAP_READ);

		/* Execute the inference */
		nn_inference(info.data);

		gst_buffer_unmap(app_buffer, &info);
		gst_buffer_unref (app_buffer);

		/* We don't need the appsink sample anymore */
		gst_sample_unref (sample);

		/* Call application callback only in playing state */
		gst_element_post_message(sink,
					 gst_message_new_application(GST_OBJECT(sink),
					 gst_structure_new_empty("inference-done")));
		return GST_FLOW_OK;
	}
	return GST_FLOW_ERROR;
}

/**
 * This function is called by Gstreamer fpsdisplaysink to get fps measurment
 * of display
 */
void gst_fps_measure_display_cb(GstElement *fpsdisplaysink,
				gdouble fps,
				gdouble droprate,
				gdouble avgfps,
				gpointer data)
{
	display_avg_fps = avgfps;
}

/**
 * This function is called when a Gstreamer error message is posted on the bus.
 */
static void gst_error_cb (GstBus *bus, GstMessage *msg, CustomData *data)
{
	GError *err;
	gchar *debug_info;

	/* Print error details on the screen */
	gst_message_parse_error (msg, &err, &debug_info);
	g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
	g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
	g_clear_error(&err);
	g_free(debug_info);

	/* Set the pipeline to READY (which stops camera streaming) */
	gst_element_set_state(data->pipeline, GST_STATE_READY);
}

/**
 * This function is called when a Gstreamer End-Of-Stream message is posted on
 * the bus. We just set the pipeline to READY (which stops camera streaming).
 */
static void gst_eos_cb (GstBus *bus, GstMessage *msg, CustomData *data)
{
	g_print("End-Of-Stream reached.\n");
	gst_element_set_state(data->pipeline, GST_STATE_READY);
}

static void gst_state_changed_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
	GstState oldState;
	GstState newState;
	gst_message_parse_state_changed(msg, &oldState, &newState, 0);
	if (newState == GST_STATE_PLAYING && oldState == GST_STATE_PAUSED) {
		gst_debug_bin_to_dot_file(GST_BIN(data->pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline_cpp_PAUSED_PLAYING");
	}
}

/**
 * This function is called when a Gstreamer "application" message is posted on
 * the bus.
 * Here we retrieve the message posted by the appsink gst_new_sample_cb callback.
 */
static void gst_application_cb (GstBus *bus, GstMessage *msg, CustomData *data)
{
	if (g_strcmp0(gst_structure_get_name(gst_message_get_structure(msg)), "inference-done") == 0) {
		/* If the message is the "inference-done", update the GTK UI */
		gtk_widget_queue_draw(data->window_overlay);
	}
}

/**
 * Construct the Gstreamer pipeline used to stream camera frame and to run
 * TensorFlow Lite inference engine using appsink.
 */
static int gst_pipeline_camera_creation(CustomData *data)
{
	GstStateChangeReturn ret;
	GstElement *pipeline, *source, *dispsink, *tee, *scale, *framerate;
	GstElement *queue1, *queue2, *convert, *convert2, *appsink, *framecrop;
	GstElement *fpsmeasure1;
	GstBus *bus;

	/* Create the pipeline */
	pipeline  = gst_pipeline_new("Image classification live camera");
	data->pipeline = pipeline;

	/* Create gstreamer elements */
	source      = gst_element_factory_make_full("v4l2src","name","camera-source","io-mode",0,NULL);
	tee         = gst_element_factory_make("tee",            "frame-tee");
	queue1      = gst_element_factory_make("queue",          "queue-1");
	queue2      = gst_element_factory_make("queue",          "queue-2");
	convert     = gst_element_factory_make("videoconvert",   "video-convert");
	convert2    = gst_element_factory_make("videoconvert",   "video-convert-wayland");
	framecrop   = gst_element_factory_make("videocrop",      "videocrop");
	scale       = gst_element_factory_make("videoscale",     "videoscale");
	dispsink    = gst_element_factory_make_full("gtkwaylandsink", "name", "gtkwsink", "drm-device",NULL,NULL);
	appsink     = gst_element_factory_make("appsink",        "app-sink");
	framerate   = gst_element_factory_make("videorate",      "video-rate");
	fpsmeasure1 = gst_element_factory_make("fpsdisplaysink", "fps-measure1");

	if (!pipeline || !source || !tee || !queue1 || !queue2 || !convert || !convert2 ||
	    !scale || !dispsink || !appsink || !framerate || !fpsmeasure1  || !framecrop) {
		g_printerr("One element could not be created. Exiting.\n");
		return -1;
	}

	/* Configure the source element */
	std::stringstream device_sstr;
	device_sstr << "/dev/" << data->camera_info.video_device;
	g_object_set(G_OBJECT(source), "device", device_sstr.str().c_str(), NULL);

	g_print("video sink used : gtkwaylandsink \n");

	/* Configure the queue elements */
	g_object_set(G_OBJECT(queue1), "max-size-buffers", 1, "leaky", 2 /* downstream */, NULL);
	g_object_set(G_OBJECT(queue2), "max-size-buffers", 1, "leaky", 2 /* downstream */, NULL);

	/* Create caps based on application parameters */
	std::stringstream sourceCaps_sstr;
	sourceCaps_sstr << data->camera_info.camera_caps << ",framerate=" << camera_fps_str << "/1";
	g_print("camera pipeline configuration : %s \n",sourceCaps_sstr.str().c_str());
	GstCaps *sourceCaps = gst_caps_from_string(sourceCaps_sstr.str().c_str());
	std::stringstream scaleCaps_sstr;
	scaleCaps_sstr << "video/x-raw,format=RGB,width=" << data->nn_input_width << ",height=" << data->nn_input_height;
	GstCaps *scaleCaps  = gst_caps_from_string(scaleCaps_sstr.str().c_str());

	/* Configure fspdisplaysink */
	g_object_set(fpsmeasure1, "signal-fps-measurements", TRUE,
		     "fps-update-interval", 2000, "text-overlay", FALSE,
		     "video-sink", dispsink, NULL);
	g_signal_connect(fpsmeasure1, "fps-measurements", G_CALLBACK(gst_fps_measure_display_cb), NULL);

	/* Configure the videocrop */
	if (crop) {
		/* Crop requested */
		int left   = cropRect.x;
		int right  = data->frame_width - left - cropRect.width;
		int top    = cropRect.y;
		int bottom = data->frame_height - top - cropRect.height;
		g_object_set(framecrop, "left", left, "right", right,
			     "top", top, "bottom", bottom, NULL);
	} else {
		/* No crop requested */
		g_object_set(framecrop, "left", 0, "right", 0,
			     "top", 0, "bottom", 0, NULL);
	}

	/* Configure appsink */
	g_object_set(appsink, "emit-signals", TRUE, "sync", FALSE,
		     "max-buffers", 1, "drop", TRUE, "caps", scaleCaps, NULL);
	g_signal_connect(appsink, "new-sample", G_CALLBACK(gst_new_sample_cb), data);

	/* Add all elements into the pipeline */
	gst_bin_add_many(GST_BIN(pipeline),
			 source, framerate, tee, convert, convert2, queue1, queue2, scale,
			 framecrop, fpsmeasure1, appsink, NULL);

	/* Link the elements together */
	if (!gst_element_link_many(source, framerate, NULL)) {
		g_error("Failed to link elements (1)");
		return -2;
	}
	if (!gst_element_link_filtered(framerate, tee, sourceCaps)) {
		g_error("Failed to link elements (2)");
		return -2;
	}
	if (!gst_element_link_many(tee, queue2, convert, framecrop, scale, appsink, NULL)) {
		g_error("Failed to link elements (3)");
		return -2;
	}
	if (!gst_element_link_many(tee, queue1, convert2, fpsmeasure1, NULL)) {
		g_error("Failed to link elements (4)");
		return -2;
	}

	gst_caps_unref(sourceCaps);
	gst_caps_unref(scaleCaps);

	/* Instruct the bus to emit signals for each received message, and
	 * connect to the interesting signals */
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_signal_watch(bus);
	g_signal_connect(G_OBJECT(bus), "message::error", (GCallback)gst_error_cb, data);
	g_signal_connect(G_OBJECT(bus), "message::eos", (GCallback)gst_eos_cb, data);
	g_signal_connect(G_OBJECT(bus), "message::application", (GCallback)gst_application_cb, data);
	g_signal_connect(G_OBJECT(bus), "message::state-changed", (GCallback)gst_state_changed_cb, data);
	gst_object_unref(bus);
	return 0;
}

/**
 * This function display the help when -h or --help is passed as parameter.
 */
static void print_help(int argc, char** argv)
{
	std::cout <<
		"Usage: " << argv[0] << " -m <model .nb> -l <label .txt file>\n"
		"\n"
		"-m --model_file <.nb file path>:  	   .nb model to be executed\n"
		"-l --label_file <label file path>:    name of file containing labels\n"
		"-i --image <directory path>:          image directory with image to be classified\n"
		"-v --video_device <n>:                video device is automatically detected but can be set (example video0)\n"
		"--crop:                               if set, the nn input image is cropped (with the expected nn aspect ratio) before being resized,\n"
		"                                      else the nn input image is only resized to the nn input size (could cause picture deformation).\n"
		"--frame_width  <val>:                 width of the camera frame (default is 640)\n"
		"--frame_height <val>:                 height of the camera frame (default is 480)\n"
		"--framerate <val>:                    framerate of the camera (default is 15fps)\n"
		"--input_mean <val>:                   model input mean (default is 127.5)\n"
		"--input_std  <val>:                   model input standard deviation (default is 127.5)\n"
		"--verbose:                            enable verbose mode\n"
		"--validation:                         enable the validation mode\n"
		"--val_run:                            set the number of draws in the validation mode\n"
		"--help:                               show this help\n";
	exit(1);
}

/**
 * This function parse the parameters of the application -m and -l ar
 * mandatory.
 */
#define OPT_FRAME_WIDTH  1000
#define OPT_FRAME_HEIGHT 1001
#define OPT_FRAMERATE    1002
#define OPT_INPUT_MEAN   1003
#define OPT_INPUT_STD    1004
#define OPT_VERBOSE      1005
#define OPT_VALIDATION   1006
#define OPT_VAL_RUN      1008

void process_args(int argc, char** argv)
{
	const char* const short_opts = "m:l:i:v:c:e:n:h";
	const option long_opts[] = {
		{"model_file",   required_argument, nullptr, 'm'},
		{"label_file",   required_argument, nullptr, 'l'},
		{"image",        required_argument, nullptr, 'i'},
		{"video_device", no_argument      , nullptr, 'v'},
		{"crop",         no_argument,       nullptr, 'c'},
		{"frame_width",  required_argument, nullptr, OPT_FRAME_WIDTH},
		{"frame_height", required_argument, nullptr, OPT_FRAME_HEIGHT},
		{"framerate",    required_argument, nullptr, OPT_FRAMERATE},
		{"input_mean",   required_argument, nullptr, OPT_INPUT_MEAN},
		{"input_std",    required_argument, nullptr, OPT_INPUT_STD},
		{"verbose",      no_argument,       nullptr, OPT_VERBOSE},
		{"validation",   no_argument,       nullptr, OPT_VALIDATION},
		{"val_run",      required_argument, nullptr, OPT_VAL_RUN},
		{"help",         no_argument,       nullptr, 'h'},
		{nullptr,        no_argument,       nullptr, 0}
	};

	while (true)
	{
		const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

		if (-1 == opt)
			break;

		switch (opt)
		{
		case 'm':
			model_file_str = std::string(optarg);
			std::cout << "model file set to: " << model_file_str << std::endl;
			break;
		case 'l':
			labels_file_str = std::string(optarg);
			std::cout << "label file set to: " << labels_file_str << std::endl;
			break;
		case 'i':
			image_dir_str = std::string(optarg);
			std::cout << "image directory set to: " << image_dir_str << std::endl;
			break;
		case 'v':
			video_device_str = std::string(optarg);
			std::cout << "camera device set to: /dev/" << video_device_str << std::endl;
			break;
		case 'c':
			crop = true;
			std::cout << "nn input image will be cropped then resized" << std::endl;
			break;
		case OPT_FRAME_WIDTH:
			camera_width_str = std::string(optarg);
			std::cout << "camera frame width set to: " << camera_width_str << std::endl;
			break;
		case OPT_FRAME_HEIGHT:
			camera_height_str = std::string(optarg);
			std::cout << "camera frame height set to: " << camera_height_str << std::endl;
			break;
		case OPT_FRAMERATE:
			camera_fps_str = std::string(optarg);
			std::cout << "camera framerate set to: " << camera_fps_str << std::endl;
			break;
		case OPT_INPUT_MEAN:
			input_mean = std::stof(optarg);
			std::cout << "input mean to: " << input_mean << std::endl;
			break;
		case OPT_INPUT_STD:
			input_std = std::stof(optarg);
			std::cout << "input standard deviation set to: " << input_std << std::endl;
			break;
		case OPT_VERBOSE:
			verbose = true;
			std::cout << "verbose mode enabled" << std::endl;
			break;
		case OPT_VALIDATION:
			validation = true;
			std::cout << "application started in validation mode" << std::endl;
			break;
		case OPT_VAL_RUN:
			val_run_str = std::string(optarg);
			std::cout << "number of draws has been set" << val_run_str << std::endl;
			break;
		case 'h': // -h or --help
		case '?': // Unrecognized option
		default:
			print_help(argc, argv);
			break;
		}
	}

	if (model_file_str.empty() || labels_file_str.empty())
		print_help(argc, argv);
}

/**
 * Function called when CTRL + C or Kill signal is detected
 */
static void int_handler(int dummy)
{
	if (gtk_main_started){
		gtk_main_quit();
	} else {
		exit(1);
	}
}

/**
 * Main function
 */
int main(int argc, char *argv[])
{
	CustomData data;
	int ret;
	size_t label_count;

	/* Catch CTRL + C signal */
	signal(SIGINT, int_handler);
	/* Catch kill signal */
	signal(SIGTERM, int_handler);

	/* Process the application parameters */
	process_args(argc, argv);

	/* Get number of CPU cores available */
	uint8_t nb_cpu_cores = 1;
	const auto processor_count = std::thread::hardware_concurrency();
	std::cout << processor_count << " cpu core(s) available\n";
	if (processor_count)
		nb_cpu_cores = (uint8_t)processor_count;

	/* Initialize our data structure */
	data.pipeline = NULL;
	data.window_main = NULL;
	data.window_overlay = NULL;
	data.new_inference = false;
	data.frame_width  = std::stoi(camera_width_str);
	data.frame_height = std::stoi(camera_height_str);
	data.val_run  = std::stoi(val_run_str);
	data.ui_box_line_width = 2.0;
	data.ui_weston_panel_thickness = 32;

	/* If image_dir is set by the user, test data picture are used instead
	 * of camera frames */
	if (image_dir_str.empty()) {
		data.frame_width  = std::stoi(camera_width_str);
		data.frame_height = std::stoi(camera_height_str);
		data.preview_enabled = true;
		std::stringstream check_camera_cmd;
		int check_camera;
		//Test if a camera is connected
		check_camera_cmd << "source " << RESOURCES_DIRECTORY << "check_camera_preview.sh";
		check_camera = system(check_camera_cmd.str().c_str());
		if (check_camera != 0){
			g_print("no camera connected \n");
			exit(1);
		}
		data.camera_info = setup_camera();
	} else {
		data.preview_enabled = false;
		/* Check if directory is empty */
		std::string file = get_files_in_directory_randomly(image_dir_str);
		if (file.empty()) {
			g_printerr("ERROR: Image directory %s is empty\n", image_dir_str.c_str());
			exit(1);
		} else {
			/* Reset the dir_file variable */
			dir_files.erase(dir_files.begin(), dir_files.end());
		}
	}

	/* NBG wrapper initialization */
	config.verbose = verbose;
	config.model_name = model_file_str;
	config.labels_file_name = labels_file_str;
	config.number_of_results = 5;

	nbg_wrapper.Initialize(&config);

	if (nbg_wrapper.ReadLabelsFile(config.labels_file_name, &labels, &label_count) != VX_SUCCESS){
		LOG(FATAL) << "ReadLabel error.\n";
		exit(1);
	}

	/* Get model input size */
	data.nn_input_width = nbg_wrapper.GetInputWidth();
	data.nn_input_height = nbg_wrapper.GetInputHeight();

	if (data.preview_enabled) {
		if (crop) {
			int frame_width = data.frame_width;
			int frame_height = data.frame_height;
			int nn_input_width = data.nn_input_width;
			int nn_input_height = data.nn_input_height;

			float nn_input_aspect_ratio = (float)nn_input_width / (float)nn_input_height;

			/* Setup a rectangle to define the crop region */
			if (nn_input_aspect_ratio >  1) {
				cropRect.width = frame_width;
				cropRect.height = int((float)frame_width / (float)nn_input_aspect_ratio);
				cropRect.x = 0;
				cropRect.y = (int)((frame_height - cropRect.height) / 2);
			} else {
				cropRect.width = int((float)frame_height * (float)nn_input_aspect_ratio);
				cropRect.height = frame_height;
				cropRect.x = (int)((frame_width - cropRect.width) / 2);
				cropRect.y = 0;
			}
		}
	}

	/* Initialize GTK */
	gtk_init(&argc, &argv);
	get_display_resolution(&data);
	gui_gtk_style(&data);
	gui_set_ui_parameters(&data);
	gui_set_icons_parameters(&data);

	if (data.preview_enabled) {
		/*  Camera preview use case */
		/* Initialize GStreamer for camera preview use case*/
		gst_init(&argc, &argv);
		/* Create the GStreamer pipeline for camera use case */
		ret = gst_pipeline_camera_creation(&data);
		if(ret)
			exit(1);
	 }

	/* Create the GUI containing the video stream  */
	g_print("Start Creating main GTK window\n");
	gui_create_main(&data);
	if (data.preview_enabled) {
		g_print("gst set pipeline playing state\n");
		gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
	}

	/* Create the second GUI containing the nn information  */
	g_print("Start Creating overlay GTK window\n");
	gui_create_overlay(&data);

	/* Start a timeout timer in validation process to close application if
	 * timeout occurs */
	if (validation) {
		data.valid_draw_count = 0;
		data.valid_timeout_id = g_timeout_add(100000,
						      valid_timeout_callback,
						      NULL);
	}

	gtk_main_started = true;
	gtk_main();
	gtk_main_started = false;

	/* Out of the main loop, clean up nicely */
	if (data.preview_enabled) {
		/* Camera preview use case */
		g_print("Returned, stopping Gst pipeline\n");
		gst_element_set_state(data.pipeline, GST_STATE_NULL);

		g_print("Deleting Gst pipeline\n");
		gst_object_unref(data.pipeline);
	}
	g_print(" Application exited properly \n");
	return 0;
}
