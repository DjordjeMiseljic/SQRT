/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */


#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "xdebug.h"
#include "xbasic_types.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xscugic.h"
#include <unistd.h>

#define SQRT0_BASE_ADDR XPAR_MYSQRTIP_0_S00_AXI_BASEADDR
#define SQRT1_BASE_ADDR XPAR_MYSQRTIP_1_S00_AXI_BASEADDR
#define SQRT2_BASE_ADDR XPAR_MYSQRTIP_2_S00_AXI_BASEADDR
#define SQRT3_BASE_ADDR XPAR_MYSQRTIP_3_S00_AXI_BASEADDR
#define SQRT0_INTR_ID XPAR_FABRIC_MYSQRTIP_0_DONE_INTERRUPT_INTR
#define SQRT1_INTR_ID XPAR_FABRIC_MYSQRTIP_1_DONE_INTERRUPT_INTR
#define SQRT2_INTR_ID XPAR_FABRIC_MYSQRTIP_2_DONE_INTERRUPT_INTR
#define SQRT3_INTR_ID XPAR_FABRIC_MYSQRTIP_3_DONE_INTERRUPT_INTR
#define INTC_DEVICE_ID 	XPAR_PS7_SCUGIC_0_DEVICE_ID

#define X_OFFSET 0x00000000
#define START_OFFSET 0x00000004
#define Y_OFFSET 0x00000008
#define READY_OFFSET 0x0000000C

#define NUM0 100
#define NUM1 1000
#define NUM2 10000
#define NUM3 32767


void Sqrt0_interrupt_handler(void *intc_inst_ptr);
void Sqrt1_interrupt_handler(void *intc_inst_ptr);
void Sqrt2_interrupt_handler(void *intc_inst_ptr);
void Sqrt3_interrupt_handler(void *intc_inst_ptr);
u32 Init_Function(u32 DeviceId);
static void DisableIntrSystem();

volatile int sqrt0_intr_done = 0;
volatile int sqrt1_intr_done = 0;
volatile int sqrt2_intr_done = 0;
volatile int sqrt3_intr_done = 0;
volatile int Error;
static XScuGic INTCInst;

u32 retval = 0;

int main()
{
    init_platform();
    int status = Init_Function(INTC_DEVICE_ID);

    print("\n\r*****Starting orbitrary test*****\n\r");
    print("Writing perfect sqares to sqrt modules\n\r");

    printf("CORE0: Input: %d Expected output %d\n\r",NUM0*NUM0,NUM0);
    Xil_Out32(SQRT0_BASE_ADDR+X_OFFSET, (u32)(NUM0*NUM0));
    printf("CORE1: Input: %d Expected output %d\n\r",NUM1*NUM1,NUM1);
    Xil_Out32(SQRT1_BASE_ADDR+X_OFFSET, (u32)(NUM1*NUM1));
    printf("CORE2: Input: %d Expected output %d\n\r",NUM2*NUM2,NUM2);
    Xil_Out32(SQRT2_BASE_ADDR+X_OFFSET, (u32)(NUM2*NUM2));
    printf("CORE3: Input: %d Expected output %d\n\r",NUM3*NUM3,NUM3);
    Xil_Out32(SQRT3_BASE_ADDR+X_OFFSET, (u32)(NUM3*NUM3));

    printf("CORE0: Sending start pulse to all cores\n\r");


    //START CORE 0
    Xil_Out32(SQRT0_BASE_ADDR+START_OFFSET, (u32)(1));
    Xil_Out32(SQRT0_BASE_ADDR+START_OFFSET, (u32)(0));

    //START CORE 1
    Xil_Out32(SQRT1_BASE_ADDR+START_OFFSET, (u32)(1));
    Xil_Out32(SQRT1_BASE_ADDR+START_OFFSET, (u32)(0));

    //START CORE 2
    Xil_Out32(SQRT2_BASE_ADDR+START_OFFSET, (u32)(1));
    Xil_Out32(SQRT2_BASE_ADDR+START_OFFSET, (u32)(0));

    //START CORE 3
    Xil_Out32(SQRT3_BASE_ADDR+START_OFFSET, (u32)(1));
    Xil_Out32(SQRT3_BASE_ADDR+START_OFFSET, (u32)(0));


    //RESET START SIGNAL ON ALL CORES





	printf("INTR_STAT:  %d,%d,%d,%d\n\r",sqrt0_intr_done,sqrt1_intr_done,sqrt2_intr_done,sqrt3_intr_done);


    while(sqrt0_intr_done==0 || sqrt1_intr_done==0 || sqrt2_intr_done==0 || sqrt3_intr_done==0)
    {
    	for(int d=0; d<10000; d++);
    	printf("INTR_STAT:  %d,%d,%d,%d\n\r",sqrt0_intr_done,sqrt1_intr_done,sqrt2_intr_done,sqrt3_intr_done);
    }
    print("***ALL CORES FINISHED***\n\r");

    printf("INTR_STAT: %d,%d,%d,%d\n\r",sqrt0_intr_done,sqrt1_intr_done,sqrt2_intr_done,sqrt3_intr_done);

    retval = Xil_In32(SQRT0_BASE_ADDR+Y_OFFSET);
    printf("CORE0: Result: %lu Expected output %d\n\r",(u32)retval,NUM0);
    retval = Xil_In32(SQRT1_BASE_ADDR+Y_OFFSET);
    printf("CORE1: Result: %lu Expected output %d\n\r",(u32)retval,NUM1);
    retval = Xil_In32(SQRT2_BASE_ADDR+Y_OFFSET);
    printf("CORE2: Result: %lu Expected output %d\n\r",(u32)retval,NUM2);
    retval = Xil_In32(SQRT3_BASE_ADDR+Y_OFFSET);
    printf("CORE3: Result: %lu Expected output %d\n\r",(u32)retval,NUM3);


    //printf("INTR_STAT: %d,%d,%d,%d\n\r",sqrt0_intr_done,sqrt1_intr_done,sqrt2_intr_done,sqrt3_intr_done);
    sleep(1);
    //printf("INTR_STAT: %d,%d,%d,%d\n\r",sqrt0_intr_done,sqrt1_intr_done,sqrt2_intr_done,sqrt3_intr_done);

    print("Successfully ran Hello World application");
    DisableIntrSystem();
    cleanup_platform();
    return 0;
}


