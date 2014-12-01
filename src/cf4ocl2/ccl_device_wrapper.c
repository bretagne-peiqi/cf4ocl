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
 *
 * Implementation of a wrapper class and its methods for OpenCL device
 * objects.
 *
 * @author Nuno Fachada
 * @date 2014
 * @copyright [GNU Lesser General Public License version 3 (LGPLv3)](http://www.gnu.org/licenses/lgpl.html)
 *
 * */

#include "ccl_device_wrapper.h"
#include "ccl_platform_wrapper.h"
#include "priv_abstract_wrapper.h"

/**
 * Device wrapper class.
 *
 * @extends ccl_wrapper
 *
 * @todo Add support for sub-devices.
 */
struct ccl_device {

	/**
	 * Parent wrapper object.
	 * @private
	 * */
	CCLWrapper base;

};

/**
 * @addtogroup CCL_DEVICE_WRAPPER
 * @{
 */

/**
 * Get the device wrapper for the given OpenCL device.
 *
 * If the wrapper doesn't exist, its created with a reference count
 * of 1. Otherwise, the existing wrapper is returned and its reference
 * count is incremented by 1.
 *
 * This function will rarely be called from client code, except when
 * clients wish to get the OpenCL device directly (using the
 * clGetDeviceIDs() function) and then wrap the OpenCL device in a
 * ::CCLDevice wrapper object.
 *
 * @public @memberof ccl_device
 *
 * @param[in] device The OpenCL device to be wrapped.
 * @return The device wrapper for the given OpenCL device.
 * */
CCL_EXPORT
CCLDevice* ccl_device_new_wrap(cl_device_id device) {

	return (CCLDevice*) ccl_wrapper_new(
		(void*) device, sizeof(CCLDevice));

}

/**
 * Decrements the reference count of the device wrapper object.
 * If it reaches 0, the device wrapper object is destroyed.
 *
 * @public @memberof ccl_device
 *
 * @param[in] dev The device wrapper object.
 * */
CCL_EXPORT
void ccl_device_destroy(CCLDevice* dev) {

	ccl_wrapper_unref((CCLWrapper*) dev, sizeof(CCLDevice),
		NULL, NULL, NULL);

}

/** @} */
