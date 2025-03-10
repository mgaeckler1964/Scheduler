object ActivityForm: TActivityForm
  Tag = 1
  Left = 2030
  Top = 300
  Width = 581
  Height = 495
  Caption = 'Tätigkeiten'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDefaultPosOnly
  Visible = True
  OnActivate = FormActivate
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 565
    Height = 73
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object DBNavigator1: TDBNavigator
      Left = 8
      Top = 40
      Width = 112
      Height = 25
      DataSource = ActivitySqlSource
      VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
      TabOrder = 5
    end
    object RestartButton: TButton
      Left = 290
      Top = 40
      Width = 55
      Height = 25
      Caption = 'Neustart'
      TabOrder = 10
      OnClick = RestartButtonClick
    end
    object StartButton: TButton
      Left = 120
      Top = 40
      Width = 33
      Height = 25
      Caption = 'Start'
      TabOrder = 6
      OnClick = StartButtonClick
    end
    object StopButton: TButton
      Left = 153
      Top = 40
      Width = 33
      Height = 25
      Caption = 'Stop'
      Enabled = False
      TabOrder = 7
      OnClick = StopButtonClick
    end
    object DeleteButton: TButton
      Left = 186
      Top = 40
      Width = 47
      Height = 25
      Caption = 'Löschen'
      TabOrder = 8
      OnClick = DeleteButtonClick
    end
    object SaveButton: TButton
      Left = 233
      Top = 40
      Width = 57
      Height = 25
      Caption = 'Speichern'
      TabOrder = 9
      OnClick = SaveButtonClick
    end
    object StartTimePicker: TDateTimePicker
      Left = 316
      Top = 8
      Width = 81
      Height = 21
      Anchors = [akTop, akRight]
      CalAlignment = dtaLeft
      Date = 38596.9668008449
      Time = 38596.9668008449
      DateFormat = dfShort
      DateMode = dmUpDown
      Kind = dtkTime
      ParseInput = False
      TabOrder = 2
    end
    object StartDatePicker: TDateTimePicker
      Left = 236
      Top = 8
      Width = 81
      Height = 21
      Anchors = [akTop, akRight]
      CalAlignment = dtaLeft
      Date = 37104
      Time = 37104
      DateFormat = dfShort
      DateMode = dmUpDown
      Kind = dtkDate
      ParseInput = False
      TabOrder = 1
    end
    object EndDatePicker: TDateTimePicker
      Left = 404
      Top = 8
      Width = 81
      Height = 21
      Anchors = [akTop, akRight]
      CalAlignment = dtaLeft
      Date = 37104
      Time = 37104
      DateFormat = dfShort
      DateMode = dmUpDown
      Kind = dtkDate
      ParseInput = False
      TabOrder = 3
    end
    object EndTimePicker: TDateTimePicker
      Left = 484
      Top = 8
      Width = 81
      Height = 21
      Anchors = [akTop, akRight]
      CalAlignment = dtaLeft
      Date = 38596.9699983681
      Time = 38596.9699983681
      DateFormat = dfShort
      DateMode = dmUpDown
      Kind = dtkTime
      ParseInput = False
      TabOrder = 4
    end
    object TaskComboBox: TComboBox
      Left = 8
      Top = 8
      Width = 213
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      ItemHeight = 13
      TabOrder = 0
    end
  end
  object DBGridActivities: TDBGrid
    Left = 0
    Top = 73
    Width = 565
    Height = 363
    Align = alClient
    DataSource = ActivitySqlSource
    ReadOnly = True
    TabOrder = 1
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    OnDrawColumnCell = DBGridActivitiesDrawColumnCell
    Columns = <
      item
        Expanded = False
        FieldName = 'StartTime'
        Title.Caption = 'Anfang'
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'EndTime'
        Title.Caption = 'Ende'
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'projectName'
        Title.Caption = 'Projekt'
        Width = 200
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'title'
        Title.Caption = 'Aufgabe'
        Width = 300
        Visible = True
      end>
  end
  object TaskSource: TDataSource
    DataSet = TaskQuery
    Left = 24
    Top = 120
  end
  object TaskQuery: TQuery
    AfterOpen = TaskQueryAfterOpen
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select'#9's.id, p.name || '#39'-'#39' || s.title as title'
      'from'#9'schedule s, project p'
      'where   p.id = s.project and'
      #9#9'(s.recurringid is null or s.recurringid <= 0) and'
      #9#9'(s.estEffort is not null and s.estEffort > 0) and'
      #9#9's.userId = :theUser'
      'order by title'
      ' '
      ' '
      ' '
      ' ')
    Left = 24
    Top = 184
    ParamData = <
      item
        DataType = ftInteger
        Name = 'theUser'
        ParamType = ptInput
        Value = 0
      end>
    object TaskQueryid: TIntegerField
      FieldName = 'id'
      Origin = 'SCHEDULERDB."schedule.DB".Id'
    end
    object TaskQuerytitle: TStringField
      FieldName = 'title'
      Origin = 'SCHEDULERDB."project.DB".Name'
      Size = 129
    end
  end
  object ActivityQuery: TQuery
    CachedUpdates = True
    AfterScroll = ActivityQueryAfterScroll
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select'#9'a.ID, a.Task, a.StartTime, a.EndTime, a.Transfered,'
      #9#9's.title,'
      #9#9'p.name as projectName, p.id as projectID'
      'from'#9'activities a, schedule s, project p'
      'where'#9'a.task = s.id and'
      #9#9's.userId = :theUser and'
      #9#9's.project = p.id'
      'order by a.StartTime desc'
      ''
      ''
      ' '
      ' '
      ' '
      ' '
      ' ')
    UpdateMode = upWhereKeyOnly
    Left = 144
    Top = 184
    ParamData = <
      item
        DataType = ftInteger
        Name = 'theUser'
        ParamType = ptInput
        Value = 0
      end>
    object ActivityQueryID: TIntegerField
      FieldName = 'ID'
      Origin = 'SCHEDULERDB."activities.DB".ID'
      Required = True
    end
    object ActivityQueryTask: TIntegerField
      FieldName = 'Task'
      Origin = 'SCHEDULERDB."activities.DB".Task'
    end
    object ActivityQueryStartTime: TDateTimeField
      FieldName = 'StartTime'
      Origin = 'SCHEDULERDB."activities.DB".StartTime'
    end
    object ActivityQueryEndTime: TDateTimeField
      FieldName = 'EndTime'
      Origin = 'SCHEDULERDB."activities.DB".EndTime'
    end
    object ActivityQuerytitle: TStringField
      FieldName = 'title'
      Origin = 'SCHEDULERDB."schedule.DB".Title'
      Size = 64
    end
    object ActivityQueryprojectName: TStringField
      FieldName = 'projectName'
      Origin = 'SCHEDULERDB."project.DB".Name'
      Size = 64
    end
    object ActivityQueryprojectID: TIntegerField
      FieldName = 'projectID'
      Origin = 'SCHEDULERDB."project.DB".ID'
    end
    object ActivityQueryTransfered: TStringField
      FieldName = 'Transfered'
      Origin = 'SCHEDULERDB."activities.DB".TRANSFERED'
      Size = 1
    end
  end
  object ActivitySqlSource: TDataSource
    DataSet = ActivityQuery
    Left = 144
    Top = 120
  end
  object MainMenu1: TMainMenu
    Left = 224
    Top = 120
    object Ttigkeiten1: TMenuItem
      Caption = '&Tätigkeiten'
      GroupIndex = 1
      object ExportProtocol: TMenuItem
        Caption = 'Export -> &Stundenabrechnung'
        OnClick = ExportProtocolClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object ExportCSV: TMenuItem
        Caption = 'Export -> &CSV'
        OnClick = ExportCSVClick
      end
      object ImportCSV: TMenuItem
        Caption = '&Import -> CSV'
        OnClick = ImportCSVClick
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object ExportAufgaben1: TMenuItem
        Caption = 'Export &Aufgaben für Mobil'
        OnClick = ExportAufgaben1Click
      end
      object ImportTtigkeiten1: TMenuItem
        Caption = 'Import &Tätigkeiten vom Mobil'
        OnClick = ImportTtigkeiten1Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object Test: TMenuItem
        Caption = '&Prüfe Zeiten'
        OnClick = TestClick
      end
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'CSV'
    Filter = 'Comma Separated Value (*.CSV)|*.CSV|Alle Dateien|*.*'
    Title = 'Export Tätigkeiten'
    Left = 224
    Top = 240
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'CSV'
    Filter = 'Comma Separated Value (*.CSV)|*.CSV|Alle Dateien|*.*'
    Title = 'Import Tätigkeiten'
    Left = 224
    Top = 184
  end
  object AutoSaveTimer: TTimer
    Enabled = False
    Interval = 60000
    OnTimer = AutoSaveTimerTimer
    Left = 24
    Top = 248
  end
  object insertQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'insert into activities'
      '('
      #9'id,'
      #9'task,'
      #9'StartTime,'
      #9'EndTime'
      ')'
      'values'
      '('
      #9':id,'
      #9':task,'
      #9':StartTime,'
      #9':EndTime'
      ')'
      ''
      ''
      ' '
      ' '
      ' ')
    Left = 144
    Top = 240
    ParamData = <
      item
        DataType = ftUnknown
        Name = 'id'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'task'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'StartTime'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'EndTime'
        ParamType = ptUnknown
      end>
  end
  object updateQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'update'#9'activities'
      'set'#9#9'task = :task,'
      #9#9'StartTime = :StartTime,'
      #9#9'EndTime = :EndTime'
      'where'#9'id = :id')
    Left = 144
    Top = 296
    ParamData = <
      item
        DataType = ftUnknown
        Name = 'task'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'StartTime'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'EndTime'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'id'
        ParamType = ptUnknown
      end>
  end
  object deleteQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'delete from activities'
      'where'#9'id = :id')
    Left = 144
    Top = 352
    ParamData = <
      item
        DataType = ftUnknown
        Name = 'id'
        ParamType = ptUnknown
      end>
  end
  object ActivityUpdateSQL: TUpdateSQL
    ModifySQL.Strings = (
      'update'#9'activities'
      'set'#9#9'task = :task,'
      #9#9'StartTime = :StartTime,'
      #9#9'EndTime = :EndTime'
      'where'#9'id = :id')
    InsertSQL.Strings = (
      'insert into activities'
      '('
      #9'id,'
      #9'task,'
      #9'StartTime,'
      #9'EndTime'
      ')'
      'values'
      '('
      #9':id,'
      #9':task,'
      #9':StartTime,'
      #9':EndTime'
      ')'
      ''
      '')
    DeleteSQL.Strings = (
      'delete from activities where id = :id')
    Left = 344
    Top = 184
  end
end
