/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_RINGBUFFER_H_HEADER_GUARD
#	error "Must be included from bx/ringbuffer.h!"
#endif // BX_RINGBUFFER_H_HEADER_GUARD

namespace bx
{
	inline RingBufferControl::RingBufferControl(uint32_t _size)
		: m_size(max(_size, 2) )
		, m_current(0)
		, m_write(0)
		, m_read(0)
	{
	}

	inline RingBufferControl::~RingBufferControl()
	{
	}

	inline bool RingBufferControl::isEmpty() const
	{
		return m_read == m_write;
	}

	inline uint32_t RingBufferControl::getSize() const
	{
		return m_size;
	}

	inline uint32_t RingBufferControl::getNumEmpty() const
	{
		return m_size - distance(m_read, m_write) - 1;
	}

	inline uint32_t RingBufferControl::getNumUsed() const
	{
		return distance(m_read, m_current);
	}

	inline uint32_t RingBufferControl::getNumReserved() const
	{
		return distance(m_current, m_write);
	}

	inline void RingBufferControl::resize(int32_t _size)
	{
		_size = 0 > _size
			// can shrink only by number of empty slots.
			? bx::max(_size, -int32_t(getNumEmpty() ) )
			: _size
			;

		m_size += _size;

		m_current += m_current >= m_write ? _size : 0;
		m_read    += m_read    >= m_write ? _size : 0;
	}

	inline uint32_t RingBufferControl::consume(uint32_t _size)
	{
		const simd32_t maxSize    = simd32_splat(distance(m_read, m_current) );
		const simd32_t size       = simd32_splat(_size);
		const simd32_t signMask   = simd32_splat(0x7fffffffu);
		const simd32_t sizeNoSign = simd32_and(size, signMask);
		const simd32_t test       = simd32_u32_sub(sizeNoSign, maxSize);
		const simd32_t result     = simd32_sels(test, size, maxSize);
		const simd32_t readVal    = simd32_splat(m_read);
		const simd32_t advance    = simd32_u32_add(readVal, result);
		const simd32_t sizeVal    = simd32_splat(m_size);
		const simd32_t read       = simd32_u32_mod(advance, sizeVal);
		m_read = read.u32;
		return result.u32;
	}

	inline uint32_t RingBufferControl::reserve(uint32_t _size, bool _mustSucceed)
	{
		const simd32_t distVal    = simd32_splat(distance(m_write, m_read) );
		const simd32_t one        = simd32_splat(1u);
		const simd32_t dist       = simd32_u32_sub(distVal, one);
		const simd32_t sizeVal    = simd32_splat(m_size);
		const simd32_t sizem1     = simd32_u32_sub(sizeVal, one);
		const simd32_t maxSize    = simd32_sels(dist, sizem1, dist);
		const simd32_t size       = simd32_splat(_size);
		const simd32_t signMask   = simd32_splat(0x7fffffffu);
		const simd32_t sizeNoSign = simd32_and(size, signMask);
		const simd32_t test       = simd32_u32_sub(sizeNoSign, maxSize);
		const simd32_t zero       = simd32_splat(0u);
		const simd32_t fail       = _mustSucceed ? zero : maxSize;
		const simd32_t result     = simd32_sels(test, size, fail);
		const simd32_t writeVal   = simd32_splat(m_write);
		const simd32_t advance    = simd32_u32_add(writeVal, result);
		const simd32_t write      = simd32_u32_mod(advance, sizeVal);
		m_write = write.u32;
		return result.u32;
	}

	inline uint32_t RingBufferControl::commit(uint32_t _size)
	{
		const simd32_t maxSize    = simd32_splat(distance(m_current, m_write) );
		const simd32_t size       = simd32_splat(_size);
		const simd32_t signMask   = simd32_splat(0x7fffffffu);
		const simd32_t sizeNoSign = simd32_and(size, signMask);
		const simd32_t test       = simd32_u32_sub(sizeNoSign, maxSize);
		const simd32_t result     = simd32_sels(test, size, maxSize);
		const simd32_t currentVal = simd32_splat(m_current);
		const simd32_t advance    = simd32_u32_add(currentVal, result);
		const simd32_t sizeVal    = simd32_splat(m_size);
		const simd32_t current    = simd32_u32_mod(advance, sizeVal);
		m_current = current.u32;
		return result.u32;
	}

