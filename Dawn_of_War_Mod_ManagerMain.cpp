/***************************************************************
 * Name:      Dawn_of_War_Mod_ManagerMain.cpp
 * Purpose:   Code for Application Frame
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

#include "Dawn_of_War_Mod_ManagerMain.h"

//helper functions
enum wxbuildinfoformat {short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
  wxString wxbuild(wxVERSION_STRING);

  if (format == long_f )
  {
    #if defined(__WXMSW__)
      wxbuild << _T("-Windows");
    #elif defined(__WXMAC__)
      wxbuild << _T("-Mac");
    #elif defined(__UNIX__)
      wxbuild << _T("-Linux");
    #endif

    #if wxUSE_UNICODE
      wxbuild << _T("-Unicode build");
    #else
      wxbuild << _T("-ANSI build");
    #endif // wxUSE_UNICODE
  }

  return wxbuild;
}

BEGIN_EVENT_TABLE(Dawn_of_War_Mod_ManagerFrame, wxFrame)
    EVT_CLOSE(Dawn_of_War_Mod_ManagerFrame::OnClose)
    EVT_MENU(wxID_EXECUTE, Dawn_of_War_Mod_ManagerFrame::OnStartMod)
    EVT_MENU(idMenuItemShowAllMods, Dawn_of_War_Mod_ManagerFrame::OnShowAllMods)
    EVT_MENU(wxID_REFRESH, Dawn_of_War_Mod_ManagerFrame::OnRefresh)
    EVT_MENU(wxID_EXIT, Dawn_of_War_Mod_ManagerFrame::OnExit)
    EVT_MENU(wxID_ABOUT, Dawn_of_War_Mod_ManagerFrame::OnAbout)
    EVT_CHECKBOX(idCheckBoxDevMode, Dawn_of_War_Mod_ManagerFrame::OnDevModeCheckBoxClicked)
    EVT_CHECKBOX(idCheckBoxNoMovies, Dawn_of_War_Mod_ManagerFrame::OnNoMoviesCheckBoxClicked)
    EVT_CHECKBOX(idCheckBoxForceHighPoly, Dawn_of_War_Mod_ManagerFrame::OnForceHighPolyCheckBoxClicked)
    EVT_BUTTON(idBtnStartMod, Dawn_of_War_Mod_ManagerFrame::OnStartMod)
    EVT_LISTBOX(idListBoxInstalledMods, Dawn_of_War_Mod_ManagerFrame::OnChangeModSelection)
    EVT_LISTBOX_DCLICK(idListBoxInstalledMods, Dawn_of_War_Mod_ManagerFrame::OnStartMod)
END_EVENT_TABLE()

/// @brief Constructor for the Mod Manager frame.
/// @param *frame A pointer to the frame.
/// @param &title The title of the frame.
Dawn_of_War_Mod_ManagerFrame::Dawn_of_War_Mod_ManagerFrame(wxFrame *frame, const wxString &title) : wxFrame(frame, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
  canRunMod = false;
  nonPlayableModsVisible = false;
  devModeActive = false;
  noMoviesActive = false;
  forceHighPolyActive = false;
  moduleFilePaths = new wxArrayString();
  modNames = new wxArrayString();
  requiredMods = new wxArrayString();

  if (!Init() || !GetModuleFilePaths() || !GetModNames() || !InitGUI())
  {
    Close();
  }
}

/// @brief Destructor for the Mod Manager frame.
Dawn_of_War_Mod_ManagerFrame::~Dawn_of_War_Mod_ManagerFrame()
{
}

/// @brief Initialiser function for the Mod Manager.
/// Asks user for a directory containing a Dawn of War/Winter Assault/Dark Crusade/Soulstorm executable
/// and records the executable name, directory and game name.
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::Init()
{
  wxDirDialog chooseFolderDialog(nullptr, "Select executable directory of Dawn of War/Winter Assault/Dark Crusade/Soulstorm", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
  chooseFolderDialog.SetIcon(wxICON(mod_manager_icon));

  if (chooseFolderDialog.ShowModal() == wxID_CANCEL)
  {
    return false;
  }
  dowExecDir = chooseFolderDialog.GetPath();

  wxDir dirHandle(dowExecDir);
  if (!dirHandle.IsOpened())
  {
    wxMessageBox("The directory you have chosen cannot be opened.", "Cannot Open Directory");
    return false;
  }

  int numOfDoWExecutables = 0;
  bool hasDOW = false;
  bool hasWA = false;
  bool hasDC = false;
  bool hasSS = false;

  if (dirHandle.HasFiles("W40k.exe"))
  {
    numOfDoWExecutables++;
    hasDOW = true;
  }

  if (dirHandle.HasFiles("W40kWA.exe"))
  {
    numOfDoWExecutables++;
    hasWA = true;
  }

  if (dirHandle.HasFiles("darkcrusade.exe"))
  {
    numOfDoWExecutables++;
    hasDC = true;
  }

  if (dirHandle.HasFiles("Soulstorm.exe"))
  {
    numOfDoWExecutables++;
    hasSS = true;
  }

  if (numOfDoWExecutables == 0)
  {
    wxMessageBox("You must choose a directory with either the Dawn of War, Winter Assault, Dark Crusade or Soulstorm executable inside it.", "Game Executable Not Found");
    return false;
  }
  else if (numOfDoWExecutables == 1)
  {
    if (hasDOW)
    {
      dowGame = "Dawn of War";
      dowExecutable = "W40k.exe";
    }
    else if (hasWA)
    {
      dowGame = "Winter Assault";
      dowExecutable = "W40kWA.exe";
    }
    else if (hasDC)
    {
      dowGame = "Dark Crusade";
      dowExecutable = "darkcrusade.exe";
    }
    else if (hasSS)
    {
      dowGame = "Soulstorm";
      dowExecutable = "Soulstorm.exe";
    }
    else
    {
      return false;
    }
  }
  else if (numOfDoWExecutables > 1)
  {
    wxArrayString choices;
    if (hasDOW) {choices.Add("Dawn of War");}
    if (hasWA) {choices.Add("Winter Assault");}
    if (hasDC) {choices.Add("Dark Crusade");}
    if (hasSS) {choices.Add("Soulstorm");}

    wxString chosenExecutable = wxGetSingleChoice("There is more than one Dawn of War executable detected in the directory you have chosen.\nPlease select which game you wish to play.", "Choose Game", choices);
    if (chosenExecutable == "Dawn of War")
    {
      dowGame = chosenExecutable;
      dowExecutable = "W40k.exe";
    }
    else if (chosenExecutable == "Winter Assault")
    {
      dowGame = chosenExecutable;
      dowExecutable = "W40kWA.exe";
    }
    else if (chosenExecutable == "Dark Crusade")
    {
      dowGame = chosenExecutable;
      dowExecutable = "darkcrusade.exe";
    }
    else if (chosenExecutable == "Soulstorm")
    {
      dowGame = chosenExecutable;
      dowExecutable = "Soulstorm.exe";
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  dirHandle.Close();
  return true;
}

/// @brief Creates all the GUI elements and populates them with relevant data.
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::InitGUI()
{
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  statusBar = CreateStatusBar();
  statusBar->SetStatusText("Created by THEONLYDarkShadow. Based on Cosmocrat's DoW MODenizer tool.");

  fileMenu = new wxMenu();
  fileMenu->Append(wxID_EXECUTE, "Run Mod...", "Run currently selected mod.");
  fileMenu->Append(idMenuItemShowAllMods, "Show All Mods", "Shows all mods, including ones that are marked as non-playable.", wxITEM_CHECK);
  fileMenu->Append(wxID_REFRESH, "", "Refresh mod list.");
  fileMenu->Append(wxID_EXIT, "", "Exit the Dawn of War Mod Manager.");
  fileMenu->Enable(wxID_EXECUTE, false);

  wxMenu* helpMenu = new wxMenu();
  helpMenu->Append(wxID_ABOUT, "", "Show information about the Dawn of War Mod Manager.");

  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append(fileMenu, "File");
  menuBar->Append(helpMenu, "Help");

  this->SetMenuBar(menuBar);

  wxSizerFlags flagsLeft(1);
  wxSizerFlags flagsLeftNoBorder(0);
  wxSizerFlags flagsRight(1);
  wxSizerFlags flagsRightExpand(1);
  flagsLeft.Left().Border(wxALL, 5);
  flagsLeftNoBorder.Left().Border(wxALL, 0);
  flagsRight.Right().Border(wxALL, 5);
  flagsRightExpand.Right().Expand().Border(wxALL, 5);

  wxFlexGridSizer* containerSizer = new wxFlexGridSizer(2, 1, 1);

  wxFlexGridSizer* topSizer = new wxFlexGridSizer(2, 1, 1);
  wxBoxSizer* directorySizer = new wxBoxSizer(wxHORIZONTAL);
  directorySizer->Add(new wxStaticText(this, wxID_ANY, "Your current " + dowGame + " directory:", wxDefaultPosition, wxDefaultSize, 0), flagsLeft);
  directorySizer->Add(new wxTextCtrl(this, wxID_ANY, dowExecDir, wxDefaultPosition, wxDefaultSize, wxTE_READONLY), flagsRight);

  wxBoxSizer* optionsSizer = new wxBoxSizer(wxVERTICAL);
  optionsSizer->Add(new wxStaticText(this, wxID_ANY, "Advanced Start Options:", wxDefaultPosition, wxDefaultSize, 0), flagsLeft);
  devModeCheckBox = new wxCheckBox(this, idCheckBoxDevMode, "-dev: Developer Mode", wxDefaultPosition, wxDefaultSize, 0);
  noMoviesCheckBox = new wxCheckBox(this, idCheckBoxNoMovies, "-nomovies: No Intro Movies", wxDefaultPosition, wxDefaultSize, 0);
  forceHighPolyCheckBox = new wxCheckBox(this, idCheckBoxForceHighPoly, "-forcehighpoly: High Poly Models at any Distance", wxDefaultPosition, wxDefaultSize, 0);
  optionsSizer->Add(devModeCheckBox, flagsLeftNoBorder);
  optionsSizer->Add(noMoviesCheckBox, flagsLeftNoBorder);
  optionsSizer->Add(forceHighPolyCheckBox, flagsLeftNoBorder);

  wxBoxSizer* startButtonSizer = new wxBoxSizer(wxHORIZONTAL);
  startModButton = new wxButton(this, idBtnStartMod, "Start " + dowGame + " With Selected Mod", wxDefaultPosition, wxSize(195, 48), 0);
  /*wxInitAllImageHandlers();
  wxBitmap checkmarkImage("Checkmark.png", wxBITMAP_TYPE_PNG);
  wxBitmap crossImage("cross.png", wxBITMAP_TYPE_PNG);
  startModButton->SetBitmap(checkmarkImage, wxRIGHT);
  startModButton->SetBitmapDisabled(crossImage);*/
  startModButton->Disable();
  startButtonSizer->AddSpacer(100);
  startButtonSizer->Add(startModButton, flagsRight);

  topSizer->Add(directorySizer, flagsLeft);
  topSizer->AddSpacer(0);
  topSizer->Add(optionsSizer, flagsLeft);
  topSizer->Add(startButtonSizer, flagsRight);


  wxFlexGridSizer* bottomSizer = new wxFlexGridSizer(2, 1, 1);
  installedModsListBox = new wxListBox(this, idListBoxInstalledMods, /*wxPoint(19, 137)*/ wxDefaultPosition, wxSize(375, 407), *modNames, wxLB_SINGLE);
  requiredModsListBox = new wxListBox(this, idListBoxRequiredMods, /*wxPoint(400, 137)*/ wxDefaultPosition, wxSize(375, 407), *requiredMods, wxLB_SINGLE);
  bottomSizer->Add(new wxStaticText(this, wxID_ANY, "Currently Installed Mods", wxDefaultPosition, wxDefaultSize, 0), flagsLeft);
  bottomSizer->Add(new wxStaticText(this, wxID_ANY, "Required Mods", wxDefaultPosition, wxDefaultSize, 0), flagsRightExpand);
  bottomSizer->Add(installedModsListBox, flagsLeft);
  bottomSizer->Add(requiredModsListBox, flagsRight);

  containerSizer->Add(topSizer, flagsLeft);
  containerSizer->AddSpacer(0);
  containerSizer->Add(bottomSizer, flagsLeft);
  this->SetSizer(containerSizer);
  this->Layout();
  containerSizer->Fit(this);

  return true;
}

