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
 * Definition of a wrapper class and its methods for OpenCL memory
 * objects.
 *
 * @author Nuno Fachada
 * @date 2014
 * @copyright [GNU Lesser General Public License version 3 (LGPLv3)](http://www.gnu.org/licenses/lgpl.html)
 * */

#include "ccl_context_wrapper.h"
#include "priv_abstract_wrapper.h"

#ifndef _CCL_PRIV_MEMOBJ_WRAPPER_H_
#define _CCL_PRIV_MEMOBJ_WRAPPER_H_

/**
 * Base class for memory object wrappers, i.e., ::CCLBuffer and
 * ::CCLImage.
 *
 * @ingroup CCL_MEMOBJ_WRAPPER
 * @extends ccl_wrapper
 * */
struct ccl_memobj {

	/**
	 * Parent wrapper object.
	 * @private
	 * */
	CCLWrapper base;

	/**
	 * Context wrapper.
	 * @private
	 * */
	CCLContext* ctx;

};

#endif
