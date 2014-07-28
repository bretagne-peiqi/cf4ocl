/*   
 * This file is part of cf4ocl (C Framework for OpenCL).
 * 
 * cf4ocl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version.
 * 
 * cf4ocl is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with cf4ocl. If not, see 
 * <http://www.gnu.org/licenses/>.
 * */
 
 /** 
 * @file
 * @brief OpenCL program wrapper.
 * 
 * @author Nuno Fachada
 * @date 2014
 * @copyright [GNU Lesser General Public License version 3 (LGPLv3)](http://www.gnu.org/licenses/lgpl.html)
 * */
 
#ifndef CL4_PROGRAM_WRAPPER_H
#define CL4_PROGRAM_WRAPPER_H 

#include <glib.h>
#include "oclversions.h"
#include "context_wrapper.h"
#include "abstract_wrapper.h"
#include "event_wrapper.h"
#include "queue_wrapper.h"
#include "kernel_wrapper.h"

/**
 * @defgroup PROGRAM The program wrapper module.
 *
 * @brief A wrapper object for OpenCL programs and functions to manage 
 * it.
 * 
 * Todo: detailed description of module.
 * 
 * @{
 */
 
/** @brief Program wrapper object. */
typedef struct cl4_program CL4Program;

/** @brief Represents a OpenCL binary object. */
typedef struct cl4_program_binary CL4ProgramBinary;

typedef void (CL_CALLBACK* cl4_program_callback)(
	cl_program program, void* user_data);

/* WRAPPER API */
/** @brief Get the program wrapper for the given OpenCL program. */
CL4Program* cl4_program_new_wrap(cl_program program);

/** @brief Decrements the reference count of the program wrapper object. 
 * If it reaches 0, the program wrapper object is destroyed. */
void cl4_program_destroy(CL4Program* prg);

/* SOURCES */
CL4Program* cl4_program_new_from_source_file(CL4Context* ctx, 
	const char* filename, GError** err);

CL4Program* cl4_program_new_from_source_files(CL4Context* ctx, 
	cl_uint count, const char** filenames, GError** err);

#define cl4_program_new_from_source(ctx, src, err) \
	cl4_program_new_with_source( \
		cl4_context_unwrap(ctx), 1, &src, NULL, err)

#define cl4_program_new_from_sources(ctx, count, strings, err) \
	cl4_program_new_with_source( \
		cl4_context_unwrap(ctx), count, strings, NULL, err)
	
CL4Program* cl4_program_new_with_source(cl_context context,
	cl_uint count, const char **strings, const size_t *lengths,
	GError** err);

/* BINARIES */
CL4Program* cl4_program_new_from_binary_file(CL4Context* ctx, 
	CL4Device* dev, const char* filename, GError** err);

CL4Program* cl4_program_new_from_binary_files(CL4Context* ctx, 
	cl_uint num_devices, CL4Device** devs, const char** filenames, 
	GError** err);

#define cl4_program_new_from_binary(ctx, dev, binary, err) \
	cl4_program_new_from_binaries(ctx, 1, &dev, &binary, err)

CL4Program* cl4_program_new_from_binaries(CL4Context* ctx,
	cl_uint num_devices, CL4Device** devs, CL4ProgramBinary** bins,
	GError** err);

CL4Program* cl4_program_new_with_binary(cl_context context,
	cl_uint num_devices, const cl_device_id* device_list,
	const size_t *lengths, const unsigned char **binaries,
	cl_int *binary_status, GError** err);

#ifdef CL_VERSION_1_2

CL4Program* cl4_program_new_with_built_in_kernels(cl_context context,
	cl_uint num_devices, const cl_device_id *device_list,
	const char *kernel_names, GError** err);

#endif
 
#define cl4_program_build(prg, options, err) \
	cl4_program_build_from_devices_full( \
		prg, 0, NULL, options, NULL, NULL, err)

cl_bool cl4_program_build_from_devices_full(CL4Program* prg, 
	cl_uint num_devices, CL4Device** devices, const char *options, 
	cl4_program_callback pfn_notify, void *user_data, GError** err);

cl_bool cl4_program_build_from_cldevices_full(CL4Program* prg, 
	cl_uint num_devices, cl_device_id* device_list, const char *options, 
	cl4_program_callback pfn_notify, void *user_data, GError** err);

CL4Kernel* cl4_program_get_kernel(
	CL4Program* prg, const char* kernel_name, GError** err);
	
CL4Event* cl4_program_run(CL4Program* prg, const char* kernel_name,
	CL4CQueue* cq, cl_uint work_dim, const size_t* global_work_offset, 
	const size_t* global_work_size, const size_t* local_work_size, 
	CL4EventWaitList evt_wait_lst, GError** err, ...)
	G_GNUC_NULL_TERMINATED;

