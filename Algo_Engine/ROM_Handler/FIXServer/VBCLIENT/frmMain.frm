VERSION 5.00
Begin VB.Form frmMain 
   Caption         =   "Order Execution Client"
   ClientHeight    =   5865
   ClientLeft      =   3705
   ClientTop       =   3585
   ClientWidth     =   11280
   LinkTopic       =   "Form1"
   ScaleHeight     =   5865
   ScaleWidth      =   11280
   Begin VB.Frame Frame2 
      Caption         =   "Order Status:"
      Height          =   3135
      Left            =   4680
      TabIndex        =   18
      Top             =   2520
      Width           =   6495
      Begin VB.ListBox lstOrderStatus 
         Height          =   2790
         Left            =   120
         TabIndex        =   21
         Top             =   240
         Width           =   6255
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "System Status:"
      Height          =   2175
      Left            =   4680
      TabIndex        =   15
      Top             =   240
      Width           =   6495
      Begin VB.ListBox lstStatus 
         Height          =   1620
         Left            =   120
         TabIndex        =   16
         Top             =   360
         Width           =   6255
      End
   End
   Begin VB.Frame Order 
      Caption         =   "Order:"
      Height          =   5415
      Left            =   120
      TabIndex        =   0
      Top             =   240
      Width           =   4455
      Begin VB.TextBox txtNewPrice 
         Height          =   315
         Left            =   3480
         TabIndex        =   25
         Text            =   "0.0"
         Top             =   2280
         Width           =   735
      End
      Begin VB.TextBox txtNewQty 
         Height          =   315
         Left            =   3480
         TabIndex        =   23
         Text            =   "20"
         Top             =   1680
         Width           =   735
      End
      Begin VB.CommandButton cmdReplace 
         Caption         =   "Replace"
         Height          =   375
         Left            =   2640
         TabIndex        =   22
         Top             =   2880
         Width           =   1575
      End
      Begin VB.CommandButton cmdShutdown 
         Caption         =   "Shutdown"
         Height          =   375
         Left            =   360
         TabIndex        =   20
         Top             =   4680
         Width           =   1575
      End
      Begin VB.CommandButton cmdInit 
         Caption         =   "Init"
         Height          =   375
         Left            =   360
         TabIndex        =   19
         Top             =   4080
         Width           =   1575
      End
      Begin VB.ComboBox cboEx 
         Height          =   315
         ItemData        =   "frmMain.frx":0000
         Left            =   2640
         List            =   "frmMain.frx":0025
         TabIndex        =   17
         Top             =   480
         Width           =   1575
      End
      Begin VB.CommandButton cmdCancel 
         Caption         =   "Cancel All"
         Enabled         =   0   'False
         Height          =   375
         Left            =   2640
         TabIndex        =   14
         Top             =   4680
         Width           =   1575
      End
      Begin VB.ComboBox cboSide 
         Height          =   315
         ItemData        =   "frmMain.frx":004F
         Left            =   1440
         List            =   "frmMain.frx":005C
         TabIndex        =   13
         Top             =   3480
         Width           =   975
      End
      Begin VB.CommandButton cmdSubmit 
         Caption         =   "Submit"
         Enabled         =   0   'False
         Height          =   375
         Left            =   2640
         TabIndex        =   11
         Top             =   4080
         Width           =   1575
      End
      Begin VB.ComboBox cboTIF 
         Height          =   315
         ItemData        =   "frmMain.frx":0072
         Left            =   1440
         List            =   "frmMain.frx":007F
         TabIndex        =   10
         Top             =   2880
         Width           =   975
      End
      Begin VB.ComboBox cboType 
         Height          =   315
         ItemData        =   "frmMain.frx":0092
         Left            =   1440
         List            =   "frmMain.frx":009C
         TabIndex        =   9
         Top             =   2280
         Width           =   975
      End
      Begin VB.TextBox txtQty 
         Height          =   315
         Left            =   1440
         TabIndex        =   8
         Text            =   "100"
         Top             =   1680
         Width           =   975
      End
      Begin VB.TextBox txtPrice 
         Height          =   315
         Left            =   1440
         TabIndex        =   7
         Text            =   "52.50"
         Top             =   1080
         Width           =   975
      End
      Begin VB.TextBox txtSymbol 
         Height          =   315
         Left            =   1440
         TabIndex        =   1
         Text            =   "MSFT"
         Top             =   480
         Width           =   975
      End
      Begin VB.Label lblNewPrice 
         Caption         =   "New Price:"
         Height          =   255
         Left            =   2640
         TabIndex        =   26
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label lblNewQty 
         Caption         =   "New Qty:"
         Height          =   255
         Left            =   2640
         TabIndex        =   24
         Top             =   1680
         Width           =   975
      End
      Begin VB.Label lblSide 
         Caption         =   "Side:"
         Height          =   255
         Left            =   240
         TabIndex        =   12
         Top             =   3480
         Width           =   1215
      End
      Begin VB.Label lblTIF 
         Caption         =   "Time in Force:"
         Height          =   255
         Left            =   240
         TabIndex        =   6
         Top             =   2880
         Width           =   1215
      End
      Begin VB.Label lblType 
         Caption         =   "Order Type:"
         Height          =   255
         Left            =   240
         TabIndex        =   5
         Top             =   2280
         Width           =   1215
      End
      Begin VB.Label lblQty 
         Caption         =   "Quantity:"
         Height          =   255
         Left            =   240
         TabIndex        =   4
         Top             =   1680
         Width           =   1215
      End
      Begin VB.Label lblPrice 
         Caption         =   "Price:"
         Height          =   255
         Left            =   240
         TabIndex        =   3
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label lblSymbol 
         Caption         =   "Symbol:"
         Height          =   255
         Left            =   240
         TabIndex        =   2
         Top             =   480
         Width           =   1215
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Private m_pTradeEngine As EXENGINELib.TradeEngine
Private m_Orders As New Collection
Implements EXENGINELib.IStatusHandler
Private m_pStatusHandler As EXENGINELib.IStatusHandler
Dim m_nSeed As Long


