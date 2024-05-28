/*
 * wrapper_tfl.hpp
 *
 * Author: Vincent Abriou <vincent.abriou@st.com> for STMicroelectronics.
 *
 * Copyright (c) 2020 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *
 *     http://www.opensource.org/licenses/BSD-3-Clause
 *
 *
 *
 * Inspired by:
 * https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/examples/label_image
 * Copyright 2017 The TensorFlow Authors. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef WRAPPER_NBG_HPP_
#define WRAPPER_NBG_HPP_

#include <algorithm>
#include <functional>
#include <queue>
#include <memory>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <getopt.h>
#include <iostream>
#include <time.h>
#include <pthread.h>
#include "vnn_utils.h"

#define GPU_CLK_FD "/sys/kernel/debug/gc/clk"
#define NUM_OF_INPUT_MAX 32
#define NUM_OF_OUTPUT_MAX 32
#define BILLION 1000000000
#define LOG(x) std::cerr

namespace wrapper_nbg
{

	double get_ms(struct timeval t) { return (t.tv_sec * 1000 + t.tv_usec / 1000); }

	struct Config
	{
		bool verbose;
		std::string input_file;
		std::string model_name;
		std::string labels_file_name;
		int number_of_results = 5;
	};

	struct Label_Results
	{
		float accuracy[10];
		int index[10];
		float inference_time;
	};

	class NBG_Wrapper
	{
	private:
		vx_context m_context;
		vx_graph m_graph;
		vx_kernel m_kernel;
		vx_node m_node;
		vx_status m_status;
		vx_tensor m_input_tensor;
		vx_tensor m_output_tensor;
		vx_int32 m_input_count;
		vx_int32 m_output_count;
		vx_uint32 m_width;
		vx_uint32 m_height;
		bool m_verbose;
		bool m_inputFloating;
		float m_inferenceTime;
		int m_numberOfResults;
		std::string m_model;
		inout_obj m_inputs[NUM_OF_INPUT_MAX];
		inout_obj m_outputs[NUM_OF_OUTPUT_MAX];
		inout_param m_inputs_param[NUM_OF_INPUT_MAX];
		inout_param m_outputs_param[NUM_OF_OUTPUT_MAX];

	public:
		NBG_Wrapper() {}

		void Initialize(Config *conf)
		{
			m_context 			= {NULL};
			m_graph 			= {NULL};
			m_kernel 			= {NULL};
			m_node 				= {NULL};
			m_status 			= VX_FAILURE;
			m_input_tensor 		= NULL;
			m_output_tensor 	= NULL;
			m_input_count 		= 0;
			m_output_count 		= 0;
			m_width 			= 0;
			m_height			= 0;
			m_inputFloating 	= false;
			m_inferenceTime 	= 0;
			m_model 			= conf->model_name;
			m_verbose 			= conf->verbose;
			m_numberOfResults 	= conf->number_of_results;

			if (!m_model.c_str())
			{
				LOG(ERROR) << "no model file name\n";
				exit(-1);
			}
			LOG(INFO) << "Loaded model " << m_model << "\n";
			char *char_model = const_cast<char *>(m_model.c_str());

			ZEROS(m_inputs_param);
			ZEROS(m_outputs_param);
			m_context = vxCreateContext();
			if (m_context == NULL)
			{
				LOG(FATAL) << "create fail: file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}

			m_graph = vxCreateGraph(m_context);
			if (m_graph == NULL)
			{
				LOG(FATAL) << "create fail: file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}

			m_kernel = vxImportKernelFromURL(m_context, VX_VIVANTE_IMPORT_KERNEL_FROM_FILE, char_model);
			m_status = vxGetStatus((vx_reference)m_kernel);
			if (m_status != VX_SUCCESS)
			{
				LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}

			m_node = vxCreateGenericNode(m_graph, m_kernel);
			m_status = vxGetStatus((vx_reference)m_node);
			if (m_status != VX_SUCCESS)
			{
				LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}

			m_status = vnn_QueryInputsAndOutputsParam(m_kernel, m_inputs_param, &m_input_count, m_outputs_param, &m_output_count);
			if (m_status != VX_SUCCESS)
			{
				LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}

			for (int j = 0; j < m_input_count; j++)
			{
				m_status |= vnn_CreateObject(m_context, &m_inputs_param[j], &m_inputs[j]);
				if (m_status != VX_SUCCESS)
				{
					LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
					exit(1);
				}
				m_status |= vxSetParameterByIndex(m_node, j, (vx_reference)m_inputs[j].u.ref);
				if (m_status != VX_SUCCESS)
				{
					LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
					exit(1);
				}
			}
			for (int j = 0; j < m_output_count; j++)
			{
				m_status |= vnn_CreateObject(m_context, &m_outputs_param[j], &m_outputs[j]);
				if (m_status != VX_SUCCESS)
				{
					LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
					exit(1);
				}
				m_status |= vxSetParameterByIndex(m_node, j + m_input_count, (vx_reference)m_outputs[j].u.ref);
				;
				if (m_status != VX_SUCCESS)
				{
					LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
					exit(1);
				}
			}

			uint64_t tmsStart, tmsEnd, msVal, usVal;
			LOG(INFO) << "Info: Compiling and verifying graph...\n";
			tmsStart = get_perf_count();
			m_status = vxVerifyGraph(m_graph);
			if (m_status != VX_SUCCESS)
			{
				LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}
			tmsEnd = get_perf_count();
			msVal = (tmsEnd - tmsStart) / 1000000;
			usVal = (tmsEnd - tmsStart) / 1000;
			LOG(INFO) << "Info: Verifying graph took: " << msVal << " ms  |  " << usVal << " us\n";

			inout_obj *obj = &m_inputs[0];
			m_input_tensor = obj->u.tensor;
			vx_int32 num_of_dims = vnn_GetTensorDims(m_input_tensor);
			vx_uint32 size[6];
			vxQueryTensor(m_input_tensor, VX_TENSOR_DIMS, size, sizeof(size));
			m_width = size[1];
			m_height = size[2];
		}

		static uint64_t get_perf_count()
		{
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			return (uint64_t)((uint64_t)ts.tv_nsec + (uint64_t)ts.tv_sec * BILLION);
		}

		void RunInference(uint8_t *img, Label_Results *results)
		{
			uint64_t tmsStart, tmsEnd;
			float msAvg, usAvg;
			float rUtil = 0;
			float rtime = 0;

			vnn_CopyDataToTensor(m_input_tensor, img);
			if (m_status != VX_SUCCESS)
			{
				LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}

			tmsStart = get_perf_count();
			m_status = vxProcessGraph(m_graph);
			tmsEnd = get_perf_count();
			if (m_status != VX_SUCCESS)
			{
				LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}
			msAvg = (float)(tmsEnd - tmsStart) / 1000000;
			usAvg = (float)(tmsEnd - tmsStart) / 1000;
			m_inferenceTime = msAvg;

			if (m_verbose)
			{
				LOG(INFO) << "Info: Initialized the graph\n";
				LOG(INFO) << "Info: Inf time Average: " << msAvg << " ms  |  " << usAvg << " us\n";
			}

			vx_float32 *buf = NULL;
			vx_int32 num = vnn_GetTensorSize(m_outputs->u.tensor);
			if (m_verbose)
				LOG(INFO) << "Num of output: " << num << "\n";
			uint32_t MaxClass[5];
			vx_float32 fMaxProb[5];
			m_status |= vnn_CopyTensorToFloat32Data(m_outputs->u.tensor, &buf);
			if (m_status != VX_SUCCESS)
			{
				LOG(FATAL) << "process fail: status=" << m_status << ", file=" << __FILE__ << ",line =" << __LINE__ << "\n";
				exit(1);
			}

			if (!get_top(buf, fMaxProb, MaxClass, num, m_numberOfResults))
			{
				LOG(FATAL) << "Fail to show result.\n";
				if (buf)
				{
					free(buf);
					exit(1);
				}
			}

			/* Get results */
			for (int i = 0; i < m_numberOfResults; i++)
			{
				if (m_verbose)
				{
					LOG(INFO) << "________________________________________\n________________________________________\n";
					LOG(INFO) << MaxClass[i] << " : " << fMaxProb[i] << "\n";
					LOG(INFO) << "________________________________________\n";
				}
				results->accuracy[i] = fMaxProb[i];
				results->index[i] = MaxClass[i];
			}
			results->inference_time = m_inferenceTime;

			if (buf)
			{
				free(buf);
			}
		}

		int GetInputWidth()
		{
			return int(m_width);
		}

		int GetInputHeight()
		{
			return int(m_height);
		}

		// Takes a file name, and loads a list of labels from it, one per line, and
		// returns a vector of the strings. It pads with empty strings so the length
		// of the result is a multiple of 16, because our model expects that.
		vx_status ReadLabelsFile(const std::string &file_name,
								 std::vector<std::string> *result,
								 size_t *found_label_count)
		{
			std::ifstream file(file_name);
			if (!file)
			{
				LOG(FATAL) << "Labels file " << file_name << " not found\n";
				return VX_FAILURE;
			}
			result->clear();
			std::string line;
			while (std::getline(file, line))
			{
				result->push_back(line);
			}
			*found_label_count = result->size();
			const int padding = 16;
			while (result->size() % padding)
			{
				result->emplace_back();
			}
			return VX_SUCCESS;
		}
	};

} // namespace wrapper_tfl

#endif // WRAPPER_TFL_HPP_
