#include <string>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <ignConfig.h>
#include <ignException.h>
#include <ignAssertion.h>
#include <ignStringTools.h>
#include <ignConvertUTF.h>

#ifdef IGN_WITH_CXX11
#include <boost/regex.hpp>
#endif

//using namespace std ;
using namespace ign::tools;

///
///
///
bool StringPattern::Search(const std::string& name, const std::string& pattern)
{
#ifdef IGN_WITH_CXX11
	boost::regex nomenclature(pattern, boost::regex::perl);
	return (boost::regex_search(name, nomenclature));
#else
	return (name.find(pattern) != std::string::npos);
#endif
}

///
///
///
bool StringPattern::Match(const std::string& name, const std::string& pattern, bool caseSensitive)
{
	int verbose = 0;
#ifdef IGN_WITH_CXX11
	if (verbose) std::cout<<"[StringPattern::Match] START with C++11"<<std::endl;
	if (caseSensitive)
	{
		boost::regex nomenclature(pattern, boost::regex::perl);
		if (verbose) std::cout<<"boost::regex_match("<<name<<", "<<nomenclature<<")"<<std::endl;
		return (boost::regex_match(name, nomenclature));
	}
	else 
	{
		boost::regex nomenclature(pattern, boost::regex::perl|boost::regex::icase);
		if (verbose) std::cout<<"boost::regex_match("<<name<<", "<<nomenclature<<") case insensitive"<<std::endl;
		return (boost::regex_match(name, nomenclature));
	}
#else
	std::string newPattern = boost::replace_all_copy(pattern, "*", "");

	if (caseSensitive)
	{
		return (name.find(newPattern) != std::string::npos);
	}
	else
	{
		std::string nameUpper (name);
		StringManip::ToUpperCase(nameUpper);
		std::string patternUpper (newPattern);
		StringManip::ToUpperCase(patternUpper);
		return (nameUpper.find(patternUpper) != std::string::npos);
	}
#endif
}

///
///
///
void StringManip::ToUpperCase(std::string& chaine)
{
	// explicit cast needed to resolve ambiguity
	boost::algorithm::to_upper(chaine);
}


///
///
///
void StringManip::ToLowerCase(std::string& chaine)

{
	// explicit cast needed to resolve ambiguity
	boost::algorithm::to_lower(chaine);
}

std::string StringManip::FromUTF8ToCurrentCodePage(const std::string& filename)
{
	return ign::convertStringFromUTF8toCurrentCodePage(filename);
}

///
///
///
bool StringManip::FindSubString(const std::string& chaine, const std::string&sub)
{
	size_t pos = chaine.find(sub);
	return (pos!=std::string::npos);
}

///
///
///
std::string StringManip::SubstituteSubString(const std::string& chaine, const std::string& sub, const std::string& sub2)
{
	std::ostringstream oss ;
	for ( size_t i = 0; i < chaine.size(); i++ ){
		if ( chaine.substr( i, sub.size() ) == sub ){
			i += sub.size() - 1 ;
			oss << sub2 ;
		}else{
			oss << chaine[i] ;
		}		
	}
	return oss.str() ;	
}

///
///
///
std::string StringManip::GetExtension(std::string const &nom)
{
	int placePoint = -1;
	for(int l=nom.size()-1;(l>=0)&&(placePoint==-1);--l)
	{
		if (nom[l]=='.')
		{
			placePoint = l;
		}
	}
	std::string ext = std::string("");
	if (placePoint!=-1)
	{
		ext.assign(nom.begin()+placePoint,nom.end());
	}
	return ext;
}

///
///
///
void StringManip::Split(const std::string& str, const std::string& delim, std::vector<std::string>& parts)
{
	size_t start, end = 0;
	while (end < str.size()) {
		start = end;
		while (start < str.size() && (delim.find(str[start]) != std::string::npos)) {
			start++;  // skip initial whitespace
		}
		end = start;
		while (end < str.size() && (delim.find(str[end]) == std::string::npos)) {
			end++; // skip to end of word
		}
		if (end-start != 0) {  // just ignore zero-length strings.
			parts.push_back(std::string(str, start, end-start));
		}
	}
}

///
///
///
size_t StringManip::ZeroPadding(const std::string& strNum)
{
	size_t nPadding = 0;
	
	size_t nNum = atoi(strNum.c_str());
	size_t nNumCodage = strNum.length();
	char testNumNoPad[10];
	sprintf(testNumNoPad, "%d", (int)nNum);
	size_t nNumNoPad = std::string(testNumNoPad).length();
	
	if(nNumNoPad!=nNumCodage) // pas de zero padding
		nPadding = nNumCodage;
	
	return nPadding;
}

///
///
///
std::string StringManip::ZeroPadding(const size_t& nNum, const size_t & nZeros)
{
	char format[80];
	if(nZeros>0)
		sprintf(format,"%%0%dd", (int)nZeros);
	else
		sprintf(format,"%%d");
	
	char strpadded[80];
	sprintf(strpadded, format, (int)nNum);
	
	return std::string(strpadded);
}

///
///
///
bool	StringManip::IsNumeric(const std::string &str, double& value)
{
	char* end = 0;
    value = std::strtod(str.c_str(), &end);
	
    return (end != 0 && *end == 0);
}

///
///
///
bool	StringManip::IsInteger(const std::string &str, int& value)
{
	double val;
	if (StringManip::IsNumeric(str, val))
	{
		if (std::strchr(str.c_str(), '.') == 0)
		{
			value = (int)val;
			return true;
		}
	}
	return false;
}

///
///
///
bool invalidChar (signed char c)
{
	return !(c >= 0 && c < 128);
}

///
///
///
void StringManip::StripUnicode(std::string &str)
{
	str.erase(remove_if(str.begin(),str.end(), invalidChar), str.end());
}

///
///
///
void StringManip::NormalizeStringCase(std::string &str, const StringCase &strCase )
{
    switch( strCase)
    {
        case UpperCase:
        {
            ToUpperCase(str);
            break;
        }
        case LowerCase:
        {
            ToLowerCase(str);
            break;

        }
        case CamelCase:
        {
            if (str.empty()) return;

            ToLowerCase(str);
            std::string firstLetter;
            if (str[0] == '.')
            {
                if (str.size() < 2) return;

                firstLetter = str.substr(1, 1);
                ToUpperCase(firstLetter);
                str[1] = firstLetter[0];
            }
            else
            {
                firstLetter = str.substr(0, 1);
                ToUpperCase(firstLetter);
                str[0] = firstLetter[0];
            }
            str[1] = firstLetter[0];
            break;
        }
        default: IGN_ASSERT(false);
    }

}
