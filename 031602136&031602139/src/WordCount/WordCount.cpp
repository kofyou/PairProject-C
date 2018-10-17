
#include "pch.h"

int main(int argc, char *argv[])
{
	counter *myCounter = new counter;
	myCounter->initParameter(argc, argv);
	
	//method one
	//myCounter->countCharLine();
	//myCounter->countWord();

	//method two
	myCounter->countAll();

	myCounter->frequency();

	myCounter->print();

	delete myCounter;

	return 0;
}

