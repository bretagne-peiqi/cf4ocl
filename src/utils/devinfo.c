/*   
 * This file is part of cf4ocl (C Framework for OpenCL).
 * 
 * cf4ocl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cf4ocl is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with cf4ocl.  If not, see <http://www.gnu.org/licenses/>.
 * */

/** 
 * @file
 * @brief Utility to query OpenCL platforms and devices.
 * 
 * @author Nuno Fachada
 * @date 2014
 * @copyright [GNU General Public License version 3 (GPLv3)](http://www.gnu.org/licenses/gpl.html)
 */

#include "devinfo.h"

/* Command line arguments and respective default values. */
static gboolean opt_all = FALSE;
static gboolean opt_basic = TRUE; /* Default. */
static gchar** opt_custom = NULL;
static guint opt_platf = G_MAXUINT;
static guint opt_dev = G_MAXUINT;
static gboolean opt_nfound = FALSE;
static gboolean opt_verb = FALSE;
static gboolean opt_list = FALSE;

/* Valid command line options. */
static GOptionEntry entries[] = {
	{"all",      'a', 0, G_OPTION_ARG_NONE,               &opt_all,
	 "Show all the available device information",         NULL},
	{"basic",    'b', 0, G_OPTION_ARG_NONE,               &opt_basic,
	 "Show basic device information (default)",           NULL},
	{"custom",   'c', 0, G_OPTION_ARG_STRING_ARRAY,       &opt_custom,
	 "Show specific information, repeat as necessary",    "cl_device_info"},
	{"device",   'd', 0, G_OPTION_ARG_INT,                &opt_dev,
	 "Specify a device to query",                         "device"},
	{"list",     'l', 0, G_OPTION_ARG_NONE,               &opt_list,
	 "List known information parameters",                 NULL},
	{"platform", 'p', 0, G_OPTION_ARG_INT,                &opt_platf,
	 "Specify a platform to query",                       "platform"},
	{"notfound", 'n', 0, G_OPTION_ARG_NONE,               &opt_nfound,
	 "Show known parameters even if not found in device", NULL},
	{"verbose",  'v', 0, G_OPTION_ARG_NONE,               &opt_verb,
	 "Show description of each parameter",                NULL},
	{ NULL, 0, 0, 0, NULL, NULL, NULL }
};

/* Information queried for basic CLI option. */
static gchar* basic_info[] = {
	"type",
	"vendor",
	"opencl_c_version",
	"max_compute_units",
	"global_mem_size",
	"max_mem_alloc_size",
	"local_mem_size",
	"local_mem_type",
	"max_work_group_size",
	NULL
};

/**
 * @brief Device query main program function.
 * 
 * @param argc Number of command line arguments.
 * @param argv Vector of command line arguments.
 * @return #CL4_SUCCESS if program returns with no error, or an error
 * code otherwise.
 */
int main(int argc, char* argv[]) {
	
	/* Error object. */
	GError* err = NULL;
	
	/* List of platform wrapper objects. */
	CL4Platforms* platforms = NULL;
	
	/* Current platform and device. */
	CL4Platform* p;
	CL4Device* d;
	
	/* Number of devices in platform. */
	guint num_devs;
	
	/* Device information value object. */
	CL4WrapperInfo* info_value = NULL;
	
	/* Device name. */
	gchar* dev_name;
	
	/* Function status. */
	gint status;
	
	/* Parse command line options. */
	cl4_device_query_args_parse(argc, argv, &err);
	gef_if_error_goto(err, CL4_ERROR_ARGS, status, error_handler);
	
	/* Check if user requested a list of known information parameters. */
	if (opt_list) {
		
		/*Yes, user requested list, present it. */
		
		g_fprintf(CL4_DEVINFO_OUT, "\nKnown information parameters:\n\n");
		for (gint i = 0; i < cl4_devquery_info_map_size; i++) {
			if (opt_verb) {
				g_fprintf(CL4_DEVINFO_OUT,	
					"\t%s\n\t\t%s.\n\n", 
					cl4_devquery_info_map[i].param_name,
					cl4_devquery_info_map[i].description);
			} else {
				g_fprintf(CL4_DEVINFO_OUT,	
					"\t%s\n", 
					cl4_devquery_info_map[i].param_name);
			}
		}
		g_fprintf(CL4_DEVINFO_OUT, "\n");

	} else {
	
		/* User didn't request list, proceed as normal query. */
		
		/* Get list of platform wrapper objects. */
		platforms = cl4_platforms_new(&err);
		gef_if_error_goto(err, GEF_USE_GERROR, status, error_handler);
		
		/* Cycle through platforms. */
		for (guint i = 0; i < cl4_platforms_count(platforms); i++) {
			
			/* Get out if this platform is not to be queried. */
			if ((opt_platf != G_MAXUINT) && (i != opt_platf))
				continue;
			
			/* Get current platform. */
			p = cl4_platforms_get_platform(platforms, i);
			
			/* Show platform information. */
			cl4_device_query_show_platform_info(p, i);
			
			/* Get number of devices. */
			num_devs = cl4_platform_get_num_devices(p, &err);
			gef_if_error_goto(err, GEF_USE_GERROR, status, error_handler);
		
			/* Cycle through devices. */
			for (guint j = 0; j < num_devs; j++) {
				
				/* Get out if this device is not to be queried. */
				if ((opt_dev != G_MAXUINT) && (j != opt_dev))
					continue;

				/* Get current device. */
				d = cl4_platform_get_device(p, j, &err);
				gef_if_error_goto(
					err, GEF_USE_GERROR, status, error_handler);
					
				/* Get device name. */
				info_value = cl4_device_info(d, CL_DEVICE_NAME, &err);
				gef_if_error_goto(
					err, GEF_USE_GERROR, status, error_handler);
				dev_name = (gchar*) info_value->value;
				
				/* Show device information. */
				g_fprintf(CL4_DEVINFO_OUT, 
					"\n\t[ Device #%d: %s ]\n\n", 
					j, dev_name);
				if (opt_all)
					cl4_device_query_show_device_info_all(d);
				else if (opt_custom)
					cl4_device_query_show_device_info_custom(d);
				else
					cl4_device_query_show_device_info_basic(d);
				
			}
			g_fprintf(CL4_DEVINFO_OUT,	"\n");
		}
	}
	/* If we got here, everything is OK. */	
	g_assert(err == NULL);
	status = CL4_SUCCESS;
	goto cleanup;
	
error_handler:

	/* If we got here there was an error, verify that it is so. */
	g_assert(err != NULL);
	g_fprintf(stderr, "%s\n", err->message);
	g_error_free(err);

cleanup:
		
	/* Free stuff! */
	if (platforms) cl4_platforms_destroy(platforms);
	g_strfreev(opt_custom);

	/* Return status. */
	return status;

}

