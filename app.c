/*
 *********************************************************************************************************
 *                                              EXAMPLE CODE
 *
 *                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
 *
 *               All rights reserved.  Protected by international copyright laws.
 *               Knowledge of the source code may NOT be used to develop a similar product.
 *               Please help us continue to provide the Embedded community with the finest
 *               software available.  Your honesty is greatly appreciated.
 *********************************************************************************************************
 */

 /*
  *********************************************************************************************************
  *
  *                                            EXAMPLE CODE
  *
  *                                     ST Microelectronics STM32
  *                                              with the
  *                                   STM3210B-EVAL Evaluation Board
  *
  * Filename      : app.c
  * Version       : V1.10
  * Programmer(s) : BAN
  *********************************************************************************************************
  */
#define APP_TASK_TETRIS_PRIO 6
#define APP_TASK_INPUT_PRIO 5
#define APP_TASK_PRINT_PRIO 7

  /*
  ***********************************************************
   *                              Tetris Logic Variable  Define
  ***********************************************************
  */
#define BW 8
#define BH 8
#define EMPTY 0
#define BRICK 1
#define WALL 2
  /*
   *********************************************************************************************************
   *                                             INCLUDE FILES
   *********************************************************************************************************
   */
#include <includes.h>
   /*
	*********************************************************************************************************
	*                                            LOCAL DEFINES
	*********************************************************************************************************
	*/


static OS_EVENT      *App_UserIFMbox;

/*
***********************************************************
 *                              Tetris Logic Variable
***********************************************************
*/
CPU_INT32U board[BW + 2][BH + 2];
CPU_INT32U nx, ny;
CPU_INT32U Shape[2][2] = { { 0, 0} ,  { 1, 0 } };
void DrawBoard();
CPU_BOOLEAN ProcessKey(CPU_INT32U dist);
void PrintBrick(CPU_BOOLEAN Show);
CPU_INT32U GetAround(CPU_INT32U x, CPU_INT32U y);
void TestFull();
CPU_BOOLEAN MoveDown();
void write_max(CPU_INT08U address, CPU_INT08U data);



/*
 *********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */



static OS_STK App_TetrisTaskStk[APP_TASK_TETRIS_STK_SIZE];
static OS_STK App_InputTaskStk[APP_TASK_INPUT_STK_SIZE];



#if ((APP_OS_PROBE_EN == DEF_ENABLED) &&    (APP_PROBE_COM_EN == DEF_ENABLED) &&    (PROBE_COM_STAT_EN == DEF_ENABLED))
static CPU_FP32 App_ProbeComRxPktSpd;
static CPU_FP32 App_ProbeComTxPktSpd;
static CPU_FP32 App_ProbeComTxSymSpd;
static CPU_FP32 App_ProbeComTxSymByteSpd;

static CPU_INT32U App_ProbeComRxPktLast;
static CPU_INT32U App_ProbeComTxPktLast;
static CPU_INT32U App_ProbeComTxSymLast;
static CPU_INT32U App_ProbeComTxSymByteLast;

static CPU_INT32U App_ProbeComCtrLast;
#endif

#if (APP_OS_PROBE_EN == DEF_ENABLED)
static CPU_INT32U App_ProbeCounts;
static CPU_BOOLEAN App_ProbeB1;

#endif


/*
 *********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************
 */

static void  App_EventCreate(void);


static void  App_TetrisTask(void        *p_arg);
static void  App_InputTask(void        *p_arg);

#if ((APP_PROBE_COM_EN == DEF_ENABLED) ||    (APP_OS_PROBE_EN == DEF_ENABLED))
static void  App_InitProbe(void);
#endif

#if (APP_OS_PROBE_EN == DEF_ENABLED)
static void  App_ProbeCallback(void);
#endif


CPU_INT32U value;




/*
 *********************************************************************************************************
 *                                        DOT MATRIX  INITIALIZATION
 *********************************************************************************************************
 */