u32 Init_Function(u32 DeviceId)
    {
    	XScuGic_Config *IntcConfig;
    	int status;
    	IntcConfig = XScuGic_LookupConfig(DeviceId);
    	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
    	if(status != XST_SUCCESS) return XST_FAILURE;


    	//Sqet interrupt enable and connect
    	XScuGic_SetPriorityTriggerType(&INTCInst, SQRT0_INTR_ID,0xA8, 3);
    	XScuGic_SetPriorityTriggerType(&INTCInst, SQRT1_INTR_ID,0xB8, 3);
    	XScuGic_SetPriorityTriggerType(&INTCInst, SQRT2_INTR_ID,0xC8, 3);
    	XScuGic_SetPriorityTriggerType(&INTCInst, SQRT3_INTR_ID,0xD8, 3);


    	status = XScuGic_Connect(&INTCInst, SQRT0_INTR_ID, (Xil_ExceptionHandler)Sqrt0_interrupt_handler, (void *)&INTCInst);
    	if(status != XST_SUCCESS) return XST_FAILURE;

    	status = XScuGic_Connect(&INTCInst, SQRT1_INTR_ID, (Xil_ExceptionHandler)Sqrt1_interrupt_handler, (void *)&INTCInst);
    	if(status != XST_SUCCESS) return XST_FAILURE;

    	status = XScuGic_Connect(&INTCInst, SQRT2_INTR_ID, (Xil_ExceptionHandler)Sqrt2_interrupt_handler, (void *)&INTCInst);
    	if(status != XST_SUCCESS) return XST_FAILURE;

    	status = XScuGic_Connect(&INTCInst, SQRT3_INTR_ID, (Xil_ExceptionHandler)Sqrt3_interrupt_handler, (void *)&INTCInst);
    	if(status != XST_SUCCESS) return XST_FAILURE;

    	XScuGic_Enable(&INTCInst, SQRT0_INTR_ID);
    	XScuGic_Enable(&INTCInst, SQRT1_INTR_ID);
    	XScuGic_Enable(&INTCInst, SQRT2_INTR_ID);
    	XScuGic_Enable(&INTCInst, SQRT3_INTR_ID);


    	Xil_ExceptionInit();
    	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
    						 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
    						 &INTCInst);
    	Xil_ExceptionEnable();
    	////////////////////////////////////////////////////
    	return XST_SUCCESS;

    }

// INTERRUPT HANDLERS
void Sqrt0_interrupt_handler(void *intc_inst_ptr)
{
	sqrt0_intr_done ++;
	//print("SQRT0 DONE!\n\r");
}
void Sqrt1_interrupt_handler(void *intc_inst_ptr)
{
	sqrt1_intr_done ++;
	//print("SQRT1 DONE!\n\r");
}
void Sqrt2_interrupt_handler(void *intc_inst_ptr)
{
	sqrt2_intr_done ++;
	//print("SQRT2 DONE!\n\r");
}
void Sqrt3_interrupt_handler(void *intc_inst_ptr)
{
	sqrt3_intr_done ++;
	//print("SQRT3 DONE!\n\r");
}

static void DisableIntrSystem()
{


	XScuGic_Disconnect(&INTCInst, SQRT0_INTR_ID);
	XScuGic_Disconnect(&INTCInst, SQRT1_INTR_ID);
	XScuGic_Disconnect(&INTCInst, SQRT2_INTR_ID);
	XScuGic_Disconnect(&INTCInst, SQRT3_INTR_ID);


}
