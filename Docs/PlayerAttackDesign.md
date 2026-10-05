# 플레이어 공격 기획서

## 1. 방향

**액션 어드벤처식 보스 전투. 핵심은 자연스러운 액션이다.**

소울라이크의 공격 커밋먼트(시작하면 끝까지 취소 불가) 대신, 공격 중간에 다음 공격·대시로 **캔슬하며 이어가는 매끄러운 연계**를 핵심으로 한다. 스킬 슬롯은 RPG처럼 적은 수만 두고, 다양성은 콤보와 파생기로 만든다.

레퍼런스: 붉은사막 — 플레이어와 보스의 매끄러운 이동·공격 연계, 카메라 연출, 환경 파괴.

보스 쪽(패턴, 텔레그래프, 그로기)은 기존 구조를 그대로 유지한다. 패턴 학습은 핵심 재미가 아니다.

## 2. 입력

| 입력 | 동작 | 입력 태그 | 사용 제한 |
| --- | --- | --- | --- |
| 좌클릭 | 약공격 | `Input.Light` | 없음 |
| 우클릭 | 강공격 | `Input.Heavy` | 없음 |
| Shift | 대시 | - | 스태미나 |
| Q | 특수 공격 1 | `Input.Special1` | 쿨다운 |
| E | 특수 공격 2 | `Input.Special2` | 쿨다운 |
| R | 궁극기 | `Input.Ultimate` | 쿨다운 |

공격 자원(쿨다운)과 이동 자원(스태미나)을 분리한다. 스킬을 많이 써도 회피가 막히지 않는다.

### 입력 바인딩

공격 입력은 **Input Action → 입력 태그** 매핑으로 바인딩한다. (Lyra 방식)

- `APlayerCharacter::AttackInputMappings`에 `{InputAction, InputTag}`를 둔다
- 모든 공격 입력은 `AttackInput(InputTag)` 하나로 받아 공격 컴포넌트에 넘긴다
- 공격 입력이 늘어나도 함수 추가 없이 매핑만 추가한다
- 바인딩은 `Started`. 누르고 있어도 한 번만 들어간다 (IA / IMC의 Trigger는 비워둔다)

## 3. 공격 구성

| 공격 | 구성 | 공격 타 태그 |
| --- | --- | --- |
| 약공격 | 3타 콤보 | `Attack.Light.1` / `.2` / `.3` |
| 파생 마무리 | 약-약-강 | `Attack.Light.Heavy` |
| 강공격 | 1타 | `Attack.Heavy` |
| 특수 공격 1 | 2단 (같은 키 재입력) | `Attack.Special1.1` / `.2` |
| 특수 공격 2 | 2단 (같은 키 재입력) | `Attack.Special2.1` / `.2` |
| 궁극기 | 1타 | `Attack.Ultimate` |

- 공격 타는 **게임플레이 태그**로 구분한다. 태그는 "이 타가 무엇인지"만 나타내고, 연계 관계는 연계 트리(4장)가 담당한다
- 특수 공격과 궁극기의 역할·연출은 미정

## 4. 연계

### 연계 트리

연계는 `FPlayerAttackTransitionTable`에 **"부모 타 + 입력 → 자식 타"**를 한 줄씩 저장한다. 공격 첫 타는 부모를 비워둔다 (단발 공격 포함).

| ParentStep | Input | ChildStep | 의미 |
| --- | --- | --- | --- |
| (비움) | `Input.Light` | `Attack.Light.1` | 약공격 시작 |
| `Attack.Light.1` | `Input.Light` | `Attack.Light.2` | 약 → 약 |
| `Attack.Light.2` | `Input.Light` | `Attack.Light.3` | 약약약 |
| `Attack.Light.2` | `Input.Heavy` | `Attack.Light.Heavy` | 약약강 파생 |
| (비움) | `Input.Heavy` | `Attack.Heavy` | 강공격 |
| (비움) | `Input.Special1` | `Attack.Special1.1` | 특수 1 - 1단 |
| `Attack.Special1.1` | `Input.Special1` | `Attack.Special1.2` | 특수 1 - 2단 |
| (비움) | `Input.Special2` | `Attack.Special2.1` | 특수 2 - 1단 |
| `Attack.Special2.1` | `Input.Special2` | `Attack.Special2.2` | 특수 2 - 2단 |
| (비움) | `Input.Ultimate` | `Attack.Ultimate` | 궁극기 |

