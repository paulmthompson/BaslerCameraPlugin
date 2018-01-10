
LIBNAME := $(notdir $(CURDIR))
OBJDIR := $(OBJDIR)/$(LIBNAME)
TARGET := $(LIBNAME).so


SRC_DIR := ${shell find ./ -type d -print}
VPATH := $(SOURCE_DIRS)

SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ := $(addprefix $(OBJDIR)/,$(notdir $(SRC:.cpp=.o)))

# Installation directories for pylon
PYLON_ROOT ?= /opt/pylon5

CPPFLAGS   += $(shell $(PYLON_ROOT)/bin/pylon-config --cflags)
LDFLAGS    += $(shell $(PYLON_ROOT)/bin/pylon-config --libs-rpath)
LDLIBS     += $(shell $(PYLON_ROOT)/bin/pylon-config --libs)
CXXFLAGS   += -I /opt/pylon5/include


BLDCMD := $(CXX) -shared -o $(OUTDIR)/$(TARGET) $(OBJ) $(LDFLAGS) $(LDLIBS) $(RESOURCES) $(TARGET_ARCH)

VPATH = $(SRC_DIR)

.PHONY: objdir

$(OUTDIR)/$(TARGET): objdir $(OBJ)
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@echo "Building $(TARGET)"
	@$(BLDCMD)

$(OBJDIR)/%.o : %.cpp
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"
	
	
objdir:
	-@mkdir -p $(OBJDIR)

clean:
	@echo "Cleaning $(LIBNAME)"
	-@rm -rf $(OBJDIR)
	-@rm -f $(OUTDIR)/$(TARGET)

-include $(OBJ:%.o=%.d)
