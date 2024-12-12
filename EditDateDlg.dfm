object EditDateDialog: TEditDateDialog
  Left = 369
  Top = 111
  Width = 424
  Height = 489
  BorderIcons = [biSystemMenu]
  Caption = 'Neuer Termin'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = True
  Position = poDefaultPosOnly
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 25
    Height = 13
    Caption = '&Start:'
    FocusControl = startDatePicker
  end
  object Label2: TLabel
    Left = 224
    Top = 8
    Width = 28
    Height = 13
    Caption = '&Ende:'
    FocusControl = endDatePicker
  end
  object Label3: TLabel
    Left = 8
    Top = 40
    Width = 23
    Height = 13
    Caption = '&Titel:'
    FocusControl = DBEditTitle
  end
  object Label4: TLabel
    Left = 8
    Top = 264
    Width = 68
    Height = 13
    Caption = 'Beschreibung:'
  end
  object Label5: TLabel
    Left = 8
    Top = 136
    Width = 29
    Height = 13
    Caption = '&Alarm:'
    FocusControl = AlarmBeforeDBEdit
  end
  object Label6: TLabel
    Left = 8
    Top = 72
    Width = 28
    Height = 13
    Caption = '&Prog.:'
    FocusControl = ProgramDBEdit
  end
  object Label7: TLabel
    Left = 8
    Top = 200
    Width = 45
    Height = 13
    Caption = 'A&ufwand:'
    FocusControl = EstEffortDBEdit
  end
  object Label8: TLabel
    Left = 104
    Top = 200
    Width = 52
    Height = 13
    Caption = '(geschätzt)'
  end
  object Label9: TLabel
    Left = 208
    Top = 200
    Width = 57
    Height = 13
    Caption = '(tatsächlich)'
  end
  object Label10: TLabel
    Left = 296
    Top = 200
    Width = 55
    Height = 13
    Caption = 'Erledigt (%):'
  end
  object Label11: TLabel
    Left = 8
    Top = 232
    Width = 43
    Height = 13
    Caption = 'Er&fordert:'
    FocusControl = RequiredDBLookupComboBox
  end
  object Label12: TLabel
    Left = 8
    Top = 168
    Width = 36
    Height = 13
    Caption = 'P&rojekt:'
    FocusControl = ProjectDBLookupComboBox
  end
  object Label13: TLabel
    Left = 8
    Top = 104
    Width = 17
    Height = 13
    Caption = '&Ort:'
    FocusControl = LocationDBEdit
  end
  object DBEditTitle: TDBEdit
    Left = 56
    Top = 40
    Width = 345
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    DataField = 'Title'
    DataSource = ScheduleSource
    TabOrder = 4
  end
  object DBMemoDescription: TDBMemo
    Left = 8
    Top = 280
    Width = 394
    Height = 136
    Anchors = [akLeft, akTop, akRight, akBottom]
    DataField = 'Description'
    DataSource = ScheduleSource
    TabOrder = 16
  end
  object ButtonOk: TButton
    Left = 8
    Top = 423
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 17
    OnClick = ButtonOkClick
  end
  object ButtonCancel: TButton
    Left = 88
    Top = 423
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Abbruch'
    TabOrder = 18
    OnClick = ButtonCancelClick
  end
  object AlarmBeforeDBEdit: TDBEdit
    Left = 56
    Top = 136
    Width = 49
    Height = 21
    DataField = 'ALARMBEFORE'
    DataSource = ScheduleSource
    TabOrder = 7
    OnChange = AlarmBeforeDBEditChange
  end
  object AlarmUnitComboBox: TDBComboBox
    Left = 112
    Top = 136
    Width = 145
    Height = 21
    Style = csDropDownList
    DataField = 'ALARMUNIT'
    DataSource = ScheduleSource
    ItemHeight = 13
    Items.Strings = (
      'Minuten'
      'Stunden'
      'Tage'
      'Wochen')
    TabOrder = 8
  end
  object ProgramDBEdit: TDBEdit
    Left = 56
    Top = 72
    Width = 345
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    DataField = 'Command'
    DataSource = ScheduleSource
    TabOrder = 5
  end
  object startDatePicker: TDateTimePicker
    Left = 56
    Top = 8
    Width = 81
    Height = 21
    CalAlignment = dtaLeft
    Date = 37104.8759152546
    Time = 37104.8759152546
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkDate
    ParseInput = False
    TabOrder = 0
  end
  object startTimePicker: TDateTimePicker
    Left = 136
    Top = 8
    Width = 65
    Height = 21
    CalAlignment = dtaLeft
    Date = 37104.8787606597
    Time = 37104.8787606597
    DateFormat = dfShort
    DateMode = dmUpDown
    Kind = dtkTime
    ParseInput = False
    TabOrder = 1
  end
  object endDatePicker: TDateTimePicker
    Left = 256
    Top = 8
    Width = 81
    Height = 21
    CalAlignment = dtaLeft
    Date = 37104.8759152546
    Time = 37104.8759152546
    DateFormat = dfShort
    DateMode = dmComboBox
    Kind = dtkDate
    ParseInput = False
    TabOrder = 2
  end
  object endTimePicker: TDateTimePicker
    Left = 336
    Top = 8
    Width = 65
    Height = 21
    CalAlignment = dtaLeft
    Date = 37104.8787606597
    Time = 37104.8787606597
    DateFormat = dfShort
    DateMode = dmUpDown
    Kind = dtkTime
    ParseInput = False
    TabOrder = 3
  end
  object EstEffortDBEdit: TDBEdit
    Left = 56
    Top = 200
    Width = 49
    Height = 21
    DataField = 'EstEffort'
    DataSource = ScheduleSource
    TabOrder = 10
  end
  object ActEffortDBEdit: TDBEdit
    Left = 160
    Top = 200
    Width = 49
    Height = 21
    DataField = 'ActEffort'
    DataSource = ScheduleSource
    TabOrder = 11
  end
  object PercentCompletedDBEdit: TDBEdit
    Left = 352
    Top = 200
    Width = 49
    Height = 21
    DataField = 'PercentCompleted'
    DataSource = ScheduleSource
    TabOrder = 13
  end
  object RequiredDBLookupComboBox: TDBLookupComboBox
    Left = 56
    Top = 232
    Width = 145
    Height = 21
    DataField = 'RequiredTask'
    DataSource = ScheduleSource
    KeyField = 'id'
    ListField = 'title'
    ListSource = RequiredSource
    TabOrder = 14
  end
  object ButtonNoTask: TButton
    Left = 208
    Top = 232
    Width = 97
    Height = 25
    Caption = 'Keine Aufgabe'
    TabOrder = 15
    OnClick = ButtonNoTaskClick
  end
  object _EffortUnitComboBox: TDBComboBox
    Left = 264
    Top = 200
    Width = 33
    Height = 21
    Style = csDropDownList
    DataField = 'EFFORTUNIT'
    DataSource = ScheduleSource
    ItemHeight = 13
    Items.Strings = (
      'h'
      'd')
    TabOrder = 12
  end
  object ProjectDBLookupComboBox: TDBLookupComboBox
    Left = 56
    Top = 168
    Width = 345
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    DataField = 'Project'
    DataSource = ScheduleSource
    KeyField = 'id'
    ListField = 'name'
    ListSource = ProjectSource
    TabOrder = 9
  end
  object LocationDBEdit: TDBEdit
    Left = 56
    Top = 104
    Width = 345
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    DataField = 'Location'
    DataSource = ScheduleSource
    TabOrder = 6
  end
  object ScheduleSource: TDataSource
    DataSet = ScheduleTable
    Left = 32
    Top = 296
  end
  object RequiredQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select'#9'id, title'
      'from'#9'schedule'
      'where'#9'(recurringid is null or recurringid <= 0) and'
      #9#9'estEffort > 0 and'
      #9#9'userId = :theUser'
      'order by title'
      ' ')
    Left = 128
    Top = 360
    ParamData = <
      item
        DataType = ftUnknown
        Name = 'theUser'
        ParamType = ptUnknown
      end>
  end
  object RequiredSource: TDataSource
    DataSet = RequiredQuery
    Left = 200
    Top = 288
  end
  object ProjectSource: TDataSource
    DataSet = ProjectQuery
    Left = 280
    Top = 296
  end
  object ProjectQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select'#9'id, name'
      'from'#9'project'
      'order by name'
      ' ')
    Left = 224
    Top = 360
  end
  object ScheduleTable: TTable
    BeforePost = ScheduleTableBeforePost
    DatabaseName = 'SchedulerDB'
    TableName = 'SCHEDULE'
    Left = 24
    Top = 368
    object ScheduleTableID: TIntegerField
      FieldName = 'ID'
      Required = True
    end
    object ScheduleTableUSERID: TIntegerField
      FieldName = 'USERID'
      Required = True
    end
    object ScheduleTableSTARTDATE: TDateTimeField
      FieldName = 'STARTDATE'
      Required = True
    end
    object ScheduleTableENDDATE: TDateTimeField
      FieldName = 'ENDDATE'
      Required = True
    end
    object ScheduleTableALARMDATE: TDateTimeField
      FieldName = 'ALARMDATE'
    end
    object ScheduleTablePROJECT: TIntegerField
      FieldName = 'PROJECT'
    end
    object ScheduleTableESTEFFORT: TFloatField
      FieldName = 'ESTEFFORT'
    end
    object ScheduleTableACTEFFORT: TFloatField
      FieldName = 'ACTEFFORT'
    end
    object ScheduleTableEFFORTUNIT: TStringField
      FieldName = 'EFFORTUNIT'
      Size = 1
    end
    object ScheduleTablePERCENTCOMPLETED: TIntegerField
      FieldName = 'PERCENTCOMPLETED'
    end
    object ScheduleTableREQUIREDTASK: TIntegerField
      FieldName = 'REQUIREDTASK'
    end
    object ScheduleTableTITLE: TStringField
      FieldName = 'TITLE'
      Required = True
      Size = 64
    end
    object ScheduleTableDESCRIPTION: TStringField
      FieldName = 'DESCRIPTION'
      Size = 255
    end
    object ScheduleTableLOCATION: TStringField
      FieldName = 'LOCATION'
      Size = 64
    end
    object ScheduleTableCOMMAND: TStringField
      FieldName = 'COMMAND'
      Size = 255
    end
    object ScheduleTableALARMBEFORE: TIntegerField
      FieldName = 'ALARMBEFORE'
    end
    object ScheduleTableALARMUNIT: TStringField
      FieldName = 'ALARMUNIT'
      Size = 8
    end
    object ScheduleTableRECURRINGID: TIntegerField
      FieldName = 'RECURRINGID'
    end
  end
end
