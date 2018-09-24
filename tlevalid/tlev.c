#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <asm/errno.h>

#include "predict/predict.h"

struct tle_t {
  char *elements[2];
};
struct tle_t tle;

#define TXT_NORM "\x1B[0m"
#define TXT_RED  "\x1B[31m"
#define TXT_GRN  "\x1B[32m"
#define TXT_YEL  "\x1B[33m"
#define TXT_BLU  "\x1B[34m"
#define TXT_MAG  "\x1B[35m"
#define TXT_CYN  "\x1B[36m"
#define TXT_WHT  "\x1B[37m"

uint64_t timestamp_ms(void)
{
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  return ((uint64_t) spec.tv_sec) * 1000 + (((uint64_t) spec.tv_nsec) / 1000000);
}

int tle_load(char *_filename, struct tle_t *_tle)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(_filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "\nError opening TLE file!");
        return -1;
    }

    int n = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(read > 50)
        {
            if(_tle->elements[n] != NULL)
            {
                free(_tle->elements[n]);
            }
            _tle->elements[n] = malloc(read);
            strncpy(_tle->elements[n], line, read);
            n++;
            if(n > 1)
            {
                break;
            }
        }
    }

    fclose(fp);
    if (line)
        free(line);
    
    if(n != 2)
    {
        fprintf(stderr,"\nError reading TLE from file!");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
  double current_julian;

  predict_orbital_elements_t *elements;
  struct predict_orbit orbit;

  if(argc != 2)
  {
    printf("tlevalid, an orbital TLE validator\n\n"
      "Usage:\ttlevalid <tle filename>\n"
      );
    exit(1);
  }

  printf("Loading TLE from file..      ");

  if(tle_load(argv[1], &tle) < 0)
  {
    printf(TXT_RED"Error loading TLE from file!"TXT_NORM"\n");
    exit(1);
  }
  printf(TXT_GRN"OK"TXT_NORM"\n");


  printf("Parsing TLE data..           ");

  elements = predict_parse_tle(tle.elements);
  if(elements == NULL)
  {
    printf(TXT_RED"FAIL"TXT_NORM" (Error parsing tle)\n");
    exit(1);
  }

  printf(TXT_GRN"OK"TXT_NORM" (Epoch: 20%01d.%2.2f)\n",
    elements->epoch_year,
    elements->epoch_day
  );


  printf("Calculating orbit..          ");

  current_julian = julian_from_timestamp_ms(timestamp_ms());
  if(predict_orbit(elements, &orbit, current_julian) < 0)
  {
    printf(TXT_RED"FAIL"TXT_NORM" (Error calculating orbit from tle)\n");
    exit(1);
  }
  if(orbit.altitude < 100
    || orbit.revolutions < 0)
  {
    printf(TXT_RED"FAIL"TXT_NORM" (Results failed sanity check: Altitude: %.3fkm, Revolutions: %ld)\n",
      orbit.altitude,
      orbit.revolutions
    );
    exit(1);
  }

  printf(TXT_GRN"OK"TXT_NORM" (Altitude: %.3fkm, Revolutions: %ld)\n",
    orbit.altitude,
    orbit.revolutions
  );

  predict_destroy_orbital_elements(elements);
  exit(0);
}
