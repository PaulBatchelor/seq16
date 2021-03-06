LDFLAGS = -lpthread -lglfw -lGL -lsporth -lsoundpipe -ldl -lm
CFLAGS = -Wall -ansi
default: seq16.so


seq16.so: seq16.c glyphs.h
	$(CC) $(CFLAGS) -fPIC -shared seq16.c -o $@ $(LDFLAGS)

glyphs.h: draw.pl
	perl $< > $@

clean:
	rm -rf seq16.so
