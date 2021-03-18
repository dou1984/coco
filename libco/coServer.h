#ifndef CO_SERVER_H
#define CO_SERVER_H
#include "evcpp.h"
#include "coClient.h"
#include "coTask.h"
#include "coFunc.h"

namespace ashan
{
	class coServer : public evServer, public VClose
	{
		struct awtAccept;
		std::unordered_map<int, std::shared_ptr<coClient>> m_clients;
		coList m_list;
		//coFunc co_process;
	public:
		//int set(const coFunc&);
		//coTask co_echo(const char* ip, int port);
		awtAccept co_accept();
		int on_accept(int fd);		
		int on_close(int fd);	
		~coServer();
		//int try_process(std::shared_ptr<coClient> c);
	private:
		struct awtAccept : coList
		{
			coServer* _this = nullptr;			
			std::shared_ptr<coClient> _client;
			std::coroutine_handle<> m_awaiting = nullptr;			
			awtAccept(coServer*);
			awtAccept(awtAccept&&) = delete;
			~awtAccept();
			bool await_ready();
			void await_suspend(std::coroutine_handle<> awaiting);
			std::shared_ptr<coClient> await_resume();
			void resume_ex();
		};
	};
}

#endif