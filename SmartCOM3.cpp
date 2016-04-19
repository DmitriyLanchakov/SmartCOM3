
#include "SmartCOM3.h"
#include <iomanip>
#include <thread>
#include <chrono>

namespace SmartCOM3
{
	/* IUnknown */

	STDMETHODIMP_(ULONG) IStClient::AddRef(void)
	{
		return InterlockedIncrement(&m_cRef);
	}
	STDMETHODIMP_(ULONG) IStClient::Release(void)
	{
		LONG cRef = InterlockedDecrement(&m_cRef);
		if (cRef == 0) delete this;
		return cRef;
	}
	STDMETHODIMP IStClient::QueryInterface(REFIID iid, void **ppv)
	{
		*ppv = nullptr;
		HRESULT hr = E_NOINTERFACE;

		if (iid == IID_IUnknown || iid == IID_IDispatch || iid == DIID_IStClient)
		{
			*ppv = static_cast<IStClient*>(static_cast<IDispatch*>(this));
			AddRef();
			hr = S_OK;
		}

		return hr;
	}

	/* IDispatch */

	STDMETHODIMP IStClient::GetTypeInfoCount(UINT *pctinfo)
	{
		if (pctinfo == NULL) return E_NOTIMPL;
		*pctinfo = 1;
		return NOERROR;
	}
	STDMETHODIMP IStClient::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo **pptinfo)
	{
		if (pptinfo == NULL) return E_INVALIDARG;
		if (itinfo != 0) return DISP_E_BADINDEX;
		*pptinfo = NULL;
		m_ptinfo->AddRef();
		*pptinfo = m_ptinfo;
		return NOERROR;
	}
	STDMETHODIMP IStClient::GetIDsOfNames(REFIID riid, OLECHAR **rgszNames, UINT cNames,
		LCID lcid, DISPID *rgdispid)
	{
		return m_ptinfo->GetIDsOfNames(rgszNames, cNames, rgdispid);
	}
	STDMETHODIMP IStClient::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS *pdispparams, VARIANT *pvarResult, EXCEPINFO *pexcepinfo, UINT *puArgErr)
	{
		if (pvarResult) VariantInit(pvarResult);

		InvokeDispatch(dispidMember, pdispparams);

		return S_OK;
	}
	void IStClient::InvokeDispatch(DISPID dispidMember, DISPPARAMS *pdispparams)
	{
		switch (dispidMember)
		{
		case 0x01: {
		   Connected();
		   break;
		}
		case 0x02: {
		   Disconnected(
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string reason
		   );
		   break;
		}
		case 0x03: {
		   UpdateQuote(
			   ws2s(pdispparams->rgvarg[22].bstrVal).c_str(), // std::string symbol
			   d2t(pdispparams->rgvarg[21].dblVal), // time_t datetime
			   static_cast<double>(pdispparams->rgvarg[20].dblVal), // double open
			   static_cast<double>(pdispparams->rgvarg[19].dblVal), // double high
			   static_cast<double>(pdispparams->rgvarg[18].dblVal), // double low
			   static_cast<double>(pdispparams->rgvarg[17].dblVal), // double close
			   static_cast<double>(pdispparams->rgvarg[16].dblVal), // double last
			   static_cast<double>(pdispparams->rgvarg[15].dblVal), // double volume
			   static_cast<double>(pdispparams->rgvarg[14].dblVal), // double size
			   static_cast<double>(pdispparams->rgvarg[13].dblVal), // double bid
			   static_cast<double>(pdispparams->rgvarg[12].dblVal), // double ask
			   static_cast<double>(pdispparams->rgvarg[11].dblVal), // double bidsize
			   static_cast<double>(pdispparams->rgvarg[10].dblVal), // double asksize
			   static_cast<double>(pdispparams->rgvarg[9].dblVal), // double open_int
			   static_cast<double>(pdispparams->rgvarg[8].dblVal), // double go_buy
			   static_cast<double>(pdispparams->rgvarg[7].dblVal), // double go_sell
			   static_cast<double>(pdispparams->rgvarg[6].dblVal), // double go_base
			   static_cast<double>(pdispparams->rgvarg[5].dblVal), // double go_base_backed
			   static_cast<double>(pdispparams->rgvarg[4].dblVal), // double high_limit
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double low_limit
			   static_cast<long>(pdispparams->rgvarg[2].lVal), // long trading_status
			   static_cast<double>(pdispparams->rgvarg[1].dblVal), // double volat
			   static_cast<double>(pdispparams->rgvarg[0].dblVal) // double theor_price
		   );
		   break;
		}
		case 0x04: {
		   UpdateBidAsk(
			   ws2s(pdispparams->rgvarg[6].bstrVal).c_str(), // std::string symbol
			   static_cast<long>(pdispparams->rgvarg[5].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[4].lVal), // long nrows
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double bid
			   static_cast<double>(pdispparams->rgvarg[2].dblVal), // double bidsize
			   static_cast<double>(pdispparams->rgvarg[1].dblVal), // double ask
			   static_cast<double>(pdispparams->rgvarg[0].dblVal) // double asksize
		   );
		   break;
		}
		case 0x05: {
		   AddTick(
			   ws2s(pdispparams->rgvarg[5].bstrVal).c_str(), // std::string symbol
			   d2t(pdispparams->rgvarg[4].dblVal), // time_t datetime
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double price
			   static_cast<double>(pdispparams->rgvarg[2].dblVal), // double volume
			   ws2s(pdispparams->rgvarg[1].bstrVal).c_str(), // std::string tradeno
			   static_cast<OrderAction>(pdispparams->rgvarg[0].bVal) // OrderAction action
		   );
		   break;
		}
		case 0x06: {
		   AddBar(
			   static_cast<long>(pdispparams->rgvarg[10].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[9].lVal), // long nrows
			   ws2s(pdispparams->rgvarg[8].bstrVal).c_str(), // std::string symbol
			   static_cast<BarInterval>(pdispparams->rgvarg[7].bVal), // BarInterval interval
			   d2t(pdispparams->rgvarg[6].dblVal), // time_t datetime
			   static_cast<double>(pdispparams->rgvarg[5].dblVal), // double open
			   static_cast<double>(pdispparams->rgvarg[4].dblVal), // double high
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double low
			   static_cast<double>(pdispparams->rgvarg[2].dblVal), // double close
			   static_cast<double>(pdispparams->rgvarg[1].dblVal), // double volume
			   static_cast<double>(pdispparams->rgvarg[0].dblVal) // double open_int
		   );
		   break;
		}
		case 0x07: {
		   SetPortfolio(
			   ws2s(pdispparams->rgvarg[4].bstrVal).c_str(), // std::string portfolio
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double cash
			   static_cast<double>(pdispparams->rgvarg[2].dblVal), // double leverage
			   static_cast<double>(pdispparams->rgvarg[1].dblVal), // double comission
			   static_cast<double>(pdispparams->rgvarg[0].dblVal) // double saldo
		   );
		   break;
		}
		case 0x08: {
		   AddTrade(
			   ws2s(pdispparams->rgvarg[6].bstrVal).c_str(), // std::string portfolio
			   ws2s(pdispparams->rgvarg[5].bstrVal).c_str(), // std::string symbol
			   ws2s(pdispparams->rgvarg[4].bstrVal).c_str(), // std::string orderid
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double price
			   static_cast<double>(pdispparams->rgvarg[2].dblVal), // double amount
			   d2t(pdispparams->rgvarg[1].dblVal), // time_t datetime
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string tradeno
		   );
		   break;
		}
		case 0x09: {
		   UpdateOrder(
			   ws2s(pdispparams->rgvarg[14].bstrVal).c_str(), // std::string portfolio
			   ws2s(pdispparams->rgvarg[13].bstrVal).c_str(), // std::string symbol
			   static_cast<OrderState>(pdispparams->rgvarg[12].bVal), // OrderState state
			   static_cast<OrderAction>(pdispparams->rgvarg[11].bVal), // OrderAction action
			   static_cast<OrderType>(pdispparams->rgvarg[10].bVal), // OrderType type
			   static_cast<OrderValidity>(pdispparams->rgvarg[9].bVal), // OrderValidity validity
			   static_cast<double>(pdispparams->rgvarg[8].dblVal), // double price
			   static_cast<double>(pdispparams->rgvarg[7].dblVal), // double amount
			   static_cast<double>(pdispparams->rgvarg[6].dblVal), // double stop
			   static_cast<double>(pdispparams->rgvarg[5].dblVal), // double filled
			   d2t(pdispparams->rgvarg[4].dblVal), // time_t datetime
			   ws2s(pdispparams->rgvarg[3].bstrVal).c_str(), // std::string orderid
			   ws2s(pdispparams->rgvarg[2].bstrVal).c_str(), // std::string orderno
			   static_cast<long>(pdispparams->rgvarg[1].lVal), // long status_mask
			   static_cast<long>(pdispparams->rgvarg[0].lVal) // long cookie
		   );
		   break;
		}
		case 0x0a: {
		   UpdatePosition(
			   ws2s(pdispparams->rgvarg[4].bstrVal).c_str(), // std::string portfolio
			   ws2s(pdispparams->rgvarg[3].bstrVal).c_str(), // std::string symbol
			   static_cast<double>(pdispparams->rgvarg[2].dblVal), // double avprice
			   static_cast<double>(pdispparams->rgvarg[1].dblVal), // double amount
			   static_cast<double>(pdispparams->rgvarg[0].dblVal) // double planned
		   );
		   break;
		}
		case 0x0f: {
		   AddTickHistory(
			   static_cast<long>(pdispparams->rgvarg[7].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[6].lVal), // long nrows
			   ws2s(pdispparams->rgvarg[5].bstrVal).c_str(), // std::string symbol
			   d2t(pdispparams->rgvarg[4].dblVal), // time_t datetime
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double price
			   static_cast<double>(pdispparams->rgvarg[2].dblVal), // double volume
			   ws2s(pdispparams->rgvarg[1].bstrVal).c_str(), // std::string tradeno
			   static_cast<OrderAction>(pdispparams->rgvarg[0].bVal) // OrderAction action
		   );
		   break;
		}
		case 0x14: {
		   AddSymbol(
			   static_cast<long>(pdispparams->rgvarg[14].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[13].lVal), // long nrows
			   ws2s(pdispparams->rgvarg[12].bstrVal).c_str(), // std::string symbol
			   ws2s(pdispparams->rgvarg[11].bstrVal).c_str(), // std::string short_name
			   ws2s(pdispparams->rgvarg[10].bstrVal).c_str(), // std::string int_name
			   ws2s(pdispparams->rgvarg[9].bstrVal).c_str(), // std::string type
			   static_cast<long>(pdispparams->rgvarg[8].lVal), // long decimals
			   static_cast<long>(pdispparams->rgvarg[7].lVal), // long lot_size
			   static_cast<double>(pdispparams->rgvarg[6].dblVal), // double punkt
			   static_cast<double>(pdispparams->rgvarg[5].dblVal), // double step
			   ws2s(pdispparams->rgvarg[4].bstrVal).c_str(), // std::string sec_ext_id
			   ws2s(pdispparams->rgvarg[3].bstrVal).c_str(), // std::string sec_exch_name
			   d2t(pdispparams->rgvarg[2].dblVal), // time_t expiry_date
			   static_cast<double>(pdispparams->rgvarg[1].dblVal), // double days_before_expiry
			   static_cast<double>(pdispparams->rgvarg[0].dblVal) // double strike
		   );
		   break;
		}
		case 0x1e: {
		   OrderSucceeded(
			   static_cast<long>(pdispparams->rgvarg[1].lVal), // long cookie
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string orderid
		   );
		   break;
		}
		case 0x1f: {
		   OrderFailed(
			   static_cast<long>(pdispparams->rgvarg[2].lVal), // long cookie
			   ws2s(pdispparams->rgvarg[1].bstrVal).c_str(), // std::string orderid
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string reason
		   );
		   break;
		}
		case 0x20: {
		   AddPortfolio(
			   static_cast<long>(pdispparams->rgvarg[4].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[3].lVal), // long nrows
			   ws2s(pdispparams->rgvarg[2].bstrVal).c_str(), // std::string portfolioName
			   ws2s(pdispparams->rgvarg[1].bstrVal).c_str(), // std::string portfolioExch
			   static_cast<PortfolioStatus>(pdispparams->rgvarg[0].bVal) // PortfolioStatus portfolioStatus
		   );
		   break;
		}
		case 0x21: {
		   SetSubscribtionCheckReult(
			   static_cast<long>(pdispparams->rgvarg[0].lVal) // long result
		   );
		   break;
		}
		case 0x22: {
		   SetMyTrade(
			   static_cast<long>(pdispparams->rgvarg[9].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[8].lVal), // long nrows
			   ws2s(pdispparams->rgvarg[7].bstrVal).c_str(), // std::string portfolio
			   ws2s(pdispparams->rgvarg[6].bstrVal).c_str(), // std::string symbol
			   d2t(pdispparams->rgvarg[5].dblVal), // time_t datetime
			   static_cast<double>(pdispparams->rgvarg[4].dblVal), // double price
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double volume
			   ws2s(pdispparams->rgvarg[2].bstrVal).c_str(), // std::string tradeno
			   static_cast<OrderAction>(pdispparams->rgvarg[1].bVal), // OrderAction buysell
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string orderno
		   );
		   break;
		}
		case 0x23: {
		   SetMyOrder(
			   static_cast<long>(pdispparams->rgvarg[15].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[14].lVal), // long nrows
			   ws2s(pdispparams->rgvarg[13].bstrVal).c_str(), // std::string portfolio
			   ws2s(pdispparams->rgvarg[12].bstrVal).c_str(), // std::string symbol
			   static_cast<OrderState>(pdispparams->rgvarg[11].bVal), // OrderState state
			   static_cast<OrderAction>(pdispparams->rgvarg[10].bVal), // OrderAction action
			   static_cast<OrderType>(pdispparams->rgvarg[9].bVal), // OrderType type
			   static_cast<OrderValidity>(pdispparams->rgvarg[8].bVal), // OrderValidity validity
			   static_cast<double>(pdispparams->rgvarg[7].dblVal), // double price
			   static_cast<double>(pdispparams->rgvarg[6].dblVal), // double amount
			   static_cast<double>(pdispparams->rgvarg[5].dblVal), // double stop
			   static_cast<double>(pdispparams->rgvarg[4].dblVal), // double filled
			   d2t(pdispparams->rgvarg[3].dblVal), // time_t datetime
			   ws2s(pdispparams->rgvarg[2].bstrVal).c_str(), // std::string id
			   ws2s(pdispparams->rgvarg[1].bstrVal).c_str(), // std::string no
			   static_cast<long>(pdispparams->rgvarg[0].lVal) // long cookie
		   );
		   break;
		}
		case 0x25: {
		   SetMyClosePos(
			   static_cast<long>(pdispparams->rgvarg[9].lVal), // long row
			   static_cast<long>(pdispparams->rgvarg[8].lVal), // long nrows
			   ws2s(pdispparams->rgvarg[7].bstrVal).c_str(), // std::string portfolio
			   ws2s(pdispparams->rgvarg[6].bstrVal).c_str(), // std::string symbol
			   static_cast<double>(pdispparams->rgvarg[5].dblVal), // double amount
			   static_cast<double>(pdispparams->rgvarg[4].dblVal), // double price_buy
			   static_cast<double>(pdispparams->rgvarg[3].dblVal), // double price_sell
			   d2t(pdispparams->rgvarg[2].dblVal), // time_t postime
			   ws2s(pdispparams->rgvarg[1].bstrVal).c_str(), // std::string order_open
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string order_close
		   );
		   break;
		}
		case 0x26: {
		   OrderCancelSucceeded(
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string orderid
		   );
		   break;
		}
		case 0x27: {
		   OrderCancelFailed(
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string orderid
		   );
		   break;
		}
		case 0x28: {
		   OrderMoveSucceeded(
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string orderid
		   );
		   break;
		}
		case 0x29: {
		   OrderMoveFailed(
			   ws2s(pdispparams->rgvarg[0].bstrVal).c_str() // std::string orderid
		   );
		   break;
		}
		default: {
			 printf("IStClient::InvokeDispatch(%d) unknown code\n", dispidMember);
			 break;
		}
		}
	}

	/* COM HELPERS */

	std::mutex IStClient::s_ApartmentsInitializationMutex;
	bool IStClient::s_ApartmentsInitializationFlag = false;
	void IStClient::InitializeApartments()
	{
		//printf("IStClient::InitializeApartments()\n");
		s_ApartmentsInitializationMutex.lock();
		if (s_ApartmentsInitializationFlag == false) {
			if (S_OK != CoInitializeEx(NULL, COINIT_MULTITHREADED)) {
				printf("IStClient::InitializeApartments() "
						"CoInitializeEx(NULL, COINIT_MULTITHREADED) FAIL\n");
				exit(1);
			}
			s_ApartmentsInitializationFlag = true;
			//printf("IStClient::InitializeApartments() OK\n");
		} else {
			//printf("IStClient::InitializeApartments() already initialized\n");
		}
		s_ApartmentsInitializationMutex.unlock();
	}
	void IStClient::UninitializeApartments()
	{
		//printf("IStClient::UninitializeApartments()\n");
		s_ApartmentsInitializationMutex.lock();
		if (s_ApartmentsInitializationFlag == true) {
			CoUninitialize();
			s_ApartmentsInitializationFlag = false;
			//printf("IStClient::UninitializeApartments() OK\n");
		} else {
			//printf("IStClient::UninitializeApartments() not initialized\n");
		}
		s_ApartmentsInitializationMutex.unlock();
	}
	HRESULT IStClient::Advise()
	{
		HRESULT hr = S_FALSE;
		IConnectionPointContainer* cpc;
		if (S_OK == m_IStServer->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc))
		{
			if (S_OK == cpc->FindConnectionPoint(DIID_IStClient, &m_pIConnectionPoint))
			{
				hr = m_pIConnectionPoint->Advise(this, &m_dwCookie);
			}
			cpc->Release();
		}
		return hr;
	}
	void IStClient::UnAdvise()
	{
		if (m_dwCookie) {
			m_pIConnectionPoint->Unadvise(m_dwCookie);
			m_pIConnectionPoint->Release();
			m_dwCookie = 0;
		}
	}
	
	IStClient::IStClient(bool autoInitializeApartments)
		: m_cRef(1), m_dwCookie(0)
	{
		//printf("IStClient::IStClient()\n");

		if (autoInitializeApartments) InitializeApartments();

		LPTYPELIB pTypeLib;
		if (S_OK != LoadRegTypeLib(LIBID_SmartCOM3Lib, 1, 0, LOCALE_SYSTEM_DEFAULT, &pTypeLib)) {
			printf("IStClient::IStClient() LoadRegTypeLib FAIL\n");
			exit(1);
		}
		if (S_OK != pTypeLib->GetTypeInfoOfGuid(DIID_IStClient, &m_ptinfo)) {
			printf("IStClient::IStClient() GetTypeInfoOfGuid FAIL\n");
			exit(1);
		}
		pTypeLib->Release();

		if (S_OK != CoCreateInstance(CLSID_StServer, NULL, CLSCTX_INPROC_SERVER, IID_IStServer,
				reinterpret_cast<void**>(&m_IStServer)))
		{
			printf("IStClient::IStClient() CoCreateInstance(Server) FAIL\n");
			exit(1);
		}
		if (S_OK != Advise()) {
			printf("IStClient::IStClient() COM Advise FAIL\n");
			exit(1);
		}
		if (S_OK != CoCreateInstance(CLSID_StServer, NULL, CLSCTX_INPROC_SERVER,
				IID_ISmartComVersion, reinterpret_cast<void**>(&m_ISmartComVersion)))
		{
			printf("IStClient::IStClient() CoCreateInstance(Version) FAIL\n");
			exit(1);
		}
		
		//printf("IStClient::IStClient() OK\n");
	}
	IStClient::~IStClient()
	{
		//printf("IStClient::~IStClient()\n");

		UnAdvise();
		m_IStServer->Release();
		m_ISmartComVersion->Release();
		m_ptinfo->Release();

		//printf("IStClient::~IStClient() OK\n");
	}

	/* SmartCOM3 METHODS */

	void IStClient::ListenQuotes(const char *symbol) {
		if (m_IStServer->raw_ListenQuotes(s2ws(symbol).c_str()))
			throw std::runtime_error("ListenQuotes(): SmartCOM3 API error!");
	}
	void IStClient::CancelQuotes(const char *symbol) {
		if (m_IStServer->raw_CancelQuotes(s2ws(symbol).c_str()))
			throw std::runtime_error("CancelQuotes(): SmartCOM3 API error!");
	}
	void IStClient::ListenBidAsks(const char *symbol) {
		if (m_IStServer->raw_ListenBidAsks(s2ws(symbol).c_str()))
			throw std::runtime_error("ListenBidAsks(): SmartCOM3 API error!");
	}
	void IStClient::CancelBidAsks(const char *symbol) {
		if (m_IStServer->raw_CancelBidAsks(s2ws(symbol).c_str()))
			throw std::runtime_error("CancelBidAsks(): SmartCOM3 API error!");
	}
	void IStClient::ListenTicks(const char *symbol) {
		if (m_IStServer->raw_ListenTicks(s2ws(symbol).c_str()))
			throw std::runtime_error("ListenTicks(): SmartCOM3 API error!");
	}
	void IStClient::CancelTicks(const char *symbol) {
		if (m_IStServer->raw_CancelTicks(s2ws(symbol).c_str()))
			throw std::runtime_error("CancelTicks(): SmartCOM3 API error!");
	}
	void IStClient::GetBars(const char *symbol, BarInterval interval, time_t since, long count) {
		if (m_IStServer->raw_GetBars(s2ws(symbol).c_str(), interval, t2d(since), count))
			throw std::runtime_error("GetBars(): SmartCOM3 API error!");
	}
	void IStClient::ListenPortfolio(const char *portfolio) {
		if (m_IStServer->raw_ListenPortfolio(s2ws(portfolio).c_str()))
			throw std::runtime_error("ListenPortfolio(): SmartCOM3 API error!");
	}
	void IStClient::CancelPortfolio(const char *portfolio) {
		if (m_IStServer->raw_CancelPortfolio(s2ws(portfolio).c_str()))
			throw std::runtime_error("CancelPortfolio(): SmartCOM3 API error!");
	}
	void IStClient::PlaceOrder(const char *portfolio, const char *symbol,
		OrderAction action, OrderType type, OrderValidity validity,
		double price, double amount, double stop, long cookie)
	{
		if (m_IStServer->raw_PlaceOrder(s2ws(portfolio).c_str(),
			s2ws(symbol).c_str(), action, type, validity, price, amount, stop, cookie))
			throw std::runtime_error("PlaceOrder(): SmartCOM3 API error!");
	}
	void IStClient::CancelOrder(const char *portfolio, const char *symbol, const char *orderid) {
		if (m_IStServer->raw_CancelOrder(s2ws(portfolio).c_str(),
			s2ws(symbol).c_str(), s2ws(orderid).c_str()))
			throw std::runtime_error("CancelOrder(): SmartCOM3 API error!");
	}
	void IStClient::MoveOrder(const char *portfolio, const char *orderid, double targetPrice) {
		if (m_IStServer->raw_MoveOrder(s2ws(portfolio).c_str(),
			s2ws(orderid).c_str(), targetPrice))
			throw std::runtime_error("MoveOrder(): SmartCOM3 API error!");
	}
	void IStClient::GetSymbols() {
		if (m_IStServer->raw_GetSymbols())
			throw std::runtime_error("GetSymbols(): SmartCOM3 API error!");
	}
	bool IStClient::IsConnected() {
		VARIANT_BOOL result = 0;
		if (m_IStServer->raw_IsConnected(&result))
			throw std::runtime_error("IsConnected(): SmartCOM3 API error!");
		if (result == 0) return false; // return bool(result) - slow performance warning
		else return true;
	}
	void IStClient::Connect(const char *ip, unsigned short port,
		const char *login, const char *password)
	{
		if (m_IStServer->raw_connect(s2ws(ip).c_str(), port,
			s2ws(login).c_str(), s2ws(password).c_str()))
			throw std::runtime_error("Connect(): SmartCOM3 API error!");
	}
	void IStClient::Disconnect() {
		if (m_IStServer->raw_disconnect())
			throw std::runtime_error("Disconnect(): SmartCOM3 API error!");
	}
	void IStClient::GetPortfolioList() {
		if (m_IStServer->raw_GetPortfolioList())
			throw std::runtime_error("GetPortfolioList(): SmartCOM3 API error!");
	}
	void IStClient::GetMyOrders(long onlyActive, const char *portfolio) {
		if (m_IStServer->raw_GetMyOrders(onlyActive, s2ws(portfolio).c_str()))
			throw std::runtime_error("GetMyOrders(): SmartCOM3 API error!");
	}
	void IStClient::GetMyTrades(const char *portfolio) {
		if (m_IStServer->raw_GetMyTrades(s2ws(portfolio).c_str()))
			throw std::runtime_error("GetMyTrades(): SmartCOM3 API error!");
	}
	void IStClient::GetMyClosePos(const char *portfolio) {
		if (m_IStServer->raw_GetMyClosePos(s2ws(portfolio).c_str()))
			throw std::runtime_error("GetMyClosePos(): SmartCOM3 API error!");
	}
	void IStClient::GetTrades(const char *symbol, time_t from, long count) {
		if (m_IStServer->raw_GetTrades(s2ws(symbol).c_str(), t2d(from), count))
			throw std::runtime_error("GetTrades(): SmartCOM3 API error!");
	}
	void IStClient::ConfigureClient(const char *paramsSet) {
		if (m_IStServer->raw_ConfigureClient(s2ws(paramsSet).c_str()))
			throw std::runtime_error("ConfigureClient(): SmartCOM3 API error!");
	}
	void IStClient::ConfigureServer(const char *paramsSet) {
		if (m_IStServer->raw_ConfigureServer(s2ws(paramsSet).c_str()))
			throw std::runtime_error("ConfigureServer(): SmartCOM3 API error!");
	}
	std::string IStClient::GetMoneyAccount(const char *portfolioID) {
		const wchar_t *result = 0;
		if (m_IStServer->raw_GetMoneyAccount(s2ws(portfolioID).c_str(), &result))
			throw std::runtime_error("GetMoneyAccount(): SmartCOM3 API error!");
		return ws2s(result);
	}

	/* SmartCOM3 VERSION METHODS */

	std::string IStClient::GetServerVersionString() {
		BSTR result = 0;
		if (m_ISmartComVersion->raw_GetServerVersionString(&result))
			throw std::runtime_error("GetServerVersionString(): SmartCOM3 API error!");
		return ws2s(result);
	}
	void IStClient::GetServerVersion(long *major, long *minor, long *build, long *rev) {
		if (m_ISmartComVersion->raw_GetServerVersion(major, minor, build, rev))
			throw std::runtime_error("GetServerVersion(): SmartCOM3 API error!");
	}
	std::string IStClient::GetClientVersionString() {
		BSTR result = 0;
		if (m_ISmartComVersion->raw_GetStClientVersionString(&result))
			throw std::runtime_error("GetClientVersionString(): SmartCOM3 API error!");
		return ws2s(result);
	}
	void IStClient::GetClientVersion(long *major, long *minor, long *build, long *rev) {
		if (m_ISmartComVersion->raw_GetStClientVersion(major, minor, build, rev))
			throw std::runtime_error("GetClientVersion(): SmartCOM3 API error!");
	}
}

