#include <windows.h>
using namespace System;

using namespace System::Reflection;
using namespace System::Security;
using namespace System::Runtime::Remoting;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::ComponentModel;
using namespace System::Windows::Input;
using namespace Microsoft::CodeAnalysis;

typedef ObservableCollection<MethodInfo^> myList;

ref class DataModel : INotifyPropertyChanged {

#pragma region Private Fields

	String^ _codeText;
	ICollection<String^>^ _errorsList = gcnew ObservableCollection<String^>();
	OutputKind selectedOutputKind = OutputKind::DynamicallyLinkedLibrary;
	array<MethodInfo^>^ methods = nullptr;
	array<FieldInfo^>^ fields = nullptr;
	Object^ objectInstance;
	System::Type^ type;
	ObjectHandle^ handle;
	ICommand^ _AddCodeCommand;
	ICommand^ _InvokeMethodCommand;
	MethodInfo^ _selectedMethod = nullptr;
	FieldInfo^ _selectedField = nullptr;
	String^ _methodParameter = "";
	String^ _methodResult = "";

#pragma endregion End of Private Fields

public:

#pragma region Public Constructors

	DataModel(String^ codeText);

#pragma endregion End of Public Constructors

#pragma region Public Events

	virtual event System::ComponentModel::PropertyChangedEventHandler^ PropertyChanged;

#pragma endregion End of Public Events

#pragma region Public Methods

	void OnPropertyChanged(String^ name) {
		PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(name));
	}

#pragma endregion End of Public Methods

#pragma region Public Properties

	property String^ CodeText {
		String^ get() {
			return _codeText;
		}

		void set(String^ value) {
			_codeText = value;
			OnPropertyChanged("CodeText");
		}
	}

	property ICollection<String^>^ ErrorsList {
		ICollection<String^>^ get() {
			return _errorsList;
		}

		void set(ICollection<String^>^ value) {
			_errorsList = value;
			OnPropertyChanged("ErrorsList");
		}
	}

	property ICommand^ AddCodeCommand {
		ICommand^ get() {
			if (_AddCodeCommand == nullptr)
			{
				_AddCodeCommand = gcnew NavigateToAddCodeCommand(this);
			}
			return _AddCodeCommand;
		}
		void set(ICommand^ value) {
			_AddCodeCommand = value;
		}
	}

	property OutputKind SelectedOutputKind {
		OutputKind get() {
			return selectedOutputKind;
		}

		void set(OutputKind value) {
			selectedOutputKind = value;
			OnPropertyChanged("SelectedOutputKind");
		}
	}

	property array<MethodInfo^>^ Methods {
		array<MethodInfo^>^ get() {
			return methods;
		}

		void set(array<MethodInfo^>^ value) {
			methods = value;
			OnPropertyChanged("Methods");
		}
	}

	property array<FieldInfo^>^ Fields {
		array<FieldInfo^>^ get() {
			return fields;
		}

		void set(array<FieldInfo^>^ value) {
			fields = value;
			OnPropertyChanged("Fields");
		}
	}

	property Object^ ObjectInstance {
		Object^ get() {
			return objectInstance;
		}

		void set(Object^ value) {
			objectInstance = value;
			OnPropertyChanged("ObjectInstance");
			if (_InvokeMethodCommand != nullptr) {
				((InvokeMethodCommandImpl^)_InvokeMethodCommand)->RaiseCanExecuteChanged();
			}
		}
	}

	property ObjectHandle^ Handle {
		ObjectHandle^ get() {
			return handle;
		}

		void set(ObjectHandle^ value) {
			handle = value;
			OnPropertyChanged("Handle");
		}
	}

	property System::Type^ Type {
		System::Type^ get() {
			return type;
		}

		void set(System::Type^ value) {
			type = value;
			OnPropertyChanged("Type");
		}
	}

	property MethodInfo^ SelectedMethod {
		MethodInfo^ get() {
			return _selectedMethod;
		}

		void set(MethodInfo^ value) {
			_selectedMethod = value;
			OnPropertyChanged("SelectedMethod");
			if (_InvokeMethodCommand != nullptr) {
				((InvokeMethodCommandImpl^)_InvokeMethodCommand)->RaiseCanExecuteChanged();
			}
		}
	}

	property FieldInfo^ SelectedField {
		FieldInfo^ get() {
			return _selectedField;
		}

		void set(FieldInfo^ value) {
			_selectedField = value;
			OnPropertyChanged("SelectedField");
		}
	}

	property String^ MethodParameter {
		String^ get() {
			return _methodParameter;
		}

		void set(String^ value) {
			_methodParameter = value;
			OnPropertyChanged("MethodParameter");
		}
	}

	property String^ MethodResult {
		String^ get() {
			return _methodResult;
		}

		void set(String^ value) {
			_methodResult = value;
			OnPropertyChanged("MethodResult");
		}
	}

	property ICommand^ InvokeMethodCommand {
		ICommand^ get() {
			if (_InvokeMethodCommand == nullptr)
			{
				_InvokeMethodCommand = gcnew InvokeMethodCommandImpl(this);
			}
			return _InvokeMethodCommand;
		}
		void set(ICommand^ value) {
			_InvokeMethodCommand = value;
		}
	}

#pragma endregion End of Public Properties

#pragma region Public Nested Classes

	ref class NavigateToAddCodeCommand : public ICommand {

		DataModel^ _viewModel;
		
		property DataModel^ ViewModel {
			DataModel^ get() {
				return _viewModel;
			}
		
			void set(DataModel^ value) {
				_viewModel = value;
			}
		}
		virtual bool CanExecute(System::Object^ parameter) = ICommand::CanExecute;
		virtual void Execute(System::Object^ parameter) = ICommand::Execute;
	public:

		NavigateToAddCodeCommand(DataModel^ viewModel);
		virtual event EventHandler^ CanExecuteChanged {
			void add(EventHandler^) {}
			void remove(EventHandler^) {}
		}
	};

	ref class InvokeMethodCommandImpl : public ICommand {

		DataModel^ _viewModel;
		EventHandler^ _canExecuteChanged;

		property DataModel^ ViewModel {
			DataModel^ get() {
				return _viewModel;
			}

			void set(DataModel^ value) {
				_viewModel = value;
			}
		}
		virtual bool CanExecute(System::Object^ parameter) = ICommand::CanExecute;
		virtual void Execute(System::Object^ parameter) = ICommand::Execute;
	public:

		InvokeMethodCommandImpl(DataModel^ viewModel);
		virtual event EventHandler^ CanExecuteChanged {
			void add(EventHandler^ handler) {
				_canExecuteChanged += handler;
			}
			void remove(EventHandler^ handler) {
				_canExecuteChanged -= handler;
			}
		}
		void RaiseCanExecuteChanged() {
			if (_canExecuteChanged != nullptr) {
				_canExecuteChanged(this, EventArgs::Empty);
			}
		}
	};

#pragma endregion End of Public Nested Classes

};

