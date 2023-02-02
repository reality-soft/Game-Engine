# Game-Engine

# 1. 컴포넌트와 시스템에서의 활용 (Entity Component System)
- Components.h 에서 필요한 새 컴포넌트를 정의합니다. 컴포넌트는 최상위 부모인 Component로부터 상속받습니다.
- 필요한 리소스는 ResourceMgr::PushResource() 함수로 리소스 매니저에 등록합니다. ID값과 파일경로를 넘겨줍니다.
- 컴포넌트는 PushResource에서 넘겨준 ID값을 가지고 있어야 합니다.
- 시스템에서 registry를 순회하며 추가한 컴포넌트 타입의 entity를 가져옵니다.
- entity에 할당된 컴포넌트 객체를 참조하여 ID값을 추출하고 이를 ResourceMgr::UseResource() 함수에 넘겨줍니다.
- 반환된 포인터를 참조하여 시스템에서 활용합니다.