/**
 * @brief Parse and verify command line arguments.
 * 
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 * @param err GLib error object for error reporting.
 * */
void cl4_device_query_args_parse(int argc, char* argv[], GError** err) {
	
	/* Make sure err is NULL or it is not set. */
	g_return_if_fail(err == NULL || *err == NULL);

	/* Command line options context. */
	GOptionContext* context = NULL;

	/* Create parsing context. */
	context = g_option_context_new(" - " CL4_DEVINFO_DESCRIPTION);
	gef_if_error_create_goto(*err, CL4_ERROR, context == NULL, 
		CL4_ERROR_ARGS, error_handler, 
		"Unable to create command line parsing context.");
	
	/* Add acceptable command line options to context. */ 
	g_option_context_add_main_entries(context, entries, NULL);
	
	/* Use context to parse command line options. */
	g_option_context_parse(context, &argc, &argv, err);
	gef_if_err_goto(*err, error_handler);
	
	/* If we get here, no need for error treatment, jump to cleanup. */
	g_assert(*err == NULL);
	goto cleanup;
	
error_handler:

	/* If we got here, everything is OK. */
	g_assert(*err != NULL);

cleanup:	

	/* Free context. */
	if (context) g_option_context_free(context);
	
	/* Return. */
	return;

}

/**
 * @brief Show platform information.
 * 
 * @param p Platform wrapper object.
 * @param idx Platform index.
 * */
void cl4_device_query_show_platform_info(CL4Platform* p, guint idx) {

	/* Platform info variables. */
	gchar *profile, *version, *name, *vendor;

	/* Error location. */
	GError* err = NULL;
	
	/* Get platform profile. */
	profile = cl4_platform_get_info_string(p, CL_PLATFORM_PROFILE, &err);
	if (err != NULL) {
		g_clear_error(&err);
		profile = "Unknown profile";
	}

	/* Get platform version. */
	version = cl4_platform_get_info_string(p, CL_PLATFORM_VERSION, &err);
	if (err != NULL) {
		g_clear_error(&err);
		profile = "Unknown version";
	}
			
	/* Get platform name. */
	name = cl4_platform_get_info_string(p, CL_PLATFORM_NAME, &err);
	if (err != NULL) {
		g_clear_error(&err);
		profile = "Unknown name";
	}

	/* Get platform vendor. */
	vendor = cl4_platform_get_info_string(p, CL_PLATFORM_VENDOR, &err);
	if (err != NULL) {
		g_clear_error(&err);
		profile = "Unknown vendor";
	}

	/*  Send info to defined stream. */
	g_fprintf(CL4_DEVINFO_OUT, "\n* Platform #%d: %s (%s)\n               %s, %s\n",
		idx, name, vendor, version, profile);
	
	/* Bye. */
	return;
}

#define cl4_device_query_output_device_info(key, value, desc) \
	if (opt_verb) { \
		g_fprintf(CL4_DEVINFO_OUT, \
			"\t\t   Parameter : %s\n" \
			"\t\t Description : %s\n" \
			"\t\t       Value : %s\n\n", \
			key, desc, value); \
	} else { \
		g_fprintf(CL4_DEVINFO_OUT, \
			"\t\t %-40.40s | %s\n", \
			key, value); \
	}
	

/**
 * @brief Show all available device information.
 * 
 * @param d Device wrapper object.
 * */
