/*
 * wrapper_onnx.hpp
 *
 * Author: Vincent Abriou <vincent.abriou@st.com> for STMicroelectronics.
 * Co-Author : Youssef Khemakhem <youssef.khemakhem@st.com> for STMicroelectronics.
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
 */

#ifndef WRAPPER_ONNX_HPP_
#define WRAPPER_ONNX_HPP_

#include <algorithm>
#include <functional>
#include <fstream>
#include <queue>
#include <memory>
#include <string>
#include <sys/time.h>
#include <vector>
#include <cmath>
#include <numeric>
#include "onnxruntime_cxx_api.h"



#define LOG(x) std::cerr



namespace wrapper_onnx {


	double get_ms(struct timeval t) { return (t.tv_sec * 1000 + t.tv_usec / 1000); }
	bool first_call = true;
	struct Config {
		bool verbose;
		float input_mean = 127.5f;
		float input_std = 127.5f;
		int number_of_threads = 2;
		int number_of_results = 5;
		std::string model_name;
		std::string labels_file_name;

	};

	struct ObjDetect_Location {
		float y0, x0, y1, x1;
	};

	struct ObjDetect_Results {
		int classe;
		float score;
		ObjDetect_Location location;
	};

	struct Frame_Results {
        std::vector<ObjDetect_Results> vect_ObjDetect_Results;
		float inference_time;
	};

	std::nullptr_t 	t ;



	class Onnx_Wrapper {
	private:


		Ort::Session                             m_session ;
		Ort::AllocatorWithDefaultOptions 		 m_allocator;
		bool                                     m_verbose;
		bool                                     m_inputFloating;
		float                                    m_inputMean;
		float                                    m_inputStd;
		float                                    m_inferenceTime;
		int                                      m_numberOfThreads;
		int                                      m_numberOfResults;


	public:
		Onnx_Wrapper():m_session(t) {}

		void Initialize(Config* conf)
		{
			m_inputFloating		= false;
			m_inferenceTime		= 0;
			m_verbose		= conf->verbose;
			m_inputMean		= conf->input_mean;
			m_inputStd		= conf->input_std;
			m_numberOfThreads	= conf->number_of_threads;
			m_numberOfResults	= conf->number_of_results;



			if (!conf->model_name.c_str()) {
				LOG(ERROR) << "no model file name\n";
				exit(-1);
			}

			/* create an environment */
			Ort::Env 								 m_env(ORT_LOGGING_LEVEL_WARNING, "Onnx_environment");
			/* create session options */
			Ort::SessionOptions 					 m_session_options;

			/* Define number of threads */
			if (m_numberOfThreads != -1) {
				m_session_options.SetIntraOpNumThreads(m_numberOfThreads);
				if (m_verbose) {
					m_session_options.SetLogSeverityLevel(0);
				}
			}

			m_session_options.DisableCpuMemArena();
			/* create a session from the ONNX model file */

			Ort::Session session(m_env,conf->model_name.c_str(), m_session_options);
			if (session==nullptr)
			{ ORT_CXX_API_THROW("", OrtErrorCode::ORT_NO_MODEL );}

		    LOG(INFO) << "Loaded model " << conf->model_name << "\n";

		    Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
	        auto tensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();
	        if (tensorInfo.GetElementType()== ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT)
	        {
				m_inputFloating = true;
				LOG(INFO) << "Floating point Onnx Model\n";
		    }
	        m_session=std::move(session) ;

		}



		void DisplaySettings()
		{
			LOG(INFO) << "input_floating    " << m_inputFloating << "\n";
			LOG(INFO) << "input_mean        " << m_inputMean << "\n";
			LOG(INFO) << "input_std         " << m_inputStd << "\n";
			LOG(INFO) << "number_of_threads " << m_numberOfThreads << "\n";
			LOG(INFO) << "number_of_results " << m_numberOfResults << "\n";

		}

