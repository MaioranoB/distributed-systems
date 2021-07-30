# the compiler
CC = g++

# the build target executable:
TARGET  = sender
TARGET2 = receiver

all: $(TARGET) $(TARGET2)

$(TARGET): $(TARGET).cpp
	$(CC) -o $(TARGET).out $(TARGET).cpp

$(TARGET2): $(TARGET2).cpp
	$(CC) -o $(TARGET2).out $(TARGET2).cpp
