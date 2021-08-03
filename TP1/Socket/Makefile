# the compiler
CC = g++

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall

# the build target executable:
TARGET = produtor
TARGET2 = consumidor

all: $(TARGET) $(TARGET2)

$(TARGET): $(TARGET).cpp
	$(CC) -o $(TARGET).out $(TARGET).cpp

$(TARGET2): $(TARGET2).cpp
	$(CC) -o $(TARGET2).out $(TARGET2).cpp

clean:
	$(RM) $(TARGET)