void cl4_device_query_show_device_info_all(CL4Device* d) {

	/* Parameter value and size. */
	CL4WrapperInfo* param_value;
	
	/* Parameter value string. */
	gchar param_value_str[CL4_DEVINFO_MAXINFOLEN];
	
	/* Error reporting object. */
	GError* err = NULL;

	/* Cycle through all supported device information names. */
	for (gint k = 0; k < cl4_devquery_info_map_size; k++) {
		
		/* Get the device information value and size. */
		param_value = cl4_device_info(
			d, cl4_devquery_info_map[k].device_info, &err);
		
		/* Check for errors. */
		if (err == NULL) {
			
			/* If no error, show current parameter value... */
			cl4_device_query_output_device_info(
				cl4_devquery_info_map[k].param_name, 
				cl4_devquery_info_map[k].format(
					param_value, param_value_str, 
					CL4_DEVINFO_MAXINFOLEN,
					cl4_devquery_info_map[k].units),
				cl4_devquery_info_map[k].description);
				
		} else {
			
			/* ...otherwise clear error... */
			g_clear_error(&err);
			if (opt_nfound) {
				/* ...and show that parameter is not available, if user 
				 * requested so. */
				cl4_device_query_output_device_info(
					cl4_devquery_info_map[k].param_name,
					CL4_DEVINFO_NA,
					cl4_devquery_info_map[k].description);
			}
		}
	}

}

/**
 * @brief Show user specified device information.
 * 
 * @param d Device wrapper object.
 * */
void cl4_device_query_show_device_info_custom(CL4Device* d) {

	/* A row of the device info_map. */
	const CL4DevQueryMap* info_row;
	
	/* Parameter value and size. */
	CL4WrapperInfo* param_value;

	/* Parameter value string. */
	gchar param_value_str[CL4_DEVINFO_MAXINFOLEN];

	/* Error reporting object. */
	GError* err = NULL;
	
	/* Custom parameter name in proper format. */
	gchar* custom_param_name;
	
	/* Index of next row of the device info_map. */
	gint idx;
	
	/* Cycle through all user specified parameter substrings. */
	for (guint i = 0; opt_custom[i] != NULL; i++) {

		/* Set index of next row to zero. */
		idx = 0;
		
		/* Put info name in proper format. */
		custom_param_name = cl4_devquery_get_prefix_final(opt_custom[i]);
		
		/* Get next row (the first one). */
		info_row = cl4_devquery_match(custom_param_name, &idx);
		
		/* Keep getting rows until we reach the end of the device 
		 * info_map. */
		while (info_row != NULL) {

			/* Get parameter value for current info_map row. */
			param_value = cl4_device_info(d, info_row->device_info, &err);
			
			/* Check for errors. */
			if (err == NULL) {

				/* If no error, show current parameter value... */
				cl4_device_query_output_device_info(
					info_row->param_name,
					info_row->format(
						param_value, param_value_str, 
						CL4_DEVINFO_MAXINFOLEN,
						info_row->units),
					info_row->description);


						
			} else {
				
				/* ...otherwise clear error... */
				g_clear_error(&err);
				if (opt_nfound) {
					/* ...and show that parameter is not available, if user 
					* requested so. */
					cl4_device_query_output_device_info(
						info_row->param_name,
						CL4_DEVINFO_NA,
						info_row->description);
				}
			}
			
			/* Get next row. */
			info_row = cl4_devquery_match(custom_param_name, &idx);
			
		}
		
		/* Free the proper format custom parameter name. */
		g_free(custom_param_name);

	}

}

/**
 * @brief Show basic device information.
 * 
 * @param d Device wrapper object.
 * */
void cl4_device_query_show_device_info_basic(CL4Device* d) {
	
	/* A row of the device info_map. */
	const CL4DevQueryMap* info_row;

	/* Parameter value and size. */
	CL4WrapperInfo* param_value;

	/* Parameter value string. */
	gchar param_value_str[CL4_DEVINFO_MAXINFOLEN];

	/* Error reporting object. */
	GError* err = NULL;

	/* Cycle through the pre-defined basic information array. */
	for (guint i = 0; basic_info[i] != NULL; i++) {

		/* Get next row. */
		info_row = cl4_devquery_prefix(basic_info[i], NULL);

		/* Check that its a valid parameter, otherwise we have a 
		 * programming error, so it is better to abort. */
		g_assert(info_row != NULL);
		
		/* Get parameter value for current info_map row. */
		param_value = cl4_device_info(d, info_row->device_info, &err);
			
		/* Check for errors. */
		if (err == NULL) {

			/* If no error, show current parameter value... */
			cl4_device_query_output_device_info(
				info_row->param_name,
				info_row->format(
					param_value, param_value_str, 
					CL4_DEVINFO_MAXINFOLEN,
					info_row->units),
				info_row->description);

		} else {

			/* ...otherwise clear error... */
			g_clear_error(&err);
			if (opt_nfound) {
				/* ...and show that parameter is not available, if user 
				 * requested so. */
				cl4_device_query_output_device_info(
					info_row->param_name,
					CL4_DEVINFO_NA,
					info_row->description);
			}
		}
		
	}

}