static void MX_GPIO_Init(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void write_byte(CPU_INT08U byte)
{
	CPU_INT32U i;
	for (i = 0; i < 8; i++)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		if (byte & 0x80) {
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
		}
		else {
			GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		}

		byte = byte << 1;  // shift left
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}
}
/*
 *********************************************************************************************************
 *                                        DOT MATRIX DATA SENDING
*
*                                                 Have to Use  write_max  function
*
 *                           How  :  CPU_INT32U  send_data;
										 in send_data  you have to set every bit

							ex )  when I want to express Dot Matrix  like this

							   111111111
							   100000001
							   100000001
							   100000001
							   100000001
							   100000001
							   100000001
							   111111111

						  I have to set send_data   every bit this way

		   send_data = 0b11111111  10000001 10000001 10000001 10000001 10000001 10000001 11111111
 *********************************************************************************************************
 */
void write_max(CPU_INT08U address, CPU_INT08U data)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	write_byte(address);
	write_byte(data);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

// function for init

void max_init(void)
{
	write_max(0x09, 0x00);       //  no decoding
	write_max(0x0a, 0x03);       //  brightness intensity
	write_max(0x0b, 0x07);       //  scan limit = 8 LEDs
	write_max(0x0c, 0x01);       //  power down =0,normal mode = 1
	write_max(0x0f, 0x00);       //  no test display
}


void EnableHCSR04PeriphClock() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}


/*
 *********************************************************************************************************
 *                                                main()
 *
 * Description : This is the standard entry point for C code.  It is assumed that your code will call
 *               main() once you have performed all necessary initialization.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *********************************************************************************************************
 */

int  main(void)
{
	CPU_INT08U os_err;

	/* Disable all ints until we are ready to accept them.  */
	BSP_IntDisAll();


	/* Initialize "uC/OS-II, The Real-Time Kernel".         */
	/* IDLE Task와 Statistics Task 생성                      */

	OSInit();

	EnableHCSR04PeriphClock();


	os_err = OSTaskCreateExt((void(*)(void *))App_TetrisTask, // Task가 수행할 함수
		(void*)0,                     // Task로 넘겨줄 인자
		(OS_STK*)&App_TetrisTaskStk[APP_TASK_TETRIS_STK_SIZE - 1],     // Task가 할당될 Stack의 Top을 가리키는 주소
		(INT8U)APP_TASK_TETRIS_PRIO,// Task의 우선 순위
		(INT16U)APP_TASK_TETRIS_PRIO,// Task를 지칭하는 유일한 식별자, Task 갯수의 극복을 위해서 사용할 예정, 현재는 우선 순위와 같게끔 설정
		(OS_STK*)&App_TetrisTaskStk[0],     // Task가 할당될 Stack의 마지막을 가리키는 주소, Stack 검사용으로 사용
		(INT32U)APP_TASK_TETRIS_STK_SIZE,// Task Stack의 크기를 의미
		(void*)0,       // Task Control Block 활용시 사용
		(INT16U)(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));// Task 생성 옵션 - 초기화 시 Stack을 0으로 채울 것인지, 부동 소수점 연산 장치 사용할 것인지 등 설정



#if (OS_TASK_NAME_SIZE >= 11)
	OSTaskNameSet(APP_TASK_TETRIS_PRIO, (CPU_INT08U*)"Tetris Task", &os_err);
#endif


	os_err = OSTaskCreateExt((void(*)(void *))App_InputTask, // Task가 수행할 함수
		(void*)0,                     // Task로 넘겨줄 인자
		(OS_STK*)&App_InputTaskStk[APP_TASK_INPUT_STK_SIZE - 1],     // Task가 할당될 Stack의 Top을 가리키는 주소
		(INT8U)APP_TASK_INPUT_PRIO,// Task의 우선 순위
		(INT16U)APP_TASK_INPUT_PRIO,// Task를 지칭하는 유일한 식별자, Task 갯수의 극복을 위해서 사용할 예정, 현재는 우선 순위와 같게끔 설정
		(OS_STK*)&App_InputTaskStk[0],     // Task가 할당될 Stack의 마지막을 가리키는 주소, Stack 검사용으로 사용
		(INT32U)APP_TASK_INPUT_STK_SIZE,// Task Stack의 크기를 의미
		(void*)0,       // Task Control Block 활용시 사용
		(INT16U)(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));// Task 생성 옵션 - 초기화 시 Stack을 0으로 채울 것인지, 부동 소수점 연산 장치 사용할 것인지 등 설정


#if (OS_TASK_NAME_SIZE >= 11)
	OSTaskNameSet(APP_TASK_INPUT_PRIO, (CPU_INT08U*)"Input Task", &os_err);
#endif




	OSStart();                                              /* Start multitasking (i.e. give control to uC/OS-II).  */

	return(0);
}




