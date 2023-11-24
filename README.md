# BufferCacheProject
---
* 코드 패턴은 가급적이면 MVC 나 MVP패턴을 유지시키면서 작성하는 것이 좋음
    - 패턴 설명: https://beomy.tistory.com/43
* Git 업로드 및 브랜치 규칙은 가급적이면
  *  각 폴더를 개발할때는 [Trunk Based Development](https://helloinyong.tistory.com/335) 를 사용하도록 하고
  *  buffer.c 를 직접적으로 수정 및 추가할 때는 [Git-flow](https://techblog.woowahan.com/2553/) 를 사용해서 작성하는 것을 권고
* 각 폴더를 개발할때는 가급적이면 이해하기 쉽게 전역 변수 선언이나 함수선언시,어떤 기능을 하기 위한 함수인지 선언 위쪽에 간략하게 **주석을 달아주기를 권고**
* 폴더 설명
  
  | 폴더 | 설명 |  비고 |  
  |---|---|---|  
  |delayed_write| delayed_write 맡은 팀원이 코드 작성 및 관리 | |  
  |buffered_read| buffered_read 맡은 팀원이 코드 작성 및 관리 | |  
  |replacement_policy| replacement_policy 맡은 팀원이 코드 작성 및 관리| |  
  |shared| 각 폴더나 각 맡은 분야에서 서로 참조하는 변수나 함수를 작성 관리 | 예)메모리버퍼변수|
  |external_tools| 디스크 파일 생성등 프로젝트에 필요한 도구들이 모여있음 | 개별 빌드 및 실행|

* 각 폴더에 있는 "export_api.h"
  - 코드 작성 완료 뒤, buffer.c(메인 코드) 함수에서 작성한 함수나 변수가 노출(public)되어 질 수 있도록 헤더를 기술
* "shared/common_shared.h"
  - 꽤나 높은 빈도수로 여러 분야에서 참조될 것 같은 공용 함수 및 변수를 기술
* "external_tools/create_empty_disk_file.c"
  -  dd명령어를 사용해 변수 *n* 에 따른 다른 크기의 디스크 파일 생성

* Makefile
  * 폴더로 나누어 놔서, 빌드하기 쉽게 Makefile로 작성
  * ~~하위 폴더에서 서브 Makefile을 만들어 따로 관리해도 괜찮으나,구현시 최상단 Makefile코드에 에러 없고 업데이트가 가능하게 끔 서브 빌드를 구현같이 구현바람~~
  * 상위 Makefile에서 터미널상에서 정상인데 object파일 생성이 안되는 이슈때문에 각 폴더에서 Makefile을 관리하는 방식으로 변경(추가/변경이 필요한 경우 각자 관리 및 수정바람)
  * 빌드 명령어
    ``` shell
        make
        #혹은
        make buffer_cache
    ``` 
  * clean 명령어
      ``` shell
          make clean
          #혹은
          make clean-all
      ``` 

기타 참조 링크
---
* https://github.com/melody1214/Trapfetch
* https://github.com/torvalds/linux/blob/master/mm/mmap.c