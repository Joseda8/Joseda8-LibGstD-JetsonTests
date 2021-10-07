/*
 * Copyright (C) 2021 BestSeat 2021
 * Author: Manuel Leiva <manuel.leiva@ridgerun.com>.
 */
//#include "bestseat_mediaserver_client.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gstd.h>
#include <time.h>

pthread_mutex_t lock;
int frames_counter = 0;

GstD *manager = NULL;
gulong handler_id = 0;

typedef struct _SinkData SinkData;
struct _SinkData
{
  char *my_message;
};

void *run_timer (void *args) {
    int msec = 0;
    clock_t before = clock ();
    do
    {
      for (int i = 0; i < 1000000; i++) {} // Some process

      clock_t difference = clock () - before;
      msec = difference * 1000 / CLOCKS_PER_SEC;

      printf ("Seconds: %d \n", msec / 1000);
      pthread_mutex_lock (&lock);
      printf ("FrameRate: %.2d \n\n", frames_counter / (msec / 1000));
      pthread_mutex_unlock (&lock);
    } while (TRUE);

    return NULL;
}


static GstdReturnCode sink_callback (GstElement *sink, void *user_data)
{ 
  GstSample *sample = NULL;
  GstBuffer *buffer = NULL;
  GstMapInfo info = {0};

  g_signal_emit_by_name (sink, "pull-sample", &sample);
  buffer = gst_sample_get_buffer(sample);
  gst_buffer_map(buffer, &info, GST_MAP_READ);

  if (sample) {
    pthread_mutex_lock (&lock);
    frames_counter++;
    pthread_mutex_unlock (&lock);
    gst_buffer_unmap (buffer, &info);
    gst_sample_unref (sample);
  }
  
  return GSTD_EOK;
}

static gboolean
int_term_handler (gpointer user_data)
{
  GMainLoop *main_loop;

  main_loop = (GMainLoop *) user_data;
  g_main_loop_quit (main_loop);

  printf("\nBye!\n");

  return TRUE;
}

int main(int argc, char *argv[])
{
    GMainLoop *main_loop;
    int ret;

    GstdObject *resource = NULL;
    GstElement *element = NULL;
    

    pthread_t thread_timer_id;
    pthread_create (&thread_timer_id, NULL, run_timer, NULL);   

    pthread_mutex_init (&lock, NULL);

    gstd_new (&manager, 0, NULL);

    g_print ("Starting...\n");
    if (!gstd_start (manager)) {
        return FALSE;
    }

    ret = gstd_create(manager, "/pipelines", "pipe_sink", "videotestsrc name=vts pattern=4 ! jpegenc ! appsink name=as emit-signals=true async=false drop=true");
    if (GSTD_EOK != ret) {
       printf ("Error: There was a problem creating a GstD: %d\n", ret);
       return -1;
    }

    gstd_read (manager, "pipelines/pipe_sink/elements/as", &resource);
    g_object_get (resource, "gstelement", &element, NULL);
    handler_id = g_signal_connect (element, "new-sample", G_CALLBACK (sink_callback), NULL);

    gstd_update (manager, "/pipelines/pipe_sink/state", "playing");
    


    g_print ("Finished\n");

    main_loop = g_main_loop_new (NULL, FALSE);

    g_unix_signal_add (SIGINT, int_term_handler, main_loop);
    g_unix_signal_add (SIGTERM, int_term_handler, main_loop);

    g_main_loop_run (main_loop);

    g_signal_handler_disconnect (element, handler_id);

    /* Clean */
    gstd_update (manager, "/pipelines/pipe_sink/state", "paused");
    gstd_delete (manager, "/pipelines", "pipe_sink");
    pthread_cancel (thread_timer_id);
    pthread_mutex_destroy (&lock);

    /* Stop any IPC array */
    gstd_stop (manager);
    gst_deinit ();
    gstd_free(manager);
    
    g_main_loop_unref (main_loop);
    main_loop = NULL;

    return 0;
}
