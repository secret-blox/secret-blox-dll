#pragma once
#include <Windows.h>
#include "address.h"
#include "iostream"

typedef int(__cdecl* RPrint)(int, const char*, ...);
typedef RPrint RPrintFunc;
RPrintFunc r_Print = reinterpret_cast<RPrintFunc>(moduleBase(Addresses::print_Address));


void rbxPrint(std::string type, const char* message) {
	int messageType = 0;

	if (type == "Error") {
		messageType = 3;
	}

	if (type == "Warning") {
		messageType = 2;
	}

	if (type == "Info") {
		messageType = 1;
	}

	if (type == "Normal") {
		messageType = 0; 
	}

	r_Print(messageType, message);
}