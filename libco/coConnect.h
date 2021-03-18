#ifndef CO_CONNECT_H
#define CO_CONNECT_H
#include "evcpp.h"
#include "coClient.h"
#include "coTask.h"
#include "coFunc.h"

namespace ashan
{
	class coConnect final :public evConnect, public VClose
	{
		struct awtConnect;
		coList m_list;		
		std::shared_ptr<coClient> m_client;
		//coFunc co_process;
	public:		
		//int set(const coFunc&);		
		awtConnect co_connect(const char* ip, int port);	
		int on_connect(int fd, const char* ip, int port);
		int err_connect(int fd, const char* _ip, int _port);
		int on_close(int fd);
		std::shared_ptr<coClient>& get();
		~coConnect();
	private:
		void resume(int fd);
		struct awtConnect : coList
		{
			coConnect * _this = nullptr;			
			std::coroutine_handle<> m_awaiting = nullptr;
			int _fd = INVALID;
			awtConnect(coConnect* o, int fd);
			awtConnect(awtConnect&&) = delete;			
			~awtConnect();
			bool await_ready();
			void await_suspend(std::coroutine_handle<> awaiting);
			std::shared_ptr<coClient> await_resume();
			void resume_ex();
		};
	};
}

#endif