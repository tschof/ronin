
#ifndef OM_MESSAGES
#define OM_MESSAGES

#include "OMMessage.h"

namespace OM
{

	enum SideEnum
	{
		Side_Buy = 1,
		Side_Sell = 2,
		Side_BuyMinus = 3,
		Side_SellPlus = 4,
		Side_Short = 5,
		Side_Exempt = 6,
		Side_Undisclosed = 7,
		Side_Cross = 8,

	};

	enum TypeEnum
	{
		Type_Market = 1,
		Type_Limit = 2,
		Type_Stop = 3,
		Type_StopLimit = 4,

	};

	enum TIFEnum
	{
		TIF_Day = 0,
		TIF_GTC = 1,
		TIF_OPG = 2,
		TIF_IC = 3,
		TIF_FOK = 4,
		TIF_GTX = 5,
		TIF_GTD = 6,

	};

	enum CapacityEnum
	{
		Capacity_Agent = 'A',
		Capacity_Principal = 'P',

	};

	enum ExchangeEnum
	{
		Exchange_Archipelago = 0,
		Exchange_Instinet = 1,
		Exchange_NYSE = 2,
		Exchange_Island = 3,
		Exchange_AMEX = 4,
		Exchange_Globex = 6,
		Exchange_Brut = 7,
		Exchange_TLW = 13,
		Exchange_ISI_SFU = 15,
		Exchange_ISI_SFV = 16,
		Exchange_ISI_SFW = 17,
		Exchange_ISI_SFX = 18,
		Exchange_ISI_SFY = 19,
		Exchange_Jeffries = 23,
		Exchange_XCaliber = 24,
		Exchange_Brass = 25,
		Exchange_BTrade = 26,
		Exchange_CHX = 27,
		Exchange_ITG = 28,
		Exchange_ADOT = 29,
		Exchange_SGES = 30,
		Exchange_DirectPlus = 31,

	};

	enum StatusEnum
	{
		Status_New = 0,
		Status_PartiallyFilled = 1,
		Status_Filled = 2,
		Status_DoneForDay = 3,
		Status_Cancelled = 4,
		Status_Replaced = 5,
		Status_PendingCancel = 6,
		Status_Stopped = 7,
		Status_Rejected = 8,
		Status_Suspended = 9,
		Status_PendingNew = 10,
		Status_Calculated = 11,
		Status_Expired = 12,
		Status_Open = 13,
		Status_CancelRejected = 14,
		Status_Corrected = 15,
		Status_Busted = 16,
		Status_ReplaceRejected = 20,
		Status_ReplacePending = 21,

	};

	class Login : public Message
	{
		public:
			Login();
			Login( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetLoginName() const
				{ return GetField(2); } 
			void SetLoginName( const std::string& value )
				{ SetField(2, value); }
		
			int GetAutoCancel() const
				{ return ConvertStringToInt( GetField(3) ); }
			void SetAutoCancel( int value )
				{ SetField( 3, ConvertIntToString(value) ); }
		
			const std::string& GetPassword() const
				{ return GetField(7); } 
			void SetPassword( const std::string& value )
				{ SetField(7, value); }
		
	
	};	

