# 소스파일 분류
### 새로운 폴더 (Engine/src/)
1. Actor/
- Enity를 갖고 씬에 할당되는 오브젝트들을 분류하였습니다.

2. Components/
- Enity에 할당되는 컴포넌트들을 분류하였습니다.
3. Core/ 
- 엔진과 더불어 엔진 내부에서 주로 돌아가는 DX, 이벤트, Fmod, 인풋, 리소스매니저, 씬, 타임매니저들을 분류하였습니다.
4. DataTypes/
- 기존 DateTypes.h에 모여있던 구조체들을 5가지 헤더로 분류하였습니다.
5. Headers/
- DllMacro, entt.hpp, GolobalFunctions 등 stdafx와 함께 묶을 헤더들을 분류하였습니다.
6. Physics/
- 쿼드트리와 충돌체, 충돌로직 소스들을 분류하였습니다.
7. ResourceTypes/
- 머터리얼, 메쉬, 쉐이더, 텍스처들을 분류하였습니다.
8. Systems/
- ECS 기반 시스템 클래스들을 분류하였습니다.
9. Tools/
-  파일입출력, ImGui 등 에디터 기능 클래스들을 분류하였습니다.
10. UI/
- UI 관련 객체 클래스와 RenderTargetMgr 및 WriteMgr를 분류하였습니다.
11. World/
- 레벨, 스카이스피어 등의 클래스들을 분류하였습니다.
