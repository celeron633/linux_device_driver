TARGET := list1_app

$(TARGET): main.o
	gcc $^ -o $@ -g -O0 -static

%.o: %.c
	gcc -c $< -o $@

all: $(TARGET)

clean:
	rm -f *.o \
	rm -f $(TARGET)
