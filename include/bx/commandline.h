/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#ifndef BX_COMMANDLINE_H_HEADER_GUARD
#define BX_COMMANDLINE_H_HEADER_GUARD

#include "string.h"

namespace bx
{
	class  Error;
	struct WriterI;

	///
	StringView tokenizeCommandLine(const StringView& _commandLine, char* _buffer, uint32_t& _bufferSize, int32_t& _argc, char* _argv[], int32_t _maxArgvs, char _term = '\0');

	/// Number of parameters for an option whose parameter is optional.
	///
	static constexpr int32_t kCommandLineOptionalParam = -1;

	/// Description of a single command line option.
	///
	struct CommandLineOption
	{
		char        shortName;   //!< Short name `-s`, or '\0' when option has no short name.
		const char* longName;    //!< Long name `--long`, or NULL when option has no long name.
		int32_t     numParams;   //!< Number of arguments option consumes, 0 for flags, or `kCommandLineOptionalParam`.
		const char* params;      //!< Parameters as they appear in help `<file path>`, or NULL for flags.
		const char* description; //!< Description as it appears in help, or NULL/empty to leave option out of it.
	};

	/// Write `_options` as the option list of a help message, one option per line.
	///
	int32_t write(WriterI* _writer, const CommandLineOption* _options, int32_t _numOptions, Error* _err);

	///
	class CommandLine
	{
	public:
		/// Parse `_argv` as options only. Positional arguments are not available, since
		/// telling them apart from option parameters requires knowing which options take
		/// a parameter.
		///
		CommandLine(int32_t _argc, char const* const* _argv);

		/// Parse `_argv` against `_options`, which describes every option the program
		/// accepts, and must outlive `CommandLine`.
		///
		CommandLine(int32_t _argc, char const* const* _argv, const CommandLineOption* _options, int32_t _numOptions);

		///
		const char* findOption(const char* _long, const char* _default) const;

		///
		const char* findOption(const char _short, const char* _long, const char* _default) const;

		///
		const char* findOption(const char* _long, int32_t _numParams = 1) const;

		///
		const char* findOption(const char _short, const char* _long = NULL, int32_t _numParams = 1) const;

		///
		const char* findOption(int32_t _skip, const char _short, const char* _long = NULL, int32_t _numParams = 1) const;

		///
		bool hasArg(const char _short, const char* _long = NULL) const;

		///
		bool hasArg(const char* _long) const;

		///
		bool hasArg(const char*& _value, const char _short, const char* _long = NULL) const;

		///
		bool hasArg(int32_t& _value, const char _short, const char* _long = NULL) const;

		///
		bool hasArg(uint32_t& _value, const char _short, const char* _long = NULL) const;

		///
		bool hasArg(float& _value, const char _short, const char* _long = NULL) const;

		///
		bool hasArg(double& _value, const char _short, const char* _long = NULL) const;

		///
		bool hasArg(bool& _value, const char _short, const char* _long = NULL) const;

		///
		int32_t getNum() const;

		///
		char const* get(int32_t _idx) const;

		/// Number of arguments that are neither an option nor an option parameter.
		///
		int32_t getNumPositional() const;

		/// Positional argument at `_idx`, or NULL when there are fewer than `_idx`+1 of
		/// them. Everything past the `--` terminator is positional.
		///
		const char* getPositional(int32_t _idx) const;

		/// First argument that looks like an option, but is not one of the described
		/// options, or NULL when there is no such argument. Useful for rejecting typos
		/// instead of silently ignoring them.
		///
		const char* findUnknownOption() const;

	private:
		///
		const char* find(int32_t _skip, const char _short, const char* _long, int32_t _numParams) const;

		/// Whether option at argv index `_idx` has its `_numParams` parameters.
		///
		bool hasParams(int32_t _idx, int32_t _numParams) const;

		/// Number of parameters the option at argv index `_idx` consumes.
		///
		int32_t getNumParams(int32_t _idx, int32_t _numParams) const;

		/// Option `_arg` describes, or NULL if it describes none. `_arg` is a whole
		/// argument, leading `-` included.
		///
		const CommandLineOption* describe(const char* _arg) const;

		/// Option with `_short`/`_long` name, or NULL if there is none.
		///
		const CommandLineOption* describe(const char _short, const char* _long) const;

		int32_t                  m_argc;
		char const* const*       m_argv;
		const CommandLineOption* m_options;
		int32_t                  m_numOptions;
	};

} // namespace bx

#endif /// BX_COMMANDLINE_H_HEADER_GUARD
