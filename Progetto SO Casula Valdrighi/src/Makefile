OUTPUT_DIR := .

SRCS := hmi.c centralEcu.c inputForHMI.c FrontWindshieldCamera.c parkAssist.c SteerByWire.c BrakeByWire.c throttleControl.c SurroundViewCameras.c 

TARGETS := $(patsubst %.c,$(OUTPUT_DIR)/%,$(SRCS))

all: $(TARGETS)

$(OUTPUT_DIR)/%: %.c 
	gcc -o $@ $^

clean:
	rm -rf $(TARGETS)
	rm -f brakePipe
	rm -f parkPipe
	rm -f throttlePipe
	rm -f steerPipe
	rm -f ecuPipe
	rm -f inputPipe
	rm -f socketProgetto
	rm -f steerHMI	
	rm -f surroundPipe
	find . -type f -name "*.log" -delete
	

.PHONY: all clean
