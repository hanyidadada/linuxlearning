SOURCES := $(wildcard *.c ./*/*.c)
TARGETLIST := $(patsubst %.c,%,$(SOURCES))

all:${TARGETLIST}
	
.PHONY:clean
clean:
	$(foreach TARGET,${TARGETLIST},rm -f $(TARGET);)