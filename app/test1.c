/*
 * Author: Jose Montoya <jose.montoya@ridgerun.com>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gstd.h>

GstD *manager = NULL;

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

    int ret = GSTD_EOK;
    int time_sleep = 1;

    GstdObject *resource = NULL;
    GstElement *element = NULL;
    
    gstd_new (&manager, 0, NULL);

    g_print ("Starting...\n");
    if (!gstd_start (manager)) {
        return FALSE;
    }

    ret = gstd_create (manager, "/pipelines", "p", "videotestsrc name=vts ! autovideosink");
    if (GSTD_EOK != ret) {
      g_print ("Failed \n");
      return 0;
    }
    
    gstd_read (manager, "pipelines/p/elements/vts", &resource);

    /* Get the gstreamer element from the resource */
    g_object_get (resource, "gstelement", &element, NULL);

    /* Change the property from the gstreamer element */
    g_object_set (element, "pattern", 1, NULL);

    gstd_update (manager, "/pipelines/p/state", "playing");
    sleep (time_sleep);
    gstd_update (manager, "/pipelines/p/state", "paused");
    sleep (time_sleep);
    gstd_delete (manager, "/pipelines", "p");

    main_loop = g_main_loop_new (NULL, FALSE);

    g_unix_signal_add (SIGINT, int_term_handler, main_loop);
    g_unix_signal_add (SIGTERM, int_term_handler, main_loop);

    g_main_loop_run (main_loop);

    /* Stop any GstD array */
    gstd_stop (manager);
    gst_deinit ();
    gstd_free(manager);
    
    g_main_loop_unref (main_loop);
    main_loop = NULL;

    return TRUE;
}