/*
 *********************************************************************************************************
 *                                          App_InputTask()
 *
 * Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
 *
 * Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : This is a task.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */


static void  App_TetrisTask(void *p_arg)
{
	CPU_INT32U * msg;
	CPU_INT32S  dist;
	CPU_INT08U err;

	CPU_INT32U nFrame;
	CPU_INT32U nStay;
	CPU_INT32U x;
	CPU_INT32U y;



	nFrame = 3;  // if you want to change  tetris brick  FALLING SPEED   chage nFrmae
					  // ex) nFrame =5    slower than nFrame =3 ;


	(void)p_arg;

	BSP_Init();
	OS_CPU_SysTickInit();
	MX_GPIO_Init();
	max_init();


#if (OS_TASK_STAT_EN > 0)
	OSStatInit();                                           /* Determine CPU capacity.                              */
#endif

#if ((APP_PROBE_COM_EN == DEF_ENABLED) ||    (APP_OS_PROBE_EN == DEF_ENABLED))
	App_InitProbe();
#endif
	/* Create application events.                           */
	/* Task간 통신을 위한 MailBox 생성                        */
	App_EventCreate();

	/*
	*   BOARD INITIALIZATION
	 *   inbound  :  EMPTY   ,  outbound : WALL
	*/
	for (x = 0; x < BW + 2; x++) {
		for (y = 0; y < BH + 2; y++) {
			board[x][y] = ((y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY);
		}
	}

	/* Task body, always written as an infinite loop.       */
	while (DEF_TRUE) {
		/* start */
		while (DEF_TRUE) {
			nx = BW / 2;
			ny = 8;
			PrintBrick(TRUE);

			if (GetAround(nx, ny) != EMPTY) {
				break;
			}
			nStay = nFrame;

			for (; 2;) {

				if (--nStay == 0) {
					nStay = nFrame;
					if (MoveDown()) {
						break;
					}
				}

				msg = (CPU_INT32U*)(OSMboxPend(App_UserIFMbox, OS_TICKS_PER_SEC / 10, &err));
				if (err == OS_NO_ERR) {
					dist = (CPU_INT32U)msg;
				}

				if (ProcessKey(dist)) {
					break;
				}
			}
		}

		OSTimeDlyHMSM(0, 0, 0, 30);
	}

}

/*
   msg 테스크
*/
static void  App_InputTask(void *p_arg)
{
	CPU_INT32U  dist;
	BSP_Init();                                             /* Initialize BSP functions.                            */
	OS_CPU_SysTickInit();
	InitHCSR04(); // 초음파센서 Timer, Port, Pin 설정

	while (DEF_TRUE) {
		dist = HCSR04GetDistance(); // 거리값 받아와서 출력해보자

		OSMboxPost(App_UserIFMbox, (void*)dist);
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
}




/*
 *********************************************************************************************************
 *                                             App_EventCreate()
 *
 * Description : Create the application events.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : App_TaskStart().
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

static void  App_EventCreate(void)
{
#if (OS_EVENT_NAME_SIZE > 12)
	CPU_INT08U os_err;
#endif

	/* Create MBOX for communication between Kbd and UserIF.*/
	/* Mail Box 생성                                         */
	/* 포인터 크기의 변수를 Task나 Interrupt Service Routine   */
	/* 에서 다른 Task 전달할 때 사용함                         */
	App_UserIFMbox = OSMboxCreate((void*)0);
#if (OS_EVENT_NAME_SIZE > 12)
	OSEventNameSet(App_UserIFMbox, "User IF Mbox", &os_err);
#endif
}





/*
 *********************************************************************************************************
 *                                             App_InitProbe()
 *
 * Description : Initialize uC/Probe target code.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : App_TaskStart().
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

#if ((APP_PROBE_COM_EN == DEF_ENABLED) ||    (APP_OS_PROBE_EN == DEF_ENABLED))
static void  App_InitProbe(void)
{
#if (APP_OS_PROBE_EN == DEF_ENABLED)
	(void)App_ProbeCounts;
	(void)App_ProbeB1;


#if ((APP_PROBE_COM_EN == DEF_ENABLED) &&    (PROBE_COM_STAT_EN == DEF_ENABLED))
	(void)App_ProbeComRxPktSpd;
	(void)App_ProbeComTxPktSpd;
	(void)App_ProbeComTxSymSpd;
	(void)App_ProbeComTxSymByteSpd;
#endif

	OSProbe_Init();
	OSProbe_SetCallback(App_ProbeCallback);
	OSProbe_SetDelay(250);
#endif

#if (APP_PROBE_COM_EN == DEF_ENABLED)
	ProbeCom_Init();                                        /* Initialize the uC/Probe communications module.       */
#endif
}
#endif