	class EnterOrder : public Message
	{
		public:
			EnterOrder();
			EnterOrder( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetTrader() const
				{ return GetField(2); } 
			void SetTrader( const std::string& value )
				{ SetField(2, value); }
		
			const std::string& GetTag() const
				{ return GetField(3); } 
			void SetTag( const std::string& value )
				{ SetField(3, value); }
		
			int GetSide() const
				{ return ConvertStringToInt( GetField(4) ); }
			void SetSide( int value )
				{ SetField( 4, ConvertIntToString(value) ); }
		
			const std::string& GetSymbol() const
				{ return GetField(5); } 
			void SetSymbol( const std::string& value )
				{ SetField(5, value); }
		
			int GetShares() const
				{ return ConvertStringToInt( GetField(6) ); }
			void SetShares( int value )
				{ SetField( 6, ConvertIntToString(value) ); }
		
			double GetPrice() const
				{ return ConvertStringToDouble( GetField(7) ); }
			void SetPrice( double value )
				{ SetField( 7, ConvertDoubleToString(value) ); }
		
			int GetType() const
				{ return ConvertStringToInt( GetField(8) ); }
			void SetType( int value )
				{ SetField( 8, ConvertIntToString(value) ); }
		
			char GetTIF() const
				{ return ConvertStringToChar( GetField(9) ); }
			void SetTIF( char value )
				{ SetField( 9, ConvertCharToString(value) ); }
		
			char GetCapacity() const
				{ return ConvertStringToChar( GetField(10) ); }
			void SetCapacity( char value )
				{ SetField( 10, ConvertCharToString(value) ); }
		
			const std::string& GetClearingID() const
				{ return GetField(11); } 
			void SetClearingID( const std::string& value )
				{ SetField(11, value); }
		
			const std::string& GetClearingAccount() const
				{ return GetField(12); } 
			void SetClearingAccount( const std::string& value )
				{ SetField(12, value); }
		
			int GetExchange() const
				{ return ConvertStringToInt( GetField(13) ); }
			void SetExchange( int value )
				{ SetField( 13, ConvertIntToString(value) ); }
		
			int GetStatus() const
				{ return ConvertStringToInt( GetField(14) ); }
			void SetStatus( int value )
				{ SetField( 14, ConvertIntToString(value) ); }
		
			const std::string& GetInstructions() const
				{ return GetField(15); } 
			void SetInstructions( const std::string& value )
				{ SetField(15, value); }
		
			char GetPriceCheck() const
				{ return ConvertStringToChar( GetField(16) ); }
			void SetPriceCheck( char value )
				{ SetField( 16, ConvertCharToString(value) ); }
		
			int GetFloor() const
				{ return ConvertStringToInt( GetField(17) ); }
			void SetFloor( int value )
				{ SetField( 17, ConvertIntToString(value) ); }
		
			int GetMinQty() const
				{ return ConvertStringToInt( GetField(18) ); }
			void SetMinQty( int value )
				{ SetField( 18, ConvertIntToString(value) ); }
		
			double GetStopPrice() const
				{ return ConvertStringToDouble( GetField(19) ); }
			void SetStopPrice( double value )
				{ SetField( 19, ConvertDoubleToString(value) ); }
		
			char GetDiscretionary() const
				{ return ConvertStringToChar( GetField(20) ); }
			void SetDiscretionary( char value )
				{ SetField( 20, ConvertCharToString(value) ); }
		
			char GetSolicited() const
				{ return ConvertStringToChar( GetField(21) ); }
			void SetSolicited( char value )
				{ SetField( 21, ConvertCharToString(value) ); }
		
			const std::string& GetLocalAccount() const
				{ return GetField(22); } 
			void SetLocalAccount( const std::string& value )
				{ SetField(22, value); }
		
			const std::string& GetFirm() const
				{ return GetField(23); } 
			void SetFirm( const std::string& value )
				{ SetField(23, value); }
		
			const std::string& GetTradeFor() const
				{ return GetField(24); } 
			void SetTradeFor( const std::string& value )
				{ SetField(24, value); }
		
			const std::string& GetDBSymbol() const
				{ return GetField(26); } 
			void SetDBSymbol( const std::string& value )
				{ SetField(26, value); }
		
			int GetMultiplier() const
				{ return ConvertStringToInt( GetField(28) ); }
			void SetMultiplier( int value )
				{ SetField( 28, ConvertIntToString(value) ); }
		
			const std::string& GetExecutionDestination() const
				{ return GetField(29); } 
			void SetExecutionDestination( const std::string& value )
				{ SetField(29, value); }
		
			const std::string& GetExpirationDate() const
				{ return GetField(30); } 
			void SetExpirationDate( const std::string& value )
				{ SetField(30, value); }
		
			char GetOptionType() const
				{ return ConvertStringToChar( GetField(31) ); }
			void SetOptionType( char value )
				{ SetField( 31, ConvertCharToString(value) ); }
		
			double GetStrikePrice() const
				{ return ConvertStringToDouble( GetField(32) ); }
			void SetStrikePrice( double value )
				{ SetField( 32, ConvertDoubleToString(value) ); }
		
			const std::string& GetUnderlying() const
				{ return GetField(33); } 
			void SetUnderlying( const std::string& value )
				{ SetField(33, value); }
		
			int GetMaxFloor() const
				{ return ConvertStringToInt( GetField(34) ); }
			void SetMaxFloor( int value )
				{ SetField( 34, ConvertIntToString(value) ); }
		
			char GetSecurityType() const
				{ return ConvertStringToChar( GetField(35) ); }
			void SetSecurityType( char value )
				{ SetField( 35, ConvertCharToString(value) ); }
		
			const std::string& GetMMAccount() const
				{ return GetField(36); } 
			void SetMMAccount( const std::string& value )
				{ SetField(36, value); }
		
			const std::string& GetMMHomeExchange() const
				{ return GetField(37); } 
			void SetMMHomeExchange( const std::string& value )
				{ SetField(37, value); }
		
			int GetOpenClose() const
				{ return ConvertStringToInt( GetField(38) ); }
			void SetOpenClose( int value )
				{ SetField( 38, ConvertIntToString(value) ); }
		
			char GetTradingSession() const
				{ return ConvertStringToChar( GetField(39) ); }
			void SetTradingSession( char value )
				{ SetField( 39, ConvertCharToString(value) ); }
		
			const std::string& GetGiveup() const
				{ return GetField(40); } 
			void SetGiveup( const std::string& value )
				{ SetField(40, value); }
		
			const std::string& GetCMTA() const
				{ return GetField(46); } 
			void SetCMTA( const std::string& value )
				{ SetField(46, value); }
		
			const std::string& GetGTDDate() const
				{ return GetField(47); } 
			void SetGTDDate( const std::string& value )
				{ SetField(47, value); }
		
			int GetCumQty() const
				{ return ConvertStringToInt( GetField(48) ); }
			void SetCumQty( int value )
				{ SetField( 48, ConvertIntToString(value) ); }
		
			int GetLeaveQty() const
				{ return ConvertStringToInt( GetField(49) ); }
			void SetLeaveQty( int value )
				{ SetField( 49, ConvertIntToString(value) ); }
		
			double GetAvgPrc() const
				{ return ConvertStringToDouble( GetField(51) ); }
			void SetAvgPrc( double value )
				{ SetField( 51, ConvertDoubleToString(value) ); }
		
			const std::string& GetExecInst() const
				{ return GetField(57); } 
			void SetExecInst( const std::string& value )
				{ SetField(57, value); }
		
			const std::string& GetSSFirm() const
				{ return GetField(58); } 
			void SetSSFirm( const std::string& value )
				{ SetField(58, value); }
		
	
	};	

