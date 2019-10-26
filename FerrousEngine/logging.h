#pragma once
#include "stdafx.h"
#include "allocation.h"
#include "strings.h"
#include "color.h"
#include "list.h"

using namespace fe::collections;

namespace fe {
	class LogOutputBase {
	public:
		virtual void write(const FeString& msg, const Color& color) = 0;
		virtual void writeLine(const FeString& msg, const Color& color) = 0;
		virtual void clear() = 0;
		virtual void close() = 0;
	};

	class Logger {
	public:
		Logger();
		~Logger();

		void addOutput(LogOutputBase* output);

		void writeLine(const FeString& msg, const Color& color = Color::white);

		void write(const FeString& msg, const Color& color = Color::white);

		/* A formattable version of write() */
		void writeF(const FeString& str, const Color& color, ...);

		/* A formattable version of writeLine()*/
		void writeLineF(const FeString& str, const Color& color, ...);

		/* A formattable version of write() */
		void writeF(const FeString& str, ...);

		/* A formattable version of writeLine()*/
		void writeLineF(const FeString& str, ...);

		void clear();

	private:
		List<LogOutputBase*> _outputs;
	};
}