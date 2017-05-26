CC = gcc
C_FLAGS = -g -o 
RM = rm
PYTHON = /usr/include/python2.7 -lpython2.7
CONTROL = ControlServer.c
OUT = ControlServer
OBJDUMP = objdump -d
DUMP_FILE = ControlServer.a

#$$(/usr/bin/python2.7-config --ldflags)
controlserver:
	$(CC) $(C_FLAGS) $(OUT) $(CONTROL) -I $(PYTHON)

all: controlserver

default: controlserver

clean:
	$(RM) $(OUT)

import:
	export PYTHONPATH=$$PYTHONPATH:~/ScienceWorkflow/ScienceWorkflow_Control_Software/Adafruit_Python_PCA9685/Adafruit_PCA9685/

dump:
	$(OBJDUMP) $(OUT) > $(DUMP_FILE)
