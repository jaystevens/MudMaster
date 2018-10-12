/**************************************************************************
*  Copyright (c) 1997-2004, Kevin Cook and Aaron O'Neil
*  All rights reserved.

*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions are 
*  met:
*  
*      * Redistributions of source code must retain the above copyright 
*        notice, this list of conditions and the following disclaimer. 
*      * Redistributions in binary form must reproduce the above copyright 
*        notice, this list of conditions and the following disclaimer in 
*        the documentation and/or other materials provided with the 
*        distribution. 
*      * Neither the name of the MMGUI Project nor the names of its 
*        contributors may be used to endorse or promote products derived 
*        from this software without specific prior written permission. 
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
*  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
*  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
*  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
*  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
*  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
*  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#pragma once

namespace Utilities
{
	static LPCTSTR WHITESPACE = _T(" \n\t\r");

	inline std::string trim_right(const std::string &source, const std::string &t = _T(" "))
	{
		std::string str = source;
		return str.erase(str.find_last_not_of(t) + 1);
	}

	inline std::string trim_left(const std::string &source, const std::string &t = _T(" "))
	{
		std::string str = source;
		return str.erase(0, str.find_first_not_of(t));
	}

	inline std::string trim(const std::string &source, const std::string &t = _T(" "))
	{
		std::string str = source;
		return trim_left(trim_right(str, t), t);
	}

	inline std::string right_string(const std::string &source, int num_chars)
	{
		return source.substr(source.length() - num_chars);
	}

	inline std::string left_string(const std::string &source, int num_chars)
	{
		return source.substr(0, num_chars);
	}

	class StringUtils
	{
	public:
		static HRESULT LoadString(UINT stringId, std::string &string);
		static void LowerString(const std::string &in, std::string &out);

		template<class OutIt>
		static void split( const std::string& s, const std::string& sep, OutIt dest ) 
		{
			std::string::size_type left = s.find_first_not_of( sep );
			std::string::size_type right = s.find_first_of( sep, left );
			while( left < right ) 
			{
				*dest = s.substr( left, right-left );
				++dest;
				left = s.find_first_not_of( sep, right );
				right = s.find_first_of( sep, left );
			}
		}

	private:
		StringUtils(void);
		~StringUtils(void);
		StringUtils(const StringUtils &);
		StringUtils &operator=(const StringUtils &);
	};
}