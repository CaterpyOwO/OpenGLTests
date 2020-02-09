//----------------------------------------------------------------------------
/// \file	Frametimer.h
/// \author	Rob Bateman mailto:robthebloke@hotmail.com
///						http://www.robthebloke.org
/// \brief	Basically I'm fed up with debugging code and having the frametimer
///			blow up my code with a 30second time delta (or worse). So, kinda
///			nicked an idea from Game Programming Gems4 to prevent nasty peaks
///			in my timing functions.
///	
/// \note	This code is provided 'as is' with no warrenty of any sort what
///			so ever. If you found any of it useful, then a little e-mail 
///			and/or a mention in the credits would be nice... ;)
/// 
///			Thanks to Dave Hunt & James Whitworth for models and random 
///			comments.... 
///	
//----------------------------------------------------------------------------

#ifndef __PORTABLE_FRAMETIMER_H___
#define __PORTABLE_FRAMETIMER_H___

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

/*!
 *	\brief	This function initialises the frame timer and should be called once 
 *			In your initialisation code.
 */
EXTERN void  InitFrameTimer();

/*!
 *	\brief	This function updates the frametimer.
 */
EXTERN void  SortFrameTimer();

/*!
 *	\brief	This functon will return access to the time taken for the last frame.
 *			This is also bound into LUA if you do not specify -DNO_LUA in the compile 
 *			options
 */
EXTERN float FrameTime();

#endif


