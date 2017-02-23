#include "Index.h"
#include <core/server/AppServer.h>

using namespace core;
using namespace core::data;

Index::Index() {
	this->file = NULL;
}

Index::~Index()
{
	if (this->file != NULL)
	{
		this->file->close();
		delete this->file;
	}
}

void Index::SetFileName(string fileName) {
	this->fileName = fileName;
}

void Index::SetDictionary(CUSTOM_MAP<size_t, dictionaryItemContainer> * dict)
{
	this->dictionary = dict;
}

bool Index::Open() {
	try {
		this->file = new std::fstream();
		this->file->open(core::server::AppServer::instance().GetDataPath() + "/" + fileName, ios::binary);
	}
	catch (std::exception &e)
	{
		ERROR_WRITE(STR("Index file not opened. File name: ") << GET_STRING(this->fileName) << STR(". Error message: ") << GET_STRING(e.what()));
		return false;
	}

	return true;
}

bool Index::Close()
{
	try {
		if (this->file != NULL)
		{
			this->file->close();
			delete this->file;
		}
	}
	catch (std::exception &e)
	{
		ERROR_WRITE(STR("Index file not closed. File name: ") << GET_STRING(this->fileName) << STR(". Error message: ") << GET_STRING(e.what()));
		return false;
	}

	return true;
}

bool Index::Create()
{
	try {
		LOG_WRITE(STR("Index File Creating Started"));
		ofstream outfile;
		outfile.open(fileName, ios::binary);

		outfile << 'E';
		outfile << 'D';
		outfile << 'B';
		outfile << '1';
		outfile << '.';
		outfile << '0';
		outfile << (0xFF & (sizeof(size_t)));
		outfile << '\0';
		//outfile.write((char*)core::intToBits(dictionary->size()), sizeof(dictionary->size()));
		outfile << '\0';
		outfile << '\0';

		auto dictEnd = dictionary->end();
		for (auto it = dictionary->begin(); it != dictEnd; ++it) {
			//char * byteArray = core::intToBits(it->first);
			//outfile.write((char*)byteArray, sizeof(size_t));
		}

		outfile.close();


		ifstream infile;
		infile.open(fileName, ios::binary);
		infile.seekg(6);

		char * byteArray = new char[sizeof(size_t)];

		infile.read((char*)byteArray, sizeof(size_t));
		unsigned long int anotherLongInt;

//		anotherLongInt = core::bitsToInt<unsigned long int>(byteArray);
		infile.seekg(8);
//		infile.read((char*)byteArray, sizeof(size_t));
//		anotherLongInt = core::bitsToInt<unsigned long int>(byteArray);

		infile.close();

		LOG_WRITE(STR("Index File Creating Finished"));
	}
	catch (std::exception &e)
	{
		ERROR_WRITE(STR("Index file not created. File name: ") << GET_STRING(this->fileName) << STR(". Error message: ") << GET_STRING(e.what()));
		return false;
	}
	return true;
}

bool Index::IsOpen()
{
	return this->file != NULL && this->file->is_open();
}