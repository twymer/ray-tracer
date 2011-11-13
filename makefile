#Performance running
BITTINESS = -m32
GXX = g++ ${BITTINESS}
CFLAGS=-O3

#debug
#CFLAGS=-g

#profiling
#CFLAGS=-pg

lab6: main.o vector.o parser.o matrix.o sphere.o lights.o cube.o triangle.o
	$(GXX) $(CFLAGS) -o lab6 main.o vector.o parser.o matrix.o sphere.o lights.o cube.o triangle.o -lm

main.o: main.cpp
	$(GXX) $(CFLAGS) -c -o main.o main.cpp

vector.o: vector.cpp
	$(GXX) $(CFLAGS) -c -o vector.o vector.cpp

parser.o: parser.cpp
	$(GXX) $(CFLAGS) -c -o parser.o parser.cpp

matrix.o: matrix.cpp
	$(GXX) $(CFLAGS) -c -o matrix.o matrix.cpp

sphere.o: sphere.cpp
	$(GXX) $(CFLAGS) -c -o sphere.o sphere.cpp

lights.o: lights.cpp
	$(GXX) $(CFLAGS) -c -o lights.o lights.cpp

cube.o: cube.cpp
	$(GXX) $(CFLAGS) -c -o cube.o cube.cpp

triangle.o: triangle.cpp
	$(GXX) $(CFLAGS) -c -o triangle.o triangle.cpp

clean:
	rm -f main.o vector.o parser.o matrix.o sphere.o lights.o cube.o triangle.o lab6 debug
