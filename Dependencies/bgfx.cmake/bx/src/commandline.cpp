/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include <bx/commandline.h>
#include <bx/readerwriter.h>
#include <bx/scanner.h>
#include <bx/string.h>

namespace bx
{
	static void skipSpace(Scanner& _scanner, char _term)
	{
		while (_scanner.peek(_term).isEmpty()
		&&    !_scanner.accept(isSpace).isEmpty() )
		{
		}
	}

	static bool isOrdinary(char _ch)
	{
		return '\0' != _ch
			&& '"'  != _ch
			&& '\\' != _ch
			&& !isSpace(_ch)
			;
	}

	// Reference(s):
	// - https://web.archive.org/web/20180629044234/https://msdn.microsoft.com/en-us/library/a1y7w461.aspx
	//
	StringView tokenizeCommandLine(const StringView& _commandLine, char* _buffer, uint32_t& _bufferSize, int32_t& _argc, char* _argv[], int32_t _maxArgvs, char _term)
	{
		Scanner scanner(_commandLine);

		int32_t argc = 0;
		char* currOut = _buffer;

		while (argc < _maxArgvs)
		{
			skipSpace(scanner, _term);

			if (scanner.isDone()
			|| !scanner.peek(_term).isEmpty() )
			{
				break;
			}

			_argv[argc] = currOut;
			++argc;

			for (bool quoted = false; !scanner.isDone() && scanner.peek(_term).isEmpty(); )
			{
				const StringView slashes = scanner.acceptWhile("\\");

				if (!slashes.isEmpty() )
				{
					if (1 == slashes.getLength()
					&& (scanner.isDone() || !scanner.peek(_term).isEmpty() ) )
					{
						break;
					}

					const bool escape = !scanner.peek('"').isEmpty();
					const int32_t num = escape
						? slashes.getLength()/2
						: slashes.getLength()
						;

					for (int32_t ii = 0; ii < num; ++ii)
					{
						*currOut = '\\';
						++currOut;
					}

					if (escape
					&&  0 != (slashes.getLength() & 1) )
					{
						scanner.accept('"');

						*currOut = '"';
						++currOut;
					}

					continue;
				}

				if (!scanner.accept('"').isEmpty() )
				{
					quoted = !quoted;
					continue;
				}

				const StringView space = scanner.accept(isSpace);

				if (!space.isEmpty() )
				{
					if (!quoted)
					{
						break;
					}

					*currOut = *space.getPtr();
					++currOut;

					continue;
				}

				const bool bulk = !isOrdinary(_term)
					&&  isOrdinary(*scanner.peek().getPtr() )
					;

				const StringView text = bulk
					? scanner.acceptWhile(isOrdinary)
					: scanner.accept()
					;

				memCopy(currOut, text.getPtr(), text.getLength() );
				currOut += text.getLength();
			}

			*currOut = '\0';
			++currOut;
		}

		*currOut = '\0';

		if (0 < argc
		&&  '\0' == _argv[argc-1][0])
		{
			--argc;
		}

		_bufferSize = uint32_t(currOut - _buffer);
		_argc       = argc;

		scanner.accept(_term);

		return StringView(scanner.getCursor().getPtr(), _commandLine.getTerm() );
	}

	static bool isOptionalParam(int32_t _numParams)
	{
		return 0 > _numParams;
	}

	static int32_t maxNumParams(int32_t _numParams)
	{
		return isOptionalParam(_numParams) ? 1 : _numParams;
	}

	constexpr int32_t kMinDescriptionColumn = 27;
	constexpr int32_t kMaxDescriptionColumn = 40;

	static bool isDocumented(const CommandLineOption& _option)
	{
		return NULL != _option.description
			&& '\0' != _option.description[0]
			;
	}

