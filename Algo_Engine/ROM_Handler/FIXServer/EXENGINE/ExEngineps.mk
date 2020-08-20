
ExEngineps.dll: dlldata.obj ExEngine_p.obj ExEngine_i.obj
	link /dll /out:ExEngineps.dll /def:ExEngineps.def /entry:DllMain dlldata.obj ExEngine_p.obj ExEngine_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del ExEngineps.dll
	@del ExEngineps.lib
	@del ExEngineps.exp
	@del dlldata.obj
	@del ExEngine_p.obj
	@del ExEngine_i.obj
