/*
 * Author: Jose Montoya <jose.montoya@ridgerun.com>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gstd.h>

GstD *manager = NULL;
gulong handler_id = 0;

typedef struct _SinkData SinkData;
struct _SinkData
{
  char *my_message;
};

static GstdReturnCode sink_callback (GstElement *sink, void *user_data)
{ 
  SinkData *sink_data = (SinkData *) user_data;
  GstSample *sample = NULL;
  GstBuffer *buffer = NULL;
  GstMapInfo info = {0};
  FILE * fptr = NULL;


  fptr = fopen("img.jpg", "w");
  g_signal_emit_by_name (sink, "pull-sample", &sample);
  buffer = gst_sample_get_buffer(sample);
  gst_buffer_map(buffer, &info, GST_MAP_READ);

  if (sample) {
    fwrite(info.data, 1, info.size+1, fptr);

    gst_buffer_unmap (buffer, &info);
    gst_sample_unref (sample);
    fclose(fptr);
  }

  g_print("Hi! %s\n", sink_data->my_message);
  
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
    GstElement *appsink = NULL;
    
    SinkData *sink_data = NULL;

    gstd_new (&manager, 0, NULL);

    g_print ("Starting...\n");
    if (!gstd_start (manager)) {
        return FALSE;
    }

    /* APP SINK TEST */
    sink_data = malloc (sizeof (SinkData));
    sink_data->my_message = strdup("GstD community");

    // ret = gstd_create(manager, "/pipelines", "pipe_sink", "videotestsrc name=vts num-buffers=1 ! jpegenc ! appsink name=as emit-signals=true async=false drop=true");
    ret = gstd_create(manager, "/pipelines", "pipe_sink", "v4l2src ! jpegdec ! jpegenc ! appsink name=as emit-signals=true async=false drop=true"); /* To use a web cam */
    if (GSTD_EOK != ret) {
       printf ("Error: There was a problem creating a GstD: %d\n", ret);
       return -1;
    }

    gstd_read (manager, "pipelines/pipe_sink/elements/as", &resource);
    g_object_get (resource, "gstelement", &appsink, NULL);
    handler_id = g_signal_connect (appsink, "new-sample", G_CALLBACK (sink_callback), sink_data);

    gstd_update (manager, "/pipelines/pipe_sink/state", "playing");
  

    g_print ("Finished\n");

    main_loop = g_main_loop_new (NULL, FALSE);

    g_unix_signal_add (SIGINT, int_term_handler, main_loop);
    g_unix_signal_add (SIGTERM, int_term_handler, main_loop);

    g_main_loop_run (main_loop);


    /* Clean */
    g_signal_handler_disconnect (appsink, handler_id);

    gstd_update (manager, "/pipelines/pipe_sink/state", "paused");
    gstd_delete (manager, "/pipelines", "pipe_sink");

    /* Stop any IPC array */
    gstd_stop (manager);
    gst_deinit ();
    gstd_free(manager);
    
    g_main_loop_unref (main_loop);
    main_loop = NULL;

    return 0;
}