	static int32_t writeOptionName(WriterI* _writer, const CommandLineOption& _option, Error* _err)
	{
		int32_t total = 0;

		if ('\0' != _option.shortName)
		{
			total += write(_writer, _err, "  -%c", _option.shortName);

			if (NULL != _option.longName)
			{
				total += write(_writer, ", ", _err);
			}
		}
		else
		{
			total += writeRep(_writer, ' ', 6, _err);
		}

		if (NULL != _option.longName)
		{
			total += write(_writer, _err, "--%s", _option.longName);
		}

		if (NULL != _option.params)
		{
			total += write(_writer, _err, " %s", _option.params);
		}

		return total;
	}

	int32_t write(WriterI* _writer, const CommandLineOption* _options, int32_t _numOptions, Error* _err)
	{
		BX_ERROR_SCOPE(_err);

		int32_t column = kMinDescriptionColumn;

		{
			SizerWriter sizer;
			Error err;

			for (int32_t ii = 0; ii < _numOptions; ++ii)
			{
				const CommandLineOption& option = _options[ii];

				if (!isDocumented(option) )
				{
					continue;
				}

				const int32_t width = writeOptionName(&sizer, option, &err) + 2;

				if (width <= kMaxDescriptionColumn)
				{
					column = max(column, width);
				}
			}
		}

		int32_t total = 0;

		for (int32_t ii = 0; ii < _numOptions && _err->isOk(); ++ii)
		{
			const CommandLineOption& option = _options[ii];

			if (!isDocumented(option) )
			{
				continue;
			}

			BX_ASSERT( (0 == option.numParams) == (NULL == option.params)
				, "Option '-%c' / '--%s' takes %d parameter(s), but help %s them."
				, '\0' == option.shortName ? '?' : option.shortName
				, NULL == option.longName  ? "?" : option.longName
				, option.numParams
				, NULL == option.params ? "doesn't show" : "shows"
				);

			int32_t width = writeOptionName(_writer, option, _err);

			total += width;

			for (StringView desc(option.description); !desc.isEmpty(); )
			{
				total += writeRep(_writer, ' ', max(1, column - width), _err);

				const StringView eol = strFindEol(desc);
				total += write(_writer, StringView(desc.getPtr(), eol.getPtr() ), _err);
				total += write(_writer, "\n", _err);

				width = 0;
				desc  = strFindNl(desc);
			}
		}

		return total;
	}

	CommandLine::CommandLine(int32_t _argc, char const* const* _argv)
		: m_argc(_argc)
		, m_argv(_argv)
		, m_options(NULL)
		, m_numOptions(0)
	{
	}

	CommandLine::CommandLine(int32_t _argc, char const* const* _argv, const CommandLineOption* _options, int32_t _numOptions)
		: m_argc(_argc)
		, m_argv(_argv)
		, m_options(_options)
		, m_numOptions(_numOptions)
	{
	}

	const char* CommandLine::findOption(const char* _long, const char* _default) const
	{
		const char* result = find(0, '\0', _long, 1);
		return result == NULL ? _default : result;
	}

	const char* CommandLine::findOption(const char _short, const char* _long, const char* _default) const
	{
		const char* result = find(0, _short, _long, 1);
		return result == NULL ? _default : result;
	}

	const char* CommandLine::findOption(const char* _long, int32_t _numParams) const
	{
		const char* result = find(0, '\0', _long, _numParams);
		return result;
	}

	const char* CommandLine::findOption(const char _short, const char* _long, int32_t _numParams) const
	{
		const char* result = find(0, _short, _long, _numParams);
		return result;
	}

	const char* CommandLine::findOption(int32_t _skip, const char _short, const char* _long, int32_t _numParams) const
	{
		const char* result = find(_skip, _short, _long, _numParams);
		return result;
	}

	bool CommandLine::hasArg(const char _short, const char* _long) const
	{
		const char* arg = findOption(_short, _long, int32_t(0) );
		return NULL != arg;
	}

