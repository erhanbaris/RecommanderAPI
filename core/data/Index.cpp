#include "Index.h"

using namespace core::data;

Index::Index() {}

void Index::SetFileName(string fileName) {
    this->fileName = fileName;
}

bool Index::Open() {
    return true;
}

bool Index::Create()
{
    return true;
}