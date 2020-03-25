unit Unit1;
interface
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Registry, StrUtils, OleCtrls, ComCtrls,
  ActiveX, URLmon, ZipForge, jpeg, ExtCtrls;

const
  VERSION = '1.0';

  DRIVE_UNKNOWN     = 0;
  DRIVE_NO_ROOT_DIR = 1;
  DRIVE_REMOVABLE   = 2;
  DRIVE_FIXED       = 3;
  DRIVE_REMOTE      = 4;
  DRIVE_CDROM       = 5;
  DRIVE_RAMDISK     = 6;

  CONFIGS: array[0..1] of string = ('autoexec.cfg','etconfig.cfg');

  NQ_URL = 'http://filebase.wolfmap.de/ET/Mods/noquarter_1-2-7_sf.zip';

type
  TCB = class(TInterfacedObject, IBindStatusCallback)
  public
    function OnStartBinding(dwReserved: DWORD; pib: IBinding): HResult; stdcall;
    function GetPriority(out nPriority): HResult; stdcall;
    function OnLowResource(reserved: DWORD): HResult; stdcall;
    function OnProgress(ulProgress, ulProgressMax, ulStatusCode: ULONG; szStatusText: LPCWSTR): HResult; stdcall;
    function OnStopBinding(hresult: HResult; szError: LPCWSTR): HResult; stdcall;
    function GetBindInfo(out grfBINDF: DWORD; var bindinfo: TBindInfo): HResult; stdcall;
    function OnDataAvailable(grfBSCF: DWORD; dwSize: DWORD; formatetc: PFormatEtc; stgmed: PStgMedium): HResult; stdcall;
    function OnObjectAvailable(const iid: TGUID; punk: IUnknown): HResult; stdcall;
  end;

  TfMain = class(TForm)
    StatusBar: TStatusBar;
    PageControl: TPageControl;
    TabPatch: TTabSheet;
    TabInstall: TTabSheet;
    TabConfig: TTabSheet;
    TabInfo: TTabSheet;
    bPatch: TButton;
    bInstall: TButton;
    ProgressBar: TProgressBar;
    ZipForge: TZipForge;
    bCancelDownload: TButton;
    Label1: TLabel;
    Label2: TLabel;
    cbHunkMegs: TComboBox;
    cbSoundMegs: TComboBox;
    bMegs: TButton;
    Memo: TMemo;
    Image1: TImage;
    Label3: TLabel;
    Image2: TImage;
    Image3: TImage;
    Label4: TLabel;
    Image4: TImage;
    Image5: TImage;
    Label5: TLabel;
    Image6: TImage;
    Image7: TImage;
    Label6: TLabel;
    Image8: TImage;
    procedure FormCreate(Sender: TObject);
    procedure bMegsClick(Sender: TObject);
    procedure bPatchClick(Sender: TObject);
    procedure FormActivate(Sender: TObject);
    procedure bInstallClick(Sender: TObject);
    procedure bCancelDownloadClick(Sender: TObject);
  private
    RegDir: string;
    WolfDir: array of string;
    ConfigFile: array of string;
    DownloadInProgress, DownloadCanceled, DownloadComplete: boolean;
    Binding: IBinding;
    procedure DisableButtons;
    procedure EnableButtons;
    function SearchRegistry : string;
    procedure SearchDrives(const Filename: string; const add: boolean);
    procedure SearchDir(const Dir, Filename: string; const add: boolean);
    procedure SearchFile(const Dir, Filename: string; const add: boolean);
    procedure SetMegs(const Filename,hunkMegs, soundMegs: string);
  end;

var
  fMain: TfMain;


implementation
{$R *.dfm}


procedure TfMain.FormCreate(Sender: TObject);
begin
  Binding := nil;
  DownloadInProgress := false;
  DownloadCanceled := false;
  DownloadComplete := false;
  DisableButtons;
  Memo.Lines.Add('ET NQ Client Tool');
  Memo.Lines.Add('Version '+ VERSION);
  Memo.Lines.Add('');
  PageControl.ActivePageIndex := 0;
  TabPatch.TabVisible := false;
  TabInstall.TabVisible := false;
  TabConfig.TabVisible := false;
end;


procedure TfMain.FormActivate(Sender: TObject);
var Len, i, c: integer;
begin
  fMain.Refresh;
  Memo.Lines.Add('(Please wait for this process to finish)');
  Memo.Lines.Add('');
  Memo.Lines.Add('Searching for ET installs:');
  // registry
  Memo.Lines.Add('    Searching registry...');
  RegDir := SearchRegistry;
  if RegDir<>'' then Memo.Lines.Add('        ET folder: "'+ RegDir +'"');
  // ET-dir
  SearchDrives('ET.exe', true);
  Memo.Lines.Add('done.');
  Memo.Lines.Add('');
  // configs
  Memo.Lines.Add('Searching for configs:');
  Len := Length(WolfDir);
  for i:=0 to Len-1 do begin
    for c:=0 to Length(CONFIGS)-1 do begin
      SearchDir(WolfDir[i], CONFIGS[c], false);
    end;
  end;
  Memo.Lines.Add('done.');
  Memo.Lines.Add('');
  TabPatch.TabVisible := true;
  TabInstall.TabVisible := true;
  TabConfig.TabVisible := true;
  EnableButtons;