/*
 *********************************************************************************************************
 *                                         AppProbeCallback()
 *
 * Description : uC/Probe OS plugin callback.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : uC/Probe OS plugin task.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

#if (APP_OS_PROBE_EN == DEF_ENABLED)
static void  App_ProbeCallback(void)
{
#if ((APP_PROBE_COM_EN == DEF_ENABLED) &&    (PROBE_COM_STAT_EN == DEF_ENABLED))
	CPU_INT32U ctr_curr;
	CPU_INT32U rxpkt_curr;
	CPU_INT32U txpkt_curr;
	CPU_INT32U sym_curr;
	CPU_INT32U symbyte_curr;
#endif



	App_ProbeCounts++;

	App_ProbeB1 = BSP_PB_GetStatus(1);




#if ((APP_PROBE_COM_EN == DEF_ENABLED) &&    (PROBE_COM_STAT_EN == DEF_ENABLED))
	ctr_curr = OSTime;
	rxpkt_curr = ProbeCom_RxPktCtr;
	txpkt_curr = ProbeCom_TxPktCtr;
	sym_curr = ProbeCom_TxSymCtr;
	symbyte_curr = ProbeCom_TxSymByteCtr;

	if ((ctr_curr - App_ProbeComCtrLast) >= OS_TICKS_PER_SEC) {
		App_ProbeComRxPktSpd = ((CPU_FP32)(rxpkt_curr - App_ProbeComRxPktLast) / (ctr_curr - App_ProbeComCtrLast)) * OS_TICKS_PER_SEC;
		App_ProbeComTxPktSpd = ((CPU_FP32)(txpkt_curr - App_ProbeComTxPktLast) / (ctr_curr - App_ProbeComCtrLast)) * OS_TICKS_PER_SEC;
		App_ProbeComTxSymSpd = ((CPU_FP32)(sym_curr - App_ProbeComTxSymLast) / (ctr_curr - App_ProbeComCtrLast)) * OS_TICKS_PER_SEC;
		App_ProbeComTxSymByteSpd = ((CPU_FP32)(symbyte_curr - App_ProbeComTxSymByteLast) / (ctr_curr - App_ProbeComCtrLast)) * OS_TICKS_PER_SEC;

		App_ProbeComCtrLast = ctr_curr;
		App_ProbeComRxPktLast = rxpkt_curr;
		App_ProbeComTxPktLast = txpkt_curr;
		App_ProbeComTxSymLast = sym_curr;
		App_ProbeComTxSymByteLast = symbyte_curr;
	}
#endif
}
#endif


/*
 *********************************************************************************************************
 *                                      App_FormatDec()
 *
 * Description : Convert a decimal value to ASCII (without leading zeros).
 *
 * Argument(s) : pstr            Pointer to the destination ASCII string.
 *
 *               value           Value to convert (assumes an unsigned value).
 *
 *               digits          The desired number of digits.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : various.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */


 /*
  *********************************************************************************************************
  *********************************************************************************************************
  *                                          uC/OS-II APP HOOKS
  *********************************************************************************************************
  *********************************************************************************************************
  */

