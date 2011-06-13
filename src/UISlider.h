#ifndef __SLIDER_H
#define __SLIDER_H

#include "UIFrame.h"

namespace OpenGL { class Texture; }

class UISlider : public UIFrame
{
protected:
  OpenGL::Texture* texture;
  OpenGL::Texture* sliderTexture;
  float scale;
  float offset;
  void (*func)(float value);
  char text[512];
  
public:
  float value;
  void setFunc(void (*f)(float value));
  void setValue(float f);
  void setText(const char *);
  UISlider(float x, float y, float width, float s,float o);
  UIFrame *processLeftClick(float mx,float my);
  bool processLeftDrag(float mx,float my, float xChange, float yChange);
  void render() const;  
};
#endif