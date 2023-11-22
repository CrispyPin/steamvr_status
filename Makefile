VERSION := 1.0.0
CXX := g++
TARGET := ./vr_status
LFLAGS := -Wl,-rpath,'$$ORIGIN' -L. -lopenvr_api

build:
	$(CXX) src/main.cpp $(CPPFLAGS) $(LFLAGS) -o $(TARGET)

release: build
	mkdir -p steamvr-status
	cp -f libopenvr_api.so steamvr-status
	cp -f $(TARGET) steamvr-status
	tar -caf steamvr-status-$(VERSION).tar.xz steamvr-status
