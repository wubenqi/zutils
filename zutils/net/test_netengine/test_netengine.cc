#include "base/at_exit.h"
#include "net/base/winsock_init.h"
#include "net/engine/net_engine_manager.h"
#include "net/engine/tcp_acceptor.h"
#include "net/engine/tcp_connector.h"

#include <iostream>
using namespace net;

/*
class IOCallBack :
	public net::IOHandler::IOHandlerDelegate {
public:
	virtual int OnNewConnection(net::IOHandler *ih) {
		// ih->SendInternal("", 4096);
		printf("OnNewConnection, ih ==> %d\n", ih);
		return 0;
	}

	virtual int OnDataReceived(net::IOHandler *ih, const char* data, int len) {
		//printf("OnDataReceived, ih ==> %d, len = %d\n", ih, len);
		ih->SendInternal(data, len);
		//ih->SendInternal("\n", 1);
		return -1;
	}

	virtual int OnConnectionClosed(net::IOHandler *ih) {
		printf("OnConnectionClosed, ih ==> %d\n", ih);
		return 0;
	}
};

class IOCallBack2 :
	public net::IOHandler::IOHandlerDelegate {
public:
	virtual int OnNewConnection(net::IOHandler *ih) {
		char t_data[]= "GET / HTTP/1.0\n\n";

		ih->SendInternal(t_data, sizeof(t_data));
		printf("OnNewConnection, ih2 ==> %d\n", ih);
		return 0;
	}

	virtual int OnDataReceived(net::IOHandler *ih, const char* data, int len) {
		//printf("OnDataReceived, ih2 ==> %d, len = %d\n", ih, len);
		//printf()

		std::string data2(data, len);
		std::cout << data2;
		// ih->SendInternal(data, len);
		//ih->SendInternal("\n", 1);
		return -1;
	}

	virtual int OnConnectionClosed(net::IOHandler *ih) {
		printf("OnConnectionClosed, ih ==> %d\n", ih);
		return 0;
	}
};

class TCPConnectorErrorCallBack :
	public net::TCPConnector::TCPConnectorDelegate {
public:
	virtual void OnConnectedError(net::TCPConnector *conn, int err_code) {
		printf("OnConnectedError, error ==> %d\n", err_code);
	}
};
*/


/*
#include <map>
#include <string>
#include "base/threading/simple_thread.h"
#include "base/threading/platform_thread.h"
#include "base/stringprintf.h"

#include "base2/scoped_lock.h"
#include "base2/shared_ptr.h"
#include <boost/shared_ptr.hpp>

#include <iostream>

struct ItemData {
	ItemData(const std::string& _name, int _color)
		: name(_name)
		, color(_color) {

	}

	~ItemData() {
		std::cout << "name ==> " << name << " Destroy" << std::endl;
	}
	void Print() {
		std::cout << "name ==> " << name << ", color ==> " << color << std::endl;
	}

	std::string name;
	int color;
};

base::Lock static_item_data_lock;
std::map<int, base::SharedPtr<ItemData> > static_item_datas;
typedef base::SharedPtr<ItemData> ItemDataPtr;

//typedef boost::shared_ptr<ItemData> ItemDataPtr;
//std::map<int, ItemDataPtr> static_item_datas;

class ResourceThread :
	public base::SimpleThread {
public:
	explicit ResourceThread(const std::string& name_prefix) 
		: base::SimpleThread(name_prefix)
		, running_(false) {
	}

	// Subclasses should override the Run method.
	virtual void Run() {
		running_ = true;
		int i=0;
		while(running_) {
			base::ScopedLock scoped_lock(static_item_data_lock);
			int j=0;
			for (std::map<int, ItemDataPtr>::iterator it=static_item_datas.begin(); it!=static_item_datas.end(); ++it) {
				ItemDataPtr item_data(new ItemData(base::StringPrintf("%d_%d", i, j++), i));
				it->second.swap(item_data);
				base::PlatformThread::Sleep(1);
			}
			++i;
		}
		
	}
private:
	bool running_;

};
*/