- 현재 타에서 이어지는 줄이 없으면 **시작(부모가 빈 줄)에서 다시 찾는다.** 그래서 아래 경우는 줄을 따로 넣지 않는다
  - 약 1타 + 강 → 강공격 / 약 3타 + 약 → 약 1타 / 강공격 + 약 → 약 1타 / 특1 2단 + 특1 → 특1 1단
- 공격 사이의 연계(예: 약약 → 특1 연계 버전)도 코드 수정 없이 줄 하나로 추가한다
- 저장은 평평한 배열, 실행은 `Build()`가 BeginPlay에서 만든 `부모 → (입력 → 자식)` 조회 표를 쓴다. 같은 (부모 + 입력)이 두 번 있으면 경고 후 건너뛴다

### 선입력

한 타 안의 입력 처리 구간을 `EAttackInputPhase`로 나눈다. **공격 컴포넌트가 관리한다.**

```
|── Startup ──|── Active ──|──────── Recovery ────────|
                     [====== InputWindow ======]
  BeforeWindow    |        InWindow           |  AfterWindow
```

| 구간 | 입력 처리 |
| --- | --- |
| `BeforeWindow` | 무시 |
| `InWindow` | 버퍼에 저장 → **전환 시점(InputWindow 끝)에** 연계 실행 |
| `AfterWindow` | 버퍼에 저장 → **공격이 끝난 뒤** 새 공격으로 실행. 종료 0.2초 전 이후 입력만 인정 |
| 공격 중 아님 | 버퍼 없이 즉시 실행 |
| 취소(대시, 전환)로 끝남 | 남은 선입력은 버림 |

- 언제 눌렀든 같은 전환 시점에 넘어가므로 콤보 리듬이 일정하다
- 전환 시점에 선입력이 없으면 Recovery를 끝까지 재생한다. 마무리 동작이 보인다
- 버퍼는 덮어쓰기. 여러 번 누르면 마지막 입력 하나만 남는다
- 종료 직전 유효 시간 `InputBufferDuration`(기본 0.2초)은 컴포넌트에 값 하나로 둔다
- 새 타가 시작되면 `BeforeWindow`로 되돌린다

### 대시 캔슬

- 궁극기를 제외한 모든 공격은 **어느 시점이든** 대시로 끊을 수 있다. 피하고 싶은 순간에 바로 피해야 조작이 매끄럽다
- 공격의 위험은 타이밍 대신 **대시 스태미나**가 만든다. 언제든 끊을 수 있지만 무한히는 못 끊는다
- 궁극기는 **판정이 끝날 때까지 무적 + 대시 캔슬 불가**, Recovery부터 무적이 풀리고 대시로 빠져나갈 수 있다
  - 끊을 수 없는 동안 맞으면 억울하므로 무적을 함께 준다
- 스태미나 소모가 확정된 뒤에 공격을 끊는다. 스태미나가 부족하면 공격은 유지된다

## 5. 한 타 데이터

```cpp
USTRUCT()
struct FPlayerAttackStep
{
	FGameplayTag StepTag;          // 이 타의 태그 (Attack.*)
	TObjectPtr<UAnimMontage> Montage;
	FCombatDamage CombatDamage;

	// 범위 판정 크기. 나중에 무기 궤적으로 교체
	float HitRadius;
	float HitRange;
};
```

