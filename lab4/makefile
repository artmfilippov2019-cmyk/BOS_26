CC = gcc
CPPFLAGS = -D_GNU_SOURCE
CFLAGS = -std=c11 -Wall -Wextra -O0 -g

PRORAMS = pinfo forkdemo forkdemo3 launcher childinfo signalwatch pipe-demo \
	fifo-writer fifo-reader shm-writer shm-reader cowdemo zombie orphan \
	threads myps myrun mytimeout safe-launch mypmap proc-audit audit-target

.PHONY: all
all: $(PROGRAMS)

threads: CFLAGS += -pthread 
safe-launch: CPPFLAGS += -DCHILDINFO_PATH='"$(abspath childinfo)"'
safe-launch: | childinfo
