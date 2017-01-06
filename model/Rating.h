#pragma once

#include "../config.h"
#include "../Model/Product.h"

using namespace std;
namespace model {
	typedef struct sRating {
	public:
		sRating(PRODUCT_TYPE productId, RATE_TYPE rating) :Rating(rating), ProductId(productId) {}
		sRating() :Rating(0), ProductId(0) {}

		RATE_TYPE Rating;
		PRODUCT_TYPE ProductId;
	} Rating;
}
