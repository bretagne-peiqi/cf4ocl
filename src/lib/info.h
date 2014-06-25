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
 * @brief Functions for obtaining information about OpenCL entities
 * such as platforms, devices, contexts, queues, kernels, etc.
 * 
 * @author Nuno Fachada
 * @date 2014
 * @copyright [GNU Lesser General Public License version 3 (LGPLv3)](http://www.gnu.org/licenses/lgpl.html)
 * */
 
#ifndef CL4_INFO_H
#define CL4_INFO_H 

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>

/**
 * @brief Information about a wrapped OpenCL entity.
 * */
typedef struct cl4_info {
	/** Device information. */
	gpointer value;
	/** Size in bytes of device information. */
	gsize size;
} CL4Info;

/** @brief Create a new CL4Info* object. */
CL4Info* cl4_info_new(gpointer value, gsize size);

/** @brief Destroy a CL4Info* object. */
void cl4_info_destroy(void* info_value);

/**
 * @brief Get information about any wrapped OpenCL object.
 * 
 * @param wrapper The wrapper object.
 * @param param_name Name of information/parameter to get.
 * @param clInfoFunction Name of OpenCL clGet*Info function.
 * @param err Return location for a GError, or NULL if error reporting
 * is to be ignored.
 * @return The requested information object. This object will
 * be automatically freed when the respective wrapper object is 
 * destroyed. If an error occurs, NULL is returned.
 * */
#define cl4_info_get(wrapper, param_name, clInfoFunction, err) \
	\
	/* Make sure err is NULL or it is not set. */ \
	g_return_val_if_fail((err) == NULL || *(err) == NULL, NULL); \
	\
	/* Make sure wrapper is not NULL. */ \
	g_return_val_if_fail((wrapper) != NULL, NULL); \
	\
	/* Information object. */ \
	CL4Info* info = NULL; \
	\
	/* If information table is not yet initialized, then \
	 * initialize it. */ \
	if (!(wrapper)->info) { \
		(wrapper)->info = g_hash_table_new_full( \
			g_direct_hash, g_direct_equal, \
			NULL, cl4_info_destroy); \
	} \
	\
	/* Check if requested information is already present in the \
	 * information table. */ \
	if (g_hash_table_contains( \
		(wrapper)->info, GUINT_TO_POINTER(param_name))) { \
		\
		/* If so, retrieve it from there. */ \
		info = g_hash_table_lookup( \
			(wrapper)->info, GUINT_TO_POINTER(param_name)); \
		\
	} else { \
		\
		/* Otherwise, get it from OpenCL device.*/ \
		cl_int ocl_status; \
		/* Device information placeholder. */ \
		gpointer param_value; \
		/* Size of device information in bytes. */ \
		gsize size_ret; \
		\
		/* Get size of information. */ \
		ocl_status = (clInfoFunction)( \
			(wrapper)->cl_object, param_name, 0, NULL, &size_ret); \
		gef_if_error_create_goto(*err, CL4_ERROR, \
			CL_SUCCESS != ocl_status, CL4_ERROR_OCL, error_handler, \
			"Function '%s': get info [size] (OpenCL error %d: %s).", \
			__func__, ocl_status, cl4_err(ocl_status)); \
		gef_if_error_create_goto(*err, CL4_ERROR, \
			size_ret == 0, CL4_ERROR_OCL, error_handler, \
			"Function '%s': get info [size] (size is 0).", \
			__func__); \
		\
		/* Allocate memory for information. */ \
		param_value = g_malloc(size_ret); \
		\
		/* Get information. */ \
		ocl_status = (clInfoFunction)( \
			(wrapper)->cl_object, param_name, size_ret, param_value, NULL); \
		gef_if_error_create_goto(*err, CL4_ERROR, \
			CL_SUCCESS != ocl_status, CL4_ERROR_OCL, error_handler, \
			"Function '%s': get context info [info] (OpenCL error %d: %s).", \
			__func__, ocl_status, cl4_err(ocl_status)); \
		\
		/* Keep information in information table. */ \
		info = cl4_info_new(param_value, size_ret); \
		g_hash_table_insert((wrapper)->info, \
			GUINT_TO_POINTER(param_name), \
			info); \
		\
	} \
	\
	/* If we got here, everything is OK. */ \
	g_assert(err == NULL || *err == NULL); \
	goto finish; \
	\
error_handler: \
	/* If we got here there was an error, verify that it is so. */ \
	g_assert(err == NULL || *err != NULL); \
	info = NULL; \
	\
finish: \
	\
	/* Return the requested information. */ \
	return info;

#endif
