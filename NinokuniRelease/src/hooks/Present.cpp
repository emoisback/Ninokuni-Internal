#include <pch.h>
#include <base.h>
#include "SDK.h"
#include "Offsets.h"

HRESULT PRESENT_CALL Base::Hooks::Present(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags)
{
	if (!Data::InitImGui)
	{
		Data::pSwapChain = thisptr;
		if (SUCCEEDED(Data::pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Data::pDxDevice11)))
		{
			Data::pDxDevice11->GetImmediateContext(&Data::pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			Data::pSwapChain->GetDesc(&sd);
			Data::hWindow = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			Data::pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			Data::pDxDevice11->CreateRenderTargetView(pBackBuffer, NULL, &Data::pMainRenderTargetView);
			pBackBuffer->Release();
			Data::oWndProc = (WNDPROC)SetWindowLongPtr(Data::hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
			ImGui_ImplWin32_Init(Data::hWindow);
			ImGui_ImplDX11_Init(Data::pDxDevice11, Data::pContext);
			Data::InitImGui = true;
		}
	}

	if (!Data::InitImGui) return Data::oPresent(thisptr, SyncInterval, Flags);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (Data::ShowMenu)
	{
		ImGui::Begin("NiNoKuni Internal | emoisback UC");
		ImGui::Checkbox("Enabled Attack Speed", &ValueData::b_AtkSpd);
		if (ValueData::b_AtkSpd)
		{
			ImGui::SliderInt("(x1000)", &ValueData::AtkSpd, 10, 30);
		}
		ImGui::Checkbox("No Animation", &ValueData::b_NoAnimation);
		ImGui::SliderFloat("SpeedHack", &ValueData::slomo, 1.0f, 10, "%.1f");
		ImGui::SliderInt("MoveSpeed Value", &ValueData::movespeed, 0, 700);

		if (!ValueData::b_console)
		{
			ImGui::Checkbox("Enabled Console", &ValueData::b_console);
		}
		else
		{
			ImGui::Text("Console Enabled.");
		}
		//ImGui::Text(ReadKey("Config", "slomo", "config.ini").c_str());

		ImGui::SliderInt("Start Attack Anim (1-4)", &ValueData::stateAtk, 1, 4);
		ImGui::SliderInt("Jump Hack", &ValueData::jumpCount, 1, 10);
		ImGui::Checkbox("Character Stuck", &ValueData::b_Stuck);
		ImGui::Checkbox("No Clip", &ValueData::b_noclip);
		ImGui::Checkbox("Invisible", &ValueData::b_invis);
		ImGui::Checkbox("Auto Reconnect", &ValueData::b_reconnect);
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (ImGui::Button("Unload"))
		{
			ImGui::End();
			ImGui::EndFrame();
			ImGui::Render();
			Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			Base::Detach();
			return Data::oPresent(thisptr, SyncInterval, Flags);
		}
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();

	Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//HACK
	UWorld* World = *(UWorld**)(Offsets::Base + Offsets::UWorld_Offset); // GWORLD
	UEngine* Engine = *(UEngine**)(Offsets::Base + Offsets::GEngine_Offset);// GEngine
	UGameInstance* GameInstance = World->m_owningGameInstance;
	ULocalPlayer* LocalPlayer = GameInstance->m_LocalPlayer.Data[0];
	APlayerController* PlayerController = LocalPlayer->m_pPlayerController;
	AActor* Actor =PlayerController->pActor;
	

	//Auto Reconnect
	uint64_t checkStamp = 0;    //Wait for UxClock init, static class.... won't move after allocated.
	uintptr_t ptrToClock = (Offsets::Base + 0x62A5D40); while (!ptrToClock) Sleep(10);
	UxClock* pClock = *reinterpret_cast<UxClock**>(ptrToClock);
	checkStamp = pClock->currentStamp;

	//This is how the game sets the timestamp...
	__timeb64 rTime;
	_ftime64_s(&rTime);
	uint64_t currentStamp = rTime.millitm + 1000 * rTime.time;

	//Atk-Speed
	if (Actor && ValueData::b_AtkSpd)
	{
		Actor->InstantKill = (ValueData::AtkSpd * 1000);
		Patch((BYTE*)(Offsets::Base + 0xB49464), (BYTE*)"\x00", 1);
	}

	//Time Delation
	if (Actor)
	{
		Actor->CustomTimeDilation = ValueData::slomo;
	}

	//Move-Speed
	if (Actor)
	{
		UCharacterMovementComponent* CharMovement = Actor->CharacterMovement;
		CharMovement->MaxWalkSpeed = ValueData::movespeed;
		CharMovement->MaxCustomMovementSpeed = ValueData::movespeed + 100.f;
	}

	//Skip Attack
	if (Actor)
	{
		Nop((BYTE*)(Offsets::Base + 0xBF3A88), 6);
		Patch((BYTE*)(Offsets::Base + 0xBF3A74), (BYTE*)"\xC7\x81", 2);
		DWORD_PTR* addr = (DWORD_PTR*)(Offsets::Base + 0xBF3A7A);
		WPM<int>(addr, (ValueData::stateAtk - 1));
		Nop((BYTE*)(Offsets::Base + 0xBF3A7E), 3);
	}

	//Auto Reconnect
	if (Actor && ValueData::b_reconnect && (pClock->currentStamp - checkStamp) > 30000)
	{
		ReconnectSocket(Offsets::Base);
		checkStamp = pClock->currentStamp;
	}

	//Character Stuck
	if (Actor && ValueData::b_Stuck)
	{
		Actor->Role = 0;
	}
	if (Actor && ValueData::b_Stuck == false)
	{
		Actor->Role = 3;
	}

	//MultiJump
	if (Actor)
	{
		Actor->JumpMaxCount = ValueData::jumpCount;
	}

	if (Actor)
	{
		static bool bResetNoClip = false;
		//if (pCharacter)
		{
			if (ValueData::b_noclip)
			{
				if (!bResetNoClip) // we need to set the value only once per toggle!
				{
					PlayerController->Character->bActorEnableCollision &= ~4; // Actor.bActorEnableCollision.Bit = 2
					Actor->CharacterMovement->MovementMode = 5;
					bResetNoClip = true;
				}
			}
			else if (bResetNoClip)
			{
				PlayerController->Character->bActorEnableCollision |= 4; // Actor.bActorEnableCollision.Bit = 2
				Actor->CharacterMovement->MovementMode = 1;
				bResetNoClip = false;
			}
		}
	}

	//No Animation
	if (Actor && ValueData::b_NoAnimation)
	{
		Actor->AnimationState = 0;
	}

	//Console Enable
	if (ValueData::b_console && ValueData::b_hasEnableConsole == false)
	{
		uintptr_t SCOI_Address = AoBScan::FindPattern("\x4C\x89\x44\x24\x18\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4",
			"xxxxxxxxxxxxxxxxxxxxx????xxx????xxx????xxx");
		StaticConstructObject_Internal = (f_StaticConstructObject_Internal)(SCOI_Address);

		UConsole* Console = reinterpret_cast<UConsole*>(StaticConstructObject_Internal(
			Engine->ConsoleClass,
			reinterpret_cast<UObject*>(Engine->GameViewportClient),
			nullptr,
			EObjectFlags::RF_NoFlags,
			EInternalObjectFlags::None,
			nullptr,
			false,
			nullptr,
			false
		));

		Engine->GameViewportClient->ViewportConsole = Console;
		ValueData::b_hasEnableConsole = true;
	}

	if (Actor && ValueData::b_invis)
	{
		SetActorHiddenInGame(Actor, ValueData::b_invis);
		ValueData::b_isinvis = true;
	}

	if (Actor && ValueData::b_invis == false && ValueData::b_isinvis)
	{
		SetActorHiddenInGame(Actor, ValueData::b_invis);
		ValueData::b_isinvis = false;
	}

	return Data::oPresent(thisptr, SyncInterval, Flags);
}