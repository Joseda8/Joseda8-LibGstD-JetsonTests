# 
# Copyright (C) 2021 BestSeat 2021
# Author: Manuel Leiva <manuel.leiva@ridgerun.com>.
#
# Library source files

test_src_1 = [
  'test1.c',
]

test_src_2_fpsdisplay = [
  'test2_fpsdisplay.c',
]

test_src_2_framerate = [
  'test2_framerate.c',
]

test_src_3 = [
  'test3_clientapp.c',
]

test_src_3_2 = [
  'test3_clientapp2.c',
]

test_src_4_appsink_callback = [
  'test4_performance_appsink_callback.c',
]

test_src_4_appsink = [
  'test4_performance_appsink.c',
]


# Create applications
exe_name = '@0@-@1@'.format('test1', project_version)
executable(exe_name,
  test_src_1,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_audio],
  c_args: c_args,
)

exe_name = '@0@-@1@'.format('test2_fpsdisplay', project_version)
executable(exe_name,
  test_src_2_fpsdisplay,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_audio],
  c_args: c_args,
)

exe_name = '@0@-@1@'.format('test2_framerate', project_version)
executable(exe_name,
  test_src_2_framerate,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_audio, thread_dep],
  c_args: c_args,
)


exe_name = '@0@-@1@'.format('test3_clientapp', project_version)
executable(exe_name,
  test_src_3,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_audio],
  c_args: c_args,
)

exe_name = '@0@-@1@'.format('test3_clientapp2', project_version)
executable(exe_name,
  test_src_3_2,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_audio, thread_dep],
  c_args: c_args,
)

exe_name = '@0@-@1@'.format('test4_appsink_callback', project_version)
executable(exe_name,
  test_src_4_appsink_callback,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_audio, thread_dep],
  c_args: c_args,
)

exe_name = '@0@-@1@'.format('test4_appsink', project_version)
executable(exe_name,
  test_src_4_appsink,
  install: true,
  include_directories : [configinc],
  dependencies : [gstd_dep, gio_unix_dep, json_glib_dep, gst_audio, thread_dep],
  c_args: c_args,
)
