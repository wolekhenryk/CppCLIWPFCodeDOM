#include <windows.h>
#include "DataModel.h"
using namespace System;
using namespace System::Windows;
using namespace System::IO;
using namespace System::Windows::Markup;
using namespace System::Windows::Controls;
using namespace System::Reflection;
using namespace System::Windows::Documents;
using namespace Microsoft::CodeAnalysis;
using namespace Microsoft::CodeAnalysis::VisualBasic;

using namespace std;

public ref class MyApplication : public Application
{
    DataModel^ h_dm;
    String^ pathToVBCode, ^ code;

    public: MyApplication(Window^ win)
    {
        pathToVBCode = gcnew String("..\\..\\..\\..\\VBCode\\Class1.vb");
        code = File::ReadAllText(pathToVBCode);
        h_dm = gcnew DataModel(code);
        win->DataContext = h_dm;

    }
};

static public ref class Start
{
public:
    static void WinMain()
    {
        Stream^ st = File::OpenRead("MainWindow.xaml");
        Window^ win = (Window^)XamlReader::Load(st, nullptr);
        Application^ app = gcnew MyApplication(win);
        app->Run(win);
    }
};
