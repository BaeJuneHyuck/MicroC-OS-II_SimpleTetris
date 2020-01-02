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
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ESC 27
#define BX 5
#define BY 1
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
CPU_INT32U board[BW + 2][BH + 2];
CPU_INT32U nx, ny;
CPU_INT32U Shape[2][2]= {  { 0, 0} ,  { 1, 0 }  };

static OS_EVENT      *App_UserIFMbox;
static OS_EVENT      *App_UserIFMbox2;

/* start */
void DrawBoard();
CPU_BOOLEAN ProcessKey(CPU_INT32U dist);
void PrintBrick(CPU_BOOLEAN Show);
CPU_INT32U GetAround(CPU_INT32U x, CPU_INT32U y);
void TestFull();
CPU_BOOLEAN MoveDown();
void write_max (CPU_INT08U address, CPU_INT08U data);


void DrawBoard(){
  CPU_INT08U i;
   CPU_INT08U x;
   CPU_INT08U y;
   CPU_INT64U msg1;
   
   for (x = 1; x <=8 ; x++) {        
       for (y = 1; y < BH + 1; y++) {
              msg1 = (msg1<<1);
              if(board[x][y] == BRICK){  // 여기에 BRIcK이 차있으면 출력해야지
                   msg1 = msg1 + 1;
              }
            }            
    }
    //OSMboxPost(App_UserIFMbox, (void*)msg1);
    for(i = 0; i< 8 ; ++i){
            
                 write_max(i+1 , (CPU_INT08U)( msg1 >> 8 * ( 7  -  i)  ));     
    }
    OSTimeDlyHMSM(0, 0, 0, 300);
}

