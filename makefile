main: main.cpp
	g++ main.cpp -o main -lwiringPi -lcurl

clean:
	rm *.o main