//#include "net/http/http_connector.h"
#if 0
int main(int argc, char* argv[]) {
	base::AtExitManager exit;

#if defined(OS_WIN)
	net::EnsureWinsockInit();
#endif  // defined(OS_WIN)

	//net::NetEngineManager::GetInstance()->Run();
	// Do work here.
	MessageLoop loop;

	printf("[Main]: Current thread id[%d]\n", base::PlatformThread::CurrentId());

	// HttpServer server2("0.0.0.0", 80);   // TODO(mbelshe): make port configurable
	IOCallBack ih;
	IOCallBack2 ih2;
	TCPConnectorErrorCallBack conn_err_callback;
	
	net::NetEngineManager net_engine_manager(8);
	net_engine_manager.Run();

#if 0
	scoped_refptr<net::TCPAcceptor> acc(new net::TCPAcceptor(&net_engine_manager, &ih)); 
	acc->Create("127.0.0.1", "80", false);

	//Sleep(5000);
#else

	std::vector<scoped_refptr<net::TCPConnector> > conn_pool(1);
	for(size_t i=0; i<conn_pool.size(); ++i) {
		conn_pool[i] = new net::TCPConnector(&net_engine_manager, &conn_err_callback, &ih2); 
		// conn->Connect("61.135.169.125", "80");
		conn_pool[i]->Connect("192.168.6.104", "22345", true);
	}

	//net::HTTPConnector conn(&net_engine_manager);
	//conn.Connect("61.135.169.125", "80");

#endif

	MessageLoop::current()->Run();

	net_engine_manager.Shutdown();

/*
	ResourceThread resource_thread("item_data_resource_thread");
	resource_thread.Start();

	for (int i=0; i<10; ++i) {
		ItemData* item_data = new ItemData(base::StringPrintf("-1_%d", i), -1);
		static_item_datas.insert(std::make_pair(i, item_data));
	}
	
	int j = 0;
	while(1) {
		//std::cout << j++ << "-----------------------------------------------------------" << std::endl;
		//for (std::map<int, ItemDataPtr>::iterator it=static_item_datas.begin(); it!=static_item_datas.end(); ++it) {
		//	std::cout << it->first << ": ";
		//	ItemDataPtr item_data = it->second; //->Print();
		//	item_data->Print();
		//	base::PlatformThread::Sleep(1);
		//}
		base::PlatformThread::Sleep(2);

		std::cout << std::endl;
	}
*/
	return 0;
}
#endif

#include <map>
#include "base2/shared_ptr.h"

class SharedPtrTest {
public:
	SharedPtrTest(const std::string& str_i) {
		i_ = str_i;
	}

	~SharedPtrTest() {
		std::cout << i_ << " ==> ~SharedPtrTest()" << std::endl;
	}

	void Print() {
		std::cout << i_ << " ==> Print()" << std::endl;
	}

	std::string i_;
};

class SharedPtrTestPool {
public:
	SharedPtrTestPool() {
		sharedptr_pool_["1"] = base::SharedPtr<SharedPtrTest>(new SharedPtrTest("1"));
		sharedptr_pool_["2"] = base::SharedPtr<SharedPtrTest>(new SharedPtrTest("2"));
		sharedptr_pool_["3"] = base::SharedPtr<SharedPtrTest>(new SharedPtrTest("3"));
		sharedptr_pool_["4"] = base::SharedPtr<SharedPtrTest>(new SharedPtrTest("4"));
		sharedptr_pool_["5"] = base::SharedPtr<SharedPtrTest>(new SharedPtrTest("5"));
		sharedptr_pool_["6"] = base::SharedPtr<SharedPtrTest>(new SharedPtrTest("6"));
	}

	~SharedPtrTestPool() {

	}

	base::SharedPtr<SharedPtrTest> Find(const std::string& str_i);
	base::SharedPtr<SharedPtrTest> Add(const std::string& str_i);
	bool Remove(const std::string& str_i);
	void RemoveAll();

private:
	std::map<std::string, base::SharedPtr<SharedPtrTest> > sharedptr_pool_;
};

base::SharedPtr<SharedPtrTest> SharedPtrTestPool::Find(const std::string& str_i) {
	base::SharedPtr<SharedPtrTest> shared_ptr_test;
	if (!sharedptr_pool_.empty()) {
		std::map<std::string, base::SharedPtr<SharedPtrTest> >::iterator it = sharedptr_pool_.find(str_i);
		if (it!=sharedptr_pool_.end()) {
			shared_ptr_test = it->second.get();
		} 
	}
	return shared_ptr_test;
}

base::SharedPtr<SharedPtrTest> SharedPtrTestPool::Add(const std::string& str_i) {
	base::SharedPtr<SharedPtrTest> shared_ptr_test = Find(str_i);
	if (shared_ptr_test.get()==NULL) {
		shared_ptr_test = new SharedPtrTest(str_i);
		sharedptr_pool_.insert(std::make_pair(str_i, shared_ptr_test));
	}

	return shared_ptr_test;
}

bool SharedPtrTestPool::Remove(const std::string& str_i) {
	bool ret = false;
	std::map<std::string, base::SharedPtr<SharedPtrTest> >::iterator it = sharedptr_pool_.find(str_i);
	if (it!=sharedptr_pool_.end()) {
		sharedptr_pool_.erase(it);
		ret = true;
	} 
	return ret;
}

void SharedPtrTestPool::RemoveAll() {
	sharedptr_pool_.clear();
}