/// @brief Gets all the file paths for all the module files in the executable directory.
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::GetModuleFilePaths()
{
  wxDir dirHandle(dowExecDir);
  if (!dirHandle.IsOpened())
  {
    wxMessageBox("The directory you have chosen cannot be opened.", "Cannot Open Directory");
    return false;
  }

  dirHandle.GetAllFiles(dowExecDir, moduleFilePaths, "*.module", wxDIR_FILES);
  moduleFilePaths->Shrink(); // Minimise ArrayString memory consumption.

  if (moduleFilePaths->GetCount() == 0)
  {
    wxMessageBox("There are no module files found in the selected directory.", "No Module Files Found");
    return false;
  }

  dirHandle.Close();
  return true;
}

/// @brief Extracts the name of the mods for the module files acquired via the GetModuleFilePaths() function.
/// @see GetModuleFilePaths()
/// @see GetModVersion()
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::GetModNames()
{
  wxFileName fileNameHandle;
  wxString moduleFilePath;
  wxString modNameAndVersion;
  size_t insertionCounter = 0;

  for (size_t loopCounter = 0; loopCounter < moduleFilePaths->GetCount(); loopCounter++)
  {
    moduleFilePath = moduleFilePaths->Item(loopCounter);

    // If a mod is non-playable and Show All Mods is not checked, skip adding it to the list of Mod Names.
    if (!nonPlayableModsVisible)
    {
      if (!CheckIfModIsPlayable(moduleFilePath))
      {
        moduleFilePaths->RemoveAt(loopCounter);
        loopCounter--;
        continue;
      }
    }

    fileNameHandle = moduleFilePath;
    modNameAndVersion = fileNameHandle.GetName() + " (Version " + GetModVersion(moduleFilePath) + ")";
    modNames->Insert(modNameAndVersion, insertionCounter);
    insertionCounter++;
  }
  modNames->Shrink(); // Minimise wxArrayString memory consumption.

  if (modNames->GetCount() == 0)
  {
    wxMessageBox("Unable to extract the names of the mods.", "No Mod Names Extracted");
    return false;
  }
  return true;
}

