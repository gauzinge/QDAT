INCLUDEFLAGS=-Iinclude/
COMPILERFLAGS+=-Wall
COMPILERFLAGS+=-Werror
COMPILERFLAGS+=-O0
COMPILERFLAGS+=-ggdb
COMPILERFLAGS+=-std=c++0x
ROOTFLAGS=`root-config --cflags`
ROOTLIBDIR=`root-config --libdir`
ROOTLIBFLAGS=`root-config --libs`


COMP=g++ $(COMPILERFLAGS) $(INCLUDEFLAGS)

all: plot process make_pdf
	
lib/plotstyle.o: src/plotstyle.cc include/plotstyle.h
	$(COMP) $(ROOTFLAGS) -c src/plotstyle.cc -o lib/plotstyle.o
	
lib/settings.o: src/settings.cc include/settings.h
	$(COMP) $(ROOTFLAGS) -c src/settings.cc -o lib/settings.o

lib/config.o: src/config.cc include/config.h
	$(COMP) $(ROOTFLAGS) -c src/config.cc -o lib/config.o
	
lib/calibration.o: src/calibration.cc include/calibration.h
	$(COMP) $(ROOTFLAGS) -c src/calibration.cc -o lib/calibration.o
	
lib/read_raw.o: src/read_raw.cc include/read_raw.h
	$(COMP) $(ROOTFLAGS) -c src/read_raw.cc -o lib/read_raw.o

lib/eventdata.o: src/eventdata.cc include/eventdata.h
		$(COMP) $(ROOTFLAGS) -c src/eventdata.cc -o lib/eventdata.o
		
lib/pedenoise.o: src/pedenoise.cc include/pedenoise.h
	$(COMP) $(ROOTFLAGS) -c src/pedenoise.cc -o lib/pedenoise.o

lib/noisemap.o: src/noisemap.cc include/noisemap.h
	$(COMP) $(ROOTFLAGS) -c src/noisemap.cc -o lib/noisemap.o
	
lib/langaus.o: src/langaus.cc include/langaus.h
		$(COMP) $(ROOTFLAGS) -c src/langaus.cc -o lib/langaus.o
		
lib/data.o: src/data.cc include/data.h
		$(COMP) $(ROOTFLAGS) -c src/data.cc -o lib/data.o
		
lib/cluster.o: src/cluster.cc include/cluster.h
		$(COMP) $(ROOTFLAGS) -c src/cluster.cc -o lib/cluster.o
		
lib/noiseprofile.o: src/plots/noiseprofile.cc include/plots.h
		$(COMP) $(ROOTFLAGS) -c src/plots/noiseprofile.cc -o lib/noiseprofile.o

lib/beamprofile.o: src/plots/beamprofile.cc include/plots.h
		$(COMP) $(ROOTFLAGS) -c src/plots/beamprofile.cc -o lib/beamprofile.o
		
lib/clusterwidth.o: src/plots/clusterwidth.cc include/plots.h
		$(COMP) $(ROOTFLAGS) -c src/plots/clusterwidth.cc -o lib/clusterwidth.o
	
lib/eta.o: src/plots/eta.cc include/plots.h
		$(COMP) $(ROOTFLAGS) -c src/plots/eta.cc -o lib/eta.o
		
lib/signal.o: src/plots/signal.cc include/plots.h
		$(COMP) $(ROOTFLAGS) -c src/plots/signal.cc -o lib/signal.o
		
lib/signal_to_noise.o: src/plots/signal_to_noise.cc include/plots.h
		$(COMP) $(ROOTFLAGS) -c src/plots/signal_to_noise.cc -o lib/signal_to_noise.o

lib/hitmap_2D.o: src/plots/hitmap_2D.cc include/plots.h
		$(COMP) $(ROOTFLAGS) -c src/plots/hitmap_2D.cc -o lib/hitmap_2D.o
		
# lib/plots.so: $(COMP) $(ROOTFLAGS) -shared -o lib/plots.so lib/noiseprofile.o lib/beamprofile.o -lm

plot: src/plot.cc \
	lib/plotstyle.o \
	lib/settings.o \
	lib/config.o \
	lib/calibration.o \
	lib/read_raw.o \
	lib/eventdata.o \
	lib/pedenoise.o \
	lib/noisemap.o \
	lib/langaus.o \
	lib/data.o \
	lib/cluster.o \
	lib/noiseprofile.o \
	lib/clusterwidth.o \
	lib/beamprofile.o \
	lib/eta.o \
	lib/signal.o \
	lib/signal_to_noise.o \
	lib/hitmap_2D.o
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/plot.cc \
	lib/plotstyle.o \
	lib/settings.o \
	lib/config.o \
	lib/calibration.o \
	lib/read_raw.o \
	lib/eventdata.o \
	lib/pedenoise.o \
	lib/noisemap.o \
	lib/langaus.o \
	lib/data.o \
	lib/cluster.o \
	lib/noiseprofile.o \
	lib/beamprofile.o \
	lib/clusterwidth.o \
	lib/eta.o \
	lib/signal.o \
	lib/signal_to_noise.o \
	lib/hitmap_2D.o \
	-o plot

process: src/process.cc \
	lib/settings.o \
	lib/plotstyle.o \
	lib/config.o \
	lib/calibration.o \
	lib/read_raw.o \
	lib/eventdata.o \
	lib/pedenoise.o \
	lib/noisemap.o \
	lib/langaus.o \
	lib/data.o \
	lib/cluster.o 
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/process.cc \
	lib/plotstyle.o \
	lib/settings.o \
	lib/config.o \
	lib/calibration.o \
	lib/read_raw.o \
	lib/eventdata.o \
	lib/pedenoise.o \
	lib/noisemap.o \
	lib/langaus.o \
	lib/data.o \
	lib/cluster.o \
	-o process

make_pdf: src/make_pdf.cc 
	$(COMP) $(ROOTFLAGS) $(ROOTLIBFLAGS) \
	src/make_pdf.cc \
	-o make_pdf
	
clean:
	rm -f process plot make_pdf lib/*.o
