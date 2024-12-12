object SetupDialog: TSetupDialog
  Left = 384
  Top = 216
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Einstellungen'
  ClientHeight = 241
  ClientWidth = 214
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 80
    Height = 13
    Caption = 'Autom. Tätigkeit:'
  end
  object Label2: TLabel
    Left = 8
    Top = 28
    Width = 56
    Height = 13
    Caption = 'Datenbank:'
  end
  object Label3: TLabel
    Left = 8
    Top = 112
    Width = 62
    Height = 13
    Caption = 'Mobil-Server:'
  end
  object Label4: TLabel
    Left = 8
    Top = 144
    Width = 22
    Height = 13
    Caption = 'Port:'
  end
  object Label5: TLabel
    Left = 8
    Top = 168
    Width = 62
    Height = 26
    Caption = 'In Infobreich minimieren:'
    WordWrap = True
  end
  object ButtonOK: TButton
    Left = 8
    Top = 208
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 4
    OnClick = ButtonOKClick
  end
  object ButtonCancel: TButton
    Left = 88
    Top = 208
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'A&bbruch'
    ModalResult = 2
    TabOrder = 5
    OnClick = ButtonCancelClick
  end
  object AutoActivityCheckBox: TCheckBox
    Left = 104
    Top = 8
    Width = 17
    Height = 17
    TabOrder = 0
  end
  object DatabaseRadioGroup: TRadioGroup
    Left = 96
    Top = 24
    Width = 105
    Height = 73
    Items.Strings = (
      'Automatisch'
      'Lokal'
      'Netzwerk')
    TabOrder = 1
  end
  object CheckBoxMobileServer: TCheckBox
    Left = 104
    Top = 112
    Width = 25
    Height = 17
    TabOrder = 2
  end
  object EditServerPort: TEdit
    Left = 104
    Top = 136
    Width = 89
    Height = 21
    TabOrder = 3
  end
  object CheckBoxInfoMinimize: TCheckBox
    Left = 104
    Top = 168
    Width = 97
    Height = 17
    TabOrder = 6
  end
end
