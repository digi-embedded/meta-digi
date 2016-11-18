CFLAGS = -Wall -g -D_GNU_SOURCE

all: tamper_sample

tamper_sample: tamper_sample.o iio_utils.o

%.o: %.c iio_utils.h

.PHONY: clean
clean:
	rm -f *.o tamper_sample
