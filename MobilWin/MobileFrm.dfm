object MobileForm: TMobileForm
  Left = 462
  Top = 560
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Mobile Zeiterfassung'
  ClientHeight = 137
  ClientWidth = 502
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 72
    Height = 13
    Caption = 'Aufgabendatei:'
  end
  object Label2: TLabel
    Left = 8
    Top = 40
    Width = 79
    Height = 13
    Caption = 'Tätigkeitendatei:'
  end
  object Label3: TLabel
    Left = 8
    Top = 72
    Width = 84
    Height = 13
    Caption = 'Aktuelle Aufgabe:'
  end
  object ClockLabel: TLabel
    Left = 8
    Top = 88
    Width = 42
    Height = 13
    Caption = '00:00:00'
  end
  object TaskFileEdit: TEdit
    Left = 104
    Top = 8
    Width = 289
    Height = 21
    TabOrder = 0
  end
  object ActivityFileEdit: TEdit
    Left = 104
    Top = 40
    Width = 289
    Height = 21
    TabOrder = 1
  end
  object TasksComboBox: TComboBox
    Left = 104
    Top = 72
    Width = 393
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 2
  end
  object Button1: TButton
    Left = 400
    Top = 8
    Width = 97
    Height = 25
    Caption = 'Durchsuchen...'
    TabOrder = 3
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 400
    Top = 40
    Width = 97
    Height = 25
    Caption = 'Durchsuchen...'
    TabOrder = 4
    OnClick = Button2Click
  end
  object StartButton: TButton
    Left = 104
    Top = 104
    Width = 75
    Height = 25
    Caption = 'Start'
    Enabled = False
    TabOrder = 5
    OnClick = StartButtonClick
  end
  object StopButton: TButton
    Left = 184
    Top = 104
    Width = 75
    Height = 25
    Caption = 'Stop'
    Enabled = False
    TabOrder = 6
    OnClick = StopButtonClick
  end
  object CloseButton: TButton
    Left = 264
    Top = 104
    Width = 75
    Height = 25
    Caption = 'Beenden'
    TabOrder = 7
    OnClick = CloseButtonClick
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'CSV'
    Filter = 'Mobile Erfassung (*.csv)|*.csv'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Title = 'Export...'
    Left = 464
    Top = 40
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'csv'
    Filter = 'Mobile Erfassung (*.csv)|*.csv'
    Title = 'Import...'
    Left = 464
  end
  object ClockTimer: TTimer
    Enabled = False
    OnTimer = ClockTimerTimer
    Left = 48
    Top = 96
  end
end
