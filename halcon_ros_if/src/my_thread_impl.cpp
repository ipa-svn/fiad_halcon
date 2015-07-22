// HDevEngine/C++ example implementations for HDevelop's display operators.
//
//   This file contains a simple implementation for single-threaded
//   applications and a more complex thread save implementation
//
// (C) 2005-2014 MVTec Software GmbH
//

#include "my_thread_impl.h"
#include <map>

/*****************************************************************************
 *****************************************************************************
 ** class MyMutexImpl
 **===========================================================================
 ** 
 *****************************************************************************
 *****************************************************************************/

#if defined(WIN32)

#include <process.h>
#include <windows.h>

MyMutexImpl::MyMutexImpl()
{
  InitializeCriticalSection(&mMutex);
}
MyMutexImpl::~MyMutexImpl()
{
  DeleteCriticalSection(&mMutex);
}
void MyMutexImpl::Lock()
{
  EnterCriticalSection(&mMutex);
}
void MyMutexImpl::Unlock()
{
  LeaveCriticalSection(&mMutex);
}

#else

MyMutexImpl::MyMutexImpl()
{
  pthread_mutex_init(&mMutex,NULL);
}
MyMutexImpl::~MyMutexImpl()
{
  pthread_mutex_destroy(&mMutex);
}
void MyMutexImpl::Lock()
{
  pthread_mutex_lock(&mMutex);
}
void MyMutexImpl::Unlock()
{
  pthread_mutex_unlock(&mMutex);
}

#endif




/*****************************************************************************
 *****************************************************************************
 ** class MyThreadImpl
 **===========================================================================
 ** 
 *****************************************************************************
 *****************************************************************************/

// map that is used for attaching the thread handles to the appropriate
// threads
static std::map<Hlong,MyThreadImpl*>  sThreadMap;
static MyMutexImpl                    sThreadMutex;

// handles of all opened windows, the last one is the active one
static MyThreadImpl* GetThreadData()
{
  sThreadMutex.Lock();
  MyThreadImpl* thread_data = sThreadMap[GetThreadId()];
  sThreadMutex.Unlock();
  return thread_data;
}


// add a new thread to the list of managed threads
//  + this thread is created within another thread (e.g., the main thread),
//    therefor the id must be read from the thread data instead of using
//    GetThreadId()
static void AddThreadData(MyThreadImpl* thread_data)
{
  // no mutex here: lock the whole thread creation and list adding 
  sThreadMap[thread_data->mThreadId] = thread_data;
}


// remove a thread from the list of managed threads
//  + this thread is joined within another thread (e.g., the main thread),
//    therefor the id must be read from the thread data instead of using
//    GetThreadId()
static void ClearThreadData(MyThreadImpl* thread_data)
{
  sThreadMutex.Lock();
  sThreadMap.erase(thread_data->mThreadId);
  sThreadMutex.Unlock();
}


#ifdef WIN32

void MyThreadImpl::Start(Hlong id, ThreadFunction* function, void* data)
{
  mId             = id;
  mThreadFunction = function;
  mData           = data;
  unsigned int thread_id;

  // lock the thread mutex from creation of the thread until adding it to the
  // list of managed threads for avoiding that the newly created thread tries
  // to use the thread data before it was added to the list
  sThreadMutex.Lock();
  mThreadHandle = (HANDLE)_beginthreadex(NULL,0,
    (unsigned(__stdcall*)(void*))mThreadFunction,this,0,&thread_id);
  mThreadId = (Hlong)thread_id;
  AddThreadData(this);
  sThreadMutex.Unlock();
}


void MyThreadImpl::Join()
{
  WaitForSingleObject(mThreadHandle,INFINITE);
  ClearThreadData(this);
}


#else


void MyThreadImpl::Start(Hlong id, ThreadFunction* function, void* data)
{
  mId             = id;
  mThreadFunction = function;
  mData           = data;
  // lock the thread mutex from creation of the thread until adding it to the
  // list of managed threads for avoiding that the newly created thread tries
  // to use the thread data before it was added to the list
  sThreadMutex.Lock();
  pthread_create(&mThreadHandle,NULL,(void*(*)(void*))mThreadFunction,this);
  mThreadId       = (Hlong)mThreadHandle;
  AddThreadData(this);
  sThreadMutex.Unlock();
}


void MyThreadImpl::Join()
{
  void* thread_return;
  pthread_join(mThreadHandle,&thread_return);
  ClearThreadData(this);
}

#endif


// handles of all opened windows, the last one is the active one
MyThreadImpl* MyThreadImpl::GetThreadData()
{
  return ::GetThreadData();
}
