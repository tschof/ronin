using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Web;
using System.Web.Services;
using Domain;
using Transport;
using System.Threading;

namespace OptionManWebService
{
	/// <summary>
	/// Summary description for OptionManWebService.
	/// </summary>
	/// 

	public class OptionManService : System.Web.Services.WebService
	{
		public OptionManService()
		{
			//CODEGEN: This call is required by the ASP.NET Web Services Designer
			//ClientSocket.getInstance();
			InitializeComponent();
		}

		private bool IsValidToken( string szToken )
		{
			try
			{
				return AccountProfiles.IsValidToken( szToken );
			}
			catch(Exception)
			{
				return false;
			}

		}

		#region Component Designer generated code
		
		//Required by the Web Services Designer 
		private IContainer components = null;
				
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if(disposing && components != null)
			{
				components.Dispose();
			}
			base.Dispose(disposing);		
		}
		
		#endregion

		[WebMethod]
		public string SecureLogon( string szUser, string szPassword )
		{
			AccountProfiles profiles = new AccountProfiles();
			try
			{
				return	profiles.Logon( szUser, szPassword );
			}
			catch( Exception e )
			{
				throw e;
			}
		}
		[WebMethod]
		public AccountProfiles SecureGetAccountProfiles( string szToken, string szUser, string szPassword )
		{
			if( !IsValidToken( szToken ) )
				return null;
			AccountProfiles profiles = new AccountProfiles();
			if( !profiles.Load( szUser ) ) //if( !profiles.Load( szUser, szPassword ) )
				return null;
			return profiles;
		}

		[WebMethod]
		public Orders SecureGetOrders( string szToken, string szUser )
		{
			if( !IsValidToken( szToken ) )
				return null;
			Orders orders = new Orders();
			orders.Load( szUser );
			return orders;
		}

		[WebMethod]
		public Executions SecureGetExecutions( string szToken, string szUser )
		{
			if( !IsValidToken( szToken ) )
				return null;
			Executions exes = new Executions();
			exes.Load( szUser );
			return exes;
		}

		[WebMethod]
		public Positions SecureGetPositions( string szToken, AccountID profile )
		{
			if( !IsValidToken( szToken ) )
				return null;
			return profile.GetPositions();
		}

		[WebMethod]
		public Positions AdminGetPositions( string szPass, string szTraderAcr, string szLocalAcct, string szClgAcct, string szFirm )
		{
			if( szPass != "opman!" )
				throw new Exception("Wrong password");
			AccountID profile = new AccountID();
			profile.m_TraderAcr = szTraderAcr;
			profile.m_LocalAcct = szLocalAcct;
			profile.m_ClgFirm = szFirm;
			profile.m_ClgAccount = szClgAcct;
			return profile.GetPositions();
		}

		[WebMethod]
		public AccountProfiles AdminGetAccountProfiles( string szPass, string szUser )
		{
			if( szPass != "opman!" )
				throw new Exception("Wrong password");
			AccountProfiles profiles = new AccountProfiles();
			if( !profiles.Load( szUser ) ) 
				return null;
			return profiles;
		}

		[WebMethod]
		public Executions AdminGetExecutions( string szPass, string szUser )
		{
			if( szPass != "opman!" )
				throw new Exception("Wrong password");
			Executions exes = new Executions();
			exes.Load( szUser );
			return exes;
		}

		[WebMethod]
		public Orders AdminGetOrders( string szPass, string szUser )
		{
			if( szPass != "opman!" )
				throw new Exception("Wrong password");
			Orders orders = new Orders();
			orders.Load( szUser );
			return orders;
		}

#if INTERNET
		[WebMethod]
		public Destinations SecureGetDestinations( string szToken, AccountProfile profile )
		{
			if( !IsValidToken( szToken ) )
				return null;
			return profile.GetDestinations();
		}
		
		[WebMethod]
		public string HelloWorld()
		{
			Domain.Order order = new Domain.Order();
			SubmitOrder( order );
			return "Hello World";
		}

