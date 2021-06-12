#ifndef __SCRIPT_MANAGER_H__
#define __SCRIPT_MANAGER_H__

#include "ScriptEngine.h"
#include "NativeInvoker.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class ScriptManagerContext : public NativeContext {
public:

	ScriptManagerContext()
		: NativeContext() {
	}

	void Reset() {

		m_nArgCount = 0;
		m_nDataCount = 0;
	}

	void* GetResultPointer() {

		return m_pReturn;
	}
};

#undef Yield

class Script {
public:

	Script( void( *function )( ), const std::string& scriptName ) {
		name = scriptName;
		scriptFiber = nullptr;
		callbackFunction = function;
		wakedAt = timeGetTime();
	}

	~Script() {

		if ( scriptFiber ) {
			DeleteFiber( scriptFiber );
		}
	}

	void Tick();

	void Yield( uint32_t time );

	void( *GetCallbackFunction() )( ) {

		return callbackFunction;
	}

	std::string     name;

	static void			Start();

private:

	HANDLE			scriptFiber;
	uint32_t		wakedAt;
	void( *callbackFunction )( );

	void			Run();
};

typedef std::map<HMODULE,std::vector<std::shared_ptr<Script>>> scriptMap;

class ScriptManagerThread : public ScriptThread {

	scriptMap					m_scripts;
	std::vector<std::string>	m_scriptNames;

public:

	void			DoRun() override;
	eThreadState	Reset( uint32_t scriptHash, void * pArgs, uint32_t argCount ) override;
	size_t					LoadScripts();
	void					FreeScripts();
	size_t					Count();
	void					AddScript( HMODULE module, void( *fn )( ) );
	void					RemoveScript( void( *fn )( ) );
	void					RemoveScript( HMODULE module );
};

namespace ScriptManager {

	void					HandleKeyEvent(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);
}

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X",
				static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}

extern ScriptManagerThread	g_ScriptManagerThread;

#endif // __SCRIPT_MANAGER_H__