end;


procedure TfMain.DisableButtons;
begin
  bPatch.Enabled := false;
  bInstall.Enabled := false;
  bCancelDownload.Enabled := false;
  bMegs.Enabled := false;
  cbHunkMegs.Enabled := false;
  cbSoundMegs.Enabled := false;
end;

procedure TfMain.EnableButtons;
begin
  bPatch.Enabled := true;
  bInstall.Enabled := true;
  bCancelDownload.Enabled := false;
  bMegs.Enabled := true;
  cbHunkMegs.Enabled := true;
  cbSoundMegs.Enabled := true;
end;


//------------------------------------------------------------------------------
//--- ADJUST CONFIGS
//------------------------------------------------------------------------------

procedure TfMain.SetMegs(const Filename, hunkMegs, soundMegs: string);
var F,F2: TextFile;
    Line, hunkLine, soundLine, tmpFilename: string;
    FoundHunkmegs, FoundSoundmegs,
    changeHunkmegs, changeSoundmegs: boolean;
begin
  changeHunkmegs := (hunkMegs <> '<no change>');
  changeSoundmegs := (soundMegs <> '<no change>');
  // change anything?..
  if (not changeHunkmegs) and (not changeSoundmegs) then Exit;
  // confirmation
  if MessageDlg('Adjust config file?: '+ Filename, mtConfirmation, [mbYes, mbNo], 0) = mrNo then begin
    Memo.Lines.Add('    Skipped: '+ Filename);
    Exit;
  end;
  //
  AssignFile(F, Filename);
  {$I-}
  Reset(F);
  {$I+}
  if IOResult <> 0 then Exit;
  FoundHunkmegs := false;
  FoundSoundmegs := false;
  hunkLine := 'seta com_hunkMegs "'+ hunkMegs +'"';
  soundLine := 'seta com_soundMegs "'+ soundMegs +'"';
  tmpFilename := Filename +'.new';
  try
    AssignFile(F2, tmpFilename);
    {$I-}
    Rewrite(F2);
    {$I+}
    if IOResult <> 0 then Exit;
    try
      repeat
        ReadLn(F, Line);
        Line := LowerCase(Line);
        // is it a com_hunkmegs line?..
        if changeHunkmegs then
          if AnsiContainsText('com_hunkmegs', Line) then begin
            Line := AdjustLineBreaks(hunkLine, tlbsCRLF);
            FoundHunkmegs := true;
          end;
        // is it a com_soundmegs line?..
        if changeSoundmegs then
          if AnsiContainsText('com_soundmegs', Line) then begin
            Line := AdjustLineBreaks(soundLine, tlbsCRLF);
            FoundSoundmegs := true;
          end;
        // write to new config file..
        WriteLn(F2, Line);
      until EOF(F);
      // check if changes have been made..
      if changeHunkmegs and (not FoundHunkmegs) then WriteLn(F2, AdjustLineBreaks(hunkLine, tlbsCRLF));
      if changeSoundmegs and (not FoundSoundmegs) then WriteLn(F2, AdjustLineBreaks(soundLine, tlbsCRLF));
    finally
      CloseFile(F2);
    end;
  finally
    CloseFile(F);
  end;
  // delete the old config, and rename the new file..
  if DeleteFile(Filename) then RenameFile(tmpFilename, Filename)
                          else DeleteFile(tmpFilename);
  Memo.Lines.Add('    Adjusted: '+ Filename);
end;


function TfMain.SearchRegistry : string;
begin
  Result := '';
  with TRegistry.Create do
    try
      RootKey := HKEY_LOCAL_MACHINE;
      if OpenKey('\Software\Activision\Wolfenstein - Enemy Territory', False) then begin
        Result := ReadString('InstallPath');
        CloseKey;
      end;
    finally
      Free;
    end;
end;


procedure TfMain.SearchFile(const Dir, Filename: string; const add: boolean);
var SRFile: TSearchRec;
    sDir, s: string;
    Len: integer;
