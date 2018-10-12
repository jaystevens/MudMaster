
Ifxps.dll: dlldata.obj Ifx_p.obj Ifx_i.obj
	link /dll /out:Ifxps.dll /def:Ifxps.def /entry:DllMain dlldata.obj Ifx_p.obj Ifx_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0500 /DREGISTER_PROXY_DLL \
		$<
# _WIN32_WINNT=0x0500 is for Win2000, change it to 0x0400 for NT4 or Win95 with DCOM

clean:
	@del Ifxps.dll
	@del Ifxps.lib
	@del Ifxps.exp
	@del dlldata.obj
	@del Ifx_p.obj
	@del Ifx_i.obj
