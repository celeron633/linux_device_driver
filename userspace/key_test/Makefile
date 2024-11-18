TARGET := key_test

$(TARGET): main.o
	gcc $^ -o $@ 

%.o: %.c
	gcc -c $< -o $@

all: $(TARGET)

clean:
	rm -f *.o \
	rm -f $(TARGET)
