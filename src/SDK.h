#pragma once

class UEngine;
class UWorld;
class UGameInstance;
class ULocalPlayer;
class APlayerController;
class AActor;
class ACharacter;
class UxClock;
class UClass;
class UGameViewportClient;
class UConsole;
class ULevel;
class UInputComponent;
class USceneComponent;
class AController;
class APlayerState;
class USkeletalMeshComponent;
class UCharacterMovementComponent;
class UCapsuleComponent;

class AoBScan {
private:
	static __forceinline bool MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
	{
		for (PBYTE value = reinterpret_cast<PBYTE>(pBuffer); *lpMask; ++lpPattern, ++lpMask, ++value)
		{
			if (*lpMask == 'x' && *reinterpret_cast<LPCBYTE>(lpPattern) != *value)
				return false;
		}
		return true;
	}
public:
	static __forceinline uintptr_t FindPattern(PVOID pBase, DWORD dwSize, LPCSTR lpPattern, LPCSTR lpMask)
	{
		dwSize -= static_cast<DWORD>(strlen(lpMask));
		for (unsigned long index = 0; index < dwSize; ++index)
		{
			PBYTE pAddress = reinterpret_cast<PBYTE>(pBase) + index;
			if (MaskCompare(pAddress, lpPattern, lpMask))
				return reinterpret_cast<uintptr_t>(pAddress);
		}
		return NULL;
	}

	static __forceinline uintptr_t FindPattern(LPCSTR lpPattern, LPCSTR lpMask)
	{
		MODULEINFO info = { 0 };
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));
		return FindPattern(info.lpBaseOfDll, info.SizeOfImage, lpPattern, lpMask);
	}
};


template<typename T> T RPM(uintptr_t address) {
	try { return *(T*)address; }
	catch (...) { return T(); }
}

template<typename T> void WPM(DWORD_PTR* address, T value) {
	try {
		DWORD oldprotect;
		VirtualProtect(address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldprotect);
		*(T*)address = value;
		VirtualProtect(address, sizeof(T), oldprotect, &oldprotect);
	}
	catch (...) { return; }
}

void Nop(BYTE* dst, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);

	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void ReconnectSocket(uintptr_t Base)
{
	typedef int(__stdcall* ReconnectSocket_)();
	ReconnectSocket_ ReconnectSocketFn = nullptr;
	ReconnectSocketFn = (ReconnectSocket_)(Base + 0x01351B70);
	ReconnectSocketFn();
}

void SetActorHiddenInGame(AActor* ent, bool bNewHidden)
{
	if ((*(DWORD64*)ent + 0x2F8))
	{
		(*(int(__fastcall**)(AActor*, bool))(*(DWORD64*)ent + 0x2F8))(ent, bNewHidden);
	}
}

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}
	T* Data;
	int32_t Count;
	int32_t Max;
};

struct Vector3 {
	float X;
	float Y;
	float Z;
};

struct UEngine
{
	char unknown1[0xF8];
	UClass* ConsoleClass;
	char unknown2[0x688];
	UGameViewportClient* GameViewportClient;
};

class UWorld
{
public:
	char pad_0x0000[0x30]; //0x0000
	ULevel* m_persistentLevel; //0x0030 
	char pad_0x0038[0x150]; //0x0038
	UGameInstance* m_owningGameInstance; //0x0188 
};

class UGameInstance
{
public:
	char pad_0x0000[0x38]; //0x0000
	TArray<ULocalPlayer*> m_LocalPlayer;
};

class ULocalPlayer
{
public:
	char pad_0x0000[0x30]; //0x0000
	APlayerController* m_pPlayerController; //0x0030 
	char pad_0x0038[0x50]; //0x0038
	Vector3 Location; //0x0088 
};

class APlayerController
{
public:
	char pad_0x0000[0x260]; //0x0000
	ACharacter* Character; //0x260
	char pad_0x0268[0x38]; //268
	AActor* pActor; //0x02A0 
};

