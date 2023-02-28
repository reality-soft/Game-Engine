# 02-28
### LOD 레벨 생성
LOD 레벨을 생성하는 방법은 다음과 같습니다.  
```c++
Level level;
level.CreateLevel(3, 8, 100, {8, 8});
// CreateLevel(최대 LOD, 셀의 규모, UV스케일, { 가로 셀, 세로 셀})
```
- 최대 LOD : 2의 최대 LOD 제곱만큼 하나의 셀이 나누어집니다. (LOD 3 : 셀 당 8칸)
- 셀의 규모 : 한 셀의 가로, 세로 규모입니다. 정점 간의 거리는 (셀 크기 / 한 셀의 칸 수) 입니다.
- 스케일 : 텍스처를 얼마나 크게 뿌릴지 결정합니다(숫자가 클수록 텍스처가 더 작게 많아짐). 한 셀의 칸 수와 UV 스케일이 같으면 한 셀에 하나의 텍스처가 뿌려집니다.
- 가로셀, 세로 셀 : 셀을 가로, 세로로 배치할 수입니다. 안정적인 쿼드트리 분할을 위해선 2의 제곱수로 넣어야 합니다.
- 쿼드트리 분할시 리프노드는 반드시 1개의 1셀을 가지도록 깊이가 자동으로 결정됩니다.

# 02-24
### 머터리얼의 통합 구조
- 아직까지 완벽하게 여러 리소스를 쉐이더로 보내고 쉐이더 내에서 가변적으로 처리하는 방법을 찾진 못했습니다.  
다만 Texture2DArray 의 사용법에 대해서는 아직 연구의 여지가 많으며, 직접 여러 이미지를 병합해 하나의 텍스처로 넘기고  
uv값으로 텍스처를 고를 수 있는 방법도 있긴 합니다.

### 정적 오브젝트에 대한 인스턴싱과 레벨 및 쿼드트리 에서의 관리
- 게임을 구성하는 모든 구성요소를 ECS로 돌리기에는 다소 무리가 있을 것으로 보입니다. 따라서 맵의 일부라고 할만한 정적 오브젝트는  
쿼드트리와 레벨 인스턴싱을 통해 관리할 생각입니다.

# 02-25
### 머터리얼의 새 구조
```c++
	{
	public:
		Material() = default;
		~Material() = default;

	public:
		string diffuse;
		string normalmap;
		string metalic;
		string roughness;
		string sepcular;
		string ambient;
		string opacity;
		string shader = "SurfacePS.cso";

		ID3D11ShaderResourceView* textures[7];

		void CreateAndSave(string filename);
		void LoadAndCreate(string filename);
	};
```
- 머터리얼은 베이스컬러를 표현현하는 디퓨즈 외에 6개의 효과 텍스처를 가집니다. 각각 표면에 특수효과(반사, 노멀, 투명도)를 부여하기 위한 텍스처로 필요에 따라 비워둘 수도 있습니다. 제가 이 7가지 텍스처를 기본으로 넣은 것은 대부분의 리소스가 이 7가지 범주 내에 들어오기 때문입니다.

- 이 텍스처들은 가변적이지 않은 7개의 ID3D11ShaderResourceView 배열에 저장된 후 쉐이더에 넘겨집니다. 픽셀 쉐이더도 마찬가지로 7개의 텍스처 배열을 사용할 것입니다. 각 텍스처 배열의 인덱스마다 샘플링한 값이 0이면 flaot4(1, 1, 1, 1)을 반환하여 일부 텍스처의 부재로 인해 전체 컬러가 누락되지 않도록 해야 합니다.


### 머터리얼에 대한 정의를 리소스로 관리하기
- 머터리얼은 여러 개의 텍스처와 하나의 픽셀 쉐이더를 사용하는 것이 특징입니다. 이러한 머터리얼마다 어떤 텍스처를 사용할지 런타임에 정해주는 것은 상당히 귀찮은 작업입니다. 따라서 가장 좋은 방법은 텍스트 파일로 이들의 정의를 저장하고 불러오는 것입니다.

```
diffuse : Wood_DF.png
normalmap : Wood_NM.png
metalic : Wood_MT.png
roughness : Wood_RN.png
sepcular : null
ambient : Wood_AO.png
opacity : null
shader : SurfacePS.cso
```

- 머터리얼의 정의에 대한 텍스트파일은 이런 구조입니다. 상당히 간단하지만 diffuse, normalmap 과 같은 태그는 머터리얼 클래스의 멤버변수를 찾는데 쓰이므로 클래스의 멤버보다 임의적으로 빼거나 추가하면 문제가 발생할 수 있습니다. 따라서 이러한 태그는 고정값이되 실제 텍스처가 없는 경우에는 null 과 같은 문자를 넣어 무마시킬 수 있습니다.
