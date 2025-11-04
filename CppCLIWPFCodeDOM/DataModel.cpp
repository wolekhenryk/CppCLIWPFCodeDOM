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

DataModel::InvokeMethodCommandImpl::InvokeMethodCommandImpl(DataModel^ viewModel)
{
    _viewModel = viewModel;
}

bool DataModel::InvokeMethodCommandImpl::CanExecute(System::Object^ parameter)
{
    return _viewModel->SelectedMethod != nullptr && _viewModel->ObjectInstance != nullptr;
}

void DataModel::InvokeMethodCommandImpl::Execute(System::Object^)
{
    if (_viewModel->SelectedMethod == nullptr)
    {
        _viewModel->MethodResult = "No method selected";
        return;
    }

    try
    {
        array<System::Object^>^ params = nullptr;

        // Check if method has parameters
        array<ParameterInfo^>^ methodParams = _viewModel->SelectedMethod->GetParameters();

        if (methodParams->Length > 0)
        {
            // For simplicity, we'll handle methods with one parameter
            if (methodParams->Length == 1)
            {
                params = gcnew array<System::Object^>(1);

                // Try to convert the parameter string to the appropriate type
                System::Type^ paramType = methodParams[0]->ParameterType;

                if (paramType == String::typeid)
                {
                    params[0] = _viewModel->MethodParameter;
                }
                else if (paramType == int::typeid || paramType == Int32::typeid)
                {
                    int value;
                    if (Int32::TryParse(_viewModel->MethodParameter, value))
                        params[0] = value;
                    else
                    {
                        _viewModel->MethodResult = "Invalid integer parameter";
                        return;
                    }
                }
                else if (paramType == double::typeid || paramType == Double::typeid)
                {
                    double value;
                    if (Double::TryParse(_viewModel->MethodParameter, value))
                        params[0] = value;
                    else
                    {
                        _viewModel->MethodResult = "Invalid double parameter";
                        return;
                    }
                }
                else
                {
                    _viewModel->MethodResult = "Unsupported parameter type: " + paramType->Name;
                    return;
                }
            }
            else
            {
                _viewModel->MethodResult = "Methods with multiple parameters not supported";
                return;
            }
        }

        // Invoke the method
        System::Object^ result = _viewModel->SelectedMethod->Invoke(_viewModel->ObjectInstance, params);

        // Display the result
        if (result != nullptr)
            _viewModel->MethodResult = result->ToString();
        else
            _viewModel->MethodResult = "(void)";
    }
    catch (Exception^ ex)
    {
        _viewModel->MethodResult = "Error: " + ex->Message;
    }
}