	class CancelOrder : public Message
	{
		public:
			CancelOrder();
			CancelOrder( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetTrader() const
				{ return GetField(2); } 
			void SetTrader( const std::string& value )
				{ SetField(2, value); }
		
			const std::string& GetTag() const
				{ return GetField(3); } 
			void SetTag( const std::string& value )
				{ SetField(3, value); }
		
			int GetSide() const
				{ return ConvertStringToInt( GetField(4) ); }
			void SetSide( int value )
				{ SetField( 4, ConvertIntToString(value) ); }
		
			const std::string& GetSymbol() const
				{ return GetField(5); } 
			void SetSymbol( const std::string& value )
				{ SetField(5, value); }
		
			int GetShares() const
				{ return ConvertStringToInt( GetField(6) ); }
			void SetShares( int value )
				{ SetField( 6, ConvertIntToString(value) ); }
		
			double GetPrice() const
				{ return ConvertStringToDouble( GetField(7) ); }
			void SetPrice( double value )
				{ SetField( 7, ConvertDoubleToString(value) ); }
		
			int GetType() const
				{ return ConvertStringToInt( GetField(8) ); }
			void SetType( int value )
				{ SetField( 8, ConvertIntToString(value) ); }
		
			char GetTIF() const
				{ return ConvertStringToChar( GetField(9) ); }
			void SetTIF( char value )
				{ SetField( 9, ConvertCharToString(value) ); }
		
			char GetCapacity() const
				{ return ConvertStringToChar( GetField(10) ); }
			void SetCapacity( char value )
				{ SetField( 10, ConvertCharToString(value) ); }
		
			const std::string& GetClearingID() const
				{ return GetField(11); } 
			void SetClearingID( const std::string& value )
				{ SetField(11, value); }
		
			const std::string& GetClearingAccount() const
				{ return GetField(12); } 
			void SetClearingAccount( const std::string& value )
				{ SetField(12, value); }
		
			int GetExchange() const
				{ return ConvertStringToInt( GetField(13) ); }
			void SetExchange( int value )
				{ SetField( 13, ConvertIntToString(value) ); }
		
			int GetStatus() const
				{ return ConvertStringToInt( GetField(14) ); }
			void SetStatus( int value )
				{ SetField( 14, ConvertIntToString(value) ); }
		
			const std::string& GetOrderTag() const
				{ return GetField(15); } 
			void SetOrderTag( const std::string& value )
				{ SetField(15, value); }
		
			const std::string& GetExchangeTag() const
				{ return GetField(16); } 
			void SetExchangeTag( const std::string& value )
				{ SetField(16, value); }
		
			const std::string& GetFirmTag() const
				{ return GetField(17); } 
			void SetFirmTag( const std::string& value )
				{ SetField(17, value); }
		
			const std::string& GetCancelTag() const
				{ return GetField(18); } 
			void SetCancelTag( const std::string& value )
				{ SetField(18, value); }
		
			const std::string& GetExecTag() const
				{ return GetField(19); } 
			void SetExecTag( const std::string& value )
				{ SetField(19, value); }
		
			const std::string& GetExecConfirm() const
				{ return GetField(20); } 
			void SetExecConfirm( const std::string& value )
				{ SetField(20, value); }
		
			const std::string& GetReference() const
				{ return GetField(21); } 
			void SetReference( const std::string& value )
				{ SetField(21, value); }
		
			char GetReport() const
				{ return ConvertStringToChar( GetField(22) ); }
			void SetReport( char value )
				{ SetField( 22, ConvertCharToString(value) ); }
		
			const std::string& GetContra() const
				{ return GetField(23); } 
			void SetContra( const std::string& value )
				{ SetField(23, value); }
		
			const std::string& GetTradeFor() const
				{ return GetField(24); } 
			void SetTradeFor( const std::string& value )
				{ SetField(24, value); }
		
			const std::string& GetQueryTag() const
				{ return GetField(25); } 
			void SetQueryTag( const std::string& value )
				{ SetField(25, value); }
		
			const std::string& GetText() const
				{ return GetField(26); } 
			void SetText( const std::string& value )
				{ SetField(26, value); }
		
			const std::string& GetOwner() const
				{ return GetField(27); } 
			void SetOwner( const std::string& value )
				{ SetField(27, value); }
		
			const std::string& GetOMExecTag() const
				{ return GetField(28); } 
			void SetOMExecTag( const std::string& value )
				{ SetField(28, value); }
		
			const std::string& GetActionID() const
				{ return GetField(59); } 
			void SetActionID( const std::string& value )
				{ SetField(59, value); }
		
	
	};	