begin
  if FindFirst(Dir + Filename, faAnyFile, SRFile) = 0 then begin
    repeat
      if (SRFile.Attr and faDirectory) <> 0 then Continue;
      s := Dir + SRFile.Name;
      // add to array..
      if add then begin
        // ET directory..
        // is there an "etmain" directory here?..
        if DirectoryExists(Dir +'etmain') then begin
          Len := Length(WolfDir);
          SetLength(WolfDir, Len+1);
          WolfDir[Len] := Dir;
          Memo.Lines.Add('        '+ s);
          Break;
        end;
      end else begin
        // config file..
        Len := Length(ConfigFile);
        SetLength(ConfigFile, Len+1);
        ConfigFile[Len] := s;
        Memo.Lines.Add('        '+ s);
      end;
      //
    until FindNext(SRFile) <> 0;
    FindClose(SRFile);
  end;
end;


procedure TfMain.SearchDir(const Dir, Filename: string; const add: boolean);
var SRDir, SRFile: TSearchRec;
    sDir, s: string;
    Len: integer;
begin
  // search directories..
  if FindFirst(Dir +'*.*', faDirectory + faHidden + faReadOnly, SRDir) = 0 then begin
    repeat
      if (SRDir.Attr and faDirectory) <> 0 then begin
        if (SRDir.Name='.') or (SRDir.Name='..') then Continue;
        sDir := Dir + SRDir.Name;
        if sDir[Length(sDir)]<>'\' then sDir := sDir +'\';
        // search files..
        SearchFile(sDir, Filename, add);
        // search subdirs..
        SearchDir(sDir, Filename, add);
      end;
    until FindNext(SRDir) <> 0;
    FindClose(SRDir);
  end;
end;


procedure TfMain.SearchDrives(const Filename: string; const add: boolean);
var Drives: array[0..128] of Char;
    pDrive: PChar;
    NDrives: integer;
begin
  SetLength(WolfDir, 0);
  NDrives := GetLogicalDriveStrings(SizeOf(Drives), @Drives);
  if NDrives = 0 then Exit;
  pDrive := Drives;
  while pDrive^ <> #0 do begin
    if GetDriveType(pDrive) = DRIVE_FIXED then begin
      Memo.Lines.Add('    Searching drive '+ pDrive +'...');
      // search drive..
      SearchDir(pDrive, Filename, add);
    end;
    Inc(pDrive, 4);
  end;
end;


procedure TfMain.bMegsClick(Sender: TObject);
var Len, i: integer;
begin
  Memo.Lines.Add('Adjusting configs:');
  Len := Length(ConfigFile);
  for i:=0 to Len-1 do
    SetMegs(ConfigFile[i], cbHunkMegs.Items[cbHunkMegs.ItemIndex], cbSoundMegs.Items[cbSoundMegs.ItemIndex]);
  Memo.Lines.Add('done.');
  Memo.Lines.Add('');
end;


//------------------------------------------------------------------------------
//--- PATCH ALL FOUND ET-INSTALLS TO 2.60b
//------------------------------------------------------------------------------

// the 2 2.60b patch files are stored in a resourcefile,
// so they get linked into the final .exe
procedure TfMain.bPatchClick(Sender: TObject);
var RS: TResourceStream;
    i, Len: integer;
begin
  Memo.Lines.Add('Patching ET:');
  Len := Length(WolfDir);
  for i:=0 to Len-1 do begin
    // confirmation
    if MessageDlg('Patch to 2.60b?: '+ WolfDir[i], mtConfirmation, [mbYes, mbNo], 0) = mrNo then Continue;
    // First rename the current .exe files (for backup).
    RenameFile(WolfDir[i]+'ET.exe', WolfDir[i] +'ET.exe.old');
    RenameFile(WolfDir[i]+'ETDED.exe', WolfDir[i] +'ETDED.exe.old');
    // load the 2 patched .exe files from the resourcefile
    // and save them into the proper directory.
    RS := TResourceStream.Create(HInstance, 'rET', 'EXE');
    try
      RS.SaveToFile(WolfDir[i] +'ET.exe');
    finally
      RS.Free;
    end;
    RS := TResourceStream.Create(HInstance, 'rETDED', 'EXE');
    try
      RS.SaveToFile(WolfDir[i]+'ETDED.exe');
    finally
      RS.Free;
    end;
    // message
    Memo.Lines.Add('    Patched: '+ WolfDir[i]);
  end;
  Memo.Lines.Add('done.');
  Memo.Lines.Add('');
end;


//------------------------------------------------------------------------------
//--- INSTALL NOQUARTER 1.2.7
//------------------------------------------------------------------------------
function TCB.GetPriority(out nPriority): HResult;
begin
  Result := E_NOTIMPL;
end;

function TCB.OnLowResource(reserved: DWORD): HResult;
begin
  Result := E_NOTIMPL;
end;

function TCB.GetBindInfo(out grfBINDF: DWORD; var bindinfo: TBindInfo): HResult;
begin
  Result := E_NOTIMPL;
end;

