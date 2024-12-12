object RecurringForm: TRecurringForm
  Tag = 3
  Left = 660
  Top = 568
  Width = 571
  Height = 498
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = 'Regelmäßge Termine'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  OldCreateOrder = True
  Position = poDefaultPosOnly
  Visible = True
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 136
    Top = 40
    Width = 69
    Height = 13
    Caption = 'Wiederholung:'
  end
  object DayLabel: TLabel
    Left = 136
    Top = 64
    Width = 59
    Height = 13
    Caption = 'Wochentag:'
  end
  object MonthLabel: TLabel
    Left = 136
    Top = 88
    Width = 33
    Height = 13
    Caption = 'Monat:'
  end
  object Bevel1: TBevel
    Left = 136
    Top = 120
    Width = 418
    Height = 1
    Anchors = [akLeft, akTop, akRight]
  end
  object DayMonthLabel: TLabel
    Left = 264
    Top = 91
    Width = 63
    Height = 13
    Caption = 'eines Monats'
  end
  object Label11: TLabel
    Left = 384
    Top = 40
    Width = 36
    Height = 13
    Caption = 'Uhrzeit:'
  end
  object Label2: TLabel
    Left = 360
    Top = 402
    Width = 70
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Tage/Stunden'
  end
  object Label3: TLabel
    Left = 136
    Top = 402
    Width = 32
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Dauer:'
  end
  object Label4: TLabel
    Left = 136
    Top = 216
    Width = 68
    Height = 13
    Caption = 'Beschreibung:'
  end
  object Label5: TLabel
    Left = 136
    Top = 136
    Width = 23
    Height = 13
    Caption = 'Titel:'
  end
  object Label6: TLabel
    Left = 136
    Top = 434
    Width = 29
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Alarm:'
  end
  object Label7: TLabel
    Left = 136
    Top = 160
    Width = 50
    Height = 13
    Caption = 'Programm:'
  end
  object Label8: TLabel
    Left = 136
    Top = 184
    Width = 17
    Height = 13
    Caption = 'Ort:'
  end
  object RepeatComboBox: TComboBox
    Left = 232
    Top = 40
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 3
    OnChange = ComboBoxChange
    Items.Strings = (
      'Täglich'
      'Wöchentlich'
      'Zweiwöchentlich'
      'Monatlich'
      'Jährlich')
  end
  object WeekDayComboBox: TComboBox
    Left = 384
    Top = 64
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 7
    OnChange = ComboBoxChange
    Items.Strings = (
      'Sonntag'
      'Montag'
      'Dienstag'
      'Mittwoch'
      'Donnerstag'
      'Freitag'
      'Samstag')
  end
  object CounterComboBox: TComboBox
    Left = 232
    Top = 64
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
    OnChange = ComboBoxChange
    Items.Strings = (
      ''
      'ersten'
      'zweiten'
      'dritten'
      'vierten'
      'letzten'
      'vorletzten')
  end
  object MonthComboBox: TComboBox
    Left = 384
    Top = 88
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 9
    OnChange = ComboBoxChange
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
  object DBGridTitleList: TDBGrid
    Left = 0
    Top = 0
    Width = 129
    Height = 459
    TabStop = False
    Align = alLeft
    DataSource = RecurringSource
    TabOrder = 0
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    Columns = <
      item
        Expanded = False
        FieldName = 'Title'
        Title.Caption = 'Titel'
        Visible = True
      end>
  end
  object MonthDayEdit: TDBEdit
    Left = 232
    Top = 88
    Width = 25
    Height = 21
    DataField = 'MonatsTag'
    DataSource = RecurringSource
    TabOrder = 8
  end
  object DBNavigator: TDBNavigator
    Left = 136
    Top = 8
    Width = 230
    Height = 25
    DataSource = RecurringSource
    TabOrder = 1
  end
  object DBEditLetzterEintrag: TDBEdit
    Left = 376
    Top = 8
    Width = 153
    Height = 21
    TabStop = False
    DataField = 'LetzterEintrag'
    DataSource = RecurringSource
    ReadOnly = True
    TabOrder = 2
  end
  object DBEditTitle: TDBEdit
    Left = 232
    Top = 136
    Width = 314
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    DataField = 'Title'
    DataSource = RecurringSource
    TabOrder = 10
  end
  object DBMemoDescription: TDBMemo
    Left = 232
    Top = 208
    Width = 314
    Height = 187
    Anchors = [akLeft, akTop, akRight, akBottom]
    DataField = 'Description'
    DataSource = RecurringSource
    TabOrder = 13
  end
  object DBEditDauerTage: TDBEdit
    Left = 232
    Top = 402
    Width = 41
    Height = 21
    Anchors = [akLeft, akBottom]
    DataField = 'DauerTage'
    DataSource = RecurringSource
    TabOrder = 14
  end
  object DBEditAlarmBefore: TDBEdit
    Left = 232
    Top = 434
    Width = 41
    Height = 21
    Anchors = [akLeft, akBottom]
    DataField = 'AlarmBefore'
    DataSource = RecurringSource
    TabOrder = 16
  end
  object AlarmUnitComboBox: TDBComboBox
    Left = 280
    Top = 434
    Width = 145
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akBottom]
    DataField = 'AlarmUnit'
    DataSource = RecurringSource
    ItemHeight = 13
    Items.Strings = (
      'Minuten'
      'Stunden'
      'Tage'
      'Wochen')
    TabOrder = 17
  end
  object sTimePicker: TDateTimePicker
    Left = 424
    Top = 40
    Width = 105
    Height = 21
    CalAlignment = dtaLeft
    Date = 36495.0237789352
    Time = 36495.0237789352
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkTime
    ParseInput = False
    TabOrder = 4
    OnChange = sTimePickerChange
  end
  object eTimePicker: TDateTimePicker
    Left = 280
    Top = 402
    Width = 73
    Height = 21
    Anchors = [akLeft, akBottom]
    CalAlignment = dtaLeft
    Date = 36495.0237789352
    Time = 36495.0237789352
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkTime
    ParseInput = False
    TabOrder = 15
    OnChange = sTimePickerChange
  end
  object DBEditCommand: TDBEdit
    Left = 232
    Top = 160
    Width = 314
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    DataField = 'Command'
    DataSource = RecurringSource
    TabOrder = 11
  end
  object WeekComboBox: TComboBox
    Left = 232
    Top = 64
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 5
    OnChange = ComboBoxChange
    Items.Strings = (
      'gerade'
      'ungerade')
  end
  object DBEditLocation: TDBEdit
    Left = 232
    Top = 184
    Width = 314
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    DataField = 'Location'
    DataSource = RecurringSource
    TabOrder = 12
  end
  object RecurringSource: TDataSource
    DataSet = RecurringQuery
    Left = 24
    Top = 32
  end
  object RecurringQuery: TQuery
    AfterInsert = RecurringQueryAfterInsert
    BeforePost = RecurringQueryBeforePost
    AfterPost = RecurringQueryAfterPost
    AfterCancel = RecurringQueryAfterCancel
    BeforeDelete = RecurringQueryBeforeDelete
    AfterScroll = RecurringQueryAfterScroll
    DatabaseName = 'SchedulerDB'
    RequestLive = True
    SQL.Strings = (
      'select *'
      'from RECURRING'
      'where  USERID = :theUser'
      'order by WIEDERHOLUNG, MONAT, MONATSTAG, WOCHENTAG'
      ' '
      ' ')
    Left = 24
    Top = 88
    ParamData = <
      item
        DataType = ftInteger
        Name = 'theUser'
        ParamType = ptUnknown
        Value = 0
      end>
  end
end
