#pragma once

#include <istream>
#include <ostream>

unsigned long long streamCopy(std::istream& in,
							  unsigned long long offset,
							  unsigned long long size,
							  unsigned long long bufferSize,
							  std::ostream* out);
