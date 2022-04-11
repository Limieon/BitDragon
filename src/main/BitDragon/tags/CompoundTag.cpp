#include "CompoundTag.hpp"
#include "NumberTag.hpp"
#include "StringTag.hpp"
#include <iostream>
#include <sstream>

namespace bd {
	void CompoundTag::add(Tag& tag) {
		tags[tag.getKey()] = &tag;
	}
	Tag* CompoundTag::get(std::string key) const {
		return tags.at(key);
	}

	void CompoundTag::setUint8(std::string key, uint8 value) {
		tags[key] = new NumberTag(key, value, false, (uint16) 1);
	}
	void CompoundTag::setUint16(std::string key, uint16 value) {
		tags[key] = new NumberTag(key, value, false, (uint16) 2);
	}
	void CompoundTag::setUint32(std::string key, uint32 value) {
		tags[key] = new NumberTag(key, value, false, (uint16) 4);
	}
	void CompoundTag::setUint64(std::string key, uint64 value) {
		tags[key] = new NumberTag(key, value, false, (uint16) 8);
	}
	void CompoundTag::setFloat(std::string key, float value) {
		tags[key] = new NumberTag(key, value, true, (uint16) 4);
	}
	void CompoundTag::setDouble(std::string key, double value) {
		tags[key] = new NumberTag(key, value, true, (uint16) 8);
	}
	void CompoundTag::setBool(std::string key, bool value) {
		tags[key] = new NumberTag(key, value ? 1 : 0, false, (uint16) 1);
	}
	void CompoundTag::setString(std::string key, std::string value) {
		tags[key] = new StringTag(key, value);
	}
	CompoundTag* CompoundTag::createCompound(std::string key) {
		tags.insert(std::pair<std::string, Tag*>(key, (Tag*) new CompoundTag()));
		return (CompoundTag*) tags.at(key);
	}

	ArrayTag<uint8>* CompoundTag::createUint8Array(std::string key) {
		tags.insert(std::pair<std::string, Tag*>(key, (Tag*) new ArrayTag<uint8>(key, 1, false)));
		return (ArrayTag<uint8>*) tags.at(key);
	}
	ArrayTag<uint16>* CompoundTag::createUint16Array(std::string key) {
		tags.insert(std::pair<std::string, Tag*>(key, (Tag*) new ArrayTag<uint16>(key, 2, false)));
		return (ArrayTag<uint16>*) tags.at(key);
	}
	ArrayTag<uint32>* CompoundTag::createUint32Array(std::string key) {
		tags.insert(std::pair<std::string, Tag*>(key, (Tag*) new ArrayTag<uint32>(key, 4, false)));
		return (ArrayTag<uint32>*) tags.at(key);
	}
	ArrayTag<uint64>* CompoundTag::createUint64Array(std::string key) {
		tags.insert(std::pair<std::string, Tag*>(key, (Tag*) new ArrayTag<uint64>(key, 8, false)));
		return (ArrayTag<uint64>*) tags.at(key);
	}
	ArrayTag<float>* CompoundTag::createFloatArray(std::string key) {
		tags.insert(std::pair<std::string, Tag*>(key, (Tag*) new ArrayTag<float>(key, 4, true)));
		return (ArrayTag<float>*) tags.at(key);
	}
	ArrayTag<double>* CompoundTag::createDoubleArray(std::string key) {
		tags.insert(std::pair<std::string, Tag*>(key, (Tag*) new ArrayTag<double>(key, 8, true)));
		return (ArrayTag<double>*) tags.at(key);
	}

