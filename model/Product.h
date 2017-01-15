#pragma once

#include <config.h>
#include <string>

using namespace std;
namespace model {
	class Product {
	public:
		Product(PRODUCT_TYPE productId, string name) :ProductId(productId), Name(name) {}
		Product() :ProductId(0), Name("") {}

		PRODUCT_TYPE ProductId;
		string Name;
	};
}