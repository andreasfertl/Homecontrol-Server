#pragma once

//#include <string>
#include <sstream>
#include <vector>


namespace StringTools
{
    std::vector<std::string> SplitString(const std::string &s, char delim);
    std::vector<std::string> SplitString(std::string const & s, unsigned int uSizeOfParts);

    unsigned int GetHexValueFromString(const std::string &s);
    std::string GetHexStringFromValue(unsigned int ui);
    std::string GetStringByToken(const std::string& s, const std::string& token, const std::string& endmarker);
	
	std::wstring AsWstring(const std::string& str);
	std::wstring AsWstring(const std::wstring& str);
	std::string AsString(const std::wstring& str);
	std::string AsString(const std::string& str);

	std::wstring FormatWithXDigits(double value, unsigned int digits);
	std::wstring FormatWithXDigits(float value, unsigned int digits);

    template<typename T>
    T StringToNumber(const std::string& numberAsString)
    {
        T number;
        
        std::stringstream ss(numberAsString);
        ss >> number;
        if (ss.fail())
            return T {};
        else
            return number;
    }
    
    template<typename T>
    T GetNumberByToken(const std::string& s, const std::string& token)
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
                //convert string to number
                return StringToNumber<T>(s.substr(pos, s.size()));
            }
        }
        return T {};
    }
}

