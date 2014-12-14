#pragma once
#include <Windows.h>

class iImage {
	int _width,_height;
	unsigned long *clrArray;
public:
	//Create an image object having the specified width and height
	iImage(int width, int height) {
		_width = width;
		_height = height;
		clrArray = new unsigned long[_width*_height];
	}

	//Delete the image
	~iImage() {
		delete [] clrArray;
	}

	//Set the color for a specific pixel
	void setClr(int x,int y,unsigned long Clr) {
		clrArray[y*_width + x] = Clr;
	}

	//Get the color of a specific pixel
	unsigned long getClr(int x,int y) {
		return clrArray[y*_width + x];
	}

	//Get the entire color array. Returns the internal array if copy parameter is false.
	//Programmer is supposed to delete the array if copied.
	unsigned long *getClrArray(bool copy = true) {
		if (copy) {
			unsigned long *newArray = new unsigned long[_width*_height];
			memcpy(newArray,clrArray,_width*_height);
			return newArray;
		}
		else return clrArray;
	}

	//Set the values for the internal array. If copy is set to false, the internal array is deleted and the new array is used as reference.
	void setClrArray(unsigned long *ClrArray, bool copy = true) {
		if (copy)
			memcpy(clrArray,ClrArray,_width*_height);
		else {
			delete []clrArray;
			clrArray = ClrArray;
		}
	}
};