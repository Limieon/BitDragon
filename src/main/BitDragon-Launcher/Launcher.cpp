#include <iostream>
#include "BitDragon/Defines.hpp"
#include "BitDragon/BitDragon.hpp"

int main(int argc, char** argv) {
	bd::CompoundTag tag;
	tag.setUint16("int", 154);
	tag.setString("string", "String");
	tag.setString("cstring", "CString");
	tag.setFloat("float", 95.732);

	bd::CompoundTag* inner = tag.createCompound("inner");
	inner->setString("tag", "This is inner Tag!");
	
	bd::ArrayTag<float>* values = inner->createFloatArray("values");
	values->add(95.732f);

	bd::ArrayTag<uint32>* arr = tag.createUint32Array("arr");
	arr->add(1);
	arr->add(2);
	arr->add(3);
	arr->add(95);

	bd::BitDragon::serialize(tag, "./test.dat");
	std::cout << tag.stringify() << std::endl;
}