#include "HttpHeaderParse.h"

#include <Poco/RegularExpression.h>
#include <cassert>

namespace parse
{

bool bytesRange(const std::string& str, FileRange* range)
{
	assert(range);

	if (str.empty())
		return true;

	Poco::RegularExpression expression("^bytes=([0-9]*)-([0-9]*)$");

	using match_iter = Poco::RegularExpression::MatchVec::const_iterator;
	Poco::RegularExpression::MatchVec matches;

	int num = expression.match(str, 0, matches, 0);
	if (num != 3)
		return false;

	if (matches[1].length > 0 && matches[2].length > 0) {
		std::string a(&str[matches[1].offset], matches[1].length);
		range->begin(std::stoull(a));

		std::string b(&str[matches[2].offset], matches[2].length);
		range->end(std::stoull(b));

		return true;
	}

	if (matches[1].length > 0) {
		std::string s(&str[matches[1].offset], matches[1].length);
		range->begin(std::stoull(s));

		return true;
	} else /*if (matches[2].length > 0) */ {
		std::string s(&str[matches[2].offset], matches[2].length);
		unsigned long long r = std::stoull(s);

		if (r > range->end()) {
			return false;
		}

		range->begin(range->end() - r + 1);
		return true;
	}
}

}
