
CFLAGS=-O3 -Wall -Wextra -Wpedantic -Werror

LIBPREDICT_DIR = predict
LIBPREDICT_SRCS += $(LIBPREDICT_DIR)/julian_date.c \
		$(LIBPREDICT_DIR)/observer.c \
		$(LIBPREDICT_DIR)/orbit.c \
		$(LIBPREDICT_DIR)/refraction.c \
		$(LIBPREDICT_DIR)/sdp4.c \
		$(LIBPREDICT_DIR)/sgp4.c \
		$(LIBPREDICT_DIR)/sun.c \
		$(LIBPREDICT_DIR)/unsorted.c

all:
	gcc tlev.c $(LIBPREDICT_SRCS) $(CFLAGS) -lm -o ../tlev
clean:
	rm -fv tlev ../tlev
