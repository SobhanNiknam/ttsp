CXX=g++
CFLAGS=-c -O3
LDFLAGS=-lm

SRC = configurationParameters.cc TTSP.cc main.cc
TTSP_OBJ=$(SRC:.cc=.o)
TTSP_EX = TTSP

all: $(TTSP_EX)

$(TTSP_EX): $(TTSP_OBJ) 
	$(CXX) $(TTSP_OBJ) $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@
	
clean:
	rm -rf *.o $(TTSP_EX) 
