APP = get-32 get-64
CFLAGS += -Wall

all: $(APP)

get-%: get.c
	$(CC) $(CFLAGS) get.c -D WIDTH=$* -o $@

clean:
	rm -f $(APP)
