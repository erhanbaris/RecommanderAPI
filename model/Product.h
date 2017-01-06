#pragma once

#include "../config.h"
#include <string>

using namespace std;
namespace model {
	typedef struct sProduct {
	public:
		sProduct(size_t productId, string name) :ProductId(productId), Name(name) {}
		sProduct() :ProductId(0), Name("") {}

		PRODUCT_TYPE ProductId;
		string Name;
	} Product;
}