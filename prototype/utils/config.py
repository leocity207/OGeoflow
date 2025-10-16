import json
from pathlib import Path
from threading import Lock
from typing import Any, Dict


class Config:
	"""
	Thread-safe, lazy-loaded configuration loader (Singleton-style).

	This class reads ``config.json`` only once and caches the data.
	Access is provided via :meth:`Config.Get`, :meth:`Config.All`, or
	dict-like access using ``Config["key"]``.

	Example::

		value = Config.Get("river", default={})
		all_data = Config.All()
		api_key = Config["river"]
	"""

	_data: Dict[str, Any] = None
	"""data dictionary of the config"""
	_lock: Lock = Lock()
	"""lock for initialization"""

	def __new__(cls, *args, **kwargs):
		"""
		Prevent direct instantiation.

		:raises RuntimeError: Always, since this class is meant to be used statically.
		"""
		raise RuntimeError("Use Config.Get() or Config[...] instead of instantiating.")

	@classmethod
	def _initialize(cls) -> None:
		"""
		initialize the configuration file into memory (thread-safe).

		This method uses double-checked locking to ensure the config
		is only loaded once, even in multi-threaded environments.

		:raises FileNotFoundError: If the ``config.json`` file does not exist.
		:raises JSONDecodeError: If the file contains invalid JSON.
		:rtype: None
		"""
		if cls._data is None:
			with cls._lock:
				if cls._data is None:  # Double-checked locking
					config_path = Path(__file__).parent.parent / "config.json"
					if not config_path.exists():
						raise FileNotFoundError(f"Config file not found: {config_path}")
					with config_path.open("r", encoding="utf-8") as f:
						cls._data = json.load(f)

	@classmethod
	def Get(cls, key: str, default: Any = None) -> Any:
		"""
		Retrieve a configuration value.

		:param key: The configuration key to retrieve.
		:param default: The value to return if the key is not found.
		:returns: The configuration value, or the default if the key does not exist.
		"""
		cls._initialize()
		return cls._data.get(key, default)

	@classmethod
	def __getitem__(cls, key: str) -> Any:
		"""
		Dict-like access to configuration values.

		:param key: The configuration key to retrieve.
		:returns: The configuration value.
		:raises KeyError: If the key does not exist in the config.
		"""
		cls._initialize()
		return cls._data[key]

	@classmethod
	def All(cls) -> Dict[str, Any]:
		"""
		Return a copy of the entire configuration dictionary.

		:returns: A copy of all configuration key-value pairs.
		"""
		cls._initialize()
		return cls._data.copy()