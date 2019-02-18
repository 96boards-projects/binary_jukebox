/*
#################################################################
# Title: binjuke.c
# Author: Sahaj Sarup
# Copyright (c) 2019 Linaro Limited
#################################################################
*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "mraa/i2c.h"
#include "fpga_mezz.h"
#include <vlc/vlc.h>

int update_panel(uint8_t num, mraa_i2c_context i2c);

int main (void)
{
  /* init libvlc for playback */
  libvlc_instance_t *inst;
  libvlc_media_player_t *mp;
  libvlc_media_t *m;
  /* load the vlc engine */
  inst = libvlc_new(0, NULL);

  /* init ncurses */
  initscr();
  if(has_colors() == FALSE)
	{	endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
  start_color();
  init_pair(2, COLOR_WHITE, COLOR_BLUE);
  init_pair(3, COLOR_YELLOW, COLOR_BLUE);

  DIR *dp;
  struct dirent *ep;
  int count = 0;
  char file_name[256][256], file_name_print[256], file_path[300], now_playing[300];
  uint8_t num = 0;

  /* Open music directory */
  dp = opendir ("./music/");
  if (dp != NULL)
    {
      while ((ep = readdir (dp)) && count < 256)
      {
	      if (ep->d_type == DT_REG)
        {
          strcpy(file_name[count],ep->d_name);
          count++;
          //puts (ep->d_name);
        }
      }
      (void) closedir (dp);
    }
  else
    perror ("Couldn't open the directory");

  sprintf(file_path, "./music/%s", file_name[num]);
  m = libvlc_media_new_path(inst, file_path);
  mp = libvlc_media_player_new_from_media(m);
  libvlc_media_release(m);
  // play the media_player
  libvlc_media_player_play(mp);
  attron(COLOR_PAIR(3));
  sprintf(now_playing, "Now Playing: %s", file_name[num]);
  mvprintw(45, 5, now_playing);
  attroff(COLOR_PAIR(3));
  sleep(1);

    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c;

    mraa_init();
    i2c = mraa_i2c_init(I2C_BUS);

    int input, ver;

    if (i2c == NULL) {
        fprintf(stderr, "Failed to initialize I2C\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }


    status = mraa_i2c_address(i2c, FPGA_MEZZI_ADDR);
    if (status != MRAA_SUCCESS) {
    }


    if(test(i2c)==0)
    {
        printf("Test Passed\n");
    }
    else
    {
      printf("Test Failed\n");
      return 1;
    }

    /* Output LED PINS */
    for (int i = 2; i <= 9; i++)
    {
        pinMode(i,OUTPUT,i2c);
    }

    /* Input pins */
    pinMode(10,INPUT,i2c);
    pinMode(11,INPUT,i2c);
    pinMode(13,INPUT,i2c);



    while(1)
    {
        if(digitalRead(10,i2c) == 1)
        {
            num = (num << 1) + 1 ;
            update_panel(num,i2c);
            /* 500ms sleep for fake debouncing */
            usleep(500000);

        }

        if(digitalRead(11,i2c) == 1)
        {
            num = (num << 1);
            update_panel(num,i2c);
            /* 500ms sleep for fake debouncing */
            usleep(500000);
        }

        if(digitalRead(13,i2c) == 1)
        {
            if(libvlc_media_player_is_playing(mp))
            {
              libvlc_media_player_stop(mp);
            }
            sprintf(file_path, "./music/%s", file_name[num]);
            m = libvlc_media_new_path(inst, file_path);
            mp = libvlc_media_player_new_from_media(m);
            libvlc_media_release(m);
            // play the media_player
            libvlc_media_player_play(mp);
            // Wait for media to actually start playback
            sleep(1);
        }


        // Start Display
        for(int i = 0, j = 0, k = 0; i < count; i++,j++)
        {

        if (i == num)
          attron(COLOR_PAIR(2));
        /* Print all file names */
        sprintf(file_name_print, "%d: %s", i, file_name[i]);
        file_name_print[15] = '\0';
        switch(j)
        {
          case 0: mvprintw(k, 0, file_name_print);
                  break;
          case 1: mvprintw(k, 20, file_name_print);
                  break;
          case 2: mvprintw(k, 40, file_name_print);
                  break;
          case 3: mvprintw(k, 60, file_name_print);
                  break;
          case 4: mvprintw(k, 80, file_name_print);
                  break;
          case 5: mvprintw(k, 100, file_name_print);
                  break;
          case 6: mvprintw(k, 120, file_name_print);
                  break;
          case 7: mvprintw(k, 140, file_name_print);
                  break;
          case 8: mvprintw(k, 160, file_name_print);
                  break;
          case 9: mvprintw(k, 180, file_name_print);
                  break;
          case 10: mvprintw(k, 200, file_name_print);
                  k = k + 2;
                  j = -1;
                  break;
        }

        if (i == num)
          attroff(COLOR_PAIR(2));
        }

        /* Move to next song */
        if (libvlc_media_player_is_playing(mp) != 1)
        {
            num++;
            update_panel(num,i2c);
            sprintf(file_path, "./music/%s", file_name[num]);
            m = libvlc_media_new_path(inst, file_path);
            mp = libvlc_media_player_new_from_media(m);
            libvlc_media_release(m);
            // play the media_player
            libvlc_media_player_play(mp);
            attron(COLOR_PAIR(3));
            sprintf(now_playing, "Now Playing: %s", file_name[num]);
            mvprintw(45, 5, now_playing);
            attroff(COLOR_PAIR(3));
            sleep(1);
        }

        refresh();
        usleep(100);
    }
    libvlc_release(inst);
    endwin();
}

int update_panel(uint8_t num, mraa_i2c_context i2c)
{
    for(int i = 0, j = 2; i < 7, j <= 9; i++, j++)
    {
        if(((num >> i) & 1U) == 1)
        {
            digitalWrite(j,HIGH,i2c);
        }

        else if(((num >> i) & 1U) == 0)
        {
            digitalWrite(j,LOW,i2c);
        }
    }
}