- 공격 객체마다 `AttackStepData`(배열)에 자기 타들을 가진다. 예: 약공격 객체 = `Light.1`, `Light.2`, `Light.3`, `Light.Heavy`
- 공격마다 몽타주를 따로 둔다. 따로 만든 클립(구매·AI 생성·믹스아모)을 섞기 쉽고, 전환이 항상 블렌드로 이어진다
- 타이밍(판정, 선입력 구간, 종료)은 데이터가 아니라 **몽타주의 AnimNotifyState**가 정한다 (7장)

### 데미지

타마다 `FCombatDamage`를 원소에 직접 둔다. 보스처럼 `TMap<공격 클래스, FCombatDamage>`로 따로 모으지 않는다.

- 한 공격이 여러 타로 나뉘어, 공격 클래스 하나에 값 하나인 TMap과 데이터 형태가 맞지 않는다
- 두 표를 클래스 키로 연결하지 않으므로 키 불일치로 데미지가 0이 되는 문제가 없다

플레이어 공격에서 쓰는 값은 `Damage`, `StaggerDamage` 두 개다. `HitIntensity`는 플레이어가 맞을 때만 쓰므로 조정하지 않는다.

## 6. 판정

| 단계 | 방식 | 설명 |
| --- | --- | --- |
| 지금 | **범위 판정 (Shape Query)** | HitWindow에서 캐릭터 앞쪽 범위를 검사 |
| 이후 | **무기 궤적 추적 (Weapon Trace)** | HitWindow 동안 칼 소켓을 이전 프레임 → 현재 프레임으로 Sweep |

- 판정은 함수 하나로 감싸서, 교체 시 안쪽만 바꾼다
- 한 번 휘두를 때 같은 대상은 한 번만 맞는다 (적중 대상 목록 유지)

## 7. 타이밍 (AnimNotifyState)

타이밍은 몽타주에 놓은 **전용 NotifyState 클래스**가 정한다. Notify가 몽타주 주인의 공격 컴포넌트를 찾아 직접 호출한다.

```
몽타주 타임라인
Track 1 : |── Startup ──[====== Hit Window ======]──── Recovery ────|
Track 2 : |───────────────────[==== Input Window ====]──────────────|
                                ↑ Begin             ↑ End
```

| NotifyState | Begin | End |
| --- | --- | --- |
| `UAnimNotifyState_HitWindow` | Active (판정 시작) | Recovery (판정 끝) |
| `UAnimNotifyState_InputWindow` | 선입력 구간 열기 (`InWindow`) | 전환 시점 (`AfterWindow` + 버퍼 확인) |
| 몽타주 종료 (`Montage_SetEndDelegate`) | - | 정상 종료면 공격 종료. 끊긴 종료(`bInterrupted`)는 무시 |

- 상태마다 클래스를 두는 기준 : 그 상태만의 로직이나 데이터가 있는가. HitWindow는 무기 궤적 판정 로직과 소켓 데이터를 가지게 된다
- 공통 부모 `UAnimNotifyState_PlayerAttack`이 공격 컴포넌트 찾기를 담당한다
- 몽타주 에디터 미리보기에는 공격 컴포넌트가 없으므로, 찾지 못하면 무시한다
- 다음 타 몽타주를 재생하면 이전 몽타주가 끊긴 종료를 보낸다. 이를 무시하지 않으면 다음 타가 시작되자마자 공격이 끝난다

## 8. 클래스 구성

```
APlayerCharacter
 ├ AttackInputMappings              Input Action → 입력 태그
 └ UPlayerAttackComponent           입력, 선입력 구간 / 버퍼, 연계, 현재 공격 관리
     ├ AttackInstances              공격 객체 배열 (Instanced)
     ├ TransitionTable              연계 트리
     └ AttackByTag                  StepTag → 공격 객체 (BeginPlay에서 자동 생성)

UPlayerAttackBase (UObject)         공격 하나의 진행. 몽타주 재생, 판정
UAnimNotifyState_PlayerAttack       NotifyState 공통 부모
 ├ UAnimNotifyState_HitWindow
 └ UAnimNotifyState_InputWindow
```

### 공격 객체를 Instanced로 두는 이유

