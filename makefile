# Makefile created by Rhyscitlema
# Explanation of file structure available at:
# http://rhyscitlema.com/applications/makefile.html

OUTPUT_FILE = lib_std.a

OBJECT_FILES = _math.o \
               _value.o \
               _stdio.o \
               _strfun.o \
               _string.o \
               _malloc.o \
               _texts.o

# needed linker libs: -lm

#-------------------------------------------------

# C compiler
CC = gcc

# archiver
AR = ar

# compiler flags
CC_FLAGS = -Wall $(CFLAGS)

# archiver flags
AR_FLAGS = -crs #$(ARFLAGS)

#-------------------------------------------------

make: $(OUTPUT_FILE)

$(OUTPUT_FILE): $(OBJECT_FILES)
	$(AR) $(AR_FLAGS) $(OUTPUT_FILE) $(OBJECT_FILES)

# remove all created files
clean:
	$(RM) *.o *.a

#-------------------------------------------------

# compile .c files to .o files
%.o: %.c *.h
	$(CC) $(CC_FLAGS) -c -o $@ $<