	bool CommandLine::hasArg(const char* _long) const
	{
		const char* arg = findOption('\0', _long, int32_t(0) );
		return NULL != arg;
	}

	bool CommandLine::hasArg(const char*& _value, const char _short, const char* _long) const
	{
		const char* arg = findOption(_short, _long, 1);
		_value = arg;
		return NULL != arg;
	}

	bool CommandLine::hasArg(int32_t& _value, const char _short, const char* _long) const
	{
		const char* arg = findOption(_short, _long, 1);
		if (NULL != arg)
		{
			fromString(&_value, arg);
			return true;
		}

		return false;
	}

	bool CommandLine::hasArg(uint32_t& _value, const char _short, const char* _long) const
	{
		const char* arg = findOption(_short, _long, 1);
		if (NULL != arg)
		{
			fromString(&_value, arg);
			return true;
		}

		return false;
	}

	bool CommandLine::hasArg(float& _value, const char _short, const char* _long) const
	{
		const char* arg = findOption(_short, _long, 1);
		if (NULL != arg)
		{
			fromString(&_value, arg);
			return true;
		}

		return false;
	}

	bool CommandLine::hasArg(double& _value, const char _short, const char* _long) const
	{
		const char* arg = findOption(_short, _long, 1);
		if (NULL != arg)
		{
			fromString(&_value, arg);
			return true;
		}

		return false;
	}

	bool CommandLine::hasArg(bool& _value, const char _short, const char* _long) const
	{
		const char* arg = findOption(_short, _long, 1);
		if (NULL != arg)
		{
			if ('0' == *arg || isEqual(arg, "false", false) )
			{
				_value = false;
			}
			else if ('0' != *arg || isEqual(arg, "true", false) )
			{
				_value = true;
			}

			return true;
		}

		return false;
	}

	const CommandLineOption* CommandLine::describe(const char* _arg) const
	{
		if ('-'  != _arg[0]
		||  '\0' == _arg[1] )
		{
			return NULL;
		}

		const char* name   = _arg + 1;
		const bool  isLong = '-' == name[0];

		if (isLong)
		{
			++name;
		}

		for (int32_t ii = 0; ii < m_numOptions; ++ii)
		{
			const CommandLineOption& option = m_options[ii];

			if (isLong)
			{
				if (NULL != option.longName
				&&  isEqual(name, option.longName, false) )
				{
					return &option;
				}
			}
			else if ('\0' != option.shortName
				 &&  option.shortName == name[0]
				 &&  1 == strLen(name) )
			{
				return &option;
			}
		}

		return NULL;
	}

	const CommandLineOption* CommandLine::describe(const char _short, const char* _long) const
	{
		for (int32_t ii = 0; ii < m_numOptions; ++ii)
		{
			const CommandLineOption& option = m_options[ii];

			if ('\0' != _short
			&&  _short == option.shortName)
			{
				return &option;
			}

			if (NULL != _long
			&&  NULL != option.longName
			&&  isEqual(_long, option.longName, false) )
			{
				return &option;
			}
		}

		return NULL;
	}

	bool CommandLine::hasParams(int32_t _idx, int32_t _numParams) const
	{
		if (0 == _numParams)
		{
			return true;
		}

		if (_idx + maxNumParams(_numParams) >= m_argc)
		{
			return false;
		}

		const CommandLineOption* option = NULL == m_options ? NULL : describe(m_argv[_idx]);

		if (NULL != m_options
		&&  (NULL == option || !isOptionalParam(option->numParams) ) )
		{
			return true;
		}

		return '-' != *m_argv[_idx+1];
	}

	int32_t CommandLine::getNumParams(int32_t _idx, int32_t _numParams) const
	{
		const CommandLineOption* option = NULL == m_options ? NULL : describe(m_argv[_idx]);

		const int32_t numParams = NULL == option ? _numParams : maxNumParams(option->numParams);

		return hasParams(_idx, numParams) ? numParams : 0;
	}

