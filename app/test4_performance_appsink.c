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

void close_tegra () {
  char line[10];
  pid_t pid = 0;
  gchar* kill_process_cmd = NULL;

  FILE *cmd = popen ("pidof tegrastats", "r");

  fgets (line, 10, cmd);

  pid = strtoul (line, NULL, 10);

  pclose (cmd);

  kill_process_cmd = g_strdup_printf ("kill %d", pid);

  system (kill_process_cmd);
}

void *measure_performance (void *args) {
    FILE *fp = NULL;

    fp = popen("tegrastats --interval 500 --logfile test4_performance_appsink.txt", "r");
    if (fp ==NULL) {
      printf ("Failed to run command");
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int ret = GSTD_EOK;
    int time_sleep = 5;

    pthread_t thread_performance_id;
    pthread_create (&thread_performance_id, NULL, measure_performance, NULL);

    gstd_new (&manager, 0, NULL);

    g_print ("Starting...\n");
    if (!gstd_start (manager)) {
        return FALSE;
    }

    ret = gstd_create(manager, "/pipelines", "pipe_sink", "videotestsrc name=vts ! jpegenc ! appsink name=as emit-signals=true async=false drop=true");
    if (GSTD_EOK != ret) {
       printf ("Error: There was a problem creating a GstD: %d\n", ret);
       return -1;
    }

    gstd_update (manager, "/pipelines/pipe_sink/state", "playing");
    sleep (time_sleep);

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
