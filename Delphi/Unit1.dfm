object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'MainForm'
  ClientHeight = 715
  ClientWidth = 1230
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 425
    Top = 14
    Width = 96
    Height = 19
    Caption = 'Temp CWU:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblCWU: TLabel
    Left = 602
    Top = 14
    Width = 35
    Height = 19
    Alignment = taRightJustify
    Caption = '15,5'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblCO: TLabel
    Left = 602
    Top = 36
    Width = 35
    Height = 19
    Alignment = taRightJustify
    Caption = '15,5'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 425
    Top = 36
    Width = 160
    Height = 19
    Caption = 'Temp zas.CO/CWU:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblDZ: TLabel
    Left = 602
    Top = 58
    Width = 35
    Height = 19
    Alignment = taRightJustify
    Caption = '15,5'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 425
    Top = 58
    Width = 79
    Height = 19
    Caption = 'Temp DZ:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label9: TLabel
    Left = 220
    Top = 14
    Width = 91
    Height = 19
    Caption = 'Temp POK:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblTW: TLabel
    Left = 322
    Top = 14
    Width = 35
    Height = 19
    Alignment = taRightJustify
    Caption = '15,5'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblTZ: TLabel
    Left = 322
    Top = 36
    Width = 35
    Height = 19
    Alignment = taRightJustify
    Caption = '15,5'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label12: TLabel
    Left = 220
    Top = 36
    Width = 93
    Height = 19
    Caption = 'Temp ZEW:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 220
    Top = 58
    Width = 28
    Height = 19
    Caption = 'PK:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblPK: TLabel
    Left = 254
    Top = 58
    Width = 117
    Height = 19
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 220
    Top = 80
    Width = 27
    Height = 19
    Caption = 'AL:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblAL: TLabel
    Left = 254
    Top = 80
    Width = 117
    Height = 19
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lblALint: TLabel
    Left = 377
    Top = 83
    Width = 42
    Height = 19
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Memo1: TMemo
    Left = 20
    Top = 25
    Width = 176
    Height = 226
    ScrollBars = ssBoth
    TabOrder = 0
  end
  object Chart1: TChart
    Left = 212
    Top = 115
    Width = 1002
    Height = 592
    Gradient.Visible = True
    Legend.Alignment = laBottom
    Legend.CheckBoxes = True
    Legend.Color = 8454143
    Legend.CustomPosition = True
    Legend.Font.Style = [fsBold]
    Legend.Font.Shadow.Visible = False
    Legend.FontSeriesColor = True
    Legend.Frame.Visible = False
    Legend.Left = 100
    Legend.LeftPercent = 10
    Legend.LegendStyle = lsSeries
    Legend.PositionUnits = muPercent
    Legend.ResizeChart = False
    Legend.Shadow.Visible = False
    Legend.Symbol.Visible = False
    Legend.Top = 568
    Legend.TopPercent = 96
    Legend.Transparent = True
    Title.Text.Strings = (
      'Temperatury')
    Title.Visible = False
    BottomAxis.AxisValuesFormat = '#.#'
    LeftAxis.AxisValuesFormat = '#.#'
    LeftAxis.MaximumOffset = 30
    LeftAxis.MinimumOffset = 30
    SeriesGroups = <
      item
        Name = 'Group1'
      end>
    View3D = False
    View3DWalls = False
    Zoom.Animated = True
    Zoom.Pen.Width = 2
    Align = alCustom
    Color = clWhite
    AutoSize = True
    TabOrder = 1
    Anchors = [akLeft, akTop, akRight, akBottom]
    DefaultCanvas = 'TGDIPlusCanvas'
    ColorPaletteIndex = 6
    object Memo3: TMemo
      Left = 90
      Top = 70
      Width = 211
      Height = 321
      Lines.Strings = (
        'Memo3')
      TabOrder = 0
      Visible = False
      WordWrap = False
    end
    object Memo4: TMemo
      Left = 385
      Top = 95
      Width = 266
      Height = 241
      Lines.Strings = (
        ':010126'
        ':0110'
        ':0111'
        ':0112'
        ':0113'
        ':0114'
        ':0122')
      TabOrder = 1
      Visible = False
    end
    object Series1: TLineSeries
      PercentFormat = '##0.##,%'
      SeriesColor = clRed
      Title = 'W domu'
      Brush.BackColor = clDefault
      Dark3D = False
      LinePen.Width = 2
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      Data = {0000000000}
    end
    object Series2: TLineSeries
      SeriesColor = 52736
      Title = 'Na zewn'#261'trz'
      Brush.BackColor = clDefault
      Dark3D = False
      LinePen.Width = 2
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      Data = {0000000000}
    end
    object Series3: TLineSeries
      SeriesColor = clFuchsia
      Title = 'CWU'
      Brush.BackColor = clDefault
      Dark3D = False
      LinePen.Width = 2
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      Data = {0000000000}
    end
    object Series4: TLineSeries
      PercentFormat = '##0.##,%'
      SeriesColor = 16744576
      Title = 'Zasilanie CO/CWU'
      Brush.BackColor = clDefault
      Dark3D = False
      LinePen.Width = 2
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      Data = {0000000000}
    end
    object Series5: TLineSeries
      SeriesColor = clSilver
      Title = 'Dolne '#378'r'#243'd'#322'o'
      Brush.BackColor = clDefault
      Dark3D = False
      LinePen.Width = 2
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
      Data = {0000000000}
    end
    object Series6: TLineSeries
      Title = 'Alarmy'
      Brush.BackColor = clDefault
      Dark3D = False
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Y'
      YValues.Order = loNone
    end
  end
  object btnPomiar: TButton
    Left = 688
    Top = 58
    Width = 96
    Height = 25
    Caption = 'Zmierz teraz'
    TabOrder = 2
    OnClick = btnPomiarClick
  end
  object Button2: TButton
    Left = 680
    Top = 8
    Width = 96
    Height = 25
    Caption = 'Wy'#322#261'cz pomiary'
    TabOrder = 3
    OnClick = Button2Click
  end
  object Memo2: TMemo
    Left = 8
    Top = 257
    Width = 188
    Height = 299
    Lines.Strings = (
      'Memo2')
    ScrollBars = ssVertical
    TabOrder = 4
    OnDblClick = Memo2DblClick
  end
  object Button1: TButton
    Left = 790
    Top = 60
    Width = 75
    Height = 25
    Caption = 'Zapisz dane'
    TabOrder = 5
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 790
    Top = 8
    Width = 135
    Height = 21
    TabOrder = 6
    Text = ':0111231'
  end
  object Button3: TButton
    Left = 931
    Top = 8
    Width = 56
    Height = 25
    Caption = 'Emulacja'
    TabOrder = 7
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 817
    Top = 29
    Width = 75
    Height = 25
    Caption = 'Wy'#347'lij rozkaz'
    TabOrder = 8
    OnClick = Button4Click
  end
  object btnLoadXml: TButton
    Left = 871
    Top = 60
    Width = 102
    Height = 25
    Caption = #321'aduj dane z XML'
    TabOrder = 9
    OnClick = btnLoadXmlClick
  end
  object cbClear: TCheckBox
    Left = 876
    Top = 91
    Width = 111
    Height = 17
    Caption = 'Wyczy'#347#263' najpierw'
    Checked = True
    State = cbChecked
    TabOrder = 10
  end
  object pretime: TEdit
    Left = 790
    Top = 33
    Width = 21
    Height = 21
    TabOrder = 11
    Text = '1'
  end
  object posttime: TEdit
    Left = 896
    Top = 33
    Width = 30
    Height = 21
    TabOrder = 12
    Text = '5'
  end
  object COM1Timer: TTimer
    Interval = 10
    OnTimer = COM1DataReceived
    Left = 740
    Top = 115
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 3400
    OnTimer = Timer1Timer
    Left = 925
    Top = 65
  end
  object ChartDataSet1: TChartDataSet
    Active = True
    Chart = Chart1
    Left = 624
    Top = 100
  end
  object XMLDoc: TXMLDocument
    Left = 678
    Top = 102
    DOMVendorDesc = 'MSXML'
  end
  object OD1: TOpenDialog
    DefaultExt = 'xml'
    Filter = 'Pliki XML|*.xml'
    Options = [ofEnableSizing]
    Left = 685
    Top = 30
  end
end
