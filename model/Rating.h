#pragma once

#include <config.h>
#include <model/Product.h>

using namespace std;
namespace model {
	class Rating {
	public:
		Rating(PRODUCT_TYPE productId, RATE_TYPE rate) :Rate(rate), ProductId(productId) {}
		Rating() :Rate(0), ProductId(0) {}

		RATE_TYPE Rate;
		PRODUCT_TYPE ProductId;
	} ;
}
