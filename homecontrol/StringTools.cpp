#include <sstream>
#include <algorithm>
#include <vector>

#include "StringTools.h"

namespace StringTools
{
    std::vector<std::string> &SplitStringInStrings(const std::string &s, char delim, std::vector<std::string> &elems)
    {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    std::vector<std::string> SplitString(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        SplitStringInStrings(s, delim, elems);
        return elems;
    }
    
    std::vector<std::string> SplitString(std::string const & s, unsigned int uSizeOfParts)
    {
        std::vector<std::string> splitstring;
        for(unsigned int offset = 0; offset < s.length(); offset += uSizeOfParts)
            splitstring.push_back(s.substr(offset, uSizeOfParts));
        return splitstring;
    }

	std::wstring AsWstring(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}

	std::wstring AsWstring(char* str)
	{
		if (str)
			return AsWstring(std::string(str));
		else
			return L"";
	}

	std::string AsString(const std::wstring& str)
	{
		return std::string(str.begin(), str.end()); //generates warning in vs 2019
	}

	std::wstring FormatWithXDigits(double value, unsigned int digits)
	{
		std::wostringstream stream;
		stream.precision(digits);
		stream << value;
		return stream.str();
	}

	std::wstring FormatWithXDigits(float value, unsigned int digits)
	{
		std::wostringstream stream;
		stream.precision(digits);
		stream << value;
		return stream.str();
	}


    //converts just a string hexvalue into a real hex
    unsigned int GetHexValueFromString(const std::string &s)
    {
        unsigned int x = 0;
        std::stringstream ss;
        ss << std::hex << s;
        ss >> x;
        return x;
    }

    //converts a unsigned int into a string
    std::string GetHexStringFromValue(unsigned int ui)
    {
        std::stringstream sstream;
        sstream << std::hex << ui;
        return sstream.str();
    }
   
    //retrieves a substring from s with token and with a special endmarker
    std::string GetStringByToken(const std::string& s, const std::string& token, const std::string& endmarker)
    {
        //try to find token
        std::size_t found = s.find(token);
        if (found != std::string::npos)
        {
            //go to pos AFTER token
            size_t pos = found + token.length();
            
            //still in range?
            if (pos < s.length())
            {
                //try to find the endmarker AFTER the token
                std::size_t foundendmarker = s.find(endmarker, pos);
                //found?
                if (foundendmarker != std::string::npos && foundendmarker < s.length() && foundendmarker > pos)
                {
                    //return substring from pos AFTER token and first counter of endmarker
                    return s.substr(pos, foundendmarker-pos);
                }
            }
        }
        return (std::string)"";
    }
}