Private Sub cmdCancel_Click()
On Error GoTo Handler
    Dim pOrder As EXENGINELib.Order
    For Each pOrder In m_Orders
        m_pTradeEngine.CancelOrder pOrder
    Next
    Exit Sub
Handler:
    MsgBox Err.Description
    Exit Sub

End Sub

Private Sub cmdInit_Click()
On Error GoTo Handler
    Set m_pTradeEngine = New EXENGINELib.TradeEngine
    m_pTradeEngine.Init "c:\FixServer\FixClient.cfg", Me
    cmdSubmit.Enabled = True
    cmdCancel.Enabled = True
    Exit Sub
Handler:
    MsgBox Err.Description
    Exit Sub
End Sub


Private Sub cmdReplace_Click()
On Error GoTo Handler
    Dim pOrder As EXENGINELib.Order
    Set pOrder = m_Orders.Item(1)
    m_pTradeEngine.ReplaceOrder pOrder, CStr(Time), CLng(txtNewQty), CDec(txtNewPrice)
    Exit Sub
Handler:
    MsgBox Err.Description
End Sub

Private Sub cmdShutdown_Click()
On Error GoTo Handler
    m_pTradeEngine.Shutdown
    Set m_pTradeEngine = Nothing
    Set m_pStatusHandler = Nothing
    cmdSubmit.Enabled = False
    cmdCancel.Enabled = False
    Exit Sub
Handler:
    MsgBox Err.Description
    Exit Sub
End Sub

Private Sub cmdSubmit_Click()
On Error GoTo Handler
    Dim pOrder As New EXENGINELib.Order
    m_pTradeEngine.CreateNewOrder pOrder
    pOrder.Symbol = txtSymbol
    pOrder.ClientOrderID = CStr(m_nSeed)
    m_nSeed = m_nSeed + 1
    pOrder.CumQty = 0
    pOrder.LeaveQty = txtQty
    pOrder.OrgQty = txtQty
    If cboType.Text = "MARKET" Then
        pOrder.OrderType = eMarket
    Else
        pOrder.OrderType = eLimit
    End If
    pOrder.Price = txtPrice
    pOrder.SecType = eEquity
    If cboTIF.Text = "DAY" Then
        pOrder.TimeInForce = eDay
    Else
        pOrder.TimeInForce = eGTC
    End If
    If cboSide.Text = "BUY" Then
        pOrder.OrderSide = eBuy
    ElseIf cboSide.Text = "SELL" Then
        pOrder.OrderSide = eSell
    Else
        pOrder.OrderSide = eShort
    End If
    pOrder.Account = "0551 USER0001 WAG USER0001"
    m_pTradeEngine.SubmitOrder pOrder
    m_Orders.Add pOrder
    Exit Sub
Handler:
    MsgBox Err.Description
End Sub



Private Sub Form_Load()
    m_nSeed = Hour(Time) * 3600 + Minute(Time) * 60 + Second(Time)
    cboEx.ListIndex = 0
    cboSide.ListIndex = 0
    cboTIF.ListIndex = 0
    cboType.ListIndex = 0
End Sub

Private Sub IStatusHandler_OnOrderStatus(ByVal Order As EXENGINELib.Order)
    Dim msg As String
    msg = "ClientOrderID " + Order.ClientOrderID + ": State " + Str(Order.OrderState) + "; Modifier " + Str(Order.OrderModifier) + "; Last Shares " + Str(Order.LastShares) + "; Exec Price " + Str(Order.ExecPrice)
    If lstOrderStatus.ListCount > 100 Then
        lstOrderStatus.Clear
    End If
    lstOrderStatus.AddItem msg
End Sub

Private Sub IStatusHandler_OnSystemStatus(ByVal msg As String)
    If lstStatus.ListCount > 100 Then
        lstStatus.Clear
    End If
    lstStatus.AddItem msg
   
End Sub