	inline uint32_t RingBufferControl::distance(uint32_t _from, uint32_t _to) const
	{
		const simd32_t to      = simd32_splat(_to);
		const simd32_t from    = simd32_splat(_from);
		const simd32_t diff    = simd32_u32_sub(to, from);
		const simd32_t sizeVal = simd32_splat(m_size);
		const simd32_t le      = simd32_u32_add(sizeVal, diff);
		const simd32_t result  = simd32_sels(diff, le, diff);
		return result.u32;
	}

	inline void RingBufferControl::reset()
	{
		m_current = 0;
		m_write   = 0;
		m_read    = 0;
	}

	inline SpScRingBufferControl::SpScRingBufferControl(uint32_t _size)
		: m_size(max(_size, 2) )
		, m_current(0)
		, m_write(0)
		, m_read(0)
	{
	}

	inline SpScRingBufferControl::~SpScRingBufferControl()
	{
	}

	inline bool SpScRingBufferControl::isEmpty() const
	{
		return m_read == m_write;
	}

	inline uint32_t SpScRingBufferControl::getSize() const
	{
		return m_size;
	}

	inline uint32_t SpScRingBufferControl::getNumEmpty() const
	{
		return m_size - distance(m_read, m_write) - 1;
	}

	inline uint32_t SpScRingBufferControl::getNumUsed() const
	{
		return distance(m_read, m_current);
	}

	inline uint32_t SpScRingBufferControl::getNumReserved() const
	{
		return distance(m_current, m_write);
	}

	inline void SpScRingBufferControl::resize(int32_t _size)
	{
		_size = 0 > _size
			// can shrink only by number of empty slots.
			? bx::max(_size, -int32_t(getNumEmpty() ) )
			: _size
			;

		m_size += _size;

		m_current += m_current >= m_write ? _size : 0;
		m_read    += m_read    >= m_write ? _size : 0;
	}

	inline uint32_t SpScRingBufferControl::consume(uint32_t _size)
	{
		const simd32_t maxSize    = simd32_splat(distance(m_read, m_current) );
		const simd32_t size       = simd32_splat(_size);
		const simd32_t signMask   = simd32_splat(0x7fffffffu);
		const simd32_t sizeNoSign = simd32_and(size, signMask);
		const simd32_t test       = simd32_u32_sub(sizeNoSign, maxSize);
		const simd32_t result     = simd32_sels(test, size, maxSize);
		const simd32_t readVal    = simd32_splat(m_read);
		const simd32_t advance    = simd32_u32_add(readVal, result);
		const simd32_t sizeVal    = simd32_splat(m_size);
		const simd32_t read       = simd32_u32_mod(advance, sizeVal);
		m_read = read.u32;
		return result.u32;
	}

	inline uint32_t SpScRingBufferControl::reserve(uint32_t _size, bool _mustSucceed)
	{
		const simd32_t distVal    = simd32_splat(distance(m_write, m_read) );
		const simd32_t one        = simd32_splat(1u);
		const simd32_t dist       = simd32_u32_sub(distVal, one);
		const simd32_t sizeVal    = simd32_splat(m_size);
		const simd32_t sizem1     = simd32_u32_sub(sizeVal, one);
		const simd32_t maxSize    = simd32_sels(dist, sizem1, dist);
		const simd32_t size       = simd32_splat(_size);
		const simd32_t signMask   = simd32_splat(0x7fffffffu);
		const simd32_t sizeNoSign = simd32_and(size, signMask);
		const simd32_t test       = simd32_u32_sub(sizeNoSign, maxSize);
		const simd32_t zero       = simd32_splat(0u);
		const simd32_t fail       = _mustSucceed ? zero : maxSize;
		const simd32_t result     = simd32_sels(test, size, fail);
		const simd32_t writeVal   = simd32_splat(m_write);
		const simd32_t advance    = simd32_u32_add(writeVal, result);
		const simd32_t write      = simd32_u32_mod(advance, sizeVal);
		m_write = write.u32;
		return result.u32;
	}

	inline uint32_t SpScRingBufferControl::commit(uint32_t _size)
	{
		const simd32_t maxSize    = simd32_splat(distance(m_current, m_write) );
		const simd32_t size       = simd32_splat(_size);
		const simd32_t signMask   = simd32_splat(0x7fffffffu);
		const simd32_t sizeNoSign = simd32_and(size, signMask);
		const simd32_t test       = simd32_u32_sub(sizeNoSign, maxSize);
		const simd32_t result     = simd32_sels(test, size, maxSize);
		const simd32_t currentVal = simd32_splat(m_current);
		const simd32_t advance    = simd32_u32_add(currentVal, result);
		const simd32_t sizeVal    = simd32_splat(m_size);
		const simd32_t current    = simd32_u32_mod(advance, sizeVal);

		// must commit all memory writes before moving m_current pointer
		// once m_current pointer moves data is used by consumer thread
		memoryBarrier();
		m_current = current.u32;
		return result.u32;
	}

