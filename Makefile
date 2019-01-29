CC=gcc
CXX=g++
CFLAGS=-std=c99 -O2
CXXFLAGS=-std=c++11 
LDFLAGS=-lm -lpthread

all: synthesis flounder

synthesis:
	@VIVADO_HLS_DIR=$(dirname $(dirname $(which vivado_hls)))
	vivado_hls -f Synthesis.tcl
	$(CXX) $(CXXFLAGS) -I./hlslib/include -I$(VIVADO_HLS_DIR)/include -c pl_graph_ops.cc -o pl_graph_ops.o

flounder: flounder.o refine.o adj_ops.o pl_graph_ops.o graph_ops.o graph.o rgraph.o adj.o ints.o space.o vtk.o size.o
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	rm -f *.o
	rm -r hls || true
	rm vivado_hls.log || true
	rm report.rpt || true