/// @brief Reads a module file and gathers all the Required Mods data from it.
/// @param moduleFilePath The file path of a single module file.
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::GetRequiredMods(wxString moduleFilePath)
{
  wxTextFile textFileHandle(moduleFilePath);

  if (!textFileHandle.Open())
  {
    wxMessageBox("Unable to open file.", "Unable to open file.");
    return false;
  }

  requiredMods->Clear(); // Empties current Required Mods array while freeing up memory.
  size_t counter = 0;

  for (wxString str = textFileHandle.GetFirstLine(); !textFileHandle.Eof(); str = textFileHandle.GetNextLine())
  {
    if (str.StartsWith("RequiredMod."))
    {
      requiredMods->Insert(str, counter);
      counter++;
    }
  }
  requiredMods->Shrink(); // Minimise wxArrayString memory consumption.

  if (requiredMods->GetCount() == 0)
  {
    requiredMods->Insert("No mods required to run!", 0);
  }
  return true;
}

/// @brief Gets the version of the mod passed to the function.
/// @param moduleFilePath The file path of a single module file.
/// @see CheckIfModVersionsMatch()
/// @return The version of the mod in std::string format.
std::string Dawn_of_War_Mod_ManagerFrame::GetModVersion(wxString moduleFilePath)
{
  wxTextFile textFileHandle(moduleFilePath);

  if (!textFileHandle.Open())
  {
    wxMessageBox("Unable to open file.", "Unable to open file.");
    return "error";
  }

  wxString temp;
  wxString modVersion;

  for (wxString str = textFileHandle.GetFirstLine(); !textFileHandle.Eof(); str = textFileHandle.GetNextLine())
  {
    if (str.StartsWith("ModVersion = ", &modVersion))
    {
      return modVersion.ToStdString();
    }
  }
  return "";
}