CPU_BOOLEAN ProcessKey(CPU_INT32U dist){     
     if (dist < 100) { // 왼쪽이다 ?
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

void PrintBrick(CPU_BOOLEAN Show){
   CPU_INT32U i;
   for (i=0;i<2;i++) {
     board[nx + Shape[i][0]][ ny + Shape[i][1] ] = ( Show ?  BRICK : EMPTY); 
   }
   DrawBoard();   
   for (i=0;i<2;i++) {
     board[nx + Shape[i][0]][ ny + Shape[i][1] ] = ( Show ?  EMPTY : EMPTY); 
   }
}

CPU_INT32U GetAround(CPU_INT32U x, CPU_INT32U y){
     CPU_INT32U i;
     CPU_INT32U k ;
     k = EMPTY;
     for (i=0;i<2;i++) {
       if(k <= board[x+Shape[i][0]][y+Shape[i][1]]){
          k = board[x+Shape[i][0]][y+Shape[i][1]];
       }
     }
     return k;
}
void TestFull(){
   CPU_INT32U i;
   CPU_INT32U x;
   CPU_INT32U y;
   CPU_INT32U ty;
     for (i=0;i<2;i++) {
          board[nx+Shape[i][0]][ny+Shape[i][1]]=BRICK;
     }
     for (y=BH;y > 0 ; y--) {
          for (x=1;x<BW+1;x++) {
              if (board[x][y] != BRICK) break;
          }
          if (x == BW+1) {
              for (ty=y;ty<BH;ty++) {
                   for (x=1;x<BW+1;x++) {
                        board[x][ty]=board[x][ty+1];
                   }
              }              
              //delay(200);
          }
     }
}
CPU_BOOLEAN MoveDown(){
   if (GetAround(nx,ny-1) != EMPTY) {
          TestFull();
          return DEF_TRUE;
     }
     PrintBrick(DEF_FALSE);
     ny--;
     PrintBrick(DEF_TRUE);
     return DEF_FALSE;
}




CPU_INT08U disp1ay[38][8]={
{0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C},//0
{0x10,0x30,0x50,0x10,0x10,0x10,0x10,0x7c},//1
{0x7E,0x2,0x2,0x7E,0x40,0x40,0x40,0x7E},//2
{0x3E,0x2,0x2,0x3E,0x2,0x2,0x3E,0x0},//3
{0x8,0x18,0x28,0x48,0xFE,0x8,0x8,0x8},//4
{0x3C,0x20,0x20,0x3C,0x4,0x4,0x3C,0x0},//5
{0x3C,0x20,0x20,0x3C,0x24,0x24,0x3C,0x0},//6
{0x3E,0x22,0x4,0x8,0x8,0x8,0x8,0x8},//7
{0x0,0x3E,0x22,0x22,0x3E,0x22,0x22,0x3E},//8
{0x3E,0x22,0x22,0x3E,0x2,0x2,0x2,0x3E},//9
{0x18,0x24,0x42,0x42,0x7E,0x42,0x42,0x42},//A
{0x3C,0x22,0x22,0x3c,0x22,0x22,0x3C,0x0},//B
{0x3C,0x40,0x40,0x40,0x40,0x40,0x40,0x3C},//C
{0x7C,0x22,0x22,0x22,0x22,0x22,0x22,0x7C},//D
{0x7C,0x40,0x40,0x7C,0x40,0x40,0x40,0x7C},//E
{0x7C,0x40,0x40,0x7C,0x40,0x40,0x40,0x40},//F
{0x3C,0x40,0x40,0x40,0x4c,0x44,0x44,0x3C},//G
{0x44,0x44,0x44,0x7C,0x44,0x44,0x44,0x44},//H
{0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x7C},//I
{0x3C,0x8,0x8,0x8,0x8,0x8,0x48,0x30},//J
{0x0,0x24,0x28,0x30,0x20,0x30,0x28,0x24},//K
{0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x7C},//L
{0x81,0xC3,0xA5,0x99,0x81,0x81,0x81,0x81},//M
{0x0,0x42,0x62,0x52,0x4A,0x46,0x42,0x0},//N
{0x3C,0x42,0x42,0x42,0x42,0x42,0x42,0x3C},//O
{0x3C,0x22,0x22,0x22,0x3C,0x20,0x20,0x20},//P
{0x1C,0x22,0x22,0x22,0x22,0x26,0x22,0x1D},//Q
{0x3C,0x22,0x22,0x22,0x3C,0x24,0x22,0x21},//R
{0x0,0x1E,0x20,0x20,0x3E,0x2,0x2,0x3C},//S
{0x0,0x3E,0x8,0x8,0x8,0x8,0x8,0x8},//T
{0x42,0x42,0x42,0x42,0x42,0x42,0x22,0x1C},//U
{0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18},//V
{0x0,0x49,0x49,0x49,0x49,0x2A,0x1C,0x0},//W
{0x0,0x41,0x22,0x14,0x8,0x14,0x22,0x41},//X
{0x41,0x22,0x14,0x8,0x8,0x8,0x8,0x8},//Y
{0x0,0x7F,0x2,0x4,0x8,0x10,0x20,0x7F},//Z
};

/* end */


/*
 *********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */



static OS_STK App_TetrisTaskStk[APP_TASK_TETRIS_STK_SIZE];
static OS_STK App_InputTaskStk[APP_TASK_INPUT_STK_SIZE];
static OS_STK App_PrintTaskStk[APP_TASK_PRINT_STK_SIZE];
// static OS_STK App_TaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];



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

// static void  App_TaskCreate(void);
static void  App_EventCreate(void);


static void  App_TetrisTask(void        *p_arg);
static void  App_InputTask(void        *p_arg);
static void  App_PrintTask(void        *p_arg);
// static void  App_TaskUserIF(void        *p_arg);
// static void  App_TaskKbd(void        *p_arg);


// static void  App_DispScr_SignOn(void);
// static void  App_DispScr_TaskNames(void);

#if ((APP_PROBE_COM_EN == DEF_ENABLED) ||    (APP_OS_PROBE_EN == DEF_ENABLED))
static void  App_InitProbe(void);
#endif

#if (APP_OS_PROBE_EN == DEF_ENABLED)
static void  App_ProbeCallback(void);
#endif


CPU_INT32U value;


void ADC1_2_IRQHandler(void) {
   if(ADC_GetITStatus(ADC1,  ADC_IT_EOC) != RESET) {
      value = ADC_GetConversionValue(ADC1);
      if(value %4 == 0)BSP_LED_On(0); // LED ON
      if(value %4 == 1)BSP_LED_On(3); // LED ON
      if(value %4 == 2)BSP_LED_On(4); // LED ON
      if(value %4 == 3)BSP_LED_On(4); // LED ON
      ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
      
   }
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */

static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  // __HAL_RCC_GPIOD_CLK_ENABLE();
  // __HAL_RCC_GPIOA_CLK_ENABLE();
  
  //HAL_GPIO_WritePin(GPIOA, clock_Pin|cs_Pin|data_Pin, GPIO_PIN_RESET);
   GPIO_ResetBits(GPIOA, GPIO_Pin_0);
   GPIO_ResetBits(GPIOA, GPIO_Pin_1);
   GPIO_ResetBits(GPIOA, GPIO_Pin_2);

   
  GPIO_InitTypeDef GPIO_InitStruct;
  
/*Configure GPIO pins : clock_Pin cs_Pin data_Pin */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void write_byte (CPU_INT08U byte)
{
        CPU_INT32U i ;
       for (i =0; i<8; i++)
       {
          //HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, 0);  // pull the clock pin low
          GPIO_ResetBits(GPIOA, GPIO_Pin_0);
          
          
          // HAL_GPIO_WritePin (GPIOA, GPIO_PIN_7, byte&0x80);  // write the MSB bit to the data pin
          if(byte&0x80){
            GPIO_SetBits(GPIOA, GPIO_Pin_2);
          } else{
            GPIO_ResetBits(GPIOA, GPIO_Pin_2);
          }
            
          byte = byte<<1;  // shift left
          
          //HAL_GPIO_WritePin (GPIOA, GPIO_PIN_5, 1);  // pull the clock pin HIGH
          GPIO_SetBits(GPIOA, GPIO_Pin_0);
       }
}

void write_max (CPU_INT08U address, CPU_INT08U data)
{
   // HAL_GPIO_WritePin (GPIOA, GPIO_PIN_6, 0);  // pull the CS pin LOW
   GPIO_ResetBits(GPIOA, GPIO_Pin_1);
   write_byte (address);
   write_byte (data); 
   
   // HAL_GPIO_WritePin (GPIOA, GPIO_PIN_6, 1);  // pull the CS pin HIGH
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
        
        
   os_err = OSTaskCreateExt((void (*)(void *))App_TetrisTask, // Task가 수행할 함수
             (void* )0,                     // Task로 넘겨줄 인자
             (OS_STK* )&App_TetrisTaskStk[APP_TASK_TETRIS_STK_SIZE - 1],     // Task가 할당될 Stack의 Top을 가리키는 주소
             (INT8U           )APP_TASK_TETRIS_PRIO,// Task의 우선 순위
             (INT16U          )APP_TASK_TETRIS_PRIO,// Task를 지칭하는 유일한 식별자, Task 갯수의 극복을 위해서 사용할 예정, 현재는 우선 순위와 같게끔 설정
             (OS_STK* )&App_TetrisTaskStk[0],     // Task가 할당될 Stack의 마지막을 가리키는 주소, Stack 검사용으로 사용
             (INT32U          )APP_TASK_TETRIS_STK_SIZE,// Task Stack의 크기를 의미
             (void* )0,       // Task Control Block 활용시 사용
             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));// Task 생성 옵션 - 초기화 시 Stack을 0으로 채울 것인지, 부동 소수점 연산 장치 사용할 것인지 등 설정

        
        
#if (OS_TASK_NAME_SIZE >= 11)
   OSTaskNameSet(APP_TASK_TETRIS_PRIO, (CPU_INT08U*)"Tetris Task", &os_err);
#endif
        
   
   os_err = OSTaskCreateExt((void (*)(void *))App_InputTask, // Task가 수행할 함수
             (void* )0,                     // Task로 넘겨줄 인자
             (OS_STK* )&App_InputTaskStk[APP_TASK_INPUT_STK_SIZE - 1],     // Task가 할당될 Stack의 Top을 가리키는 주소
             (INT8U           )APP_TASK_INPUT_PRIO,// Task의 우선 순위
             (INT16U          )APP_TASK_INPUT_PRIO,// Task를 지칭하는 유일한 식별자, Task 갯수의 극복을 위해서 사용할 예정, 현재는 우선 순위와 같게끔 설정
             (OS_STK* )&App_InputTaskStk[0],     // Task가 할당될 Stack의 마지막을 가리키는 주소, Stack 검사용으로 사용
             (INT32U          )APP_TASK_INPUT_STK_SIZE,// Task Stack의 크기를 의미
             (void* )0,       // Task Control Block 활용시 사용
             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));// Task 생성 옵션 - 초기화 시 Stack을 0으로 채울 것인지, 부동 소수점 연산 장치 사용할 것인지 등 설정

        
#if (OS_TASK_NAME_SIZE >= 11)
   OSTaskNameSet(APP_TASK_INPUT_PRIO, (CPU_INT08U*)"Input Task", &os_err);
#endif
  
   
   
   /*
   os_err = OSTaskCreateExt((void (*)(void *))App_PrintTask, // Task가 수행할 함수
             (void* )0,                     // Task로 넘겨줄 인자
             (OS_STK* )&App_PrintTaskStk[APP_TASK_PRINT_STK_SIZE - 1],     // Task가 할당될 Stack의 Top을 가리키는 주소
             (INT8U           )APP_TASK_PRINT_PRIO,// Task의 우선 순위
             (INT16U          )APP_TASK_PRINT_PRIO,// Task를 지칭하는 유일한 식별자, Task 갯수의 극복을 위해서 사용할 예정, 현재는 우선 순위와 같게끔 설정
             (OS_STK* )&App_PrintTaskStk[0],     // Task가 할당될 Stack의 마지막을 가리키는 주소, Stack 검사용으로 사용
             (INT32U          )APP_TASK_PRINT_STK_SIZE,// Task Stack의 크기를 의미
             (void* )0,       // Task Control Block 활용시 사용
             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));// Task 생성 옵션 - 초기화 시 Stack을 0으로 채울 것인지, 부동 소수점 연산 장치 사용할 것인지 등 설정

        
        
#if (OS_TASK_NAME_SIZE >= 11)
   OSTaskNameSet(APP_TASK_PRINT_PRIO, (CPU_INT08U*)"Print Task", &os_err);
#endif
        
   */
        

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
        CPU_INT32U i;
        CPU_INT32U dly;
        CPU_INT32U * msg;
        CPU_INT32S  dist;
        CPU_INT32U prev_dist;
        CPU_INT08U err;
        CPU_INT32U diff;
  /* start */
        CPU_INT32U nFrame;
        CPU_INT32U nStay;
         CPU_INT32U x;
         CPU_INT32U y;
         
         
    
        nFrame = 3; 
        

   (void)p_arg;

   BSP_Init();                                             /* Initialize BSP functions.                            */
   OS_CPU_SysTickInit();                                   /* Initialize the SysTick.                              */
        MX_GPIO_Init();
        max_init();
        InitHCSR04(); // 초음파센서 Timer, Port, Pin 설정

#if (OS_TASK_STAT_EN > 0)
   OSStatInit();                                           /* Determine CPU capacity.                              */
#endif

#if ((APP_PROBE_COM_EN == DEF_ENABLED) ||    (APP_OS_PROBE_EN == DEF_ENABLED))
   App_InitProbe();
#endif
   /* Create application events.                           */
   /* Task간 통신을 위한 MailBox 생성                        */
             App_EventCreate();

             /* Create application tasks.                            */
             /* LCD 갱신 Task, 키보드 입력 Task 생성                    */
             // App_TaskCreate();
              for (x = 0; x < BW + 2; x++) {
                for (y = 0; y < BH + 2; y++) {
                   board[x][y] = ( (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY );                        
                }
             }   
        
   /* Task body, always written as an infinite loop.       */
   while (DEF_TRUE) {
     /* start */
              
            
                /* 테트리스 코드 시작  */
                while(DEF_TRUE){
                    nx = BW / 2;
                    ny = 8;
                    PrintBrick(TRUE);                    
                    
                    if (GetAround(nx, ny) != EMPTY) {                      
                      for(i=1;i<9;i++){ //  char * str   data type입니당
                          write_max (i,  disp1ay  [1]  [i-1]  );
                      }
                      break;
                    }
                    nStay = nFrame;
                    
                    for (; 2;) {
                            
                            if (--nStay == 0) {
                                    nStay = nFrame;
                                    if (MoveDown()) {
                                      for(i=1;i<9;i++){ //  char * str   data type입니당
                                          write_max (i,  disp1ay  [3]  [i-1]  );
                                      }
                                      break;
                                    }
                            }  
                            
                            msg = (CPU_INT32U*)(OSMboxPend(App_UserIFMbox, OS_TICKS_PER_SEC / 10, &err));
                            if (err == OS_NO_ERR) {
                                      dist =  (	CPU_INT32U  ) msg;
                            }
                            
                            //dist = HCSR04GetDistance(); // 거리값 받아와서 출력해보자
                            if (ProcessKey(dist)) {  
                              break;                              
                            }
                    }   
                }
                
                OSTimeDlyHMSM(0, 0, 0, 30);
                prev_dist = dist;
        }
        
}

/*
   msg 테스크
*/
static void  App_InputTask(void *p_arg)
{
        CPU_INT32U i;
        CPU_INT32U j;
        CPU_INT32U dly;
        CPU_INT32U  dist;
        CPU_INT32U prev_dist;
        CPU_INT32U diff;
        BSP_Init();                                             /* Initialize BSP functions.                            */
        OS_CPU_SysTickInit();
        InitHCSR04(); // 초음파센서 Timer, Port, Pin 설정
        
        
        
        
        
        
        while(DEF_TRUE){
              dist = HCSR04GetDistance(); // 거리값 받아와서 출력해보자
              
              
              OSMboxPost(App_UserIFMbox, (void*)dist);
              OSTimeDlyHMSM(0, 0, 0, 100);
        }
}


/*
   print 테스크
*/
static void  App_PrintTask(void *p_arg)
{ 
        CPU_INT64U  *msg;
        CPU_INT64U  key ;
        CPU_INT08U err;
        CPU_INT64U i;
        CPU_INT32U j;
        BSP_Init();
        MX_GPIO_Init();
        max_init();
        OS_CPU_SysTickInit();
        
        
        while(DEF_TRUE){
            BSP_LED_Off(4);
            BSP_LED_On(3); 
         msg = (CPU_INT64U*)(OSMboxPend(App_UserIFMbox, OS_TICKS_PER_SEC / 10, &err));
          if (err == OS_NO_ERR) {
		        key =  (	CPU_INT64U  ) msg;
		}
        }
}
          /*
          for(i = 0; i< 8 ; ++i){
            
                 write_max(i+1 , (CPU_INT08U)( key >> 8 * ( 7  -  i)  ));     
           }
          
                      OSTimeDlyHMSM(0, 0, 0, 600);
          for(i=1;i<9;i++){ //  char * str   data type입니당
                          write_max (i,  disp1ay  [4]  [i-1]  );
                      }
                      OSTimeDlyHMSM(0, 0, 0, 600);/*
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
////////////////
static void  App_EventCreate2(void)
{
#if (OS_EVENT_NAME_SIZE > 12)
   CPU_INT08U os_err;
#endif

   /* Create MBOX for communication between Kbd and UserIF.*/
   /* Mail Box 생성                                         */
   /* 포인터 크기의 변수를 Task나 Interrupt Service Routine   */
   /* 에서 다른 Task 전달할 때 사용함                         */
   App_UserIFMbox2 = OSMboxCreate((void*)0);
#if (OS_EVENT_NAME_SIZE > 12)
   OSEventNameSet(App_UserIFMbox2, "User IF Mbox", &os_err);
#endif
}


/*
 *********************************************************************************************************
 *                                            App_TaskCreate()
 *
 * Description : Create the application tasks.
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
/*
static void  App_TaskCreate(void)
{
   CPU_INT08U os_err;

   // LCD를 갱신시키는 Task 생성
   os_err = OSTaskCreateExt((void (*)(void *))App_TaskUserIF,
             (void* )0,
             (OS_STK* )&App_TaskUserIFStk[APP_TASK_USER_IF_STK_SIZE - 1],
             (INT8U           )APP_TASK_USER_IF_PRIO,
             (INT16U          )APP_TASK_USER_IF_PRIO,
             (OS_STK* )&App_TaskUserIFStk[0],
             (INT32U          )APP_TASK_USER_IF_STK_SIZE,
             (void* )0,
             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 9)
   OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &os_err);
#endif
   // Keyboard 입력을 받는 Task 생성
   os_err = OSTaskCreateExt((void (*)(void *))App_TaskKbd,
             (void* )0,
             (OS_STK* )&App_TaskKbdStk[APP_TASK_KBD_STK_SIZE - 1],
             (INT8U           )APP_TASK_KBD_PRIO,
             (INT16U          )APP_TASK_KBD_PRIO,
             (OS_STK* )&App_TaskKbdStk[0],
             (INT32U          )APP_TASK_KBD_STK_SIZE,
             (void* )0,
             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#if (OS_TASK_NAME_SIZE >= 9)
   OSTaskNameSet(APP_TASK_KBD_PRIO, "Keyboard", &os_err);
#endif
        
}

*/


/*
 *********************************************************************************************************
 *                                            App_TaskKbd()
 *
 * Description : Monitor the state of the push buttons and passes messages to AppTaskUserIF()
 *
 * Argument(s) : p_arg       Argument passed to 'App_TaskKbd()' by 'OSTaskCreate()'.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : This is a task.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */
/*
// Keyboard 입력을 받는 Task
static void  App_TaskKbd(void *p_arg)
{
   CPU_BOOLEAN b1_prev;
   CPU_BOOLEAN b1;
   CPU_INT08U key;


   (void)p_arg;

   b1_prev = DEF_FALSE;
   key = 1;

   while (DEF_TRUE) {
      b1 = BSP_PB_GetStatus(BSP_PB_ID_KEY);

      if ((b1 == DEF_TRUE) && (b1_prev == DEF_FALSE)) {
         if (key == 2) {
            key = 1;
         } else {
            key++;
         }
         // MailBox에 Task에서 입력받은 값 Key를 전달
         OSMboxPost(App_UserIFMbox, (void*)key);
      }

      b1_prev = b1;

      OSTimeDlyHMSM(0, 0, 0, 20);
   }
}
*/

/*
 *********************************************************************************************************
 *                                            App_TaskUserIF()
 *
 * Description : Updates LCD.
 *
 * Argument(s) : p_arg       Argument passed to 'App_TaskUserIF()' by 'OSTaskCreate()'.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : This is a task.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */
/*
// LCD 갱신을 위한 Task
static void  App_TaskUserIF(void *p_arg)
{
   CPU_INT08U  *msg;
   CPU_INT08U err;
   CPU_INT32U nstate;
   CPU_INT32U pstate;


   (void)p_arg;


   App_DispScr_SignOn();
   OSTimeDlyHMSM(0, 0, 1, 0);
   nstate = 1;
   pstate = 1;


   while (DEF_TRUE) {
      // 다른 Task에서 Mailbox에 전달한 저장된 값을 받음
      msg = (CPU_INT08U*)(OSMboxPend(App_UserIFMbox, OS_TICKS_PER_SEC / 10, &err));
      if (err == OS_NO_ERR) {
         nstate = (CPU_INT32U)msg;
      }

      if (nstate != pstate) {
         pstate = nstate;
      }

      switch (nstate) {
      case 2:
         App_DispScr_TaskNames();
         break;

      case 1:
      default:
         App_DispScr_SignOn();
         break;
      }
   }
}
*/

/*
 *********************************************************************************************************
 *                                          App_DispScr_SignOn()
 *
 * Description : Display uC/OS-II system information on the LCD.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : App_TaskUserIF().
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

static void  App_DispScr_SignOn(void)
{
}



/*
 *********************************************************************************************************
 *                                          App_DispScr_SignOn()
 *
 * Description : Display uC/OS-II system information on the LCD.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : App_TaskUserIF().
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

static void  App_DispScr_TaskNames(void)
{
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