function TCB.OnDataAvailable(grfBSCF: DWORD; dwSize: DWORD; formatetc: PFormatEtc; stgmed: PStgMedium): HResult;
begin
  Result := E_NOTIMPL;
end;

function TCB.OnObjectAvailable(const iid: TGUID; punk: IUnknown): HResult;
begin
  Result := E_NOTIMPL;
end;

function TCB.OnStartBinding(dwReserved: DWORD; pib: IBinding): HResult;
begin
  if not Assigned(fMain) then Exit;
  if fMain.DownloadCanceled then begin
    fMain.Binding := nil;
    Result := E_FAIL;
  end else begin
    fMain.Binding := pib;
    fMain.ProgressBar.Max := 100;
    fMain.ProgressBar.Position := 0;
    fMain.StatusBar.SimpleText := 'Downloading: '+ NQ_URL;
    Result := S_OK;
  end;
end;

function TCB.OnStopBinding(hresult: HResult; szError: LPCWSTR): HResult;
begin
  Result := S_OK;
  if not Assigned(fMain) then Exit;
  fMain.ProgressBar.Max := 100;
  fMain.ProgressBar.Position := 0;
  fMain.StatusBar.SimpleText := '';
end;

function TCB.OnProgress(ulProgress, ulProgressMax, ulStatusCode: ULONG; szStatusText: LPCWSTR): HResult;
begin
  Result := S_OK;
  if not Assigned(fMain) then Exit;
  if fMain.DownloadCanceled then begin
    fMain.StatusBar.SimpleText := 'Download Aborted';
    fMain.Memo.Lines.Add('        Download Aborted');
    fMain.DownloadComplete := false;
    Exit;
  end;
  fMain.ProgressBar.Max := ulProgressMax;
  fMain.ProgressBar.Position := ulProgress;
  //fMain.StatusBar.SimpleText := string(szStatusText);
  case ulStatusCode of
    BINDSTATUS_CONNECTING:        fMain.StatusBar.SimpleText := 'Connecting...';
    BINDSTATUS_BEGINDOWNLOADDATA: fMain.StatusBar.SimpleText := 'Starting Download: '+ NQ_URL;
    BINDSTATUS_DOWNLOADINGDATA:   fMain.StatusBar.SimpleText := 'Downloading: '+ NQ_URL;
    BINDSTATUS_ENDDOWNLOADDATA:   fMain.StatusBar.SimpleText := 'Download Ended: '+ NQ_URL;
    BINDSTATUS_USINGCACHEDCOPY:   fMain.StatusBar.SimpleText := 'Using Cached Copy: '+ NQ_URL;
  end;
  Application.ProcessMessages;
end;

procedure TfMain.bInstallClick(Sender: TObject);
var src,dst: PChar;
    CB: TCB;
    Zip: TZipForge;
    Len, i: integer;
begin
  // check status
  if DownloadInProgress then Exit;
  DownloadInProgress := true;
  DownloadCanceled := false;
  DownloadComplete := false;
  DisableButtons;
  bCancelDownload.Enabled := true;
  //
  try
    Memo.Lines.Add('Downloading NoQuarter 1.2.7 ...(please wait)');
    src := NQ_URL;
    dst := PChar(ExtractFilePath(Application.ExeName) + '\nq127.zip');
    CB := TCB.Create;
    try
      if not UrlDownloadToFile(nil, src, dst, 0, CB) = 0 then Exit;
      if DownloadCanceled then Exit;
      // unpack and install..
      Zip := TZipForge.Create(nil);
      try
        Zip.FileName := dst;
        Zip.OpenArchive(fmOpenRead);
        Len := Length(WolfDir);
        for i:=0 to Len-1 do begin
          // confirmation
          if MessageDlg('Install NoQuarter 1.2.7 into?: '+ WolfDir[i], mtConfirmation, [mbYes, mbNo], 0) = mrNo then Continue;
          //
          Memo.Lines.Add('        Installing NQ 1.2.7 into: '+ WolfDir[i]);
          Zip.BaseDir := WolfDir[i];
          Zip.ExtractFiles('*.*');
        end;
      finally
        Zip.CloseArchive;
      end;
    finally
      {CB.Free;} // don't do this..
      Memo.Lines.Add('done.');
      Memo.Lines.Add('');
    end;
  finally
    DownloadInProgress := false;
    DownloadCanceled := false;
    EnableButtons;
    bCancelDownload.Enabled := false;
  end;
end;

procedure TfMain.bCancelDownloadClick(Sender: TObject);
begin
  if not DownloadInProgress then Exit;
  if Assigned(Binding) then begin
    Binding.Abort;
    Binding := nil;
  end;
  DownloadCanceled := true;
  fMain.Memo.Lines.Add('        Download Aborted');
end;

end.
