// GameTimer.cpp
// Shell engine version 2020
// Chris Rook
// Last modified 20/09/2018

#include "GameTimer.h"

GameTimer::GameTimer()
{
	if(QueryPerformanceFrequency(&freq)==0)		// Find the timer frequency
	{
		freq.QuadPart=0;									// Set to zero if function failed

	}

	mdGameRate = 1.0;                   // Can adjust this to make the game faster/slower
                                        // Trivial to add functions that let the programmer adjust this.
                                        // I just haven't done it.
	mdMinimumFrameTime=0;
	mdFrameTime = 0;
}

// Use to set the frameTime. Call this once each frame. 
// The function will also delay until the minumum frame time
// has elapsed since the last call to mark.
// Unless the function "setMinimumFrameTime()" has been called
// the minimum time will be zero, in which case, the mark() function will
// not delay.
// Note that
// the frame time is based on the interval between the last two
// calls to mark(). Until this function has been called twice, it
// will be unreliable.
void GameTimer::mark()
{

	if (freq.QuadPart<=0)		// If frequency is zero (if QueryPerformanceCounter failed)
	{
		last.QuadPart=0;			// Set everything to zero. Something is badly wrong.
		mdFrameTime=0;
	}
	else
	{
		LARGE_INTEGER now;
		mdFrameTime=0.0;						      // Set to zero, ready for loop
		while(mdFrameTime<=mdMinimumFrameTime)	// This is a loop that causes a delay until minimum frame time has elapsed
                                             // Using "sleep" is just not accurate enough
		{

			QueryPerformanceCounter(&now);		// Get current time

			mdFrameTime=(now.QuadPart - last.QuadPart)/(double)freq.QuadPart;
											// Find time elapsed since last mark, and divide by
											// frequency to convert to seconds
		}

		mdFrameTime = mdFrameTime * mdGameRate;
		last=now;						// Update mark time with current time

	}
}


// Sets the minimum frame time (in seconds). The mark() function will delay until
// the minimum time has elapsed since the last call to mark().
// If not set, the minimum time is zero.
void GameTimer::setMinimumFrameTime(double minTime)
{
	if(minTime>0.0)      // Can't have a negative minimum time
	{
		mdMinimumFrameTime = minTime;
	}
	else 
		mdMinimumFrameTime=0.0;
}
