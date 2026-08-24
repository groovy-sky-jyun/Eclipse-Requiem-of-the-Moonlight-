# Eclipse

페이즈에 따라 패턴이 달라지는 3인칭 보스전 액션.


| 장르 | 인원 | 기간 | 엔진 |
|---|---|---|---|
| 3인칭 보스전 액션 | 1인 | 2026.04 — 진행 중 | Unreal Engine 5.7 |


---
## 주요 기능
### 보스
- **3단계 페이즈** — 체력 비율에 따라 전환되며, 페이즈마다 사용 가능한 패턴과 가중치가 달라짐

- **공격 패턴 6종** — BloodBolt · DarkSweep · ShadowCrash · WraithDrop · LunarBeam · EclipseVeil

- **가중치 룰렛 선택** — 쿨타임 · 플레이어와의 거리 · 전투당 사용 횟수를 걸러낸 뒤 가중치로 추첨

- **스태거** — 피격 데미지가 누적되면 경직. 페이즈마다 임계값이 다르고, 일정 시간 맞지 않으면 초기화

- **미니언 소환** — WraithDrop이 EQS로 플레이어를 포위하는 위치를 산출해 망령을 배치

  
### 전투 공간
- **보스 아레나** — 플레이어 진입을 감지해 전투를 시작하고, 전투 중 이탈을 차단

- 아레나 중심이 EclipseVeil · LunarBeam 같은 **광역 패턴의 좌표 기준**이 됨


### 플레이어
- 기본 공격 **콤보**, 특수 공격 2종, 궁극기, 방어, 대시


### 전투 흐름
- **GameMode가 시작 · 승리 · 패배 · 재시작을 관리**

- 전투 시작 / 종료 시점을 델리게이트로 알려 UI · 연출 · 사운드가 붙을 수 있도록 분리

---
## 조작법

| 키 | 동작 |
|---|---|
| `W` `A` `S` `D` | 이동 |
| `마우스` | 시점 |
| `Space` | 점프 |
| `좌클릭` | 기본 공격 (콤보) |
| `우클릭` | 방어 |
| `Q` | 특수 공격 1 |
| `E` | 특수 공격 2 |
| `R` | 궁극기 |
| `F` | 대시 |

---
## 설계

### 보스 컴포넌트 분리
보스의 상태와 행동을 두 컴포넌트로 나눴습니다.

| 컴포넌트 | 책임 |
|---|---|
| `UBossPhaseComponent` | 페이즈 전환 · 스태거 누적 |
| `UBossAttackComponent` | 공격 인스턴스 소유 · 선택된 공격 실행 |

페이즈가 바뀌는 원인은 체력과 피격 누적이라 **공격 선택과 무관**합니다. 공격 쪽은 현재 페이즈를 묻기만 합니다.

  
### 공격 선택
Behavior Tree의 `BTTask_SelectAttack`이 페이즈별 풀에서 하나를 고릅니다.

```

1. 쿨타임이 지나지 않은 패턴 제외

2. 거리 조건을 벗어난 패턴 제외

3. 사용 횟수를 소진한 패턴 제외

4. 남은 패턴을 가중치 비율로 추첨

```

가중치는 같은 페이즈 안에서의 **상대값**이라 합계가 100일 필요가 없습니다. 페이즈가 올라갈수록 상위 패턴의 가중치를 높여 등장 빈도를 조절합니다.


### 공격 하나 = 클래스 하나

각 패턴은 `UBossAttackBase`를 상속한 클래스로 분리했습니다.

```

UBossAttackBase
├─ OnStart()    패턴 시작 — 몽타주 재생 · 투사체 스폰 · 타이머 등록
└─ OnFinish()   정리 — 타이머 해제 · 스폰물 회수

```

패턴 하나의 내부 구현이 다른 패턴에 영향을 주지 않고, `UBossAttackComponent`는 어떤 패턴인지 몰라도 실행할 수 있습니다.

---
## 기술 스택

| | |
|---|---|
| 엔진 | Unreal Engine 5.7 |
| 언어 | C++ · Blueprint (에셋 조립) |
| AI | Behavior Tree · Blackboard · EQS · NavMesh |
| 모듈 | AIModule · GameplayTags · Niagara · UMG · EnhancedInput |
| 개발 툴 | Visual Studio · Git |

---
## 프로젝트 구조

```

Source/Eclipse/
├─ AI/
│  ├─ BossAIController          보스 전용 Blackboard 키 정의
│  ├─ WraithAIController        미니언 AI
│  ├─ BTTask_SelectAttack       페이즈별 풀에서 패턴 추첨
│  ├─ BTTask_ExecuteAttack      선택된 패턴 실행
│  ├─ BTTask_OrbitPlayer        플레이어 주위 선회
│  ├─ BTTask_StaggerRecover     경직 회복
│  └─ BTService_UpdatePhase     체력 비율 감시 · 페이즈 전환
├─ Character/
│  ├─ Player/PlayerCharacter    콤보 · 특수 공격 · 방어 · 대시
│  └─ Enemy/
│     ├─ EnemyBase              적 공통 (체력 · 피격 · 사망)
│     ├─ EnemyBoss              보스
│     └─ EnemyMinion            Wraith 망령
├─ Combat/
│  ├─ Attacks/                  보스 패턴 6종
│  ├─ BossAttackBase            패턴 공통 인터페이스
│  ├─ BossAttackComponent       패턴 소유 · 실행
│  ├─ BossPhaseComponent        페이즈 · 스태거
│  └─ BossAttackPoolRow         패턴 풀 데이터 규격
├─ Interface/CombatInterface    피격 처리 공통 인터페이스
├─ BossArena                    전투 공간 · 진입 감지 · 이탈 차단
└─ EclipseGameMode              전투 흐름 관리

```

패턴을 추가하려면 `UBossAttackBase`를 상속한 클래스를 만들고, `EBossAttackType`에 항목을 더한 뒤 해당 페이즈의 풀에 등록하면 됩니다.

---
## 빌드
Unreal Engine 5.7과 Visual Studio가 필요합니다.

```

1. Eclipse.uproject 우클릭 → Generate Visual Studio project files

2. Eclipse.sln 열기 → Development Editor / Win64 로 빌드

3. Eclipse.uproject 실행

```