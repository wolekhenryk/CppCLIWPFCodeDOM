#include "DataModel.h"
#include <msclr/marshal_cppstd.h>
using namespace System::IO;
using namespace System::Reflection;

DataModel::DataModel(String^ codeText)
{
	_codeText = codeText;
}

DataModel::NavigateToAddCodeCommand::NavigateToAddCodeCommand(DataModel^ viewModel)
{
    _viewModel = viewModel;
}

bool DataModel::NavigateToAddCodeCommand::CanExecute(System::Object^ parameter) 
{
    return true;
}

void DataModel::NavigateToAddCodeCommand::Execute(System::Object^)
{
    _viewModel->ErrorsList->Clear();

    // Zakładam, że SelectedOutputKind to Microsoft::CodeAnalysis::OutputKind (enum)
    auto result = Lab5CL::VbCompilerService::Compile(
        _viewModel->CodeText,
        "DynamicVB",
        _viewModel->SelectedOutputKind
    );

    if (!result->Success)
    {
        for each (String ^ d in result->Diagnostics)
            _viewModel->ErrorsList->Add(d);
        return;
    }

    auto asmBytes = result->AssemblyImage;
    if (asmBytes == nullptr || asmBytes->Length == 0)
    {
        _viewModel->ErrorsList->Add("Brak artefaktu PE.");
        return;
    }

    auto ms = gcnew MemoryStream(asmBytes);
    System::Reflection::Assembly^ x = Assembly::Load(ms->ToArray());

    auto types = x->GetExportedTypes();
    if (types->Length == 0)
    {
        _viewModel->ErrorsList->Add("Brak publicznych typów w zbudowanym assembly.");
        return;
    }

    _viewModel->Type = types[0];
    _viewModel->ObjectInstance = Activator::CreateInstance(_viewModel->Type);
    _viewModel->Methods = _viewModel->Type->GetMethods(
        BindingFlags::Public | BindingFlags::Instance | BindingFlags::DeclaredOnly);
    _viewModel->Fields = _viewModel->Type->GetFields(
        BindingFlags::Public | BindingFlags::Instance | BindingFlags::DeclaredOnly);

    _viewModel->ErrorsList->Add("OK: skompilowano, załadowano i zreflektowano.");
}


