all: threebodyproblem

threebodyproblem: threebodyproblem.o
	c++ -o threebodyproblem threebodyproblem.o -ltrapfpe -lpgplot -lcpgplot -lX11 -lm 

threebodyproblem.o: threebodyproblem.cpp
	c++ -c threebodyproblem.cpp

clean:
	rm -f *.o threebodyproblem