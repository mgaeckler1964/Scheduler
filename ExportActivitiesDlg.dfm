object ExportActivitiesDialog: TExportActivitiesDialog
  Left = 344
  Top = 207
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Export Stundenabrechnung'
  ClientHeight = 243
  ClientWidth = 389
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 24
    Width = 36
    Height = 13
    Caption = '&Projekt:'
  end
  object Label2: TLabel
    Left = 8
    Top = 56
    Width = 33
    Height = 13
    Caption = '&Monat:'
  end
  object Label3: TLabel
    Left = 8
    Top = 120
    Width = 28
    Height = 13
    Caption = 'Datei:'
  end
  object Label4: TLabel
    Left = 8
    Top = 152
    Width = 41
    Height = 13
    Caption = 'Runden:'
  end
  object ProjectCombo: TComboBox
    Left = 56
    Top = 16
    Width = 240
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 0
    OnChange = ValueChange
  end
  object MonthCombo: TComboBox
    Left = 56
    Top = 48
    Width = 186
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 1
    OnChange = ValueChange
    Items.Strings = (
      'Januar'
      'Februar'
      'März'
      'April'
      'Mai'
      'Juni'
      'Juli'
      'August'
      'September'
      'Oktober'
      'November'
      'Dezember')
  end
  object YearSelect: TUpDown
    Left = 280
    Top = 48
    Width = 16
    Height = 21
    Anchors = [akTop, akRight]
    Associate = YearEdit
    Min = 1900
    Max = 2999
    Position = 2000
    TabOrder = 3
    Thousands = False
    Wrap = False
  end
  object YearEdit: TEdit
    Left = 246
    Top = 48
    Width = 34
    Height = 21
    Anchors = [akTop, akRight]
    ReadOnly = True
    TabOrder = 2
    Text = '2000'
    OnChange = ValueChange
  end
  object FileEdit: TEdit
    Left = 56
    Top = 112
    Width = 240
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 6
  end
  object ButtonBrowse: TButton
    Left = 304
    Top = 112
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = '&Durchsuchen'
    TabOrder = 7
    OnClick = ButtonBrowseClick
  end
  object ButtonOk: TButton
    Left = 8
    Top = 212
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = '&OK'
    Default = True
    ModalResult = 1
    TabOrder = 11
    OnClick = ButtonOkClick
  end
  object ButtonCancel: TButton
    Left = 88
    Top = 212
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = '&Abbruch'
    ModalResult = 2
    TabOrder = 12
  end
  object DeleteCheckBox: TCheckBox
    Left = 144
    Top = 176
    Width = 60
    Height = 17
    Anchors = [akTop]
    Caption = 'Löschen'
    TabOrder = 10
  end
  object RoundComboBox: TComboBox
    Left = 56
    Top = 144
    Width = 240
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 8
    Items.Strings = (
      '0'
      '1'
      '5'
      '10'
      '15'
      '30'
      '60')
  end
  object TransferCheckBox: TCheckBox
    Left = 56
    Top = 176
    Width = 75
    Height = 17
    Anchors = [akTop]
    Caption = 'Übertragen'
    TabOrder = 9
  end
  object DateTimePickerStart: TDateTimePicker
    Left = 56
    Top = 80
    Width = 116
    Height = 21
    CalAlignment = dtaLeft
    Date = 40226.6792785995
    Time = 40226.6792785995
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkDate
    ParseInput = False
    TabOrder = 4
  end
  object DateTimePickerEnd: TDateTimePicker
    Left = 180
    Top = 80
    Width = 116
    Height = 21
    CalAlignment = dtaLeft
    Date = 40226.6792785995
    Time = 40226.6792785995
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkDate
    ParseInput = False
    TabOrder = 5
  end
  object ProjectQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select'#9'distinct name'
      'from'#9'project')
    UniDirectional = True
    Left = 264
    Top = 192
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'csv'
    Filter = 'CSV-Dateien(*.csv)|*.csv'
    Title = 'Exportdatei'
    Left = 208
    Top = 192
  end
end
