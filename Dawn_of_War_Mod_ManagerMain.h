/***************************************************************
 * Name:      Dawn_of_War_Mod_ManagerMain.h
 * Purpose:   Defines Application Frame
 * Author:    THEONLYDarkShadow
 * Created:   2015-06-25
 * Copyright: THEONLYDarkShadow
 * License:
 **************************************************************/

#ifndef DAWN_OF_WAR_MOD_MANAGERMAIN_H
#define DAWN_OF_WAR_MOD_MANAGERMAIN_H

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "Dawn_of_War_Mod_ManagerApp.h"


#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/aboutdlg.h>

class Dawn_of_War_Mod_ManagerFrame: public wxFrame
{
  public:
    Dawn_of_War_Mod_ManagerFrame(wxFrame *frame, const wxString& title);
    ~Dawn_of_War_Mod_ManagerFrame();
    bool Init();
    bool InitGUI();
    bool GetModuleFilePaths();
    bool GetModNames();
    bool GetRequiredMods(wxString moduleFilePath);
    std::string GetModVersion(wxString moduleFilePath);
    std::string GetDesiredModVersion(size_t itemNumber, wxString fileString, wxTextFile &textFileHandle);
    bool CheckIfModVersionsMatch(size_t itemNumber, wxString moduleFilePath, wxString &fileString, wxTextFile &textFileHandle);
    bool CheckForInstalledMods(wxString moduleFilePath);
    bool CheckForModDirectory(wxString moduleFilePath);
    bool CheckIfModIsPlayable(wxString moduleFilePath);
    void RunMod();

  protected:
    bool canRunMod;
    bool devModeActive;
    bool noMoviesActive;
    bool forceHighPolyActive;
    enum {idBtnStartMod = 1000, idCheckBoxDevMode, idCheckBoxNoMovies, idCheckBoxForceHighPoly, idListBoxInstalledMods, idListBoxRequiredMods};
    wxString dowExecDir;
    wxString dowGame;
    wxString dowExecutable;
    wxArrayString* moduleFilePaths;
    wxArrayString* modNames;
    wxArrayString* requiredMods;
    wxCheckBox* devModeCheckBox;
    wxCheckBox* noMoviesCheckBox;
    wxCheckBox* forceHighPolyCheckBox;
    wxButton* startModButton;
    wxListBox* installedModsListBox;
    wxListBox* requiredModsListBox;
    wxStatusBar* statusBar;
    wxMenu* fileMenu;

  private:
    void OnDevModeCheckBoxClicked(wxCommandEvent& event);
    void OnNoMoviesCheckBoxClicked(wxCommandEvent& event);
    void OnForceHighPolyCheckBoxClicked(wxCommandEvent& event);
    void OnStartMod(wxCommandEvent& event);
    void OnChangeModSelection(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnResize(wxSizeEvent& event);
    DECLARE_EVENT_TABLE()
};

#endif // DAWN_OF_WAR_MOD_MANAGERMAIN_H