	class ReplaceOrder : public Message
	{
		public:
			ReplaceOrder();
			ReplaceOrder( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetTrader() const
				{ return GetField(2); } 
			void SetTrader( const std::string& value )
				{ SetField(2, value); }
		
			const std::string& GetTag() const
				{ return GetField(3); } 
			void SetTag( const std::string& value )
				{ SetField(3, value); }
		
			int GetSide() const
				{ return ConvertStringToInt( GetField(4) ); }
			void SetSide( int value )
				{ SetField( 4, ConvertIntToString(value) ); }
		
			const std::string& GetSymbol() const
				{ return GetField(5); } 
			void SetSymbol( const std::string& value )
				{ SetField(5, value); }
		
			int GetShares() const
				{ return ConvertStringToInt( GetField(6) ); }
			void SetShares( int value )
				{ SetField( 6, ConvertIntToString(value) ); }
		
			double GetPrice() const
				{ return ConvertStringToDouble( GetField(7) ); }
			void SetPrice( double value )
				{ SetField( 7, ConvertDoubleToString(value) ); }
		
			int GetType() const
				{ return ConvertStringToInt( GetField(8) ); }
			void SetType( int value )
				{ SetField( 8, ConvertIntToString(value) ); }
		
			char GetTIF() const
				{ return ConvertStringToChar( GetField(9) ); }
			void SetTIF( char value )
				{ SetField( 9, ConvertCharToString(value) ); }
		
			char GetCapacity() const
				{ return ConvertStringToChar( GetField(10) ); }
			void SetCapacity( char value )
				{ SetField( 10, ConvertCharToString(value) ); }
		
			const std::string& GetClearingID() const
				{ return GetField(11); } 
			void SetClearingID( const std::string& value )
				{ SetField(11, value); }
		
			const std::string& GetClearingAccount() const
				{ return GetField(12); } 
			void SetClearingAccount( const std::string& value )
				{ SetField(12, value); }
		
			int GetExchange() const
				{ return ConvertStringToInt( GetField(13) ); }
			void SetExchange( int value )
				{ SetField( 13, ConvertIntToString(value) ); }
		
			int GetStatus() const
				{ return ConvertStringToInt( GetField(14) ); }
			void SetStatus( int value )
				{ SetField( 14, ConvertIntToString(value) ); }
		
			const std::string& GetOrderTag() const
				{ return GetField(15); } 
			void SetOrderTag( const std::string& value )
				{ SetField(15, value); }
		
			const std::string& GetExchangeTag() const
				{ return GetField(16); } 
			void SetExchangeTag( const std::string& value )
				{ SetField(16, value); }
		
			const std::string& GetFirmTag() const
				{ return GetField(17); } 
			void SetFirmTag( const std::string& value )
				{ SetField(17, value); }
		
			const std::string& GetCancelTag() const
				{ return GetField(18); } 
			void SetCancelTag( const std::string& value )
				{ SetField(18, value); }
		
			const std::string& GetExecTag() const
				{ return GetField(19); } 
			void SetExecTag( const std::string& value )
				{ SetField(19, value); }
		
			const std::string& GetExecConfirm() const
				{ return GetField(20); } 
			void SetExecConfirm( const std::string& value )
				{ SetField(20, value); }
		
			const std::string& GetReference() const
				{ return GetField(21); } 
			void SetReference( const std::string& value )
				{ SetField(21, value); }
		
			char GetReport() const
				{ return ConvertStringToChar( GetField(22) ); }
			void SetReport( char value )
				{ SetField( 22, ConvertCharToString(value) ); }
		
			const std::string& GetContra() const
				{ return GetField(23); } 
			void SetContra( const std::string& value )
				{ SetField(23, value); }
		
			const std::string& GetTradeFor() const
				{ return GetField(24); } 
			void SetTradeFor( const std::string& value )
				{ SetField(24, value); }
		
			const std::string& GetQueryTag() const
				{ return GetField(25); } 
			void SetQueryTag( const std::string& value )
				{ SetField(25, value); }
		
			const std::string& GetText() const
				{ return GetField(26); } 
			void SetText( const std::string& value )
				{ SetField(26, value); }
		
			const std::string& GetOwner() const
				{ return GetField(27); } 
			void SetOwner( const std::string& value )
				{ SetField(27, value); }
		
			const std::string& GetOMExecTag() const
				{ return GetField(28); } 
			void SetOMExecTag( const std::string& value )
				{ SetField(28, value); }
		
			const std::string& GetGTDDate() const
				{ return GetField(47); } 
			void SetGTDDate( const std::string& value )
				{ SetField(47, value); }
		
			int GetLeaveQty() const
				{ return ConvertStringToInt( GetField(49) ); }
			void SetLeaveQty( int value )
				{ SetField( 49, ConvertIntToString(value) ); }
		
			const std::string& GetExecInst() const
				{ return GetField(57); } 
			void SetExecInst( const std::string& value )
				{ SetField(57, value); }
		
			const std::string& GetSSFirm() const
				{ return GetField(58); } 
			void SetSSFirm( const std::string& value )
				{ SetField(58, value); }
		
			const std::string& GetActionID() const
				{ return GetField(59); } 
			void SetActionID( const std::string& value )
				{ SetField(59, value); }
		
			const std::string& GetNewOrderID() const
				{ return GetField(60); } 
			void SetNewOrderID( const std::string& value )
				{ SetField(60, value); }
		
	
	};	

