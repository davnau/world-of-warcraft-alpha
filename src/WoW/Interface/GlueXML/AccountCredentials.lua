ACCOUNT_CREATE_URL = "http://127.0.0.1:8080/account/create";

function AccountCredentials_Show()
	AccountLoginCredentials:Show();
	local accountName = GetLastAccountName();
	if ( accountName and strlen(accountName) > 0 ) then
		AccountLoginAccountEdit:SetText(accountName);
	else
		AccountLoginAccountEdit:SetText("");
	end
	AccountLoginPasswordEdit:SetText("");
	AccountLoginAccountEdit:SetFocus();
end

function AccountCredentials_Hide()
	AccountLoginCredentials:Hide();
end

function AccountLogin_Login()
	if ( not AccountLoginCredentials:IsVisible() ) then
		PlaySound("gsLogin");
		AccountCredentials_Show();
		return;
	end

	local accountName = AccountLoginAccountEdit:GetText();
	local password = AccountLoginPasswordEdit:GetText();
	if ( not accountName or strlen(accountName) == 0 ) then
		AccountLoginAccountEdit:SetFocus();
		return;
	end
	if ( not password or strlen(password) == 0 ) then
		AccountLoginPasswordEdit:SetFocus();
		return;
	end

	PlaySound("gsLogin");
	DefaultServerLogin(accountName, password);
end

function AccountCredentials_AccountEnter()
	AccountLoginPasswordEdit:SetFocus();
end

function AccountCredentials_AccountTab()
	AccountLoginPasswordEdit:SetFocus();
end

function AccountCredentials_PasswordTab()
	AccountLoginAccountEdit:SetFocus();
end
