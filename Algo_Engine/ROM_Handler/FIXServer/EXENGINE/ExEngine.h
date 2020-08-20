/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Aug 10 10:15:20 2005
 */
/* Compiler settings for C:\Projects\OrderExecution\SERVERS\FIXServer\EXENGINE\ExEngine.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ExEngine_h__
#define __ExEngine_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IOrder_FWD_DEFINED__
#define __IOrder_FWD_DEFINED__
typedef interface IOrder IOrder;
#endif 	/* __IOrder_FWD_DEFINED__ */


#ifndef __IStatusHandler_FWD_DEFINED__
#define __IStatusHandler_FWD_DEFINED__
typedef interface IStatusHandler IStatusHandler;
#endif 	/* __IStatusHandler_FWD_DEFINED__ */


#ifndef __ITradeEngine_FWD_DEFINED__
#define __ITradeEngine_FWD_DEFINED__
typedef interface ITradeEngine ITradeEngine;
#endif 	/* __ITradeEngine_FWD_DEFINED__ */


#ifndef __TradeEngine_FWD_DEFINED__
#define __TradeEngine_FWD_DEFINED__

#ifdef __cplusplus
typedef class TradeEngine TradeEngine;
#else
typedef struct TradeEngine TradeEngine;
#endif /* __cplusplus */

#endif 	/* __TradeEngine_FWD_DEFINED__ */


#ifndef __Order_FWD_DEFINED__
#define __Order_FWD_DEFINED__

#ifdef __cplusplus
typedef class Order Order;
#else
typedef struct Order Order;
#endif /* __cplusplus */

#endif 	/* __Order_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_ExEngine_0000 */
/* [local] */ 


enum OrderType
    {	eMarket	= 0,
	eLimit	= eMarket + 1
    };

enum TimeInForce
    {	eDay	= 0,
	eMOC	= eDay + 1,
	eGTC	= eMOC + 1,
	eGTD	= eGTC + 1
    };

enum SecType
    {	eEquity	= 0,
	eOption	= eEquity + 1,
	eFuture	= eOption + 1
    };

enum OrderState
    {	eNew	= 0,
	eOpen	= eNew + 1,
	eClose	= eOpen + 1
    };

enum OrderModifier
    {	ePending	= 0,
	eAcknowledged	= ePending + 1,
	ePartialFilled	= eAcknowledged + 1,
	eFilled	= ePartialFilled + 1,
	eCancelPending	= eFilled + 1,
	eCancelReject	= eCancelPending + 1,
	eCancelled	= eCancelReject + 1,
	eStopped	= eCancelled + 1,
	eSuspended	= eStopped + 1,
	eExpired	= eSuspended + 1,
	eReplacePending	= eExpired + 1,
	eCalculated	= eReplacePending + 1,
	eRejected	= eCalculated + 1,
	eReplaced	= eRejected + 1
    };

enum OrderSide
    {	eBuy	= 0,
	eSell	= eBuy + 1,
	eShort	= eSell + 1
    };

enum OpenClose
    {	eOpenClose_Open	= 0,
	eOpenClose_Close	= eOpenClose_Open + 1
    };

enum CallPut
    {	eCallPut_Call	= 0,
	eCallPut_Put	= eCallPut_Call + 1
    };

enum ExecInst
    {	eExecInst_AON	= 0,
	eExecInst_None	= eExecInst_AON + 1
    };

enum Capacity
    {	eCapacity_Agent	= 'A',
	eCapacity_Principal	= 'P',
	eCapacity_BrokerDealer	= 'R',
	eCapacity_MarketMaker	= 'S',
	eCapacity_PrimaryMM	= 'T',
	eCapacity_Linkage	= 'U',
	eCapacity_Customer	= 'C',
	eCapacity_MarketMakerFirm	= 'M',
	eCapacity_BrokerDealerFirm	= 'B',
	eCapacity_FirmFirm	= 'F'
    };


extern RPC_IF_HANDLE __MIDL_itf_ExEngine_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ExEngine_0000_v0_0_s_ifspec;

#ifndef __IOrder_INTERFACE_DEFINED__
#define __IOrder_INTERFACE_DEFINED__

