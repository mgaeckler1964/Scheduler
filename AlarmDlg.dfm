object AlarmDialog: TAlarmDialog
  Left = 595
  Top = 349
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Alarm'
  ClientHeight = 144
  ClientWidth = 295
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = True
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object AlarmText: TLabel
    Left = 8
    Top = 16
    Width = 47
    Height = 13
    Caption = 'AlarmText'
  end
  object Label1: TLabel
    Left = 8
    Top = 88
    Width = 78
    Height = 13
    Caption = '&Erneut vorlegen:'
    FocusControl = RedoEdit
  end
  object Button1: TButton
    Left = 8
    Top = 112
    Width = 75
    Height = 25
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object RedoEdit: TEdit
    Left = 96
    Top = 88
    Width = 65
    Height = 21
    TabOrder = 0
  end
  object RedoUnitCombo: TComboBox
    Left = 168
    Top = 88
    Width = 113
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
    Items.Strings = (
      'Minuten'
      'Stunden'
      'Tagen'
      'Wochen')
  end
  object Button2: TButton
    Left = 88
    Top = 112
    Width = 75
    Height = 25
    Cancel = True
    Caption = '&Abbruch'
    ModalResult = 2
    TabOrder = 3
  end
end
