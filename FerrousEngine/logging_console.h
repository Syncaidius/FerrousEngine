#pragma once
#include "logging.h"

namespace fe {
	class ConsoleLogOutput : public LogOutputBase {
	public:
		ConsoleLogOutput();
		~ConsoleLogOutput();

	protected:
		friend class Logger;

		void write(const FeString& msg, const Color& color) override;
		void writeLine(const FeString& msg, const Color& color) override;
		void clear() override;
		void close() override;
	private:
		WORD getColorFlags(const Color& color);
		void* _console_handle;
	};
}