	uint8 CompoundTag::getUint8(std::string key) const {
		NumberTag* tag = (NumberTag*) tags.at(key);
		return std::any_cast<uint8>(tag->getValue());
	}
	uint16 CompoundTag::getUint16(std::string key) const {
		NumberTag* tag = (NumberTag*) tags.at(key);
		return std::any_cast<uint16>(tag->getValue());
	}
	uint32 CompoundTag::getUint32(std::string key) const {
		NumberTag* tag = (NumberTag*) tags.at(key);
		return std::any_cast<uint32>(tag->getValue());
	}
	uint64 CompoundTag::getUint64(std::string key) const {
		NumberTag* tag = (NumberTag*) tags.at(key);
		return std::any_cast<uint64>(tag->getValue());
	}
	float CompoundTag::getFloat(std::string key) const {
		NumberTag* tag = (NumberTag*) tags.at(key);
		return std::any_cast<float>(tag->getValue());
	}
	double CompoundTag::getDouble(std::string key) const {
		NumberTag* tag = (NumberTag*) tags.at(key);
		return std::any_cast<double>(tag->getValue());
	}
	bool CompoundTag::getBool(std::string key) const {
		NumberTag* tag = (NumberTag*) tags.at(key);
		return std::any_cast<uint8>(tag->getValue()) == 1;
	}
	std::string CompoundTag::getString(std::string key) const {
		return ((StringTag*) tags.at(key))->getValue();
	}
	CompoundTag& CompoundTag::getCompound(std::string key) const {
		return *(((CompoundTag*) tags.at(key)));
	}

	void CompoundTag::serialize(std::ostream& stream) const {
		std::cout << "Writing new compound Tag!" << std::endl;

		uint8 startID = BD_TAG_ID_CTAG_START;
		uint8 endID = BD_TAG_ID_CTAG_END;
		uint8 null = 0;

		stream.write(reinterpret_cast<const char*>(&startID), sizeof(uint8));

		for(auto it = tags.begin(); it != tags.end(); ++it) {
			std::string key = it->first + ";";
			Tag* tag = it->second;
			uint8 tagID = tag->getID();

			std::cout << "Tag " << key << " (" << ((uint32) tagID) << ")" << std::endl;

			stream.write(reinterpret_cast<const char*>(&tagID), sizeof(uint8));
			stream.write(key.c_str(), key.size());
			if(TagSizeable* tagS = dynamic_cast<TagSizeable*>(tag)) {
				uint16 size = tagS->getSize();
				bool floating = tagS->isFloating();
				stream.write(reinterpret_cast<const char*>(&size), sizeof(uint16));
				
				if(tag->getID() == BD_TAG_ID_NUMBER) {
					NumberTag* tagN =  (NumberTag*) tag;
					std::any number = tagN->getValue();

					switch(size) {
						case 1: {
							uint8 num8 = std::any_cast<uint8>(number);
							stream.write(reinterpret_cast<const char*>(&num8), sizeof(uint8));
							break;
						}
						case 2: {
							uint16 num16 = std::any_cast<uint16>(number);
							stream.write(reinterpret_cast<const char*>(&num16), sizeof(uint16));
							break;
						}
						case 4: {
							if(floating) {
								float num32 = std::any_cast<float>(number);
								stream.write(reinterpret_cast<const char*>(&num32), sizeof(float));
							} else {
								uint32 num32 = std::any_cast<uint32>(number);
								stream.write(reinterpret_cast<const char*>(&num32), sizeof(uint32));
							}
							break;
						}
						case 8: {
							if(floating) {
								double num64 = std::any_cast<double>(number);
								stream.write(reinterpret_cast<const char*>(&num64), sizeof(double));
							} else {
								uint64 num64 = std::any_cast<double>(number);
								stream.write(reinterpret_cast<const char*>(&num64), sizeof(uint64));
							}
							break;
						}
					}
				} else if(tag->getID() == BD_TAG_ID_STRING) {
					std::string value = ((StringTag*) tag)->getValue();
					stream.write(value.c_str(), value.size());
				} else if(tag->getID() == BD_TAG_ID_ARRAY) {
					switch(size) {
						case 1: {
							ArrayTag<uint8>* arr = ((ArrayTag<uint8>*) tag);
							arr->serialize(stream);
							break;
						}
						case 2: {
							ArrayTag<uint16>* arr = ((ArrayTag<uint16>*) tag);
							arr->serialize(stream);
							break;
						}
						case 4: {
							if(floating) {
								ArrayTag<float>* arr = ((ArrayTag<float>*) tag);
								arr->serialize(stream);
							} else {
								ArrayTag<uint32>* arr = ((ArrayTag<uint32>*) tag);
								arr->serialize(stream);
							}
							break;
						}
						case 8: {
							if(floating) {
								ArrayTag<double>* arr = ((ArrayTag<double>*) tag);
								arr->serialize(stream);
							} else {
								ArrayTag<uint64>* arr = ((ArrayTag<uint64>*) tag);
								arr->serialize(stream);
							}
							break;
						}
					}
				}
			}
			if(tag->getID() == BD_TAG_ID_COMPOUND) {
				((CompoundTag*) tag)->serialize(stream);
			}
		}

		stream.write(reinterpret_cast<const char*>(&endID), sizeof(uint8));
	}

