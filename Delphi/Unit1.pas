unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, rs232, ExtCtrls, StdCtrls, TeEngine, Series, TeeProcs, TeeStore,Chart, DB, TeeData,
  xmldom, XMLIntf, msxmldom, XMLDoc, VclTee.TeeGDIPlus, Xml.adomxmldom,
  Xml.omnixmldom;

type
  TMainForm = class(TForm)
    COM1Timer: TTimer;
    Memo1: TMemo;
    Label1: TLabel;
    lblCWU: TLabel;
    lblCO: TLabel;
    Label4: TLabel;
    lblDZ: TLabel;
    Label6: TLabel;
    Label9: TLabel;
    lblTW: TLabel;
    lblTZ: TLabel;
    Label12: TLabel;
    Chart1: TChart;
    btnPomiar: TButton;
    Button2: TButton;
    Series2: TLineSeries;
    Series3: TLineSeries;
    Series4: TLineSeries;
    Series5: TLineSeries;
    Memo2: TMemo;
    Button1: TButton;
    Edit1: TEdit;
    Series1: TLineSeries;
    Button3: TButton;
    Timer1: TTimer;
    Button4: TButton;
    ChartDataSet1: TChartDataSet;
    btnLoadXml: TButton;
    XMLDoc: TXMLDocument;
    OD1: TOpenDialog;
    cbClear: TCheckBox;
    pretime: TEdit;
    posttime: TEdit;
    Memo3: TMemo;
    Memo4: TMemo;
    Label2: TLabel;
    lblPK: TLabel;
    Label3: TLabel;
    lblAL: TLabel;
    lblALint: TLabel;
    Series6: TLineSeries;
    procedure FormCreate(Sender: TObject);
    procedure COM1DataReceived(Sender: TObject);
    procedure btnPomiarClick(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure btnLoadXmlClick(Sender: TObject);
    procedure Memo2DblClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  const
  KONIEC_WIERSZA=#13#10;
var
  MainForm: TMainForm;
  NOSERIAL:Boolean;
  COM1: TSerialPort;
  oldczas: string;

implementation

{$R *.dfm}

procedure WriteLog(dane:String);
begin
  MainForm.Memo1.Lines.Add(dane);
end;

function fixtemp(dane:String):string;
var
  iwart:integer;
  wart: double;
  wy:string;
begin

 iwart:=StrToIntDef(dane,15);
// iwart:=StrToIntDef(trim(dane),0);
 wart:=iwart/10;
 wy:=FloatToStr(wart);
 if pos(',',wy)=0 then wy:=wy+',0';
 result:=wy;
// mainform.memo2.lines.add('"'+dane+'"['+inttostr(length(dane))+']='+inttostr(iwart)+'='+result);

end;

function fixbit(dane:String):string;
var
  x,pot,iwart:integer;
  wart: integer;
  wy:string;
begin

 wart:=StrToIntDef(dane,0);
 pot:=1;
 wy:='';

for x:=0 to 7 do
 begin
   if (wart and pot)=pot then wy:='1 '+wy else wy:='0 '+wy;
   pot:=pot+pot;
 end;
 result:=wy;

end;

function fixhexbit(dane:String):string;
var
  x,pot,iwart:integer;
  wart: integer;
  wy:string;
begin

 wart:=StrToInt64('$'+dane);
 pot:=1;
 wy:='';

for x:=0 to 7 do
 begin
   if (wart and pot)=pot then wy:='1 '+wy else wy:='0 '+wy;
   pot:=pot+pot;
 end;
 result:=wy;

end;

procedure DodajPomiar(czart:TChart;czas:string;seria:integer;wartosc:string);
var
  wtmp: double;
  n: Integer;
begin
 if wartosc<>'' then
  begin
    wtmp:=StrToFloatDef(wartosc,0.0);
    n:=czart.Series[seria].Count;
    if (n=0) then
       begin
          if (wtmp<>0.0) then
          begin
          if seria=0 then czart.Series[seria].Add(wtmp,czas)
            else czart.Series[seria].Add(wtmp,czas);
          end;
       end
      else
       begin
          if seria=0 then czart.Series[seria].Add(wtmp,czas)
            else czart.Series[seria].Add(wtmp,czas);
       end;
  end;

end;

procedure TMainForm.btnPomiarClick(Sender: TObject);
var
  czas: string;
  tmpflo:Double;
begin
czas:=FormatDateTime('hh:nn:ss',now);

DodajPomiar(Chart1,czas,0,lblTW.Caption);
DodajPomiar(Chart1,czas,1,lblTZ.Caption);
DodajPomiar(Chart1,czas,2,lblCWU.Caption);
DodajPomiar(Chart1,czas,3,lblCO.Caption);
DodajPomiar(Chart1,czas,4,lblDZ.Caption);
DodajPomiar(Chart1,czas,5,lblALint.Caption);

// memo2.Lines.Add(Inttostr( Chart1.Series[0].Count));
// memo2.Lines.Add(Inttostr( Chart1.Series[1].Count));

end;

procedure TMainForm.Button1Click(Sender: TObject);
begin
//chart1.SaveToMetafileEnh('dane.xml');


with TSeriesDataXML.Create(Chart1) do
try
  SaveToFile('dane'+FormatDateTime('yyyymmdd_hhnn',now)+'.xml');
finally
  Free;
end;

end;

procedure TMainForm.Button2Click(Sender: TObject);
begin
Com1Timer.Enabled:=false;
 timer1.Enabled:=false;
end;

procedure TMainForm.Button3Click(Sender: TObject);
var
  c0,c1,c2,c3,c4,c5: integer;
begin
//memo1.Clear;
 c0:=StrToInt(FormatDateTime('ss',now)) div 3;

 timer1.Enabled:=true;

 if c0>10 then c0:=21-c0;
 
 c1:=(11+c0)*10;
 c2:=(17-c0)*10;
 c3:=(5+c0)*10;
 c4:=(22-c0)*8;
 c5:=(15+c0)*7;
 edit1.Text:=':0110'+IntToStr(c5);
  Application.ProcessMessages;
 sleep(200);
  Application.ProcessMessages;

 edit1.Text:=':0111'+IntToStr(c1);
  Application.ProcessMessages;
 sleep(200);
  Application.ProcessMessages;

 edit1.Text:=':0112'+IntToStr(c2);
  Application.ProcessMessages;
 sleep(200);
  Application.ProcessMessages;

 edit1.Text:=':0113'+IntToStr(c3);
  Application.ProcessMessages;
 sleep(200);
  Application.ProcessMessages;

 edit1.Text:=':0114'+IntToStr(c4);
  Application.ProcessMessages;

end;

procedure TMainForm.Button4Click(Sender: TObject);
var
 prt,ppt:integer;
begin
 com1.Set_rts(true);
 com1.Set_dtr(true);
 prt:=StrToIntDef(pretime.Text,0);
 if prt>0 then sleep(prt);
// Com1.WriteString(edit1.Text+#13#10,true);
 Com1.WriteString(memo4.Text+#13#10,true);
 ppt:=StrToIntDef(posttime.Text,0);
 if ppt>0 then sleep(ppt);
 com1.Set_dtr(false);
 com1.Set_rts(false);
end;

procedure TMainForm.btnLoadXmlClick(Sender: TObject);
var
    xx,yy,SeriesNode,PointNode,PointsNode,StartItemNode: IXMLNode;
   strTEXT,strY:String;
   s,n:integer;
   dblY:Double;
begin

try
 try
 if OD1.Execute=false then exit;

 memo3.Lines.LoadFromFile(OD1.FileName);
 var wiersz1:=memo3.Lines[0];
  if pos('UTF-16',wiersz1)>0 then
   begin
    wiersz1:=StringReplace(wiersz1,'UTF-16','UTF-8',[]);
    memo3.Lines[0]:=wiersz1;
    memo3.Lines.SaveToFile(OD1.FileName,TEncoding.UTF8);
   end;

   memo3.Clear;

    XMLDoc.FileName := OD1.FileName;
    XMLDoc.Active := true;
     n := XMLDoc.DocumentElement.ChildNodes.Count;
//     WriteLog('liczba: '+IntToStr(n));

    if cbClear.Checked then
      begin
        for s := 0 to chart1.SeriesCount - 1 do
           chart1.Series[s].Clear;
           chart1.Zoom.Active:=false;
      end;
    StartItemNode :=XMLDoc.DocumentElement;
    // XMLDoc.DocumentElement.ChildNodes['chart'];
    s:=0;
    SeriesNode := StartItemNode.ChildNodes.First;
  repeat
  if SeriesNode.HasAttribute('title')=true then
      begin
//        memo1.Lines.Add(IntToStr(s+1)+ ' seria:'+seriesnode.Attributes['title']);
//        chart1.Series[s].Color:=TColor(seriesnode.Attributes['color']);

      end;

//    chart1.AutoRepaint:=false;

    PointsNode := SeriesNode.ChildNodes.First;
    PointNode:=pointsnode.ChildNodes.First;
    n:=0;
    repeat
      if (PointNode.HasAttribute('text')=true) and (PointNode.HasAttribute('text')=true) then
        begin
          strTEXT := PointNode.Attributes['text'];
          dblY := PointNode.Attributes['Y'];
//      WriteLog('txt: ' + strtext + ', strY:'+ strY);
          chart1.Series[s].Add(dblY,strTEXT);
        end;

      if n mod 500=0 then
       begin
//          chart1.AutoRepaint:=true;
          application.ProcessMessages;
//          chart1.AutoRepaint:=false;
       end;
      inc(n);
      PointNode:= PointNode.NextSibling;
    until (PointNode = nil);

      SeriesNode:= SeriesNode.NextSibling;
      inc(s);
    until (SeriesNode = nil);

 except
  on e:Exception do WriteLog('B³¹d pobierania pozycji dokumentu: '+e.Message);
 end;

finally
 //
          chart1.AutoRepaint:=true;
          application.ProcessMessages;
end;
end;

procedure TmainForm.COM1DataReceived(Sender: TObject);
var
 stmp,xxs,p1,p2,dane,sTs:AnsiString;
 dbnrek,nrek,numer,n,x:integer;
 x1,x2,twrid,ilosc,dlen:integer;
 iddokoptimy:Integer;
 pokazZamkniete:boolean;
 poz_kropki:integer;
  jest: boolean;
  roz: string;
  wart: string;
  czas: string;
  m: integer;

label again;
label shwmenu;

begin
//// if count<5 then exit;


try
 try
again:

// WriteLog('A:'+FormatDateTime('hh:nn:ss.zzz',now));
 if NOSERIAL=false then p1:=COM1.ReadString(15,1)
  else p1:=edit1.Text;
// :0123<cr><lf>

// WriteLog('B:'+FormatDateTime('hh:nn:ss.zzz',now));


 p1:=trim(p1);

//if p1<>'' then memo3.Lines.Add(p1);

//if memo4.Lines.Count>100 then memo4.Clear;

 n:=pos(':',p1);
 if n>0 then p1:=copy(p1,n+1,99); //obetnij przed :

 m:=pos(':',p1);
 if m>0 then p1:=copy(p1,1,m-1); //obetnij po :

 m:=pos(':',p1);
 if m>0 then p1:=copy(p1,1,m-1); //obetnij po :

//p1:=stringreplace(p1,KONIEC_WIERSZA,'',[rfReplaceAll]);
 m:=pos(#13,p1);
if m>0 then p1:=copy(p1,1,m-1); //obetnij cr

 p1:=trim(p1);

 if (p1='') or (length(p1)<4) or (length(p1)>8) or ((m=0) and (n=0)) then exit;

 n:=memo1.Lines.Count;
 jest:=false;
 for x := 0 to n - 1 do
    begin
      if copy(memo1.Lines[x],1,4)=copy(p1,1,4) then
        begin
          jest:=true;
          if length(p1)>4 then
          //length(Memo1.Lines[x]) then
             begin
               Memo1.Lines[x]:=p1;


{
#define cmdsetrelays   ":0101"    0b=spr+cwu+dz, 0a=cwu+dz, 08=cwu
#define cmdgettempco   ":0114"             - CO
#define cmdgettempcwu  ":0113"           - CWU
#define cmdgettempdz   ":0112"              - Dolne ¿ród³o
#define cmdgettemppog  ":0111"           - pogodówka
#define cmdgettemppok  ":0110"           - czujnik pokojowy
#define cmdreadinput   ":0122"              - wejœcia zezwoleñ i alarmów


}
                roz:=trim(copy(p1,1,4));
                wart:=trim(copy(p1,5,99));
            if wart<>'' then
              begin
                if roz='0101' then
                                begin // PK
                                  if wart<>'0' then
                                      lblPK.Caption:=fixhexbit(wart);
                                end
                else if roz='0110' then
                                begin // wew
                                  lblTW.Caption:=fixtemp(wart);
                                end
                else if roz='0111' then
                                begin // zew
                                  lblTZ.Caption:=fixtemp(wart);
                                end
                else if roz='0112' then
                                begin // DZ
                                  lblDZ.Caption:=fixtemp(wart);
                                end
                else if roz='0113' then
                                begin // CWU
                                  lblCWU.Caption:=fixtemp(wart);
                                end
                else if roz='0114' then
                                begin // zas.CO
                                  lblCO.Caption:=fixtemp(wart);
                                end
                else if roz='0122' then
                                begin // AL
                                  lblAL.Caption:=fixbit(wart);
                                  lblALint.Caption:=wart;
                                end;

              end;

                czas:=FormatDateTime('hh:nn:ss',now);
                if (copy(czas,7,2)='00')
//                   or (copy(czas,7,2)='10')
                   or (copy(czas,7,2)='20')
//                   or (copy(czas,7,2)='30')
                   or (copy(czas,7,2)='40')
//                   or (copy(czas,7,2)='50')
                    then
                      begin
                        if oldczas<>czas then
                          begin
                            btnPomiar.click;
                            oldczas:=czas;
                          end;
                      end;

             end;
          break;
        end;
    end;
  if not jest then Memo1.Lines.Add(p1);


   except

   end;

 finally

 end;

end;

procedure TMainForm.FormCreate(Sender: TObject);
begin

   NOSERIAL:=False;
  try
    try                            // adres IP z pliku ip.txt
      COM1:=TSerialPort.Create;
      COM1.ComNumber:=1;
      COM1.BaudRate:=19200;
      COM1.Open;
      if Com1.Active=false then
         begin
           NOSERIAL:=true;
 WriteLog('!!!!! Nie mogê otworzyæ portu szeregowego.');
//           application.MessageBox('', '', MB_OK + MB_ICONERROR);
         end;
 Com1Timer.Enabled:=true;
    except
       on E: Exception do
        begin
          WriteLog('B³¹d otwierania portu szeregowego: '+e.Message);
//          application.MessageBox(PChar('B³¹d otwierania portu szeregowego: '+e.Message), '', MB_OK + MB_ICONERROR);
          NOSERIAL:=true;
        end;
    end;
  finally
    //
  end;


end;

procedure TMainForm.Memo2DblClick(Sender: TObject);
begin
Memo2.Clear;
end;

procedure TMainForm.Timer1Timer(Sender: TObject);
begin
button3.Click;
end;

end.