/// @brief Gets the desired version of the required mod passed to the function.
/// @param itemNumber An index of a desired required mod in the requiredMods array.
/// @param fileString A line of text from a module file.
/// @param &textFileHandle A reference to a wxTextFile handler.
/// @see CheckIfModVersionsMatch()
/// @return The desired version of the required mod in std::string format.
std::string Dawn_of_War_Mod_ManagerFrame::GetDesiredModVersion(size_t itemNumber, wxString fileString, wxTextFile &textFileHandle)
{
  wxString desiredModVersion;

  if (fileString.StartsWith(requiredMods->Item(itemNumber)))
  {
    fileString = textFileHandle.GetPrevLine();
    if (fileString.StartsWith(";; Version = ", &desiredModVersion) || fileString.StartsWith("// Version = ", &desiredModVersion) || fileString.StartsWith("-- Version = ", &desiredModVersion))
    {
      fileString = textFileHandle.GetNextLine(); // Prevents infinite loop.
      return desiredModVersion.ToStdString();
    }
    fileString = textFileHandle.GetNextLine(); // Prevents infinite loop.
  }
  return "";
}

/// @brief Checks to see if the desired version of a required mod matches the actual version of a required mod.
/// @param itemNumber An index of a desired required mod in the requiredMods array.
/// @param moduleFilePath The file path of a single module file.
/// @param &fileString A reference to a line of text from a module file.
/// @param &textFileHandle A reference to a wxTextFile handler.
/// @see GetModVersion()
/// @see GetDesiredModVersion()
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::CheckIfModVersionsMatch(size_t itemNumber, wxString moduleFilePath, wxString &fileString, wxTextFile &textFileHandle)
{
  GetRequiredMods(moduleFilePath);
  size_t numOfModuleFilePaths = moduleFilePaths->Count();
  std::string modVersion;
  std::string desiredModVersion;
  wxString temp;

  for (; !textFileHandle.Eof(); fileString = textFileHandle.GetNextLine())
  {
    if (fileString.StartsWith("RequiredMod."))
    {
      temp = requiredMods->Item(itemNumber);
      temp = temp.Trim(); // Remove spaces from end of line.
      temp = temp.AfterLast(' ');

      for (size_t counter = 0; counter < numOfModuleFilePaths; counter++)
      {
        if (moduleFilePaths->Item(counter).Find(temp) != wxNOT_FOUND)
        {
          modVersion = GetModVersion(moduleFilePaths->Item(counter));
          break;
        }
      }

      desiredModVersion = GetDesiredModVersion(itemNumber, fileString, textFileHandle);

      if (modVersion == desiredModVersion || desiredModVersion == "")
      {
        fileString = textFileHandle.GetNextLine();
        return true;
      }
      else if (modVersion != desiredModVersion)
      {
        fileString = textFileHandle.GetNextLine();
        return false;
      }
    }
  }
  return true;
}