		[WebMethod]
		public AccountProfiles Logon( string szUser, string szPassword )
		{
			AccountProfiles profiles = new AccountProfiles();
			if( !profiles.Load( szUser ) )
				return null;
			return profiles;
		}

		[WebMethod]
		public Orders GetOrders( AccountID profile )
		{
			return profile.GetOrders();
		}

		[WebMethod]
		public Destinations GetDestinations( AccountProfile profile )
		{
			return profile.GetDestinations();
		}

		[WebMethod]
		public Executions GetExecutions( AccountID profile )
		{
			return profile.GetExecutions();
		}

		[WebMethod]
		public Positions GetPositions( AccountID profile )
		{
			return profile.GetPositions();
		}

		[WebMethod]
		public Domain.RequestResponse SubmitOrder( Domain.Order order )
		{
			OMConnection.getInstance().Send( order.Encode(), true );			
			return new Domain.RequestResponse("Order Submitted!");
		}
		
		[WebMethod]
		public Domain.RequestResponse CancelOrder( Domain.Order order )
		{
			return new Domain.RequestResponse("Not Implemented");
		}

		[WebMethod]
		public Domain.RequestResponse ReviseOrder( Domain.Order order )
		{
			return new Domain.RequestResponse("Not Implemented");
		}

		[WebMethod]
		public bool ShutDown()
		{
			try
			{
				OMConnection.getInstance().ShutDown();
				OMDSConnection.getInstance().ShutDown();
			}
			catch(Exception)
			{
			}
			return true;
		}

		[WebMethod]
		public bool Initialize()
		{
			OMConnection.getInstance();
			OMDSConnection.getInstance();
			return true;
		}

		[WebMethod]
		public Expire GetExpires( OptionSeries series, string szExpire )
		{
			for( int i = 0; i < series.m_Options.Count; i++ )
			{
				Expire exp = (Expire)series.m_Options[i];
				if( exp.m_szExpire == szExpire )
					return exp;
			}
			return null;
		}

		[WebMethod]
		public Exchange[] GetExchange( OptionSeries series )
		{
			Exchange[] ex = new Exchange[series.m_Exchanges.Count];
			for( int i = 0; i < series.m_Exchanges.Count; i++ )
			{
				ex[i] = (Exchange)series.m_Exchanges[i];
			}
			return ex;
		}

		[WebMethod]
		public Strike GetStrikes( Expire expire, float fStrike )
		{
			for( int i = 0; i < expire.m_Strikes.Count; i++ )
			{
				Strike st = (Strike)expire.m_Strikes[i];
				if( st.m_fStrike == fStrike )
					return st;
			}
			return null;
		}

		[WebMethod]
		public OptionSeries GetOptionSeries( string szBase )
		{
			if( szBase.Length == 0 )
			{
				OptionSeries newSeries = new OptionSeries();
				newSeries.Test();
				return newSeries;
			}
			OptionSeries series = null;
			int nRetry = 0;
			while( nRetry < 3 )
			{
				series = QuoteManager.getInstance().GetOptionSeries(szBase);
				if( series == null )
					Thread.Sleep(10000);
				else
					break;
				nRetry++;
			}
			return series;
		}

		[WebMethod]
		public QuoteOption GetOptionQuote( string szBase )
		{
			QuoteOption quote = null;
			int nRetry = 0;
			while( nRetry < 3 )
			{
				quote = QuoteManager.getInstance().GetOptionQuote(szBase);
				if( quote != null )
					break;
				nRetry++;
				Thread.Sleep(1000);
			}
			return quote;
		}

		[WebMethod]
		public Quote GetStockQuote( string szSymbol )
		{
			Quote quote = null;
			int nRetry = 0;
			while( nRetry < 3 )
			{
				quote = QuoteManager.getInstance().GetStockQuote(szSymbol);
				if( quote != null )
					break;
				nRetry++;
				Thread.Sleep(1000);
			}
			return quote;
		}
#endif

	}
}
