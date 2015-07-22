// HDevEngine/C++ example implementations for HDevelop's display operators.
//
//   This file contains a simple implementation for single-threaded
//   applications and a more complex thread save implementation
//
// (C) 2007-2014 MVTec Software GmbH
//

#include "my_hdevoperatorimpl.h"


#define HCkDev(DEV_OP)        \
try                           \
{                             \
  DEV_OP;                     \
  return H_MSG_TRUE;          \
}                             \
catch (HException& exc)       \
{                             \
  return exc.ErrorNumber();   \
}                            


using namespace HalconCpp;

/*****************************************************************************
 * MyHDevOperatorImpl
 *****************************************************************************
 * This is a simple implementation of the HDevelop's display operators.
 * Attention: This class must not be used in a multi-threaded application
 ******************************************************************************/


void MyHDevOperatorImpl::SetDefaultWindow(Hlong win_id)
{
  GetWinIds().SetDefaultWindow(win_id);
}


Hlong MyHDevOperatorImpl::GetDefaultWindow()
{
  return GetWinIds().GetDefaultWindow();
}


size_t MyHDevOperatorImpl::GetCount() const
{
  return GetWinIds().GetCount();
}


void MyHDevOperatorImpl::AddWindow(Hlong id)
{
  GetWinIds().AddWindow(id);
}


Hlong MyHDevOperatorImpl::PopWindow()
{
  return GetWinIds().PopWindow();
}


Hlong MyHDevOperatorImpl::SetWindow(Hlong id)
{
  return GetWinIds().SetWindow(id);
}


// overloaded display operators
int MyHDevOperatorImpl::DevOpenWindow(const HTuple& row,
                                      const HTuple& col,
                                      const HTuple& width,
                                      const HTuple& height,
                                      const HTuple& background, 
                                      HTuple* win_id)
{
  try
  {
	printf("Open window\n");
    // set background color
	SetSystem("int_zooming", "false");
	HTuple* win_types;
    OpenWindow(row,col,width,height,"root","visible","",win_id);

    printf("Past open window with size %d %d\n", width.I(), height.I());

    // save window id
    GetWinIds().AddWindow(win_id[0].L());
    return H_MSG_TRUE;
  }
  catch (HException& exc)
  {
    return exc.ErrorNumber();
  }
}


int MyHDevOperatorImpl::DevSetWindowExtents(const HTuple& row,
                                            const HTuple& col,
                                            const HTuple& width,
                                            const HTuple& height)
{
  HCkDev(SetWindowExtents(GetCurrentWindow(),row,col,width,height));
}


int MyHDevOperatorImpl::DevSetPart(const HTuple& row1,
                                   const HTuple& col1,
                                   const HTuple& row2,
                                   const HTuple& col2)
{
  HCkDev(SetPart(GetCurrentWindow(),row1,col1,row2,col2));
}


int MyHDevOperatorImpl::DevSetWindow(const HTuple& win_id)
{
  GetWinIds().SetWindow(win_id[0].L());
  return H_MSG_TRUE;
}


int MyHDevOperatorImpl::DevGetWindow(HTuple* win_id)
{
  if (win_id)
    *win_id = HTuple(GetCurrentWindow());
  return H_MSG_TRUE;
}


int MyHDevOperatorImpl::DevClearWindow()
{
  HCkDev(ClearWindow(GetCurrentWindow()));
}


int MyHDevOperatorImpl::DevCloseWindow()
{
  // no return, no exception: do not throw an exception if no window is open
  try
  {
    Hlong win_id = GetWinIds().PopWindow();
    if (win_id >= 0)
      CloseWindow(win_id);
  }
  catch (HException&) {}
  return H_MSG_TRUE;
}


int MyHDevOperatorImpl::DevDisplay(const HObject& obj)
{
	printf("should display image now\n");
	// set window coordinate sys to correspond to image
	HTuple im_w;
	HTuple im_h;
	GetImageSize(obj, &im_w, &im_h);
	SetPart(GetCurrentWindow(), 0,0, im_w-1, im_h-1);

	// diyplay
	HCkDev(DispObj(obj,GetCurrentWindow()));

}


int MyHDevOperatorImpl::DevSetDraw(const HTuple& draw)
{
  HCkDev(SetDraw(GetCurrentWindow(),draw));
}


int MyHDevOperatorImpl::DevSetShape(const HTuple& shape)
{
  HCkDev(SetShape(GetCurrentWindow(),shape));
}


int MyHDevOperatorImpl::DevSetColor(const HTuple& color)
{
  HCkDev(SetColor(GetCurrentWindow(),color));
}


int MyHDevOperatorImpl::DevSetColored(const HTuple& colored)
{
  HCkDev(SetColored(GetCurrentWindow(),colored));
}


int MyHDevOperatorImpl::DevSetLut(const HTuple& lut)
{
  HCkDev(SetLut(GetCurrentWindow(),lut));
}


int MyHDevOperatorImpl::DevSetPaint(const HTuple& paint)
{
  HCkDev(SetPaint(GetCurrentWindow(),paint));
}


int MyHDevOperatorImpl::DevSetLineWidth(const HTuple& width)
{
  HCkDev(SetLineWidth(GetCurrentWindow(),width));
}


Hlong MyHDevOperatorImpl::GetCurrentWindow() const
{
  return GetWinIds().GetCurrentWindow();
}


// handles of all opened windows, the last one is the active one
MyHDevOperatorImpl::WinIdContainer& MyHDevOperatorImpl::GetWinIds()
{
  mMutex.Lock();
  WinIdContainer& win_ids = mWinIdsMap[GetThreadId()];
  mMutex.Unlock();
  return win_ids;
}


const MyHDevOperatorImpl::WinIdContainer& MyHDevOperatorImpl::GetWinIds() const
{
  return const_cast<MyHDevOperatorImpl*>(this)->GetWinIds();
}




/*****************************************************************************
 * MyWinIdContainer
 *****************************************************************************
 * Class that collects all open windows and the currently active window of one
 * thread
 ******************************************************************************/

MyHDevOperatorImpl::WinIdContainer::WinIdContainer()
  : mDefaultId(-1)
{
}


Hlong MyHDevOperatorImpl::WinIdContainer::GetCurrentWindow() const
{
  return mIds.empty() ? mDefaultId : mIds.back();
}


void MyHDevOperatorImpl::WinIdContainer::AddWindow(Hlong id)
{
  mIds.push_back(id);
}


Hlong MyHDevOperatorImpl::WinIdContainer::PopWindow()
{
  if (mIds.empty())
    return -1;
  Hlong id = mIds.back();
  mIds.pop_back();
  return id;
}


Hlong MyHDevOperatorImpl::WinIdContainer::SetWindow(Hlong id)
{
  ListType::iterator it = Find(id);
  // handle not found
  if (it == mIds.end())
    return H_MSG_FALSE;
  // put handle at end of list
  mIds.erase(it);
  mIds.push_back(id);
  return H_MSG_TRUE;
}


MyHDevOperatorImpl::WinIdContainer::ListType::iterator
MyHDevOperatorImpl::WinIdContainer::Find(Hlong id)
{
  for (ListType::iterator it = mIds.begin(); it != mIds.end(); ++it)
    if (*it == id)
      return it;
  return mIds.end();
}