class TestServer :
	public WebdogsIOHandler::WebdogsIOHandlerDelegate {
public:
	TestServer(NetEngineManager* engine_manager, IOHandlerFactory* ih_factory)
		: ALLOW_THIS_IN_INITIALIZER_LIST(acc_(new net::TCPAcceptor(engine_manager, ih_factory, this))) {
		acc_->Create("127.0.0.1", "12345", false);
	}

	virtual int OnNewConnection(IOHandler *ih) {
		return 0;
	}

	virtual int OnDataReceived(WebdogsIOHandler *ih, uint16 message_type, const char* data, int len) { 
		//char data[8];
		//WebdogsPacketHeader::GeneratePacketHeader(data, 123, 11);
		//ih->Send()
		std::cout << "TestServer::OnDataReceived ==> " << data << std::endl;

		char data2[19];
		WebdogsPacketHeader::GeneratePacketHeader((uint8*)data2, 123, 11);
		memcpy(data2+8, data, len);
		ih->SendInternal(data2, 19);
		
		return 0;
	}

	virtual int OnConnectionClosed(IOHandler *ih) { 
		return 0;
	}


private:
	scoped_refptr<net::TCPAcceptor> acc_; //(new net::TCPAcceptor(&net_engine_manager, &ih)); 
	// acc->Create("127.0.0.1", "80", false);

};


class TestConn :
	public WebdogsIOHandler::WebdogsIOHandlerDelegate,
	public net::TCPConnector::TCPConnectorDelegate {
public:
	TestConn(NetEngineManager* engine_manager, IOHandlerFactory* ih_factory)
		: ALLOW_THIS_IN_INITIALIZER_LIST(conn_(new net::TCPConnector(engine_manager, ih_factory, this, this))) {
			conn_->Connect("127.0.0.1", "12345", true);
	}

	virtual int OnNewConnection(IOHandler *ih) {
		char data[19];
		WebdogsPacketHeader::GeneratePacketHeader((uint8*)data, 123, 11);
		strcpy(data+8, "0123456789");
		ih->SendInternal(data, 19);
		return 0;
	}

	virtual int OnDataReceived(WebdogsIOHandler *ih, uint16 message_type, const char* data, int len) { 
		//char data[8];
		//WebdogsPacketHeader::GeneratePacketHeader(data, 123, 11);
		//ih->Send()

		std::cout << "TestConn::OnDataReceived ==> " << data << std::endl;

		char data2[19];
		WebdogsPacketHeader::GeneratePacketHeader((uint8*)data2, 123, 11);
		memcpy(data2+8, data, len);
		ih->SendInternal(data2, 19);

		return 0;
	}

	virtual int OnConnectionClosed(IOHandler *ih) { 
		return 0;
	}

	virtual void OnConnectedError(net::TCPConnector *conn, int err_code) {
		printf("OnConnectedError, error ==> %d\n", err_code);
	}

private:
	scoped_refptr<net::TCPConnector> conn_; //(new net::TCPAcceptor(&net_engine_manager, &ih)); 
};

int main(int argc, char* argv[]) {
	base::AtExitManager exit;

	#if defined(OS_WIN)
	net::EnsureWinsockInit();
	#endif  // defined(OS_WIN)

	//net::NetEngineManager::GetInstance()->Run();
	// Do work here.
	MessageLoop loop;

	net::NetEngineManager net_engine_manager(8);
	net_engine_manager.Run();

	// IOHandlerFactory* ih_factory = new WebdogsIOHandlerFactory();

	TestServer server(&net_engine_manager, WebdogsIOHandlerFactory::GetInstance());

	Sleep(10);

	TestConn conn(&net_engine_manager, WebdogsIOHandlerFactory::GetInstance());

/*
	std::vector<scoped_refptr<net::TCPConnector> > conn_pool(1);
	for(size_t i=0; i<conn_pool.size(); ++i) {
		conn_pool[i] = new net::TCPConnector(&net_engine_manager, ih_factory, &conn_err_callback, &ih2); 
		// conn->Connect("61.135.169.125", "80");
		conn_pool[i]->Connect("192.168.6.104", "22345", true);
	}
*/

	//net::HTTPConnector conn(&net_engine_manager);
	//conn.Connect("61.135.169.125", "80");

/*
	{
		SharedPtrTestPool test_pool;
		std::string str_i("10");
		{
			base::SharedPtr<SharedPtrTest> test = test_pool.Find(str_i);
			if (test.get()!=NULL) {
			} else {
				test = test_pool.Add(str_i);
				if (test.get()!=NULL) {
					test->Print();
				} else {
					// 超过工作池的最大个数了
					// 存起来
				}
			}

		}

	}
*/

	MessageLoop::current()->Run();
}
