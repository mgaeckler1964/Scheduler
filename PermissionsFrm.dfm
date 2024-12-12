object PermissionsForm: TPermissionsForm
  Left = 470
  Top = 371
  BorderStyle = bsSingle
  Caption = 'Rechte'
  ClientHeight = 270
  ClientWidth = 449
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object DBGrid1: TDBGrid
    Left = 8
    Top = 40
    Width = 241
    Height = 225
    DataSource = DataSourceACLs
    TabOrder = 0
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -11
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
  end
  object DBNavigator: TDBNavigator
    Left = 8
    Top = 8
    Width = 240
    Height = 25
    DataSource = DataSourceACLs
    TabOrder = 1
  end
  object RadioGroupSchedules: TRadioGroup
    Left = 256
    Top = 40
    Width = 185
    Height = 65
    Caption = 'Termine'
    Items.Strings = (
      'Kein Recht'
      'Lesen'
      'Schreiben')
    TabOrder = 2
    OnClick = RadioGroupSchedulesClick
  end
  object RadioGroupRecurring: TRadioGroup
    Left = 256
    Top = 112
    Width = 185
    Height = 65
    Caption = 'Regelmässige Termine'
    Items.Strings = (
      'Kein Recht'
      'Lesen'
      'Schreiben')
    TabOrder = 3
    OnClick = RadioGroupRecurringClick
  end
  object RadioGroupActivities: TRadioGroup
    Left = 256
    Top = 184
    Width = 185
    Height = 65
    Caption = 'Tätigkeiten'
    Items.Strings = (
      'Kein Recht'
      'Lesen'
      'Schreiben')
    TabOrder = 4
    OnClick = RadioGroupActivitiesClick
  end
  object QueryAll: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select id, username from user_tab order by username')
    Left = 168
    Top = 168
  end
  object DataSourceACLs: TDataSource
    DataSet = TableACLs
    Left = 88
    Top = 160
  end
  object TableACLs: TTable
    AfterInsert = TableACLsAfterInsert
    AfterScroll = TableACLsAfterScroll
    DatabaseName = 'SchedulerDB'
    TableName = 'ACLS.db'
    Left = 24
    Top = 152
    object TableACLsSCHEDULER_ID: TIntegerField
      FieldName = 'SCHEDULER_ID'
      Required = True
      Visible = False
    end
    object TableACLsUSER_ID: TIntegerField
      FieldName = 'USER_ID'
      Required = True
      Visible = False
    end
    object TableACLsPERMISSIONS: TIntegerField
      FieldName = 'PERMISSIONS'
      Required = True
      Visible = False
    end
    object TableACLsUSERNAME: TStringField
      DisplayLabel = 'Benutzer/Gruppe'
      DisplayWidth = 40
      FieldKind = fkLookup
      FieldName = 'USERNAME'
      LookupDataSet = QueryAll
      LookupKeyFields = 'id'
      LookupResultField = 'username'
      KeyFields = 'USER_ID'
      Lookup = True
    end
  end
end
