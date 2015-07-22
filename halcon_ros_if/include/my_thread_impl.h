// HDevEngine/C++ example implementation for portable thread handling
//
// (C) 2005-2014 MVTec Software GmbH
//


#ifndef MY_THREAD_IMPL_H
#define MY_THREAD_IMPL_H

#ifdef __APPLE__
#include <HALCONCpp/HalconCpp.h>
#else
#include "halconcpp/HalconCpp.h"
#endif

//
// we need a mutex for synchronizing the access to the 
#if defined(WIN32)

// disable warning C4786: symbol greater than 255 character,
#pragma warning(disable : 4786)

#include <windows.h>
#include <process.h>    /* _beginthread, _endthread */

typedef CRITICAL_SECTION MyMutexType;
typedef HANDLE           MyThreadHandleType;

inline Hlong GetThreadId()
{
  return Hlong(GetCurrentThreadId());
}

#else

#include "pthread.h"

typedef pthread_mutex_t MyMutexType;
typedef pthread_t       MyThreadHandleType;

inline Hlong GetThreadId()
{
  return Hlong(pthread_self());
}

#endif





/*****************************************************************************
 *****************************************************************************
 ** class MyMutexImpl
 **===========================================================================
 ** 
 *****************************************************************************
 *****************************************************************************/
class MyMutexImpl
{
public:

  MyMutexImpl();
  virtual ~MyMutexImpl();
  void Lock();
  void Unlock();

protected:

  MyMutexType mMutex;
};



/*****************************************************************************
 *****************************************************************************
 ** class MyThreadImpl
 **===========================================================================
 ** 
 *****************************************************************************
 *****************************************************************************/
class MyThreadImpl
{
  typedef Herror (ThreadFunction)(void*);

public:

  MyThreadImpl() : mThreadId(0), mId(0), mThreadFunction(0), mData(0) {}
  void Start(Hlong id, ThreadFunction* function, void* data);
  void Join();

  // returns the thread instance for the currently active thread
  static MyThreadImpl* GetThreadData();

  MyThreadHandleType   mThreadHandle;
  Hlong                mThreadId;
  Hlong                mId;
  ThreadFunction*      mThreadFunction;
  void*                mData;
};




#endif // #ifndef MY_THREAD_IMPL_H
