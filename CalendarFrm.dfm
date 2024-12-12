object CalendarForm: TCalendarForm
  Tag = 2
  Left = 597
  Top = 415
  Width = 816
  Height = 564
  Caption = 'Kalender'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsMDIChild
  OldCreateOrder = True
  Position = poDefault
  Visible = True
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 800
    Height = 161
    Align = alTop
    TabOrder = 0
    object DayLabel: TLabel
      Left = 8
      Top = 8
      Width = 72
      Height = 19
      Caption = 'DayLabel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Panel2: TPanel
      Left = 575
      Top = 1
      Width = 224
      Height = 159
      Align = alRight
      Caption = 'Panel2'
      TabOrder = 0
      object MonthCombo: TComboBox
        Left = 16
        Top = 8
        Width = 137
        Height = 21
        Style = csDropDownList
        DropDownCount = 12
        ItemHeight = 13
        TabOrder = 0
        OnChange = MonthComboChange
        Items.Strings = (
          'Januar'
          'Februar'
          'März'
          'April'
          'Mai'
          'Juni'
          'July'
          'August'
          'September'
          'Oktober'
          'November'
          'Dezember')
      end
      object YearSpin: TCSpinEdit
        Left = 160
        Top = 8
        Width = 49
        Height = 22
        TabStop = True
        ParentColor = False
        TabOrder = 1
        OnChange = YearSpinChange
      end
      object Scheduler: TScheduler
        Left = 0
        Top = 32
        Width = 224
        Height = 129
        TabStop = True
        ParentColor = False
        TabOrder = 2
        OnChange = SchedulerChange
        HolidayBackgrnd = clMaroon
        AppointmentBackgrnd = cl3DLight
        HolidayFont.Charset = DEFAULT_CHARSET
        HolidayFont.Color = clWindowText
        HolidayFont.Height = -11
        HolidayFont.Name = 'MS Sans Serif'
        HolidayFont.Style = []
        AppointmentFont.Charset = DEFAULT_CHARSET
        AppointmentFont.Color = clWindowText
        AppointmentFont.Height = -11
        AppointmentFont.Name = 'MS Sans Serif'
        AppointmentFont.Style = [fsBold]
      end
    end
  end
  object DayView: TDayView
    Left = 0
    Top = 161
    Width = 800
    Height = 364
    Align = alClient
    ColCount = 2
    DefaultRowHeight = 18
    FixedCols = 0
    RowCount = 48
    FixedRows = 0
    ScrollBars = ssVertical
    TabOrder = 1
    FreeTimeBackgrnd = clRed
    WorkTimeBackgrnd = clYellow
    TimeRowBackgrnd = clBtnFace
    AppointmentBackgrnd = clWhite
    ColWidths = (
      40
      730)
  end
  object SchedulesQuery: TQuery
    DatabaseName = 'SchedulerDB'
    SQL.Strings = (
      'select distinct StartDate, EndDate, Title'
      'from schedule'
      'where userId = :theUser'
      'and startDate >= :firstDay'
      'and startDate < :lastDay'
      ' '
      ' ')
    Left = 128
    Top = 16
    ParamData = <
      item
        DataType = ftUnknown
        Name = 'theUser'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'firstDay'
        ParamType = ptUnknown
      end
      item
        DataType = ftUnknown
        Name = 'lastDay'
        ParamType = ptUnknown
      end>
  end
end