	class Status : public Message
	{
		public:
			Status();
			Status( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetTrader() const
				{ return GetField(2); } 
			void SetTrader( const std::string& value )
				{ SetField(2, value); }
		
			const std::string& GetTag() const
				{ return GetField(3); } 
			void SetTag( const std::string& value )
				{ SetField(3, value); }
		
			int GetSide() const
				{ return ConvertStringToInt( GetField(4) ); }
			void SetSide( int value )
				{ SetField( 4, ConvertIntToString(value) ); }
		
			const std::string& GetSymbol() const
				{ return GetField(5); } 
			void SetSymbol( const std::string& value )
				{ SetField(5, value); }
		
			int GetShares() const
				{ return ConvertStringToInt( GetField(6) ); }
			void SetShares( int value )
				{ SetField( 6, ConvertIntToString(value) ); }
		
			double GetPrice() const
				{ return ConvertStringToDouble( GetField(7) ); }
			void SetPrice( double value )
				{ SetField( 7, ConvertDoubleToString(value) ); }
		
			int GetType() const
				{ return ConvertStringToInt( GetField(8) ); }
			void SetType( int value )
				{ SetField( 8, ConvertIntToString(value) ); }
		
			char GetTIF() const
				{ return ConvertStringToChar( GetField(9) ); }
			void SetTIF( char value )
				{ SetField( 9, ConvertCharToString(value) ); }
		
			char GetCapacity() const
				{ return ConvertStringToChar( GetField(10) ); }
			void SetCapacity( char value )
				{ SetField( 10, ConvertCharToString(value) ); }
		
			const std::string& GetClearingID() const
				{ return GetField(11); } 
			void SetClearingID( const std::string& value )
				{ SetField(11, value); }
		
			const std::string& GetClearingAccount() const
				{ return GetField(12); } 
			void SetClearingAccount( const std::string& value )
				{ SetField(12, value); }
		
			int GetExchange() const
				{ return ConvertStringToInt( GetField(13) ); }
			void SetExchange( int value )
				{ SetField( 13, ConvertIntToString(value) ); }
		
			int GetStatus() const
				{ return ConvertStringToInt( GetField(14) ); }
			void SetStatus( int value )
				{ SetField( 14, ConvertIntToString(value) ); }
		
			const std::string& GetOrderTag() const
				{ return GetField(15); } 
			void SetOrderTag( const std::string& value )
				{ SetField(15, value); }
		
			const std::string& GetExchangeTag() const
				{ return GetField(16); } 
			void SetExchangeTag( const std::string& value )
				{ SetField(16, value); }
		
			const std::string& GetFirmTag() const
				{ return GetField(17); } 
			void SetFirmTag( const std::string& value )
				{ SetField(17, value); }
		
			const std::string& GetCancelTag() const
				{ return GetField(18); } 
			void SetCancelTag( const std::string& value )
				{ SetField(18, value); }
		
			const std::string& GetExecTag() const
				{ return GetField(19); } 
			void SetExecTag( const std::string& value )
				{ SetField(19, value); }
		
			const std::string& GetExecConfirm() const
				{ return GetField(20); } 
			void SetExecConfirm( const std::string& value )
				{ SetField(20, value); }
		
			const std::string& GetReference() const
				{ return GetField(21); } 
			void SetReference( const std::string& value )
				{ SetField(21, value); }
		
			char GetReport() const
				{ return ConvertStringToChar( GetField(22) ); }
			void SetReport( char value )
				{ SetField( 22, ConvertCharToString(value) ); }
		
			const std::string& GetContra() const
				{ return GetField(23); } 
			void SetContra( const std::string& value )
				{ SetField(23, value); }
		
			const std::string& GetTradeFor() const
				{ return GetField(24); } 
			void SetTradeFor( const std::string& value )
				{ SetField(24, value); }
		
			const std::string& GetQueryTag() const
				{ return GetField(25); } 
			void SetQueryTag( const std::string& value )
				{ SetField(25, value); }
		
			const std::string& GetText() const
				{ return GetField(26); } 
			void SetText( const std::string& value )
				{ SetField(26, value); }
		
			const std::string& GetOwner() const
				{ return GetField(27); } 
			void SetOwner( const std::string& value )
				{ SetField(27, value); }
		
			const std::string& GetOMExecTag() const
				{ return GetField(28); } 
			void SetOMExecTag( const std::string& value )
				{ SetField(28, value); }
		
			const std::string& GetExpirationDate() const
				{ return GetField(30); } 
			void SetExpirationDate( const std::string& value )
				{ SetField(30, value); }
		
			char GetOptionType() const
				{ return ConvertStringToChar( GetField(31) ); }
			void SetOptionType( char value )
				{ SetField( 31, ConvertCharToString(value) ); }
		
			double GetStrikePrice() const
				{ return ConvertStringToDouble( GetField(32) ); }
			void SetStrikePrice( double value )
				{ SetField( 32, ConvertDoubleToString(value) ); }
		
			const std::string& GetUnderlying() const
				{ return GetField(33); } 
			void SetUnderlying( const std::string& value )
				{ SetField(33, value); }
		
			char GetSecurityType() const
				{ return ConvertStringToChar( GetField(35) ); }
			void SetSecurityType( char value )
				{ SetField( 35, ConvertCharToString(value) ); }
		
			int GetOpenClose() const
				{ return ConvertStringToInt( GetField(38) ); }
			void SetOpenClose( int value )
				{ SetField( 38, ConvertIntToString(value) ); }
		
			const std::string& GetAccount() const
				{ return GetField(45); } 
			void SetAccount( const std::string& value )
				{ SetField(45, value); }
		
			double GetCumQty() const
				{ return ConvertStringToDouble( GetField(48) ); }
			void SetCumQty( double value )
				{ SetField( 48, ConvertDoubleToString(value) ); }
		
			double GetLeaveQty() const
				{ return ConvertStringToDouble( GetField(49) ); }
			void SetLeaveQty( double value )
				{ SetField( 49, ConvertDoubleToString(value) ); }
		
			const std::string& GetExecutionTime() const
				{ return GetField(50); } 
			void SetExecutionTime( const std::string& value )
				{ SetField(50, value); }
		
			double GetAvgPx() const
				{ return ConvertStringToDouble( GetField(51) ); }
			void SetAvgPx( double value )
				{ SetField( 51, ConvertDoubleToString(value) ); }
		
			const std::string& GetActionID() const
				{ return GetField(59); } 
			void SetActionID( const std::string& value )
				{ SetField(59, value); }
		
			const std::string& GetNewOrderID() const
				{ return GetField(60); } 
			void SetNewOrderID( const std::string& value )
				{ SetField(60, value); }
		
			int GetOrderStatus() const
				{ return ConvertStringToInt( GetField(61) ); }
			void SetOrderStatus( int value )
				{ SetField( 61, ConvertIntToString(value) ); }
		
	
	};	

	class Invalid : public Message
	{
		public:
			Invalid();
			Invalid( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetText() const
				{ return GetField(26); } 
			void SetText( const std::string& value )
				{ SetField(26, value); }
		
	
	};	

	class Query : public Message
	{
		public:
			Query();
			Query( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetOwner() const
				{ return GetField(27); } 
			void SetOwner( const std::string& value )
				{ SetField(27, value); }
		
	
	};	

	class Logout : public Message
	{
		public:
			Logout();
			Logout( const Message& message );


			time_t GetTimestamp() const
				{ return ConvertStringToTime( GetField(1) ); }
			void SetTimestamp( time_t value )
				{ SetField( 1, ConvertTimeToString(value) ); }
		
			const std::string& GetLoginName() const
				{ return GetField(2); } 
			void SetLoginName( const std::string& value )
				{ SetField(2, value); }
		
	
	};	

}

#endif
