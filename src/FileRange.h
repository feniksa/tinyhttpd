#pragma once

#include <cassert>
#include <string>

// struct to define range of bytes: [begin, end]
class FileRange
{
public:
	void begin(unsigned long long __begin) noexcept { _begin = __begin; }
	void end(unsigned long long __end) noexcept { _end = __end; }

	unsigned long long begin() const { assert(_begin < _fileSize); return _begin; }
	unsigned long long end() const { assert(_end < _fileSize); return _end; }
	unsigned long long fileSize() const noexcept { return _fileSize; }

	FileRange(unsigned long begin = 0, unsigned long end = 0, unsigned long fileSize = 0)
	: _begin(begin),
	  _end(end),
	  _fileSize(fileSize)
	{
	}

	unsigned long long length() const
	{
		assert(_end >= _begin);

		return _end - _begin + 1;
	}

	bool validate() const noexcept
	{
		return (_begin <= _end && _begin < _fileSize && _end < _fileSize);
	}

private:
	unsigned long long _begin;
	unsigned long long _end;
	unsigned long long _fileSize;
};

std::string to_string(const FileRange&);
