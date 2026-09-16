/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include <bx/settings.h>
#include <bx/filepath.h>
#include <bx/scanner.h>

namespace bx
{
	static StringView settingsAlloc(AllocatorI* _allocator, const StringView& _str)
	{
		const int32_t len = _str.getLength();

		if (0 == len)
		{
			return StringView();
		}

		char* ptr = (char*)alloc(_allocator, len);
		memCopy(ptr, _str.getPtr(), len);

		return StringView(ptr, len);
	}

	static void settingsFree(AllocatorI* _allocator, StringView& _str)
	{
		if (!_str.isEmpty() )
		{
			free(_allocator, const_cast<char*>(_str.getPtr() ) );
		}

		_str.clear();
	}

	static void settingsAssign(AllocatorI* _allocator, StringView& _dst, const StringView& _src)
	{
		const StringView next = settingsAlloc(_allocator, _src);
		settingsFree(_allocator, _dst);
		_dst = next;
	}

	struct Settings::Ini
	{
		static constexpr int32_t kGlobal  =  0;
		static constexpr int32_t kInvalid = -1;

		struct Property
		{
			StringView name;
			StringView value;
		};

		struct Section
		{
			StringView name;
			Property*  props;
			uint32_t   count;
			uint32_t   capacity;
		};

		void init(AllocatorI* _allocator)
		{
			m_allocator = _allocator;
			m_sections  = NULL;
			m_count     = 0;
			m_capacity  = 0;

			clear();
		}

		void shutdown()
		{
			reset();

			if (NULL != m_sections)
			{
				free(m_allocator, m_sections);
				m_sections = NULL;
			}

			m_capacity = 0;
		}

		void clear()
		{
			reset();

			addSection(StringView() );
		}

		void load(const StringView& _data)
		{
			clear();

			Scanner scanner(_data);
			int32_t section = kGlobal;

			while (!scanner.isDone() )
			{
				scanner.accept(Scanner::Class::Space);

				if (scanner.isDone() )
				{
					break;
				}

				Scanner line(scanner.acceptUntil(Scanner::Class::EndOfLine) );

				// Line comment.
				if (!line.accept(';').isEmpty() )
				{
					continue;
				}

				// [section] header.
				if (!line.accept('[').isEmpty() )
				{
					line.accept(Scanner::Class::Space);
					const StringView name = strRTrimSpace(line.acceptUntil("]") );

					if (!line.accept(']').isEmpty()
					&&  !name.isEmpty() )
					{
						const int32_t existing = findSection(name);
						section = kInvalid == existing ? addSection(name) : existing;
					}

					continue;
				}

				// name = value property.
				const StringView name = strRTrimSpace(line.acceptUntil("=") );

				if (line.accept('=').isEmpty()
				||  name.isEmpty() )
				{
					continue;
				}

				line.accept(Scanner::Class::Space);
				setProperty(section, name, strRTrimSpace(line.acceptAll() ) );
			}
		}

		int32_t find(const StringView& _name, StringView& _propertyName) const
		{
			const FilePath uri(_name);
			const StringView path(strTrim(uri.getPath(), "/") );

			_propertyName = uri.getFileName();

			if (path.isEmpty() )
			{
				return kGlobal;
			}

			const int32_t section = findSection(path);

			return kInvalid == section ? kGlobal : section;
		}

		int32_t findSection(const StringView& _name) const
		{
			for (uint32_t ss = 0; ss < m_count; ++ss)
			{
				const StringView& name = m_sections[ss].name;

				if (isEqual(name, _name, false) )
				{
					return int32_t(ss);
				}
			}

			return kInvalid;
		}

		int32_t addSection(const StringView& _name)
		{
			if (m_count == m_capacity)
			{
				m_capacity = 0 == m_capacity ? 8 : m_capacity * 2;
				m_sections = (Section*)realloc(m_allocator, m_sections, m_capacity * sizeof(Section) );
			}

			Section& section = m_sections[m_count];
			section.name     = settingsAlloc(m_allocator, _name);
			section.props    = NULL;
			section.count    = 0;
			section.capacity = 0;

			return int32_t(m_count++);
		}

		void removeSection(int32_t _section)
		{
			if (kGlobal >= _section
			||  uint32_t(_section) >= m_count)
			{
				return;
			}

			freeSection(m_sections[_section]);

			for (uint32_t ss = uint32_t(_section); ss+1 < m_count; ++ss)
			{
				m_sections[ss] = m_sections[ss+1];
			}

			--m_count;
		}

		int32_t findProperty(int32_t _section, const StringView& _name) const
		{
			if (0 > _section
			||  uint32_t(_section) >= m_count)
			{
				return kInvalid;
			}

			const Section& section = m_sections[_section];

			for (uint32_t pp = 0; pp < section.count; ++pp)
			{
				const StringView& name = section.props[pp].name;

				if (isEqual(name, _name, false) )
				{
					return int32_t(pp);
				}
			}

			return kInvalid;
		}

		void setProperty(int32_t _section, const StringView& _name, const StringView& _value)
		{
			if (0 > _section
			||  uint32_t(_section) >= m_count)
			{
				return;
			}

			const int32_t property = findProperty(_section, _name);

			Section& section = m_sections[_section];

			if (kInvalid != property)
			{
				settingsAssign(m_allocator, section.props[property].value, _value);
				return;
			}

			if (section.count == section.capacity)
			{
				section.capacity = 0 == section.capacity ? 8 : section.capacity * 2;
				section.props    = (Property*)realloc(m_allocator, section.props, section.capacity * sizeof(Property) );
			}

			Property& prop = section.props[section.count];
			prop.name  = settingsAlloc(m_allocator, _name);
			prop.value = settingsAlloc(m_allocator, _value);

			++section.count;
		}

