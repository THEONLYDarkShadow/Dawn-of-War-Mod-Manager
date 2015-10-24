/***************************************************************
 * Name:      Dawn_of_War_Mod_ManagerApp.cpp
 * Purpose:   Code for Application Class
 * Author:    THEONLYDarkShadow
 * Created:   2015-06-25
 * Copyright: THEONLYDarkShadow
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
  #include "wx_pch.h"
#endif

#ifdef __BORLANDC__
  #pragma hdrstop
#endif //__BORLANDC__

#include "Dawn_of_War_Mod_ManagerApp.h"
#include "Dawn_of_War_Mod_ManagerMain.h"

IMPLEMENT_APP(Dawn_of_War_Mod_ManagerApp);

/// @brief Initialises the application.
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerApp::OnInit()
{
  Dawn_of_War_Mod_ManagerFrame* frame = new Dawn_of_War_Mod_ManagerFrame(0L, _("Dawn of War Mod Manager"));
  frame->SetIcon(wxICON(mod_manager_icon)); // To Set App Icon
  frame->Show();
  return true;
}
