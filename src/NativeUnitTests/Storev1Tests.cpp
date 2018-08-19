#include "stdafx.h"
#include "CppUnitTest.h"
#include "../PasswordFilter/passwordevaluator.h"
#include "../PasswordFilter/registry.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NativeUnitTests
{
	TEST_CLASS(Storev1Tests)
	{
		registry reg;

	public:
		TEST_METHOD(TestBannedPasswordOnSetRaw)
		{
			SetValue(L"ValidateRawPasswordOnSet", 1);

			LPWSTR password = L"password";
			Assert::IsFalse(ProcessPasswordRaw(password, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}

		TEST_METHOD(TestBannedPasswordOnSetNormalized)
		{
			SetValue(L"ValidateNormalizedPasswordOnSet", 1);
			LPWSTR password = L"!!$P@s sw_o+rd$#%^$";
			Assert::IsFalse(ProcessPasswordNormalized(password, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}

		TEST_METHOD(TestGoodPasswordOnSetRaw)
		{
			SetValue(L"ValidateRawPasswordOnSet", 1);
			
			GUID gidReference;
			HRESULT hCreateGuid = CoCreateGuid(&gidReference);
			WCHAR* wszUuid = NULL;
			UuidToStringW(&gidReference, (RPC_WSTR*)&wszUuid);

			Assert::IsTrue(ProcessPasswordRaw(wszUuid, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}

		TEST_METHOD(TestGoodPasswordOnSetNormalized)
		{
			SetValue(L"ValidateNormalizedPasswordOnSet", 1);
			GUID gidReference;
			HRESULT hCreateGuid = CoCreateGuid(&gidReference);
			WCHAR* wszUuid = NULL;
			UuidToStringW(&gidReference, (RPC_WSTR*)&wszUuid);

			Assert::IsTrue(ProcessPasswordNormalized(wszUuid, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}
		
		TEST_METHOD(TestBannedPasswordOnChangeRaw)
		{
			SetValue(L"ValidateRawPasswordOnChange", 1);

			LPWSTR password = L"password";
			Assert::IsFalse(ProcessPasswordRaw(password, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}

		TEST_METHOD(TestBannedPasswordOnChangeNormalized)
		{
			SetValue(L"ValidateNormalizedPasswordOnChange", 1);
			LPWSTR password = L"!!$P@s sw_o+rd$#%^$";
			Assert::IsFalse(ProcessPasswordNormalized(password, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}

		TEST_METHOD(TestGoodPasswordOnChangeRaw)
		{
			SetValue(L"ValidateRawPasswordOnChange", 1);

			GUID gidReference;
			HRESULT hCreateGuid = CoCreateGuid(&gidReference);
			WCHAR* wszUuid = NULL;
			UuidToStringW(&gidReference, (RPC_WSTR*)&wszUuid);

			Assert::IsTrue(ProcessPasswordRaw(wszUuid, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}

		TEST_METHOD(TestGoodPasswordOnChangeNormalized)
		{
			SetValue(L"ValidateNormalizedPasswordOnChange", 1);
			GUID gidReference;
			HRESULT hCreateGuid = CoCreateGuid(&gidReference);
			WCHAR* wszUuid = NULL;
			UuidToStringW(&gidReference, (RPC_WSTR*)&wszUuid);

			Assert::IsTrue(ProcessPasswordNormalized(wszUuid, std::wstring(L"accountName"), std::wstring(L"full name"), TRUE, reg));
		}

		TEST_CLASS_INITIALIZE(Initialize)
		{
			SetValue(L"HashCheckMode", 1);
		}
	};
}