class AActor
{
public:
	char pad_0x0000[0x58]; //0x0000
	unsigned char bTearOff; //0x0058 
	unsigned char bCanBeDamaged; //0x0059 
	unsigned char bIsEditorOnlyActor; //0x005A 
	unsigned char bReplicates; //0x005B 
	unsigned char bActorEnableCollision; //0x005C 
	char pad_0x005D[0x37]; //0x005D
	float InitialLifeSpan; //0x0094 
	float CustomTimeDilation; //0x0098 
	char pad_0x009C[0x54]; //0x009C
	unsigned char Role; //0x00F0 
	char pad_0x00F1[0x7]; //0x00F1
	UInputComponent* InputComponent; //0x00F8 
	float NetCullDistanceSquared; //0x0100 
	__int32 NetTag; //0x0104 
	char pad_0x0108[0x28]; //0x0108
	USceneComponent* RootComponent; //0x0130 
	char pad_0x0138[0xF0]; //0x0138
	unsigned char bUseControllerRotation; //0x0228 
	char pad_0x0229[0x3]; //0x0229
	float BaseEyeHeight; //0x022C 
	unsigned char AutoPossessPlayer; //0x0230 
	unsigned char AutoPossessAI; //0x0231 
	unsigned char RemoteViewPitch; //0x0232 
	char pad_0x0233[0x5]; //0x0233
	AController* AIControllerClass; //0x0238 
	APlayerState* PlayerState; //0x0240 
	char pad_0x0248[0x8]; //0x0248
	AController* LastHitBy; //0x0250 
	AController* Controller; //0x0258 
	char pad_0x0260[0x4]; //0x0260
	Vector3 ControlInputVector; //0x0264 
	Vector3 LastControlInputVector; //0x0270 
	char pad_0x027C[0x4]; //0x027C
	USkeletalMeshComponent* Mesh; //0x0280 
	UCharacterMovementComponent* CharacterMovement; //0x0288 
	UCapsuleComponent* CapsuleComponent; //0x0290 
	float AnimRootMotionTranslationScale; //0x02F8 
	Vector3 BaseTranslationOffset; //0x02FC 
	char pad_0x0308[0x8]; //0x0308
	Vector3 BaseRotationOffset; //0x0310 
	float ReplicatedServerLastTransformUpdateTimeStamp; //0x0320 
	float ReplayLastTransformUpdateTimeStamp; //0x0324 
	unsigned char ReplicatedMovementMode; //0x0328 
	unsigned char bInBaseReplication; //0x0329 
	char pad_0x032A[0x2]; //0x032A
	float CrouchedEyeHeight; //0x032C 
	unsigned char bIsCrouched; //0x0330 
	unsigned char bWasJumping; //0x0331 
	char pad_0x0332[0x66]; //0x0332
	float JumpKeyHoldTime; //0x0334 
	float JumpForceTimeRemaining; //0x0338 
	float ProxyJumpForceStartedTime; //0x033C 
	float JumpMaxHoldTime; //0x0340 
	__int32 JumpMaxCount; //0x0344 
	__int32 JumpCurrentCount; //0x0348 
	char pad_0x034C[0x1C7]; //0x034C
	__int32 AnimationState; // 0x514
	char pad_0x0518[0x330]; //0x034C
	__int32 InstantKill; //0x0804 
	char pad_0x0808[0x1E89]; //0x0808
	unsigned char HitLocationPrecision; //0x2691 
	unsigned char NeedDissolve; //0x2692 
	unsigned char CombatState; //0x2693  
	unsigned char IdleAnimEnabled; //0x2694 
	char pad_0x2695[0x3]; //0x2695
	float DashSpeedFactor; //0x2698 
	float VisualEffectScaleFactor; //0x269C 
	char pad_0x26A0[0xF0]; //0x26A0
	unsigned char UsePhysicalAnimationOnDeath; //0x2790 
	unsigned char UseBlendSpaceToTurn; //0x2791 
	char pad_0x2792[0x6]; //0x2792
	__int32 BaseNonRenderedUpdateRate; //0x2798 
	__int32 MaxEvalRateForInterpolation; //0x279C 
	char pad_0x27A0[0x10]; //0x27A0
	unsigned char IgnoreMinDrawDistance; //0x27B0 
	unsigned char IgnoreMaxDrawDistance; //0x27B1 
};

