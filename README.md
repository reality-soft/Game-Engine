# 23-02-11
###  피직스 라이브러리 적용  
ReactPhysics3D (RP3D) 를 설치 밎 적용하였고 컴파일 및 디버그로 정상적인 링크와 함수호출을 확인했습니다.
https://www.reactphysics3d.com/usermanual.html#x1-160005 여기서 친절하고 자세한 사용 설명서를 확인할 수 있습니다.

### 마우스 레이캐스트 추가  
마우스 좌표를 통해 3D 공간 좌표를 구하고 이를 레이의 출발 지점으로 하여 카메라의 look벡터에 1000을 곱한 거리까지 레이가 발사됩니다.  
MapTool의 MapTool.cpp 54번째 줄에 중단점을 걸고 레이의 출발지점과 시작지점의 좌표를 확인하실 수 있습니다.  
레이는 정상적으로 발사되는것 같지만 맵을 하나의 평면 콜리전으로 하여 충돌체크는 아직 해결해야 할 과제입니다.  
충돌이 정상적으로 이루어지면, 그 다음은 기하 쉐이더를 통한 정점 편집이 되겠습니다.  