CL4Event* cl4_program_run_v(CL4Program* prg, const char* kernel_name,
	CL4CQueue* cq, cl_uint work_dim, const size_t* global_work_offset, 
	const size_t* global_work_size, const size_t* local_work_size, 
	CL4EventWaitList evt_wait_lst, GError** err, va_list args);
	
CL4ProgramBinary* cl4_program_get_binary(CL4Program* prg, CL4Device* dev,
	GError** err);

cl_bool cl4_program_save_binary(CL4Program* prg, CL4Device* dev,
	const char* filename, GError** err);

cl_bool cl4_program_save_all_binaries(CL4Program* prg, 
	const char* file_prefix, const char* file_suffix, GError** err);

/**
 * @brief Get program information object. To get the program binaries
 * use the cl4_program_get_binary() function instead, as this macro will
 * return NULL when the CL_PROGRAM_BINARIES parameter is requested.
 * 
 * @param prg The program wrapper object.
 * @param param_name Name of information/parameter to get.
 * @param err Return location for a GError, or NULL if error reporting
 * is to be ignored.
 * @return The requested information object. This object will be 
 * automatically freed when the wrapper object is destroyed. If an error 
 * occurs or if the CL_PROGRAM_BINARIES parameter is requested, NULL is 
 * returned.
 * */
#define cl4_program_info(prg, param_name, err) \
	(param_name == CL_PROGRAM_BINARIES) \
	? NULL \
	: cl4_wrapper_get_info((CL4Wrapper*) prg, NULL, param_name, \
		(cl4_wrapper_info_fp) clGetProgramInfo, CL_TRUE, err)

/**
 * @brief Get program build information object.
 * 
 * @param prg The program wrapper object.
 * @param dev The device wrapper object to which to build refers to.
 * @param param_name Name of information/parameter to get.
 * @param err Return location for a GError, or NULL if error reporting
 * is to be ignored.
 * @return The requested information object. This object will be 
 * automatically freed when the wrapper object is destroyed. If an error 
 * occurs, NULL is returned.
 * */
#define cl4_program_build_info(prg, dev, param_name, err) \
	cl4_wrapper_get_info((CL4Wrapper*) prg, (CL4Wrapper*) dev, \
		param_name, (cl4_wrapper_info_fp) clGetProgramBuildInfo, \
		CL_FALSE, err)

/** 
 * @brief Increase the reference count of the program object.
 * 
 * @param prg The program wrapper object. 
 * */
#define cl4_program_ref(prg) \
	cl4_wrapper_ref((CL4Wrapper*) prg)

/**
 * @brief Alias to cl4_program_destroy().
 * 
 * @param prg Program wrapper object to destroy if reference count 
 * is 1, otherwise just decrement the reference count.
 * */
#define cl4_program_unref(prg) cl4_program_destroy(prg)

/**
 * @brief Get the OpenCL program object.
 * 
 * @param prg The program wrapper object.
 * @return The OpenCL program object.
 * */
#define cl4_program_unwrap(prg) \
	((cl_program) cl4_wrapper_unwrap((CL4Wrapper*) prg))		

/** 
 * @brief Get ::CL4Device wrapper at given index. 
 * 
 * @param prg The program wrapper object.
 * @param index Index of device in program.
 * @param err Return location for a GError, or NULL if error reporting 
 * is to be ignored.
 * @return The ::CL4Device wrapper at given index or NULL if an error 
 * occurs.
 * */
#define cl4_program_get_device(prg, index, err) \
	cl4_dev_container_get_device((CL4DevContainer*) prg, \
	cl4_program_get_cldevices, index, err)

/**
 * @brief Return number of devices in program.
 * 
 * @param prg The program wrapper object.
 * @param err Return location for a GError, or NULL if error reporting 
 * is to be ignored.
 * @return The number of devices in program or 0 if an error occurs or 
 * is otherwise not possible to get any device.
 * */
#define cl4_program_get_num_devices(prg, err) \
	cl4_dev_container_get_num_devices((CL4DevContainer*) prg, \
	cl4_program_get_cldevices, err)

/** @} */

/** @brief Create a new ::CL4ProgramBinary object with a given value 
 * size. */
CL4ProgramBinary* cl4_program_binary_new(
	unsigned char* data, size_t size);

#define cl4_program_binary_new_empty() \
	cl4_program_binary_new(NULL, 0);

/** @brief Destroy a ::CL4ProgramBinary object. */
void cl4_program_binary_destroy(CL4ProgramBinary* bin);

/** @brief Implementation of cl4_dev_container_get_cldevices() for the
 * program wrapper. */
CL4WrapperInfo* cl4_program_get_cldevices(
	CL4DevContainer* devcon, GError** err);

#endif