		void DisplayModelInformation()
		{

			auto input_name = m_session.GetInputNameAllocated(0, m_allocator);
			const size_t num_input_nodes = m_session.GetInputCount();
			size_t num_output_nodes = m_session.GetOutputCount();

			LOG(INFO) << "tensors size: " <<  num_input_nodes + num_output_nodes << "\n";
			LOG(INFO) << "inputs: " << num_input_nodes << "\n";
			LOG(INFO) << "input(0) name: " << input_name.get() << "\n";

			/* Log information about input tensors */
			size_t numInputNodes = m_session.GetInputCount();
            for (size_t i = 0; i < numInputNodes; i++) {
				Ort::TypeInfo inputTypeInfo = m_session.GetInputTypeInfo(i);
				auto tensor_info = inputTypeInfo.GetTensorTypeAndShapeInfo();
				auto tensor_input_name = m_session.GetInputNameAllocated(i, m_allocator);
				if (tensor_input_name.get())
				{
					LOG(INFO) << i << ": " << tensor_input_name.get() << ", "
							  << tensor_info.GetElementCount() << ", "   // the number of elements specified by the tensor shape (all dimensions multiplied by each other)
							  << tensor_info.GetElementType() << ", "
							  << tensor_info.GetDimensionsCount() << "\n" ;
				}
            }


			/* Log information about output tensors */
            size_t numOutputNodes = m_session.GetOutputCount();
		    for (size_t i = 0; i < numOutputNodes; i++)
		    {
				Ort::TypeInfo outputTypeInfo = m_session.GetOutputTypeInfo(i);
				auto tensor_info = outputTypeInfo.GetTensorTypeAndShapeInfo();
				auto tensor_output_name = m_session.GetOutputNameAllocated(i, m_allocator);
				if (tensor_output_name.get())
				{
					LOG(INFO) << i << ": " << tensor_output_name.get() << ", "
				              << tensor_info.GetElementCount()<< ", "
				              << tensor_info.GetElementType()<< ", "
							  << tensor_info.GetDimensionsCount() << "\n" ;
                }
		    }
		}

		bool IsModelQuantized()
		{
			return !m_inputFloating;
		}

		int GetInputWidth()
		{
			std::vector<int64_t> input_shape = m_session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			return input_shape[2];
		}

		int GetInputHeight()
		{
			std::vector<int64_t> input_shape = m_session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			return input_shape[1];
		}

		int GetInputChannels()
		{
			std::vector<int64_t> input_shape = m_session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			return input_shape[3];
		}

		unsigned int GetNumberOfInputs()
		{
			return m_session.GetInputCount();
		}

		unsigned int GetNumberOfOutputs()
		{
			return m_session.GetOutputCount();
		}

		unsigned int GetOutputSize(int index)
		{
			Ort::TypeInfo type_info = m_session.GetOutputTypeInfo(index);
			// assume output dims to be something like (1, 1, ... ,size)
			return type_info.GetTensorTypeAndShapeInfo().GetShape()[type_info.GetTensorTypeAndShapeInfo().GetShape().size() - 1];
         }


		void RunInference(uint8_t* img, Frame_Results* results)
		{
			if (m_inputFloating)
				RunInference<float>(img, results);
			else
				RunInference<uint8_t>(img, results);
		}

		template <typename T>
				T Vector_Product(const std::vector<T>& vect)
				{
				 return accumulate(vect.begin(), vect.end(), 1, std::multiplies<T>());
				}