	const char* CommandLine::find(int32_t _skip, const char _short, const char* _long, int32_t _numParams) const
	{
		BX_ASSERT(false
			|| NULL == m_options
			|| NULL != describe(_short, _long)
			, "Option '-%c' / '--%s' is not described in the option table."
			, '\0' == _short ? '?' : _short
			, NULL == _long  ? "?" : _long
			);
		BX_ASSERT(false
			|| NULL == m_options
			|| NULL == describe(_short, _long)
			|| _numParams == describe(_short, _long)->numParams
			|| (isOptionalParam(describe(_short, _long)->numParams)
			&&  _numParams <= 1)
			, "Option '-%c' / '--%s' is described as taking %d parameter(s), but is read as taking %d."
			, '\0' == _short ? '?' : _short
			, NULL == _long  ? "?" : _long
			, NULL == describe(_short, _long) ? 0 : describe(_short, _long)->numParams
			, _numParams
			);

		for (int32_t ii = 0; ii < m_argc && !isEqual(m_argv[ii], "--"); ++ii)
		{
			const char* arg = m_argv[ii];
			if ('-' == *arg)
			{
				++arg;
				if (_short == *arg)
				{
					if (1 == strLen(arg) )
					{
						if (0 == _skip)
						{
							if (0 == _numParams)
							{
								return "";
							}

							if (hasParams(ii, _numParams) )
							{
								return m_argv[ii+1];
							}

							return NULL;
						}

						--_skip;
						ii += getNumParams(ii, _numParams);
					}
				}
				else if (NULL != _long
					 &&  '-'  == *arg
					 &&  isEqual(arg+1, _long, false) )
				{
					if (0 == _skip)
					{
						if (0 == _numParams)
						{
							return "";
						}

						if (hasParams(ii, _numParams) )
						{
							return m_argv[ii+1];
						}

						return NULL;
					}

					--_skip;
					ii += getNumParams(ii, _numParams);
				}
			}
		}

		return NULL;
	}

	int32_t CommandLine::getNum() const
	{
		return m_argc;
	}

	char const* CommandLine::get(int32_t _idx) const
	{
		return m_argv[_idx];
	}

	int32_t CommandLine::getNumPositional() const
	{
		int32_t num = 0;

		while (NULL != getPositional(num) )
		{
			++num;
		}

		return num;
	}

	const char* CommandLine::getPositional(int32_t _idx) const
	{
		BX_ASSERT(NULL != m_options
			, "Positional arguments require describing options, otherwise there is no way "
			  "to tell an option parameter from a positional argument."
			);

		int32_t num = 0;

		for (int32_t ii = 0; ii < m_argc; ++ii)
		{
			const char* arg = m_argv[ii];

			if (isEqual(arg, "--") )
			{
				for (++ii; ii < m_argc; ++ii)
				{
					if (_idx == num++)
					{
						return m_argv[ii];
					}
				}

				return NULL;
			}

			const CommandLineOption* option = describe(arg);

			if (NULL != option)
			{
				ii += getNumParams(ii, option->numParams);

				continue;
			}

			if ('-'  == arg[0]
			&&  '\0' != arg[1] )
			{
				continue;
			}

			if (_idx == num++)
			{
				return arg;
			}
		}

		return NULL;
	}

	const char* CommandLine::findUnknownOption() const
	{
		BX_ASSERT(NULL != m_options
			, "Finding unknown options requires describing the known ones."
			);

		for (int32_t ii = 0; ii < m_argc && !isEqual(m_argv[ii], "--"); ++ii)
		{
			const char* arg = m_argv[ii];

			const CommandLineOption* option = describe(arg);

			if (NULL != option)
			{
				ii += getNumParams(ii, option->numParams);
			}
			else if ('-'  == arg[0]
				 &&  '\0' != arg[1] )
			{
				return arg;
			}
		}

		return NULL;
	}

} // namespace bx
