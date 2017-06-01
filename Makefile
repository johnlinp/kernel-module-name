APP = get-32 get-64
CFLAGS += -Wall -std=c99

all: $(APP)

get-%: get.c
	$(CC) $(CFLAGS) get.c -D WIDTH=$* -o $@

clean:
	rm -f $(APP)