		void removeProperty(int32_t _section, int32_t _property)
		{
			if (0 > _section
			||  uint32_t(_section) >= m_count)
			{
				return;
			}

			Section& section = m_sections[_section];

			if (0 > _property
			||  uint32_t(_property) >= section.count)
			{
				return;
			}

			settingsFree(m_allocator, section.props[_property].name);
			settingsFree(m_allocator, section.props[_property].value);

			for (uint32_t pp = uint32_t(_property); pp+1 < section.count; ++pp)
			{
				section.props[pp] = section.props[pp+1];
			}

			--section.count;
		}

		StringView getPropertyValue(int32_t _section, int32_t _property) const
		{
			if (0 > _section
			||  uint32_t(_section) >= m_count)
			{
				return StringView();
			}

			const Section& section = m_sections[_section];

			return (0 <= _property && uint32_t(_property) < section.count)
				? section.props[_property].value
				: StringView()
				;
		}

		uint32_t getPropertyCount(int32_t _section) const
		{
			return (0 <= _section && uint32_t(_section) < m_count)
				? m_sections[_section].count
				: 0
				;
		}

		int32_t save(WriterI* _writer, Error* _err) const
		{
			int32_t total = 0;

			for (uint32_t ss = 0; ss < m_count; ++ss)
			{
				const Section& section = m_sections[ss];

				if (!section.name.isEmpty() )
				{
					total += bx::write(_writer, "[", _err);
					total += bx::write(_writer, section.name, _err);
					total += bx::write(_writer, "]\n", _err);
				}

				for (uint32_t pp = 0; pp < section.count; ++pp)
				{
					total += bx::write(_writer, section.props[pp].name, _err);
					total += bx::write(_writer, "=", _err);
					total += bx::write(_writer, section.props[pp].value, _err);
					total += bx::write(_writer, "\n", _err);
				}

				if (0 != total)
				{
					total += bx::write(_writer, "\n", _err);
				}
			}

			return total;
		}

		void freeSection(Section& _section)
		{
			for (uint32_t pp = 0; pp < _section.count; ++pp)
			{
				settingsFree(m_allocator, _section.props[pp].name);
				settingsFree(m_allocator, _section.props[pp].value);
			}

			if (NULL != _section.props)
			{
				free(m_allocator, _section.props);
			}

			settingsFree(m_allocator, _section.name);

			_section.props    = NULL;
			_section.count    = 0;
			_section.capacity = 0;
		}

		void reset()
		{
			for (uint32_t ss = 0; ss < m_count; ++ss)
			{
				freeSection(m_sections[ss]);
			}

			m_count = 0;
		}

		AllocatorI* m_allocator;
		Section*    m_sections;
		uint32_t    m_count;
		uint32_t    m_capacity;
	};

	Settings::Settings(AllocatorI* _allocator, const void* _data, uint32_t _len)
		: m_allocator(_allocator)
		, m_ini(BX_NEW(_allocator, Ini) )
	{
		m_ini->init(_allocator);
		load(_data, _len);
	}

	Settings::~Settings()
	{
		m_ini->shutdown();
		deleteObject(m_allocator, m_ini);
	}

	void Settings::clear()
	{
		m_ini->clear();
	}

	void Settings::load(const void* _data, uint32_t _len)
	{
		if (NULL == _data
		||  0    == _len)
		{
			m_ini->clear();
			return;
		}

		m_ini->load(StringView( (const char*)_data, int32_t(_len) ) );
	}

	StringView Settings::get(const StringView& _name) const
	{
		StringView propertyName;
		const int32_t section = m_ini->find(_name, propertyName);

		return m_ini->getPropertyValue(section, m_ini->findProperty(section, propertyName) );
	}

	void Settings::set(const StringView& _name, const StringView& _value)
	{
		const FilePath uri(_name);
		const StringView  path(strTrim(uri.getPath(), "/") );
		const StringView& fileName(uri.getFileName() );

		int32_t section = Ini::kGlobal;

		if (!path.isEmpty() )
		{
			section = m_ini->findSection(path);

			if (Ini::kInvalid == section)
			{
				section = m_ini->addSection(path);
			}
		}

		m_ini->setProperty(section, fileName, _value);
	}

	void Settings::remove(const StringView& _name) const
	{
		StringView propertyName;
		const int32_t section = m_ini->find(_name, propertyName);

		const int32_t property = m_ini->findProperty(section, propertyName);

		if (Ini::kInvalid == property)
		{
			return;
		}

		m_ini->removeProperty(section, property);

		if (Ini::kGlobal != section
		&&  0 == m_ini->getPropertyCount(section) )
		{
			m_ini->removeSection(section);
		}
	}

	int32_t Settings::read(ReaderSeekerI* _reader, Error* _err)
	{
		int32_t size = int32_t(getRemain(_reader) );

		void* data = bx::alloc(m_allocator, size);

		int32_t total = bx::read(_reader, data, size, _err);
		load(data, size);

		bx::free(m_allocator, data);

		return total;
	}

	int32_t Settings::write(WriterI* _writer, Error* _err) const
	{
		return m_ini->save(_writer, _err);
	}

	int32_t read(ReaderSeekerI* _reader, Settings& _settings, Error* _err)
	{
		BX_ERROR_SCOPE(_err);
		return _settings.read(_reader, _err);
	}

	int32_t write(WriterI* _writer, const Settings& _settings, Error* _err)
	{
		BX_ERROR_SCOPE(_err);
		return _settings.write(_writer, _err);
	}

} // namespace bx
