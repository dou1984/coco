#include "CLog.h"
#include "coClient.h"
#include "coMsg.h"
#include "awaiter.h"
#include "coAsync.h"

using milliseconds = std::chrono::milliseconds;
using system_clock = std::chrono::system_clock;

namespace ashan
{
	bool g_proto_comsg = true;
	coClient::coClient(int _fd, VClose *_Close) : evClient(_fd, _Close)
	{
		evTimer::start(2.0, 2.0);
	}
	coClient::~coClient()
	{
		_read_clear();
		_write_clear();
	}
	int coClient::on_recv()
	{
		if (g_proto_comsg)
		{
			while (is_ready())
				on_process();
		}
		else
		{
			on_process_ex();
			m_ReadBuffer.clear();
		}
		return 0;
	}
	bool coClient::is_ready()
	{
		if (m_ReadBuffer.size() < sizeof(coMsg))
			return false;
		coMsg *_msg = (coMsg *)m_ReadBuffer.data();
		return _msg->size() <= m_ReadBuffer.size();
	}
	void coClient::on_process()
	{
		coMsg *_msg = (coMsg *)m_ReadBuffer.data();
		assert(m_ReadBuffer.size() >= sizeof(coMsg));
		if (_msg->index != e_rpc_msg_ack)
		{
			send_ack();
			on_process(_msg);
		}
		else if (auto it = m_writelist.begin(); it != m_writelist.end())
		{
			auto _awt = ((awtWrite *)it);			
			m_writelist.erase(_awt);
			_awt->resume_ex();
		}
		m_ReadBuffer.pop_front(_msg->size());
	}
	void coClient::on_process_ex()
	{
		if (co_process)
		{
			co_process(shared_from_this());
		}
	}
	void coClient::on_process(coMsg *msg)
	{
		auto idx = msg->index;
		if (auto it = m_process.find(idx); it != m_process.end())
		{
			auto _awt = it->second;
			_awt->set_return(msg->size());
			_awt->resume_ex();
		}
		else if (co_process)
		{
			co_process(shared_from_this());
		}
		else
		{
			auto _msg = get_data();
			CO_DBG("recv msg not process index:%d", _msg->index);
		}
	}
	coClient::awtRead coClient::co_read(uint32_t _idx)
	{
		size_t _waittime = m_waittime;
		m_waittime = DEFINE_WAITTIME;
		return {this, _idx, _waittime};
	}
	coClient::awtWrite coClient::co_write_ex(ioBuffer<char>& _buf)
	{
		coMsg* _msg = (coMsg*)_buf.data();
		if (++m_index >= MAX_RPC_INDEX) [[unlikely]]
			m_index = 0;
		_msg->index = m_index;
		assert(_msg->size() == _buf.size());
		return co_write(_buf);
	}
	coClient::awtWrite coClient::co_write(ioBuffer<char>& _buf)
	{
		coMsg* _msg = (coMsg*)_buf.data();
		assert(_msg->size() == _buf.size());
		_send(_buf);
		return { this, _msg->index, m_waittime };
	}
	coClient::awtClose coClient::co_close()
	{
		return {this};
	}
	void coClient::safe_append(coClient& o)
	{
		for (auto& it : o.m_WriteBuffer)
		{
			m_WriteBuffer.emplace_back(std::move(it));
		}
		if (!o.m_ReadBuffer.empty())
		{
			m_ReadBuffer.push_back(o.m_ReadBuffer.data(), o.m_ReadBuffer.size());
			o.m_ReadBuffer.clear();
		}		
		m_readlist.append(o.m_readlist);
		m_process = std::move(o.m_process);
		std::swap(m_index, o.m_index);
		if (o.co_process)
			co_process = std::move(o.co_process);
	}
	int coClient::send_ack()
	{
		coMsg ack(0, e_rpc_msg_ack);
		return _send(ack.ptr(), ack.size());
	}
	uint32_t coClient::co_insert(coClient::awtRead *_awt, uint32_t _idx)
	{
		m_readlist.push_front(_awt);
		m_process.emplace(_idx, _awt);
		return _idx;
	}
	uint32_t coClient::co_erase(coClient::awtRead *_awt, uint32_t _idx)
	{
		m_readlist.erase(_awt);
		m_process.erase(_idx);
		return _idx;
	}
	const coMsg *coClient::get_data()
	{
		return (coMsg *)m_ReadBuffer.data();
	}
	void coClient::set_waittime(size_t _waittime)
	{
		m_waittime = _waittime;
	}
	int coClient::on_timer()
	{
		auto _t = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		coList *it;
		while ((it = m_readlist.begin()) != m_readlist.end())
		{
			auto _awt = ((awtRead *)it);
			if (_awt->_time > _t)
				break;
			_awt->set_return(e_rpc_msg_timeout);
			_awt->resume_ex();
		}
		while ((it = m_writelist.begin()) != m_writelist.end())
		{
			auto _awt = ((awtWrite *)it);
			if (_awt->_time > _t)
				break;
			_awt->set_return(e_rpc_msg_timeout);
			_awt->resume_ex();
		}
		return 0;
	}
	int coClient::_read_clear()
	{
		coList *it;
		while ((it = m_readlist.begin()) != m_readlist.end())
		{
			auto _awt = ((awtRead *)it);
			_awt->set_return(e_rpc_msg_close);
			m_readlist.erase(_awt);
			_awt->resume_ex();
		}
		return 0;
	}
	int coClient::_write_clear()
	{
		coList *it;
		while ((it = m_writelist.begin()) != m_writelist.end())
		{
			auto _awt = ((awtWrite *)it);
			_awt->set_return(e_rpc_msg_close);
			m_writelist.erase(_awt);
			_awt->resume_ex();
		}
		return 0;
	}
	int coClient::_close_clear()
	{
		coList* it;
		while ((it = m_closelist.begin()) != m_closelist.end())
		{
			auto _awt = ((awtClose*)it);
			m_closelist.erase(_awt);
			_awt->resume_ex();
		}
		return 0;
	}
	int coClient::on_close(int _fd)
	{
		_read_clear();
		_write_clear();
		int r = evClient::on_close(_fd);
		_close_clear();
		return r;
	}
	coClient::awtRead::awtRead(coClient *_cli, uint32_t index, size_t _wait) : _this(_cli)
	{
		_idx = _this->co_insert(this, index);
		_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch()) + std::chrono::milliseconds(_wait);
	}
	coClient::awtRead::~awtRead()
	{
		m_awaiting = nullptr;
		_this->co_erase(this, _idx);
	}
	uint32_t coClient::awtRead::await_resume()
	{
		return _value;
	}
	void coClient::awtRead::set_return(uint32_t _val)
	{
		_value = _val;
	}
	bool coClient::awtRead::await_ready()
	{
		if (_idx < e_rpc_msg_timeout)
			return false;
		set_return(_idx);
		return true;
	}
	void coClient::awtRead::await_suspend(std::coroutine_handle<> awaiting)
	{
		m_awaiting = awaiting;
	}
	void coClient::awtRead::resume_ex()
	{
		if (m_awaiting && !m_awaiting.done())
			m_awaiting.resume();
	}
	coClient::awtWrite::awtWrite(coClient *_, uint32_t val, size_t _wait) : _this(_), _value(val)
	{
		_this->m_writelist.push_back(this);
		_time = std::chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch()) + std::chrono::milliseconds(_wait);
	}
	coClient::awtWrite::~awtWrite()
	{
		if (!empty())
			_this->m_writelist.erase(this);
		_this = nullptr;
		m_awaiting = nullptr;
	}
	uint32_t coClient::awtWrite::await_resume()
	{
		return _value;
	}
	bool coClient::awtWrite::await_ready()
	{
		return false;
	}
	void coClient::awtWrite::await_suspend(std::coroutine_handle<> awaiting)
	{
		m_awaiting = awaiting;
	}
	void coClient::awtWrite::set_return(uint32_t _val)
	{
		_value = _val;
	}
	void coClient::awtWrite::resume_ex()
	{
		if (m_awaiting)
		{
			auto _awaiting = m_awaiting;
			m_awaiting = nullptr;
			_awaiting.resume();
		}
	}
	coClient::awtClose::awtClose(coClient *_) : _this(_)
	{
		_this->m_closelist.push_back(this);
	}
	coClient::awtClose::~awtClose()
	{
		if (!empty())
			_this->m_closelist.erase(this);
		m_awaiting = nullptr;
		_this = nullptr;
	}
	void coClient::awtClose::await_resume()
	{
	}
	bool coClient::awtClose::await_ready()
	{
		return !_this->valid();
	}
	void coClient::awtClose::await_suspend(std::coroutine_handle<> awaiting)
	{
		m_awaiting = awaiting;
	}
	void coClient::awtClose::resume_ex()
	{
		if (m_awaiting)
		{
			auto _awaiting = m_awaiting;
			m_awaiting = nullptr;
#ifdef DELAY_RESUME
			coAsync::at()->push_back(_awaiting);
#else
			_awaiting.resume();
#endif
		}
	}
}