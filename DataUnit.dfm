object theDataModule: TtheDataModule
  OldCreateOrder = False
  Left = 724
  Top = 235
  Height = 640
  Width = 870
  object ProjectTable: TTable
    AfterInsert = ProjectTableAfterInsert
    BeforeDelete = ProjectTableBeforeDelete
    DatabaseName = 'SchedulerDB'
    FieldDefs = <
      item
        Name = 'ID'
        Attributes = [faRequired]
        DataType = ftInteger
      end
      item
        Name = 'NAME'
        Attributes = [faRequired]
        DataType = ftString
        Size = 64
      end>
    IndexDefs = <
      item
        Name = 'ProjectTableIndex1'
        Fields = 'ID'
        Options = [ixPrimary, ixUnique]
      end
      item
        Name = 'ProjectNameIdx'
        Fields = 'Name'
        Options = [ixUnique, ixCaseInsensitive]
      end>
    IndexName = 'ProjectNameIdx'
    StoreDefs = True
    TableName = 'project.db'
    Left = 27
    Top = 13
    object ProjectTableID: TIntegerField
      FieldName = 'ID'
      Required = True
      Visible = False
    end
    object ProjectTableNAME: TStringField
      DisplayLabel = 'Name'
      FieldName = 'NAME'
      Required = True
      Size = 64
    end
  end
  object ProjectSource: TDataSource
    DataSet = ProjectTable
    Left = 119
    Top = 13
  end
  object ActivityMaxIdQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select max(id) from activities')
    Left = 32
    Top = 72
  end
end
