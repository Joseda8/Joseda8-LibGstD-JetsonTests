/*
 * Copyright (C) 2021 BestSeat 2021
 * Author: Manuel Leiva <manuel.leiva@ridgerun.com>.
 */
//#include "bestseat_mediaserver_client.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gstd.h>
#include <pthread.h>

GstD *manager = NULL;
gulong handler_id = 0;

typedef struct _SinkData SinkData;
struct _SinkData
{
  char *my_message;
};

static GstdReturnCode sink_callback (GstElement *sink, void *user_data)
{ 
  GstSample *sample = NULL;
  GstBuffer *buffer = NULL;
  GstMapInfo info = {0};
  
  g_signal_emit_by_name (sink, "pull-sample", &sample);
  buffer = gst_sample_get_buffer(sample);
  gst_buffer_map(buffer, &info, GST_MAP_READ);

  if (sample) {

    gst_buffer_unmap (buffer, &info);
    gst_sample_unref (sample);
  }
  
  return GSTD_EOK;
}

void close_tegra () {
  char line[5];
  pid_t pid = 0;
  gchar* kill_process_cmd = NULL;

  FILE *cmd = popen ("pidof tegrastats", "r");

  fgets (line, 5, cmd);

  pid = strtoul (line, NULL, 10);

  pclose (cmd);

  kill_process_cmd = g_strdup_printf ("kill %d", pid);

  system (kill_process_cmd);
}

void *measure_performance (void *args) {
    FILE *fp = NULL;

    fp = popen("tegrastats --interval 500 --logfile test4_performance_appsink_callback.txt", "r");
    if (fp ==NULL) {
      printf ("Failed to run command");
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int ret = GSTD_EOK;
    int time_sleep = 5;

    GstdObject *resource = NULL;
    GstElement *element = NULL;
    
    SinkData *sink_data = NULL;

    pthread_t thread_performance_id;

    pthread_create (&thread_performance_id, NULL, measure_performance, NULL);

    gstd_new (&manager, 0, NULL);

    g_print ("Starting...\n");
    if (!gstd_start (manager)) {
        return FALSE;
    }

    sink_data = malloc (sizeof (SinkData));
    sink_data->my_message = strdup("GstD community");

    ret = gstd_create(manager, "/pipelines", "pipe_sink", "videotestsrc name=vts ! jpegenc ! appsink name=as emit-signals=true async=false drop=true");
    if (GSTD_EOK != ret) {
       printf ("Error: There was a problem creating a GstD: %d\n", ret);
       return -1;
    }

    gstd_read (manager, "pipelines/pipe_sink/elements/as", &resource);
    g_object_get (resource, "gstelement", &element, NULL);
    handler_id = g_signal_connect (element, "new-sample", G_CALLBACK (sink_callback), sink_data);

    gstd_update (manager, "/pipelines/pipe_sink/state", "playing");
    sleep (time_sleep);
    g_signal_handler_disconnect (element, handler_id);

    gstd_update (manager, "/pipelines/pipe_sink/state", "paused");
    gstd_delete (manager, "/pipelines", "pipe_sink");

    g_print ("Finished\n");

    pthread_cancel (thread_performance_id);
    close_tegra ();

    /* Stop any IPC array */
    gstd_stop (manager);
    gst_deinit ();
    gstd_free(manager);
    

    return 0;
}
