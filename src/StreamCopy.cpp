#include "StreamCopy.h"
#include <cassert>
#include <vector>

unsigned long long streamCopy(std::istream& in, unsigned long long offset, unsigned long long size, unsigned long long bufferSize, std::ostream* out)
{
	assert(bufferSize > 0);
	assert(size > 0);

	std::vector<char> buffer(bufferSize);
	unsigned long long len = 0;

	in.seekg(offset);

	std::streamsize readed;
	while (in && *out) {
		in.read(buffer.data(), std::min(size, bufferSize));
		readed = in.gcount();

		if (readed == 0)
			break;

		out->write(buffer.data(), readed);

		len += readed;
		size -= readed;
	}

	return len;
}
