
// For some freaking reason neither of Q_OS_WIN32 or Q_WS_WIN worked here
// for me so I had to introduce an extra "__SPECIAL_WINDOWS_DEFINE__" 
// that is set in CMakeLists.txt for Windows only.
// We don't need those defines for *nix systems so it's just empty for them.

//#ifdef Q_WS_WIN
//#ifdef Q_OS_WIN32

#ifdef __SPECIAL_WINDOWS_DEFINE__

	#ifdef juffed_engine_qsci_EXPORTS
		#define QSCI_ENGINE_EXPORT Q_DECL_EXPORT
	#else
		#define QSCI_ENGINE_EXPORT Q_DECL_IMPORT
	#endif

#else

	#define QSCI_ENGINE_EXPORT

#endif