#if (OS_APP_HOOKS_EN > 0)
  /*
   *********************************************************************************************************
   *                                      TASK CREATION HOOK (APPLICATION)
   *
   * Description : This function is cal when a task is created.
   *
   * Argument(s) : ptcb   is a pointer to the task control block of the task being created.
   *
   * Note(s)     : (1) Interrupts are disabled during this call.
   *********************************************************************************************************
   */

void  App_TaskCreateHook(OS_TCB *ptcb)
{
#if ((APP_OS_PROBE_EN == DEF_ENABLED) &&    (OS_PROBE_HOOKS_EN == DEF_ENABLED))
	OSProbe_TaskCreateHook(ptcb);
#endif
}

/*
 *********************************************************************************************************
 *                                    TASK DELETION HOOK (APPLICATION)
 *
 * Description : This function is called when a task is deleted.
 *
 * Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
 *
 * Note(s)     : (1) Interrupts are disabled during this call.
 *********************************************************************************************************
 */

void  App_TaskDelHook(OS_TCB *ptcb)
{
	(void)ptcb;
}

/*
 *********************************************************************************************************
 *                                      IDLE TASK HOOK (APPLICATION)
 *
 * Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
 *               has been added to allow you to do such things as STOP the CPU to conserve power.
 *
 * Argument(s) : none.
 *
 * Note(s)     : (1) Interrupts are enabled during this call.
 *********************************************************************************************************
 */

#if OS_VERSION >= 251
void  App_TaskIdleHook(void)
{
}
#endif

/*
 *********************************************************************************************************
 *                                        STATISTIC TASK HOOK (APPLICATION)
 *
 * Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
 *               statistics task.  This allows your application to add functionality to the statistics task.
 *
 * Argument(s) : none.
 *********************************************************************************************************
 */

void  App_TaskStatHook(void)
{
}

/*
 *********************************************************************************************************
 *                                        TASK SWITCH HOOK (APPLICATION)
 *
 * Description : This function is called when a task switch is performed.  This allows you to perform other
 *               operations during a context switch.
 *
 * Argument(s) : none.
 *
 * Note(s)     : (1) Interrupts are disabled during this call.
 *
 *               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
 *                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
 *                  task being switched out (i.e. the preempted task).
 *********************************************************************************************************
 */

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook(void)
{
#if ((APP_OS_PROBE_EN == DEF_ENABLED) &&    (OS_PROBE_HOOKS_EN == DEF_ENABLED))
	OSProbe_TaskSwHook();
#endif
}
#endif

/*
 *********************************************************************************************************
 *                                     OS_TCBInit() HOOK (APPLICATION)
 *
 * Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
 *               up most of the TCB.
 *
 * Argument(s) : ptcb    is a pointer to the TCB of the task being created.
 *
 * Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
 *********************************************************************************************************
 */

