#
# Copyright (C) 2021 BestSeat 2021
# Author: Manuel Leiva <manuel.leiva@ridgerun.com>.
#

project('test_project',
        ['c'],
	version : '0.1.0',
	meson_version : '>= 0.50',)

project_name = meson.project_name()
project_version = meson.project_version()
version_arr = project_version.split('.')
version_major = version_arr[0].to_int()
version_minor = version_arr[1].to_int()
version_micro = version_arr[2].to_int()

if version_arr.length() == 4
  version_nano = version_arr[3].to_int()
else
  version_nano = 0
endif

version_is_dev = version_minor % 2 == 1 and version_micro < 90

# Get an object returns describing a compiler
cc = meson.get_compiler('c')

# Dependencies
gstd_dep = dependency('gstd-1.0')
gio_unix_dep  = dependency('gio-unix-2.0', version : '>=2.44.1')
json_glib_dep = dependency('json-glib-1.0', version : '>=0.16.2')
gst_audio = dependency('gstreamer-audio-1.0')
thread_dep = dependency('threads')

# Define header directories
configinc = include_directories('.')


# Define compiler args and include directories
c_args = ['-DHAVE_CONFIG_H']

# Define installation directories
library_install_dir = get_option('libdir')
pkgconfig_install_dir = join_paths(get_option('libdir'), 'pkgconfig')

# Verify if the warning flags are available in the compiler
# If the flags is availale for the compiler it wiil be used in all compiler
# invocations with the exception of compile tests.
warning_flags = []

foreach extra_arg : warning_flags
  if cc.has_argument (extra_arg)
    # Add flag to the compiler command line
    add_project_arguments([extra_arg], language: 'c')
  endif
endforeach

# Create an empty configuration object to set config.h information
cdata = configuration_data()
cdata.set_quoted('BESTSEAT_VERSION', project_version)
configure_file(output : 'config.h', configuration : cdata)
subdir('app')
