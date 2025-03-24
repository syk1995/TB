#############################################
#                                           #
############  makefile  #####################
#                                           #
#############################################

GENERAL=ECAL
PLOT=cepcplotstyle
TARGET1=HitMap

##set up control#######
TOP:= $(shell pwd)
DIR_OBJ:= $(TOP)/obj
DIR_BIN:= $(TOP)/bin
DIR_SRC:= $(TOP)/src
DIR_INCLUDE:= $(TOP)/include
PLOT_SRC := /home/llr/ilc/shi/code/Plot_style/cepcplotstyle.cxx
PLOT_INCLUDE := /home/llr/ilc/shi/code/Plot_style 

##ROOT#######################################
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

#CPPLIBS = $(ROOTLIBS) $(ROOTGLIBS) -ltbb -lSpectrum
CPPLIBS = $(ROOTLIBS) $(ROOTGLIBS)
DIR_INCLUDE+=$(ROOTCFLAGS)

# #set up compilers#
CXX = g++
CPPFLAGS = -g -Wall -I$(DIR_INCLUDE) -I$(PLOT_INCLUDE)

####### Make Execuatbles#######
all: $(TARGET1)

$(DIR_OBJ)/%.o:$(DIR_SRC)/%.cxx  
	$(CXX) $(CPPFLAGS)  -c $(DIR_SRC)/$(notdir $<)  -o $(DIR_OBJ)/$(notdir $@)
$(DIR_OBJ)/$(PLOT).o: $(PLOT_SRC)
	$(CXX) $(CPPFLAGS) -c $< -o $@

#$(TARGET1):$(DIR_OBJ)/$(GENERAL).o $(DIR_OBJ)/$(TARGET1).o $(DIR_OBJ)/$(PLOT).o
#	$(CXX) $(DIR_OBJ)/$(GENERAL).o $(DIR_OBJ)/$(TARGET1).o $(CPPLIBS)  -o $(DIR_BIN)/$(notdir $@)

$(TARGET1): $(DIR_OBJ)/$(GENERAL).o $(DIR_OBJ)/$(TARGET1).o $(DIR_OBJ)/$(PLOT).o
	$(CXX) $^ $(CPPLIBS) -o $(DIR_BIN)/$(notdir $@)	



.PHONY:clean
clean:  
	rm -f $(DIR_OBJ)/*.o rm -f main
