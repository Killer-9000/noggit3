#include "UIHelp.h"

#include "UIText.h" // UIText
#include "Noggit.h" // arial14
#include "Video.h" // video

const int winWidth = 765;
const int winHeight = 600;
  
UIHelp::UIHelp( )
: UICloseWindow( video.xres() / 2.0f - winWidth / 2.0f, video.yres() / 2.0f - winHeight / 2.0f, winWidth, winHeight, "Keybindings", true )
{
  addChild( new UIText( 30.0f, 30.0f, 
    "Basic controls:\n"
    "\n"          
    "  Left mouse dragged - rotate camera\n"
    "  Left mouse - select chunk or object\n"
    "  Both mouse buttons - move forward\n"
    "  I - invert mouse up and down\n"
    "  Q, E - move vertically up, down\n"
    "  A, D, W, S - move left, right, forward, backward\n"
    "  M - show minimap\n"
    "  U - 2D texture editor\n"
    //"C - chunk settings\n" //! \todo: C chunk settings must get fixed first. Then turn on this again
    "  H - help\n"
    "  SHIFT + R - turn camera 180 degres\n"
    "  SHIFT + F4 - change to auto select mode\n"
    "  ESC - exit to main menu\n"
    "\n"
    "Toggles:\n"  
    "\n"
    "  F1 - toggle M2s\n"
    "  F2 - toggle WMO doodads set\n" 
    "  F3 - toggle ground\n"
    "  F4 - toggle GUI\n"
    "  F6 - toggle WMOs\n"
    "  F7 - toggle chunk (red) and ADT (green) lines\n"
    "  F8 - toggle detailed infotext\n"
    "  F9 - toggle map contour\n"
    "  F - toggle fog\n"
    "  TAB - toggle UI view\n"
    "  X - texture palette\n"
    "  CTRL + X - detail window\n"
    "  R/T - Move trough the editing modes\n"
    "\n"
    "Files:\n"
    "  F5 - save bookmark\n"
    "  F10 - reload BLP\n"
    "  F11 - reload M2s\n"
    "  F12 - reload wmo\n"
    "  SHIFT + J - reload ADT tile\n"
    "  CTRL + S -  Save all changed ADT tiles\n"
    "  CTRL + SHIFT + S - Save ADT tiles camera position\n"
    , arial14, eJustifyLeft )
  );
  
  addChild( new UIText( 370.0f, 30.0f, 
    "Edit ground:\n"
    "  SHIFT + F1 - toggle ground edit mode\n"
    "  T - change terrain mode\n"
    "  Y - changes brush type\n"  
    "  ALT + left mouse + mouse move - change brush size\n"
    "Terrain mode \"raise / lower\":\n"
    "  SHIFT + Left mouse - raise terrain\n"
    "  ALT + Left mouse - lower terrain\n"
    "Terrain mode \"flatten / blur\"\n"
    "  SHIFT + Left mouse click - flatten terrain\n"
    "  ALT + Left mouse  click - blur terrain\n"  
    "  Z - change the mode in the option window\n"
    "\n"
    "Edit objects if a model is selected with left click:\n"
    "  Hold middle mouse - move object\n"
    "  ALT + Hold middle mouse - scale M2\n"
    "  SHIFT / CTRL / ALT + Hold left mouse - rotate object\n"
    "  0 - 9 - change doodads set of selected WMO\n"
    "  CTRL + R - Reset rotation\n"
    "  PageDown - Set object to Groundlevel\n"
    "  CTRL + C - Copy object to clipboard\n"
    "  CTRL + V - Paste object on mouse position\n"
    "  - / + - scale M2\n"
    "  Numpad 7 / 9 - rotate object\n"
    "  Numpad 4 / 8 / 6 / 2 - vertical position\n"
    "  Numpad 1 / 3 -  move up/dow\n"
    "    holding SHIFT: double speed \n" 
    "    holding CTRL: triple speed \n"
    "    holding SHIFT and CTRL together: half speed \n"
    "\n"
    "Edit texture:\n"
    "  CTRL + SHIFT + left mouse - clear all textures on chunk\n"
    "  CTRL + left mouse - draw texture or fills if chunk is empty\n"
    "\n"
    "Adjust:\n"
    "  O / P - slower/faster movement\n"
    "  B / N - slower/faster time\n"
    "  SHIFT + - / + - fog distance when no model is selected\n"
    , arial14, eJustifyLeft )
  );
}