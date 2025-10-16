import logging
from threading import Lock
from typing import Optional

from .config import Config


class Logger:
	"""
	Thread-safe singleton wrapper for Python's logging system.

	This class ensures the application uses a single, consistently
	configured logger. The configuration is loaded from ``config.json``
	under the ``LOG`` key.

	Configuration keys under ``LOG``:
		- ``level`` (str): Logging level (e.g., ``"DEBUG"``, ``"INFO"``). Default: ``"INFO"``.
	"""

	_instance: Optional[logging.Logger] = None
	"""logging instance to log to"""
	_lock: Lock = Lock()
	"""lock for initialization"""

	def __new__(cls, *args, **kwargs):
		"""
		Prevent direct instantiation.

		:raises RuntimeError: Always, since this class is meant to be used statically.
		"""
		raise RuntimeError("Use Logger.Get_Logger() instead of instantiating.")

	@classmethod
	def _Initialize(cls) -> None:
		"""
		Initialize the logger configuration (thread-safe).

		Reads log settings from configuration and configures the root logger.
		Uses double-checked locking to ensure it is initialized only once.

		"""
		if cls._instance is None:
			with cls._lock:
				if cls._instance is None:
					try:
						log_config = Config.Get("LOG", {})
						log_level_str = str(log_config.get("level", "INFO")).upper()
						log_level = getattr(logging, log_level_str, logging.INFO)
					except Exception:
						log_level = logging.INFO

					logging.basicConfig(
						level=log_level,
						format="%(asctime)s [%(levelname)s]: %(message)s",
						datefmt="%H:%M:%S",
					)

					cls._instance = logging.getLogger("AppLogger")
					cls._instance.setLevel(log_level)

	@classmethod
	def Get_Logger(cls) -> logging.Logger:
		"""
		Get the singleton logger instance.

		:returns: The logger instance configured for the application.
		"""
		cls._Initialize()
		return cls._instance

	@classmethod
	def Info(cls, message: str) -> None:
		"""
		Log an Informational message.

		:param message: The message to log.
		"""
		cls.Get_Logger().info(message)

	@classmethod
	def Error(cls, message: str) -> None:
		"""
		Log an error message.

		:param message: The message to log.
		"""
		cls.Get_Logger().error(message)

	@classmethod
	def Warning(cls, message: str) -> None:
		"""
		Log a warning message.

		:param message: The message to log.
		"""
		cls.Get_Logger().warning(message)

	@classmethod
	def Exception(cls, message: str, exc: Exception = None) -> None:
		"""
		Log an exception and raise it (or wrap it if none is provided).

		:param message: The error message to log.
		:param exc: The exception to raise. If None, raises RuntimeError with the message.
		:raises Exception: The given exception or a RuntimeError if not provided.
		"""
		cls.Get_Logger().exception(message)
		if exc is not None:
			raise exc
		raise RuntimeError(message)