/// @brief Checks for mods that are installed.
/// Checks to make sure that a required mod listed via the GetRequiredMods() function
/// actually exists in the executable directory.
/// @param moduleFilePath The file path of a single module file.
/// @see CheckForModDirectory()
/// @see CheckIfModVersionsMatch()
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::CheckForInstalledMods(wxString moduleFilePath)
{
  wxDir dirHandle(dowExecDir);
  if (!dirHandle.IsOpened())
  {
    wxMessageBox("The directory cannot be opened.", "Cannot Open Directory");
    return false;
  }

  wxTextFile textFileHandle(moduleFilePath);

  if (!textFileHandle.Open())
  {
    wxMessageBox("Unable to open file.", "Unable to open file.");
    return false;
  }

  size_t numOfRequiredMods = requiredMods->GetCount();
  size_t numOfInstalledMods = 0;
  wxString currentModName;
  wxString currentModNameAfterLast;
  wxString fileString = textFileHandle.GetFirstLine();

  for (size_t counter = 0; counter < numOfRequiredMods; counter++)
  {
    currentModName = requiredMods->Item(counter);

    if (!currentModName.StartsWith("No mods required to run!"))
    {
      currentModNameAfterLast = currentModName.AfterLast(' ');

      if (dirHandle.HasFiles(currentModNameAfterLast + ".module") && CheckForModDirectory(moduleFilePath))
      {
        if (CheckIfModVersionsMatch(counter, moduleFilePath, fileString, textFileHandle))
        {
          currentModName += " is installed!";
          numOfInstalledMods++;
        }
        else
        {
          currentModName += " has an incompatible version installed!";
        }
      }
      else
      {
        currentModName += " is missing!";
      }

      requiredModsListBox->SetString(counter, currentModName);
    }
    else
    {
      numOfInstalledMods++;
    }
  }

  dirHandle.Close();

  if (numOfRequiredMods == numOfInstalledMods)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/// @brief Checks for existence of mod directory.
/// Helper function for CheckForInstalledMods() function to makes sure the required mod directory
/// actually exists in the executable directory.
/// @param moduleFilePath The file path of a single module file.
/// @see CheckForInstalledMods()
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::CheckForModDirectory(wxString moduleFilePath)
{
  wxTextFile textFileHandle(moduleFilePath);

  if (!textFileHandle.Open())
  {
    wxMessageBox("Unable to open file.", "Unable to open file.");
    return false;
  }

  wxDir dirHandle(dowExecDir);

  if (!dirHandle.IsOpened())
  {
    wxMessageBox("The directory cannot be opened.", "Cannot Open Directory");
    return false;
  }

  wxString modFolder;

  for (wxString str = textFileHandle.GetFirstLine(); !textFileHandle.Eof(); str = textFileHandle.GetNextLine())
  {
    if (str.StartsWith("ModFolder = ", &modFolder))
    {
      if (dirHandle.HasSubDirs(modFolder))
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }

  return false;
}

/// @brief Checks if the supplied mod is marked as playable or non-playable.
/// @param moduleFilePath The file path of a single module file.
/// @return Boolean.
bool Dawn_of_War_Mod_ManagerFrame::CheckIfModIsPlayable(wxString moduleFilePath)
{
  wxTextFile textFileHandle(moduleFilePath);

  if (!textFileHandle.Open())
  {
    wxMessageBox("Unable to open file.", "Unable to open file.");
    return false;
  }

  wxString isPlayable;

  for (wxString str = textFileHandle.GetFirstLine(); !textFileHandle.Eof(); str = textFileHandle.GetNextLine())
  {
    if (str.StartsWith("Playable = ", &isPlayable))
    {
      if (isPlayable == "1")
      {
        return true;
      }
      else
      {
        return false;
      }
    }
  }

  return false;
}

/// @brief Runs the selected mod with selected command-line options, if possible.
void Dawn_of_War_Mod_ManagerFrame::RunMod()
{
  wxString currentlySelectedMod = modNames->Item(installedModsListBox->GetSelection());
  wxString command = dowExecDir + "\\" + dowExecutable + " -modname " + currentlySelectedMod;

  if (devModeActive)
  {
    command += " -dev";
  }

  if (noMoviesActive)
  {
    command += " -nomovies";
  }

  if (forceHighPolyActive)
  {
    command += " -forcehighpoly";
  }

  int answer = wxMessageBox("Start the following mod?\n" + command, "Start Mod?", wxYES|wxNO);

  if (answer == wxYES)
  {
    wxExecute(command);
  }
}

/// @brief Clears the contents of the Installed Mods List Box and the Required Mods List Box, then repopulates the former.
void Dawn_of_War_Mod_ManagerFrame::RefreshModListBoxes()
{
  moduleFilePaths->Clear();
  modNames->Clear();
  requiredMods->Clear();
  installedModsListBox->Clear();
  requiredModsListBox->Clear();
  canRunMod = false;
  startModButton->Disable();
  fileMenu->Enable(wxID_EXECUTE, false);

  GetModuleFilePaths();
  GetModNames();
  installedModsListBox->InsertItems(*modNames, 0);
}

/// @brief Event handler for when the Developer Mode Check Box is clicked.
void Dawn_of_War_Mod_ManagerFrame::OnDevModeCheckBoxClicked(wxCommandEvent &event)
{
  if (devModeCheckBox->IsChecked())
  {
    devModeActive = true;
  }
  else
  {
    devModeActive = false;
  }
}

/// @brief Event Handler for when the No Movies Check Box is clicked.
void Dawn_of_War_Mod_ManagerFrame::OnNoMoviesCheckBoxClicked(wxCommandEvent &event)
{
  if (noMoviesCheckBox->IsChecked())
  {
    noMoviesActive = true;
  }
  else
  {
    noMoviesActive = false;
  }
}

/// @brief Event Handler for when the Force High Poly Check Box is clicked.
void Dawn_of_War_Mod_ManagerFrame::OnForceHighPolyCheckBoxClicked(wxCommandEvent &event)
{
  if (forceHighPolyCheckBox->IsChecked())
  {
    forceHighPolyActive = true;
  }
  else
  {
    forceHighPolyActive = false;
  }
}

/// @brief Event Handler for when the Start Mod Button is pressed.
/// @see RunMod()
void Dawn_of_War_Mod_ManagerFrame::OnStartMod(wxCommandEvent &event)
{
  if (canRunMod)
  {
    RunMod();
  }
}

/// @brief Event Handler for when the selection in the Installed Mods List Box is changed.
/// @see GetRequiredMods()
/// @see CheckForInstalledMods()
/// @see CheckIfModIsPlayable()
void Dawn_of_War_Mod_ManagerFrame::OnChangeModSelection(wxCommandEvent &event)
{
  wxString moduleFilePath = moduleFilePaths->Item(installedModsListBox->GetSelection());

  if (GetRequiredMods(moduleFilePath))
  {
    requiredModsListBox->Clear();
    requiredModsListBox->InsertItems(*requiredMods, 0);
  }

  bool requiredModsAreInstalled = CheckForInstalledMods(moduleFilePath);
  bool modIsMarkedAsPlayable = CheckIfModIsPlayable(moduleFilePath);

  if (requiredModsAreInstalled && modIsMarkedAsPlayable)
  {
    statusBar->SetStatusText("This mod can be played.");
    startModButton->Enable();
    fileMenu->Enable(wxID_EXECUTE, true);
    canRunMod = true;
  }
  else if (!modIsMarkedAsPlayable)
  {
    statusBar->SetStatusText("This mod is marked as non-playable.");
    startModButton->Disable();
    fileMenu->Enable(wxID_EXECUTE, false);
    canRunMod = false;
  }
  else
  {
    statusBar->SetStatusText("This mod cannot be played.");
    startModButton->Disable();
    fileMenu->Enable(wxID_EXECUTE, false);
    canRunMod = false;
  }
}

/// @brief Event Handler for when the Mod Manager is closed.
void Dawn_of_War_Mod_ManagerFrame::OnClose(wxCloseEvent &event)
{
  Destroy();
}

/// @brief Event Handler for when the Show All Mods Menu Item is clicked.
void Dawn_of_War_Mod_ManagerFrame::OnShowAllMods(wxCommandEvent &event)
{
  if (fileMenu->IsChecked(idMenuItemShowAllMods))
  {
    nonPlayableModsVisible = true;
    statusBar->SetStatusText("All mods are now shown.");
  }
  else
  {
    nonPlayableModsVisible = false;
    statusBar->SetStatusText("Only playable mods are now shown.");
  }

  RefreshModListBoxes();
}

/// @brief Event Handler for when the Refresh Mod Menu Item is clicked.
/// @see GetModuleFilePaths()
/// @see GetModNames()
/// @see RefreshModListBoxes()
void Dawn_of_War_Mod_ManagerFrame::OnRefresh(wxCommandEvent &event)
{
  statusBar->SetStatusText("Refreshing mod list...");
  RefreshModListBoxes();
  statusBar->SetStatusText("Mod list has been refreshed.");
}

/// @brief Event Handler for when the Exit Menu Item is clicked.
void Dawn_of_War_Mod_ManagerFrame::OnExit(wxCommandEvent &event)
{
  Close();
}

/// @brief Event Handler for when the About Menu Item is clicked.
void Dawn_of_War_Mod_ManagerFrame::OnAbout(wxCommandEvent &event)
{
  wxAboutDialogInfo aboutInfo;
  aboutInfo.SetName("Dawn of War Mod Manager");
  aboutInfo.SetVersion("1.1. Built with " + wxbuildinfo(long_f) + ".");
  aboutInfo.SetDescription("A mod manager for all the Dawn of War 1 games.\nBased on Cosmocrat's DoW MODenizer.\nIcon from the DoW MODenizer.");
  aboutInfo.SetCopyright("(C) 2015 THEONLYDarkShadow");
  aboutInfo.AddDeveloper("THEONLYDarkShadow");

  wxAboutBox(aboutInfo);
}
