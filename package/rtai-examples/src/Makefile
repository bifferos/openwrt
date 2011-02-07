

all: rtai-led-blink


rtai-led-blink: rtai-led-blink.c
	$(CC) $(LXRT_CFLAGS) -o $@ $< $(LXRT_LDFLAGS) -llxrt

clean:
	rm -f *.o *~ rtai-led-blink

