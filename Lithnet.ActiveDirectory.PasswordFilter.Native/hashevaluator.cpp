#include "stdafx.h"
#include "hasher.h"
#include <fstream>
#include "registry.h"
#include "hashevaluator.h"
#include "shlwapi.h"

const int STORE_VERSION = 1;

bool IsHashInStore(std::wstring hash)
{
	std::wstring range = hash.substr(0, 5);
	return IsHashInStore(hash, range);
}

bool IsHashInStore(std::wstring hash, std::wstring range)
{
	std::wstring path = GetStoreFileName(range);

	DWORD attr = GetFileAttributes(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
	{
		return false;
	}

	return IsHashInFileBS(path, hash);
}

std::wstring GetStoreFileName(std::wstring range)
{
	std::wstring path = GetRegValue(L"Store", L"");

	if (path == L"")
	{
		throw new std::invalid_argument("No store path was specified");
	}

	if (!path.empty() && *path.rbegin() != '\\')
		path += '\\';

	path += L"SHA1\\";
	path += range;
	path += L".txt";

	return path;
}

std::wstring GetStoreFileNamev1(std::wstring range)
{
	std::wstring path = GetRegValue(L"Store", L"");

	if (path == L"")
	{
		throw new std::invalid_argument("No store path was specified");
	}

	if (!path.empty() && *path.rbegin() != '\\')
		path += '\\';

	path += L"v1\\";
	path += range;
	path += L".db";

	return path;
}


bool IsHashInFile(std::wstring filename, std::wstring hash)
{
	return false;
	std::wifstream file(filename.c_str());
	std::wstring line;

	while (std::getline(file, line))
	{
		if (line.compare(hash) == 0)
		{
			return true;
		}
	}

	return false;
}

bool hasBOM(std::ifstream & is)
{
	/* Read the first byte. */
	char const c0 = is.get();
	if (c0 != '\xEF') {
		is.putback(c0);
		return false;
	}

	/* Read the second byte. */
	char const c1 = is.get();
	if (c1 != '\xBB') {
		is.putback(c1);
		is.putback(c0);
		return false;
	}

	/* Peek the third byte. */
	char const c2 = is.peek();
	if (c2 != '\xBF') {
		is.putback(c1);
		is.putback(c0);
		return false;
	}

	return true; // This file contains a BOM for UTF-8.
}

bool IsHashInFileBS(std::wstring filename, std::wstring hash)
{
	char hashVal[SHA1_HASH_LENGTH + 1];

	wcstombs(hashVal, hash.c_str(), SHA1_HASH_LENGTH + 1);

	std::ifstream file(filename.c_str(), std::ios::binary | std::ios::in);

	int headerCount = 0;
	if (hasBOM(file))
	{
		headerCount = 3;
	}

	std::string search;
	int firstRow = 0, currentRow = 0, lastRow = 0, length = 0;
	bool found = false;

	file.seekg(0, std::ios::end);
	length = file.tellg();

	file.seekg(0, std::ios::beg);

	lastRow = ((length - headerCount) / (SHA1_HASH_LENGTH + ROW_TERMINATOR_LENGTH));

	while (firstRow <= lastRow)
	{
		currentRow = (firstRow + lastRow + 1) / 2;
		file.seekg(headerCount + (currentRow * (SHA1_HASH_LENGTH + ROW_TERMINATOR_LENGTH)), std::ios::beg);

		char rowData[SHA1_HASH_LENGTH];

		file.read(rowData, SHA1_HASH_LENGTH);

		search = std::string(rowData, SHA1_HASH_LENGTH);

		if (search < hashVal)
		{
			firstRow = currentRow + 1;
		}
		else if (search > hashVal)
		{
			lastRow = currentRow - 1;
		}
		else
		{
			return true;
		}
	}

	return false;
}