// HDevEngine/C++ example implementations for HDevelop's display operators.
// Original file, part of exemples provided by halcon installation:
//   This file contains a simple implementation for single-threaded
//   applications and a more complex thread save implementation
//
// (C) 2007-2014 MVTec Software GmbH
//
// Extended with image adaption for nice visualization.
//


#ifndef MY_HDEV_OPERATOR_IMPL_H
#define MY_HDEV_OPERATOR_IMPL_H

#ifdef _WIN32
// disable warning C4786: symbol greater than 255 character,
#pragma warning(disable : 4786)
#endif

#ifdef __APPLE__
#include <HALCONCpp/HalconCpp.h>
#include <HDevEngineCpp/HDevEngineCpp.h>
#else
#include "halconcpp/HalconCpp.h"
#include "hdevengine/HDevEngineCpp.h"
#endif

#include <list>
#include <map>

#include "my_thread_impl.h"



/*****************************************************************************
 * MyHDevOperatorImpl
 *****************************************************************************
 * This is a simple implementation of the HDevelop's display operators.
 ******************************************************************************/
class MyHDevOperatorImpl : public HDevEngineCpp::HDevOperatorImplCpp
{
public:

  // Set a default window for programs or procedure that rely on an open
  // window
  void  SetDefaultWindow(Hlong win_id);
  Hlong GetDefaultWindow();

  Hlong       GetCurrentWindow()  const;
  size_t      GetCount()          const;
  void        AddWindow(Hlong id);
  Hlong       PopWindow();
  Hlong       SetWindow(Hlong id);

  // overloaded display operators
  virtual int DevOpenWindow(const HalconCpp::HTuple& row,
                            const HalconCpp::HTuple& col,
                            const HalconCpp::HTuple& width, 
                            const HalconCpp::HTuple& height,
                            const HalconCpp::HTuple& background,
                            HalconCpp::HTuple* win_id);
  virtual int DevSetWindowExtents(const HalconCpp::HTuple& row,
                                  const HalconCpp::HTuple& col,
                                  const HalconCpp::HTuple& width,
                                  const HalconCpp::HTuple& height);
  virtual int DevSetPart(const HalconCpp::HTuple& row1,
                         const HalconCpp::HTuple& col1,
                         const HalconCpp::HTuple& row2,
                         const HalconCpp::HTuple& col2);
  virtual int DevSetWindow(const HalconCpp::HTuple& win_id);
  virtual int DevGetWindow(HalconCpp::HTuple* win_id);
  virtual int DevClearWindow();
  virtual int DevCloseWindow();
  virtual int DevDisplay(const HalconCpp::HObject& obj);
  virtual int DevSetDraw(const HalconCpp::HTuple& draw);
  virtual int DevSetShape(const HalconCpp::HTuple& shape);
  virtual int DevSetColor(const HalconCpp::HTuple& color);
  virtual int DevSetColored(const HalconCpp::HTuple& colored);
  virtual int DevSetLut(const HalconCpp::HTuple& lut);
  virtual int DevSetPaint(const HalconCpp::HTuple& paint);
  virtual int DevSetLineWidth(const HalconCpp::HTuple& width);

protected:

  /***************************************************************************
   * MyWinIdContainer
   *   Class that collects all open windows and the currently active window
   *   of one thread
   ***************************************************************************/
  class WinIdContainer
  {
  public:
    typedef std::list<Hlong> ListType;

    WinIdContainer();
    void   SetDefaultWindow(Hlong win_id)
    {
      mDefaultId = win_id;
    }
    Hlong  GetDefaultWindow() const
    {
      return mDefaultId;
    }
    Hlong  GetCurrentWindow() const;
    size_t GetCount() const
    {
      return mIds.size();
    }
    void   AddWindow(Hlong id);
    Hlong  PopWindow();
    Hlong  SetWindow(Hlong id);
    ListType::iterator Find(Hlong id);

  protected:
    ListType  mIds;
    Hlong     mDefaultId;
  };

  // handles of all opened windows, the last one is the active one
  WinIdContainer&       GetWinIds();
  const WinIdContainer& GetWinIds() const;

  // map that is used for attaching the window handles to the appropriate
  // threads
  std::map<Hlong,WinIdContainer>  mWinIdsMap;
  MyMutexImpl                     mMutex;
};


#endif // #ifndef MY_HDEV_OPERATOR_IMPL_H

