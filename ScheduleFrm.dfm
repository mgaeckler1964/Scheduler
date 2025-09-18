object ScheduleForm: TScheduleForm
  Tag = 4
  Left = 345
  Top = 306
  Width = 994
  Height = 540
  Caption = 'Termine'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  Menu = MainMenu1
  OldCreateOrder = True
  Position = poDefault
  Visible = True
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object ScheduleGrid: TDBGrid
    Left = 0
    Top = 25
    Width = 978
    Height = 367
    Align = alClient
    DataSource = ScheduleSource
    TabOrder = 0
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    OnDrawColumnCell = ScheduleGridDrawColumnCell
    Columns = <
      item
        Expanded = False
        FieldName = 'AlarmDate'
        Title.Caption = 'Alarmzeit'
        Width = 110
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'StartDate'
        Title.Caption = 'Anfang'
        Width = 110
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'EndDate'
        Title.Caption = 'Ende'
        Width = 110
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'Title'
        Title.Caption = 'Titel'
        Width = 350
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'ProjectName'
        Title.Caption = 'Projekt'
        Visible = True
      end>
  end
  object DBMemo1: TDBMemo
    Left = 0
    Top = 392
    Width = 978
    Height = 89
    Align = alBottom
    DataField = 'Description'
    DataSource = ScheduleSource
    TabOrder = 1
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 978
    Height = 25
    Align = alTop
    TabOrder = 2
    object BitBtnNew: TBitBtn
      Left = 96
      Top = 0
      Width = 75
      Height = 25
      Caption = 'Neu'
      TabOrder = 1
      OnClick = NeuerTerminClick
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        0400000000000001000000000000000000001000000010000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        33333333FF33333333FF333993333333300033377F3333333777333993333333
        300033F77FFF3333377739999993333333333777777F3333333F399999933333
        33003777777333333377333993333333330033377F3333333377333993333333
        3333333773333333333F333333333333330033333333F33333773333333C3333
        330033333337FF3333773333333CC333333333FFFFF77FFF3FF33CCCCCCCCCC3
        993337777777777F77F33CCCCCCCCCC3993337777777777377333333333CC333
        333333333337733333FF3333333C333330003333333733333777333333333333
        3000333333333333377733333333333333333333333333333333}
      NumGlyphs = 2
    end
    object BitBtnChange: TBitBtn
      Left = 171
      Top = 0
      Width = 75
      Height = 25
      Caption = 'Ändern'
      TabOrder = 2
      OnClick = TerminBearbeitenClick
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        0400000000000001000000000000000000001000000010000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        333333333333333333FF33333333333330003333333333333777333333333333
        300033333333333337773333333333333333333333333333333F333333333333
        3300333333333333337733333333333333003333333333333377333333333333
        3333333333333333333F333333333333330033333333F33333773333333C3333
        330033333337FF3333773333333CC333333333FFFFF77FFF3FF33CCCCCCCCCC3
        003337777777777F77F33CCCCCCCCCC3003337777777777377333333333CC333
        333333333337733333FF3333333C333330003333333733333777333333333333
        3000333333333333377733333333333333333333333333333333}
      NumGlyphs = 2
    end
    object BitBtnDelete: TBitBtn
      Left = 246
      Top = 0
      Width = 75
      Height = 25
      Caption = 'Löschen'
      TabOrder = 3
      OnClick = TerminLschenClick
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        0400000000000001000000000000000000001000000010000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        333333333333333333FF33333333333330003333333333333777333333333333
        300033FFFFFF3333377739999993333333333777777F3333333F399999933333
        3300377777733333337733333333333333003333333333333377333333333333
        3333333333333333333F333333333333330033333F33333333773333C3333333
        330033337F3333333377333CC3333333333333F77FFFFFFF3FF33CCCCCCCCCC3
        993337777777777F77F33CCCCCCCCCC399333777777777737733333CC3333333
        333333377F33333333FF3333C333333330003333733333333777333333333333
        3000333333333333377733333333333333333333333333333333}
      NumGlyphs = 2
    end
    object BitBtnDeleteAll: TBitBtn
      Left = 393
      Top = 0
      Width = 75
      Height = 25
      Caption = 'Alte Lösch.'
      TabOrder = 5
      OnClick = BitBtnDeleteAllClick
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        0400000000000001000000000000000000001000000010000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333000000000
        3333333777777777F3333330F777777033333337F3F3F3F7F3333330F0808070
        33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
        33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
        333333F7F7F7F7F7F3F33030F080707030333737F7F7F7F7F7333300F0808070
        03333377F7F7F7F773333330F080707033333337F7F7F7F7F333333070707070
        33333337F7F7F7F7FF3333000000000003333377777777777F33330F88877777
        0333337FFFFFFFFF7F3333000000000003333377777777777333333330777033
        3333333337FFF7F3333333333000003333333333377777333333}
      NumGlyphs = 2
    end
    object DBNavigator: TDBNavigator
      Left = 0
      Top = 0
      Width = 100
      Height = 25
      DataSource = ScheduleSource
      VisibleButtons = [nbFirst, nbPrior, nbNext, nbLast]
      TabOrder = 0
    end
    object RadioAll: TRadioButton
      Left = 472
      Top = 0
      Width = 41
      Height = 17
      Caption = 'Alle'
      Checked = True
      TabOrder = 6
      TabStop = True
      OnClick = ButtonAllClick
    end
    object DatesRadio: TRadioButton
      Left = 512
      Top = 0
      Width = 57
      Height = 17
      Caption = 'Termine'
      TabOrder = 7
      OnClick = ButtonDatesClick
    end
    object TasksRadio: TRadioButton
      Left = 576
      Top = 0
      Width = 73
      Height = 17
      Caption = 'Aufgaben'
      TabOrder = 8
      OnClick = ButtonTasksClick
    end
    object FinishBitBtn: TBitBtn
      Left = 320
      Top = 0
      Width = 75
      Height = 25
      Caption = 'Abschließ.'
      TabOrder = 4
      OnClick = FinishBitBtnClick
      Glyph.Data = {
        DE010000424DDE01000000000000760000002800000024000000120000000100
        0400000000006801000000000000000000001000000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333333333333333330000333333333333333333333333F33333333333
        00003333344333333333333333388F3333333333000033334224333333333333
        338338F3333333330000333422224333333333333833338F3333333300003342
        222224333333333383333338F3333333000034222A22224333333338F338F333
        8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
        33333338F83338F338F33333000033A33333A222433333338333338F338F3333
        0000333333333A222433333333333338F338F33300003333333333A222433333
        333333338F338F33000033333333333A222433333333333338F338F300003333
        33333333A222433333333333338F338F00003333333333333A22433333333333
        3338F38F000033333333333333A223333333333333338F830000333333333333
        333A333333333333333338330000333333333333333333333333333333333333
        0000}
      NumGlyphs = 2
    end
  end
  object ScheduleSource: TDataSource
    DataSet = datesQuery
    Left = 96
    Top = 216
  end
  object datesQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select'#9#9'schedule.*, project.name as ProjectName'
      'from'#9#9'schedule'
      '   left OUTER JOIN'#9#9'Project'
      '   on'#9#9#9#9#9'(Project.ID = Schedule.Project)'
      'where'#9#9'userId = :theUser'
      'order by'#9'enddate, startdate, ProjectName, Title'
      ' '
      ' '
      ' '
      ' '
      ' '
      ' '
      ' '
      ' ')
    Left = 16
    Top = 216
    ParamData = <
      item
        DataType = ftInteger
        Name = 'theUser'
        ParamType = ptUnknown
        Value = 0
      end>
  end
  object MainMenu1: TMainMenu
    Left = 168
    Top = 216
    object Termin1: TMenuItem
      Caption = '&Termin'
      GroupIndex = 1
      object NeuerTermin: TMenuItem
        Caption = '&Neuer Termin'
        OnClick = NeuerTerminClick
      end
      object TerminBearbeiten: TMenuItem
        Caption = 'Termin &Bearbeiten'
        OnClick = TerminBearbeitenClick
      end
      object TerminLschen: TMenuItem
        Caption = 'Termin &Löschen'
        OnClick = TerminLschenClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object Export2: TMenuItem
        Caption = 'Im-/Export &Casio'
        object Import1: TMenuItem
          Caption = '&Import...'
          OnClick = Import1Click
        end
        object N2: TMenuItem
          Caption = '-'
        end
        object ExportAll: TMenuItem
          Caption = 'Export &Alle...'
          OnClick = ExportAllClick
        end
        object ExportNew: TMenuItem
          Caption = 'Export &Neue...'
          OnClick = ExportNewClick
        end
      end
      object ImExportOutlook1: TMenuItem
        Caption = 'Im-/Export &Outlook'
        object Import2: TMenuItem
          Caption = '&Import...'
          OnClick = Import2Click
        end
        object N3: TMenuItem
          Caption = '-'
        end
        object ExportAlle1: TMenuItem
          Caption = 'Export &Alle...'
          OnClick = ExportAlle1Click
        end
        object ExportNeue1: TMenuItem
          Caption = 'Export &Neue...'
          OnClick = ExportNeue1Click
        end
      end
    end
  end
  object dbaseTable: TTable
    AutoCalcFields = False
    FieldDefs = <
      item
        Name = 'DATUM'
        DataType = ftDate
      end
      item
        Name = 'ANFANG'
        DataType = ftFloat
      end
      item
        Name = 'ENDE'
        DataType = ftFloat
      end
      item
        Name = 'ALARM'
        DataType = ftFloat
      end
      item
        Name = 'TEXT'
        DataType = ftString
        Size = 128
      end>
    StoreDefs = True
    TableName = 'y:\schedule\casio.dbf'
    TableType = ttDBase
    Left = 240
    Top = 216
    object dbaseTableDATUM: TDateField
      FieldName = 'DATUM'
    end
    object dbaseTableANFANG: TFloatField
      FieldName = 'ANFANG'
    end
    object dbaseTableENDE: TFloatField
      FieldName = 'ENDE'
    end
    object dbaseTableALARM: TFloatField
      FieldName = 'ALARM'
    end
    object dbaseTableTEXT: TStringField
      FieldName = 'TEXT'
      Size = 128
    end
  end
  object SaveDialog: TSaveDialog
    Filter = 'Casio (*.dbf)|*.dbf|Outlook 2000 (*.csv)|*.csv'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Title = 'Export...'
    Left = 320
    Top = 216
  end
  object newQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select'#9'*'
      'from'#9'schedule'
      'where'#9'userId = :theUser  and id > :lastId'
      ' ')
    UniDirectional = True
    Left = 24
    Top = 136
    ParamData = <
      item
        DataType = ftInteger
        Name = 'theUser'
        ParamType = ptUnknown
        Value = 0
      end
      item
        DataType = ftUnknown
        Name = 'lastId'
        ParamType = ptUnknown
      end>
  end
  object updateUserSQL: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'update'#9'user_tab'
      'set'#9#9'lastExport = :lastId'
      'where'#9'id = :theUser'
      ' '
      ' ')
    UniDirectional = True
    Left = 96
    Top = 136
    ParamData = <
      item
        DataType = ftUnknown
        Name = 'lastId'
        ParamType = ptUnknown
      end
      item
        DataType = ftInteger
        Name = 'theUser'
        ParamType = ptUnknown
        Value = 0
      end>
  end
  object OpenDialog: TOpenDialog
    Filter = 'DBase (*.dbf)|*.dbf|Outlook 2000 (*.csv)|*.csv'
    Title = 'Import...'
    Left = 392
    Top = 216
  end
  object insertDateSQL: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'insert into schedule'
      '('
      #9'UserId,'
      #9'StartDate,'
      #9'EndDate,'
      #9'AlarmDate,'
      #9'Title,'
      #9'Description,'
      #9'Location,'
      #9'AlarmBefore,'
      #9'AlarmUnit'
      ')'
      'values'
      '('
      #9':UserId,'
      #9':StartDate,'
      #9':EndDate,'
      #9':AlarmDate,'
      #9':Title,'
      #9':Description,'
      #9':Location,'
      #9':AlarmBefore,'
      #9#39'Minuten'#39
      ')'
      ' '
      ' '
      ' '
      ' ')
    Left = 184
    Top = 136
    ParamData = <
      item
        DataType = ftLargeint
        Name = 'UserId'
        ParamType = ptInput
      end
      item
        DataType = ftDateTime
        Name = 'StartDate'
        ParamType = ptInput
      end
      item
        DataType = ftDateTime
        Name = 'EndDate'
        ParamType = ptInput
      end
      item
        DataType = ftDateTime
        Name = 'AlarmDate'
        ParamType = ptInput
      end
      item
        DataType = ftString
        Name = 'Title'
        ParamType = ptInput
      end
      item
        DataType = ftString
        Name = 'Description'
        ParamType = ptInput
      end
      item
        DataType = ftString
        Name = 'Location'
        ParamType = ptInput
      end
      item
        DataType = ftSmallint
        Name = 'AlarmBefore'
        ParamType = ptInput
      end>
  end
  object activeDatesQuery: TQuery
    DatabaseName = 'SchedulerDB'
    RequestLive = True
    SQL.Strings = (
      'select'#9'*'
      'from'#9'SCHEDULE'
      'where'#9'USERID = :theUser and'
      #9#9'('
      #9#9#9'(ALARMDATE is not null and'#9'ALARMDATE <= :theDate) or'
      #9#9#9'(COMMAND is not null and STARTDATE <= :theDate)'
      #9#9')'
      ' '
      ' '
      ' '
      ' '
      ' '
      ' '
      ' ')
    Left = 56
    Top = 72
    ParamData = <
      item
        DataType = ftInteger
        Name = 'theUser'
        ParamType = ptUnknown
        Value = 0
      end
      item
        DataType = ftDate
        Name = 'theDate'
        ParamType = ptUnknown
      end
      item
        DataType = ftDate
        Name = 'theDate'
        ParamType = ptUnknown
      end>
  end
  object theTimer: TTimer
    OnTimer = theTimerTimer
    Left = 160
    Top = 64
  end
end