struct UxClock
{
	uintptr_t vft = 0;
	uint64_t firstTimeStamp = 0;
	uint64_t currentStamp = 0;
};

class ACharacter
{
public:
	char pad_0x0000[0x5c]; //0x0000
	byte bActorEnableCollision; //0x005C 
};

struct UClass;

struct UConsole;

struct UGameViewportClient
{
	char unknown1[0x40];
	UConsole* ViewportConsole;
};

class USceneComponent
{
public:
	char pad_0x0000[0x11C]; //0x0000
	Vector3 RelativeLocation; //0x011C 
	Vector3 RelativeRotation; //0x0128 
	Vector3 RelativeScale3D; //0x0134 
	Vector3 ComponentVelocity; //0x0140 
	char bComponentToWorldUpdated : 1; // 0x14c(0x01)
	char pad_14C_1 : 1; // 0x14c(0x01)
	char bAbsoluteLocation : 1; // 0x14c(0x01)
	char bAbsoluteRotation : 1; // 0x14c(0x01)
	char bAbsoluteScale : 1; // 0x14c(0x01)
	char bVisible : 1; // 0x14c(0x01)
	char bShouldBeAttached : 1; // 0x14c(0x01)
	char bShouldSnapLocationWhenAttached : 1; // 0x14c(0x01)
	char bShouldSnapRotationWhenAttached : 1; // 0x14d(0x01)
	char bShouldUpdatePhysicsVolume : 1; // 0x14d(0x01)
	char bHiddenInGame : 1; // 0x14d(0x01)
	char bBoundsChangeTriggersStreamingDataRebuild : 1; // 0x14d(0x01)
	char bUseAttachParentBound : 1; // 0x14d(0x01)
	char pad_14D_5 : 3; // 0x14d(0x01)
	char pad_0x014E[0x1]; //0x014E
	unsigned char Mobility; //0x014F 
	unsigned char DetailMode; //0x0150 
};