/* interface IOrder */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IOrder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A026997E-B143-4963-8513-B54A837A84AA")
    IOrder : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClientOrderID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ClientOrderID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Symbol( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Symbol( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrgQty( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrgQty( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LeaveQty( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LeaveQty( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CumQty( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CumQty( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Price( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Price( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AvgPrice( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AvgPrice( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SecType( 
            /* [retval][out] */ enum SecType __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SecType( 
            /* [in] */ enum SecType newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrderState( 
            /* [retval][out] */ enum OrderState __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrderState( 
            /* [in] */ enum OrderState newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeInForce( 
            /* [retval][out] */ enum TimeInForce __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TimeInForce( 
            /* [in] */ enum TimeInForce newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrderModifier( 
            /* [retval][out] */ enum OrderModifier __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrderModifier( 
            /* [in] */ enum OrderModifier newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrderType( 
            /* [retval][out] */ enum OrderType __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrderType( 
            /* [in] */ enum OrderType newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OrderSide( 
            /* [retval][out] */ enum OrderSide __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OrderSide( 
            /* [in] */ enum OrderSide newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Account( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Account( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Destination( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Destination( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastShares( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LastShares( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExecPrice( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExecPrice( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Strike( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Strike( 
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Expiration( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Expiration( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CallPut( 
            /* [retval][out] */ enum CallPut __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CallPut( 
            /* [in] */ enum CallPut newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OpenClose( 
            /* [retval][out] */ enum OpenClose __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OpenClose( 
            /* [in] */ enum OpenClose newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Text( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Text( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExecID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExecID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CMTA( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CMTA( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_GiveUp( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_GiveUp( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShortLender( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShortLender( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExecInst( 
            /* [retval][out] */ enum ExecInst __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExecInst( 
            /* [in] */ enum ExecInst newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Capacity( 
            /* [retval][out] */ enum Capacity __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Capacity( 
            /* [in] */ enum Capacity newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MMAccount( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MMAccount( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HomeExchange( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HomeExchange( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOrderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOrder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOrder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOrder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IOrder __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IOrder __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IOrder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IOrder __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClientOrderID )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClientOrderID )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Symbol )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Symbol )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OrgQty )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OrgQty )( 
            IOrder __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LeaveQty )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LeaveQty )( 
            IOrder __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CumQty )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CumQty )( 
            IOrder __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Price )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Price )( 
            IOrder __RPC_FAR * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AvgPrice )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AvgPrice )( 
            IOrder __RPC_FAR * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SecType )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum SecType __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SecType )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum SecType newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OrderState )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum OrderState __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OrderState )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum OrderState newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TimeInForce )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum TimeInForce __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TimeInForce )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum TimeInForce newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OrderModifier )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum OrderModifier __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OrderModifier )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum OrderModifier newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OrderType )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum OrderType __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OrderType )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum OrderType newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OrderSide )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum OrderSide __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OrderSide )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum OrderSide newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Account )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Account )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Destination )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Destination )( 
            IOrder __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LastShares )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LastShares )( 
            IOrder __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExecPrice )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ExecPrice )( 
            IOrder __RPC_FAR * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Strike )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Strike )( 
            IOrder __RPC_FAR * This,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Expiration )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Expiration )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CallPut )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum CallPut __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CallPut )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum CallPut newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OpenClose )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum OpenClose __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OpenClose )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum OpenClose newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Text )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Text )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExecID )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ExecID )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CMTA )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CMTA )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GiveUp )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GiveUp )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShortLender )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShortLender )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExecInst )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum ExecInst __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ExecInst )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum ExecInst newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Capacity )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ enum Capacity __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Capacity )( 
            IOrder __RPC_FAR * This,
            /* [in] */ enum Capacity newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MMAccount )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MMAccount )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HomeExchange )( 
            IOrder __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HomeExchange )( 
            IOrder __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IOrderVtbl;

    interface IOrder
    {
        CONST_VTBL struct IOrderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOrder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOrder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOrder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOrder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IOrder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IOrder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IOrder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IOrder_get_ClientOrderID(This,pVal)	\
    (This)->lpVtbl -> get_ClientOrderID(This,pVal)

#define IOrder_put_ClientOrderID(This,newVal)	\
    (This)->lpVtbl -> put_ClientOrderID(This,newVal)

#define IOrder_get_Symbol(This,pVal)	\
    (This)->lpVtbl -> get_Symbol(This,pVal)

#define IOrder_put_Symbol(This,newVal)	\
    (This)->lpVtbl -> put_Symbol(This,newVal)

#define IOrder_get_OrgQty(This,pVal)	\
    (This)->lpVtbl -> get_OrgQty(This,pVal)

#define IOrder_put_OrgQty(This,newVal)	\
    (This)->lpVtbl -> put_OrgQty(This,newVal)

#define IOrder_get_LeaveQty(This,pVal)	\
    (This)->lpVtbl -> get_LeaveQty(This,pVal)

#define IOrder_put_LeaveQty(This,newVal)	\
    (This)->lpVtbl -> put_LeaveQty(This,newVal)

#define IOrder_get_CumQty(This,pVal)	\
    (This)->lpVtbl -> get_CumQty(This,pVal)

#define IOrder_put_CumQty(This,newVal)	\
    (This)->lpVtbl -> put_CumQty(This,newVal)

#define IOrder_get_Price(This,pVal)	\
    (This)->lpVtbl -> get_Price(This,pVal)

#define IOrder_put_Price(This,newVal)	\
    (This)->lpVtbl -> put_Price(This,newVal)

#define IOrder_get_AvgPrice(This,pVal)	\
    (This)->lpVtbl -> get_AvgPrice(This,pVal)

#define IOrder_put_AvgPrice(This,newVal)	\
    (This)->lpVtbl -> put_AvgPrice(This,newVal)

#define IOrder_get_SecType(This,pVal)	\
    (This)->lpVtbl -> get_SecType(This,pVal)

#define IOrder_put_SecType(This,newVal)	\
    (This)->lpVtbl -> put_SecType(This,newVal)

#define IOrder_get_OrderState(This,pVal)	\
    (This)->lpVtbl -> get_OrderState(This,pVal)

#define IOrder_put_OrderState(This,newVal)	\
    (This)->lpVtbl -> put_OrderState(This,newVal)

#define IOrder_get_TimeInForce(This,pVal)	\
    (This)->lpVtbl -> get_TimeInForce(This,pVal)

#define IOrder_put_TimeInForce(This,newVal)	\
    (This)->lpVtbl -> put_TimeInForce(This,newVal)

#define IOrder_get_OrderModifier(This,pVal)	\
    (This)->lpVtbl -> get_OrderModifier(This,pVal)

#define IOrder_put_OrderModifier(This,newVal)	\
    (This)->lpVtbl -> put_OrderModifier(This,newVal)

#define IOrder_get_OrderType(This,pVal)	\
    (This)->lpVtbl -> get_OrderType(This,pVal)

#define IOrder_put_OrderType(This,newVal)	\
    (This)->lpVtbl -> put_OrderType(This,newVal)

#define IOrder_get_OrderSide(This,pVal)	\
    (This)->lpVtbl -> get_OrderSide(This,pVal)

#define IOrder_put_OrderSide(This,newVal)	\
    (This)->lpVtbl -> put_OrderSide(This,newVal)

#define IOrder_get_Account(This,pVal)	\
    (This)->lpVtbl -> get_Account(This,pVal)

#define IOrder_put_Account(This,newVal)	\
    (This)->lpVtbl -> put_Account(This,newVal)

#define IOrder_get_Destination(This,pVal)	\
    (This)->lpVtbl -> get_Destination(This,pVal)

#define IOrder_put_Destination(This,newVal)	\
    (This)->lpVtbl -> put_Destination(This,newVal)

#define IOrder_get_LastShares(This,pVal)	\
    (This)->lpVtbl -> get_LastShares(This,pVal)

#define IOrder_put_LastShares(This,newVal)	\
    (This)->lpVtbl -> put_LastShares(This,newVal)

#define IOrder_get_ExecPrice(This,pVal)	\
    (This)->lpVtbl -> get_ExecPrice(This,pVal)

#define IOrder_put_ExecPrice(This,newVal)	\
    (This)->lpVtbl -> put_ExecPrice(This,newVal)

#define IOrder_get_Strike(This,pVal)	\
    (This)->lpVtbl -> get_Strike(This,pVal)

#define IOrder_put_Strike(This,newVal)	\
    (This)->lpVtbl -> put_Strike(This,newVal)

#define IOrder_get_Expiration(This,pVal)	\
    (This)->lpVtbl -> get_Expiration(This,pVal)

#define IOrder_put_Expiration(This,newVal)	\
    (This)->lpVtbl -> put_Expiration(This,newVal)

#define IOrder_get_CallPut(This,pVal)	\
    (This)->lpVtbl -> get_CallPut(This,pVal)

#define IOrder_put_CallPut(This,newVal)	\
    (This)->lpVtbl -> put_CallPut(This,newVal)

#define IOrder_get_OpenClose(This,pVal)	\
    (This)->lpVtbl -> get_OpenClose(This,pVal)

#define IOrder_put_OpenClose(This,newVal)	\
    (This)->lpVtbl -> put_OpenClose(This,newVal)

#define IOrder_get_Text(This,pVal)	\
    (This)->lpVtbl -> get_Text(This,pVal)

#define IOrder_put_Text(This,newVal)	\
    (This)->lpVtbl -> put_Text(This,newVal)

#define IOrder_get_ExecID(This,pVal)	\
    (This)->lpVtbl -> get_ExecID(This,pVal)

#define IOrder_put_ExecID(This,newVal)	\
    (This)->lpVtbl -> put_ExecID(This,newVal)

#define IOrder_get_CMTA(This,pVal)	\
    (This)->lpVtbl -> get_CMTA(This,pVal)

#define IOrder_put_CMTA(This,newVal)	\
    (This)->lpVtbl -> put_CMTA(This,newVal)

#define IOrder_get_GiveUp(This,pVal)	\
    (This)->lpVtbl -> get_GiveUp(This,pVal)

#define IOrder_put_GiveUp(This,newVal)	\
    (This)->lpVtbl -> put_GiveUp(This,newVal)

#define IOrder_get_ShortLender(This,pVal)	\
    (This)->lpVtbl -> get_ShortLender(This,pVal)

#define IOrder_put_ShortLender(This,newVal)	\
    (This)->lpVtbl -> put_ShortLender(This,newVal)

#define IOrder_get_ExecInst(This,pVal)	\
    (This)->lpVtbl -> get_ExecInst(This,pVal)

#define IOrder_put_ExecInst(This,newVal)	\
    (This)->lpVtbl -> put_ExecInst(This,newVal)

#define IOrder_get_Capacity(This,pVal)	\
    (This)->lpVtbl -> get_Capacity(This,pVal)

#define IOrder_put_Capacity(This,newVal)	\
    (This)->lpVtbl -> put_Capacity(This,newVal)

#define IOrder_get_MMAccount(This,pVal)	\
    (This)->lpVtbl -> get_MMAccount(This,pVal)

#define IOrder_put_MMAccount(This,newVal)	\
    (This)->lpVtbl -> put_MMAccount(This,newVal)

#define IOrder_get_HomeExchange(This,pVal)	\
    (This)->lpVtbl -> get_HomeExchange(This,pVal)

#define IOrder_put_HomeExchange(This,newVal)	\
    (This)->lpVtbl -> put_HomeExchange(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_ClientOrderID_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_ClientOrderID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_ClientOrderID_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_ClientOrderID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Symbol_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Symbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Symbol_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_Symbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_OrgQty_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_OrgQty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_OrgQty_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IOrder_put_OrgQty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_LeaveQty_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_LeaveQty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_LeaveQty_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IOrder_put_LeaveQty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_CumQty_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_CumQty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_CumQty_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IOrder_put_CumQty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Price_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Price_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Price_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ double newVal);


void __RPC_STUB IOrder_put_Price_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_AvgPrice_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_AvgPrice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_AvgPrice_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ double newVal);


void __RPC_STUB IOrder_put_AvgPrice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_SecType_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum SecType __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_SecType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_SecType_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum SecType newVal);


void __RPC_STUB IOrder_put_SecType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_OrderState_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum OrderState __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_OrderState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_OrderState_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum OrderState newVal);


void __RPC_STUB IOrder_put_OrderState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_TimeInForce_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum TimeInForce __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_TimeInForce_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_TimeInForce_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum TimeInForce newVal);


void __RPC_STUB IOrder_put_TimeInForce_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_OrderModifier_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum OrderModifier __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_OrderModifier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_OrderModifier_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum OrderModifier newVal);


void __RPC_STUB IOrder_put_OrderModifier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_OrderType_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum OrderType __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_OrderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_OrderType_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum OrderType newVal);


void __RPC_STUB IOrder_put_OrderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_OrderSide_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum OrderSide __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_OrderSide_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_OrderSide_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum OrderSide newVal);


void __RPC_STUB IOrder_put_OrderSide_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Account_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Account_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Account_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_Account_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Destination_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Destination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Destination_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IOrder_put_Destination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_LastShares_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_LastShares_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_LastShares_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IOrder_put_LastShares_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_ExecPrice_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_ExecPrice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_ExecPrice_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ double newVal);


void __RPC_STUB IOrder_put_ExecPrice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Strike_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Strike_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Strike_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ double newVal);


