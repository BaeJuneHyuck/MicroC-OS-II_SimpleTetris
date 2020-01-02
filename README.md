# 2019, PNU CSE, Embedded System Term Project, Tetris

임베디드시스템 텀프로젝트 : 테트리스

    임베디드 시스템 060 분반 
    담당 백윤주 교수님
    201624441 김영빈 
    201424464 배준혁

# 소개

 * 본 텀 프로젝트는 RTOS(Real Time Operating System)를 적용하여 Task간의 통신이 가능한 테트리스 임베디드 시스템 Application을 개발하였습니다. 초음파 센서를 활용하여 테트리스 블록의 움직임을 제어합니다. 또한 RTOS에서 Real Time을 지원하기 위해 제공하는 함수를 이용하여 Task간의 Communication을 Block되지 않도록 하였습니다. 

# 텀 프로젝트 개요 
 
 1) 배경지식 
 * RTOS 
  Real Time Operating System, 실시간 운영 체제 또는 RTOS는 실시간 응용 프로그램을 위해 개발된 운영 체제이다. RTOS는 일반적인 OS와 달리 효율성보다 실시간성을 중시한다. 따라서 특정 시간내에 반드시 특정 작업이 실행되어야 하는 시스템에 적합하다.  
 
 * UC MicroC/OS-II 
 MicroC/OS(Micro-Controller Operating Systems, µC/OS) version 2는 임베디드 소프트웨어 개발자 Jean J. Labrosse가 1991년에 설계한 실시간 운영 체제이다.  운영체제는 5종류의 상태를 가지는 Task들을 각각의 우선 순위를 통해서 관리한다. UCOS에서 실행중인 Task들은 인터럽트, 세마포, 메세지 큐를 이용하여 소통한다. 아주 적은 용량으로 OS에 필요한 기능들을 구현하였고 간단한 구조를 가진 것이 큰 장점이다. 항공전자, 의료, 데이터 통신 장치 등 다양한 임베디드 시스템에서 사용된다. 
 

 2) 사용한 모듈 및 센서 

 * MAX7219 Dot Matrix Module  
 
 ![preview](https://github.com/BaeJuneHyuck/MicroC-OS-II_SimpleTetris/blob/master/capture/1.jpg?raw=true)
 
5V의 전압에서 작동하는 8X8 도트 매트릭스이다. 도트 매트릭스는 버스 정류장, 간판등에서 일반적으로 사용되며 여러 LED를 2차원 배열의 형태로 연결하여 쉽게 사용할 수 있게 해준다. 해당 모듈은 VCC, GND, DIN, CS, CLK의 5개의 단자를 가지고 있다. clk을 low로 설정한 뒤 출력할 데이터를 레지스터에 입력, 다시 clk을 high로 설정하여 출력한다. 
 

 * 초음파 거리센서 모듈 HC-SR04P 

 ![preview](https://github.com/BaeJuneHyuck/MicroC-OS-II_SimpleTetris/blob/master/capture/2.jpg?raw=true)
 
초음파 거리센서 모듈은 초음파를 내보내고 다시 돌아오는 시간을 이용하여 대상과의 거리를 측정하는 센서이다. 해당 센서는 Vcc, Gnd, Trig, Echo 핀을 사용한다. Trig가 High일때 센서가 초음파를 내보낸다. 초음파를 내보낸 이후 Echo를 High상태로 유지하여 값을 듣는 상태가 된다. 초음파는 정면의 물체와 부딛힌 후 다시 돌아오며 이때 echo가 Low로 바뀌며 그 사이의 시간을 측정하여 값을 저장한다. 해당 시간이 소리의 속도로 물체와의 거리를 두 번 왕복하는데 걸린 시간이다. 
 
 3) 개발 시스템
* STM32VL-DISCOVERY

 ![preview](https://github.com/BaeJuneHyuck/MicroC-OS-II_SimpleTetris/blob/master/capture/3.png?raw=true)
 
ARM의 Cortex-M3 CPU를 기반으로 하는 개발보드이다. 16개의 GPIO Pin을 지원하는 4개의Port를 가지고 있으며 3개의 USART채널, 2개의 SPI 채널, I2C채널 및 15개의 ADC 채널 5개의 타이머를 지원하는 보드이다. ST-Link를 지원하기 때문에 ST-Link 디버거를 내장하여 간단하게 USB 연결포트로 STM32 DISCOVERY를 디버깅 할 수 있다.

# capture


 ![preview](https://github.com/BaeJuneHyuck/MicroC-OS-II_SimpleTetris/blob/master/capture/4.png?raw=true)
 
 ![preview](https://github.com/BaeJuneHyuck/MicroC-OS-II_SimpleTetris/blob/master/capture/5.png?raw=true)
시연 영상

https://drive.google.com/file/d/1PkvcmsW1Pm5LGX_8gmTtLpdBN8o9CABE/view?usp=sharing 