	std::string CompoundTag::stringify() const {
		std::stringstream ss;
		ss << "{";

		bool first = true;
		for(auto it = tags.begin(); it != tags.end(); ++it) {
			if(!first) ss << ",";

			std::string key = it->first;
			Tag* tag = it->second;
			uint8 tagID = tag->getID();

			std::cout << "Tag " << key << " (" << ((uint32) tagID) << ")" << std::endl;

			ss << key << ":";
			if(tagID == BD_TAG_ID_NUMBER) {
				NumberTag* tagN = dynamic_cast<NumberTag*>(tag);
				std::any num = tagN->getValue();
				bool floating = tagN->isFloating();
				uint16 size = dynamic_cast<TagSizeable*>(tag)->getSize();

				switch(size) {
					case 1: {
						uint8 num8 = std::any_cast<uint8>(num);
						ss << num8 << BD_TAG_DATA_TYPE_INT8;
						break;
					}
					case 2: {
						uint16 num16 = std::any_cast<uint16>(num);
						ss << num16 << BD_TAG_DATA_TYPE_INT16;
						break;
					}
					case 4: {
						if(floating) {
							float num32 = std::any_cast<float>(num);
							ss << num32 << BD_TAG_DATA_TYPE_FLOAT;
						} else {
							uint32 num32 = std::any_cast<uint32>(num);
							ss << num32 << BD_TAG_DATA_TYPE_INT32;
						}
						break;
					}
					case 8: {
						if(floating) {
							double num64 = std::any_cast<double>(num);
							ss << num64 << BD_TAG_DATA_TYPE_DOUBLE;
						} else {
							uint64 num64 = std::any_cast<uint64>(num);
							ss << num64 << BD_TAG_DATA_TYPE_INT64;
						}
						break;
					}
				}
			} else if(tagID == BD_TAG_ID_STRING) {
				ss << "\"" << ((StringTag*) tag)->getValue() << "\"";
			} else if(tagID == BD_TAG_ID_COMPOUND) {
				ss << ((CompoundTag*) tag)->stringify();
			} else if(tagID == BD_TAG_ID_ARRAY) {
				TagSizeable* tagS = dynamic_cast<TagSizeable*>(tag);
				uint16 size = tagS->getSize();
				bool floating = tagS->isFloating();

				switch(size) {
					case 1: {
						((ArrayTag<uint8>*) tag)->stringify(ss);
						break;
					}
					case 2: {
						((ArrayTag<uint16>*) tag)->stringify(ss);
						break;
					}
					case 4: {
						if(floating) {
							((ArrayTag<float>*) tag)->stringify(ss);
						} else {
							((ArrayTag<uint32>*) tag)->stringify(ss);
						}
						break;
					}
					case 8: {
						if(floating) {
							((ArrayTag<double>*) tag)->stringify(ss);
						} else {
							((ArrayTag<uint64>*) tag)->stringify(ss);
						}
						break;
					}
				}
			}

			first = false;
		}

		ss << "}";

		return ss.str();
	}

	void CompoundTag::deSerialize(std::istream& stream) {
	}
}