	inline uint32_t SpScRingBufferControl::distance(uint32_t _from, uint32_t _to) const
	{
		const simd32_t to      = simd32_splat(_to);
		const simd32_t from    = simd32_splat(_from);
		const simd32_t diff    = simd32_u32_sub(to, from);
		const simd32_t sizeVal = simd32_splat(m_size);
		const simd32_t le      = simd32_u32_add(sizeVal, diff);
		const simd32_t result  = simd32_sels(diff, le, diff);
		return result.u32;
	}

	inline void SpScRingBufferControl::reset()
	{
		m_current = 0;
		m_write   = 0;
		m_read    = 0;
	}

	template <typename ControlT>
	inline ReadRingBufferT<ControlT>::ReadRingBufferT(ControlT& _control, const char* _buffer, uint32_t _size)
		: m_control(_control)
		, m_read(_control.m_read)
		, m_end(m_read+_size)
		, m_size(_size)
		, m_buffer(_buffer)
	{
		BX_ASSERT(_control.getNumUsed() >= _size, "%d >= %d", _control.getNumUsed(), _size);
	}

	template <typename ControlT>
	inline ReadRingBufferT<ControlT>::~ReadRingBufferT()
	{
	}

	template <typename ControlT>
	inline void ReadRingBufferT<ControlT>::end()
	{
		m_control.consume(m_size);
	}

	template <typename ControlT>
	inline void ReadRingBufferT<ControlT>::read(char* _data, uint32_t _len)
	{
		const uint32_t eof = (m_read + _len) % m_control.m_size;
		uint32_t wrap = 0;
		const char* from = &m_buffer[m_read];

		if (eof < m_read)
		{
			wrap = m_control.m_size - m_read;
			memCopy(_data, from, wrap);
			_data += wrap;
			from = (const char*)&m_buffer[0];
		}

		memCopy(_data, from, _len-wrap);

		m_read = eof;
	}

	template <typename ControlT>
	inline void ReadRingBufferT<ControlT>::skip(uint32_t _len)
	{
		m_read += _len;
		m_read %= m_control.m_size;
	}

	template <typename ControlT>
	inline WriteRingBufferT<ControlT>::WriteRingBufferT(ControlT& _control, char* _buffer, uint32_t _size)
		: m_control(_control)
		, m_size(_size)
		, m_buffer(_buffer)
	{
		uint32_t size = m_control.reserve(_size);
		BX_UNUSED(size);
		BX_ASSERT(size == _size, "%d == %d", size, _size);
		m_write = m_control.m_current;
		m_end = m_write+_size;
	}

	template <typename ControlT>
	inline WriteRingBufferT<ControlT>::~WriteRingBufferT()
	{
	}

	template <typename ControlT>
	inline void WriteRingBufferT<ControlT>::end()
	{
		m_control.commit(m_size);
	}

	template <typename ControlT>
	inline void WriteRingBufferT<ControlT>::write(const char* _data, uint32_t _len)
	{
		const uint32_t eof = (m_write + _len) % m_control.m_size;
		uint32_t wrap = 0;
		char* to = &m_buffer[m_write];

		if (eof < m_write)
		{
			wrap = m_control.m_size - m_write;
			memCopy(to, _data, wrap);
			_data += wrap;
			to = (char*)&m_buffer[0];
		}

		memCopy(to, _data, _len-wrap);

		m_write = eof;
	}

	template <typename ControlT>
	inline void WriteRingBufferT<ControlT>::write(ReadRingBufferT<ControlT>& _read, uint32_t _len)
	{
		const uint32_t eof = (_read.m_read + _len) % _read.m_control.m_size;
		uint32_t wrap = 0;
		const char* from = &_read.m_buffer[_read.m_read];

		if (eof < _read.m_read)
		{
			wrap = _read.m_control.m_size - _read.m_read;
			write(from, wrap);
			from = (const char*)&_read.m_buffer[0];
		}

		write(from, _len-wrap);

		_read.m_read = eof;
	}

	template <typename ControlT>
	inline void WriteRingBufferT<ControlT>::skip(uint32_t _len)
	{
		m_write += _len;
		m_write %= m_control.m_size;
	}

} // namespace bx