class UCharacterMovementComponent
{
public:
	char pad_0x0000[0x150]; //0x0000
	float GravityScale; //0x0150 
	float MaxStepHeight; //0x0154 
	float JumpZVelocity; //0x0158 
	float JumpOffJumpZFactor; //0x015C 
	float WalkableFloorAngle; //0x0160 
	float WalkableFloorZ; //0x0164 
	unsigned char MovementMode; //0x0168 
	unsigned char CustomMovementMode; //0x0169 
	char pad_0x016A[0x2]; //0x016A
	float GroundFriction; //0x016C 
	char pad_0x0170[0x1C]; //0x0170
	float MaxWalkSpeed; //0x018C 
	float MaxWalkSpeedCrouched; //0x0190 
	float MaxSwimSpeed; //0x0194 
	float MaxFlySpeed; //0x0198 
	float MaxCustomMovementSpeed; //0x019C 
	float MaxAcceleration; //0x01A0 
	float MinAnalogWalkSpeed; //0x01A4 
	float BrakingFrictionFactor; //0x01A8 
	float BrakingFriction; //0x01AC 
	float BrakingSubStepTime; //0x01B0 
	float BrakingDecelerationWalking; //0x01B4 
	float BrakingDecelerationFalling; //0x01B8 
	float BrakingDecelerationSwimming; //0x01BC 
	float BrakingDecelerationFlying; //0x01C0 
	float AirControl; //0x01C4 
	float AirControlBoostMultiplier; //0x01C8 
	float AirControlBoostVelocityThreshold; //0x01CC 
	float FallingLateralFriction; //0x01D0 
	float CrouchedHalfHeight; //0x01D4 
	float Buoyancy; //0x01D8 
	float PerchRadiusThreshold; //0x01DC 
	float PerchAdditionalHeight; //0x01E0 
	Vector3 RotationRate; //0x01E4 
	unsigned char bUseControllerDesiredRotation; //0x01F0 
	unsigned char bForceMaxAccel; //0x01F1 
	unsigned char bTouchForceScaledToMass; //0x01F2 
	unsigned char bScalePushForceToVelocity; //0x01F3 
	char pad_0x01F4[0x4]; //0x01F4
	USceneComponent* DeferredUpdatedMoveComponent; //0x01F8 
	float MaxOutOfWaterStepHeight; //0x0200 
	float OutofWaterZ; //0x0204 
	float Mass; //0x0208 
	float StandingDownwardForceScale; //0x020C 
	float InitialPushForceFactor; //0x0210 
	float PushForceFactor; //0x0214 
	float PushForcePointZOffsetFactor; //0x0218 
	float TouchForceFactor; //0x021C 
	float MinTouchForce; //0x0220 
	float MaxTouchForce; //0x0224 
	float RepulsionForce; //0x0228 
	Vector3 Acceleration; //0x022C 
	char pad_0x0238[0x8]; //0x0238
	Vector3 LastUpdateLocation; //0x0250 
	Vector3 LastUpdateVelocity; //0x025C 
	float ServerLastTransformUpdateTimeStamp; //0x0268 
	float ServerLastClientGoodMoveAckTime; //0x026C 
	float ServerLastClientAdjustmentTime; //0x0270 
	Vector3 PendingImpulseToApply; //0x0274 
	Vector3 PendingForceToApply; //0x0280 
	float AnalogInputModifier; //0x028C 
	char pad_0x0290[0xC]; //0x0290
	float MaxSimulationTimeStep; //0x029C 
	__int32 MaxSimulationIterations; //0x02A0 
	__int32 MaxJumpApexAttemptsPerSimulation; //0x02A4 
	float MaxDepenetrationWithGeometry; //0x02A8 
	float MaxDepenetrationWithGeometryAsProxy; //0x02AC 
	float MaxDepenetrationWithPawn; //0x02B0 
	float MaxDepenetrationWithPawnAsProxy; //0x02B4 
	float NetworkSimulatedSmoothLocationTime; //0x02B8 
	float NetworkSimulatedSmoothRotationTime; //0x02BC 
	float ListenServerNetworkSimulatedSmoothLocationTime; //0x02C0 
	float ListenServerNetworkSimulatedSmoothRotationTime; //0x02C4 
	float NetProxyShrinkRadius; //0x02C8 
	float NetProxyShrinkHalfHeight; //0x02CC 
	float NetworkMaxSmoothUpdateDistance; //0x02D0 
	float NetworkNoSmoothUpdateDistance; //0x02D4 
	float NetworkMinTimeBetweenClientAckGoodMoves; //0x02D8 
	float NetworkMinTimeBetweenClientAdjustments; //0x02DC 
	float NetworkMinTimeBetweenClientAdjustmentsLargeCorrection; //0x02E0 
	float NetworkLargeClientCorrectionDistance; //0x02E4 
	float LedgeCheckThreshold; //0x02E8 
	float JumpOutOfWaterPitch; //0x02EC 
	char pad_0x02F0[0x94]; //0x02F0
	unsigned char DefaultLandMovementMode; //0x0384 
	unsigned char DefaultWaterMovementMode; //0x0385 
	unsigned char GroundMovementMode; //0x0386 
	unsigned char bMaintainHorizontalGroundVelocity; //0x0387 
	unsigned char bCheatFlying; //0x0388 
	unsigned char bAlwaysCheckFloor; //0x0389 
	unsigned char bHasRequestedVelocity; //0x038A 
	char pad_0x038B[0x11]; //0x038B
	float AvoidanceConsiderationRadius; //0x039C 
	Vector3 RequestedVelocity; //0x03A0 
	__int32 AvoidanceUID; //0x03AC 
	char pad_0x03B0[0xC]; //0x03B0
	float AvoidanceWeight; //0x03BC 
	Vector3 PendingLaunchVelocity; //0x03C0 
	char pad_0x03CC[0xA4]; //0x03CC
	float NavMeshProjectionInterval; //0x0470 
	float NavMeshProjectionTimer; //0x0474 
	float NavMeshProjectionInterpSpeed; //0x0478 
	float NavMeshProjectionHeightScaleUp; //0x047C 
	float NavMeshProjectionHeightScaleDown; //0x0480 
	float NavWalkingFloorDistTolerance; //0x0484 
	char pad_0x0488[0x48]; //0x0488
	float MinTimeBetweenTimeStampResets; //0x04D0 
	char pad_0x04D4[0x10C]; //0x04D4
	Vector3 AnimRootMotionVelocity; //0x05E0 
	char pad_0x05EC[0x21C]; //0x05EC
};