- 같은 클래스를 데이터만 다르게 여러 개 쓴다 (약공격, 강공격)
- `TSubclassOf` + `NewObject`는 클래스 기본값을 복사하므로, 데이터를 다르게 주려면 공격마다 Blueprint 자식이 필요하고 데이터가 여러 에셋으로 흩어진다
- Instanced는 컴포넌트 Details 한곳에서 모든 공격 데이터를 편집한다
- 객체를 재사용하므로 이전 실행 상태가 남지 않게 시작할 때 초기화한다

보스 공격은 `NewObject`를 유지한다. 공격마다 로직이 달라 공격 하나가 클래스 하나이고, 실행 중 상태(투사체, 소환물, 타이머)가 많아 매번 새 객체가 안전하다.

### 공격 실행 흐름

```
입력 태그 ──(연계 트리)──▶ 공격 타 태그 ──(AttackByTag)──▶ 공격 객체 ──▶ PlayStep(타 태그)
```

- `StartAttack(StepTag)` : 공격 중이면 현재 공격을 끊고, 해당 공격 객체의 `PlayStep`을 호출한다
- `PlayStep` : 태그로 타 데이터를 찾아 몽타주를 재생한다. 모든 타가 같은 흐름이라 시작(Begin)과 다음 타 재생을 나누지 않는다
- 공격 객체는 연계인지 새 사용인지 알 필요가 없다. 태그별 데이터를 재생할 뿐이다
- 공격 상태(실행 중, Active / Recovery)는 공격 객체, 입력 상태(선입력 구간, 버퍼, 현재 타)는 컴포넌트가 가진다

`UBossAttackBase`와 공유하지 않는다. 보스 공격은 AI가 고르고 Timer로 진행하지만, 플레이어 공격은 입력으로 시작하고 몽타주·연계·캔슬로 진행해 흐름이 다르다.

## 9. 제외 / 보류

| 항목 | 상태 | 비고 |
| --- | --- | --- |
| 가드 | 제외 | 방어는 대시로만 한다 |
| 대시 무적 시간 (i-frame) | 보류 | 현재 없음. 추후 추가 |
| 저스트 회피 | 보류 | 대시 초반 짧은 구간에 공격이 닿으면 성공. 보상 미정 |
| 환상검 (`ABlade`) | 보류 | 공격·스킬·궁극기 연출 확정 후 사용 여부 결정. 기존 코드는 연결 해제 상태 |
| 기존 기본 공격 타겟팅 | 보류 | 화면 중앙 대상 선택 방식. 새 구조에서 유지 여부 미정 |
| 연계 트리 에디터 툴 | 보류 | Details에서 트리(들여쓰기)로 편집. 기능 마무리 후 디테일 기간에 제작 |

## 10. 일정

**~ 10월 10~15일 : 기능 마무리**

1. 공격 구조 + 약/강 콤보 + 파생기 + 대시 캔슬
2. 특수 공격 1·2, 궁극기
3. 입력 잠금(`bIsInputLocked`) 연결
4. 환경 상호작용
5. 카메라 연출
6. VFX / 사운드 연결

**이후 : 디테일**

- 범위 판정 → 무기 궤적 교체
- 대시 무적 시간, 저스트 회피
- 연계 트리 에디터 툴

## 11. 미정 / 남은 작업

- 특수 공격 1·2, 궁극기의 역할과 연출
- 스킬별 쿨다운 값
- 타마다 데미지·판정 범위 수치
- 환상검 사용 여부
- 타겟팅 / 락온 방식
- **연계 / 새 사용 구분** : 쿨다운은 새 사용(시작 줄에서 찾음)에만 적용한다. `FindNextStep`이 어느 줄에서 찾았는지 알려주도록 확장 필요
- **Cancel 조건** : 정확한 기준은 "다른 공격 객체이거나, 새 사용"이다. 공격 단위로 유지하는 것(궁극기 무적, 공격 내내 켜는 이펙트)이 생기면 이 기준으로 바꾼다
