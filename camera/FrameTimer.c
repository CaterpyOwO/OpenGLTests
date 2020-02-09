//----------------------------------------------------------------------------
/// \file	Frametimer.c
/// \author	Rob Bateman mailto:robthebloke@hotmail.com
///						http://www.robthebloke.org
/// \brief	Basically I'm fed up with debugging code and having the frametimer
///			blow up my code with a 30second time delta (or worse). So, kinda
///			nicked an idea from Game Programming Gems4 to prevent nasty peaks
///			in my timing functions.
//----------------------------------------------------------------------------

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>
#endif

static float g_AverageFrametime=0.0001f;
static float g_lastFiveTimes[5] = {0.0001f,0.0001f,0.0001f,0.0001f,0.0001f};
static unsigned char g_idx=0;

/* limit the max frame time update to 4 frames per second */
#define MAX_TIME_DELTA 0.25f


/*
 *	================================================================ Frame timing (WIN32)
 */
#ifdef WIN32
	#include <windows.h>

	float	g_fStartOfFrame_,
			g_fEndOfFrame_  ,
			g_fRateInv_     ;
	__int64 StartClk_       ;

	/*
	 *	Initialise the frame counter
	 */
	void  InitFrameTimer()
	{
		__int64 Rate;
		g_fRateInv_ = 1.0f/(float)CLOCKS_PER_SEC;
		
		if( !QueryPerformanceFrequency( ( LARGE_INTEGER* )&Rate) )
			return;

		if(!Rate)
			return;

		g_fRateInv_=1.0f/(float)Rate;
		if(!QueryPerformanceCounter( ( LARGE_INTEGER* )&StartClk_) )
			return;
		
	}

	/*
	 *	Get the current time
	 */
	float CurrentTime_()
	{
		__int64 EndClk;
		QueryPerformanceCounter(( LARGE_INTEGER* )&EndClk);
		return  ( EndClk - StartClk_ )*g_fRateInv_;
	}



#else	/*============================================= Irix frametiming */

	#include <sys/time.h> /* Linux system clock functions */
	#include <time.h>


	double g_fStartOfFrame_=0.0,g_fEndOfFrame_=0.0;

	double CurrentTime_() /* request frame time from Linux */
	{
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		return (double)tv.tv_sec + (double)tv.tv_usec/(1000*1000);
	}

	void  InitFrameTimer() {
		g_fStartOfFrame_ = CurrentTime_();
	}

#endif


/*
 *	Update frame time
 */
void  SortFrameTimer()
{
	++g_idx;
	g_idx%=0x05;

	do
	{
		g_fEndOfFrame_= CurrentTime_();
	}
	while(g_fEndOfFrame_==g_fStartOfFrame_);

	{
		float f = g_fEndOfFrame_ - g_fStartOfFrame_ ;
		if(f > MAX_TIME_DELTA) {
			f = MAX_TIME_DELTA;
		}
		g_lastFiveTimes[g_idx] = f;
	}
	g_fStartOfFrame_  =  g_fEndOfFrame_ ;

	{
		float *p = g_lastFiveTimes;
		float *e = p+5;

		g_AverageFrametime=0.0f;

		for( ; p != e; ++p )
		{
			g_AverageFrametime += *p;
		}

		g_AverageFrametime *= 0.2f;
	}
}



/*!
*	\brief	This function provides the frame time to the user. This is exactly the
*			same value as the dt parameter for the Update function. Primarily this
*			is provided to pass the value through to LUA if needed!
*/
float FrameTime()
{
	return g_AverageFrametime;
}

