// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/ui/Window.h>
#include <noggit/Selection.h>

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QWidget>

class UIText;
class UITextBox;
class WMOInstance;

class UIRotationEditor : public QWidget
{
public:
  UIRotationEditor();

  void select(selection_type entry);
  void clearSelect();
  void updateValues();
  bool hasSelection() const { return _selection; }

  bool hasFocus() const {return false;}

private:
  void maybe_updateWMO();
  math::vector_3d* rotationVect;
  math::vector_3d* posVect;
  float* scale;

  bool _selection;
  WMOInstance* _wmoInstance;

  QDoubleSpinBox* _rotation_x;
  QDoubleSpinBox* _rotation_z;
  QDoubleSpinBox* _rotation_y;
  QDoubleSpinBox* _position_x;
  QDoubleSpinBox* _position_z;
  QDoubleSpinBox* _position_y;
  QDoubleSpinBox* _scale;
};
