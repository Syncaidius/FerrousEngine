#pragma once
#include "settings.h"
#include "allocation.h"
#include "game_time.h"
#include "logging.h"

namespace fe {
	/* A base class on which your build an application, to interact with Ferrous Engine.*/
	class FerrousInstance {
	public:
		FerrousInstance();
		~FerrousInstance();

		/* initialize the engine. */
		void start();

		/* shutdown the engine.*/
		void stop();

		/* Gets the settings info bound to the current ferrous instance. */
		Settings* getSettings();

		/* Gets the logger bound to the current ferrous instance. */
		Logger* getLog();

		/* Gets the timing bound to the current ferrous instance. */
		GameTime* getTime();

	protected:
		/* Invoked when an update is performed.*/
		virtual void onUpdate(GameTime* time) = 0;

		/* Invoked right before the update loop is started. This is usually after a call to start(). */
		virtual void onStart(GameTime* time, Settings* settings) = 0;

		/* Invoked right after the update loop is stopped. The is usually after a call to stop(). */
		virtual void onStopping() = 0;

	private:
		Memory* _mem;
		Settings* _settings;
		GameTime* _time;
		Logger* _log;
		bool _stopping;
	};
}