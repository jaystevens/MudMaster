/********************************************************************************
	Copyright (C) 2004-2005 Sjaak Priester	

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Tinter; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************************/

// QPerformanceTimer
// Class to measure performance time
//
// Usage: simply define a QPerformanceTimer variable in a C++ block.
// At construction, it measures the start time.
// At destruction, it measures the stop time. It calculates the difference
// and puts the result (in microseconds) in the associated int.
// In other words: this class measures and records its own lifetime.
// If the output is set to -1, it's an indication of overflow, but this
// will only occur after more than half an hour.
//
// Example:
//
//
//	SomeFunction()
//	{
//		... some code...
//
//		int elapsedTime;
//		{	// start of the block we want to monitor
//			QPerformanceTimer(elapsedTime);
//
//			... some lengthy process...
//
//		}	// end of block, elapsed time is recorded in elapsedTime
//
//		printf("Time = %d microseconds", elapsedTime);
//	}
//
// Version 1.1 (C) 2004-2005, Sjaak Priester, Amsterdam.
// mailto:sjaak@sjaakpriester.nl
// Modified by K Werk 2008
// designed to receive string from call to identify it 
// and changed so output is sent to debug output
// so input pointer to int removed

#pragma once

class QPerformanceTimer
{
public:
	QPerformanceTimer(CString strCaller)
		: strOutput(strCaller)
	{
		::QueryPerformanceCounter(&m_Start);
	}

	~QPerformanceTimer(void)
	{
		//if (m_Start.QuadPart == 0)
		//{
		//	m_Output = -1;		// system doesn't support performance counter
		//	return;
		//}

		LARGE_INTEGER stop;
		LARGE_INTEGER freq;
		
		::QueryPerformanceCounter(&stop);
		::QueryPerformanceFrequency(&freq);

		stop.QuadPart -= m_Start.QuadPart;
		stop.QuadPart *= 10000; // make it ms 000;
		stop.QuadPart /= freq.QuadPart;

		strOutput.AppendFormat(" elapsed = %u tenths of ms\n",stop.LowPart);
		::OutputDebugString(strOutput);

		//if (stop.HighPart != 0) m_Output = -1;
		//else m_Output = stop.LowPart;
	}
protected:
	LARGE_INTEGER m_Start;
//	int& m_Output;
	CString strOutput;
};
