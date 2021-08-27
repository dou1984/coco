#ifndef CO_CLIENT_H
#define CO_CLIENT_H
#include "coList.h"
#include "coMsg.h"
#include "awaiter.h"
#include "evClient.h"
#include "coTask.h"
#include "coFunc.h"

namespace ashan
{
#define DEFINE_WAITTIME (15 * 1000)

	class coClient final : public evClient, public evTimer, public std::enable_shared_from_this<coClient>
	{	
	public:
		enum e_config
		{
			e_proto_comsg,	//使用解析coMsg
			e_write_ack,	//send message成功后会收到ack消息 
			e_config_end,
		};		
		struct awtRead : coList
		{
			coClient* _this = nullptr;
			uint32_t _idx = RPC_INVALID;
			uint32_t _value = 0;
			std::coroutine_handle<> m_awaiting = nullptr;
			std::chrono::milliseconds _time;
			awtRead(coClient* _, uint32_t index, size_t _wait);
			~awtRead();
			uint32_t await_resume();		
			bool await_ready();			
			void await_suspend(std::coroutine_handle<> awaiting);
			void set_return(uint32_t _val);			
			void resume_ex();
		};		
		struct awtWrite : coList
		{
			coClient* _this = nullptr;		
			uint32_t _value = 0;
			std::coroutine_handle<> m_awaiting = nullptr;
			std::chrono::milliseconds _time;
			awtWrite(coClient* _, uint32_t val, size_t _wait);
			~awtWrite();
			uint32_t await_resume();
			bool await_ready();
			void await_suspend(std::coroutine_handle<> awaiting);	
			void set_return(uint32_t _val);
			void resume_ex();
		};
		struct awtClose : coList
		{
			coClient* _this = nullptr;
			std::coroutine_handle<> m_awaiting = nullptr;
			awtClose(coClient* _);
			~awtClose();
			void await_resume();
			bool await_ready();
			void await_suspend(std::coroutine_handle<> awaiting);
			void resume_ex();
		};		
	public:
		coClient(int, VClose*);	
		~coClient();
		int on_recv();		
		bool is_ready();
		void on_process();	
		void on_process_ex();
		void on_process(coMsg*);		
		void safe_append(coClient& o);
		void set_waittime(size_t);
		const coMsg* get_data();
		uint32_t lastindex() { return m_index; }

		awtRead co_read(uint32_t _idx = e_rpc_sys_msg_custom);
		awtWrite co_write(ioBuffer<char>& _buf);
		awtWrite co_iwrite(ioBuffer<char>& _buf);//自动设置index
		awtClose co_close();		
		coTask co_rpc(ioBuffer<char>& _buf, std::function<void(const coMsg*)>&& f);
	public:
		std::string m_addr;
		coFunc co_process;
		std::bitset<e_config_end> m_config;
	private:		
		coList m_readlist;				
		coList m_writelist;
		coList m_closelist;
		uint32_t m_index = 0;
		uint32_t m_waittime = DEFINE_WAITTIME;
		std::unordered_map<uint32_t, awtRead*> m_process;
		std::list<awtRead*> m_process_list;
	private:		
		int on_close(int);
		int on_timer();
		int _read_clear();
		int _write_clear();
		int _close_clear();
		int send_ack();
		uint32_t co_insert(awtRead* awt, uint32_t _idx);
		uint32_t co_erase(awtRead* awt, uint32_t _idx);		
	};
}
#endif
