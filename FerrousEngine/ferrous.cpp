#include "ferrous.h"

FerrousInstance::FerrousInstance() {
	_mem = Memory::get(); // TODO this is where we should set the page size.
	_time = _mem->allocType<GameTime>();
	_settings = _mem->allocType<Settings>();
	_log = _mem->allocType<Logger>();
	_stopping = false;
}

FerrousInstance::~FerrousInstance() {
	_mem->deallocType(_settings);
	_mem->deallocType(_time);
	_mem->deallocType(_log);

	_time = nullptr;
	_settings = nullptr;
	_mem = nullptr;
}

Settings* FerrousInstance::getSettings() {
	return _settings;
}

Logger* FerrousInstance::getLog() {
	return _log;
}

GameTime* FerrousInstance::getTime() {
	return _time;
}

void FerrousInstance::start() {
	// TODO device and asset initialization goes here

	onStart(_time, _settings);

	while (!_stopping) {
		uint32_t updates_needed = _time->tick();

		for (int i = 0; i < updates_needed; i++) {
			_log->writeLineF("Frame %d -- time: %f ms -- delta: %f", _time->getFrameId(), _time->getFrameTime(), _time->getDelta());
			onUpdate(_time);
		}
	}

	onStopping();
	_log->writeLineF("Finished shut down");
	_stopping = false;
}

void FerrousInstance::stop() {
	assert(!_stopping); // Stop called more than once.

	_stopping = true;
}