//****  0. Documentation Section
-------------------------------------------------------------
                                                            //   This program calculates the area of a square shaped room
                                                            //   Author: Ben Fridolin
                                                            //   Date: 20/8/2016
                                                            //

                                                            // 1.  Pre-processor Directive Section
                                                            --------------------------------------------------------------
#include <stdio.h> // Diamond braces for sys library: Standard I/O
#include "uart.h"  // Quotes for user library : UART lib

                                                                                                                        // 2.  Global Declaration Section
                                                                                                                        ---------------------------------------------------------------
                                                                                                                                                                                      //   global variable
                                                                                                                                                                                      unsigned long error;

//   Function prototype
//   Compiler aid for "type checking"
void Initialize(void);
unsigned long Calc_Area(unsigned long s); // Says Calc_Area expects
                                          // an unsigned long and returns an unsigned long

// 3.  Subroutine Section
---------------------------------------------------------------

                                                              int
                                                              main(void)
{
   unsigned long side; // room wall metres
   unsigned long area; // size squared metres

   UART_Init(); // call subroutine to init uart lib
   Initialize();

   printf("This program calculates the area of a square shaped room\n");

   while (side != 0)
   {
      printf("Give room side (zero to quit) : ");
      scanf("%ld", &side);
      area = Calc_Area(side);
      if (area != 0)
      {
         printf("Area of the room with side of %ld m is %ld sqr m\n", side, area);
      }
      else
      {
         printf("\n Size cannot exceed 25 metres\n");
      }
   }
   printf("Goodbye (Mistake count = %ld\n", error);
}

// Initialize global
void Initialize(void)
{
   error = 0;
}

// Calculates area

unsigned long Calc_Area(unsigned long s)
{
   unsigned long result;

   if (s <= 25)
   {
      result = s * s;
   }
   else
   {
      result = 0;
      error = error + 1;
   }
   return (result);
}