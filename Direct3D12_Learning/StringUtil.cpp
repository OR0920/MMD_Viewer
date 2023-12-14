#include"StringUtil.h"

#include"System.h"

void newArray_GetExtention(char** extPtr, const char* const filename)
{
	int length;
	int dotId = 0;
	for (length = 0; filename[length] != '\0'; ++length)
	{
		auto& c = filename[length];
		if (c == '.')
		{
			dotId = length;
		}
	}
	int extLength = length - dotId;//ƒhƒbƒg‚Í”²‚­
	*extPtr = new char[extLength] {};

	for (int i = extLength; i > 0; --i)
	{
		(*extPtr)[i - 1] = filename[dotId + i];
	}
}



// prim*secondary0 14 + 1 
// * 4
// prim0 5
// secondary0 10
bool newArray_SplitFileName(char** primaryTex, char** secondaryTex, const char* const filename)
{
	int splitID = -1;
	int length;
	for (length = 0; true; ++length)
	{
		auto& c = filename[length];
		if (c == '\0')
		{
			length += 1;
			break;
		}
		if (c == '*')
		{
			splitID = length;
		}
	}
	if (splitID == -1)
	{
		return false;
	}
	if (primaryTex == nullptr || secondaryTex == nullptr)
	{
		return true;
	}
	int primaryPathLength = splitID + 1;// * ‚ðNULL•¶Žš‚É•ÏŠ·‚·‚é‚½‚ß
	int secondaryPathLength = length - primaryPathLength;

	*primaryTex = new char[primaryPathLength] {'\0'};
	*secondaryTex = new char[secondaryPathLength] {'\0'};

	for (int i = 0; true; ++i)
	{
		if (i < (primaryPathLength - 1))
		{
			(*primaryTex)[i] = filename[i];
		}
		if (i < (secondaryPathLength - 1))
		{
			(*secondaryTex)[i] = filename[primaryPathLength + i];
		}
		if (i > primaryPathLength && i > secondaryPathLength)
		{
			break;
		}
	}

	return true;
}