#if OS_VERSION >= 204
void  App_TCBInitHook(OS_TCB *ptcb)
{
	(void)ptcb;
}
#endif

/*
 *********************************************************************************************************
 *                                        TICK HOOK (APPLICATION)
 *
 * Description : This function is called every tick.
 *
 * Argument(s) : none.
 *
 * Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
 *********************************************************************************************************
 */

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook(void)
{
#if ((APP_OS_PROBE_EN == DEF_ENABLED) &&    (OS_PROBE_HOOKS_EN == DEF_ENABLED))
	OSProbe_TickHook();
#endif
}
#endif
#endif

/*
 *********************************************************************************************************
 *                                        Tetris  Function Implementaion
 *********************************************************************************************************
 */

void DrawBoard() {
	CPU_INT08U i;
	CPU_INT08U x;
	CPU_INT08U y;
	CPU_INT64U msg1;

	for (x = 1; x <= 8; x++) {
		for (y = 1; y < BH + 1; y++) {
			msg1 = (msg1 << 1);
			if (board[x][y] == BRICK) {  // if board [x][y]  is brick  we have send dot maxtrix 1  : TURN ON LIGHT
				msg1 = msg1 + 1;
			}
		}
	}
	for (i = 0; i < 8; ++i) {
		write_max(i + 1, (CPU_INT08U)(msg1 >> 8 * (7 - i)));
	}
	OSTimeDlyHMSM(0, 0, 0, 300);
}

CPU_BOOLEAN ProcessKey(CPU_INT32U dist) {
	if (dist < 100) { //  if dist < 100 means   CURRENT BRICK SHOULD GO LEFT SIDE  
		if (GetAround(nx - 1, ny) == EMPTY) {
			PrintBrick(FALSE);
			nx--;
			PrintBrick(DEF_TRUE);
		}
	}
	else {
		if (GetAround(nx + 1, ny) == EMPTY) {
			PrintBrick(FALSE);
			nx++;
			PrintBrick(TRUE);
		}
	}
	return DEF_FALSE;
}

void PrintBrick(CPU_BOOLEAN Show) {
	CPU_INT32U i;
	for (i = 0; i < 2; i++) {
		board[nx + Shape[i][0]][ny + Shape[i][1]] = (Show ? BRICK : EMPTY);
	}
	DrawBoard();
	for (i = 0; i < 2; i++) {
		board[nx + Shape[i][0]][ny + Shape[i][1]] = (Show ? EMPTY : EMPTY);
	}
}

CPU_INT32U GetAround(CPU_INT32U x, CPU_INT32U y) {
	CPU_INT32U i;
	CPU_INT32U k;
	k = EMPTY;
	for (i = 0; i < 2; i++) {
		if (k <= board[x + Shape[i][0]][y + Shape[i][1]]) {
			k = board[x + Shape[i][0]][y + Shape[i][1]];
		}
	}
	return k;
}
void TestFull() {
	CPU_INT32U i;
	CPU_INT32U x;
	CPU_INT32U y;
	CPU_INT32U ty;
	for (i = 0; i < 2; i++) {
		board[nx + Shape[i][0]][ny + Shape[i][1]] = BRICK;
	}
	for (y = BH; y > 0; y--) {
		for (x = 1; x < BW + 1; x++) {
			if (board[x][y] != BRICK) break;
		}
		if (x == BW + 1) {
			for (ty = y; ty < BH; ty++) {
				for (x = 1; x < BW + 1; x++) {
					board[x][ty] = board[x][ty + 1];
				}
			}
		}
	}
}
CPU_BOOLEAN MoveDown() {
	if (GetAround(nx, ny - 1) != EMPTY) {
		TestFull();
		return DEF_TRUE;
	}
	PrintBrick(DEF_FALSE);
	ny--;
	PrintBrick(DEF_TRUE);
	return DEF_FALSE;
}


