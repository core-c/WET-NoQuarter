program NQ_tool;

uses
  Forms,
  Unit1 in 'Unit1.pas' {fMain};

{$R *.res}
{$R patch260b.RES}


begin
  Application.Initialize;
  Application.Title := 'ET NQ Client Tool';
  Application.CreateForm(TfMain, fMain);
  Application.Run;
end.