void __RPC_STUB IOrder_put_Strike_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Expiration_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Expiration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Expiration_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_Expiration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_CallPut_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum CallPut __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_CallPut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_CallPut_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum CallPut newVal);


void __RPC_STUB IOrder_put_CallPut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_OpenClose_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum OpenClose __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_OpenClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_OpenClose_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum OpenClose newVal);


void __RPC_STUB IOrder_put_OpenClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Text_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Text_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_Text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_ExecID_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_ExecID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_ExecID_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_ExecID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_CMTA_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_CMTA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_CMTA_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_CMTA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_GiveUp_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_GiveUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_GiveUp_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_GiveUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_ShortLender_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_ShortLender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_ShortLender_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_ShortLender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_ExecInst_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum ExecInst __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_ExecInst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_ExecInst_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum ExecInst newVal);


void __RPC_STUB IOrder_put_ExecInst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_Capacity_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ enum Capacity __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_Capacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_Capacity_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ enum Capacity newVal);


void __RPC_STUB IOrder_put_Capacity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_MMAccount_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_MMAccount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_MMAccount_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_MMAccount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IOrder_get_HomeExchange_Proxy( 
    IOrder __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOrder_get_HomeExchange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IOrder_put_HomeExchange_Proxy( 
    IOrder __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOrder_put_HomeExchange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOrder_INTERFACE_DEFINED__ */


#ifndef __IStatusHandler_INTERFACE_DEFINED__
#define __IStatusHandler_INTERFACE_DEFINED__

/* interface IStatusHandler */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IStatusHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("46E010E4-B292-402F-9D3D-C53353D7072E")
    IStatusHandler : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnOrderStatus( 
            /* [in] */ IOrder __RPC_FAR *pOrder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnSystemStatus( 
            /* [in] */ BSTR szMsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStatusHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IStatusHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IStatusHandler __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IStatusHandler __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IStatusHandler __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IStatusHandler __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IStatusHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IStatusHandler __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnOrderStatus )( 
            IStatusHandler __RPC_FAR * This,
            /* [in] */ IOrder __RPC_FAR *pOrder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnSystemStatus )( 
            IStatusHandler __RPC_FAR * This,
            /* [in] */ BSTR szMsg);
        
        END_INTERFACE
    } IStatusHandlerVtbl;

    interface IStatusHandler
    {
        CONST_VTBL struct IStatusHandlerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStatusHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IStatusHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IStatusHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IStatusHandler_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IStatusHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IStatusHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IStatusHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IStatusHandler_OnOrderStatus(This,pOrder)	\
    (This)->lpVtbl -> OnOrderStatus(This,pOrder)

#define IStatusHandler_OnSystemStatus(This,szMsg)	\
    (This)->lpVtbl -> OnSystemStatus(This,szMsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IStatusHandler_OnOrderStatus_Proxy( 
    IStatusHandler __RPC_FAR * This,
    /* [in] */ IOrder __RPC_FAR *pOrder);


void __RPC_STUB IStatusHandler_OnOrderStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IStatusHandler_OnSystemStatus_Proxy( 
    IStatusHandler __RPC_FAR * This,
    /* [in] */ BSTR szMsg);


void __RPC_STUB IStatusHandler_OnSystemStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStatusHandler_INTERFACE_DEFINED__ */


#ifndef __ITradeEngine_INTERFACE_DEFINED__
#define __ITradeEngine_INTERFACE_DEFINED__

/* interface ITradeEngine */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITradeEngine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A9EBA757-1FD3-4BB5-86B0-ADA2870A6898")
    ITradeEngine : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateNewOrder( 
            /* [out] */ IOrder __RPC_FAR *__RPC_FAR *ppOrder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SubmitOrder( 
            /* [in] */ IOrder __RPC_FAR *pOrder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelOrder( 
            /* [in] */ IOrder __RPC_FAR *pOrder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReplaceOrder( 
            /* [in] */ IOrder __RPC_FAR *pOrder,
            /* [in] */ BSTR szNewClientID,
            /* [in] */ long nNewQty,
            /* [in] */ double nNewPrice) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ BSTR szPath,
            /* [in] */ IStatusHandler __RPC_FAR *pHandler) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Shutdown( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITradeEngineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITradeEngine __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITradeEngine __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITradeEngine __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateNewOrder )( 
            ITradeEngine __RPC_FAR * This,
            /* [out] */ IOrder __RPC_FAR *__RPC_FAR *ppOrder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SubmitOrder )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ IOrder __RPC_FAR *pOrder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CancelOrder )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ IOrder __RPC_FAR *pOrder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReplaceOrder )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ IOrder __RPC_FAR *pOrder,
            /* [in] */ BSTR szNewClientID,
            /* [in] */ long nNewQty,
            /* [in] */ double nNewPrice);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            ITradeEngine __RPC_FAR * This,
            /* [in] */ BSTR szPath,
            /* [in] */ IStatusHandler __RPC_FAR *pHandler);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Shutdown )( 
            ITradeEngine __RPC_FAR * This);
        
        END_INTERFACE
    } ITradeEngineVtbl;

    interface ITradeEngine
    {
        CONST_VTBL struct ITradeEngineVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITradeEngine_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITradeEngine_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITradeEngine_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITradeEngine_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITradeEngine_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITradeEngine_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITradeEngine_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITradeEngine_CreateNewOrder(This,ppOrder)	\
    (This)->lpVtbl -> CreateNewOrder(This,ppOrder)

#define ITradeEngine_SubmitOrder(This,pOrder)	\
    (This)->lpVtbl -> SubmitOrder(This,pOrder)

#define ITradeEngine_CancelOrder(This,pOrder)	\
    (This)->lpVtbl -> CancelOrder(This,pOrder)

#define ITradeEngine_ReplaceOrder(This,pOrder,szNewClientID,nNewQty,nNewPrice)	\
    (This)->lpVtbl -> ReplaceOrder(This,pOrder,szNewClientID,nNewQty,nNewPrice)

#define ITradeEngine_Init(This,szPath,pHandler)	\
    (This)->lpVtbl -> Init(This,szPath,pHandler)

#define ITradeEngine_Shutdown(This)	\
    (This)->lpVtbl -> Shutdown(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITradeEngine_CreateNewOrder_Proxy( 
    ITradeEngine __RPC_FAR * This,
    /* [out] */ IOrder __RPC_FAR *__RPC_FAR *ppOrder);


void __RPC_STUB ITradeEngine_CreateNewOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITradeEngine_SubmitOrder_Proxy( 
    ITradeEngine __RPC_FAR * This,
    /* [in] */ IOrder __RPC_FAR *pOrder);


void __RPC_STUB ITradeEngine_SubmitOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITradeEngine_CancelOrder_Proxy( 
    ITradeEngine __RPC_FAR * This,
    /* [in] */ IOrder __RPC_FAR *pOrder);


void __RPC_STUB ITradeEngine_CancelOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITradeEngine_ReplaceOrder_Proxy( 
    ITradeEngine __RPC_FAR * This,
    /* [in] */ IOrder __RPC_FAR *pOrder,
    /* [in] */ BSTR szNewClientID,
    /* [in] */ long nNewQty,
    /* [in] */ double nNewPrice);


void __RPC_STUB ITradeEngine_ReplaceOrder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITradeEngine_Init_Proxy( 
    ITradeEngine __RPC_FAR * This,
    /* [in] */ BSTR szPath,
    /* [in] */ IStatusHandler __RPC_FAR *pHandler);


void __RPC_STUB ITradeEngine_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ITradeEngine_Shutdown_Proxy( 
    ITradeEngine __RPC_FAR * This);


void __RPC_STUB ITradeEngine_Shutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITradeEngine_INTERFACE_DEFINED__ */



#ifndef __EXENGINELib_LIBRARY_DEFINED__
#define __EXENGINELib_LIBRARY_DEFINED__

/* library EXENGINELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_EXENGINELib;

EXTERN_C const CLSID CLSID_TradeEngine;

#ifdef __cplusplus

class DECLSPEC_UUID("A5E302A7-3D1F-4C72-B613-047956EEADE0")
TradeEngine;
#endif

EXTERN_C const CLSID CLSID_Order;

#ifdef __cplusplus

class DECLSPEC_UUID("B480D0B5-F2B8-4F6D-B5D9-B22992E30024")
Order;
#endif
#endif /* __EXENGINELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
