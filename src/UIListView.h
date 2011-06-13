#ifndef __UILISTVIEW_H
#define __UILISTVIEW_H

#include "UIFrame.h"

class UIScrollBar;

class UIListView : public UIFrame
{
public:
  UIListView(float xPos, float yPos, float w, float h, int elementHeight);
  ~UIListView(void);
  void addElement(UIFrame *element);
  void setElementsHeight(int h);
  int getElementsCount();
  void recalcElements(unsigned int value);
  void clear();
  
private:
  int elements_height;
  int elements_start;
  int elements_rows;
  UIScrollBar *scrollbar;
};

#endif