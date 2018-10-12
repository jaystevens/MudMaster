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
#include <afxtempl.h>

namespace MMCollections
{
	template<class TYPE, class ARG_TYPE=const TYPE&>
	class CSortedArray 
		: public CArray<TYPE, ARG_TYPE>
	{
	public:
		//Constructors / Destructors
		CSortedArray();

		//Typedefs
		typedef int COMPARE_FUNCTION(ARG_TYPE element1, ARG_TYPE element2); 
		typedef COMPARE_FUNCTION* LPCOMPARE_FUNCTION;

		//Methods
		int  OrderedInsert(ARG_TYPE newElement, int nCount=1);
		void Sort(int nLowIndex=0, int nHighIndex=-1);
		int  Find(ARG_TYPE element, int nLowIndex=0, int nHighIndex=-1);
		void SetCompareFunction(LPCOMPARE_FUNCTION lpfnCompareFunction) { m_lpfnCompareFunction = lpfnCompareFunction; };
		LPCOMPARE_FUNCTION GetCompareFunction() const { return m_lpfnCompareFunction; };

	protected:
		LPCOMPARE_FUNCTION m_lpfnCompareFunction;
		void swap(ARG_TYPE element1, ARG_TYPE element2);
	};

	template<class TYPE, class ARG_TYPE>
		CSortedArray<TYPE, ARG_TYPE>::CSortedArray()
	{
		m_lpfnCompareFunction = NULL; 
	}

	template<class TYPE, class ARG_TYPE>
		void CSortedArray<TYPE, ARG_TYPE>::swap(ARG_TYPE element1, ARG_TYPE element2)
	{
		TYPE temp = element1;
		element1 = element2;
		element2 = temp;
	}

	template<class TYPE, class ARG_TYPE>
		int CSortedArray<TYPE, ARG_TYPE>::OrderedInsert(ARG_TYPE newElement, int nCount)
	{
		ASSERT(m_lpfnCompareFunction != NULL); //Did you forget to call SetCompareFunction prior to calling this function

		if(GetUpperBound() == -1)
		{
			InsertAt(0, newElement, nCount);
			return 0;
		}

		for(int i = 0; i < GetSize(); ++i)
		{
			TYPE &v = GetAt(i);
			int nVal = m_lpfnCompareFunction(v, newElement);
			if(nVal == 1 || nVal == 0)
			{
				InsertAt(i, newElement, nCount);
				return i;
			}
		}

		INT_PTR nIndex = GetSize();
		InsertAt(nIndex, newElement, nCount);
		return nIndex;
	}

	template<class TYPE, class ARG_TYPE>
		int CSortedArray<TYPE, ARG_TYPE>::Find(ARG_TYPE element, int nLowIndex, int nHighIndex)
	{
		ASSERT(m_lpfnCompareFunction != NULL); //Did you forget to call SetCompareFunction prior to calling this function

		//If there are no items in the array, then return immediately
		if (GetSize() == 0)
			return -1;

		INT_PTR left = nLowIndex;
		INT_PTR right = nHighIndex;
		if (right == -1)
			right = GetUpperBound();
		ASSERT(left <= right);

		if (left == right) //Straight comparision fewer than 2 elements to search
		{
			BOOL bFound = (m_lpfnCompareFunction(ElementAt(left), element) == 0);
			if (bFound)
				return left;
			else
				return -1;
		}

		//do a binary chop to find the location where the element should be inserted
		int nFoundIndex = -1;
		while (nFoundIndex == -1 && left != right)
		{
			int nCompareIndex;
			if (right == (left+2))
				nCompareIndex = left+1;
			else
				nCompareIndex = ((right - left)/2) + left;

			int nCompare = m_lpfnCompareFunction(ElementAt(nCompareIndex), element);
			switch (nCompare)
			{
			case -1:
				{
					if ((right - left) == 1)
					{
						if (m_lpfnCompareFunction(ElementAt(right), element) == 0)
							nFoundIndex = right;
						else if (m_lpfnCompareFunction(ElementAt(left), element) == 0)
							nFoundIndex = left;
						else
							left = right;
					}
					else
						left = nCompareIndex;
					break;
				}
			case 0:
				{
					nFoundIndex = nCompareIndex;
					break;
				}
			case 1:
				{
					if ((right - left) == 1)
					{
						if (m_lpfnCompareFunction(ElementAt(right), element) == 0)
							nFoundIndex = right;
						else if (m_lpfnCompareFunction(ElementAt(left), element) == 0)
							nFoundIndex = left;
						else
							right = left;
					}
					else
						right = nCompareIndex;
					break;
				}
			default:
				{
					ASSERT(FALSE); //Your compare function has been coded incorrectly. It should
					//return -1, 0 or 1 similiar to the way the C Runtime function
					//"qsort" works
					break;
				}
			}
		}

		return nFoundIndex;
	}

	template<class TYPE, class ARG_TYPE>
		void CSortedArray<TYPE, ARG_TYPE>::Sort(int nLowIndex, int nHighIndex)
	{
		ASSERT(m_lpfnCompareFunction != NULL); //Did you forget to call SetCompareFunction prior to calling this function

		//If there are no items in the array, then return immediately
		if (GetSize() == 0)
			return;

		int left = nLowIndex;
		int right = nHighIndex;
		if (right == -1)
			right = GetUpperBound();

		if (right-left <= 0) //Do nothing if fewer than 2 elements are to be sorted
			return;

		// (not really needed, just to optimize)
		if (right-left == 1)  //Do a simple comparison if only 2 elements
		{
			if (m_lpfnCompareFunction(ElementAt(right), ElementAt(left)) == -1)
				swap(ElementAt(left), ElementAt(right));
			return;
		}

		swap(ElementAt(left), ElementAt((left+right)/2));      //move partition element to begining 
		int last = left;

		for (int i=left+1; i<=right; i++) //Partition
		{
			if (m_lpfnCompareFunction(ElementAt(i), ElementAt(left)) == -1)
				swap(ElementAt(++last), ElementAt(i));
		}

		swap(ElementAt(left), ElementAt(last)); //Restore partition element
		Sort(left, last-1);
		Sort(last+1, right);  
	}
}