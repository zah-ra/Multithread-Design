CC = g++ -std=c++11

all: HousePricePrediction.out

HousePricePrediction.out: main.o
	${CC} main.o -o HousePricePrediction.out -pthread

main.o: main.cpp
	${CC} -c main.cpp -pthread

.PHONY: clean
clean:
	rm *.o
	rm *.out