class UCapsuleComponent
{
public:
	char pad_0x0000[0x11C]; //0x0000
	Vector3 RelativeLocation; //0x011C
};

struct UObject;

enum EObjectFlags
{
	RF_NoFlags = 0x00000000,
	RF_Public = 0x00000001,
	RF_Standalone = 0x00000002,
	RF_MarkAsNative = 0x00000004,
	RF_Transactional = 0x00000008,
	RF_ClassDefaultObject = 0x00000010,
	RF_ArchetypeObject = 0x00000020,
	RF_Transient = 0x00000040,
	RF_MarkAsRootSet = 0x00000080,
	RF_TagGarbageTemp = 0x00000100,
	RF_NeedInitialization = 0x00000200,
	RF_NeedLoad = 0x00000400,
	RF_KeepForCooker = 0x00000800,
	RF_NeedPostLoad = 0x00001000,
	RF_NeedPostLoadSubobjects = 0x00002000,
	RF_NewerVersionExists = 0x00004000,
	RF_BeginDestroyed = 0x00008000,
	RF_FinishDestroyed = 0x00010000,
	RF_BeingRegenerated = 0x00020000,
	RF_DefaultSubObject = 0x00040000,
	RF_WasLoaded = 0x00080000,
	RF_TextExportTransient = 0x00100000,
	RF_LoadCompleted = 0x00200000,
	RF_InheritableComponentTemplate = 0x00400000,
	RF_DuplicateTransient = 0x00800000,
	RF_StrongRefOnFrame = 0x01000000,
	RF_NonPIEDuplicateTransient = 0x02000000,
	RF_Dynamic = 0x04000000,
	RF_WillBeLoaded = 0x08000000,
};

enum EInternalObjectFlags
{
	None = 0,
	ReachableInCluster = 1 << 23,
	ClusterRoot = 1 << 24,
	Native = 1 << 25,
	Async = 1 << 26,
	AsyncLoading = 1 << 27,
	Unreachable = 1 << 28,
	PendingKill = 1 << 29,
	RootSet = 1 << 30,
	GarbageCollectionKeepFlags = Native | Async | AsyncLoading,
	AllFlags = ReachableInCluster | ClusterRoot | Native | Async | AsyncLoading | Unreachable | PendingKill | RootSet,
};

typedef UObject* (__fastcall* f_StaticConstructObject_Internal)(
	UClass* Class,
	UObject* InOuter,
	void* Name,
	EObjectFlags SetFlags,
	EInternalObjectFlags InternalSetFlags,
	UObject* Template,
	bool  bCopyTransientsFromClassDefaults,
	void* InstanceGraph,
	bool  bAssumeTemplateIsArchetype
	);
static f_StaticConstructObject_Internal StaticConstructObject_Internal;