		template <class T>
		void RunInference(uint8_t* img, Frame_Results* results)
		{
			int input_height = GetInputHeight();
			int input_width = GetInputWidth();
			int input_channels = GetInputChannels();
			auto sizeInBytes = input_height * input_width * input_channels;
			auto input_name = m_session.GetInputNameAllocated(0, m_allocator);
			if (m_verbose) {
				LOG(INFO) << "input: " << input_name.get() << "\n";
				LOG(INFO) << "number of inputs: " << GetNumberOfInputs() << "\n";
				LOG(INFO) << "number of outputs: " << GetNumberOfOutputs() << "\n";
			}

			/* Prepare input output tensors */
			std::vector<Ort::Value> inputTensors;
			std::vector<Ort::Value> outputTensors;


			std::vector< int64_t > inputDims=m_session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			size_t inputTensorSize =Vector_Product(inputDims); //sizeInBytes
			std::vector<int64_t> outputDims = m_session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
			size_t outputTensorSize = Vector_Product(outputDims);


			Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

			/* Prepare empty output tensor */
			std::vector<float> outputTensorValues(outputTensorSize);
			outputTensors.push_back(Ort::Value::CreateTensor(memoryInfo, outputTensorValues.data(), outputTensorSize, outputDims.data(), outputDims.size()));

			/* Get input */
			float*  in  ;
			if (m_inputFloating) {
				for (int i = 0; i < sizeInBytes; i++){
					in[i] =(img[i] - m_inputMean) / m_inputStd;
					inputTensors.push_back(Ort::Value::CreateTensor(memoryInfo, &(in[i]) , inputTensorSize, inputDims.data(),inputDims.size()));}
			} else {
				for (int i = 0; i < sizeInBytes; i++)
				   inputTensors.push_back(Ort::Value::CreateTensor(memoryInfo, &(img[i]), inputTensorSize, inputDims.data(),inputDims.size()));
			}

		    /* Get input names */
			size_t num_input_nodes = GetNumberOfInputs();
			input_name = m_session.GetInputNameAllocated(0, m_allocator);
			const char* inputNames[] = {input_name.get()};

			/* Get Output names */
			size_t num_output_nodes = m_session.GetOutputCount();
			std::vector<std::string> output_names(num_output_nodes);
			for (size_t i = 0; i != num_output_nodes; ++i) {
				auto output_name = m_session.GetOutputNameAllocated(i, m_allocator);
				assert(output_name != nullptr);
				output_names[i] = output_name.get();
			}
			std::vector<const char*> outputNames(num_output_nodes);
			{
				for (size_t i = 0; i != num_output_nodes; ++i) {
					outputNames[i] = output_names[i].c_str();
				}
			}

			/* Run Inference */
			Ort::RunOptions run_options;
			if (m_verbose) {
				run_options.SetRunLogSeverityLevel(0);
			}

			struct timeval start_time, stop_time;
			gettimeofday(&start_time, nullptr);

			std::cout << "Running inference ..." << std::endl;
			outputTensors = m_session.Run(run_options, inputNames, inputTensors.data(), num_input_nodes, outputNames.data(), num_output_nodes);

			gettimeofday(&stop_time, nullptr);
			m_inferenceTime = (get_ms(stop_time) - get_ms(start_time));

			/* Get results */
			float *locations = outputTensors[0].GetTensorMutableData<float>();
			float *classes = outputTensors[1].GetTensorMutableData<float>();
			float *scores = outputTensors[2].GetTensorMutableData<float>();

			// get the output size by getting the size of the
			// output tensor 1 that represents the classes
			auto output_size = GetOutputSize(1);

			// creation of an ObjDetect_Results struct to store values
			// of detected object of the frame
			ObjDetect_Results Obj_detected;

			// the outputs are already sorted by descending order
			if (first_call){
				for (unsigned int i = 0; i < output_size; i++) {
					Obj_detected.classe =(int)classes[i];
					Obj_detected.score = scores[i];
					Obj_detected.location.y0 = locations[(i * 4) + 0];
					Obj_detected.location.x0 = locations[(i * 4) + 1];
					Obj_detected.location.y1 = locations[(i * 4) + 2];
					Obj_detected.location.x1 = locations[(i * 4) + 3];
					results->vect_ObjDetect_Results.push_back(Obj_detected);
				}
				results->inference_time = m_inferenceTime;
			} else {
				for (unsigned int i = 0; i < output_size; i++) {
					results->vect_ObjDetect_Results.erase(results->vect_ObjDetect_Results.begin()+i);
					Obj_detected.classe =(int)classes[i];
					Obj_detected.score = scores[i];
					Obj_detected.location.y0 = locations[(i * 4) + 0];
					Obj_detected.location.x0 = locations[(i * 4) + 1];
					Obj_detected.location.y1 = locations[(i * 4) + 2];
					Obj_detected.location.x1 = locations[(i * 4) + 3];
					results->vect_ObjDetect_Results.insert(results->vect_ObjDetect_Results.begin()+i,Obj_detected);
				}
				results->inference_time = m_inferenceTime;
			}
			first_call = false;
		}

		// Takes a file name, and loads a list of labels from it, one per line, and
		// returns a vector of the strings. It pads with empty strings so the length
		// of the result is a multiple of 16, because our model expects that.
		bool  ReadLabelsFile(const std::string& file_name,
					    std::vector<std::string>* result,
					    size_t* found_label_count)
		{
			std::ifstream file(file_name);
			if (!file) {
				LOG(FATAL) << "Labels file " << file_name << " not found\n";
				return 0;
			}
			result->clear();
			std::string line;
			while (std::getline(file, line)) {
				result->push_back(line);
			}
			*found_label_count = result->size();
			const int padding = 16;
			while (result->size() % padding) {
				result->emplace_back();
			}
			return 1;
		}
	};

}  // namespace wrapper_onnx

#endif  // WRAPPER_ONNX_HPP_
