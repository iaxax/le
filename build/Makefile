# Example Makefile for ROSE users
# This makefile is provided as an example of how to use ROSE when ROSE is
# installed (using "make install"). The makefile is tested as part of the
# "make distcheck" rule (run as part of tests before any SVN checkin).
# rule *run as part of "make distcheck"


# Location of rose installation
ROSE_INSTALL_DIR = /home/anson/Code/Github/rose_build_dir

# Location of include directory after "make install"
ROSE_INCLUDE_DIR = $(ROSE_INSTALL_DIR)/include/rose

# Location of boost installation
BOOST_INSTALL_DIR = /home/anson/Software/boost/boost_1_61_0_build

# Location of Boost include directory
BOOST_CPPFLAGS = -pthread -I $(BOOST_INSTALL_DIR)/include/boost

# Location of library directory after "make install"
ROSE_LIB_DIR = $(ROSE_INSTALL_DIR)/lib

# Location of root of project
LE_ROOT_DIR = /home/anson/Code/C++/le

# Location of source code
LE_SOURCE_DIR = $(LE_ROOT_DIR)/src

# Location of Loop Extraction include directory
LE_INCLUDE_DIR = $(LE_ROOT_DIR)/include

CXX         = g++
CPPFLAGS    = -std=c++11 -g -Wall
LDFLAGS     = -lboost_system

ROSE_LIBS = $(ROSE_LIB_DIR)/librose.la

executableFiles = le

objectFiles = astHelper.o visitor.o \
							loopNameAllocator.o loopExtraction.o \
							loop.o printer.o main.o

# Default make rule to use
all: $(executableFiles)
    @if [ x$${ROSE_IN_BUILD_TREE:+present} = xpresent ]; then echo "ROSE_IN_BUILD_TREE should not be set" >&2; exit 1; fi

$(executableFiles): $(objectFiles)
	/bin/sh $(ROSE_INSTALL_DIR)/libtool --mode=link $(CXX) $(CPPFLAGS) $(LDFLAGS) \
	-I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) $(BOOST_CPPFLAGS) \
	-o $@ $^ $(ROSE_LIBS)

astHelper.o: $(LE_SOURCE_DIR)/astHelper.cpp \
	$(LE_INCLUDE_DIR)/astHelper.h $(LE_INCLUDE_DIR)/message.h
	$(CXX) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) -c $< -o $@

loop.o: $(LE_SOURCE_DIR)/loop.cpp $(LE_INCLUDE_DIR)/loop.h
	$(CXX) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) -c $< -o $@

loopExtraction.o: $(LE_SOURCE_DIR)/loopExtraction.cpp \
	$(LE_INCLUDE_DIR)/loopExtraction.h $(LE_INCLUDE_DIR)/message.h \
	$(LE_INCLUDE_DIR)/constraint.h $(LE_INCLUDE_DIR)/printer.h \
	$(LE_INCLUDE_DIR)/visitor.h $(LE_INCLUDE_DIR)/astHelper.h \
	$(LE_INCLUDE_DIR)/loopNameAllocator.h
	$(CXX) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) -c $< -o $@

loopNameAllocator.o: $(LE_SOURCE_DIR)/loopNameAllocator.cpp \
	$(LE_INCLUDE_DIR)/loopNameAllocator.h
	$(CXX) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) -c $< -o $@

main.o: $(LE_SOURCE_DIR)/main.cpp
	$(CXX) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) -c $< -o $@

printer.o: $(LE_SOURCE_DIR)/printer.cpp \
	$(LE_INCLUDE_DIR)/printer.h $(LE_INCLUDE_DIR)/astHelper.h
	$(CXX) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) -c $< -o $@

visitor.o: $(LE_SOURCE_DIR)/visitor.cpp $(LE_INCLUDE_DIR)/visitor.h
	$(CXX) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(LE_INCLUDE_DIR) -c $< -o $@

.PHONY clean:
	rm -f $(executableFiles) $(objectFiles)
