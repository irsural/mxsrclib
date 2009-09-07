//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("boundtst.res");
USEFORM("main.cpp", MainForm);
USEUNIT("mainprg.cpp");
USEUNIT("config.cpp");
USEUNIT("..\mxsrclib\irsstdg.cpp");
USEUNIT("..\mxsrclib\irsstd.cpp");
USEUNIT("..\mxsrclib\arch\cur\irsstdl.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TMainForm), &MainForm);
     Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
