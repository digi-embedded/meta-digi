CFLAGS = -Wall -g -D_GNU_SOURCE

all: adc_sample

adc_sample: adc_sample.o iio_utils.o

%.o: %.c iio_utils.h

.PHONY: clean
clean:
	rm -f *.o adc_sample
